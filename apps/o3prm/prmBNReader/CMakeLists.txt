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

project(PRMBNREADER)
cmake_minimum_required(VERSION 2.8)
set(AGRUM_INSTALLATION_DIRECTORY "~/usr")
message(STATUS "aGrUM installation path : ${AGRUM_INSTALLATION_DIRECTORY} ")
if (NOT DEFINED aGrUM_DIR)
  set(aGrUM_DIR "${AGRUM_INSTALLATION_DIRECTORY}/lib/cmake/aGrUM/")
endif ()
find_package(aGrUM NO_MODULE)
if (aGrUM_FOUND)
  include(${AGRUM_USE_FILE})
else (aGrUM_FOUND)
  message(FATAL_ERROR "Please install aGrUM")
endif (aGrUM_FOUND)
if( CMAKE_BUILD_TYPE )
  message(STATUS  "version : '${CMAKE_BUILD_TYPE}'")
else ()
  set( CMAKE_BUILD_TYPE RELEASE)
  message(STATUS  "default version : ${CMAKE_BUILD_TYPE}")
endif()

file(GLOB PRMBNREADER_SOURCE ${PRMBNREADER_SOURCE_DIR}/src/*.cpp)
file(GLOB PRMBNREADER_INCLUDE ${PRMBNREADER_SOURCE_DIR}/src/*.h)

add_executable (prmBnReader ${PRMBNREADER_SOURCE})
target_link_libraries(prmBnReader ${AGRUM_LIBRARIES})
install (TARGETS prmBnReader DESTINATION bin)
