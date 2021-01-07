
if (PYTHON_TARGET)
  set(PYTHON_EXECUTABLE ${PYTHON_TARGET})
  unset(PYTHON_INCLUDE_DIRS CACHE)
  unset(PYTHON_LIBRARIES CACHE)
  unset(PYTHON_VERSION_STRING CACHE)
  unset(PYTHON_VERSION_MAJOR CACHE)
endif (PYTHON_TARGET)

find_package (PythonInterp ${PYAGRUM_REQUIRED_PYTHON_VERSION} REQUIRED)
find_package (PythonLibs   ${PYTHON_VERSION_STRING} EXACT  REQUIRED)
# for linux wheels only 
#set(PYTHON_INCLUDE_DIR "@PYTHON_INCLUDE_PATH@")

if (PYTHON_VERSION_MAJOR VERSION_GREATER 2)
  list (APPEND CMAKE_SWIG_FLAGS "-py3")
endif ()
add_definitions ("-DSWIG_TYPE_TABLE=pyproba")

# PYTHON_INSTALL
if (NOT DEFINED PYTHON_INSTALL)
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_PREFIX}'))"
    OUTPUT_VARIABLE PYTHON_INSTALL
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif ()

set(GENERATED_PYTHON "${CMAKE_CURRENT_SOURCE_DIR}/generated-files${PYTHON_VERSION_MAJOR}")


message(STATUS "================================")
message(STATUS "python version : ${PYTHON_VERSION_STRING}")
message(STATUS "python installation : ${PYTHON_INSTALL}")
message(STATUS "python include : ${PYTHON_INCLUDE_DIR}")
message(STATUS "python library : ${PYTHON_LIBRARY}")
message(STATUS "================================")
link_directories( ${PYTHON_LIBRARY})

