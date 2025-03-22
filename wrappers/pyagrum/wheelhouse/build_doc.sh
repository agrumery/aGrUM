#!/bin/bash

apt-get install -y pandoc

root=$PWD

virtualenv py310
source py310/bin/activate

python act clean

echo "Install pyAgrum requirements..."
pip install -r wrappers/pyagrum/testunits/requirements.txt
pip install -r wrappers/pyagrum/doc/sphinx/requirements.txt
pip uninstall -y pyagrum

echo "Building pyAgrum doc..."
python act install release pyAgrum -j halfexcept1 -d $root/py310
python act doc pyAgrum
cd wrappers/pyagrum/doc/sphinx
make html

echo "Script execution completed."
