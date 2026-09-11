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
  if(SWIG_FOUND)
    # OBJECT_DEPENDS hardens the compile step itself (not just the target-level
    # ordering from add_dependencies below) against a build tree that survives
    # partially between CI runs: MSBuild/VS's own up-to-date check for a
    # generated source only looks at that source file's own mtime, and can
    # decide the .obj is current even though the SWIG/passForPyType custom
    # commands were supposed to (re)write it -- e.g. if copy_if_different
    # skips a write because content is unchanged while raw/ or the stamp
    # advanced, or the tree was only partially cleaned. Depending explicitly
    # on the stamp and the raw SWIG outputs forces MSBuild/Ninja to recheck
    # the compile whenever those actually change, closing that skip window.
    set_source_files_properties(${GENERATED_PYTHON}/${MODULENAME_LOWER}PYTHON_wrap.cxx PROPERTIES
      GENERATED TRUE
      OBJECT_DEPENDS "${_PASSFORPYTYPE_STAMP};${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}PYTHON_wrap.cxx;${CMAKE_SWIG_OUTDIR}/${MODULENAME_LOWER}PYTHON_wrap.h"
    )
  else()
    set_source_files_properties(${GENERATED_PYTHON}/${MODULENAME_LOWER}PYTHON_wrap.cxx PROPERTIES GENERATED TRUE)
  endif()
  target_compile_definitions(${_PYAGRUMLIB} PRIVATE SWIG_TYPE_TABLE=pyproba)
  # GUM_PUBLIC is blanked to nothing on every agrum${OPTION} module library under
  # BUILD_PYTHON (Modules.agrum.cmake) so a plain-GUM_PUBLIC-tagged class never gets
  # re-exposed by -fvisibility=hidden/dllexport in pyAgrum builds -- but that blanking
  # was never applied to the SWIG wrap targets themselves (this macro, one call per
  # module: pyagrum/id/mrf/cn/cm/prm). Each wrap TU pulls in its module's umbrella
  # header (e.g. core's #include <agrum/bn.h>), and umbrella headers freely reach
  # across module boundaries for convenience (agrum/bn.h itself #includes PRM's
  # O3prmBNReader.h/O3prmBNWriter.h for O3PRM-format BN I/O) -- so a wrap TU can end up
  # compiling a class it never actually wraps or calls. On MSVC specifically, merely
  # having a __declspec(dllexport)-tagged class's full definition visible is enough:
  # unlike GCC/Clang (which only emit an inline member if it's actually ODR-used),
  # MSVC eagerly instantiates and emits EVERY inline member of a dllexport'd class,
  # since the DLL's ABI must be complete regardless of what this TU itself calls.
  # Without this blanking, a class reached only transitively through an umbrella
  # header -- never wrapped or called by this TU at all -- can still have its
  # out-of-line members pulled into the wrap object file and linked against a
  # sibling module's .lib that this target never links, producing an unresolved
  # external. Blanking GUM_PUBLIC here, uniformly for every module (not just core),
  # closes that gap for any leaf's own umbrella-header reach.
  target_compile_definitions(${_PYAGRUMLIB} PRIVATE GUM_PUBLIC=)
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
  #
  # "Embedded once" has to mean the WHOLE compiled surface of BASE/BN, not just
  # whatever core's own SWIG wrap file happens to reference: a plain
  # target_link_libraries() on a STATIC archive only pulls in the .o members
  # needed to resolve symbols already seen by the linker (classic archive
  # semantics) -- so a symbol used only by a leaf module (e.g. gum::Formula,
  # used solely by PRM's o3prm parser; gum::BayesNetFactory<double>;
  # gum::Rational<double>) would silently never make it into _pyagrum.so at
  # all, no matter how it's tagged. PYGUM_PUBLIC only controls whether an
  # *included* symbol is exported -- not whether it gets included in the first
  # place -- so both are needed together: whole-archive for inclusion,
  # PYGUM_PUBLIC for visibility. Force the whole archive in with the
  # platform-specific linker flag.
  if(MODULENAME_LOWER STREQUAL "pyagrum")
    # Whole-archive is the ONLY link mechanism used for BASE/BN into core: do
    # NOT also add a plain target_link_libraries(... agrumBASE agrumBN) here.
    # Doing so lists the same .a three times in the final link line -- once
    # forced, once plain, once more transitively (agrumBN PUBLIC-links
    # agrumBASE) -- and ld64 does not cleanly coalesce weak/template symbols
    # pulled in through both a forced and a plain reference to the same
    # archive: the same mangled symbol (e.g.
    # gum::BayesNetFactory<double>::BayesNetFactory(gum::BayesNet<double>*))
    # can end up duplicated, each copy demoted to local linkage instead of
    # being merged into one exported definition. Pull just the include dirs
    # we actually need instead of a full target dependency.
    target_include_directories(${_PYAGRUMLIB} PRIVATE ${AGRUM_SOURCE_DIR} ${AGRUM_BINARY_DIR})
    # This TU (core's own SWIG wrap file) whole-archives agrumBASE/agrumBN's compiled
    # object code directly into itself (see the platform branches below), so it is
    # really a third "owner" of every PYGUM_SHARED_PUBLIC symbol alongside agrumBASE/
    # agrumBN -- without this define, PYGUM_SHARED_PUBLIC resolves to dllimport here
    # (config.h.in: GUM_FOR_SWIG defined, PYGUM_SHARED_EXPORTING not) even though the
    # symbol's actual definition ends up in this very binary. MSVC's linker tolerates
    # that mismatch with an LNK4217 warning (locally-defined symbol imported), but GNU
    # ld (MinGW) rejects it outright: unlike link.exe, it never resolves an __imp_*
    # thunk against a same-binary definition, only against a real import library --
    # so leaving this TU on dllimport turned every whole-archived symbol it touches
    # into a hard "undefined reference to `__imp_...'" on MinGW.
    target_compile_definitions(${_PYAGRUMLIB} PRIVATE PYGUM_SHARED_EXPORTING)
    # Same gap, same mechanism, for GUM_SHARED_PUBLIC (BASE-only so far,
    # config.h.in): Modules.agrum.cmake defines GUM_SHARED_EXPORTING on
    # agrumBASE's own target and blanks GUM_SHARED_PUBLIC to nothing on every
    # agrum<MODULE> static-lib target for pyAgrum builds, but neither of those
    # scopes covers this TU -- core's own wrap file is a third owner of every
    # GUM_SHARED_PUBLIC symbol exactly like it is for PYGUM_SHARED_PUBLIC above,
    # via the same whole-archive embedding below. Without this, GUM_SHARED_PUBLIC
    # resolves to plain dllimport here (config.h.in: GUM_SHARED_EXPORTING not
    # defined) while the symbol's actual definition (Timer, IdCondSet,
    # RecordCounter, ScoringCache, GammaLog2, FormulaPart, ...) is blanked to no
    # attribute at all in agrumBASE's own compilation -- link.exe then can't
    # resolve the __imp_* thunk this TU emits against agrumBASE.lib's plain
    # (non-decorated) symbol, an unresolved external on every GUM_SHARED_PUBLIC-
    # tagged BASE class this wrap file touches. This is a distinct gap from the
    # umbrella-header GUM_PUBLIC one handled by the
    # target_compile_definitions(... GUM_PUBLIC=) call earlier in this macro:
    # that one covers plain-GUM_PUBLIC classes reached transitively through an
    # umbrella header, this one covers GUM_SHARED_PUBLIC's own producer/consumer
    # split.
    target_compile_definitions(${_PYAGRUMLIB} PRIVATE GUM_SHARED_EXPORTING)
    # $<TARGET_FILE:...> in target_link_options is not always enough to
    # guarantee build order under parallel make -- force it explicitly so a
    # clean -j build doesn't try to link before agrumBASE/agrumBN exist.
    add_dependencies(${_PYAGRUMLIB} agrumBASE agrumBN)
    if(APPLE)
      target_link_options(${_PYAGRUMLIB} PRIVATE
        -Wl,-force_load,$<TARGET_FILE:agrumBASE>
        -Wl,-force_load,$<TARGET_FILE:agrumBN>)
    elseif(MSVC)
      # MSVC has no force-load-only mechanism: /WHOLEARCHIVE requires the
      # archive to also be a normal link input. Link only agrumBN here, NOT
      # agrumBASE too -- agrumBN already PUBLIC-links agrumBASE (see
      # Modules.agrum.cmake), so listing agrumBASE explicitly here would be a
      # redundant duplicate in the final link line (once via this call, once
      # transitively via agrumBN, plus /WHOLEARCHIVE referencing it by name).
      target_link_libraries(${_PYAGRUMLIB} PRIVATE agrumBN)
      target_link_options(${_PYAGRUMLIB} PRIVATE
        /WHOLEARCHIVE:$<TARGET_FILE_NAME:agrumBASE>
        /WHOLEARCHIVE:$<TARGET_FILE_NAME:agrumBN>)
    else()
      # GNU ld (Linux and MinGW alike): whole-archive is enough on its own on Linux,
      # where -fvisibility=hidden + the PYGUM_PUBLIC/PYGUM_SHARED_PUBLIC tagging
      # (config.h.in) already controls exactly what's exported. MinGW needs one more
      # flag on top of that: ld's default PE behavior is to auto-export every global
      # symbol from a DLL as long as NO object being linked carries an explicit
      # dllexport attribute, and consumers auto-import plain (undecorated) references
      # back out of it -- this is what let MinGW builds work even though the vast
      # majority of agrumBASE/agrumBN's public surface (HashFunc<*>, _hashTableLog2_,
      # trim, isInteger, isNumerical, remove_newline, ...) was never tagged
      # PYGUM_SHARED_PUBLIC, only ever needing it on MSVC's own producer/consumer
      # split. The _WIN32 fix in config.h.in made PYGUM_SHARED_PUBLIC/PYGUM_PUBLIC
      # emit real dllexport on MinGW too (correctly, for MSVC-parity on the symbols
      # that ARE tagged) -- but the moment ld sees ANY explicit dllexport in the link,
      # it silently switches the whole DLL from "export everything" to "export only
      # what's tagged", so every one of those untagged-but-needed symbols vanished
      # from the leaf modules' undefined-reference resolution in one shot.
      # --export-all-symbols forces ld back to exporting everything regardless of
      # that switch; the explicit dllexport/dllimport pairs added above/in
      # config.h.in stay in effect for the symbols that do carry them, so nothing
      # is lost there.
      target_link_options(${_PYAGRUMLIB} PRIVATE
        -Wl,--whole-archive $<TARGET_FILE:agrumBASE> $<TARGET_FILE:agrumBN> -Wl,--no-whole-archive)
      if(MINGW)
        target_link_options(${_PYAGRUMLIB} PRIVATE -Wl,--export-all-symbols)
      endif()
    endif()
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
  # temp CMAKE_INSTALL_PREFIX is deleted right after zipping -- dlopen would then
  # look for libagrumBASE under a temp directory that no longer exists. Core
  # pyagrum/_pyagrum.so is itself the shared dependency
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

  # LTO: eliminates dead code across translation units at link time -- but
  # every pyAgrum SWIG module (core AND leaf) now crosses a real DLL boundary
  # to resolve BASE/BN symbols: core embeds the whole BASE/BN archive on
  # purpose (see the whole-archive force-load above) so symbols only
  # referenced from a *submodule* stay available, and every leaf module
  # resolves those same symbols back out of core's exported surface. LTO's
  # cross-TU view does not extend cleanly across that boundary on any
  # toolchain tested so far:
  #   - core, ld64 (macOS): LTO silently demotes/duplicates
  #     whole-archive-only symbols to internal linkage despite PYGUM_PUBLIC
  #     (observed: gum::BayesNetFactory<double>, gum::HashFunc<Set<unsigned
  #     long>> -- used only by pyagrum.prm/.cn, never by core's own SWIG wrap
  #     code).
  #   - leaf, MinGW/GCC: LTRANS fails with "undefined reference" for symbols
  #     genuinely exported by core's .pyd, because GCC's LTO pipeline doesn't
  #     see core's import library as satisfying them (observed building
  #     _id.pyd against ticpp/TiXml symbols exported by _pyagrum.pyd).
  # Disable LTO for all of them rather than chase this per-toolchain.
  if(_AGRUM_IPO_SUPPORTED)
    message(STATUS "pyAgrum: LTO disabled for ${_PYAGRUMLIB} (crosses a DLL boundary to reach BASE/BN symbols; LTO does not resolve cleanly across it on ld64 or MinGW/GCC)")
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

