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

file(GLOB PRMBNREADER_SOURCE ${PRMBNREADER_SOURCE_DIR}/src/*.cpp)
file(GLOB PRMBNREADER_INCLUDE ${PRMBNREADER_SOURCE_DIR}/src/*.h)

add_executable (prmBnReader ${PRMBNREADER_SOURCE})
target_link_libraries(prmBnReader ${AGRUM_LIBRARIES})
install (TARGETS prmBnReader DESTINATION bin)
