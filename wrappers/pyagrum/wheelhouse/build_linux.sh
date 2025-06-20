#!/bin/bash

# Requires two parameters:
#   - target type (aGrUM or pyAgrum)

set -e -x

if [ "$#" -ne 1 ]; then
    echo "Expected one parameter: target type (aGrUM or pyAgrum)"
    exit 1
fi

TARGET=$1

echo "Cleaning previous builds..."
python3 act clean

# Check the target type
if [ "${TARGET}" == "aGrUM" ]; then
    echo "Building aGrUM..."
    python3 act --no-fun test release aGrUM -j halfexcept1 -t all -m all
elif [ "${TARGET}" == "pyAgrum" ]; then
    echo "Building pyagrum..."
    pip3 install -r wrappers/pyagrum/testunits/requirements.txt
    python3 act --no-fun test release pyAgrum -j halfexcept1 -m quick -t all
else
    echo "Invalid target type: ${TARGET}. Expected 'aGrUM' or 'pyAgrum'"
    exit 1
fi

echo "Script execution completed."
