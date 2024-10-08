if(PYTHON_TARGET)
  set(Python_EXECUTABLE ${PYTHON_TARGET})
  unset(Python_INCLUDE_DIRS CACHE)
  unset(Python_LIBRARIES CACHE)
  unset(Python_VERSION CACHE)
  unset(Python_VERSION_MAJOR CACHE)
endif(PYTHON_TARGET)

if(CMAKE_VERSION VERSION_LESS 3.24)
  find_package(Python ${PYAGRUM_REQUIRED_PYTHON_VERSION} COMPONENTS Interpreter Development REQUIRED)
else()
  find_package(Python ${PYAGRUM_REQUIRED_PYTHON_VERSION} COMPONENTS Interpreter Development.Module REQUIRED)
endif()

# list (APPEND CMAKE_SWIG_FLAGS "-py3")
list(APPEND CMAKE_SWIG_FLAGS "-fastdispatch")
list(APPEND CMAKE_SWIG_FLAGS "-module")
list(APPEND CMAKE_SWIG_FLAGS "rawPyAgrum")

add_definitions("-DSWIG_TYPE_TABLE=pyproba")

# PYTHON_INSTALL
if(NOT DEFINED PYTHON_INSTALL AND NOT CMAKE_CROSSCOMPILING)
  execute_process(COMMAND ${Python_EXECUTABLE} -c "import sysconfig, os; print(sysconfig.get_path('platlib').replace(sysconfig.get_path('data'), '').lstrip(os.path.sep))"
    OUTPUT_VARIABLE PYTHON_INSTALL OUTPUT_STRIP_TRAILING_WHITESPACE)
  file(TO_CMAKE_PATH "${PYTHON_INSTALL}" PYTHON_INSTALL)
endif()

if(NOT DEFINED PYTHON_INSTALL)
  if(WIN32)
    set(PYTHON_INSTALL Lib/site-packages)
  else()
    set(PYTHON_INSTALL ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages)
  endif()
endif()

