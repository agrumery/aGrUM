import os
import shutil
from pathlib import Path

# Répertoire courant : pyagrum/pyLibs/explain
src_dir = Path(__file__).parent

# Dossier de destination : pyagrum/env/lib/python3.13/site-packages/pyagrum/explain
dest_dir = src_dir.parents[5] / '.venv' / 'lib' / 'python3.13' / 'site-packages' / 'pyagrum' / 'lib'

# Créer le dossier s'il n'existe pas
dest_dir.mkdir(parents=True, exist_ok=True)

# Copier tous les .py (en écrasant si existe)
for file in src_dir.glob("*.py"):
    dest_file = dest_dir / file.name
    shutil.copy(file, dest_file)
    print(f"✅ {file.name} copié vers {dest_file}")