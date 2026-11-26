#!/usr/bin/env bash
# Réécrit tout l'historique du dépôt pour qu'il n'y ait plus qu'un seul contributeur :
#  - auteur et committer de TOUS les commits remplacés par ton identité
#  - toutes les lignes "Co-authored-by:" supprimées des messages
#  - la ligne "Generated with Claude Code" supprimée
#
# Usage : ./single-contributor.sh [URL_DU_REPO] ["Ton Nom"] [ton@email]
# Prérequis : git-filter-repo  (pip install git-filter-repo  /  brew install git-filter-repo)
#
# IMPORTANT : l'e-mail doit être un e-mail vérifié de ton compte GitHub
# (ou ton ID@users.noreply.github.com), sinon GitHub ne te rattachera pas les commits.

set -euo pipefail

REPO_URL="${1:-git@github.com:valerebron/virages.git}"
NEW_NAME="${2:-$(git config --global user.name || true)}"
NEW_EMAIL="${3:-$(git config --global user.email || true)}"

if [[ -z "$NEW_NAME" || -z "$NEW_EMAIL" ]]; then
  echo "Erreur : nom ou e-mail manquant. Passe-les en arguments ou configure git config --global." >&2
  exit 1
fi

if ! command -v git-filter-repo >/dev/null 2>&1; then
  echo "Erreur : git-filter-repo n'est pas installé (pip install git-filter-repo)." >&2
  exit 1
fi

export NEW_NAME NEW_EMAIL
WORKDIR="$(mktemp -d)/virages-clean.git"

echo "==> Clone miroir (toutes les branches et tags) dans $WORKDIR"
git clone --mirror "$REPO_URL" "$WORKDIR"
cd "$WORKDIR"

echo "==> Contributeurs AVANT :"
git log --all --format='%an <%ae>' | sort | uniq -c | sort -rn

echo "==> Réécriture de l'historique"
git filter-repo --commit-callback '
import os, re
name  = os.environ["NEW_NAME"].encode()
email = os.environ["NEW_EMAIL"].encode()

commit.author_name     = name
commit.author_email    = email
commit.committer_name  = name
commit.committer_email = email

msg = commit.message
msg = re.sub(rb"(?im)^[ \t]*co-authored-by:.*(?:\r?\n|$)", b"", msg)
msg = re.sub(rb"(?im)^.*generated with .*claude code.*(?:\r?\n|$)", b"", msg)
commit.message = msg.rstrip() + b"\n"
'

echo "==> Contributeurs APRÈS :"
git log --all --format='%an <%ae>' | sort | uniq -c | sort -rn

echo
read -r -p "Pousser (force) vers $REPO_URL ? [o/N] " ans
if [[ "${ans,,}" != "o" ]]; then
  echo "Annulé. Le dépôt nettoyé reste dans : $WORKDIR"
  exit 0
fi

# filter-repo retire le remote origin
git remote add origin "$REPO_URL" 2>/dev/null || git remote set-url origin "$REPO_URL"
git push --force --all origin
git push --force --tags origin

echo "==> Terminé."
echo "Pense à : désactiver temporairement la protection de branche si le push a été refusé,"
echo "et à re-cloner le dépôt partout ailleurs (git fetch + reset --hard ne suffit pas toujours)."
