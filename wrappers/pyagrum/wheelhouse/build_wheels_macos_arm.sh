#!/bin/zsh

# Requires four parameters:
#   - conda env to use

# Source the zshrc to ensure the environment is set up correctly
echo "Sourcing .zshrc to set up the environment..."
source /Users/agrum/.zshrc

# Exit immediately if any command has a non-zero exit status and print commands and their arguments as they are executed
set -e -x

# Check for the correct number of parameters
if [ "$#" -ne 1 ]; then
    echo "Expected one parameter: conda environment"
    exit 1
fi

CONDA_ENV=$1

# Remove existing directories and create a fresh wheels directory
echo "Preparing the environment by cleaning and creating necessary directories..."
mkdir -p ${CI_PROJECT_DIR}/wheels

# Activate the specified conda environment
echo "Activating the conda environment: $CONDA_ENV..."
conda activate $CONDA_ENV

# Update version and build the wheel
echo "Updating the project version with the build number..."
python ${CI_PROJECT_DIR}/wrappers/pyagrum/wheelhouse/scripts/update_version.py ${CI_PROJECT_DIR} 0

echo "Cleaning previous build artifacts..."
python act clean

echo "Building the pyAgrum wheel..."
python act release wheel pyAgrum -j halfexcept1 -d ${CI_PROJECT_DIR}/wheels --compiler=clang

# Deactivate the conda environment
echo "Build complete. Deactivating the conda environment..."
conda deactivate

echo "Script execution completed successfully."