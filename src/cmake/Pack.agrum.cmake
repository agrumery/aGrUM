## for creation of cmake package AGRUM
set(AGRUM_LIBRARY ${LIBAGRUM})
set (AGRUM_LIBRARIES ${AGRUM_LIBRARY})
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
if (OPENMP_FOUND)
  list (APPEND AGRUM_LIBARIES gomp)
endif ()
configure_file(
  "${CMAKE_CURRENT_CMAKE_DIR}/aGrUMConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/aGrUMConfig.cmake"
)
include("cmake/CPackAgrum.cmake")
include(CPack)
