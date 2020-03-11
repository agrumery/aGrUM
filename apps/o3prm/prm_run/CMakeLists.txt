###########################################################################
#   Copyright (C) 2005-2020 by Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)   #
#   info_at_agrum_dot_org                                               #
#                                                                         #
#   This program is free software; you can redistribute it and/or modify  #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 2 of the License, or     #
#   (at your option) any later version.                                   #
#                                                                         #
#   This program is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#   GNU General Public License for more details.                          #
#                                                                         #
#   You should have received a copy of the GNU General Public License     #
#   along with this program; if not, write to the                         #
#   Free Software Foundation, Inc.,                                       #
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
###########################################################################

# Declaring the project and generale info about it
PROJECT( PRM_RUN )

CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

SET( PRM_RUN_VERSION_MAJOR "3" )
SET( PRM_RUN_VERSION_MINOR "0" )
SET( PRM_RUN_VERSION_PATCH "0" )

# Configuring cmake
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED on )

# Listing source files
SET( PRM_RUN_MAIN "${CMAKE_SOURCE_DIR}/src/main.cpp" )

# Using C++ 14 
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )

# Use this if 
#set(AGRUM_INSTALLATION_DIRECTORY "/usr/local/")

#if (NOT DEFINED aGrUM_DIR)
#  set(aGrUM_DIR "${AGRUM_INSTALLATION_DIRECTORY}/lib/cmake/aGrUM/")
#endif ()

find_package(aGrUM NO_MODULE)

if (aGrUM_FOUND)
  include(${AGRUM_USE_FILE})
else (aGrUM_FOUND)
  message(FATAL_ERROR "Please install aGrUM")
endif (aGrUM_FOUND)

# cmake -DCMAKE_BUILD_TYPE=DEBUG
# or
# cmake -DCMAKE_BUILD_TYPE=RELEASE
#     RELEASE is the default option (thanks to the next 3 lines)
IF( CMAKE_BUILD_TYPE )
  MESSAGE( STATUS  "version : '${CMAKE_BUILD_TYPE}'" )
ELSE ()
  SET( CMAKE_BUILD_TYPE RELEASE)
  MESSAGE( STATUS  "default version : ${CMAKE_BUILD_TYPE}" )
ENDIF()

# Adding boost dependencies
SET(BOOST_USE_STATIC_LIBS OFF) 
SET(BOOST_USE_MULTITHREADED ON)  
SET(BOOST_USE_STATIC_RUNTIME OFF) 
FIND_PACKAGE(Boost REQUIRED COMPONENTS program_options) 

# Adding include dirs and files
INCLUDE_DIRECTORIES( ${aGrUM_INCLUDES} ${Boost_INCLUDE_DIRS})

## Building the executable
ADD_EXECUTABLE( prm_run ${PRM_RUN_MAIN} )
TARGET_LINK_LIBRARIES( prm_run ${AGRUM_LIBRARIES_${CMAKE_BUILD_TYPE}} ${Boost_PROGRAM_OPTIONS_LIBRARY})

INSTALL(TARGETS prm_run
        RUNTIME DESTINATION bin)

