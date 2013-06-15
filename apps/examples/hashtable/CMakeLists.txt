project(EXAMPLE)
cmake_minimum_required(VERSION 2.8)

include("${EXAMPLE_SOURCE_DIR}/../config.txt")
set(aGrUM_DIR "${AGRUM_INSTALLATION_DIRECTORY}/lib/aGrUM/")

find_package(aGrUM)

if (aGrUM_FOUND)
  include_directories(${AGRUM_INCLUDE_DIR})
  link_directories(${AGRUM_LIB_DIR})
else (aGrUM_FOUND)
  message(FATAL_ERROR "Please install aGrUM")
endif (aGrUM_FOUND)

# cmake -DCMAKE_BUILD_TYPE=DEBUG 
# or
# cmake -DCMAKE_BUILD_TYPE=RELEASE 
#     RELEASE is the default option (thanks to the next 3 lines)
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE RELEASE)
endif()

file(GLOB EXAMPLE_SOURCE ${EXAMPLE_SOURCE_DIR}/*.cpp)
file(GLOB EXAMPLE_INCLUDE ${EXAMPLE_SOURCE_DIR}/*.h)

add_executable (example ${EXAMPLE_SOURCE})

if (${CMAKE_BUILD_TYPE} STREQUAL "RELEASE") # release : act install release
  message(STATUS "Release version")
  target_link_libraries(example agrum)
else() # debug : act install debug
  message(STATUS "Debug version")
  target_link_libraries(example agrum-dbg)
endif()