if(NOT DEFINED PYTHON_EXTENSION_MODULE_SUFFIX AND NOT CMAKE_CROSSCOMPILING)
  execute_process(COMMAND ${Python_EXECUTABLE} -c "import importlib.machinery; print(importlib.machinery.EXTENSION_SUFFIXES[0])"
    OUTPUT_VARIABLE PYTHON_EXTENSION_MODULE_SUFFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

set(GENERATED_PYTHON "${CMAKE_CURRENT_SOURCE_DIR}/generated-files")

message(STATUS "================================")
message(STATUS "python version : ${Python_VERSION}")
message(STATUS "python installation : ${PYTHON_INSTALL}")
message(STATUS "python include : ${Python_INCLUDE_DIRS}")
message(STATUS "python library : ${Python_LIBRARIES}")
message(STATUS "================================")
link_directories(${Python_LIBRARY})

# == Initializing aGrUM
set(AGRUM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
set(AGRUM_FILE_VERSION "${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION.txt")
include(${AGRUM_FILE_VERSION})
include("${AGRUM_SOURCE_DIR}/cmake/Config.agrum.cmake")
configure_file("${AGRUM_SOURCE_DIR}/cmake/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/agrum/config.h")

# file(GLOB_RECURSE AGRUM_SOURCES "${AGRUM_SOURCE_DIR}/agrum/*.cpp"  ${AGRUM_SOURCE_DIR}/agrum/external/lrslib/lrslib.c ${AGRUM_SOURCE_DIR}/agrum/external/lrslib/lrsmp.c)

# ==
# == Adding files in build tree
# ==
set(SWIG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../swig")
file(GLOB PYAGRUM_EXTENSIONS ${CMAKE_CURRENT_SOURCE_DIR}/extensions/*.h)
file(GLOB MAINSWIGFILES ${SWIG_SOURCE_DIR}/*.i)
file(GLOB PYAGRUMFILES swigsrc/*.i)
list(APPEND MAINSWIGFILES pyAgrum.i)

# ==
# == Adding agrum sources
# ==
# foreach(AGRUMFILE ${AGRUM_SOURCES})
# set_source_files_properties(${AGRUMFILE} PROPERTIES CPLUSPLUS ON)
# list(APPEND BUILDSWIGFILES ${AGRUMFILE})
# endforeach(AGRUMFILE ${AGRUM_SOURCES})
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

# ==
# == Adding agrum sources
# ==
include_directories(${Python_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/swigsrc)
include_directories(${AGRUM_SOURCE_DIR})

# inutile ?
# include_directories(BEFORE ${AGRUM_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/generated_files)
include_directories(BEFORE ${AGRUM_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/doc)
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${SWIG_SOURCE_DIR})

set(CMAKE_SWIG_OUTDIR ${GENERATED_PYTHON})

if(SWIG_FOUND)
  set(_PYAGRUMLIB "pyAgrum")
  set(SWIG_MODULE_${_PYAGRUMLIB}_EXTRA_DEPS ${BUILDSWIGFILES})
  swig_add_library(pyAgrum LANGUAGE python SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/pyAgrum.i)

  if(CMAKE_VERSION VERSION_LESS 3.13)
    set(_PYAGRUMLIB ${SWIG_MODULE_pyAgrum_REAL_NAME})
  else()
    set(_PYAGRUMLIB "pyAgrum")
  endif()

  add_custom_command(OUTPUT ${CMAKE_SWIG_OUTDIR}/pyAgrum.py DEPENDS ${CMAKE_SWIG_OUTDIR}/rawPyAgrum.py COMMAND ${Python_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/passForPyType.py" "${CMAKE_SWIG_OUTDIR}/rawPyAgrum.py" "${CMAKE_SWIG_OUTDIR}/pyAgrum.py")
else(SWIG_FOUND)
  set(_PYAGRUMLIB "_pyAgrum")
  file(GLOB SWIG_GENERATED_FILES "${GENERATED_PYTHON}/*.cxx")
  add_library(${_PYAGRUMLIB} MODULE ${SWIG_GENERATED_FILES})
  set_target_properties(${_PYAGRUMLIB} PROPERTIES PREFIX "")
  set_target_properties(${_PYAGRUMLIB} PROPERTIES LINKER_LANGUAGE CXX)

  if(WIN32)
    set_target_properties(${_PYAGRUMLIB} PROPERTIES SUFFIX ".pyd")
  else(WIN32)
    set_target_properties(${_PYAGRUMLIB} PROPERTIES SUFFIX ".so")
  endif(WIN32)
endif(SWIG_FOUND)

target_link_libraries(${_PYAGRUMLIB} ${LIBAGRUM})

if(TARGET Python::Module)
  target_link_libraries(${_PYAGRUMLIB} Python::Module)
else()
  include(TargetLinkLibrariesWithDynamicLookup)
  target_link_libraries_with_dynamic_lookup(${_PYAGRUMLIB} ${Python_LIBRARIES})
endif()

set_target_properties(${_PYAGRUMLIB} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

if(DEFINED PYTHON_EXTENSION_MODULE_SUFFIX)
  set_target_properties(${_PYAGRUMLIB} PROPERTIES SUFFIX "${PYTHON_EXTENSION_MODULE_SUFFIX}")
endif()

set_target_properties(${_PYAGRUMLIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
target_sources(${_PYAGRUMLIB} PUBLIC ${AGRUM_SOURCES})

# for additionnal rule such test pyAgrum
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SWIG_OUTDIR}/pyAgrum.py" "${CMAKE_CURRENT_BINARY_DIR}/.")
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs" "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/causal" "${CMAKE_CURRENT_BINARY_DIR}/causal")
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/causal/causalEffectEstimation" "${CMAKE_CURRENT_BINARY_DIR}/causal/causalEffectEstimation")
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/skbn" "${CMAKE_CURRENT_BINARY_DIR}/skbn")
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/clg" "${CMAKE_CURRENT_BINARY_DIR}/clg")
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/ctbn" "${CMAKE_CURRENT_BINARY_DIR}/ctbn")

# updating generated cat requirements.txt from pyAgrum/requirements.txt and pyAgrum/optional_requirements.txt
set(PYAGRUM_REQUIREMENTS_LIST
  ${CMAKE_CURRENT_SOURCE_DIR}/../../binder
  ${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx
  ${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx/notebooks
  ${CMAKE_CURRENT_SOURCE_DIR}/testunits
)

FILE(READ ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt PYAGRUM_REQ_CONTENT)
FILE(READ ${CMAKE_CURRENT_SOURCE_DIR}/optional_requirements.txt PYAGRUM_OPT_REQ_CONTENT)

FOREACH(reqdir ${PYAGRUM_REQUIREMENTS_LIST})
  message(STATUS "** act Notification: updating requirements.txt in ${reqdir}")
  FILE(READ ${reqdir}/specific_requirements.txt PYAGRUM_SPECIFIC_REQ_CONTENT)
  FILE(WRITE ${reqdir}/requirements.txt "################################\n")
  FILE(APPEND ${reqdir}/requirements.txt "# generated : do not change it !\n")
  FILE(APPEND ${reqdir}/requirements.txt "################################\n")
  FILE(APPEND ${reqdir}/requirements.txt "${PYAGRUM_REQ_CONTENT}\n")
  FILE(APPEND ${reqdir}/requirements.txt "#### optional requirements #####\n")
  FILE(APPEND ${reqdir}/requirements.txt "${PYAGRUM_OPT_REQ_CONTENT}\n")
  FILE(APPEND ${reqdir}/requirements.txt "################################\n")
  FILE(APPEND ${reqdir}/requirements.txt "${PYAGRUM_SPECIFIC_REQ_CONTENT}\n")
ENDFOREACH()

# REQUIREMENTS files updated
file(GLOB GUMPY_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/*.py")
file(GLOB GUMPY_CONFIG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/*.ini")
file(GLOB GUMLIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/lib/*.py")
file(GLOB GUMCAUSAL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/causal/*.py")
file(GLOB GUMCAUSALEFFECT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/causal/causalEffectEstimation/*.py")
file(GLOB GUMSKBN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/skbn/*.py")
file(GLOB GUMCLG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/clg/*.py")
file(GLOB GUMCTBN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/ctbn/*.py")

# include("${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION.txt")
set(PYAGRUM_VERSION "${AGRUM_VERSION_MAJOR}.${AGRUM_VERSION_MINOR}.${AGRUM_VERSION_PATCH}")

# temporary deactivation
# if (DEFINED AGRUM_VERSION_TWEAK)
# set(PYAGRUM_VERSION "${PYAGRUM_VERSION}.{$AGRUM_VERSION_TWEAK}")
# endif ()
set(PYAGRUM_DISTDIR "pyAgrum-${PYAGRUM_VERSION}.dist-info")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/__init__.in.py" "${CMAKE_CURRENT_BINARY_DIR}/__init__.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/deprecated.in.py" "${CMAKE_CURRENT_BINARY_DIR}/deprecated.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/METADATA.in" "${CMAKE_CURRENT_BINARY_DIR}/METADATA")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/WHEEL.in" "${CMAKE_CURRENT_BINARY_DIR}/WHEEL")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/deprecated.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/METADATA DESTINATION ${PYTHON_INSTALL}/${PYAGRUM_DISTDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/WHEEL DESTINATION ${PYTHON_INSTALL}/${PYAGRUM_DISTDIR})
install(FILES ${GENERATED_PYTHON}/pyAgrum.py DESTINATION ${PYTHON_INSTALL}/pyAgrum)

install(TARGETS ${_PYAGRUMLIB} DESTINATION ${PYTHON_INSTALL}/pyAgrum)

install(FILES ${GUMPY_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${GUMPY_CONFIG_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum)
install(FILES ${GUMLIB_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/lib)
install(FILES ${GUMCAUSAL_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/causal)
install(FILES ${GUMCAUSALEFFECT_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/causal/causalEffectEstimation)
install(FILES ${GUMCLG_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/clg)
install(FILES ${GUMCTBN_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/ctbn)
install(FILES ${GUMSKBN_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/skbn)

# # show all cmake variables
# get_cmake_property(_variableNames VARIABLES)
# foreach (_variableName ${_variableNames})
# message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
