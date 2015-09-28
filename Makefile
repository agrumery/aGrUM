#since some IDEs need a Makefile
#here is a simple wrapper to act embedded in Makefile
#
ACT=./act --no-fun

JOBS=2

library:
	$(ACT) lib release aGrUM -j $(JOBS)

librarydebug:
	$(ACT) lib debug aGrUM -j $(JOBS)

install:
	$(ACT) install release aGrUM -j $(JOBS)

installdebug:
	$(ACT) install debug aGrUM -j $(JOBS)

test:
	$(ACT) test release -j $(JOBS)

testdebug:
	$(ACT) test debug -j $(JOBS)

testall:
	$(ACT) test release -t all -m all -j $(JOBS)

testalldebug:
	$(ACT) test debug -t all -m all -j $(JOBS)

run:
	$(ACT) run release -j $(JOBS)

rundebug:
	$(ACT) run debug -j $(JOBS)

pyAgrum:
	$(ACT) wrapper pyAgrum -j $(JOBS)

clean:
	$(ACT) clean

last:
	$(ACT)

doc:
	$(ACT) doc release aGrUM 
	cat build/src/warning.txt
doc:
	$(ACT) doc debug aGrUM 
	cat build/src/warning.txt

clang-format:
	$(ACT) autoindent

