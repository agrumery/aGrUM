if (POLICY CMP0018)
  # ignore the POSITION_INDEPENDENT_CODE property for all targets and
  # use the modified value of CMAKE_SHARED_LIBRARY_<Lang>_FLAGS for
  # SHARED and MODULE libraries.
  cmake_policy(SET CMP0018 OLD)
endif ()

option(FOR_PYTHON2 "ON if pyAgrum will be compiler agains python2. Python3 is uses by default" "OFF")

if (PYTHON_TARGET)
  set(PYTHON_EXECUTABLE ${PYTHON_TARGET})
  unset(PYTHON_INCLUDE_DIR CACHE)
  unset(PYTHON_LIBRARY CACHE)
  unset(PYTHON_VERSION_STRING CACHE)
  unset(PYTHON_VERSION_MAJOR CACHE)
endif (PYTHON_TARGET)

if (FOR_PYTHON2)
  find_package(PythonInterp 2 REQUIRED)
  find_package(PythonLibs   2 REQUIRED)

  set(PYAGRUM_MODULE "pyAgrum")
  set(PYAGRUM_FUNCTIONS "functions")
else(FOR_PYTHON2)
  find_package(PythonInterp 3 REQUIRED)
  find_package(PythonLibs   3 REQUIRED)

  set(CMAKE_SWIG_FLAGS  "${CMAKE_SWIG_FLAGS};-py3" )

  #change in module naming from inside module between python2 and python3
  set(PYAGRUM_MODULE ".pyAgrum")
  set(PYAGRUM_FUNCTIONS ".functions")
endif(FOR_PYTHON2)

# PYTHON_INSTALL
if (NOT DEFINED PYTHON_INSTALL)
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_PREFIX}'))"
    OUTPUT_VARIABLE PYTHON_INSTALL
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif ()

set(GENERATED_PYTHON "${CMAKE_CURRENT_SOURCE_DIR}/generated-files${PYTHON_VERSION_MAJOR}")
set(_PYAGRUMLIB "_pyAgrum")


message(STATUS "================================")
message(STATUS "python version : ${PYTHON_VERSION_STRING}")
message(STATUS "python installation : ${PYTHON_INSTALL}")
message(STATUS "python include : ${PYTHON_INCLUDE_DIR}")
message(STATUS "python library : ${PYTHON_LIBRARY}")
message(STATUS "================================")


