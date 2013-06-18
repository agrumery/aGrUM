project(AGRUM)

cmake_minimum_required(VERSION 2.8)

include("version.txt")

set(CMAKE_CURRENT_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include("${CMAKE_CURRENT_CMAKE_DIR}/CompilOptions.agrum.cmake")

include("${CMAKE_CURRENT_CMAKE_DIR}/Config.agrum.cmake")
configure_file("${CMAKE_CURRENT_CMAKE_DIR}/config.h.in" "${AGRUM_BINARY_DIR}/agrum/config.h")

file(GLOB_RECURSE AGRUM_SOURCES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/*.cpp)
file(GLOB_RECURSE AGRUM_INCLUDES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/*.h)
file(GLOB_RECURSE AGRUM_INLINES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/*.inl)
file(GLOB_RECURSE AGRUM_TEMPLATES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/*.tcc)

file(GLOB_RECURSE AGRUM_C_SOURCES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/CN/lrslib/lrslib.c ${AGRUM_SOURCE_DIR}/agrum/CN/lrslib/lrsmp.c)

### CONDITIONAL RULES (if flex/bison have been found, we add rules) ####
include("${CMAKE_CURRENT_CMAKE_DIR}/FlexBison.agrum.cmake")
# if(BISON_FLEX_EXECUTABLE STREQUAL "ok")
#     FlexBisonTarget(learning/XmlCsvParser parseXmlCsv)
# endif(BISON_FLEX_EXECUTABLE STREQUAL "ok")
####################################################################

### CONDITIONAL RULES (if COCO/R has been found, we add rules) ####
include("${CMAKE_CURRENT_CMAKE_DIR}/CocoR.agrum.cmake")
if(NOT COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")
     CocoRTarget(BN/io/BIF/cocoR SyntaxBIF gum:BIF)
     CocoRTarget(BN/io/DSL/cocoR SyntaxDSL gum:DSL)
     CocoRTarget(BN/io/net/cocoR SyntaxNet gum:net)
     CocoRTarget(prm/o3prm/cocoR SyntaxO3PRM gum:prm:o3prm)
     CocoRTarget(prm/o3prmr/cocoR SyntaxO3PRMR gum:prm:o3prmr)
     CocoRTarget(FMDP/io/dat/cocoR SyntaxDat gum:MDPDAT)
endif(NOT COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")
####################################################################

include("${CMAKE_CURRENT_CMAKE_DIR}/openMP.cmake")

include_directories(${AGRUM_BINARY_DIR})
include_directories(${AGRUM_SOURCE_DIR})
link_directories(${AGRUM_BINARY_DIR}/lib)

set(AGRUM_FILES ${AGRUM_SOURCES} ${AGRUM_C_SOURCES} ${AGRUM_INLINES} ${AGRUM_INCLUDES} ${AGRUM_TEMPLATES})

add_library(${LIBAGRUM} SHARED ${AGRUM_FILES})
set_target_properties(${LIBAGRUM} PROPERTIES VERSION ${AGRUM_VERSION} SOVERSION ${AGRUM_VERSION_MAJOR})

## target docs
include("${CMAKE_CURRENT_CMAKE_DIR}/Doxygen.agrum.cmake")

## TARGET run
file(GLOB_RECURSE RUN_AGRUM_SOURCES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/run/*.cpp)
file(GLOB_RECURSE RUN_AGRUM_INCLUDES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/run/*.h)
add_executable(run EXCLUDE_FROM_ALL ${RUN_AGRUM_SOURCES})
target_link_libraries(run ${LIBAGRUM})

## TARGET test
include("${CMAKE_CURRENT_CMAKE_DIR}/CxxTest.agrum.cmake")

include("${CMAKE_CURRENT_CMAKE_DIR}/Install.agrum.cmake")
include("${CMAKE_CURRENT_CMAKE_DIR}/Pack.agrum.cmake")
