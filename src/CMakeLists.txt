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
if (COCOR_EXECUTABLE)
    message(STATUS "** aGrUM notification: Coco/R targets")
    CocoRTarget(base/core/math/cocoR SyntaxFormula gum:formula BASE)
    CocoRTarget(BN/io/UAI/cocoR SyntaxUAIBN gum:UAIBN BN)
    CocoRTarget(BN/io/BIF/cocoR SyntaxBIF gum:BIF BN)
    CocoRTarget(BN/io/DSL/cocoR SyntaxDSL gum:DSL BN)
    CocoRTarget(BN/io/net/cocoR SyntaxNet gum:net BN)
    CocoRTarget(PRM/o3prmr/cocoR SyntaxO3PRMR gum:prm:o3prmr PRM)
    CocoRTarget(PRM/o3prm/cocoR SyntaxO3PRM gum:prm:o3prm PRM)
    CocoRTarget(FMDP/io/dat/cocoR SyntaxDat gum:MDPDAT FMDP)
    CocoRTarget(MRF/io/UAI/cocoR SyntaxUAIMRF gum:UAIMRF MRF)
endif ()
####################################################################

# o4DGContext.cpp:67:91: error: no matching function for call to 'log2(int)
set_source_files_properties(agrum/base/multidim/utils/FunctionGraphUtilities/operators/o4DGContext.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

# warning: 'nanodbc::result::result_impl' has a field '{anonymous}::bound_column* nanodbc::result::result_impl::bound_columns_' whose type uses the anonymous namespace
set_source_files_properties(agrum/base/external/nanodbc/nanodbc.cpp  PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

#collect all source files 
buildFileListsWithModules()

####################################################################
target_link_libraries(agrumBASE PRIVATE ${ODBC_LIBRARIES} ${GUM_COVERAGE_LINK_LIBRARY})
if (OPENMP_FOUND)
    target_link_libraries(agrumBASE PUBLIC OpenMP::OpenMP_CXX)
endif ()
if (BUILD_PYTHON OR BUILD_JAVA)
    target_compile_definitions(agrumBASE PUBLIC GUM_FOR_SWIG)
endif ()
if (MSVC)
    target_compile_definitions(agrumBASE PUBLIC NOMINMAX)
    target_compile_definitions(agrumBASE PUBLIC _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(agrumBASE PUBLIC _CRT_NONSTDC_NO_DEPRECATE)
    target_compile_options(agrumBASE PUBLIC /bigobj)
endif ()
if (MINGW)
    target_compile_definitions(agrumBASE PUBLIC GUM_NO_EXTERN_TEMPLATE_CLASS)
endif ()

if (NOT AGRUM_INLINE)
    target_compile_definitions(agrumBASE PUBLIC GUM_NO_INLINE)
endif ()

## TARGET library
add_library(${LIBAGRUM} INTERFACE)
foreach (OPTION ${LIST_OF_MODULES})
    target_link_libraries(${LIBAGRUM} INTERFACE agrum${OPTION})
endforeach()

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
