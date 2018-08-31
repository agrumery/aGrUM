if(POLICY CMP0042) # use of @rpath
  cmake_policy(SET CMP0042 OLD)
endif(POLICY CMP0042)

if (NOT DEFINED CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

if (NOT DEFINED GUM_RANDOMSEED)
  set(GUM_RANDOMSEED 0)
endif()

#workaround : cmake force the value of CMAKE_INSTALL_PREFIX
set(BACKUP_CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

include("VERSION.txt")
set(AGRUM_VERSION "${AGRUM_VERSION_MAJOR}.${AGRUM_VERSION_MINOR}.${AGRUM_VERSION_PATCH}")

cmake_policy(SET CMP0048 NEW) # VERSION arg in 'project'
project (agrum VERSION ${AGRUM_VERSION})

set(CMAKE_INSTALL_PREFIX ${BACKUP_CMAKE_INSTALL_PREFIX})

cmake_minimum_required (VERSION 3.1)

# options
option(BUILD_SHARED_LIBS "build shared|static libs" ON)
option (BUILD_PYTHON "build python bindings" ON)
option (BUILD_JAVA "build java bindings (experimental)" OFF) ## Very experimental for now.

set (AGRUM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (AGRUM_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake)

set (LIBAGRUM "agrum")
include (CompilOptions.agrum)
include (openMP)

add_subdirectory (src)
add_subdirectory (wrappers)

