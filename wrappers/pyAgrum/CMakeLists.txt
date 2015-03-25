project(pyAgrum)

cmake_minimum_required(VERSION 2.6)

if(WIN32)
  message(STATUS "** aGrUM notification : Windows detected")
else(WIN32)
  if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    message(STATUS "** aGrUM notification : Apple detected")
  else(APPLE)
    message(STATUS "** aGrUM notification : Linux detected")
  endif(APPLE)
endif(WIN32)

option(FOR_PYTHON2 "ON if pyAgrum will be compiler agains python2. Python3 is uses by default" "OFF")

# PYTHON 2 OR 3
if (FOR_PYTHON2) # we use 3 by default
  set(PYTHONVERSION "2")
  # hackish
  if( WIN32)
	set(PYTHON_EXECUTABLE "python.exe")
  else(WIN32)
	set(PYTHON_EXECUTABLE "python2")
  endif(WIN32)
  
  find_package(PythonInterp REQUIRED)
  set(CMAKE_SWIG_FLAGS  "-Wall" )
  
  #change in module naming from inside module between python2 and python3
  set(PYAGRUM_MODULE "pyAgrum")
else (FOR_PYTHON2) 
  set(PYTHONVERSION "3")
  # hackish
  if(WIN32)
	set(PYTHON_EXECUTABLE "python.exe")
  else(WIN32)
	set(PYTHON_EXECUTABLE "python3")
  endif(WIN32)
  
  find_package(PythonInterp 3 REQUIRED)
  
  set(CMAKE_SWIG_FLAGS  "-Wall" )
  set(CMAKE_SWIG_FLAGS  "${CMAKE_SWIG_FLAGS};-py3" )
  
  #change in module naming from inside module between python2 and python3
  set(PYAGRUM_MODULE ".pyAgrum")
endif (FOR_PYTHON2)
set(GENERATED_PYTHON "${CMAKE_CURRENT_SOURCE_DIR}/generated-files${PYTHONVERSION}")
set(_PYAGRUMLIB "_pyAgrum")



#== Initializing aGrUM
set(AGRUM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
include("${AGRUM_SOURCE_DIR}/version.txt")

#== modules path
list (APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
list (APPEND CMAKE_MODULE_PATH ${AGRUM_SOURCE_DIR}/cmake)


## if you do not want to use swig
set(USE_SWIG true)

if (USE_SWIG)
  find_package(SWIG 3)
  if (SWIG_FOUND)
    message(STATUS "** aGrUM notification : Swig 3 found")
  else ()  
    message(STATUS "** aGrUM notification : Swig 3 not found : you cannot change *.i files !")
  endif (SWIG_FOUND)
else()
  message(STATUS "** aGrUM notification : Swig not used : you cannot change *.i files !")
  set(SWIG_FOUND false)
endif()

if (SWIG_FOUND)
  include(${SWIG_USE_FILE})
  # modification of SWIG_ADD_MODULE to take outdir into account for generated cpp wrapper.
  include("cmake/GUM-UseSWIG.cmake")
endif(SWIG_FOUND)



if (APPLE) # let's assume that PYTHON is there
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print((sysconfig.get_config_vars('prefix'))[0])"
    OUTPUT_VARIABLE PYTHON_LOCAL_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print((sysconfig.get_config_vars('CONFINCLUDEPY'))[0])"
    OUTPUT_VARIABLE PYTHON_INCLUDE_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print((sysconfig.get_config_vars('LIBDIR'))[0])"
    OUTPUT_VARIABLE PYTHON_LIBPATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print((sysconfig.get_config_vars('VERSION'))[0])"
    OUTPUT_VARIABLE PYTHON_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  #todo : test if PYTHON_VERSION>3.2
  set(PYTHON_LIBRARIES "${PYTHON_LIBPATH}/libPython${PYTHON_VERSION}.dylib")
else()
  find_package(PythonLibs ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR} REQUIRED)
endif()


include("Config.agrum")
configure_file("${AGRUM_SOURCE_DIR}/cmake/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/agrum/config.h")

include(openMP)

find_package (CXX11)
if (CXX11_FOUND)
  set (CPLUSPLUSONZE ${CXX11_FLAGS} )
else ()
  message(FATAL_ERROR "aGrUM/pyAgrum is now using C++11. Please find a compiler (for instance GCC) C++11 compliant")
endif ()

execute_process(
  COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_PREFIX}'))"
  OUTPUT_VARIABLE PYTHON_INSTALL
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_version())"
  OUTPUT_VARIABLE PYTHON_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)


