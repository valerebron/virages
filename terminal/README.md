# minitel.md — Nouveau PCB logique unique pour Minitel 1B → terminal Raspberry Pi 3B / 4

## Objectif du projet

Retirer la **carte logique d'origine du Minitel 1B** (celle qui porte le port DIN, le contrôleur TELIC, etc.) et la remplacer par un **unique PCB personnalisé** qui transforme le Minitel en terminal utilisable avec un **Raspberry Pi 3B ou 4**.

**La prise DIN n'est pas utilisée comme connecteur** : avec le retrait de la carte logique d'origine, le port DIN disparaît avec elle. Cependant, comme le montre Chapelier Fou, il est possible de prélever le **12 V, la masse, le TX et le RX directement sous le connecteur DIN en soudant des fils sur la carte mère**, là où le connecteur est situé.

Le nouveau PCB réunit trois fonctions sur un seul support :

1. **Conversion de la nappe clavier Minitel 1B en périphérique USB standard** pour le Raspberry Pi
2. **Entrée vidéo composite** depuis le Raspberry Pi vers l'écran cathodique du Minitel
3. **Alimentation du Raspberry Pi** (5 V) prélevée depuis la carte d'alimentation du Minitel 1B (~12 V)

---

## 0. Le Minitel 1B — ce qui est conservé et ce qui est retiré

| Élément | Sort | Rôle dans le nouveau projet |
|---------|------|-----------------------------|
| Carte logique d'origine (port DIN, TELIC, etc.) | **Retirée complètement** | Remplacée par le nouveau PCB unique |
| Prise DIN 5 broches | **Disparaît avec la carte logique** | Non utilisée ; le 12 V est prélevé directement sur la carte d'alimentation interne du Minitel |
| Nappe clavier (matrice lignes × colonnes) | **Conservée** | Raccordée au nouveau PCB → scan GPIO → clavier USB pour le Pi |
| Écran cathodique intégré | **Conservé** | Reçoit du vidéo composite depuis le Raspberry Pi via le nouveau PCB |
| Carte d'alimentation interne (220 V → tensions continues) | **Conservée** | Fournit le ~12 V prélevé par le nouveau PCB pour générer le 5 V du Pi |

> ⚠️ **Danger CRT :** le Minitel est un écran cathodique. Même débranché, le tube et le transformateur de ligne (flyback) peuvent retenir plusieurs milliers de volts. Les condensateurs peuvent rester chargés longtemps après extinction. Ne touchez pas au culot haute tension de l'anode ni aux circuits proches du transformateur si vous n'êtes pas expérimenté.

---

## 1. Fonction 1 — Nappe clavier Minitel 1B → clavier USB (Raspberry Pi)

Le clavier du Minitel 1B est une **matrice** (pas d'USB natif, pas de contrôleur clavier indépendant accessible). Il n'y a pas de contrôleur USB intégré. Il faut donc lire la matrice depuis le Raspberry Pi et émuler un clavier USB.

### Approche : scan de la matrice sur les GPIO du Raspberry Pi

**Projet de référence :** https://github.com/Cl0v1s/Minitel-Keyboard

- Script Python qui scanne un **clavier matriciel Minitel 1** (compatible 1B) via les **GPIO** du Raspberry Pi.
- Dépendances : `evdev`, `RPi.GPIO` (`pip3 install evdev RPi.GPIO`).
- Fichiers : `keyboard.py` (scan matrice), `keyboard.conf` / `keymap.conf` (mappage touches → keycodes `evdev`, auto-enregistrement).
- Une fois détecté, le clavier apparaît comme un **clavier USB standard** pour le système (getty, login, bash, etc.).
- Nécessite de **relever le brochage de la nappe clavier** du Minitel 1B et de fabriquer un connecteur adapté sur le nouveau PCB.

### Intégration sur le nouveau PCB

- Connecteur pour la nappe clavier du Minitel 1B (côté clavier).
- Broches GPIO du Raspberry Pi dédiées au scan de la matrice (lignes et colonnes).
- Le Raspberry Pi tourne avec le script de scan + `evdev` pour exposer le clavier en périphérique d'entrée standard.

---

## 2. Fonction 2 — Vidéo composite (Raspberry Pi → écran Minitel 1B)

Le nouveau PCB doit prendre la **sortie composite du Raspberry Pi** et l'injecter sur l'**écran cathodique du Minitel 1B**.

### Référence existante : https://github.com/lucblender/lx-composite-to-minitel

- Projet KiCad d'ajout d'une entrée vidéo composite sur un Minitel.
- PCB en **SMD**, BOM avec références Mouser, fabriqué chez PCBWay.
- Reprend le travail de **cfp-radio** (partie vidéo uniquement).

### Contrainte mécanique à adapter sur le nouveau PCB

Le connecteur de sortie de la référence lucblender est en pitch **2,54 mm**, tandis que le connecteur d'origine de la carte afficheur du Minitel est en pitch **5,08 mm**.

Sur votre nouveau PCB unique :
- Prévoir un **connecteur 2,54 mm côté Raspberry Pi** (entrée composite).
- Prévoir l'interface vers la carte afficheur du Minitel 1B (soit un câble adaptateur 2,54 mm → 5,08 mm, soit un connecteur directement adapté si vous redessinez l'interface afficheur).
- Si vous retirez la carte afficheur d'origine, vous pouvez laisser le nouveau PCB "flotter" dans l'espace libéré, ou le fixer proprement.

