 project(EXAMPLE)
cmake_minimum_required(VERSION 2.8)
# classical options 
# -DCMAKE_BUILD_TYPE=DEBUG|RELEASE 
# -G "MinGW Makefiles" 

# cmake -DCMAKE_BUILD_TYPE=DEBUG
# or
# cmake -DCMAKE_BUILD_TYPE=RELEASE
#     RELEASE is the default option (thanks to the next lines)
if( CMAKE_BUILD_TYPE )
  message(STATUS  "version : '${CMAKE_BUILD_TYPE}'")
else ()
  set( CMAKE_BUILD_TYPE RELEASE)
  message(STATUS  "default version : ${CMAKE_BUILD_TYPE}")
endif()

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



file(GLOB EXAMPLE_SOURCE ${EXAMPLE_SOURCE_DIR}/*.cpp)
file(GLOB EXAMPLE_INCLUDE ${EXAMPLE_SOURCE_DIR}/*.h)

add_executable (example ${EXAMPLE_SOURCE})
if (${CMAKE_BUILD_TYPE}  STREQUAL "RELEASE") # release : act install release
  target_link_libraries(example agrum pthread)
else() # debug : act install debug
  target_link_libraries(example agrum-dbg pthread)
endif()

install (TARGETS example DESTINATION bin)

