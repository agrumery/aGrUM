## for creation of cmake package AGRUM
set(AGRUM_LIBRARY ${LIBAGRUM})
set (AGRUM_LIBRARIES ${AGRUM_LIBRARY})
if (OPENMP_FOUND)
  list (APPEND AGRUM_LIBARIES gomp)
endif ()
configure_file(
  "${CMAKE_CURRENT_CMAKE_DIR}/aGrUMConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/aGrUMConfig.cmake"
)
include("cmake/CPackAgrum.cmake")
include(CPack)
