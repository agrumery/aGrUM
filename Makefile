OPTION_VERBOSE =
#OPTION_VERBOSE = "-DCMAKE_VERBOSE_MAKEFILE=ON"
OPTION_NBR_PROCESSOR =

# which version will be used for "install" and "pack" rules ?
PUBLIC_VERSION= release
default: release

current: ready
	(cd build/current ; make -j $(OPTION_NBR_PROCESSOR))

run: current
	(cd build/current ; make -j $(OPTION_NBR_PROCESSOR)agrumDemo )
	cmake -E time build/current/run/agrumDemo

test: current
	(cd build/current ; make -j $(OPTION_NBR_PROCESSOR) test )
	(cd build/current/testunits ; ./test)


#############################################################################
windows: ready_for_windows
	(cd build/windows ; make -j $(OPTION_NBR_PROCESSOR))

testwindows: windows
	(cd build/windows ; make -j $(OPTION_NBR_PROCESSOR) test )

runwindows: windows
	(cd build/windows ; make -j $(OPTION_NBR_PROCESSOR) agrumDemo )
#############################################################################
release: ready
	(cd build/release ; make -j $(OPTION_NBR_PROCESSOR))

runrelease: release
	(cd build/release ; make -j $(OPTION_NBR_PROCESSOR) agrumDemo )
	( cd build/release/run ; ./agrumDemo)

testrelease: release
	(cd build/release ; make -j $(OPTION_NBR_PROCESSOR) test )
	(cd build/release/testunits ; ./test)

#############################################################################
debug: ready
	(cd build/debug ; make -j $(OPTION_NBR_PROCESSOR))

rundebug: debug
	(cd build/debug ; make -j $(OPTION_NBR_PROCESSOR) agrumDemo )
	(cd build/debug/run ; ./agrumDemo)

testdebug: debug
	(cd build/debug ; make -j $(OPTION_NBR_PROCESSOR) test )
	(cd build/debug/testunits ; ./test)

#############################################################################
all: current debug release doc

everything: all
	(cd build/current ; make -j  $(OPTION_NBR_PROCESSOR) agrumDemo )
	(cd build/release ; make -j  $(OPTION_NBR_PROCESSOR) agrumDemo )
	(cd build/debug ; make -j  $(OPTION_NBR_PROCESSOR) agrumDemo )
	(cd build/current ; make -j  $(OPTION_NBR_PROCESSOR) test )
	(cd build/release ; make -j  $(OPTION_NBR_PROCESSOR) test )
	(cd build/debug ; make -j  $(OPTION_NBR_PROCESSOR) test )

doc:
	cmake -E time doxygen
############################################################################
install:
	make ${PUBLIC_VERSION}
	(cd build/${PUBLIC_VERSION} ; make install)

install_debug:
	make debug
	(cd build/debug ; make install)

uninstall:
	(cd build/${PUBLIC_VERSION} ; make uninstall)

uninstall_debug:
	(cd build/debug ; make uninstall)

pack: ${PUBLIC_VERSION}
	(cd build/${PUBLIC_VERSION} ; make package)

pack_windows: windows
	(cd build/windows; make package)

rpm: pack
	(cd build/${PUBLIC_VERSION} ; cpack -D CPACK_RPM_PACKAGE_DEBUG=1 -D CPACK_RPM_SPEC_INSTALL_POST="/bin/true" -G RPM)

deb: pack
	(cd build/${PUBLIC_VERSION} ; cpack -D CPACK_RPM_PACKAGE_DEBUG=1 -D CPACK_DEB_SPEC_INSTALL_POST="/bin/true" -G DEB)

tgz: pack
	(cd build/${PUBLIC_VERSION} ; cpack -D CPACK_RPM_PACKAGE_DEBUG=1 -D CPACK_DEB_SPEC_INSTALL_POST="/bin/true" -G TGZ)

distrib: pack pack_windows

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
	(cd build/current ; cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=CURRENT ../../src ${OPTION_VERBOSE})
	(cd build/release ; cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=RELEASE ../../src ${OPTION_VERBOSE})
	(cd build/debug ; cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=DEBUG ../../src ${OPTION_VERBOSE})

ready_for_windows: build src/CMakeLists.txt src/agrum/CMakeLists.txt src/run/CMakeLists.txt src/testunits/CMakeLists.txt
	(cd build/windows ; cmake -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../../src/cmake/Toolchain-mingw32.cmake -DCMAKE_INSTALL_PREFIX=build/windows/_CPack_Packages/win32/TGZ/agrum-0.6.0-win32 -DCMAKE_BUILD_TYPE=RELEASE ../../src ${OPTION_VERBOSE})
