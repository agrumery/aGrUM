project(PRM_RUN)
cmake_minimum_required(VERSION 2.8)

# do not forget to change this line if needed ("act install -d...")
# set(AGRUM_INSTALLATION_DIRECTORY "/home/nin/agrum")
# set(aGrUM_DIR "${AGRUM_INSTALLATION_DIRECTORY}/lib/aGrUM/")

find_package(aGrUM)

if (aGrUM_FOUND)
   include_directories(${AGRUM_INCLUDE_DIR})
   link_directories(${AGRUM_LIB_DIR})
else (aGrUM_FOUND)
  message(FATAL_ERROR "Please install aGrUM")
endif (aGrUM_FOUND)

file(GLOB PRM_RUN_SOURCE ${PRM_RUN_SOURCE_DIR}/*.cpp)
file(GLOB PRM_RUN_INCLUDE ${PRM_RUN_SOURCE_DIR}/*.h)

add_executable (prm_run ${PRM_RUN_SOURCE})
target_link_libraries(prm_run agrum)

install(TARGETS prm_run RUNTIME DESTINATION bin)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

