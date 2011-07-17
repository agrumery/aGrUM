## installation module for recognize agrum (see documentation "How to use agrum")
#include(CMakeExportBuildSettings)
#cmake_export_build_settings(${CMAKE_CURRENT_BINARY_DIR}/aGrUMBuildSettings.cmake)
export_library_dependencies(${CMAKE_CURRENT_BINARY_DIR}/aGrUMLibraryDepends.cmake)
install(FILES
    ${CMAKE_CURRENT_CMAKE_DIR}/aGrUMUse.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/aGrUMConfig.cmake
#  ${CMAKE_CURRENT_BINARY_DIR}/aGrUMBuildSettings.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/aGrUMLibraryDepends.cmake
    DESTINATION lib/aGrUM
    )
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/agrum.pc
  DESTINATION lib/pkgconfig
)

set(CXX_FLAGS "${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")
configure_file(
  "${CMAKE_CURRENT_CMAKE_DIR}/agrum.pc.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/agrum.pc"
)

install(CODE "MESSAGE(\"\n\n************************\")")
install(CODE "MESSAGE(\"* Installing Agrum ... *\")")
install(CODE "MESSAGE(\"************************\n\n\")")
install(DIRECTORY ${AGRUM_SOURCE_DIR}/agrum DESTINATION include FILES_MATCHING PATTERN ".svn" EXCLUDE PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.tcc" PATTERN "*.inl")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/agrum/config.h DESTINATION include)
install(TARGETS ${LIBAGRUM} DESTINATION lib)
