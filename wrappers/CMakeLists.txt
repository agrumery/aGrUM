## if you do not want to use swig
option (USE_SWIG "use swig to generate bindings" ON)

if (USE_SWIG)
  find_package(SWIG 3)
  if (SWIG_FOUND)
    message(STATUS "** aGrUM notification : Swig 3 found")
    include(${SWIG_USE_FILE})

    # modification of SWIG_ADD_MODULE to take outdir into account for generated cpp wrapper.
    include (${CMAKE_CURRENT_SOURCE_DIR}/pyAgrum/cmake/GUM-UseSWIG.cmake)
  else ()
    message(STATUS "** aGrUM notification : Swig 3 not found : you cannot change *.i files !")
  endif ()
else()
  message(STATUS "** aGrUM notification : Swig not used : you cannot change *.i files !")
endif()

option (BUILD_PYTHON "python bindings" ON)
if (BUILD_PYTHON)
  find_package (PythonLibs)
  find_package (PythonInterp)
  if (PYTHON_EXECUTABLE AND PYTHONLIBS_FOUND)
    add_subdirectory (pyAgrum)
  endif ()
endif ()

## Very experimental for now.
option (BUILD_JAVA "java bindings" OFF)
if (BUILD_JAVA)
  find_package (JNI)
  if (JNI_FOUND)
    add_subdirectory (jAgrum)
  endif ()
endif ()
