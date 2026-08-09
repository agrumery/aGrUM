
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

# Libs: one -lagrum<MODULE> per module actually built (mirrors the install loop below)
set(AGRUM_PC_LIBS "")
foreach (OPTION ${LIST_OF_MODULES})
    if (BUILD_${OPTION} OR BUILD_ALL)
        set(AGRUM_PC_LIBS "${AGRUM_PC_LIBS} -lagrum${OPTION}")
    endif ()
endforeach ()

# Threading flavor propagated to pkg-config consumers
if (GUM_THREADS MATCHES "stl")
    set(AGRUM_PC_THREAD_CFLAGS "")
    set(AGRUM_PC_THREAD_LIBS "${CMAKE_THREAD_LIBS_INIT}")
elseif (GUM_THREADS MATCHES "omp")
    set(AGRUM_PC_THREAD_CFLAGS "${OpenMP_CXX_FLAGS}")
    set(AGRUM_PC_THREAD_LIBS "${OpenMP_CXX_FLAGS}")
endif ()

configure_file(
        "${CMAKE_CURRENT_CMAKE_DIR}/agrum.pc.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/agrum.pc"
        @ONLY
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
