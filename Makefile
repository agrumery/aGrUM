#since some IDEs need a Makefile
#here is a simple wrapper to act embedded in Makefile

library:
	act agrum release -p linux -j 4

debug:
	act agrum debug -p linux -j 4

test:
	act test release -t all -p linux -j 4

testdebug:
	act test debug -t all -p linux -j 4

run:
	act run release -p linux -j 4

rundebug:
	act run debug -p linux -j 4

windows:
	act agrum release -p windows -j 4

pyAgrum:
	act wrapper pyAgrum -p linux -j 4
