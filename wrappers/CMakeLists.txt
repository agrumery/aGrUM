include (TargetLinkLibrariesWithDynamicLookup)

## if you do not want to use swig
option (USE_SWIG "use swig to generate bindings" ON)

if (USE_SWIG)

  find_package(SWIG 4.0)
  if (SWIG_FOUND)
    message(STATUS "** aGrUM notification : Swig 4 found")

    if (POLICY CMP0078)
      # new swig target name
      cmake_policy (SET CMP0078 NEW)
    endif ()

    if (POLICY CMP0086)
      # UseSWIG honors SWIG_MODULE_NAME via -module
      cmake_policy (SET CMP0086 NEW)
    endif ()

    include (${SWIG_USE_FILE})

  else ()
    message(STATUS "** aGrUM notification : Swig 4 not found : you cannot change *.i files !")
  endif ()
else()
  message(STATUS "** aGrUM notification : Swig not used : you cannot change *.i files !")
endif()

if (BUILD_PYTHON)
  add_subdirectory (pyAgrum)
endif ()

if (BUILD_JAVA)
  find_package (JNI)
  if (JNI_FOUND)
    add_subdirectory (jAgrum)
  else ()
    message(STATUS "** aGrUM notification : JNI not found : you cannot compile jAgrum module!")
  endif ()
endif ()