execute_process(
  COMMAND ${PYTHON_EXECUTABLE} -c "import distutils.sysconfig; print(distutils.sysconfig.get_python_inc())"
  OUTPUT_VARIABLE PYTHON_INCLUDE_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

message("================================")
message("version : ${PYTHON_VERSION}")  
message("installation : ${PYTHON_INSTALL}")  
message("include : ${PYTHON_INCLUDE_PATH}")
message("================================")


#== Initializing aGrUM
set(AGRUM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
include("${AGRUM_SOURCE_DIR}/version.txt")
include("${AGRUM_SOURCE_DIR}/cmake/Config.agrum.cmake")
configure_file("${AGRUM_SOURCE_DIR}/cmake/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/agrum/config.h")

#==
#== Adding files in build tree
#==
set(SWIG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../swig")
file(GLOB MAINSWIGFILES ${SWIG_SOURCE_DIR}/*.i)
file(GLOB PYAGRUMFILES *.i)
file(GLOB PYAGRUM_EXTENSIONS ${CMAKE_CURRENT_SOURCE_DIR}/extensions/*.h)

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
include_directories(BEFORE ${AGRUM_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/generated_files)
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE AGRUM_CORE_SOURCES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${AGRUM_SOURCE_DIR}/agrum/core/*.cpp)
file(GLOB_RECURSE AGRUM_BN_SOURCES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${AGRUM_SOURCE_DIR}/agrum/BN/*.cpp)
file(GLOB_RECURSE AGRUM_ID_SOURCES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${AGRUM_SOURCE_DIR}/agrum/ID/*.cpp)
file(GLOB_RECURSE AGRUM_CN_SOURCES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${AGRUM_SOURCE_DIR}/agrum/CN/*.cpp)
file(GLOB_RECURSE AGRUM_GRAPHICALMODEL_SOURCES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${AGRUM_SOURCE_DIR}/agrum/graphicalModels/*.cpp)
file(GLOB_RECURSE AGRUM_GRAPHS_SOURCES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${AGRUM_SOURCE_DIR}/agrum/graphs/*.cpp)
file(GLOB_RECURSE AGRUM_MULTIDIM_SOURCES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${AGRUM_SOURCE_DIR}/agrum/multidim/*.cpp)
file(GLOB_RECURSE AGRUM_VARIABLES_SOURCES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${AGRUM_SOURCE_DIR}/agrum/variables/*.cpp)
file(GLOB_RECURSE AGRUM_LEARNING_SOURCES RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${AGRUM_SOURCE_DIR}/agrum/learning/*.cpp)

set(AGRUM_C_SOURCES ${AGRUM_SOURCE_DIR}/agrum/core/math/lrslib/lrslib.c ${AGRUM_SOURCE_DIR}/agrum/core/math/lrslib/lrsmp.c)
set(AGRUM_CPP_SOURCES ${AGRUM_CORE_SOURCES} ${AGRUM_BN_SOURCES} ${AGRUM_ID_SOURCES} ${AGRUM_CN_SOURCES} ${AGRUM_GRAPHICALMODEL_SOURCES} ${AGRUM_GRAPHS_SOURCES} ${AGRUM_MULTIDIM_SOURCES} ${AGRUM_VARIABLES_SOURCES}  ${AGRUM_LEARNING_SOURCES})


set(SWIG_MODULE${_PYAGRUMLIB}_EXTRA_DEPS ${BUILDSWIGFILES})
set(CMAKE_SWIG_OUTDIR ${GENERATED_PYTHON})
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/${SWIGFILE} PROPERTIES SWIG_FLAGS  "-I${AGRUM_SOURCE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/../swig -I${SWIG_SOURCE_DIR}")

set(AGRUM_OPTIMIZATION "-O3 -Wall -Wno-uninitialized -DNDEBUG" )
add_definitions("-Wall")
add_definitions("-pedantic")


set(AGRUM_INLINING_POLICY "")
set(CMAKE_FLAGS_RELEASE "${AGRUM_OPTIMIZATION} ${AGRUM_INLINING_POLICY} -I${AGRUM_SOURCE_DIR} -I${PYTHON_INCLUDE_PATH} -I${CMAKE_CURRENT_SOURCE_DIR}  -I${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_FLAGS_RELEASE} ${CPLUSPLUSONZE}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_FLAGS_RELEASE} -g")

foreach(CPP_FILE ${AGRUM_CPP_SOURCES})
    set_source_files_properties(${CPP_FILE} PROPERTIES COMPILE_FLAGS  ${CMAKE_CXX_FLAGS_RELEASE})
endforeach(CPP_FILE ${AGRUM_CPP_SOURCES})

foreach(C_FILE ${AGRUM_C_SOURCES})
    set_source_files_properties(${C_FILE} PROPERTIES COMPILE_FLAGS ${CMAKE_C_FLAGS_RELEASE})
endforeach(C_FILE ${AGRUM_C_SOURCES})

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/pyAgrum.i PROPERTIES SWIG_FLAGS "-I${SWIG_SOURCE_DIR}")

if (SWIG_FOUND)
  gum_swig_add_module(pyAgrum python ${CMAKE_CURRENT_SOURCE_DIR}/pyAgrum.i ${AGRUM_CPP_SOURCES} ${AGRUM_C_SOURCES})
  swig_link_libraries(pyAgrum ${PYTHON_LIBRARIES})
else (SWIG_FOUND)
  file(GLOB SWIG_GENERATED_FILES "${GENERATED_PYTHON}/*.cxx")
  add_library(${_PYAGRUMLIB}  SHARED ${AGRUM_CPP_SOURCES} ${AGRUM_C_SOURCES} ${SWIG_GENERATED_FILES})
  target_link_libraries(${_PYAGRUMLIB} ${PYTHON_LIBRARIES})
endif (SWIG_FOUND)

set_target_properties(${_PYAGRUMLIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(${_PYAGRUMLIB} PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}")
set_target_properties(${_PYAGRUMLIB} PROPERTIES LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp -lpthread")


# for additionnal rule such test pyAgrum
add_custom_command(TARGET ${_PYAGRUMLIB} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SWIG_OUTDIR}/pyAgrum.py" "${CMAKE_CURRENT_BINARY_DIR}/.")

  set_target_properties(${_PYAGRUMLIB} PROPERTIES PREFIX "")
if (WIN32)
  set_target_properties(${_PYAGRUMLIB} PROPERTIES SUFFIX ".pyd")
else (WIN32)
  set_target_properties(${_PYAGRUMLIB} PROPERTIES SUFFIX ".so")
endif (WIN32)

file(GLOB GUMLIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.py")
file(GLOB GUMLIB_UTILS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/utils/*.py")

include("${CMAKE_CURRENT_SOURCE_DIR}/../../src/version.txt")
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
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${_PYAGRUMLIB}.dll.a DESTINATION ${PYTHON_INSTALL}/pyAgrum)
else (WIN32)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_PYAGRUMLIB}.so DESTINATION ${PYTHON_INSTALL}/pyAgrum)
endif (WIN32)

install(FILES ${GUMLIB_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/lib)
install(FILES ${GUMLIB_UTILS_FILES} DESTINATION ${PYTHON_INSTALL}/pyAgrum/lib/utils)
