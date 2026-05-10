#!/bin/bash

set -e -x

echo "Starting script..."

# Ensure that exactly one parameter has been provided.
if [ "$#" -ne 1 ]; then
  echo "Error: Expected exactly one parameter for the conda environment."
  exit 1
fi

PYENV=$1
echo "Using environment: ${PYENV}"

# Activate the specified environment.
echo "Activating environment ${PYENV}..."
source activate ${PYENV}

# Uninstall any existing pyAgrum installations.
echo "Uninstalling existing pyAgrum and pyAgrum-nightly packages..."
pip uninstall -y pyAgrum pyAgrum-nightly

# Install new wheels from the wheels directory
echo "Installing new wheels..."
pip install ${CI_PROJECT_DIR}/wheels/*.whl

# Run the tests.
echo "Running gumTest.py to validate installation..."
python ${CI_PROJECT_DIR}/wrappers/pyagrum/testunits/gumTest.py

# Uninstall pyAgrum/pyAgrum-nightly after testing.
echo "Uninstalling pyAgrum/pyAgrum-nightly..."
pip uninstall -y pyAgrum pyAgrum-nightly

echo "Script execution finished."