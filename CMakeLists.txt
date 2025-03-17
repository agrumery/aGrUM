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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

cmake_minimum_required(VERSION 3.18)
if (POLICY CMP0042) # use of @rpath
    cmake_policy(SET CMP0042 NEW)
endif (POLICY CMP0042)
if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()
if (NOT DEFINED GUM_RANDOMSEED)
    set(GUM_RANDOMSEED 0)
endif ()
set(BACKUP_CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

include("VERSION.txt")
set(AGRUM_VERSION "${AGRUM_VERSION_MAJOR}.${AGRUM_VERSION_MINOR}.${AGRUM_VERSION_PATCH}")

cmake_policy(SET CMP0048 NEW) # VERSION arg in 'project'

project(agrum VERSION ${AGRUM_VERSION})

set(CMAKE_INSTALL_PREFIX ${BACKUP_CMAKE_INSTALL_PREFIX})


# options
option(BUILD_SHARED_LIBS "build shared|static libs" ON)
option(BUILD_PYTHON "build python bindings" OFF)
option(BUILD_JAVA "build java bindings (experimental)" OFF) ## Very experimental for now.
option(USE_OPENMP "use openmp" ON)

set(AGRUM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(AGRUM_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake)

set(LIBAGRUM "agrum")
include(CompilOptions.agrum)
if (USE_OPENMP)
    include(openMP)
endif ()

enable_testing()

add_subdirectory(src)
add_subdirectory(wrappers)
