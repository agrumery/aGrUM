if(POLICY CMP0042) # use of @rpath
  cmake_policy(SET CMP0042 OLD)
endif(POLICY CMP0042)

if(POLICY CMP0048) # AGRUM_VERSION is build by version.txt not by project command
  cmake_policy(SET CMP0048 OLD)
endif(POLICY CMP0048)

if (NOT DEFINED CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

if (NOT DEFINED GUM_RANDOMSEED)
  set(GUM_RANDOMSEED 0)
endif()

#workaround : cmake force the value of CMAKE_INSTALL_PREFIX
set(BACKUP_CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
project (agrum)
set(CMAKE_INSTALL_PREFIX ${BACKUP_CMAKE_INSTALL_PREFIX})

cmake_minimum_required (VERSION 3.1)

include("VERSION.txt")

set (AGRUM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (AGRUM_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake)

set (LIBAGRUM "agrum")
include (CompilOptions.agrum)
include (openMP)

add_subdirectory (src)
add_subdirectory (wrappers)

