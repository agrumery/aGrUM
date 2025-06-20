#!/bin/bash

# Requires one parameter:
#   - conda env to use

set -e -x

if [ "$#" -ne 2 ]; then
    echo "Expected one parameter: conda environment, target type (aGrUM or pyAgrum)"
    exit 1
fi

CONDA_ENV=$1
TARGET=$2

echo "Sourcing .zshrc to set up the environment..."
source /Users/agrum/.zshrc

echo "Activating Conda environment: ${CONDA_ENV}"
conda activate $CONDA_ENV

echo "Cleaning previous builds..."
python act clean

if [ "${TARGET}" == "aGrUM" ]; then
    echo "Building aGrUM..."
    python act --no-fun test release aGrUM -j halfexcept1 -t all -m all --compiler=clang
elif [ "${TARGET}" == "pyAgrum" ]; then
    echo "Building pyagrum..."
    python act --no-fun test release pyAgrum -j halfexcept1  -m quick -t all --compiler=clang
else
    echo "Invalid target type: ${TARGET}. Expected 'aGrUM' or 'pyAgrum'"
    exit 1
fi

echo "Deactivating Conda environment..."
conda deactivate

echo "Script execution completed."
