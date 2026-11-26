L'app que je développe sert à lire des carte NFC, une carte NFC = 1 ficher,
la carte est scanné,
elle apparait sur l'écran,
je click dessus
je lis le fichier associé à la carte avec le lecteur correspondant au format du fichier. 

Chaque carte lue est ajouter (stockage local avec PWA) à la collection (sorte de bibliothèque de carte)
Je pe

C'est une App Capacitor qui s'installe
- soit sur un android en APK
- soit sur un serveur WEB en PWA

ANDROID
Le front sur android est dédié à une interface particulière puisque les device android se trouve dans une boite en bois avec un couvercle qui masque en parti l'écran pour ne laissé apparaitre qu'un cercle au centre de l'écran (sur toute la largeur de l'écran).
les écrans maquette se dessine toujours dans un cercle du coups.
Ce front à pour seule interface utilisateur un lecteur de carte NFC.
Les écran bascules selon cette interface.
Aussi l'apk expose un back office web prévu pour les smartphones.
Dans ce back office on retrouve les formulaires de gestions des cartes (ajout/edition/liste)

WEB
La version web 

Conception UI/UX – Application NFC Card Reader
1. Présentation du projet

Je développe une application permettant de lire, gérer et créer des cartes NFC.

Le principe fondamental de l'application est simple :

Une carte NFC correspond à un fichier numérique.

Lorsqu'une carte NFC est scannée :

L'application détecte la carte.
La carte apparaît à l'écran.
L'utilisateur sélectionne la carte.
L'application ouvre le fichier associé.
Le fichier est affiché dans un lecteur adapté à son format.

Exemples de formats possibles :

PDF : lecteur PDF.
Image : visionneuse d'images.
Audio : lecteur audio.
Vidéo : lecteur vidéo.
Texte : lecteur de texte.
Autres formats : lecteur adapté.

Chaque carte NFC découverte est automatiquement ajoutée à une collection locale, qui fonctionne comme une bibliothèque numérique de cartes.

L'utilisateur peut ainsi :

Scanner une nouvelle carte.
Retrouver les cartes déjà scannées.
Parcourir sa collection.
Ouvrir une carte et consulter son contenu.
Créer une nouvelle carte.
Associer un fichier à une carte NFC.
2. Architecture technique et plateformes

L'application est développée avec Capacitor.

Elle est distribuée sous deux formes :

Android : application native installable sous forme d'APK.
Web : application accessible depuis un navigateur et installable en PWA.

Les deux versions partagent le même socle fonctionnel, mais proposent des expériences utilisateur différentes.

Le stockage de la collection est local, avec une logique de persistance compatible PWA.

Il faut donc penser l'interface comme un produit cohérent, mais avec deux modes d'utilisation distincts.

3. EXPÉRIENCE ANDROID – Lecteur NFC immersif
3.1. Contrainte physique majeure

Les appareils Android sont installés dans un boîtier en bois.

Le boîtier possède un couvercle qui masque une grande partie de l'écran.

Seule une zone circulaire située au centre de l'écran reste visible.

Cette zone circulaire :

Occupe toute la largeur de l'écran.
Est centrée horizontalement.
Constitue l'unique zone visible pour l'utilisateur.
Détermine entièrement la conception de l'interface.

CONTRAINTE ABSOLUE : tous les écrans Android doivent être conçus exclusivement à l'intérieur d'un cercle.

Aucun élément fonctionnel important ne doit être placé en dehors de cette zone.

Les maquettes doivent être pensées comme des interfaces circulaires, et non comme des interfaces mobiles classiques simplement recadrées.

Il faut notamment anticiper :

Les textes qui risquent d'être coupés.
Les boutons proches des bords.
Les informations secondaires.
Les transitions entre les écrans.
La lisibilité à distance.
Les interactions tactiles dans une zone réduite.

Le résultat doit être élégant, minimaliste, immédiatement compréhensible et parfaitement adapté à cette contrainte physique.

3.2. Philosophie de l'interface Android

L'application Android est avant tout un lecteur NFC.