---

## 3. Fonction 3 — Alimentation du Raspberry Pi depuis le Minitel 1B

### Ce qui a déjà été fait (sources documentées)

Plusieurs projets ont déjà documenté l'alimentation d'un Raspberry Pi depuis un Minitel 1B. Vous n'avez pas besoin de concevoir cette partie à partir de zéro.

#### A. Chapelier Fou — « Minitel 2.0 » (2019)
**URL :** https://blog.chapelierfou.org/a-minitel-2.0.html

- Intègre un Raspberry Pi dans le boîtier d'un Minitel 1B (emplacement d'extension arrière).
- **Alimentation :** régulateur 12 V → 5 V. Il utilise un **régulateur électronique de voiture**, mais indique qu'un **5V UBEC (Universal Battery Eliminator Circuit)** de modélisme RC ferait parfaitement l'affaire.
- Le Pi est alimenté par la sortie 5 V du régulateur vers un pin 5 V du GPIO.
- Adaptation des niveaux série vers 3,3 V du Pi.
- Remarque : dans son montage, Chapelier Fou prélève le 12 V sous l'emplacement du connecteur DIN sur la carte mère. Comme vous retirez complètement la carte logique d'origine, vous devrez prélever ce 12 V **directement sur la carte d'alimentation interne du Minitel** (voir §3 ci-dessus).

#### B. faxm0dem/pi0tel (GitHub) — uHat pour Pi Zero sur DIN-5 Minitel
**URL :** https://github.com/faxm0dem/pi0tel

- uHat conçu pour se glisser sur un **Raspberry Pi Zero / Zero W** et se brancher sur la **DIN-5 d'un Minitel-2**.
- Inclut un **convertisseur Buck LM2596** sur le hat pour alimenter le Pi depuis le 12 V du Minitel.
- Le schéma et le PCB KiCad sont disponibles dans le dépôt.
- Notes de conception précises pour le buck LM2596 (voir ci-dessous).

#### C. ghettobastler — « Pi to Minitel adapter »
**URL :** https://ghettobastler.com/pi_to_minitel_adapter.html

- Ajoute un régulateur sur la broche 5 du DIN-5 du Minitel 1B (qui fournit du **8,5 V**) pour alimenter le Raspberry Pi via ses GPIO.
- Schémas téléchargeables sur le site.

---

### Solution recommandée pour votre brique 1

Puisque vous retirez la carte logique d'origine, la méthode de Chapelier Fou est la plus adaptée : souder 4 fils sous l'emplacement du connecteur DIN sur la carte mère pour récupérer **12 V + GND**, puis passer par un convertisseur buck 12 V → 5 V.

#### Composants conseillés (version simple et pas chère)

| Composant | Spécification | Prix estimé | Usage |
|-----------|--------------|-------------|-------|
| **Module buck LM2596** (circuit bleu avec potentiomètre) | Entrée 4–35 V, sortie réglable 1,2–35 V, **3 A max** | 2–4 € | Abaisser le 12 V du Minitel en 5 V pour le Pi |
| **Fusible rapide** 250 V, **2,5 A** ou 3 A | Cartouche 5 × 20 mm | 0,50–1 € | Protéger le Minitel en cas de court-circuit |
| **Porte-fusible** pour cartouche 5 × 20 mm | À souder ou à vis | 1–2 € | Maintenir le fusible |
| **Borniers à vis** 2 pôles (ou connecteur JST 2 broches) | Entrée et sortie | 1–2 € | Connexions propres |
| **Fils isolés** | 0,5 mm² minimum, rouge et noir | ~2 € | Câblage |
| **Gaine thermorétractable** | Divers diamètres | 1–2 € | Isoler les soudures |
| **Condensateur électrolytique** 100 µF ou 470 µF, 25 V | Lissage en sortie (recommandé Pi 4) | 0,50 € | Stabilité du 5 V |
| **Condensateur céramique** 100 nF (0,1 µF), 50 V | Découplage haute fréquence | 0,20 € | Stabilité du 5 V |

