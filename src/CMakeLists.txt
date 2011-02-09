project(AGRUM)
cmake_minimum_required(VERSION 2.6)

include("version.txt")

set(CMAKE_CURRENT_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(AGRUM_OPTIMIZATION "-O3")
set(AGRUM_INLINING_POLICY "")#"-fno-inline-small-functions")
include("cmake/CompilOptions.agrum.cmake")

file(GLOB_RECURSE AGRUM_SOURCES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/*.cpp)
file(GLOB_RECURSE AGRUM_INCLUDES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/*.h)
file(GLOB_RECURSE AGRUM_INLINES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/*.inl)
file(GLOB_RECURSE AGRUM_TEMPLATES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/*.tcc)


### CONDITIONAL RULES (if flex/bison have been found, we add rules) ####
include("cmake/FlexBison.agrum.cmake")
if(BISON_FLEX_EXECUTABLE STREQUAL "ok")
	FlexBisonTarget(learning/XmlCsvParser parseXmlCsv)
endif(BISON_FLEX_EXECUTABLE STREQUAL "ok")
####################################################################

### CONDITIONAL RULES (if COCO/R has been found, we add rules) ####
include("cmake/CocoR.agrum.cmake")
if(NOT COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")
	CocoRTarget(BN/io/BIF/cocoR SyntaxBIF gum:BIF)
	CocoRTarget(BN/io/DSL/cocoR SyntaxDSL gum:DSL)
	CocoRTarget(prm/skool/cocoR SyntaxSKOOL gum:prm:skool)
	CocoRTarget(prm/skoor/cocoR SyntaxSKOOR gum:prm:skoor)
endif(NOT COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")
####################################################################

include_directories(${AGRUM_BINARY_DIR})
include_directories(${AGRUM_SOURCE_DIR})
link_directories(${AGRUM_BINARY_DIR}/lib)

set(AGRUM_FILES "${AGRUM_SOURCES} ${AGRUM_INLINES} ${AGRUM_INCLUDES} ${AGRUM_TEMPLATES}")

add_library(${LIBAGRUM} SHARED ${AGRUM_SOURCES})
set_target_properties(${LIBAGRUM} PROPERTIES VERSION ${AGRUM_VERSION} SOVERSION ${AGRUM_VERSION_MAJOR})

## target docs
include("cmake/Doxygen.agrum.cmake")

## TARGET run
file(GLOB_RECURSE RUN_AGRUM_SOURCES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/run/*.cpp)
file(GLOB_RECURSE RUN_AGRUM_INCLUDES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/run/*.h)
add_executable(run EXCLUDE_FROM_ALL ${RUN_AGRUM_SOURCES})
target_link_libraries(run ${LIBAGRUM})

## TARGET test
include("cmake/CxxTest.agrum.cmake")

include("cmake/Install.agrum.cmake")
include("cmake/Pack.agrum.cmake")
