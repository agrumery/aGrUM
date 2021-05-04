# please check that pyAgrum has been compiled in that direction
# for instance using
# python act lib pyAgrum release
SRCDIR=../../..
DESTDIR=${SRCDIR}/build/release

#swig -c++ -python -outdir $(DESTDIR) -py3 -I../../swig -I../../../src -I../../../build/release -I.. pyAgrum.i
# please run this command if this Makefile fails : `act lib pyAgrum --python3`
run:
	cd ${DESTDIR} && make pyAgrum_swig_compilation
	cp ${SRCDIR}/wrappers/pyAgrum/generated-files3/pyAgrum.py ${DESTDIR}/wrappers/pyAgrum/.
	cd sphinx && make html
