#!/bin/bash
DEST="/Users/rayanenasri/Documents/Internships/aGrUMLab/.venv/lib/python3.13/site-packages/pyagrum"
# Copier le dossier explain
rsync -av --delete --dry-run --exclude='__pycache__' explain/ "$DEST/explain/"
