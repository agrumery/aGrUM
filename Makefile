#since some IDEs need a Makefile
#here is a simple wrapper to act embedded in Makefile
ACT=./act
JOBS=7

SKOOL_TEST=GSpan

library:
	$(ACT) agrum release -p linux -j $(JOBS)

install:
	$(ACT) release install -p linux -j $(JOBS)

debug:
	$(ACT) agrum debug -p linux -j $(JOBS)

test:
	$(ACT) test release -t all -p linux -j $(JOBS)

testdebug:
	$(ACT) test debug -t all -p linux -j $(JOBS)

run:
	$(ACT) run release -p linux -j $(JOBS)

rundebug:
	$(ACT) run debug -p linux -j $(JOBS)

windows:
	$(ACT) agrum release -p windows -j $(JOBS)

pyAgrum:
	$(ACT) wrapper pyAgrum -p linux -j $(JOBS)

skooltest:
	$(ACT) test release -t $(SKOOL_TEST) -p linux -j 4

skooltestdebug:
	$(ACT) test debug -t $(SKOOL_TEST) -p linux -j 4


clean:
	$(ACT) clean

last:
	$(ACT)

cppcheck:
	cppcheck src --force --enable=all 2>cppcheck.txt
	wc -l cppcheck.txt
