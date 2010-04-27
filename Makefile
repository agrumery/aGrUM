#since some IDEs need a Makefile
#here is a simple wrapper to act embedded in Makefile
ACT=./act
JOBS=4

MY_TEST=Skool

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

mytest:
	$(ACT) test release -t $(MY_TEST) -p linux -j 4

mytestdebug:
	$(ACT) test debug -t $(MY_TEST) -p linux -j 4


clean:
	$(ACT) clean
