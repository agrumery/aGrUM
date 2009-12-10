project(AGRUM)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_CURRENT_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

## allow gcc compilation on Windows (jw)
if(WIN32)
  set(WIN32_STYLE_FLAGS 1)
  set(CMAKE_INSTALL_PREFIX "")
endif(WIN32)
if(MINGW)
  set(WIN32_STYLE_FLAGS 0)
  set(UNIX_STYLE_FLAGS  1)
  set(CMAKE_INSTALL_PREFIX /usr)
endif(MINGW)

if(UNIX)
  set(WIN32_STYLE_FLAGS 0)
  set(UNIX_STYLE_FLAGS  1)
  set(CMAKE_INSTALL_PREFIX /usr)
endif(UNIX)
if(APPLE)
  #set(CMAKE_CXX_COMPILER /opt/local/bin/g++-mp-4.3)
  set(CMAKE_INSTALL_PREFIX /usr)
endif(APPLE)

set(AGRUM_VERSION_MAJOR "0")
set(AGRUM_VERSION_MINOR "6")
set(AGRUM_VERSION_PATCH "0")
set(AGRUM_VERSION "${AGRUM_VERSION_MAJOR}.${AGRUM_VERSION_MINOR}.${AGRUM_VERSION_PATCH}")

#new configuration (which will be the default one)
#set(CMAKE_BUILD_TYPE current)

set(CMAKE_CXX_FLAGS_DEBUG "-Wall -pedantic -O0 -pg -ggdb -DGUM_NO_INLINE")
set(CMAKE_C_FLAGS_DEBUG "-Wall  -pedantic -O0 -pg -ggdb -DGUM_NO_INLINE")

if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  # to detect Intel 64 bits platform : inline tends to be a bug for 64bits (???)
  if(APPLE)
    set(CMAKE_CXX_FLAGS_CURRENT "-O2 -DNDEBUG -Wall -pedantic")
    set(CMAKE_C_FLAGS_CURRENT "-O2 -DNDEBUG -Wall -pedantic")

    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-Wall -O3 -DNDEBUG")
  else (APPLE)
    set(CMAKE_CXX_FLAGS_CURRENT "-O2 -DNDEBUG -Wall -pedantic -fno-inline-small-functions")
    set(CMAKE_C_FLAGS_CURRENT "-O2 -DNDEBUG -Wall -pedantic -fno-inline-small-functions")

    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3 -DNDEBUG -fno-inline-small-functions")
    set(CMAKE_C_FLAGS_RELEASE "-Wall -O3 -DNDEBUG -fno-inline-small-functions")
  endif(APPLE)

  set(AGRUM_PROCESSOR "64")
else("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  if(APPLE)
    set(CMAKE_CXX_FLAGS_CURRENT "-O2 -DNDEBUG -Wall -pedantic")
    set(CMAKE_C_FLAGS_CURRENT "-O2 -DNDEBUG -Wall -pedantic")

    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-Wall -O3 -DNDEBUG")
  else (APPLE)
    set(CMAKE_CXX_FLAGS_CURRENT "-O2 -DNDEBUG -Wall -pedantic -fno-inline-small-functions")
    set(CMAKE_C_FLAGS_CURRENT "-O2 -DNDEBUG -Wall -pedantic -fno-inline-small-functions")

    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3 -DNDEBUG -pedantic -fno-inline-small-functions")
    set(CMAKE_C_FLAGS_RELEASE "-Wall -O3 -DNDEBUG -pedantic -fno-inline-small-functions")
  endif(APPLE)

  set(AGRUM_PROCESSOR "32")
endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

if (${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
  set(LIBAGRUM "agrum-dbg")
else (${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
  set(LIBAGRUM "agrum")
endif (${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")

subdirs(agrum EXCLUDE_FROM_ALL run testunits)
include_directories(${AGRUM_SOURCE_DIR})
if(WIN32)
  set(LIBRARY_OUTPUT_PATH ${AGRUM_BINARY_DIR})
endif(WIN32)
if(UNIX)
  set(LIBRARY_OUTPUT_PATH ${AGRUM_BINARY_DIR}/lib)
endif(UNIX)
if(APPLE)
  set(LIBRARY_OUTPUT_PATH ${AGRUM_BINARY_DIR}/lib)
endif(APPLE)

## checking some dependecies
find_program(FLEX_EXECUTABLE flex)
if(FLEX_EXECUTABLE STREQUAL "FLEX_EXECUTABLE-NOTFOUND")
    message(FATAL_ERROR "Please install flex!")
endif(FLEX_EXECUTABLE STREQUAL "FLEX_EXECUTABLE-NOTFOUND")

find_program(BISON_EXECUTABLE bison)
if(BISON_EXECUTABLE STREQUAL "BISON_EXECUTABLE-NOTFOUND")
    message(FATAL_ERROR "Please install bison!")
endif(BISON_EXECUTABLE STREQUAL "BISON_EXECUTABLE-NOTFOUND")

include("cmake/FlexBison.cmake")

## uninstall rule
configure_file(
  "${CMAKE_CURRENT_CMAKE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE
  @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

## for creation of cmake package AGRUM
get_filename_component(AGRUM_INSTALL_LIBRARY AGRUM_LIBRARY NAME)
set(AGRUM_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${AGRUM_INSTALL_LIBRARY})
set(AGRUM_INCLUDE ${CMAKE_INSTALL_PREFIX}/include)
configure_file(
  "${CMAKE_CURRENT_CMAKE_DIR}/aGrUMConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/aGrUMConfig.cmake"
)
include(CMakeExportBuildSettings)

## installation module for recognize agrum (see documentation "How to use agrum")
cmake_export_build_settings(${CMAKE_CURRENT_BINARY_DIR}/aGrUMBuildSettings.cmake)
export_library_dependencies(${CMAKE_CURRENT_BINARY_DIR}/aGrUMLibraryDepends.cmake)
install(FILES
    ${CMAKE_CURRENT_CMAKE_DIR}/aGrUMUse.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/aGrUMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/aGrUMBuildSettings.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/aGrUMLibraryDepends.cmake
    DESTINATION lib/aGrUM
    )
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/agrum.pc
  DESTINATION lib/pkgconfig
)

set(CXX_FLAGS "${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")
configure_file(
  "${CMAKE_CURRENT_CMAKE_DIR}/agrum.pc.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/agrum.pc"
)

## pack module ##############################################################################
include("cmake/CPackAgrum.cmake")
include(CPack)