Elle ne doit pas ressembler à une application de gestion classique.

L'utilisateur n'a pas accès à une navigation traditionnelle avec menu, barre d'onglets ou sidebar.

Son unique interaction avec le système est le lecteur NFC.

Le parcours doit être extrêmement simple :

L'utilisateur approche une carte NFC.
L'application détecte la carte.
Une représentation visuelle de la carte apparaît dans le cercle.
L'utilisateur clique sur cette représentation.
Le contenu associé s'ouvre dans le lecteur adapté.
L'utilisateur peut revenir à l'écran d'attente pour scanner une autre carte.

L'interface doit être conçue pour donner une impression de simplicité, de fluidité et presque de magie.

3.3. Les écrans Android à concevoir

Prévoir les écrans suivants :

Écran 1 – En attente de lecture

Écran principal de l'application.

Objectif : inviter l'utilisateur à approcher une carte NFC.

Éléments possibles :

Une animation circulaire discrète.
Une icône NFC.
Un texte du type « Approchez votre carte ».
Un retour visuel indiquant que le lecteur est actif.

L'écran doit être très épuré.

Écran 2 – Carte détectée

Lorsqu'une carte est scannée :

Afficher une représentation visuelle de la carte.
Afficher son nom.
Afficher éventuellement son type de contenu.
Proposer une interaction évidente pour ouvrir la carte.

La carte doit devenir l'élément central de l'écran circulaire.

Écran 3 – Lecture du contenu

Lorsqu'on sélectionne la carte, ouvrir le lecteur correspondant au format du fichier.

Prévoir une interface de lecture adaptée aux différents formats :

PDF.
Image.
Audio.
Vidéo.
Texte.

Chaque lecteur doit conserver la logique visuelle circulaire.

Prévoir un moyen simple de revenir à l'écran principal du lecteur NFC.

Écran 4 – Carte inconnue ou non associée

Prévoir un état lorsqu'une carte NFC est détectée mais qu'aucun fichier ne lui est associé.

Afficher une information claire et une indication permettant de comprendre que la carte doit être configurée depuis le back-office.

Écran 5 – Chargement et états intermédiaires

Prévoir les états :

Lecture NFC en cours.
Chargement du fichier.
Fichier introuvable.
Format non pris en charge.
Erreur de lecture.
Carte déjà connue.

Ces états doivent être traités avec la même exigence graphique que les écrans principaux.

4. BACK-OFFICE ANDROID – Interface de gestion

L'application Android expose également un back-office Web.

Contrairement à l'interface principale, ce back-office est destiné à être utilisé directement sur un smartphone.

Il possède donc une interface mobile classique, indépendante de la contrainte circulaire du boîtier.

Ce back-office permet de gérer les cartes NFC.

Fonctionnalités à prévoir
Liste des cartes
Afficher les cartes enregistrées.
Visualiser leur nom.
Visualiser leur type de contenu.
Visualiser leur fichier associé.
Accéder à leur édition.
Ajouter une carte.
Création d'une carte

Prévoir un formulaire permettant :

De renseigner le nom de la carte.
De sélectionner ou scanner son identifiant NFC.
D'ajouter un fichier.
De visualiser le type de fichier détecté.
D'associer le fichier à la carte.
D'enregistrer la carte.
Édition d'une carte

Permettre de :

Modifier son nom.
Modifier son fichier associé.
Consulter son identifiant NFC.
Remplacer le contenu.
Supprimer la carte.

L'interface doit être optimisée pour une utilisation sur smartphone.

5. EXPÉRIENCE WEB – Application complète

La version Web reprend toutes les fonctionnalités de l'application Android, mais ajoute une véritable bibliothèque numérique.

Elle est accessible depuis un navigateur et peut être installée comme PWA.

Contrairement à l'interface Android embarquée dans le boîtier, la version Web possède une interface classique, responsive et complète.

Elle doit être pensée comme une application de consultation et de gestion de collection.

5.1. Navigation principale

Prévoir une navigation claire avec les sections suivantes :

