############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

if(PYTHON_TARGET)
  set(Python_EXECUTABLE ${PYTHON_TARGET})
  unset(Python_INCLUDE_DIRS CACHE)
  unset(Python_LIBRARIES CACHE)
  unset(Python_VERSION CACHE)
  unset(Python_VERSION_MAJOR CACHE)
endif()
option(AGRUM_PYTHON_SABI "Use Python stable ABI" ON)
option(AGRUM_PYTHON_NOGIL "Use Python GIL Disabled" OFF)

# SABI and NOGIL options are mutually exclusive
if(AGRUM_PYTHON_SABI AND AGRUM_PYTHON_NOGIL)
  message(FATAL_ERROR "AGRUM_PYTHON_SABI and AGRUM_PYTHON_NOGIL are mutually exclusive options. Please set only one of them.")
endif()

if(AGRUM_PYTHON_SABI AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.26)
  find_package(Python ${PYAGRUM_REQUIRED_PYTHON_VERSION} COMPONENTS Interpreter Development.SABIModule NumPy REQUIRED)
else()
  find_package(Python ${PYAGRUM_REQUIRED_PYTHON_VERSION} COMPONENTS Interpreter Development.Module NumPy REQUIRED)
endif()

list(APPEND CMAKE_SWIG_FLAGS "-fastdispatch")

if(AGRUM_PYTHON_SABI)
  set(SWIG_COMPILE_FLAGS "-DPy_LIMITED_API=0x030A0000" CACHE STRING "C++ compiler flags used for wrapper code")
elseif(AGRUM_PYTHON_NOGIL)
  set(SWIG_COMPILE_FLAGS "-DPy_GIL_DISABLED -DPy_NO_ENABLE_SHARED -DPy_NOGIL" CACHE STRING "C++ compiler flags used for wrapper code")
else()
  set(SWIG_COMPILE_FLAGS "" CACHE STRING "C++ compiler flags used for wrapper code")
endif()

if(WIN32)
  set(INSTALL_PYTHONDIR Lib/site-packages CACHE STRING "python site-dir")
else()
  set(INSTALL_PYTHONDIR ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages)
endif()

message(STATUS "================================")
message(STATUS "python version : ${Python_VERSION}")
message(STATUS "python installation : ${INSTALL_PYTHONDIR}")
message(STATUS "python include : ${Python_INCLUDE_DIRS}")
message(STATUS "python library : ${Python_LIBRARIES}")
message(STATUS "================================")

