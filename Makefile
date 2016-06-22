#since some IDEs need a Makefile
#here is a simple wrapper to act embedded in Makefile
#
ACT=./act --no-fun

last:
	$(ACT)
	
library:
	$(ACT) lib release aGrUM 

librarydebug:
	$(ACT) lib debug aGrUM 

install:
	$(ACT) install release aGrUM 

installdebug:
	$(ACT) install debug aGrUM 

test:
	$(ACT) test release aGrUM

testdebug:
	$(ACT) test debug aGrUM

testall:
	$(ACT) test release -t all -m all 

testalldebug:
	$(ACT) test debug -t all -m all 

run:
	$(ACT) run release 

rundebug:
	$(ACT) run debug 

pyAgrum:
	$(ACT) install release pyAgrum 

testpyAgrum:
	$(ACT) install test pyAgrum 

clean:
	$(ACT) clean

doc:
	$(ACT) doc release aGrUM
	cat build/release/warning.txt

docdebug:
	$(ACT) doc debug aGrUM
	cat build/debug/warning.txt

clang-format:
	$(ACT) autoindent

gource:
	#gource --key  -s 0.05  -1200x800 --highlight-users --dir-colour AAAAAA --hide filenames,mouse -o - | ffmpeg -y -r 20 -f image2pipe -vcodec ppm -i - -vcodec libvpx -b 20000K agrum.webm
	gource --key -s 0.05 -1400x800 --logo ./src/docs/mini_agrum2.png --title "aGrUM Dev" --auto-skip-seconds 1 --multi-sampling  --highlight-users --dir-colour AAAAAA --max-files 0 --hide mouse,filenames --stop-at-end --output-framerate 25 --background-colour 000000 --colour-images --font-size 15 -o - | ffmpeg -y -r 25 -f image2pipe -vcodec ppm -i - -vcodec libvpx -b:20000 agrum.webm
