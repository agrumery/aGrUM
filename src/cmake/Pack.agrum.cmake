## for creation of cmake package AGRUM
get_filename_component(AGRUM_INSTALL_LIBRARY AGRUM_LIBRARY NAME)
set(AGRUM_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${AGRUM_INSTALL_LIBRARY})
set(AGRUM_INCLUDE ${CMAKE_INSTALL_PREFIX}/include)
configure_file(
  "${CMAKE_CURRENT_CMAKE_DIR}/aGrUMConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/aGrUMConfig.cmake"
)
include("cmake/CPackAgrum.cmake")
include(CPack)
