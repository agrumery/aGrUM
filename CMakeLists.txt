project (agrum)

cmake_minimum_required (VERSION 2.8)

if (NOT DEFINED CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

if (POLICY CMP0018)
  cmake_policy (SET CMP0018 OLD)
endif ()

if (POLICY CMP0042)
  cmake_policy (SET CMP0042 OLD)
endif ()

set (AGRUM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (AGRUM_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/src)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake)

set (LIBAGRUM "agrum")
include (CompilOptions.agrum)
include (openMP)

add_subdirectory (src)
add_subdirectory (wrappers)

