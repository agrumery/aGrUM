if (POLICY CMP0018)
  # ignore the POSITION_INDEPENDENT_CODE property for all targets and
  # use the modified value of CMAKE_SHARED_LIBRARY_<Lang>_FLAGS for
  # SHARED and MODULE libraries.
  cmake_policy(SET CMP0018 OLD)
endif ()

option(FOR_PYTHON2 "ON if pyAgrum will be compiler agains python2. Python3 is uses by default" "OFF")

# PYTHON 2 OR 3
if (FOR_PYTHON2) # we use 3 by default
  # hackish
  if( WIN32)
    find_program(PYTHON_EXECUTABLE NAMES "python.exe")
  else(WIN32)
    if (APPLE)
      set(PYTHON_EXECUTABLE "/usr/local/bin/python2")
    else(APPLE)
      set(PYTHON_EXECUTABLE "/usr/bin/python2")
    endif(APPLE)
	  find_package(PythonInterp 2 REQUIRED)
  endif(WIN32)
else (FOR_PYTHON2)
  # hackish
  if(WIN32)
    find_program(PYTHON_EXECUTABLE NAMES "python.exe")
  else(WIN32)
    if (APPLE)
      set(PYTHON_EXECUTABLE "/usr/local/bin/python3")
    else(APPLE)
      set(PYTHON_EXECUTABLE "/usr/bin/python3")
    endif(APPLE)
	  find_package(PythonInterp 3 REQUIRED)
  endif(WIN32)
endif (FOR_PYTHON2)


execute_process(
  COMMAND ${PYTHON_EXECUTABLE} -c "import distutils.sysconfig; print(distutils.sysconfig.get_python_inc())"
  OUTPUT_VARIABLE PYTHON_INCLUDE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print((sysconfig.get_config_vars('VERSION')[0])[0:1])"
  OUTPUT_VARIABLE PYTHON_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_PREFIX}'))"
  OUTPUT_VARIABLE PYTHON_INSTALL
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_config_vars('VERSION')[0])"
  OUTPUT_VARIABLE PYTHON_COMPLETE_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# maybe something to do for MAC ???
 if (APPLE) # let's assume that PYTHON is there
   execute_process(
     COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print((sysconfig.get_config_vars('prefix'))[0])"
     OUTPUT_VARIABLE PYTHON_LOCAL_PATH
     OUTPUT_STRIP_TRAILING_WHITESPACE
   )
   execute_process(
     COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print((sysconfig.get_config_vars('LIBDIR'))[0])"
     OUTPUT_VARIABLE PYTHON_LIBPATH
     OUTPUT_STRIP_TRAILING_WHITESPACE
   )

  set(PYTHON_LIBRARY "${PYTHON_LIBPATH}/libPython${PYTHON_COMPLETE_VERSION}.dylib")
  set(PYTHONLIBS_VERSION_STRING ${PYTHON_COMPLETE_VERSION})
 else()
   if (WIN32)
	   execute_process(
		 COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print((sysconfig.get_config_vars('BINDIR'))[0])"
		 OUTPUT_VARIABLE PYTHON_LOCAL_PATH
		 OUTPUT_STRIP_TRAILING_WHITESPACE
	   )
	   execute_process(
		 COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print((sysconfig.get_config_vars('BINDIR'))[0])"
		 OUTPUT_VARIABLE PYTHON_LIBPATH
		 OUTPUT_STRIP_TRAILING_WHITESPACE
	   )
	  set(PYTHON_LIBRARY "${PYTHON_LOCAL_PATH}/libs/python${PYTHON_COMPLETE_VERSION}.lib")
    set(PYTHONLIBS_VERSION_STRING ${PYTHON_COMPLETE_VERSION})
   else()
	find_package(PythonLibs ${PYTHON_COMPLETE_VERSION} REQUIRED)
   endif()
 endif()

if (${PYTHON_VERSION} EQUAL "2")
  #change in module naming from inside module between python2 and python3
  set(PYAGRUM_MODULE "pyAgrum")
  set(PYAGRUM_FUNCTIONS "functions")
else ()
  set(CMAKE_SWIG_FLAGS  "${CMAKE_SWIG_FLAGS};-py3" )

  #change in module naming from inside module between python2 and python3
  set(PYAGRUM_MODULE ".pyAgrum")
  set(PYAGRUM_FUNCTIONS ".functions")
endif ()

set(GENERATED_PYTHON "${CMAKE_CURRENT_SOURCE_DIR}/generated-files${PYTHON_VERSION}")
set(_PYAGRUMLIB "_pyAgrum")


message(STATUS "================================")
message(STATUS "python version : ${PYTHON_COMPLETE_VERSION}")
message(STATUS "python installation : ${PYTHON_INSTALL}")
message(STATUS "python include : ${PYTHON_INCLUDE_DIR}")
message(STATUS "python library : ${PYTHON_LIBRARY}")
#message(STATUS "libagrum : ${LIBAGRUM}")
message(STATUS "================================")


#== Initializing aGrUM
set(AGRUM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
set(AGRUM_FILE_VERSION "${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION.txt")
include(${AGRUM_FILE_VERSION})
include("${AGRUM_SOURCE_DIR}/cmake/Config.agrum.cmake")
configure_file("${AGRUM_SOURCE_DIR}/cmake/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/agrum/config.h")
file(GLOB_RECURSE AGRUM_SOURCES "${AGRUM_SOURCE_DIR}/agrum/*.cpp"  ${AGRUM_SOURCE_DIR}/agrum/core/math/lrslib/lrslib.c ${AGRUM_SOURCE_DIR}/agrum/core/math/lrslib/lrsmp.c)

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
foreach(AGRUMFILE ${AGRUM_SOURCES})
  set_source_files_properties(${AGRUMFILE} PROPERTIES CPLUSPLUS ON)
  list(APPEND BUILDSWIGFILES ${AGRUMFILE})
endforeach(AGRUMFILE ${AGRUM_SOURCES})

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
  #swig_link_libraries(pyAgrum ${LIBAGRUM})
  swig_link_libraries(pyAgrum ${PYTHON_LIBRARY})  #for Apple ?
else (SWIG_FOUND)
  file(GLOB SWIG_GENERATED_FILES "${GENERATED_PYTHON}/*.cxx")
  add_library (${_PYAGRUMLIB} MODULE ${SWIG_GENERATED_FILES})
  #target_link_libraries (${_PYAGRUMLIB} ${LIBAGRUM})
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
set(PYAGRUM_EGGFILE "pyAgrum-${PYAGRUM_VERSION}-py${PYTHON_VERSION}.egg-info")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/__init__.in.py" "${CMAKE_CURRENT_BINARY_DIR}/__init__.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/functions.in.py" "${CMAKE_CURRENT_BINARY_DIR}/functions.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/egg-info.in" "${CMAKE_CURRENT_BINARY_DIR}/${PYAGRUM_EGGFILE}")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/functions.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PYAGRUM_EGGFILE} DESTINATION ${PYTHON_INSTALL})
install(FILES ${GENERATED_PYTHON}/pyAgrum.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)

if (WIN32)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_PYAGRUMLIB}.pyd DESTINATION ${PYTHON_INSTALL}/pyAgrum)
else (WIN32)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_PYAGRUMLIB}.so DESTINATION ${PYTHON_INSTALL}/pyAgrum)
endif (WIN32)

install(FILES ${GUMLIB_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/lib)
install(FILES ${GUMLIB_UTILS_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/lib/_utils)
