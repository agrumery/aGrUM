#!/bin/bash

# Need one parameter:
#   - python dir (e.g., cp39-cp39)

set -e -x

if [ "$#" -ne 1 ]; then
    echo "Expected one parameter: python dir (e.g., cp39-cp39)."
    return 1
fi

BUILD_NUMBER=0

CONDA_ENV=$1

python ${CI_PROJECT_DIR}/wrappers/pyAgrum/wheelhouse/scripts/update_version.py ${CI_PROJECT_DIR} ${BUILD_NUMBER}

. /Users/agrum/miniconda3/etc/profile.d/conda.sh
conda activate $CONDA_ENV
python act clean
python act release wheel pyAgrum -j halfexcept1 -d ${CI_PROJECT_DIR}/wheels
conda deactivate