# please check that pyAgrum has been compiled in that direction
# for instance using
# python act lib pyAgrum release
SRCDIR=../../..
DESTDIR=${SRCDIR}/build/pyAgrum/release

#swig -c++ -python -outdir $(DESTDIR) -py3 -I../../swig -I../../../src -I../../../build/pyAgrum/release -I.. pyagrum.i
# please run this command if this Makefile fails : `act lib pyAgrum --python3`
run:
	cd sphinx && make html
