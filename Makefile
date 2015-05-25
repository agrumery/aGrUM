#since some IDEs need a Makefile
#here is a simple wrapper to act embedded in Makefile
#
CLANGFORMAT=clang-format-3.5

ACT=./act --no-fun 
JOBS=7

library:
	$(ACT) agrum release -p linux -j $(JOBS)

install:
	$(ACT) release install -p linux -j $(JOBS)

debug:
	$(ACT) agrum debug -p linux -j $(JOBS)

test:
	$(ACT) test release -p linux -j $(JOBS)

testdebug:
	$(ACT) test debug  -p linux -j $(JOBS)

testall:
	$(ACT) test release -t all -p linux -j $(JOBS)

testalldebug:
	$(ACT) test debug -t all -p linux -j $(JOBS)

run:
	$(ACT) run release -p linux -j $(JOBS)

rundebug:
	$(ACT) run debug -p linux -j $(JOBS)

windows:
	$(ACT) agrum release -p windows -j $(JOBS)

pyAgrum:
	$(ACT) wrapper pyAgrum -p linux -j $(JOBS)


clean:
	$(ACT) clean

last:
	$(ACT)

linuxreleasedoc:
	$(ACT) release doc -p linux
	cat build/linux/release/warning.txt

clang-format:
	for i in `find src -name "*.h"` ; do $(CLANGFORMAT) -i $$i; done
	for i in `find src -name "*.cpp"` ; do $(CLANGFORMAT) -i $$i; done
	for i in `find src -name "*.tcc"` ; do $(CLANGFORMAT) -i $$i; done
	for i in `find src -name "*.inl"` ; do $(CLANGFORMAT) -i $$i; done

