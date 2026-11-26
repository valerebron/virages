# PTT Dial Tester

Banc d'essai pour cadrans téléphoniques à impulsions.

## Matériel

- Seeed Studio XIAO ESP32-S3
- 1 entrée GPIO (D10)
- Condensateur 100 nF entre D10 et GND
- Contact d'impulsion du cadran

## Objectifs

- Décodage fiable des impulsions
- Mesure de la cadence (PPS)
- Mesure du rapport Break / Make
- Diagnostic automatique
- Réglage des cadrans U43, S63

## État du projet

- ✅ Initialisation
- ⏳ Lecture du GPIO
- ⏳ Décodeur
- ⏳ Statistiques