# == Initializing aGrUM
set(AGRUM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
set(AGRUM_FILE_VERSION "${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION.txt")
include(${AGRUM_FILE_VERSION})
include("${AGRUM_SOURCE_DIR}/cmake/Config.agrum.cmake")

# pyAgrum forces STL threads to avoid OpenMP + fork() deadlocks when Python uses multiprocessing
set(GUM_THREADS "stl" CACHE STRING "threads flavor" FORCE)
message(STATUS "pyAgrum: forcing STL threads (OpenMP disabled to prevent fork/OpenMP deadlocks)")
include("${AGRUM_SOURCE_DIR}/cmake/threads.cmake")

configure_file("${AGRUM_SOURCE_DIR}/cmake/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/agrum/config.h")

# ==
# == Adding files in build tree
# ==
set(SWIG_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../swig")
file(GLOB PYAGRUM_EXTENSIONS ${CMAKE_CURRENT_SOURCE_DIR}/extensions/*.h)
file(GLOB PYAGRUMFILES swigsrc/*.i)

# generate swig into /raw then post-process into /generated-files
set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/raw)
set(GENERATED_PYTHON "${CMAKE_CURRENT_SOURCE_DIR}/generated-files")
file(MAKE_DIRECTORY ${CMAKE_SWIG_OUTDIR})

# LTO support — evaluated once at file scope so the result is available inside the macro
include(CheckIPOSupported)
check_ipo_supported(RESULT _AGRUM_IPO_SUPPORTED OUTPUT _AGRUM_IPO_OUTPUT)
if(NOT _AGRUM_IPO_SUPPORTED)
  message(STATUS "pyAgrum: LTO not supported by this toolchain (${_AGRUM_IPO_OUTPUT})")
endif()

# pyAgrum never wraps FMDP in SWIG (independent module, no Python bindings -- see
# src/modules.txt), so it is never linked in below (agrumBASE/agrumBN for core,
# agrum${MODULENAME} for submodules -- see gum_add_python_module). Shared headers
# (extensions/helpers.h) must stay free of other-module types for this to hold --
# a dynamic_cast<ASTjointProba<double>*> (ASTjointProba living in CM) used to leak
# through helpers.h into every module including core, which briefly forced core to
# link the full module set; that helper moved to extensions/helpersCM.h, included
# only from cm.i, so core only ever needs BASE/BN.

macro(gum_add_python_module MODULENAME)
  string(TOLOWER "${MODULENAME}" MODULENAME_LOWER)

  # Optional 2nd arg: install subdirectory under pyagrum/ (e.g. "mrf" for
  # pyagrum.mrf). Defaults to "." (install straight under pyagrum/, today's
  # only case) so a single-arg call keeps installing where it always has.
  # Mirrored by _PYAGRUM_BUILD_SUBDIR below: the .so/.py must land in the
  # same subdirectory in the build tree too, since act test runs pyAgrum
  # straight out of the build tree (PYTHONPATH), not the installed wheel.
  set(_PYAGRUM_INSTALL_DIR "${INSTALL_PYTHONDIR}/pyagrum")
  set(_PYAGRUM_BUILD_SUBDIR "${CMAKE_CURRENT_BINARY_DIR}")
  if(${ARGC} GREATER 1)
    set(_PYAGRUM_INSTALL_DIR "${INSTALL_PYTHONDIR}/pyagrum/${ARGV1}")
    set(_PYAGRUM_BUILD_SUBDIR "${CMAKE_CURRENT_BINARY_DIR}/${ARGV1}")
    file(MAKE_DIRECTORY ${_PYAGRUM_BUILD_SUBDIR})
  endif()

  # Not every module has a generic wrappers/swig/aGrUM_wrap_<MODULENAME>.i
  # layer (PRM's whole API is PRMexplorer.h, included directly) -- only add
  # it to DEPENDS when it actually exists, otherwise Make/Ninja errors with
  # "no rule to make target" for a prerequisite nothing produces.
  set(_PYAGRUM_WRAP_FILE ${SWIG_SOURCE_DIR}/aGrUM_wrap_${MODULENAME}.i)
  if(NOT EXISTS ${_PYAGRUM_WRAP_FILE})
    set(_PYAGRUM_WRAP_FILE "")
  endif()

  if(SWIG_FOUND)
    add_custom_command(OUTPUT ${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}.py
      ${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}PYTHON_wrap.cxx
      ${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}PYTHON_wrap.h
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULENAME_LOWER}.i
      ${_PYAGRUM_WRAP_FILE}
      ${PYAGRUMFILES}
      COMMAND ${SWIG_EXECUTABLE} -c++ -python ${CMAKE_SWIG_FLAGS}
      -outdir ${CMAKE_SWIG_OUTDIR}
      -I${AGRUM_SOURCE_DIR} -I${SWIG_SOURCE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/swigsrc/ -I${CMAKE_CURRENT_SOURCE_DIR}/doc
      -o ${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}PYTHON_wrap.cxx
      ${CMAKE_CURRENT_SOURCE_DIR}/${MODULENAME_LOWER}.i
      COMMENT "SWIG: generating ${MODULENAME} Python bindings"
    )

    # passForPyType post-processes raw/ → generated-files/.
    # OUTPUT is a stamp file (build tree, always touched) so Ninja uses it — not the
    # generated-files/ timestamps — to decide whether to re-run this command.
    # generated-files/ are BYPRODUCTS: Ninja knows they may be written here, but their
    # timestamps do not trigger re-runs. passForPyType only writes them when content
    # changes, so pyagrumPYTHON_wrap.cxx.o is recompiled only when truly necessary.
    set(_PASSFORPYTYPE_STAMP ${CMAKE_CURRENT_BINARY_DIR}/${MODULENAME_LOWER}_passForPyType.stamp)
    add_custom_command(OUTPUT ${_PASSFORPYTYPE_STAMP}
      BYPRODUCTS
        ${GENERATED_PYTHON}/${MODULENAME_LOWER}.py
        ${GENERATED_PYTHON}/${MODULENAME_LOWER}PYTHON_wrap.cxx
        ${GENERATED_PYTHON}/${MODULENAME_LOWER}PYTHON_wrap.h
      DEPENDS
        ${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}.py
        ${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}PYTHON_wrap.cxx
        ${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}PYTHON_wrap.h
      COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/passForPyType.py
              ${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}.py
              ${GENERATED_PYTHON}/${MODULENAME_LOWER}.py
      COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/passForPyType.py
              ${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}PYTHON_wrap.cxx
              ${GENERATED_PYTHON}/${MODULENAME_LOWER}PYTHON_wrap.cxx
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              ${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}PYTHON_wrap.h
              ${GENERATED_PYTHON}/${MODULENAME_LOWER}PYTHON_wrap.h
      COMMAND ${CMAKE_COMMAND} -E touch ${_PASSFORPYTYPE_STAMP}
      COMMENT "passForPyType: post-processing ${MODULENAME} SWIG output"
    )

    # Custom target so the library can declare an explicit ordering dependency on the stamp.
    add_custom_target(${MODULENAME_LOWER}_swig_generated DEPENDS ${_PASSFORPYTYPE_STAMP})
  endif()

  set(_PYAGRUMLIB _${MODULENAME_LOWER})
  # Core "pyagrum" is SHARED, not MODULE: every split submodule (_mrf.so, _id.so,
  # ...) links against it directly to reach BASE/BN (see target_link_libraries
  # below), so it must be a real linkable shared library, not a Python-only
  # loadable bundle. Submodules stay MODULE -- nothing links against them.
  if(MODULENAME_LOWER STREQUAL "pyagrum")
    add_library(${_PYAGRUMLIB} SHARED ${GENERATED_PYTHON}/${MODULENAME_LOWER}PYTHON_wrap.cxx)
  else()
    add_library(${_PYAGRUMLIB} MODULE ${GENERATED_PYTHON}/${MODULENAME_LOWER}PYTHON_wrap.cxx)
  endif()
  set_source_files_properties(${GENERATED_PYTHON}/${MODULENAME_LOWER}PYTHON_wrap.cxx PROPERTIES GENERATED TRUE)
  target_compile_definitions(${_PYAGRUMLIB} PRIVATE SWIG_TYPE_TABLE=pyproba)
  if(SWIG_FOUND)
    add_dependencies(${_PYAGRUMLIB} ${MODULENAME_LOWER}_swig_generated)
  endif()
  set_target_properties(${_PYAGRUMLIB} PROPERTIES PREFIX "")

  if(WIN32)
    set_target_properties(${_PYAGRUMLIB} PROPERTIES SUFFIX ".pyd")
  else()
    set_target_properties(${_PYAGRUMLIB} PROPERTIES SUFFIX ".so")
  endif()

  set_target_properties(${_PYAGRUMLIB} PROPERTIES NO_SONAME ON)

  # BASE/BN are embedded once into core pyagrum (SHARED, see add_library above);
  # every submodule links against that single instance instead of re-embedding
  # its own copy -- this is what keeps process-global state (e.g.
  # CompleteProjectionRegister4MultiDim, used by Tensor::min/max/sum/product) a
  # single shared instance across every pyAgrum SWIG extension.
  if(MODULENAME_LOWER STREQUAL "pyagrum")
    target_link_libraries(${_PYAGRUMLIB} PRIVATE agrumBASE agrumBN)
  else()
    target_link_libraries(${_PYAGRUMLIB} PRIVATE agrum${MODULENAME} _pyagrum)
  endif()
  target_include_directories(${_PYAGRUMLIB} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${Python_NumPy_INCLUDE_DIRS})
  
  if(AGRUM_PYTHON_SABI AND TARGET Python::SABIModule)
    target_link_libraries(${_PYAGRUMLIB} PRIVATE Python::SABIModule)
  else()
    target_link_libraries(${_PYAGRUMLIB} PRIVATE Python::Module)
  endif()

  get_filename_component(PY_LIB_DIR "${Python_LIBRARIES}" DIRECTORY)
  target_link_directories(${_PYAGRUMLIB} PRIVATE ${PY_LIB_DIR})

  # Relative RPATH, not absolute: an absolute ${CMAKE_INSTALL_PREFIX}-based RPATH
  # works for `act install`'s stable prefix but dangles for a wheel build, whose
  # temp CMAKE_INSTALL_PREFIX is deleted right after zipping (reproduced earlier:
  # dlopen looked for libagrumBASE under a /var/folders/.../actXXXXX/lib that no
  # longer existed). Core pyagrum/_pyagrum.so is itself the shared dependency
  # submodules link against (see target_link_libraries above), installed straight
  # into pyagrum/ -- so core pyagrum/_pyagrum.so needs same-dir RPATH (it depends
  # on nothing outside its own directory), submodules (pyagrum/<mod>/_<mod>.so)
  # need one level up to reach it -- this also transparently covers the build tree
  # (CMake's default auto-computed BUILD_RPATH already finds co-located build
  # outputs; INSTALL_RPATH only takes effect once `cmake --install` actually runs).
  if(${ARGC} GREATER 1)
    set(_PYAGRUM_RPATH_UP "/..")
  else()
    set(_PYAGRUM_RPATH_UP "")
  endif()
  if(APPLE)
    set(_PYAGRUM_RPATH "@loader_path${_PYAGRUM_RPATH_UP}")
  else()
    set(_PYAGRUM_RPATH "$ORIGIN${_PYAGRUM_RPATH_UP}")
  endif()
  set_target_properties(${_PYAGRUMLIB} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE INSTALL_RPATH ${_PYAGRUM_RPATH})

  set_target_properties(${_PYAGRUMLIB} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${_PYAGRUM_BUILD_SUBDIR} LIBRARY_OUTPUT_DIRECTORY_RELEASE ${_PYAGRUM_BUILD_SUBDIR}
    RUNTIME_OUTPUT_DIRECTORY ${_PYAGRUM_BUILD_SUBDIR} RUNTIME_OUTPUT_DIRECTORY_RELEASE ${_PYAGRUM_BUILD_SUBDIR})

  if(SWIG_COMPILE_FLAGS)
    target_compile_options(${_PYAGRUMLIB} PRIVATE ${SWIG_COMPILE_FLAGS})
  endif()
  set_target_properties(${_PYAGRUMLIB} PROPERTIES UNITY_BUILD OFF)

  # LTO: eliminates dead code across translation units at link time
  if(_AGRUM_IPO_SUPPORTED)
    set_property(TARGET ${_PYAGRUMLIB} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
  
  # Dead-strip: drop unreachable functions/data at link time. Only safe for leaf
  # modules (nothing links against them): core pyagrum is now a real SHARED
  # library that submodules link against (see add_library above), so dead-strip
  # would drop symbols (e.g. typeinfo for classes only referenced from a
  # submodule, like gum::ASTjointProba<double> from pyagrum.cm) that this
  # translation unit never itself references but external consumers still need.
  #   macOS: -dead_strip works natively on Mach-O without per-section flags
  #   Linux: requires -ffunction-sections/-fdata-sections on all TUs (set in CompilOptions)
  #   MSVC:  /OPT:REF removes unreferenced COMDATs; /OPT:ICF folds identical ones
  if(NOT MODULENAME_LOWER STREQUAL "pyagrum")
    if(APPLE)
      target_link_options(${_PYAGRUMLIB} PRIVATE -Wl,-dead_strip)
    elseif(MSVC)
      target_link_options(${_PYAGRUMLIB} PRIVATE /OPT:REF /OPT:ICF)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
      target_link_options(${_PYAGRUMLIB} PRIVATE -Wl,--gc-sections)
    endif()
  endif()

  install(FILES ${GENERATED_PYTHON}/${MODULENAME_LOWER}.py DESTINATION ${_PYAGRUM_INSTALL_DIR})
  install(TARGETS ${_PYAGRUMLIB} DESTINATION ${_PYAGRUM_INSTALL_DIR})

  # Copy the Python proxy into the build tree (needed for `act test`, which
  # runs pyAgrum straight out of the build tree, not the installed wheel).
  # A plain `add_custom_command(TARGET ... POST_BUILD ...)` (the previous
  # approach) only fires when ${_PYAGRUMLIB} itself gets rebuilt/relinked --
  # a .i change confined to a %pythoncode block updates
  # ${GENERATED_PYTHON}/${MODULENAME_LOWER}.py (via passForPyType) without
  # touching the generated .cxx, so the .so has no reason to relink and the
  # POST_BUILD copy silently never runs, leaving a stale .py in the build
  # tree. An OUTPUT-based command instead depends on the .py file itself,
  # regardless of whether the .so needed rebuilding.
  set(_PY_PROXY_COPY_STAMP ${CMAKE_CURRENT_BINARY_DIR}/${MODULENAME_LOWER}_py_proxy_copy.stamp)
  add_custom_command(OUTPUT ${_PY_PROXY_COPY_STAMP}
    DEPENDS ${GENERATED_PYTHON}/${MODULENAME_LOWER}.py
    COMMAND ${CMAKE_COMMAND} -E copy "${GENERATED_PYTHON}/${MODULENAME_LOWER}.py" "${_PYAGRUM_BUILD_SUBDIR}/."
    COMMAND ${CMAKE_COMMAND} -E touch ${_PY_PROXY_COPY_STAMP}
    COMMENT "copying ${MODULENAME} Python proxy into the build tree"
  )
  add_custom_target(${MODULENAME_LOWER}_py_proxy_copy ALL DEPENDS ${_PY_PROXY_COPY_STAMP})
  if(SWIG_FOUND)
    add_dependencies(${MODULENAME_LOWER}_py_proxy_copy ${MODULENAME_LOWER}_swig_generated)
  endif()

endmacro()

#foreach(MODULE ${LIST_OF_MODULES})
#  if(MODULE MATCHES "FMDP")
#    continue()
#
#  gum_add_python_module(${MODULE})
#endforeach()
gum_add_python_module(pyagrum)


option(PYAGRUM_WITH_MRF "build pyagrum.mrf as a separate extension module" ON)
option(PYAGRUM_WITH_ID "build pyagrum.id as a separate extension module" ON)
option(PYAGRUM_WITH_CN "build pyagrum.cn as a separate extension module" ON)
option(PYAGRUM_WITH_CM "build pyagrum.cm as a separate extension module" ON)
option(PYAGRUM_WITH_PRM "build pyagrum.prm as a separate extension module" ON)

# Static subpackage __init__.py: just re-exports the compiled extension.
# Written directly (no source template needed) and installed from the same path.
#
# gum_add_python_module's 1st arg (MODULENAME) must match the *actual*
# on-disk case of wrappers/swig/aGrUM_wrap_<MODULENAME>.i (BASE, BN, MRF,
# ID, CN, CM -- all uppercase, matching src/modules.txt) -- NOT the lowercase
# Python package name. Passing lowercase here worked by accident on macOS
# (APFS is case-insensitive by default) but breaks the CMake dependency
# tracking on a case-sensitive filesystem (Linux): Make/Ninja would look for
# a literal aGrUM_wrap_mrf.i, find nothing able to produce it, and fail.
foreach(_pysubmod_upper MRF ID CN CM PRM)
  string(TOLOWER ${_pysubmod_upper} _pysubmod)
  if(PYAGRUM_WITH_${_pysubmod_upper} AND BUILD_${_pysubmod_upper})
    gum_add_python_module(${_pysubmod_upper} ${_pysubmod})
    # from .<mod> (the SWIG-generated Python proxy, e.g. mrf.py, which itself
    # does `from . import _mrf`), NOT from ._<mod> (the raw C extension) --
    # mirrors how pyagrum/__init__.py itself does `from .pyagrum import X`.
    #
    # "from .. import *" first: re-exports the core (BASE+BN) namespace so
    # `import pyagrum.mrf as gum` is a drop-in superset of `import pyagrum as
    # gum` -- code that mixes core classes (Tensor, Instantiation, fastBN...)
    # with module-specific ones needs only the one scoped import.
    # __getattr__ fallback: delegate names not covered by "from .. import *"
    # (e.g. getPosterior, only reachable through pyagrum's own lazy
    # __getattr__ shim, see cmake/__init__.in.py) to the parent package,
    # so e.g. `import pyagrum.cm as gum; gum.getPosterior(...)` works without
    # duplicating the lazy-submodule-loading logic here.
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${_pysubmod}/__init__.py "from .. import *\nfrom .${_pysubmod} import *\n\ndef __getattr__(name):\n  import pyagrum as _pyagrum\n  return getattr(_pyagrum, name)\n")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_pysubmod}/__init__.py DESTINATION ${INSTALL_PYTHONDIR}/pyagrum/${_pysubmod})
  endif()
endforeach()

# copy pyLibs at execution time so they are available without a build step.
# Done once, outside gum_add_python_module, so calling the macro for several
# modules later (Phase 2) won't re-copy the same tree once per module.
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs" "${CMAKE_CURRENT_BINARY_DIR}")
foreach(_pylib causal causalEffectEstimation skbn qBNSampling clg ctbn explain)
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/${_pylib}" "${CMAKE_CURRENT_BINARY_DIR}/${_pylib}")
endforeach()

# updating generated cat requirements.txt from pyagrum/requirements.txt and pyAgrum/optional_requirements.txt
set(PYAGRUM_REQUIREMENTS_LIST
  ${CMAKE_CURRENT_SOURCE_DIR}/../../binder
  ${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx
  ${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx/notebooks
  ${CMAKE_CURRENT_SOURCE_DIR}/testunits
)

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt PYAGRUM_REQ_CONTENT)
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/optional_requirements.txt PYAGRUM_OPT_REQ_CONTENT)

foreach(reqdir ${PYAGRUM_REQUIREMENTS_LIST})
  message(STATUS "** act Notification: updating requirements.txt in ${reqdir}")
  file(READ ${reqdir}/specific_requirements.txt PYAGRUM_SPECIFIC_REQ_CONTENT)
  file(WRITE ${reqdir}/requirements.txt "################################\n")
  file(APPEND ${reqdir}/requirements.txt "# generated : do not change it !\n")
  file(APPEND ${reqdir}/requirements.txt "################################\n")
  file(APPEND ${reqdir}/requirements.txt "${PYAGRUM_REQ_CONTENT}\n")
  file(APPEND ${reqdir}/requirements.txt "#### optional requirements #####\n")
  file(APPEND ${reqdir}/requirements.txt "${PYAGRUM_OPT_REQ_CONTENT}\n")
  file(APPEND ${reqdir}/requirements.txt "################################\n")
  file(APPEND ${reqdir}/requirements.txt "${PYAGRUM_SPECIFIC_REQ_CONTENT}\n")
endforeach()
# REQUIREMENTS files updated

file(GLOB GUMPY_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/*.py")
file(GLOB GUMPY_CONFIG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/*.ini")
file(GLOB GUMLIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/lib/*.py")
file(GLOB GUMCAUSAL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/causal/*.py")
file(GLOB GUMCAUSALEFFECT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/causalEffectEstimation/*.py")
file(GLOB GUMSKBN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/skbn/*.py")
file(GLOB GUMQBNSAMPLING_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/qBNSampling/*.py")
file(GLOB GUMCLG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/clg/*.py")
file(GLOB GUMCTBN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/ctbn/*.py")
file(GLOB GUMBNMIXTURE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/bnmixture/*.py")
file(GLOB GUMEXPLAIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/explain/*.py")
file(GLOB GUMEXPLAINNOTEBOOKS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pyLibs/explain/notebook/*.py")

# include("${CMAKE_CURRENT_SOURCE_DIR}/../../VERSION.txt")
set(PYAGRUM_VERSION "${AGRUM_VERSION_MAJOR}.${AGRUM_VERSION_MINOR}.${AGRUM_VERSION_PATCH}")

# temporary deactivation
# if (DEFINED AGRUM_VERSION_TWEAK)
# set(PYAGRUM_VERSION "${PYAGRUM_VERSION}.{$AGRUM_VERSION_TWEAK}")
# endif ()
set(PYAGRUM_DISTDIR "pyagrum-${PYAGRUM_VERSION}.dist-info")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/__init__.in.py" "${CMAKE_CURRENT_BINARY_DIR}/__init__.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/deprecated.in.py" "${CMAKE_CURRENT_BINARY_DIR}/deprecated.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.in.py" "${CMAKE_CURRENT_BINARY_DIR}/common.py")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/METADATA.in" "${CMAKE_CURRENT_BINARY_DIR}/METADATA")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/WHEEL.in" "${CMAKE_CURRENT_BINARY_DIR}/WHEEL")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE.md" "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.md")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSES/LGPL-3.0-or-later.txt" "${CMAKE_CURRENT_BINARY_DIR}/LICENSES/LGPL-3.0-or-later.txt")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSES/MIT.txt" "${CMAKE_CURRENT_BINARY_DIR}/LICENSES/MIT.txt")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/__init__.py DESTINATION ${INSTALL_PYTHONDIR}/pyagrum)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/deprecated.py DESTINATION ${INSTALL_PYTHONDIR}/pyagrum)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/common.py DESTINATION ${INSTALL_PYTHONDIR}/pyagrum)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/METADATA DESTINATION ${INSTALL_PYTHONDIR}/${PYAGRUM_DISTDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/WHEEL DESTINATION ${INSTALL_PYTHONDIR}/${PYAGRUM_DISTDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LICENSE.md DESTINATION ${INSTALL_PYTHONDIR}/${PYAGRUM_DISTDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LICENSES/LGPL-3.0-or-later.txt DESTINATION ${INSTALL_PYTHONDIR}/${PYAGRUM_DISTDIR}/LICENSES)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LICENSES/MIT.txt DESTINATION ${INSTALL_PYTHONDIR}/${PYAGRUM_DISTDIR}/LICENSES)

install(FILES ${GUMPY_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum)
install(FILES ${GUMPY_CONFIG_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum)
install(FILES ${GUMLIB_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum/lib)
install(FILES ${GUMCAUSAL_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum/causal)
install(FILES ${GUMCAUSALEFFECT_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum/causalEffectEstimation)
install(FILES ${GUMCLG_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum/clg)
install(FILES ${GUMCTBN_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum/ctbn)
install(FILES ${GUMSKBN_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum/skbn)
install(FILES ${GUMQBNSAMPLING_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum/qBNSampling)
install(FILES ${GUMBNMIXTURE_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum/bnmixture)
install(FILES ${GUMEXPLAIN_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum/explain)
install(FILES ${GUMEXPLAINNOTEBOOKS_FILES} DESTINATION ${INSTALL_PYTHONDIR}/pyagrum/explain/notebook)


add_test(NAME python_quick COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testunits/gumTest.py -m all -t all)
set_tests_properties(python_quick PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/..")
