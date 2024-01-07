#!/bin/bash

# Need one parameter:
#   - python dir (e.g., cp39-cp39)

set -e -x

if [ "$#" -ne 1 ]; then
    echo "Expected one parameter: python dir (e.g., cp39-cp39)."
    return 1
fi

PYDIR=$1

echo "Creating wheels directory..."
mkdir -p ${CI_PROJECT_DIR}/wheels

echo "Updating version with build number..."
/opt/python/${PYDIR}/bin/python ${CI_PROJECT_DIR}/wrappers/pyAgrum/wheelhouse/scripts/update_version.py ${CI_PROJECT_DIR} 0

echo "Building pyAgrum wheel..."
/opt/python/${PYDIR}/bin/python act release nightly_wheel pyAgrum -j halfexcept1 -d ${CI_PROJECT_DIR}/wheels

echo "Script execution completed."
