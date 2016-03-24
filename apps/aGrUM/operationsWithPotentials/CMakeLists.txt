project(OPERATIONS_WITH_POTENTIAL)
cmake_minimum_required(VERSION 2.8)
# classical options 
# -DCMAKE_BUILD_TYPE=DEBUG|RELEASE 
# -G "MinGW Makefiles" 

include("${OPERATIONS_WITH_POTENTIAL_SOURCE_DIR}/../config.txt")
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

file(GLOB OPERATIONS_WITH_POTENTIAL_SOURCE ${OPERATIONS_WITH_POTENTIAL_SOURCE_DIR}/*.cpp)
file(GLOB OPERATIONS_WITH_POTENTIAL_INCLUDE ${OPERATIONS_WITH_POTENTIAL_SOURCE_DIR}/*.h)

add_executable (operationsWithPotentials ${OPERATIONS_WITH_POTENTIAL_SOURCE})
target_link_libraries(operationsWithPotentials ${AGRUM_LIBRARIES_${CMAKE_BUILD_TYPE}})
install (TARGETS operationsWithPotentials DESTINATION bin)