**Total estimé : 8 à 15 €**

#### Option « neuf, le moins d'opérations » : adaptateur allume-cigare 12 V → USB 5 V

Si vous voulez du neuf avec le moins de soudure possible, achetez un **adaptateur allume-cigare 12 V → USB 5 V 2,1 A ou 2,4 A** (5–10 € neuf sur Amazon/eBay). C'est un boîtier tout fait déjà calibré en 5 V.

**Les seules opérations à faire :**

1. Coupez le câble côté allume-cigare, gardez la prise USB (côté Pi).
2. Repérez le **+12 V et le 0 V** sur la carte d'alimentation interne du Minitel (multimètre, Minitel allumé, côté basse tension uniquement).
3. Soudez le fil rouge (+) sur le **+12 V** du Minitel. **Ajoutez obligatoirement un petit fusible rapide 2,5 A sur ce fil, dès sa sortie de la carte d'alimentation du Minitel** (voir § ci-dessous).
4. Soudez le fil noir (GND) sur la **masse** du Minitel.
5. **Vérifiez au multimètre** que la prise USB sort bien 5 V.
6. Branchez l'USB sur le Raspberry Pi.

> ⚡ **Attention au courant :** un adaptateur à **700 mA est insuffisant** pour un Raspberry Pi 3B ou 4. Le Pi a besoin d'au moins **1,5 A** (Pi 3B) à **2,5 A** (Pi 4). Prenez du **2,1 A ou 2,4 A minimum**.

> ⚠️ **Vérifiez impérativement la sortie USB au multimètre avant de brancher le Pi :** au-dessus de 5,2 V, vous grillez le Pi.

#### Alternative : module buck LM2596 (si vous voulez régler vous-même)

Si vous préférez un module réglable (2–4 €), prenez un **LM2596 12 V → 5 V, 3 A**. Il faudra régler le potentiomètre à exactement 5,00 V avant de brancher le Pi.

#### Ajouter le fusible de protection (obligatoire)

Même si votre adaptateur allume-cigare a peut-être un fusible interne, vous **devez** ajouter un fusible sur le **+12 V dès sa sortie de la carte d'alimentation du Minitel**, pour protéger le câble et la carte alim du Minitel en cas de court-circuit.

| Composant | Spécification | Prix estimé |
|-----------|--------------|-------------|
| **Fusible rapide** 250 V, **2,5 A** ou 3 A | Cartouche 5 × 20 mm | 0,50 – 1 € |
| **Porte-fusible** pour cartouche 5 × 20 mm | À souder ou à vis | 1 – 2 € |
| **Total** | | **~2 à 3 €** |

**Où le mettre :** coupez le fil rouge (+12 V) entre la carte alim du Minitel et l'adaptateur, soudez chaque extrémité sur le porte-fusible, insérez le fusible 2,5 A, isolez avec de la gaine thermorétractable.

Le fil noir (0 V) reste direct, sans fusible.

#### Schéma de câblage de la brique 1

```
Carte d'alimentation interne du Minitel 1B (~12 V continu)
   │
   ├───►  +12 V ─── [ Fusible 2,5 A ] ───►  +IN du buck LM2596
   │
   └───►  0 V (masse) ────────────────────►  -IN du buck LM2596
                                                        │
                                                        ▼
                                                    +5 V ───► Raspberry Pi (GPIO pin 2 ou 4)
                                                    GND ───► Raspberry Pi (GPIO pin 6, 9, 14...)
                                                        │
                                                        └───► [ Condensateurs 100 µF + 100 nF ] près de la sortie
```

**Règle d'or :** le **+12 V d'entrée** du buck passe **obligatoirement par le fusible** dès qu'il sort de la carte alim du Minitel.

#### Réglage critique (AVANT de brancher le Pi)

1. Reconnectez le Minitel au secteur (uniquement pour avoir le 12 V sur la carte alim).
2. Allumez le Minitel.
3. Mesurez au multimètre la tension de sortie du buck (**+OUT par rapport à -OUT**).
4. Réglez le petit potentiomètre bleu du buck pour obtenir exactement **5,00 V** (jamais au-dessus de 5,2 V, sinon vous grillez le Pi).
5. Éteignez le Minitel et débranchez-le du secteur avant de connecter le Raspberry Pi.

