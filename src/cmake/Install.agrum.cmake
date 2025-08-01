
include(GNUInstallDirs)

## installation module for recognize agrum (see documentation "How to use agrum")
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/aGrUMConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/aGrUMConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aGrUM
        )
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/agrum.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        )

set(CXX_FLAGS "${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")
configure_file(
        "${CMAKE_CURRENT_CMAKE_DIR}/agrum.pc.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/agrum.pc"
)

install(CODE "MESSAGE(\"\n\n************************\")")
install(CODE "MESSAGE(\"* Installing Agrum ... *\")")
install(CODE "MESSAGE(\"************************\n\n\")")
install(DIRECTORY ${AGRUM_SOURCE_DIR}/agrum DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh")
install(DIRECTORY ${AGRUM_BINARY_DIR}/agrum DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN PATTERN "*.h")
install(TARGETS ${LIBAGRUM}
        EXPORT ${LIBAGRUM}-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT ${LIBAGRUM}-targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aGrUM
        COMPONENT Development)

foreach (OPTION ${LIST_OF_MODULES})
        if (BUILD_${OPTION} OR BUILD_ALL)
          install(TARGETS agrum${OPTION}
                  EXPORT agrum${OPTION}-targets
                  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
          install(EXPORT agrum${OPTION}-targets
                  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aGrUM
                  COMPONENT Development)
        endif()
endforeach()