#== Initializing aGrUM
set(AGRUM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
set(AGRUM_FILE_VERSION "${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION.txt")
include(${AGRUM_FILE_VERSION})
include("${AGRUM_SOURCE_DIR}/cmake/Config.agrum.cmake")
configure_file("${AGRUM_SOURCE_DIR}/cmake/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/agrum/config.h")
#file(GLOB_RECURSE AGRUM_SOURCES "${AGRUM_SOURCE_DIR}/agrum/*.cpp"  ${AGRUM_SOURCE_DIR}/agrum/core/math/lrslib/lrslib.c ${AGRUM_SOURCE_DIR}/agrum/core/math/lrslib/lrsmp.c)

#==
#== Adding files in build tree
#==
set(SWIG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../swig")
file(GLOB MAINSWIGFILES ${SWIG_SOURCE_DIR}/*.i)
file(GLOB PYAGRUMFILES *.i)
file(GLOB PYAGRUM_EXTENSIONS ${CMAKE_CURRENT_SOURCE_DIR}/extensions/*.h)

#==
#== Adding agrum sources
#==
#foreach(AGRUMFILE ${AGRUM_SOURCES})
#  set_source_files_properties(${AGRUMFILE} PROPERTIES CPLUSPLUS ON)
#  list(APPEND BUILDSWIGFILES ${AGRUMFILE})
#endforeach(AGRUMFILE ${AGRUM_SOURCES})

foreach(PYFILE ${PYAGRUMFILES})
  set_source_files_properties(${PYFILE} PROPERTIES CPLUSPLUS ON)
  list(APPEND BUILDSWIGFILES ${PYFILE})
endforeach(PYFILE ${PYAGRUMFILES})

foreach(SWIGFILE ${MAINSWIGFILES})
  set_source_files_properties(${SWIGFILE} PROPERTIES CPLUSPLUS ON)
  list(APPEND BUILDSWIGFILES ${SWIGFILE})
endforeach(SWIGFILE ${MAINSWIGFILES})

foreach(EXTFILE ${PYAGRUM_EXTENSIONS})
  set_source_files_properties(${EXTFILE} PROPERTIES CPLUSPLUS ON)
  list(APPEND BUILDSWIGFILES ${EXTFILE})
endforeach(EXTFILE ${PYAGRUM_EXTENSIONS})

#==
#== Adding agrum sources
#==
include_directories (${PYTHON_INCLUDE_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${AGRUM_SOURCE_DIR})
include_directories(BEFORE ${AGRUM_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/generated_files)
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
include_directories (${SWIG_SOURCE_DIR})

set(SWIG_MODULE${_PYAGRUMLIB}_EXTRA_DEPS ${BUILDSWIGFILES})
set(CMAKE_SWIG_OUTDIR ${GENERATED_PYTHON})


if (SWIG_FOUND)
  gum_swig_add_module(pyAgrum python ${CMAKE_CURRENT_SOURCE_DIR}/pyAgrum.i)
  swig_link_libraries(pyAgrum ${LIBAGRUM})
  swig_link_libraries(pyAgrum ${PYTHON_LIBRARY})  #for Apple ?
else (SWIG_FOUND)
  file(GLOB SWIG_GENERATED_FILES "${GENERATED_PYTHON}/*.cxx")
  add_library (${_PYAGRUMLIB} MODULE ${SWIG_GENERATED_FILES})
  target_link_libraries (${_PYAGRUMLIB} ${LIBAGRUM})
  target_link_libraries (${_PYAGRUMLIB} ${PYTHON_LIBRARY})
  set_target_properties(${_PYAGRUMLIB} PROPERTIES PREFIX "")
  set_target_properties(${_PYAGRUMLIB} PROPERTIES LINKER_LANGUAGE CXX)
  if (WIN32)
    set_target_properties(${_PYAGRUMLIB} PROPERTIES SUFFIX ".pyd")
  else (WIN32)
    set_target_properties(${_PYAGRUMLIB} PROPERTIES SUFFIX ".so")
  endif (WIN32)
endif (SWIG_FOUND)

set_target_properties(${_PYAGRUMLIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
target_sources (${_PYAGRUMLIB} PUBLIC ${AGRUM_SOURCES})

# for additionnal rule such test pyAgrum
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SWIG_OUTDIR}/pyAgrum.py" "${CMAKE_CURRENT_BINARY_DIR}/.")


file(GLOB GUMLIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.py")
file(GLOB GUMLIB_UTILS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/_utils/*.py")

#include("${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION.txt")
set(PYAGRUM_VERSION "${AGRUM_VERSION_MAJOR}.${AGRUM_VERSION_MINOR}.${AGRUM_VERSION_PATCH}")
set(PYAGRUM_EGGFILE "pyAgrum-${PYAGRUM_VERSION}-py${PYTHON_VERSION_MAJOR}.egg-info")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/__init__.in.py" "${CMAKE_CURRENT_BINARY_DIR}/__init__.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/functions.in.py" "${CMAKE_CURRENT_BINARY_DIR}/functions.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/egg-info.in" "${CMAKE_CURRENT_BINARY_DIR}/${PYAGRUM_EGGFILE}")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/functions.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PYAGRUM_EGGFILE} DESTINATION ${PYTHON_INSTALL})
install(FILES ${GENERATED_PYTHON}/pyAgrum.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)

install(TARGETS ${_PYAGRUMLIB} DESTINATION ${PYTHON_INSTALL}/pyAgrum)

install(FILES ${GUMLIB_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/lib)
install(FILES ${GUMLIB_UTILS_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/lib/_utils)

# # show all cmake variables
# get_cmake_property(_variableNames VARIABLES)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
