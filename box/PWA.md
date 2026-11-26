# Développement d'une application PWA multi-plateforme

## Objectif

Faire évoluer le projet actuel vers une application installable et utilisable hors ligne, disponible sous deux formes :

1. **PWA Web standard** : application statique hébergeable sur GitHub Pages ou tout serveur HTTP statique.
2. **Application Android (APK)** : version enrichie donnant accès aux fonctionnalités natives d'Android.

Les deux versions doivent partager autant que possible le même socle applicatif, le même code métier et la même interface.

## 1. Version PWA Web standard

La version Web doit être entièrement autonome et ne nécessiter aucun backend pour son fonctionnement courant.

### Hébergement
- Génération d'un site statique.
- Compatibilité avec GitHub Pages.
- Aucune dépendance à un serveur applicatif.
- Installation possible depuis le navigateur via les mécanismes PWA.
- Configuration correcte du `manifest.json` et du Service Worker.

### Fonctionnement hors ligne
- Fonctionnement intégral sans connexion Internet après le premier chargement.
- Mise en cache des ressources applicatives (HTML, CSS, JavaScript, images, etc.).
- Persistance locale des données et des cartes dans le navigateur.
- Utilisation d'IndexedDB pour le stockage des cartes et des données structurées.
- Conservation des données après fermeture et redémarrage de l'application.
- Gestion des mises à jour du cache et des nouvelles versions de l'application.

### Gestion des cartes
- Importation, consultation et manipulation des cartes directement dans le navigateur.
- Stockage local persistant.
- Aucun transfert automatique vers un serveur distant.
- Fonctionnement hors ligne de toutes les fonctionnalités compatibles avec les API Web.

## 2. Version Android (APK)

La version Android doit reprendre toutes les fonctionnalités de la PWA standard et y ajouter les capacités natives accessibles via Android.

L'objectif est de conserver une expérience utilisateur et un fonctionnement identiques entre les deux versions, tout en exploitant les possibilités supplémentaires de l'APK.

### Fonctionnalités natives supplémentaires

**A. Gestion native des fichiers**
- Accès au système de fichiers Android.
- Importation et exportation de fichiers.
- Ouverture et sauvegarde de fichiers via les sélecteurs natifs Android.
- Gestion des URI et des permissions nécessaires.
- Possibilité de manipuler des fichiers sans passer par les limitations du navigateur.

**B. Gestion de la luminosité ambiante (capteur LUX)**
- Lecture du capteur de luminosité ambiante Android.
- Adaptation automatique de l'interface en fonction de la luminosité.
- Gestion dynamique du thème et de la lisibilité (clair/sombre).
- Prévoir des seuils configurables pour éviter les changements intempestifs.

**C. Détection de présence d'une carte NFC**
- Détection de la proximité d'une carte NFC.
- Détection de la présentation ou du retrait d'une carte lorsque la technologie le permet.
- Exploitation des capacités NFC natives d'Android.
- Gestion des événements NFC et des permissions nécessaires.
- Communication entre le module NFC natif et l'interface applicative.

## 3. Architecture technique

Étudier et proposer une architecture permettant de maintenir un socle de code commun.

La solution doit permettre :
- De partager l'interface et la logique métier entre Web et Android.
- D'isoler les fonctionnalités natives Android dans des modules spécifiques.
- De détecter automatiquement l'environnement d'exécution (navigateur ou APK).
- D'activer ou désactiver les fonctionnalités selon les capacités réellement disponibles.
- De conserver une compatibilité maximale avec l'hébergement statique.

Étudier notamment la pertinence de Capacitor pour générer l'application Android à partir du projet Web existant.

## 4. Gestion des fonctionnalités selon la plateforme

Mettre en place une couche d'abstraction permettant de distinguer les capacités disponibles.

| Fonctionnalité | PWA Web | APK Android |
|---|---|---|
| Consultation des cartes | Oui | Oui |
| Importation de cartes | Oui | Oui |
| Persistance hors ligne | Oui | Oui |
| Stockage local | IndexedDB | Stockage natif / IndexedDB |
| Import / export natif | Limité aux API Web | Oui |
| Capteur de luminosité LUX | Selon API disponible | Oui, via API native |
| Détection NFC | Selon compatibilité Web NFC | Oui, via API native |
| Installation | Navigateur | APK |

Les fonctionnalités non disponibles sur une plateforme ne doivent pas provoquer d'erreur : elles doivent être désactivées proprement.

## 5. Livrables attendus

- Une version PWA statique fonctionnelle et installable.
- Une version Android compilable en APK.
- Un socle de code commun.
- Une architecture modulaire pour les fonctionnalités natives.
- Une documentation de compilation et de déploiement.
- Une documentation des différences fonctionnelles entre les deux plateformes.

## 6. Méthode de travail

Avant toute modification :

1. Analyser l'architecture actuelle du projet.
2. Identifier les technologies et les dépendances existantes.
3. Évaluer les possibilités de réutilisation du code.
4. Identifier les éventuelles incompatibilités avec GitHub Pages et Android.
5. Proposer une architecture technique adaptée.

Ensuite, procéder à l'implémentation par étapes, sans régression sur les fonctionnalités existantes.

**Contrainte fondamentale :** la version Web doit rester totalement autonome, statique et hébergeable sur GitHub Pages. Les fonctionnalités natives doivent être des extensions de la version Android et ne doivent jamais devenir des dépendances obligatoires du fonctionnement de la PWA.