#== Initializing aGrUM
set(AGRUM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
set(AGRUM_FILE_VERSION "${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION.txt")
include(${AGRUM_FILE_VERSION})
include("${AGRUM_SOURCE_DIR}/cmake/Config.agrum.cmake")
configure_file("${AGRUM_SOURCE_DIR}/cmake/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/agrum/config.h")
#file(GLOB_RECURSE AGRUM_SOURCES "${AGRUM_SOURCE_DIR}/agrum/*.cpp"  ${AGRUM_SOURCE_DIR}/agrum/external/lrslib/lrslib.c ${AGRUM_SOURCE_DIR}/agrum/external/lrslib/lrsmp.c)

#==
#== Adding files in build tree
#==
set(SWIG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../swig")
file(GLOB PYAGRUM_EXTENSIONS ${CMAKE_CURRENT_SOURCE_DIR}/extensions/*.h)
file(GLOB MAINSWIGFILES ${SWIG_SOURCE_DIR}/*.i)
file(GLOB PYAGRUMFILES swigsrc/*.i)
list(APPEND MAINSWIGFILES pyAgrum.i)

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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/swigsrc)
include_directories(${AGRUM_SOURCE_DIR})
#inutile ? 
#include_directories(BEFORE ${AGRUM_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/generated_files)
include_directories(BEFORE ${AGRUM_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/doc)
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
include_directories (${SWIG_SOURCE_DIR})

set(CMAKE_SWIG_OUTDIR ${GENERATED_PYTHON})

if (SWIG_FOUND)
  set(_PYAGRUMLIB "pyAgrum")
  set(SWIG_MODULE_${_PYAGRUMLIB}_EXTRA_DEPS ${BUILDSWIGFILES})
  if (CMAKE_VERSION VERSION_LESS 3.8)
    swig_add_module (pyAgrum python ${CMAKE_CURRENT_SOURCE_DIR}/pyAgrum.i)
  else ()
    swig_add_library (pyAgrum LANGUAGE python SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/pyAgrum.i)
  endif ()
  if (CMAKE_VERSION VERSION_LESS 3.13)
    set(_PYAGRUMLIB ${SWIG_MODULE_pyAgrum_REAL_NAME})
  else ()
    set(_PYAGRUMLIB "pyAgrum")
  endif ()
else (SWIG_FOUND)
  set(_PYAGRUMLIB "_pyAgrum")
  file(GLOB SWIG_GENERATED_FILES "${GENERATED_PYTHON}/*.cxx")
  add_library (${_PYAGRUMLIB} MODULE ${SWIG_GENERATED_FILES})
  set_target_properties(${_PYAGRUMLIB} PROPERTIES PREFIX "")
  set_target_properties(${_PYAGRUMLIB} PROPERTIES LINKER_LANGUAGE CXX)
  if (WIN32)
    set_target_properties(${_PYAGRUMLIB} PROPERTIES SUFFIX ".pyd")
  else (WIN32)
    set_target_properties(${_PYAGRUMLIB} PROPERTIES SUFFIX ".so")
  endif (WIN32)
endif (SWIG_FOUND)


target_link_libraries (${_PYAGRUMLIB} ${LIBAGRUM})
target_link_libraries_with_dynamic_lookup (${_PYAGRUMLIB} ${PYTHON_LIBRARIES})

set_target_properties(${_PYAGRUMLIB} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})

set_target_properties(${_PYAGRUMLIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
target_sources (${_PYAGRUMLIB} PUBLIC ${AGRUM_SOURCES})

# for additionnal rule such test pyAgrum
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SWIG_OUTDIR}/pyAgrum.py" "${CMAKE_CURRENT_BINARY_DIR}/.")
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs" "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/causal" "${CMAKE_CURRENT_BINARY_DIR}/causal")
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/skbn" "${CMAKE_CURRENT_BINARY_DIR}/skbn")

# updating generated cat requirements.txt from pyAgrum/requirements.txt and pyAgrum/optional_requirements.txt
set(PYAGRUM_REQUIREMENTS_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/../../binder
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx
        ${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/notebooks
        )

FILE(READ ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt PYAGRUM_REQ_CONTENT)
FILE(READ ${CMAKE_CURRENT_SOURCE_DIR}/optional_requirements.txt PYAGRUM_OPT_REQ_CONTENT)

FOREACH(reqdir ${PYAGRUM_REQUIREMENTS_LIST})
  message(STATUS "** act Notification: updating requirements.txt in ${reqdir}")
  FILE(READ ${reqdir}/specific_requirements.txt PYAGRUM_SPECIFIC_REQ_CONTENT)
  FILE(WRITE  ${reqdir}/requirements.txt "################################\n")
  FILE(APPEND ${reqdir}/requirements.txt "# generated : do not change it !\n")
  FILE(APPEND ${reqdir}/requirements.txt "################################\n")
  FILE(APPEND ${reqdir}/requirements.txt "${PYAGRUM_REQ_CONTENT}\n")
  FILE(APPEND ${reqdir}/requirements.txt "#### optional requirements #####\n")
  FILE(APPEND ${reqdir}/requirements.txt "${PYAGRUM_OPT_REQ_CONTENT}\n")
  FILE(APPEND ${reqdir}/requirements.txt "################################\n")
  FILE(APPEND ${reqdir}/requirements.txt "${PYAGRUM_SPECIFIC_REQ_CONTENT}\n")
ENDFOREACH()
# REQUIRMENTS files updated


file(GLOB GUMPY_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/*.py")
file(GLOB GUMPY_CONFIG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/*.ini")
file(GLOB GUMLIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/lib/*.py")
file(GLOB GUMLIB_UTILS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/lib/_utils/*.py")
file(GLOB GUMCAUSAL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/causal/*.py")
file(GLOB GUMSKBN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/skbn/*.py")

#include("${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION.txt")
set(PYAGRUM_VERSION "${AGRUM_VERSION_MAJOR}.${AGRUM_VERSION_MINOR}.${AGRUM_VERSION_PATCH}")
if (DEFINED AGRUM_VERSION_TWEAK)
  set(PAGRUM_VERSION "${PYAGRUM_VERSION}.{$AGRUM_VERSION_TWEAK}")
endif ()

set(PYAGRUM_EGGFILE "pyAgrum-${PYAGRUM_VERSION}-py${PYTHON_VERSION_MAJOR}.egg-info")
set(PYAGRUM_DISTDIR "pyAgrum-${PYAGRUM_VERSION}.dist-info")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/__init__.in.py" "${CMAKE_CURRENT_BINARY_DIR}/__init__.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/deprecated.in.py" "${CMAKE_CURRENT_BINARY_DIR}/deprecated.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/egg-info.in" "${CMAKE_CURRENT_BINARY_DIR}/${PYAGRUM_EGGFILE}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/METADATA.in" "${CMAKE_CURRENT_BINARY_DIR}/METADATA")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/WHEEL.in" "${CMAKE_CURRENT_BINARY_DIR}/WHEEL")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/deprecated.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PYAGRUM_EGGFILE} DESTINATION ${PYTHON_INSTALL})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/METADATA DESTINATION ${PYTHON_INSTALL}/${PYAGRUM_DISTDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/WHEEL DESTINATION ${PYTHON_INSTALL}/${PYAGRUM_DISTDIR})
install(FILES ${GENERATED_PYTHON}/pyAgrum.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)

install(TARGETS ${_PYAGRUMLIB} DESTINATION ${PYTHON_INSTALL}/pyAgrum)


install(FILES ${GUMPY_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${GUMPY_CONFIG_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${GUMLIB_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/lib)
install(FILES ${GUMLIB_UTILS_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/lib/_utils)
if (NOT PYTHON_VERSION_STRING VERSION_LESS 3.7)
  install(FILES ${GUMCAUSAL_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/causal)
endif ()
install(FILES ${GUMSKBN_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/skbn)

# # show all cmake variables
# get_cmake_property(_variableNames VARIABLES)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
