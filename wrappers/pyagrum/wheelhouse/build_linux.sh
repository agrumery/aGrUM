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
    /opt/python/${PYDIR}/bin/python act install release pyAgrum -j halfexcept1 -m all -t all -d build

    # TEMPORARY DIAGNOSTIC (2026-09-07): audits every leaf module import
    # independently instead of relying on the test harness, which aborts
    # entirely on the first ImportError and so only ever reveals one broken
    # GUM_PUBLIC/PYGUM_SHARED_PUBLIC export per CI run (see
    # wheelhouse/scripts/diagnose_leaf_imports.py). `|| true` so a failure
    # here does not mask act test's own pass/fail signal below. Remove this
    # call (and the script) once the audit is complete.
    /opt/python/${PYDIR}/bin/python wrappers/pyagrum/wheelhouse/scripts/diagnose_leaf_imports.py || true

    /opt/python/${PYDIR}/bin/python act test 
else
    echo "Invalid target type: ${TARGET}. Expected 'aGrUM' or 'pyAgrum'"
    exit 1
fi

echo "Script execution completed."
