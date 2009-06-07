FLEX=flex
BISON=bison
OUTPUTS=BIFScanner.cpp BIFParser.cpp

.PHONY: all clean

all: BIFParser.cpp

BIFParser.cpp: BIFScanner.cpp BIFParser.ypp
	${BISON} -v -y BIFParser.ypp -o BIFParser.cpp

BIFScanner.cpp: BIFScanner.lpp
	${FLEX} -o BIFScanner.cpp BIFScanner.lpp #&& \
	#sed -e 's/isatty (int )/isatty (int ) throw()/' BIFScanner.cpp > BIFScanner.cpp.tmp && \
	#mv -f BIFScanner.cpp.tmp BIFScanner.cpp

clean:
	rm -f BIFScanner.cpp *.hh \
				BIFParser.hpp BIFParser.cpp BIFParser.tab.* BIFParser.output

