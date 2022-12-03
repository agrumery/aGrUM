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

#OLDPATH=$PATH
#export PATH=/opt/python/${PYDIR}/bin:${PATH}

#PYINCLUDEDIR=$(ls -1 /opt/python/${PYDIR}/include)

#virtualenv venv  -p /opt/python/${PYDIR}/bin/python
#source venv/bin/activate
echo "*************************"
/opt/python/${PYDIR}/bin/python -c 'from distutils import sysconfig; print(sysconfig.get_python_inc())'
echo "*************************"
/opt/python/${PYDIR}/bin/python act clean
#python act release nightly_wheel pyAgrum -j 7 --python3target=/opt/python/${PYDIR}/bin/python
/opt/python/${PYDIR}/bin/python act install release pyAgrum -j 7 --dry-run
/opt/python/${PYDIR}/bin/python act install release pyAgrum -j 7
