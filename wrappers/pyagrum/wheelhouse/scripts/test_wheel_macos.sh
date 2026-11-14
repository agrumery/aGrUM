#!/bin/bash

# Tests the wheel(s) of wheels/ in a throwaway venv, private to this job.
#
# The conda env given as parameter (py311, ..., py315) is only used as the base
# interpreter: nothing is installed in it anymore. Before, the wheel was
# pip-(un)installed directly in the shared conda env, so two concurrent jobs on
# the same Python version broke each other, a cancelled job left pyAgrum
# installed, and the test dependencies were frozen at the env's creation date.
# The dependencies now come fresh from requirements.txt, as on Linux (pip's
# per-user cache makes it fast after the first job).

set -e -x -o pipefail

echo "Starting script..."

# Ensure that exactly one parameter has been provided.
if [ "$#" -ne 1 ]; then
  echo "Error: Expected exactly one parameter for the conda environment."
  exit 1
fi

PYENV=$1
echo "Using environment (base interpreter): ${PYENV}"

echo "Activating environment ${PYENV}..."
source activate ${PYENV}

VENV="${TMPDIR:-/tmp}/pyagrum-test-${CI_JOB_ID}"
PY="${VENV}/bin/python"

# Same job id = a retried job: start from scratch. Removed on any exit.
rm -rf "${VENV}"
trap 'rm -rf "${VENV}"' EXIT

echo "Creating the job venv: ${VENV}"
python -m venv "${VENV}"

echo "Installing the test requirements..."
"${PY}" -m pip install --upgrade pip
"${PY}" -m pip install -r "${CI_PROJECT_DIR}/wrappers/pyagrum/testunits/requirements.txt"

echo "Installing new wheels..."
"${PY}" -m pip install "${CI_PROJECT_DIR}"/wheels/*.whl

echo "Running gumTest.py to validate installation..."
"${PY}" "${CI_PROJECT_DIR}/wrappers/pyagrum/testunits/gumTest.py"

echo "Script execution finished."
