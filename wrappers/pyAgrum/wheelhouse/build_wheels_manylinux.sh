#!/bin/bash

# Need one parameter:
#   - python dir (e.g., cp39-cp39)

set -e -x

if [ "$#" -ne 1 ]; then
    echo "Expected one parameter: python dir (e.g., cp39-cp39)."
    return 1
fi

BUILD_NUMBER=0

python ${CI_PROJECT_DIR}/wrappers/pyAgrum/wheelhouse/scripts/update_version.py ${CI_PROJECT_DIR} $BUILD_NUMBER

PYDIR=$1

/opt/python/${PYDIR}/bin/python act clean
/opt/python/${PYDIR}/bin/python act release nightly_wheel pyAgrum -j 7