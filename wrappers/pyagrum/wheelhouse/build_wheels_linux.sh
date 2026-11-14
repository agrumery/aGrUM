#!/bin/bash

# Need one parameter:
#   - python dir (e.g., cp311-cp311)

set -e -x

if [ "$#" -lt 1 ]; then
    echo "Expected at least one parameter: python dir (e.g., cp311-cp311). Additional options are optional."
    exit 1
fi

PYDIR=$1
shift
ADDITIONAL_OPTIONS="$@"

echo "Creating wheels directory..."
mkdir -p ${CI_PROJECT_DIR}/wheels

echo "Updating version with build number..."
/opt/python/${PYDIR}/bin/python ${CI_PROJECT_DIR}/wrappers/pyagrum/wheelhouse/scripts/update_version.py ${CI_PROJECT_DIR} 0

echo "Installing numpy because of new dependency"
/opt/python/${PYDIR}/bin/pip install numpy

echo "Building pyAgrum wheel..."
/opt/python/${PYDIR}/bin/python act release wheel pyAgrum -j half -d ${CI_PROJECT_DIR}/wheels ${ADDITIONAL_OPTIONS}

echo "Script execution completed."
