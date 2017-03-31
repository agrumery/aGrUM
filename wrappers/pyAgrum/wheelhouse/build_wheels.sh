#!/bin/bash
set -e -x

# Setting up paths to where everything is/will be
AGRUM_DIR=/opt/aGrUM
PYAGRUM_DIR=/opt/aGrUM/wrappers/pyAgrum
WHEELHOUSE_DIR=/opt/wheelhouse

# Downloading aGrUM and initializing the wheelhouse dir
git clone https://gitlab.com/agrumery/aGrUM.git ${AGRUM_DIR}
mkdir -p ${WHEELHOUSE_DIR}/pyAgrum
cp -R ${PYAGRUM_DIR} ${WHEELHOUSE_DIR}/pyAgrum/
cp ${AGRUM_DIR}/wrappers/swig/* ${WHEELHOUSE_DIR}/pyAgrum/pyAgrum

# Extracting pyAgrum version from VERSION.txt
MAJOR=$(cat ${AGRUM_DIR}/VERSION.txt | grep AGRUM_VERSION_MAJOR | sed 's/set(AGRUM_VERSION_MAJOR "\(.*\)")/\1/')
MINOR=$(cat ${AGRUM_DIR}/VERSION.txt | grep AGRUM_VERSION_MINOR | sed 's/set(AGRUM_VERSION_MINOR "\(.*\)")/\1/')
PATCH=$(cat ${AGRUM_DIR}/VERSION.txt | grep 'set(AGRUM_VERSION_PATCH' | sed 's/set(AGRUM_VERSION_PATCH "\([0-9]*\.[0-9]*\)")/\1/')

PYAGRUM_VERSION="${MAJOR}.${MINOR}.${PATCH}"

# Doing cmake work for the moment
sed 's/@PYAGRUM_VERSION@/'${PYAGRUM_VERSION}'/' \
  < ${WHEELHOUSE_DIR}/pyAgrum/pyAgrum/cmake/setup.in.py \
  > ${WHEELHOUSE_DIR}/pyAgrum/setup.py

sed 's/@PYAGRUM_VERSION@/'${PYAGRUM_VERSION}'/' \
  < ${WHEELHOUSE_DIR}/pyAgrum/pyAgrum/cmake/functions.in.py \
  > ${WHEELHOUSE_DIR}/pyAgrum/pyAgrum/functions.py

sed 's/@PYAGRUM_VERSION@/'${PYAGRUM_VERSION}'/' \
  < ${WHEELHOUSE_DIR}/pyAgrum/pyAgrum/cmake/__init__.in.py \
  > ${WHEELHOUSE_DIR}/pyAgrum/pyAgrum/__init__.py

cp ${PYAGRUM_DIR}/wheelhouse/files/* ${WHEELHOUSE_DIR}/pyAgrum/

# Removing unecessary files from the wheelhouse
rm -rf ${WHEELHOUSE_DIR}/pyAgrum/pyAgrum/cmake
rm -rf ${WHEELHOUSE_DIR}/pyAgrum/pyAgrum/generated-files*
rm -rf ${WHEELHOUSE_DIR}/pyAgrum/pyAgrum/wheelhouse
rm -rf ${WHEELHOUSE_DIR}/pyAgrum/pyAgrum/sphinx
rm -rf ${WHEELHOUSE_DIR}/pyAgrum/pyAgrum/testunits

# Compile and install aGrUM
cd ${AGRUM_DIR}
/opt/python/cp36-cp36m/bin/python act install release aGrUM --withoutSQL --no-fun
cd -

# Compile wheels
for PYBIN in /opt/python/*/bin
do
  "${PYBIN}/pip" wheel ${WHEELHOUSE_DIR}/pyAgrum -w ${WHEELHOUSE_DIR}
done

# Bundle external shared libraries into the wheels
for whl in ${WHEELHOUSE_DIR}/*.whl
do
  auditwheel repair "$whl" -w ${WHEELHOUSE_DIR}
done

# Moves wheels in shared Volume if exists
mkdir -p /aGrUM/wheels
cp ${WHEELHOUSE_DIR}/*.whl /aGrUM/wheels
cd /aGrUM/wheels
ls -1 | grep -v "manylinux1" | xargs rm -f
ls -1 | grep "numpy" | xargs rm -f
cd -