# ==========================================================================
# Experimental: shrink the core to the symbols the built leaf modules
# actually use. See md_docs/reportModularizedPyAgrum.md (§4/§8/§9) and
# cmake/trim_core_exports.py for the rationale: the PYGUM_SHARED_PUBLIC
# source annotations control what CAN be exported, not what a
# whole-archived core keeps once dead-code removal runs -- this derives the
# real, currently-needed subset from the actually-built leaf binaries
# (undefined-symbol scan), then relinks the core in place restricted to
# that subset plus its own Python entry point, with dead code removal
# finally turned on for the core too (see the dead-strip comment on
# gum_add_python_module above for why it stays off on core's *primary*
# link -- that link still has to keep everything, since it happens before
# any leaf module exists to be scanned).
#
# Runs strictly after core and every enabled leaf module have already
# linked normally against the untouched, fully-exported core, so the leaf
# modules' own build is entirely unaffected -- only the core's output file
# gets overwritten afterward. Symbol resolution for the already-built leaf
# modules happens by name at load time on every platform covered here, so
# relinking core afterward does not require rebuilding them.
#
# Not attempted for MinGW: it already falls back to --export-all-symbols
# (see the whole-archive branch above) precisely because most of
# BASE/BN's surface was never individually tagged, for MSVC-parity reasons
# alone -- restricting exports there needs that fallback removed first, a
# separate, riskier change left for later.
# ==========================================================================
option(PYAGRUM_TRIM_CORE_EXPORTS "Shrink core's kept symbol set to what the built leaf modules actually use (experimental)" ON)

