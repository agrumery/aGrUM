# Makefile
# Define LowerCase function
lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

# Cette commande fonctionne sur unix, et renvoie le nom de le l'OS,
# Mais elle ne fonctione pas sous windows, donc elle renvoie une chaine vide.
OS = $(shell uname)
ifeq ($(OS),Linux)
	MAKE=make
	CMAKE=cmake
else
	MAKE=mingw32-make
	CMAKE="C:\Program Files\CMake 2.8\bin\cmake.exe"
	UNIX= -G "MinGW Makefiles"
	OS=Windows
endif

default: release

################################################################################

before: agrum
	$(CMAKE) -E remove -f ../../build/linux/release/CMakeFiles/test.dir/test.cpp.o
	$(CMAKE) -E make_directory build/Debug
	$(CMAKE) -E make_directory build/Release
	$(CMAKE) -E make_directory build/MinSizeRel

agrum:
#ifeq ($(OS),Linux)
	(cd ../../ && $(CMAKE) -E make_directory build/$(call lc,$(OS))/release)
	(cd ../../build/$(call lc,$(OS))/release && $(CMAKE) $(UNIX) ../../../src -DCMAKE_BUILD_TYPE=Release && $(MAKE) -j 4)
#else
#	(cd ../../ && $(CMAKE) -E make_directory build/windows/release)
#	(cd ../../build/windows/release && $(CMAKE) $(UNIX) ../../../src -DCMAKE_BUILD_TYPE=Release && $(MAKE) -j 4)
#endif
################################################################################

before_debug: agrum_debug
	$(CMAKE) -E remove -f ../../build/linux/release/CMakeFiles/test.dir/test.cpp.o
	$(CMAKE) -E make_directory build/Debug
	$(CMAKE) -E make_directory build/Release
	$(CMAKE) -E make_directory build/MinSizeRel

agrum_debug:
#ifeq ($(OS),Linux)
	(cd ../../ && $(CMAKE) -E make_directory build/$(call lc,$(OS))/debug)
	(cd ../../build/$(call lc,$(OS))/debug && $(CMAKE) $(UNIX) ../../../src -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_VERBOSE_MAKEFILE=1 && $(MAKE) -j 4)
#else
#	(cd ../../ && $(CMAKE) -E make_directory build/windows/release)
#	(cd ../../build/windows/release && $(CMAKE) $(UNIX) ../../../src -DCMAKE_BUILD_TYPE=Release && $(MAKE) -j 4)
#endif

################################################################################

debug: before_debug
	(cd build/Debug && $(CMAKE) $(UNIX) ../../src -DCMAKE_BUILD_TYPE=Debug && $(MAKE) -j 4)

release: before
	(cd build/Release && $(CMAKE) $(UNIX) ../../src -DCMAKE_BUILD_TYPE=Release && $(MAKE) -j 4)

minrelease: before
	(cd build/MinSizeRel && $(CMAKE) $(UNIX) ../../src -DCMAKE_BUILD_TYPE=MinSizeRel && $(MAKE) -j 4)

################################################################################

clean:
	$(CMAKE) -E remove_directory bin
	$(CMAKE) -E remove_directory build
	$(CMAKE) -E remove_directory usr

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


