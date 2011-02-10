# Makefile

# Cette commande fonctionne sur unix, et renvoie le nom de le l'OS,
# Mais elle ne fonctione pas sous windows, donc elle renvoie une chaine vide.
OS := $(shell uname)
ifeq ($(OS),Linux)
	MAKE=make
	CMAKE=cmake
else
	MAKE=mingw32-make
	CMAKE="C:\Program Files\CMake 2.8\bin\cmake.exe"
	UNIX= -G "MinGW Makefiles"
endif

default: release

before: agrum
# On teste si le répertoire n'existe pas déjà.
# Permet d'éviter les messages d'erreurs sous windows particulièrement.
ifeq ("$(wildcard build)", "")
#	mkdir build;
#	mkdir build/Debug;
#	mkdir build/Release;
#	mkdir build/MinSizeRel;
	$(CMAKE) -E make_directory build/Debug
	$(CMAKE) -E make_directory build/Release
	$(CMAKE) -E make_directory build/MinSizeRel
endif

debug: before
	(cd build/Debug && $(CMAKE) $(UNIX) ../../src -DCMAKE_BUILD_TYPE=Debug && $(MAKE) -j 4)

release: before
	(cd build/Release && $(CMAKE) $(UNIX) ../../src -DCMAKE_BUILD_TYPE=Release && $(MAKE) -j 4)

minrelease: before
	(cd build/MinSizeRel && $(CMAKE) $(UNIX) ../../src -DCMAKE_BUILD_TYPE=MinSizeRel && $(MAKE) -j 4)

clean:
ifeq ($(OS),Linux)
	rm -rf build/
	rm -rf bin/skoobeditor
	rm -rf usr/
else
#	rmdir /s /q build
#	del bin\SkoobEditor.exe
	$(CMAKE) -E remove_directory build
	$(CMAKE) -E remove bin/SkoobEditor.exe
endif

installer: release
	"C:\Program Files\NSIS\makensis.exe" installer_script.nsi

dpkg: release
	(mkdir -p usr/bin)
	(mkdir -p usr/share/doc)
	(cp CHANGELOG.txt usr/share/doc/)
	(cp LICENSE.txt usr/share/doc/)
	(cp README.txt usr/share/doc/)
	(cp bin/Release/skoobeditor usr/bin/)
	(cd .. && dpkg-deb --build skoobeditor)

## Compile aGrUM. Ne fonctionne pas encore. voir CMAKE_SYSTEM_NAME
agrum:
ifeq ($(OS),Linux)
	(cd ../../ && $(CMAKE) -E make_directory build/linux/Release)
	(cd ../../build/linux/release && $(CMAKE) $(UNIX) ../../../src -DCMAKE_BUILD_TYPE=Release && $(MAKE) -j 4)
else
	(cd ../../ && $(CMAKE) -E make_directory build/windows/Release)
	(cd ../../build/windows/release && $(CMAKE) $(UNIX) ../../../src -DCMAKE_BUILD_TYPE=Release && $(MAKE) -j 4)
endif
	
