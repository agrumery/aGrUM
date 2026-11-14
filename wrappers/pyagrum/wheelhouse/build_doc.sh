#!/bin/bash

# Fail on the first error: without it, only the exit code of the last command
# (make html) counted, and sphinx does not run with -W, so a failed
# `act install` or `pip install` could still end in a green job.
# (No -u: the virtualenv activate script is not guaranteed nounset-safe.)
set -e -x -o pipefail

apt-get update -qq
apt-get install -y -qq pandoc

root=$PWD

virtualenv py311
source py311/bin/activate

python act clean

echo "Install pyAgrum requirements..."
pip install -r wrappers/pyagrum/testunits/requirements.txt
pip install -r wrappers/pyagrum/doc/sphinx/requirements.txt
pip uninstall -y pyagrum

echo "Building pyAgrum doc..."
python act install release pyAgrum -j halfexcept1 -d $root/py311
python act doc pyAgrum
cd wrappers/pyagrum/doc/sphinx
make html

echo "Script execution completed."
