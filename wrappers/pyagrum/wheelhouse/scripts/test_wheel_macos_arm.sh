#!/bin/zsh

# Source .zshrc to set up the environment
source /Users/agrum/.zshrc

set -e -x

# Check for the required parameter
if [ "$#" -ne 1 ]; then
    echo "Error: Expected one parameter for the conda or pyenv environment."
    exit 1
fi

CONDA_ENV=$1
echo "Starting the setup for environment: ${CONDA_ENV}"

# Activate the conda environment
echo "Activating conda environment: ${CONDA_ENV}"
conda activate ${CONDA_ENV}

# Uninstall any existing pyAgrum packages
echo "Uninstalling existing pyAgrum and pyAgrum-nightly packages..."
pip uninstall -y pyAgrum pyAgrum-nightly

# Install new wheels from the wheels directory
echo "Installing new wheels..."
pip install ${CI_PROJECT_DIR}/wheels/*.whl

# Run the gumTest.py to test the installation
echo "Running gumTest.py to test the pyAgrum installation..."
python ${CI_PROJECT_DIR}/wrappers/pyagrum/testunits/gumTest.py

# Uninstall pyAgrum/pyAgrum-nightly after testing
echo "Uninstalling pyAgrum/pyAgrum-nightly..."
pip uninstall -y pyAgrum pyAgrum-nightly

echo "Script execution completed successfully."