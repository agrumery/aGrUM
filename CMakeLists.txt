############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
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
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

cmake_minimum_required(VERSION 3.18)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()
if (NOT DEFINED GUM_RANDOMSEED)
    set(GUM_RANDOMSEED 0)
endif ()
set(BACKUP_CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

include("VERSION.txt")
set(AGRUM_VERSION "${AGRUM_VERSION_MAJOR}.${AGRUM_VERSION_MINOR}.${AGRUM_VERSION_PATCH}")

project(agrum VERSION ${AGRUM_VERSION})

message(STATUS "** aGrUM Notification : cpp standard C++20")
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_INSTALL_PREFIX ${BACKUP_CMAKE_INSTALL_PREFIX})


# options
option(BUILD_SHARED_LIBS "build shared|static libs" ON)
option(BUILD_PYTHON "build python bindings" OFF)
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

include(threads)

# Toolchain-wide compile requirements, set via directory scope (add_compile_definitions /
# add_compile_options / link_libraries) instead of target_compile_definitions(agrumBASE PUBLIC ...):
# this way they reach every target under src/ and wrappers/ regardless of the
# target_link_libraries graph. Needed because pyAgrum leaf submodules (PRM/MRF/CN/ID/CM) no longer
# transitively re-link agrumBASE/agrumBN (see buildFileListsWithModules() in Modules.agrum.cmake) --
# re-embedding them collided with _pyagrum's PYGUM_PUBLIC dllexports on MSVC (LNK2005) -- so they can
# no longer inherit these through that PUBLIC link chain either.
if (GUM_THREADS MATCHES "stl")
    if (Threads_FOUND)
        link_libraries(Threads::Threads)
        add_compile_definitions(GUM_THREADS_USE_STL)
    endif ()
elseif (GUM_THREADS MATCHES "omp")
    if (OpenMP_FOUND)
        link_libraries(OpenMP::OpenMP_CXX)
        add_compile_definitions(GUM_THREADS_USE_OMP)
    endif ()
endif ()

if (BUILD_PYTHON)
    add_compile_definitions(GUM_FOR_SWIG)
endif ()

if (MSVC)
    add_compile_definitions(NOMINMAX _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE)
    add_compile_options(/bigobj)
endif ()

if (MINGW OR MSVC)
    # MSVC: "extern template class" combined with PYGUM_PUBLIC/GUM_PUBLIC (__declspec(dllexport))
    # triggers C4910 ("dllexport and extern are incompatible on an explicit instantiation").
    add_compile_definitions(GUM_NO_EXTERN_TEMPLATE_CLASS)
endif ()

if (NOT AGRUM_INLINE)
    add_compile_definitions(GUM_NO_INLINE)
endif ()

if (_AGRUM_CLANG_SUPPORTS_ASSUME_UNIQUE_VTABLES)
    add_compile_options("-fno-assume-unique-vtables")
endif ()

add_subdirectory(src)
add_subdirectory(wrappers)