if(PYAGRUM_TRIM_CORE_EXPORTS AND NOT MINGW)
  set(_PYAGRUM_LEAF_FILES "")
  set(_PYAGRUM_LEAF_TARGETS "")
  foreach(_pysubmod_upper MRF ID CN CM PRM)
    string(TOLOWER ${_pysubmod_upper} _pysubmod)
    if(PYAGRUM_WITH_${_pysubmod_upper} AND BUILD_${_pysubmod_upper})
      list(APPEND _PYAGRUM_LEAF_TARGETS _${_pysubmod})
      list(APPEND _PYAGRUM_LEAF_FILES "$<TARGET_FILE:_${_pysubmod}>")
    endif()
  endforeach()

  if(_PYAGRUM_LEAF_TARGETS)
    set(_PYAGRUM_EXPORTS_FILE ${CMAKE_CURRENT_BINARY_DIR}/core_exports_trimmed.txt)
    set(_PYAGRUM_SCAN_STAMP ${CMAKE_CURRENT_BINARY_DIR}/core_trim_scan.stamp)

    # CMAKE_CXX_FLAGS & co are plain space-separated strings, not CMake
    # ;-separated lists -- used unquoted in a VERBATIM custom_command
    # COMMAND, each stays ONE argument instead of splitting into per-flag
    # arguments, so e.g. "-fopenmp=libomp -Wall ..." gets handed to the
    # compiler as the single (bogus) value of "-fopenmp=". separate_arguments
    # turns them into real lists first. _PYAGRUM_CXX_FLAGS_LIST is only read
    # by the APPLE/Linux *link* commands below (they invoke the compiler as a
    # Unix-style linker driver) -- MSVC links via a plain /link step, and
    # compiling the wrap source itself goes through a real OBJECT library
    # target (see _pyagrum_core_trim_obj below), which gets every compile
    # flag/definition _pyagrum itself gets the normal CMake way (CXX_STANDARD,
    # MSVC's /bigobj and NOMINMAX, -fvisibility=hidden, ...) instead of a
    # hand-copied list -- a hand-copied list is exactly what silently missed
    # the C++ standard flag once already (CMAKE_CXX_STANDARD is applied by
    # CMake as a per-target /std:c++NN flag, never folded into
    # CMAKE_CXX_FLAGS).
    separate_arguments(_PYAGRUM_CXX_FLAGS_LIST UNIX_COMMAND "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
    if(WIN32)
      separate_arguments(_PYAGRUM_SHARED_LINKER_FLAGS_LIST WINDOWS_COMMAND "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
    else()
      separate_arguments(_PYAGRUM_SHARED_LINKER_FLAGS_LIST UNIX_COMMAND "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
    endif()

    if(APPLE)
      set(_PYAGRUM_TRIM_FORMAT apple)
    elseif(MSVC)
      set(_PYAGRUM_TRIM_FORMAT msvc)
    else()
      set(_PYAGRUM_TRIM_FORMAT gnu)
    endif()

    # Computed once, reused both by _pyagrum_core_trim_obj's target_link_libraries
    # below and by the MSVC relink command's $<TARGET_LINKER_FILE:...> further down
    # -- gum_add_python_module(pyagrum) above makes this same choice a third time
    # for _pyagrum itself; that copy can't be deduped away without touching that
    # pre-existing, shared-across-every-module macro.
    if(AGRUM_PYTHON_SABI AND TARGET Python::SABIModule)
      set(_PYAGRUM_CORE_PYTHON_TARGET Python::SABIModule)
    else()
      set(_PYAGRUM_CORE_PYTHON_TARGET Python::Module)
    endif()

    # Recompile core's one wrap source through a real OBJECT library target.
    # Compile definitions/include directories/options are read straight back
    # off _pyagrum itself via $<TARGET_PROPERTY:...> instead of a hand-copied
    # second list, so this automatically stays in sync with whatever
    # gum_add_python_module(pyagrum) above sets on _pyagrum (SWIG_TYPE_TABLE,
    # PYGUM_SHARED_EXPORTING, AGRUM_SOURCE_DIR/AGRUM_BINARY_DIR,
    # Python_NumPy_INCLUDE_DIRS, SWIG_COMPILE_FLAGS) -- a hand-copied list would
    # silently drift out of sync with that set instead. TARGET_PROPERTY only
    # reads what was set directly on a target, not requirements inherited
    # through its own PRIVATE links, so the Python include dir (which
    # _pyagrum picks up transitively by linking Python::Module/SABIModule)
    # still needs linking in directly below.
    #
    # CMake also generates the actual compile command itself, so this also
    # carries every directory-scoped flag applied to every target (CXX_STANDARD
    # 20, MSVC's /bigobj and NOMINMAX, -fvisibility=hidden, ...) with no
    # hand-copying at all.
    #
    # $<TARGET_OBJECTS:...> reliably expands for an OBJECT library target on
    # every generator (unlike for _pyagrum itself, a SHARED target). A
    # hardcoded, Make/Ninja-shaped path straight to _pyagrum's own compiled
    # .o (e.g. CMakeFiles/_pyagrum.dir/...) would work on APPLE/Linux but not
    # under the Visual Studio generator's differently shaped, per-configuration
    # object layout -- this OBJECT-library recompile is the one mechanism that
    # works identically on all three platforms.
    add_library(_pyagrum_core_trim_obj OBJECT EXCLUDE_FROM_ALL ${GENERATED_PYTHON}/pyagrumPYTHON_wrap.cxx)
    # CMake auto-enables -fPIC for SHARED/MODULE targets (that's how _pyagrum itself gets
    # it) but NOT for a plain OBJECT library -- it has no way to know its objects will end
    # up in a shared object. Without this, GCC/ld on Linux reject the relink with
    # "relocation R_X86_64_32S ... can not be used when making a shared object" (harmless
    # on macOS/MSVC, where code is PIC-by-default/PIC-irrelevant respectively).
    set_target_properties(_pyagrum_core_trim_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_definitions(_pyagrum_core_trim_obj PRIVATE $<TARGET_PROPERTY:_pyagrum,COMPILE_DEFINITIONS>)
    target_include_directories(_pyagrum_core_trim_obj PRIVATE $<TARGET_PROPERTY:_pyagrum,INCLUDE_DIRECTORIES>)
    target_compile_options(_pyagrum_core_trim_obj PRIVATE $<TARGET_PROPERTY:_pyagrum,COMPILE_OPTIONS>)
    target_link_libraries(_pyagrum_core_trim_obj PRIVATE ${_PYAGRUM_CORE_PYTHON_TARGET})
    if(SWIG_FOUND)
      add_dependencies(_pyagrum_core_trim_obj pyagrum_swig_generated)
    endif()
    set(_PYAGRUM_CORE_WRAP_OBJ $<TARGET_OBJECTS:_pyagrum_core_trim_obj>)

    set(_PYAGRUM_TRIM_ARGS
      --core $<TARGET_FILE:_pyagrum>
      --format ${_PYAGRUM_TRIM_FORMAT}
      --keep PyInit__pyagrum
      --out ${_PYAGRUM_EXPORTS_FILE})
    foreach(_leaf ${_PYAGRUM_LEAF_FILES})
      list(APPEND _PYAGRUM_TRIM_ARGS --leaf ${_leaf})
    endforeach()
    if(MSVC)
      list(APPEND _PYAGRUM_TRIM_ARGS --core-basename $<TARGET_FILE_NAME:_pyagrum>)
    endif()

    add_custom_command(OUTPUT ${_PYAGRUM_SCAN_STAMP}
      DEPENDS _pyagrum ${_PYAGRUM_LEAF_TARGETS}
      COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/trim_core_exports.py ${_PYAGRUM_TRIM_ARGS}
      COMMAND ${CMAKE_COMMAND} -E touch ${_PYAGRUM_SCAN_STAMP}
      COMMENT "pyAgrum: scanning built leaf modules for core symbols actually used"
      VERBATIM)

    set(_PYAGRUM_RELINK_STAMP ${CMAKE_CURRENT_BINARY_DIR}/core_trim_relink.stamp)
    if(APPLE)
      add_custom_command(OUTPUT ${_PYAGRUM_RELINK_STAMP}
        DEPENDS ${_PYAGRUM_SCAN_STAMP} _pyagrum_core_trim_obj
        COMMAND ${CMAKE_CXX_COMPILER} ${_PYAGRUM_CXX_FLAGS_LIST} ${_PYAGRUM_SHARED_LINKER_FLAGS_LIST}
                -dynamiclib -Wl,-headerpad_max_install_names
                -Wl,-force_load,$<TARGET_FILE:agrumBASE> -Wl,-force_load,$<TARGET_FILE:agrumBN>
                -Xlinker -undefined -Xlinker dynamic_lookup
                -Wl,-dead_strip -Wl,-exported_symbols_list,${_PYAGRUM_EXPORTS_FILE}
                -install_name $<TARGET_FILE_NAME:_pyagrum>
                -Wl,-rpath,@loader_path
                -o $<TARGET_FILE:_pyagrum>
                ${_PYAGRUM_CORE_WRAP_OBJ}
        COMMAND ${CMAKE_COMMAND} -E touch ${_PYAGRUM_RELINK_STAMP}
        COMMENT "pyAgrum: relinking core restricted to the symbols actually used (macOS)"
        VERBATIM)
    elseif(MSVC)
      add_custom_command(OUTPUT ${_PYAGRUM_RELINK_STAMP}
        DEPENDS ${_PYAGRUM_SCAN_STAMP} _pyagrum_core_trim_obj
        COMMAND ${CMAKE_CXX_COMPILER} /nologo /LD
                ${_PYAGRUM_CORE_WRAP_OBJ}
                /link /NOLOGO
                $<TARGET_LINKER_FILE:agrumBASE> $<TARGET_LINKER_FILE:agrumBN>
                /WHOLEARCHIVE:$<TARGET_FILE_NAME:agrumBASE> /WHOLEARCHIVE:$<TARGET_FILE_NAME:agrumBN>
                /LIBPATH:$<TARGET_FILE_DIR:agrumBASE> /LIBPATH:$<TARGET_FILE_DIR:agrumBN>
                /DEF:${_PYAGRUM_EXPORTS_FILE} /OPT:REF /OPT:ICF
                /OUT:$<TARGET_FILE:_pyagrum> /IMPLIB:$<TARGET_LINKER_FILE:_pyagrum>
                $<TARGET_LINKER_FILE:${_PYAGRUM_CORE_PYTHON_TARGET}>
                ${_PYAGRUM_SHARED_LINKER_FLAGS_LIST}
        COMMAND ${CMAKE_COMMAND} -E touch ${_PYAGRUM_RELINK_STAMP}
        COMMENT "pyAgrum: relinking core restricted to the symbols actually used (MSVC -- links the already-compiled _pyagrum_core_trim_obj object, generator-agnostic)"
        VERBATIM)
    else()
      # Resolved into plain variables (not left as raw $<TARGET_PROPERTY:...>
      # tokens in COMMAND below): on glibc >= 2.34 (pthread folded into libc),
      # both properties are genuinely empty, and an empty generator-expression
      # result still lands as a real empty argv entry in a raw custom_command
      # COMMAND -- gcc forwards that to ld as a bogus input filename ("ld:
      # cannot find : No such file or directory"). A plain empty CMake
      # variable, by contrast, vanishes entirely when substituted unquoted.
      get_target_property(_PYAGRUM_THREADS_LINK_LIBS Threads::Threads INTERFACE_LINK_LIBRARIES)
      get_target_property(_PYAGRUM_THREADS_LINK_OPTS Threads::Threads INTERFACE_LINK_OPTIONS)
      if(NOT _PYAGRUM_THREADS_LINK_LIBS)
        set(_PYAGRUM_THREADS_LINK_LIBS "")
      endif()
      if(NOT _PYAGRUM_THREADS_LINK_OPTS)
        set(_PYAGRUM_THREADS_LINK_OPTS "")
      endif()
      add_custom_command(OUTPUT ${_PYAGRUM_RELINK_STAMP}
        DEPENDS ${_PYAGRUM_SCAN_STAMP} _pyagrum_core_trim_obj
        COMMAND ${CMAKE_CXX_COMPILER} ${_PYAGRUM_CXX_FLAGS_LIST} ${_PYAGRUM_SHARED_LINKER_FLAGS_LIST}
                -shared
                -Wl,--whole-archive $<TARGET_FILE:agrumBASE> $<TARGET_FILE:agrumBN> -Wl,--no-whole-archive
                ${_PYAGRUM_THREADS_LINK_LIBS}
                ${_PYAGRUM_THREADS_LINK_OPTS}
                -Wl,--gc-sections -Wl,--version-script=${_PYAGRUM_EXPORTS_FILE}
                -Wl,-rpath,$ORIGIN
                -o $<TARGET_FILE:_pyagrum>
                ${_PYAGRUM_CORE_WRAP_OBJ}
        COMMAND ${CMAKE_COMMAND} -E touch ${_PYAGRUM_RELINK_STAMP}
        COMMENT "pyAgrum: relinking core restricted to the symbols actually used (Linux)"
        VERBATIM)
    endif()

    add_custom_target(pyagrum_core_trim ALL DEPENDS ${_PYAGRUM_RELINK_STAMP})
  endif()
endif()

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