---

## 4. Architecture du nouveau PCB unique (3-en-1)

Le nouveau PCB remplace la carte logique d'origine du Minitel 1B et réunit :

| Fonction | Entrée (côté Minitel) | Sortie (côté Raspberry Pi / écran) | Technologie clé |
|----------|----------------------|------------------------------------|-----------------|
| 1. Clavier | Nappe clavier Minitel 1B (matrice) | Clavier USB / GPIO pour Raspberry Pi | Scan matrice GPIO + `evdev` (Cl0v1s/Minitel-Keyboard) |
| 2. Vidéo composite | Composite depuis Raspberry Pi | Vidéo composite vers écran Minitel 1B | Entrée composite (lx-composite-to-minitel) |
| 3. Alimentation | ~12 V carte d'alimentation interne | 5 V > 2 A pour Raspberry Pi | Convertisseur buck 12 V → 5 V + fusible |

### Spécifications du PCB

- **Format** : adapté pour tenir dans l'emplacement libéré après retrait de la carte logique d'origine (mesurer au calibre avant de dessiner).
- **Connecteurs côté Minitel :**
  - Connecteur nappe clavier (matrice),
  - Prise d'alimentation depuis la carte d'alimentation interne (~12 V + 0 V),
  - Interface vers l'écran cathodique (composite).
- **Connecteurs côté Raspberry Pi :**
  - USB (ou GPIO) pour le clavier,
  - Entrée vidéo composite,
  - Alimentation 5 V en entrée,
  - GPIO pour le scan clavier.
- **Niveaux :** adaptation 12 V / 5 V / 3,3 V si nécessaire.
- **Masse commune** entre le Minitel, le nouveau PCB et le Raspberry Pi.

---

## 5. Étapes de réalisation suggérées

### Phase 1 — Valider l'alimentation (brique 1)

1. **Démonter le Minitel 1B** et mesurer l'emplacement de l'ancienne carte logique.
2. **Localiser le ~12 V continu** directement sur la **carte d'alimentation interne** du Minitel 1B (côté basse tension, après le transformateur/redresseur).
3. **Monter le buck 12 V → 5 V** sur une platine d'essai avec fusible et condensateurs.
4. **Réglage à 5,00 V** au multimètre, puis test avec le Raspberry Pi (Pi démarre-t-il ?).
5. Une fois validé, intégrer le buck dans le boîtier.

### Phase 2 — Vidéo et clavier

6. **Valider l'entrée composite** sur l'écran du Minitel 1B (PCB lucblender ou équivalent).
7. **Relever le brochage de la nappe clavier** et valider le scan matrice sur breadboard.
8. **Dessiner le schéma unique** dans KiCad (s'inspirer de `av-to-minitel` pour la partie vidéo, et des schémas de Chapelier Fou / pi0tel pour l'alim).
9. **Fabriquer un prototype** et tester avec un Raspberry Pi 3B ou 4.
10. **Retirer la carte logique d'origine** et intégrer le nouveau PCB dans le boîtier du Minitel 1B.

---

## 6. Sources (crawl de `history.csv` + recherche complémentaire)

Le fichier `history.csv` contient 306 URLs. Une colonne **`site crawled ?`** a été ajoutée : `true` pour les sites effectivement consultés, `false` pour les autres.

### Sites consultés et infos retenues

