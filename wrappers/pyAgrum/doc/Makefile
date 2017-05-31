DESTDIR=$(shell python -c "import os;import pyAgrum;print(os.path.dirname(pyAgrum.__file__))")

run:
	swig -c++ -python -outdir $(DESTDIR) -py3 -I../../swig -I../../../src -I../../../build/release -I.. pyAgrum.i
	cd sphinx && make html
