OPTION_VERBOSE =
#OPTION_VERBOSE = "-DCMAKE_VERBOSE_MAKEFILE=ON"
OPTION_NBR_PROCESSOR =

# which version will be used for "install" and "pack" rules ?
PUBLIC_VERSION= "release"
default: release

current: ready
	cmake -E chdir build/current make -j $(OPTION_NBR_PROCESSOR) 

run: current
	cmake -E chdir build/current make -j $(OPTION_NBR_PROCESSOR) agrumDemo
	cmake -E time build/current/run/agrumDemo

test: current
	cmake -E chdir build/current make -j $(OPTION_NBR_PROCESSOR) test
	cmake -E time cmake -E chdir build/current/testunits ./test


#############################################################################
windows: ready_for_windows
	cmake -E time cmake -E chdir build/windows make -j $(OPTION_NBR_PROCESSOR)

#############################################################################
release: ready
	cmake -E time cmake -E chdir build/release make -j $(OPTION_NBR_PROCESSOR) 

runrelease: release
	cmake -E chdir build/release make -j $(OPTION_NBR_PROCESSOR) agrumDemo
	cmake -E time build/release/run/agrumDemo

testrelease: release
	cmake -E chdir build/release make -j $(OPTION_NBR_PROCESSOR) test
	cmake -E time cmake -E chdir build/release/testunits ./test

#############################################################################
debug: ready
	cmake -E time cmake -E chdir build/debug make -j $(OPTION_NBR_PROCESSOR)

rundebug: debug
	cmake -E chdir build/debug make -j $(OPTION_NBR_PROCESSOR) agrumDemo
	cmake -E time build/debug/run/agrumDemo

testdebug: debug
	cmake -E chdir build/debug make -j $(OPTION_NBR_PROCESSOR) test
	cmake -E time cmake -E chdir build/debug/testunits ./test

#############################################################################
all: current debug release doc

everything: all
	cmake -E chdir build/current make -j  $(OPTION_NBR_PROCESSOR) agrumDemo
	cmake -E chdir build/release make -j  $(OPTION_NBR_PROCESSOR) agrumDemo
	cmake -E chdir build/debug make -j  $(OPTION_NBR_PROCESSOR) agrumDemo
	cmake -E chdir build/current make -j  $(OPTION_NBR_PROCESSOR) test
	cmake -E chdir build/release make -j  $(OPTION_NBR_PROCESSOR) test
	cmake -E chdir build/debug make -j  $(OPTION_NBR_PROCESSOR) test

doc:
	cmake -E time doxygen
############################################################################
install:
	make ${PUBLIC_VERSION}
	cmake -E chdir build/${PUBLIC_VERSION} make install

uninstall:
	make ${PUBLIC_VERSION}
	cmake -E chdir build/${PUBLIC_VERSION} make uninstall

pack: install
	cmake -E chdir build/${PUBLIC_VERSION} make package

rpm: pack
	cmake -E chdir build/${PUBLIC_VERSION} cpack -D CPACK_RPM_PACKAGE_DEBUG=1 -D CPACK_RPM_SPEC_INSTALL_POST="/bin/true" -G RPM

deb: pack
	cmake -E chdir build/${PUBLIC_VERSION} cpack -D CPACK_RPM_PACKAGE_DEBUG=1 -D CPACK_DEB_SPEC_INSTALL_POST="/bin/true" -G DEB
#############################################################################
clean_build:
	cmake -E remove_directory build

clean_doc:
	cmake -E remove_directory docs/latex
	cmake -E remove_directory docs/html
	cmake -E remove -f docs/agrum.tag docs/warning.txt

clean: clean_build clean_doc

### DIRECTORIES and cmake ##################################################
build:
	cmake -E make_directory build
	cmake -E make_directory build/current
	cmake -E make_directory build/release
	cmake -E make_directory build/debug
	cmake -E make_directory build/windows

ready: build src/CMakeLists.txt src/agrum/CMakeLists.txt src/run/CMakeLists.txt src/testunits/CMakeLists.txt
	cmake -E chdir build/current cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=CURRENT ../../src ${OPTION_VERBOSE}
	cmake -E chdir build/release cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=RELEASE ../../src ${OPTION_VERBOSE}
	cmake -E chdir build/debug cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=DEBUG ../../src ${OPTION_VERBOSE}

ready_for_windows: build src/CMakeLists.txt src/agrum/CMakeLists.txt src/run/CMakeLists.txt src/testunits/CMakeLists.txt
  cmake -E chdir build/windows cmake -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../../src/cmake/Toolchain-mingw32.cmake -DCMAKE_INSTALL_PREFIX==. -DCMAKE_BUILD_TYPE=RELEASE ../../src ${OPTION_VERBOSE}
