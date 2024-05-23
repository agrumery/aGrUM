#!/bin/bash

apt-get install -y pandoc

root=$PWD

virtualenv py39
source py39/bin/activate

python act clean

echo "Install pyAgrum requirements..."
pip install -r wrappers/pyAgrum/testunits/requirements.txt
pip install -r wrappers/pyAgrum/doc/sphinx/requirements.txt
pip uninstall -y pyAgrum

echo "Building pyAgrum doc..."
python act install release pyAgrum -j halfexcept1 -d $root/py39
python act doc pyAgrum
cd wrappers/pyAgrum/doc/sphinx
make html

echo "Script execution completed."
