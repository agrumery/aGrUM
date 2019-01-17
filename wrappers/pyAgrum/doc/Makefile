# please check that pyAgrum has been compiled in that direction
# for instance using
# python act lib pyAgrum release
DESTDIR=../../../build/release/wrappers/pyAgrum

run:
	#swig -c++ -python -outdir $(DESTDIR) -py3 -I../../swig -I../../../src -I../../../build/release -I.. pyAgrum.i
	cd sphinx && make html
