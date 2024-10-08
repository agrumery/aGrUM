include(GenerateExportHeader)

set(CMAKE_CURRENT_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(Config.agrum)

include("modules.txt")
include(Modules.agrum)

include(Nanodbc.agrum)
include(threads)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in" "${AGRUM_BINARY_DIR}/agrum/config.h")

### CONDITIONAL RULES (if COCO/R has been found, we add rules) ####
include(CocoR.agrum)
if (NOT COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")
    message(STATUS "** aGrUM notification: Coco/R targets")
    CocoRTarget(tools/core/math/cocoR SyntaxFormula gum:formula BASE)
    CocoRTarget(BN/io/UAI/cocoR SyntaxUAIBN gum:UAIBN BN)
    CocoRTarget(BN/io/BIF/cocoR SyntaxBIF gum:BIF BN)
    CocoRTarget(BN/io/DSL/cocoR SyntaxDSL gum:DSL BN)
    CocoRTarget(BN/io/net/cocoR SyntaxNet gum:net BN)
    CocoRTarget(PRM/o3prmr/cocoR SyntaxO3PRMR gum:prm:o3prmr PRM)
    CocoRTarget(PRM/o3prm/cocoR SyntaxO3PRM gum:prm:o3prm PRM)
    CocoRTarget(FMDP/io/dat/cocoR SyntaxDat gum:MDPDAT FMDP)
    CocoRTarget(MRF/io/UAI/cocoR SyntaxUAIMRF gum:UAIMRF MRF)
endif (NOT COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")
####################################################################

#collect all source files 
buildFileListsWithModules()

include_directories(BEFORE ${AGRUM_BINARY_DIR})
include_directories(BEFORE ${AGRUM_SOURCE_DIR})
link_directories(${AGRUM_BINARY_DIR}/lib)

set(AGRUM_FILES ${AGRUM_SOURCES} ${AGRUM_C_SOURCES} ${AGRUM_INLINES} ${AGRUM_INCLUDES} ${AGRUM_TEMPLATES})

#needed for some IDE (such as jetbrain's)
set(SOURCE_FILES ${AGRUM_FILES})

####################################################################
## TARGET library
add_library(${LIBAGRUM} ${AGRUM_FILES})
target_compile_features(${LIBAGRUM} PUBLIC cxx_std_20)
set_target_properties(${LIBAGRUM} PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(${LIBAGRUM} PROPERTIES DEBUG_POSTFIX "-dbg")
set_target_properties(${LIBAGRUM} PROPERTIES VERSION ${AGRUM_VERSION} SOVERSION ${AGRUM_VERSION_MAJOR})
target_link_libraries(${LIBAGRUM} ${ODBC_LIBRARIES} ${GUM_COVERAGE_LINK_LIBRARY})
if (OPENMP_FOUND)
    set_target_properties(${LIBAGRUM} PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})
endif ()
if (MINGW)
    target_compile_definitions(${LIBAGRUM} PUBLIC GUM_NO_EXTERN_TEMPLATE_CLASS)
endif ()
GENERATE_EXPORT_HEADER(${LIBAGRUM}
        BASE_NAME ${LIBAGRUM}
        EXPORT_MACRO_NAME ${LIBAGRUM}_EXPORT
        EXPORT_FILE_NAME ${LIBAGRUM}_Export.h
        STATIC_DEFINE ${LIBAGRUM}_BUILT_AS_STATIC
        )
target_include_directories(${LIBAGRUM} INTERFACE
        $<INSTALL_INTERFACE:include>
        )

# Add targets to the build-tree export set
export(TARGETS ${LIBAGRUM} FILE ${LIBAGRUM}-targets.cmake)

####################################################################
## TARGET docs
include(Doxygen.agrum)

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
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
        IMMEDIATE @ONLY
)

add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
        )
