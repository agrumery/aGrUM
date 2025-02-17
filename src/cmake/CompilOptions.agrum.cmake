include(CheckCXXCompilerFlag)
set(WARNING_FLAGS "-Wall -pedantic -Wno-psabi")
check_cxx_compiler_flag("${WARNING_FLAGS}" _SUPPORT_WARNING_FLAGS)
if (_SUPPORT_WARNING_FLAGS)
    set(CMAKE_CXX_FLAGS "${WARNING_FLAGS} ${CMAKE_CXX_FLAGS}")
endif ()


if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(AGRUM_PROCESSOR "64")
else ()
    set(AGRUM_PROCESSOR "32")
endif ()

set(AGRUM_VERSION "${AGRUM_VERSION_MAJOR}.${AGRUM_VERSION_MINOR}.${AGRUM_VERSION_PATCH}")

if (MSVC)
    set(CMAKE_CXX_FLAGS "/utf-8 /MP ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "/utf-8 /MP ${CMAKE_C_FLAGS}")
endif ()

if (MINGW OR CMAKE_BUILD_TYPE MATCHES "^(DEBUG|Debug|debug)$")
    option(AGRUM_INLINE "inline code" OFF)
else ()
    option(AGRUM_INLINE "inline code" ON)
endif ()

if (CMAKE_BUILD_TYPE MATCHES "^(DEBUG|Debug|debug)$")
    message(STATUS "** aGrUM Notification : mode DEBUG")
    set(CMAKE_CXX_FLAGS "-DGUM_DEBUG_MODE ${CMAKE_CXX_FLAGS}")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_EXE_LINKER_FLAGS " -ggdb ${CMAKE_EXE_LINKER_FLAGS}") # add -pg for profiling
        set(CMAKE_SHARED_LINKER_FLAGS " -ggdb ${CMAKE_SHARED_LINKER_FLAGS}") # add -pg for profiling
        set(CMAKE_MODULE_LINKER_FLAGS " -ggdb ${CMAKE_MODULE_LINKER_FLAGS}") # add -pg for profiling
    endif ()
else ()
    message(STATUS "** aGrUM Notification : mode ${CMAKE_BUILD_TYPE}")
endif ()

if (WIN32)
    set(LIBRARY_OUTPUT_PATH ${AGRUM_BINARY_DIR})
endif (WIN32)
if (UNIX)
    set(LIBRARY_OUTPUT_PATH ${AGRUM_BINARY_DIR}/lib)
endif (UNIX)

if (USE_PROFILE)
    set(CMAKE_CXX_FLAGS "-g -pg ${CMAKE_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS " -g -pg ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS " -g -pg ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS " -g -pg ${CMAKE_MODULE_LINKER_FLAGS}")
endif ()

if (CMAKE_VERBOSE_MAKEFILE)
    set(GUM_TRACE_ON FALSE)
endif ()

if (GUM_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer --coverage ${CMAKE_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "-fno-omit-frame-pointer --coverage ${CMAKE_C_FLAGS}")
        set(GUM_COVERAGE_LINK_LIBRARY "gcov")
    else ()
        message(FATAL_ERROR "** Configuration error: code coverage is only supported with GNU GCC.")
    endif ()
else (GUM_COVERAGE)
    set(GUM_COVERAGE_LINK_LIBRARY "")
endif (GUM_COVERAGE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag("-fno-assume-unique-vtables" _AGRUM_CLANG_SUPPORTS_ASSUME_UNIQUE_VTABLES)
    if (_AGRUM_CLANG_SUPPORTS_ASSUME_UNIQUE_VTABLES)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-assume-unique-vtables")
    endif ()
endif ()

set(CMAKE_CXX_FLAGS "${AGRUM_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