| Site | Infos clés |
|------|------------|
| https://github.com/Cl0v1s/Minitel-Keyboard | Clavier matrice Minitel 1/1B → GPIO Raspberry Pi, Python, evdev, RPi.GPIO, mappage auto des touches |
| https://github.com/lucblender/lx-composite-to-minitel | Entrée composite Minitel, KiCad SMD, adaptateur 2,54 mm ↔ 5,08 mm, cfp-radio |
| https://pila.fr/wordpress/archives/361 | Minitel 1B/2, brochage DIN 5, pull-up 200 kΩ, 2N2222A, specs entrée/sortie Minitel, 1200/4800 bauds 7E1 |
| https://tonyisageek.blogspot.com/p/telic-alcatel-minitel-running-as-usb.html | Minitel Telic → USB PL2303HX, 2N2222A, 10k/22k/220k, mapping broches, stty 1200 7E1, getty |
| https://www.furrtek.org/index.php?a=telinux | Minitel 1B bistandard → terminal Linux série, 2× 2N2222, 2× 10k + 1× 15k, niveaux 5 V ↔ 12 V |
| https://x0r.fr/blog/5 | Minitel 1B Telic comme terminal Linux, config Fnct+T A/E, Fnct+P 4, agetty/mgetty |
| https://x0r.fr/blog/19 | Minitel sur Raspberry Pi, GPIO 14/15 direct, résistances 4,7k/12k, alim Pi depuis DIN broche 5 via buck 12V→5V >1A |
| https://www.jelora.fr/post/2020/04/12/Sortie-video-composite-RGB-couleur-et-n-b-et-sortie-audio-pour-Minitel-1B.html | EF9345 RGB + synchro 15 kHz, AD722 RGB→composite, LM1881, RGB 5V→0,7V, N&B 30/59/11, audio NE555/74LS08 |
| https://millevaches.hydraule.org/info/minitel/specs/ | Spécifications techniques Minitel 1B (STUM) |
| https://blog.chapelierfou.org/a-minitel-2.0.html | **Intégration Pi dans Minitel 1B, prélèvement 12V/GND/TX/RX sous connecteur DIN sur carte mère, régulateur 12V→5V (voiture ou UBEC RC), panneau arrière imprimé 3D, PyMinitel, terminfo m1b-x80** |
| https://github.com/faxm0dem/pi0tel | **uHat Pi Zero sur DIN-5 Minitel, buck LM2596 sur le hat, schéma et PCB KiCad, notes composants LM2596 (Cin 270/330µF, Cout 470/25 ou 560/16, diode 800mA/10V)** |
| https://ghettobastler.com/pi_to_minitel_adapter.html | Régulateur sur broche 5 du DIN (8,5V) pour alimenter le Pi via GPIO |

---

## 7. Todo (nouveau PCB Minitel 1B)

- [ ] Identifier le modèle exact de Minitel 1B et vérifier l'accès à la nappe clavier, carte alim et écran.
- [ ] Localiser le ~12 V continu sur la carte d'alimentation interne du Minitel 1B (côté basse tension).
- [ ] Relever le brochage de la nappe clavier du Minitel 1B.
- [ ] Valider le scan matrice clavier → GPIO/RPi sur breadboard.
- [ ] Valider l'entrée composite sur l'écran du Minitel 1B.
- [ ] Concevoir le convertisseur buck 12 V → 5 V (> 2 A) avec fusible et condensateurs (réutiliser schéma Chapelier Fou / pi0tel).
- [ ] Dessiner le PCB unique KiCad (3-en-1 : clavier, composite, alim).
- [ ] Fabriquer un prototype et tester avec un RPi 3B / 4.
- [ ] Retirer la carte logique d'origine et intégrer le nouveau PCB dans le boîtier.

---

## 8. Sécurité

- **Ne travaillez jamais sous tension.** Débranchez le Minitel, attendez que les condensateurs se déchargent (plusieurs minutes pour le CRT), vérifiez l'absence de tension au multimètre avant de toucher la carte alim ou la carte mère.
- **Danger CRT :** tensions de plusieurs kV sur le tube et le transformateur de ligne. Ne touchez pas au culot d'anode ni aux circuits haute tension.
- **Restez sur le côté basse tension** (~12 V continu) si vous n'êtes pas expérimenté avec le secteur 220 V.
- **Le fusible est obligatoire** en entrée du buck (sur le +12 V) pour protéger le Minitel en cas de court-circuit.
- **Masse commune obligatoire** : le 0 V de la carte alim/carte mère Minitel, le 0 V du buck et le GND du Raspberry Pi doivent être reliés ensemble.
- **Aucun fil dénudé ne doit toucher le châssis métallique** du Minitel.

### Gants de protection

- **Ne portez pas de gants en latex** pour souder : ils fondent avec le fer à souder (300–400 °C) et ne protègent pas du 220 V.
- **Ne portez pas de gants de soudure en cuir épais** : vous ne pourrez pas souder avec précision.
- Si vous voulez garder les composants propres (pas de traces de doigts), des **gants en nitrile jetables** (5–10 € la boîte) ou des **gants fins en coton** suffisent, mais uniquement **hors tension**. Ils ne protègent pas de l'électricité.
- La meilleure protection reste de **débrancher le Minitel** et de vérifier l'absence de tension au multimètre avant de commencer.

---

*Document généré à partir du crawl de `history.csv` et des sources listées ci-dessus, centré sur le Minitel 1B avec remplacement complet de la carte logique par un nouveau PCB unique (sans prise DIN comme connecteur, mais prélèvement 12V possible sous l'emplacement DIN sur la carte mère). À compléter à mesure de l'avancement du projet hardware.*
