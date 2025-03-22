#!/bin/bash

# Requires one parameter:
#   - conda env to use

set -e -x

if [ "$#" -ne 1 ]; then
    echo "Expected one parameter: conda environment"
    exit 1
fi

CONDA_ENV=$1

echo "Creating wheels directory..."
mkdir -p ${CI_PROJECT_DIR}/wheels

echo "Updating version with build number..."
python ${CI_PROJECT_DIR}/wrappers/pyagrum/wheelhouse/scripts/update_version.py ${CI_PROJECT_DIR} 0

echo "Initializing Conda..."
. /Users/agrum/miniconda3/etc/profile.d/conda.sh

echo "Activating Conda environment: ${CONDA_ENV}"
conda activate $CONDA_ENV

echo "Cleaning previous builds..."
python act clean

echo "Building pyAgrum wheel..."
python act release wheel pyAgrum -j halfexcept1 -d ${CI_PROJECT_DIR}/wheels

echo "Deactivating Conda environment..."
conda deactivate

echo "Script execution completed."
