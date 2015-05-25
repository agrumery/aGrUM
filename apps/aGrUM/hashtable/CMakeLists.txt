project(EXAMPLE)
cmake_minimum_required(VERSION 2.8)

include("${EXAMPLE_SOURCE_DIR}/../config.txt")
if (NOT DEFINED aGrUM_DIR)
  set(aGrUM_DIR "${AGRUM_INSTALLATION_DIRECTORY}/lib/cmake/aGrUM/")
endif ()

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
if( CMAKE_BUILD_TYPE )
  message(STATUS  "version : '${CMAKE_BUILD_TYPE}'")
else ()
  set( CMAKE_BUILD_TYPE RELEASE)
  message(STATUS  "default version : ${CMAKE_BUILD_TYPE}")
endif()

file(GLOB EXAMPLE_SOURCE ${EXAMPLE_SOURCE_DIR}/*.cpp)
file(GLOB EXAMPLE_INCLUDE ${EXAMPLE_SOURCE_DIR}/*.h)

add_executable (example ${EXAMPLE_SOURCE})
target_link_libraries(example ${AGRUM_LIBRARIES})
install (TARGETS example DESTINATION bin)

