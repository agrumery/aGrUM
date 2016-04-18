if (POLICY CMP0018)
  # ignore the POSITION_INDEPENDENT_CODE property for all targets and
  # use the modified value of CMAKE_SHARED_LIBRARY_<Lang>_FLAGS for
  # SHARED and MODULE libraries.
  cmake_policy(SET CMP0018 OLD)
endif ()

if(APPLE)
	set(CMAKE_MACOSX_RPATH ON)
endif()


set (CMAKE_CURRENT_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(Config.agrum)

include("modules.txt")
include(Modules.agrum)

include(Nanodbc.agrum)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in" "${AGRUM_BINARY_DIR}/agrum/config.h")

### CONDITIONAL RULES (if COCO/R has been found, we add rules) ####
include(CocoR.agrum)
if(NOT COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")
     message(STATUS "** aGrUM notification: Coco/R targets")
     CocoRTarget(core/math/cocoR SyntaxFormula gum:formula BASE)
     CocoRTarget(BN/io/UAI/cocoR SyntaxUAI gum:UAI BN)
     CocoRTarget(BN/io/BIF/cocoR SyntaxBIF gum:BIF BN)
     CocoRTarget(BN/io/DSL/cocoR SyntaxDSL gum:DSL BN)
     CocoRTarget(BN/io/net/cocoR SyntaxNet gum:net BN)
     CocoRTarget(PRM/o3prmr/cocoR SyntaxO3PRMR gum:prm:o3prmr PRM)
     CocoRTarget(PRM/o3prm/cocoR SyntaxO3PRM gum:prm:o3prm PRM)
     CocoRTarget(FMDP/io/dat/cocoR SyntaxDat gum:MDPDAT FMDP)
endif(NOT COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")
####################################################################

#collect all source files 
buildFileListsWithModules()

include_directories(BEFORE ${AGRUM_BINARY_DIR})
include_directories(BEFORE ${AGRUM_SOURCE_DIR})
link_directories(${AGRUM_BINARY_DIR}/lib)
set(AGRUM_FILES ${AGRUM_SOURCES} ${AGRUM_C_SOURCES} ${AGRUM_INLINES} ${AGRUM_INCLUDES} ${AGRUM_TEMPLATES})

####################################################################
## TARGET library
add_library(${LIBAGRUM} ${AGRUM_FILES})
set_target_properties (${LIBAGRUM} PROPERTIES DEBUG_POSTFIX "-dbg")
set_target_properties(${LIBAGRUM} PROPERTIES VERSION ${AGRUM_VERSION} SOVERSION ${AGRUM_VERSION_MAJOR})
target_link_libraries( ${LIBAGRUM} ${ODBC_LIBRARIES} ${GUM_COVERAGE_LINK_LIBRARY})

####################################################################
## TARGET docs
include(Doxygen.agrum)

####################################################################
## TARGET run
file(GLOB_RECURSE RUN_AGRUM_SOURCES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/run/*.cpp)
file(GLOB_RECURSE RUN_AGRUM_INCLUDES RELATIVE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/run/*.h)
add_executable(run EXCLUDE_FROM_ALL ${RUN_AGRUM_SOURCES})
set_target_properties(run PROPERTIES COMPILE_FLAGS "-DGUM_RANDOMSEED=${GUM_RANDOMSEED}")
target_link_libraries(run ${LIBAGRUM})

####################################################################
## TARGET test
include(CxxTest.agrum)

####################################################################
## TARGET install
include(Install.agrum)

####################################################################
## TARGET pack
include(Pack.agrum)

####################################################################
# TARGET uninstall
configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  IMMEDIATE @ONLY
)

add_custom_target ( uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)
