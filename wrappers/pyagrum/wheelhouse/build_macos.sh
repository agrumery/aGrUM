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

echo "Initializing Conda..."
. /Users/agrum/miniconda3/etc/profile.d/conda.sh

echo "Activating Conda environment: ${CONDA_ENV}"
conda activate $CONDA_ENV

echo "Cleaning previous builds..."
python act clean

if [ "${TARGET}" == "aGrUM" ]; then
    echo "Building aGrUM..."
    python act test release aGrUM -j halfexcept1 -t all -m all --compiler=clang 
elif [ "${TARGET}" == "pyAgrum" ]; then
    echo "Building pyagrum..."
    python act install release pyAgrum -j halfexcept1 -t all -m quick --compiler=clang -d build
    python act test
else
    echo "Invalid target type: ${TARGET}. Expected 'aGrUM' or 'pyAgrum'"
    exit 1
fi

echo "Deactivating Conda environment..."
conda deactivate

echo "Script execution completed."