Collection : bibliothèque de toutes les cartes enregistrées.
Lecteur NFC : interface permettant de scanner et ouvrir les cartes.
Créer une carte : création d'une nouvelle carte.
Gestion des cartes : administration, modification et suppression.

La navigation peut prendre la forme d'une sidebar sur desktop et d'une navigation adaptée sur mobile.

5.2. Bibliothèque / Collection

C'est l'écran central de la version Web.

Chaque carte scannée est automatiquement ajoutée à cette bibliothèque locale.

L'utilisateur doit pouvoir parcourir sa collection et retrouver facilement ses contenus.

Prévoir :

Une grille de cartes visuelles.
Une représentation graphique de chaque carte.
Le nom de la carte.
Une indication du format du fichier.
Une date d'ajout éventuellement.
Une recherche.
Des filtres par type de contenu.
Un accès direct à la lecture.

La bibliothèque doit donner envie d'être parcourue.

Chaque carte doit être visuellement identifiable, avec une identité graphique propre et une présentation soignée.

Prévoir plusieurs états :

Collection vide.
Collection avec quelques cartes.
Collection importante.
Recherche sans résultat.
5.3. Fiche d'une carte

Lorsqu'on sélectionne une carte dans la bibliothèque :

Afficher les informations de la carte.
Présenter le fichier associé.
Permettre de lire le contenu.
Permettre de modifier la carte.
Permettre de supprimer la carte.
5.4. Lecteur de contenu

Comme sur Android, le lecteur doit s'adapter automatiquement au format du fichier.

L'expérience Web peut utiliser davantage d'espace et proposer des commandes plus complètes.

6. Direction artistique

Je souhaite une interface moderne, élégante et minimaliste.

L'application doit avoir une véritable identité visuelle.

Éviter absolument l'aspect d'un dashboard SaaS générique.

L'univers graphique doit évoquer :

La collection.
Les objets physiques.
La technologie NFC.
La découverte de contenus.
La bibliothèque personnelle.
L'interaction entre un objet physique et un contenu numérique.

Privilégier :

Une typographie soignée.
Une hiérarchie visuelle claire.
Des espaces généreux.
Des animations discrètes et fluides.
Des cartes graphiques élégantes.
Des icônes simples et cohérentes.
Une palette de couleurs maîtrisée.

L'interface Android circulaire et l'interface Web doivent partager le même langage graphique, tout en respectant leurs usages respectifs.

Ne pas chercher à reproduire artificiellement un cercle dans la version Web.

Le cercle est une contrainte physique propre à l'expérience Android.

7. Ce que j'attends de la conception

Je souhaite obtenir une proposition UI/UX complète, pas uniquement une page d'accueil.

Concevoir les interfaces et les parcours nécessaires aux deux plateformes.

Organiser le travail en trois ensembles :

A. Application Android

Tous les écrans circulaires.
Tous les états du lecteur NFC.
Les différents lecteurs de fichiers.
Les transitions entre les écrans.

B. Back-office Android

Liste des cartes.
Création d'une carte.
Édition d'une carte.
Gestion des fichiers associés.

C. Application Web / PWA

Dashboard ou accueil.
Bibliothèque de cartes.
Recherche et filtres.
Fiche d'une carte.
Lecteur de contenu.
Création et édition.
Gestion de la collection.
8. Consignes importantes pour les maquettes
Commencer par définir un système de design commun.
Concevoir les écrans Android en respectant strictement la zone circulaire visible.
Ne jamais placer d'élément indispensable hors du cercle sur Android.
Concevoir séparément le back-office mobile et l'application Web.
Prévoir les états vides, les erreurs et les chargements.
Utiliser des contenus fictifs réalistes pour rendre les maquettes crédibles.
Assurer une cohérence graphique entre tous les écrans.
Privilégier une expérience utilisateur intuitive et sans complexité inutile.

Point essentiel : il faut concevoir deux expériences produit complémentaires : un lecteur NFC immersif et minimaliste sur Android, et une bibliothèque numérique complète sur le Web.

Ne pas fusionner ces deux expériences dans une interface unique générique.
