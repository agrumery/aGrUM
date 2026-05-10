#!/bin/bash

set -e -x

echo "Script execution started."

# Vérification du paramètre attendu (PYDIR, ex: cp310-cp310)
if [ "$#" -lt 1 ]; then
  echo "Error: Expected at least one parameter for the python install directory."
  exit 1
fi

PYDIR=$1
echo "Python installation directory: ${PYDIR}"

# Nettoyage d'un éventuel ancien venv et création du nouveau
echo "Creating and activating a virtual environment..."
rm -rf ${CI_PROJECT_DIR}/venv
virtualenv venv --python=/opt/python/${PYDIR}/bin/python
source venv/bin/activate

# Installation des dépendances
echo "Installing dependencies..."
# Le chemin est mis à jour car nous sommes déjà à la racine du dépôt aGrUM
pip install -r ${CI_PROJECT_DIR}/wrappers/pyagrum/testunits/requirements.txt

# Installation de la wheel générée par le job précédent
echo "Installing wheels from the wheels directory..."
pip install ${CI_PROJECT_DIR}/wheels/*.whl

# Exécution des tests
echo "Running gumTest.py with MPLBACKEND set to 'AGG'..."
MPLBACKEND="AGG" python ${CI_PROJECT_DIR}/wrappers/pyagrum/testunits/gumTest.py

echo "Script execution finished."