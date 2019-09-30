
include (CheckCXXCompilerFlag)
set (WARNING_FLAGS "-Wall -pedantic")
check_cxx_compiler_flag("${WARNING_FLAGS}" _SUPPORT_WARNING_FLAGS)
if (_SUPPORT_WARNING_FLAGS)
  set (CMAKE_CXX_FLAGS "${WARNING_FLAGS} ${CMAKE_CXX_FLAGS}")
endif ()

find_package (CXX14)
if (CXX14_FOUND)
	if (MSVC)
		# looks like it is not needed for visual C++
	else()
		set (AGRUM_CXX_FLAGS "${CXX14_FLAGS} ${AGRUM_CXX_FLAGS}" )
	endif ()
else ()
  message(FATAL_ERROR "** aGrUM error: aGrUM is now using C++14. Please find a compiler (for instance GCC) C++14 compliant")
endif ()


if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  set(AGRUM_PROCESSOR "64")
else()
  set(AGRUM_PROCESSOR "32")
endif()

set(AGRUM_VERSION "${AGRUM_VERSION_MAJOR}.${AGRUM_VERSION_MINOR}.${AGRUM_VERSION_PATCH}")

if (MSVC)
  set (CMAKE_CXX_FLAGS "/DNOMINMAX /bigobj /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_DEPRECATE /MP ${CMAKE_CXX_FLAGS}")
  set (CMAKE_C_FLAGS "/DNOMINMAX /bigobj /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_DEPRECATE /MP ${CMAKE_C_FLAGS}")
endif()

if (MINGW)
  set (CMAKE_CXX_FLAGS "-DGUM_NO_INLINE ${CMAKE_CXX_FLAGS}")
endif()

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
  set (CMAKE_CXX_FLAGS "-DGUM_DEBUG_MODE -DGUM_NO_INLINE ${CMAKE_CXX_FLAGS}")
  if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_EXE_LINKER_FLAGS " -ggdb ${CMAKE_EXE_LINKER_FLAGS}") # add -pg for profiling
    set(CMAKE_SHARED_LINKER_FLAGS " -ggdb ${CMAKE_SHARED_LINKER_FLAGS}") # add -pg for profiling
    set(CMAKE_MODULE_LINKER_FLAGS " -ggdb ${CMAKE_MODULE_LINKER_FLAGS}") # add -pg for profiling
  endif ()
endif ()

if(WIN32)
  set(LIBRARY_OUTPUT_PATH ${AGRUM_BINARY_DIR})
endif(WIN32)
if(UNIX)
  set(LIBRARY_OUTPUT_PATH ${AGRUM_BINARY_DIR}/lib)
endif(UNIX)

if (CMAKE_VERBOSE_MAKEFILE)
  set (GUM_TRACE_ON FALSE)
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
  set (GUM_COVERAGE_LINK_LIBRARY "" )
endif(GUM_COVERAGE)

if (BUILD_PYTHON OR BUILD_JAVA)
  set (CMAKE_CXX_FLAGS "-DSWIG ${CMAKE_CXX_FLAGS}")
endif ()

set(CMAKE_CXX_FLAGS "${AGRUM_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
