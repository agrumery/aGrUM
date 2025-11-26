############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

include(GenerateExportHeader)
set(CMAKE_CURRENT_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(Config.agrum)
include("modules.txt")
include(Modules.agrum)
include(Nanodbc.agrum)
include(threads)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in" "${AGRUM_BINARY_DIR}/agrum/config.h")
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

# warning: 'nanodbc::result::result_impl' has a field '{anonymous}::bound_column* nanodbc::result::result_impl::bound_columns_' whose type uses the anonymous namespace
set_source_files_properties(agrum/base/external/nanodbc/nanodbc.cpp  PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

#collect all source files 
buildFileListsWithModules()

####################################################################
target_link_libraries(agrumBASE PRIVATE ${ODBC_LIBRARIES} ${GUM_COVERAGE_LINK_LIBRARY})
if (OPENMP_FOUND)
    target_link_libraries(agrumBASE PUBLIC OpenMP::OpenMP_CXX)
endif ()
if (BUILD_PYTHON)
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

if (_AGRUM_CLANG_SUPPORTS_ASSUME_UNIQUE_VTABLES)
    target_compile_options (agrumBASE PUBLIC "-fno-assume-unique-vtables")
endif ()

# precompile most often used headers (grep -nrI '#include <agrum' src/|cut -d " " -f 2| sort|uniq -c| sort -n)
target_precompile_headers(agrumBASE PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/agrum/agrum.h>")
target_precompile_headers(agrumBASE PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/agrum/base/variables/labelizedVariable.h>")
target_precompile_headers(agrumBASE PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/agrum/base/core/math/math_utils.h>")
target_precompile_headers(agrumBASE PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/agrum/base/multidim/tensor.h>")
if (TARGET agrumBN)
  target_precompile_headers(agrumBN PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/agrum/BN/BayesNet.h>")
  target_precompile_headers(agrumBN PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/agrum/BN/inference/lazyPropagation.h>")
endif ()

## TARGET library
add_library(${LIBAGRUM} INTERFACE)
foreach (OPTION ${LIST_OF_MODULES})
    if (BUILD_${OPTION} OR BUILD_ALL)
        target_link_libraries(${LIBAGRUM} INTERFACE agrum${OPTION})
    endif()
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
