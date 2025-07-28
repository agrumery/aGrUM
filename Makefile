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

pyAgrum:
	$(ACT) install release pyAgrum 

testpyAgrum:
	$(ACT) install test pyAgrum 

clean:
	$(ACT) clean

doc:
	$(ACT) doc release aGrUM
	cat build/aGrUM/release/warning.txt

docdebug:
	$(ACT) doc debug aGrUM
	cat build/aGrUM/debug/warning.txt


gource:
	#gource --key  -s 0.05  -1200x800 --highlight-users --dir-colour AAAAAA --hide filenames,mouse -o - | ffmpeg -y -r 20 -f image2pipe -vcodec ppm -i - -vcodec libvpx -b 20000K agrum.webm
	#gource --key -s 0.05 -1400x800 --logo ./src/docs/mini_agrum2.png --title "aGrUM Dev" --auto-skip-seconds 1 --multi-sampling  --highlight-users --dir-colour AAAAAA --max-files 0 --hide mouse,filenames --stop-at-end --output-framerate 25 --background-colour 000000 --colour-images --font-size 15 -o - | ffmpeg -y -r 25 -f image2pipe -vcodec ppm -i - -vcodec libvpx -b:20000 agrum.webm
	gource --key -s 0.05 -1400x800 --logo ./src/docs/mini_agrum2.png --title "aGrUM Dev" --auto-skip-seconds 1 --multi-sampling  --highlight-users --dir-colour AAAAAA --max-files 0 --hide mouse,filenames --stop-at-end --output-framerate 25 --background-colour 000000 --colour-images --font-size 15 -o - | ffmpeg -y -threads 0  -r 40 -f image2pipe -vcodec ppm -vcodec libvpx -vb 1000k -minrate 100k -maxrate 30000k -bufsize 1000K -qmin 3 -qmax 30 -rc_lookahead 16 -keyint_min 0 -g 360  -skip_threshold 0 -level 116 -preset veryslow -ab 192k agrum.webm
