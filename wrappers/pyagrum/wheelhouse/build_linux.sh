#!/bin/bash

# Requires two parameters:
#   - target type (aGrUM or pyAgrum)

set -e -x

if [ "$#" -ne 2 ]; then
    echo "Expected two parameters: target type (aGrUM or pyAgrum) and python dir (e.g., cp310-cp310)"
    exit 1
fi

TARGET=$1
PYDIR=$2
shift
ADDITIONAL_OPTIONS="$@"

echo "Cleaning previous builds..."
/opt/python/${PYDIR}/bin/python act clean

# Check the target type
if [ "${TARGET}" == "aGrUM" ]; then
    echo "Building aGrUM..."
    /opt/python/${PYDIR}/bin/python act test release aGrUM -j halfexcept1 -m all -t all
elif [ "${TARGET}" == "pyAgrum" ]; then
    echo "Building pyagrum..."
    /opt/python/${PYDIR}/bin/pip install -r wrappers/pyagrum/testunits/requirements.txt
    /opt/python/${PYDIR}/bin/python act install release pyAgrum -j halfexcept1 -m quick -t all -d build
    /opt/python/${PYDIR}/bin/python act test 
else
    echo "Invalid target type: ${TARGET}. Expected 'aGrUM' or 'pyAgrum'"
    exit 1
fi

echo "Script execution completed."
