project(EXAMPLE)
cmake_minimum_required(VERSION 2.8)
# classical options 
# -DCMAKE_BUILD_TYPE=Debug|Release 
# -G "MinGW Makefiles" 

# cmake -DCMAKE_BUILD_TYPE=Debug
# or
# cmake -DCMAKE_BUILD_TYPE=Release
#     Release is the default option (thanks to the next lines)

set (CMAKE_CXX_STANDARD 14)

include("${EXAMPLE_SOURCE_DIR}/../config.txt")
if (NOT DEFINED aGrUM_DIR)
  set(aGrUM_DIR "${AGRUM_INSTALLATION_DIRECTORY}/lib/cmake/aGrUM/")
endif ()

find_package(aGrUM)

if (aGrUM_FOUND)
  include(${AGRUM_USE_FILE})
else (aGrUM_FOUND)
  message(FATAL_ERROR "Please install aGrUM")
endif (aGrUM_FOUND)

file(GLOB EXAMPLE_SOURCE ${EXAMPLE_SOURCE_DIR}/*.cpp)
file(GLOB EXAMPLE_INCLUDE ${EXAMPLE_SOURCE_DIR}/*.h)

add_executable (example ${EXAMPLE_SOURCE})
target_link_libraries(example agrum)

install (TARGETS example DESTINATION bin)

