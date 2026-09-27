#first option is BUILD_ALL
option(BUILD_ALL "" OFF)

#creating all options
foreach (OPTION ${MODULES})
    option(BUILD_${OPTION} "build module ${OPTION}" OFF)
    message(STATUS "  (+) defining  BUILD_${OPTION}")
endforeach ()

# If BUILD_ALL was explicitly requested, propagate to all individual modules so
# that stale cache entries from a previous partial build don't override it.
if (BUILD_ALL)
    foreach (OPTION ${MODULES})
        set(BUILD_${OPTION} ON CACHE BOOL "build module ${OPTION}" FORCE)
    endforeach ()
endif ()

############### DEPENDENCIES BETWEEN OPTIONS ################
message(STATUS "** aGrUM Notification: Checking dependencies ...")
set(CHECK_DEPS 1)
while (${CHECK_DEPS} EQUAL 1)
    set(CHECK_DEPS 0)

    foreach (OPTION ${MODULES})
        if (BUILD_${OPTION})
            if (${OPTION}_DEPS)
                foreach (DEP ${${OPTION}_DEPS})
                    if (BUILD_${DEP})
                    else ()
                        set(BUILD_${DEP} "ON")
                        set(CHECK_DEPS 1)
                        message(STATUS "  (+) adding ${DEP}")
                    endif ()
                endforeach ()
            endif ()
        endif ()
    endforeach ()
endwhile ()

############### CONSTRAINTS BETWEEN OPTIONDS ################
# we want that
#  -if BUILD_ALL=ON, other ON is non important (just FYI because nothing to do),
#  -if there is at least one module = OFF, then BUILD_ALL is OFF
#  -if no module is ON then BUILD_ALL is ON
set(NBR_OPTIONS 0)
set(TOTAL_OPTIONS 0)
foreach (OPTION ${MODULES})
    math(EXPR TOTAL_OPTIONS "${TOTAL_OPTIONS}+1")
    if (BUILD_${OPTION})
        math(EXPR NBR_OPTIONS "${NBR_OPTIONS}+1")
    endif ()
endforeach ()

# CACHE FORCE (not a plain set()): wrappers/pyagrum/CMakeLists.txt is a sibling
# add_subdirectory(), not a descendant of this scope, and checks BUILD_ALL too
# when no explicit -DBUILD_<MODULE> flag was passed (e.g. a plain `cmake
# -DBUILD_PYTHON=ON`, as conda-forge recipes do).
if (NBR_OPTIONS EQUAL 0 OR NBR_OPTIONS EQUAL TOTAL_OPTIONS)
    set(BUILD_ALL ON CACHE BOOL "build every module" FORCE)
else ()
    set(BUILD_ALL OFF CACHE BOOL "build every module" FORCE)
endif ()

# modules are all options (except ALL) + BASE module
set(LIST_OF_MODULES "BASE" ${MODULES})
list(REMOVE_DUPLICATES LIST_OF_MODULES)
set(LIST_OF_MODULES ${LIST_OF_MODULES} PARENT_SCOPE)

# creating the lists of files by module
foreach (MODULE ${LIST_OF_MODULES})
    set(AGRUM_${MODULE}_SOURCES "")
    set(AGRUM_${MODULE}_INCLUDES "")
    set(AGRUM_${MODULE}_INLINES "")
    set(AGRUM_${MODULE}_TEMPLATES "")
    set(AGRUM_${MODULE}_C_SOURCES "")

    foreach (DIR ${${MODULE}_DIRS})
        file(GLOB_RECURSE LOOP_SRC ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/${DIR}/*.cpp)
        set(AGRUM_${MODULE}_SOURCES ${AGRUM_${MODULE}_SOURCES} ${LOOP_SRC})
        file(GLOB_RECURSE LOOP_HEADER ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/${DIR}/*.h)
        set(AGRUM_${MODULE}_INCLUDES ${AGRUM_${MODULE}_INCLUDES} ${LOOP_HEADER})
    endforeach ()
endforeach ()

#credal networks has a special case for C files
if (BUILD_CN OR BUILD_ALL)
    file(GLOB_RECURSE AGRUM_CN_C_SOURCES ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/base/external/lrslib/lrslib.c ${AGRUM_SOURCE_DIR}/agrum/base/external/lrslib/lrsmp.c)
endif ()

# ticpp is an internal dependency used only by BN's XML readers
# (BIFXMLBNReader/XDSLBNReader) and core's own SWIG wrap TU: moved out of BASE
# into BN so it gets exported once from agrumBN like any other BN symbol,
# instead of a private per-consumer copy risking duplicate RTTI.
file(GLOB TICPP_SOURCES "${AGRUM_SOURCE_DIR}/agrum/base/external/tinyxml/ticpp/*.cpp")
list(REMOVE_ITEM AGRUM_BASE_SOURCES ${TICPP_SOURCES})
if (BUILD_BN OR BUILD_ALL)
    list(APPEND AGRUM_BN_SOURCES ${TICPP_SOURCES})
endif ()

if (BUILD_ALL)
    message(STATUS "** aGrUM Notification: Building all")
else ()
    message(STATUS "** aGrUM Notification: Building specific module(s)")
endif ()

#this macro has to be executed when recolt of module file lists is finished (after CocoR targets for instance)
macro(buildFileListsWithModules)
    message(STATUS "** aGrUM Notification: Generating files lists")

    # BASE headers get listed as sources of every OTHER module's target too,
    # purely so IDEs group them there for browsing. CocoR-generated headers
    # (Parser.h/Scanner.h) must be excluded from that cross-module list: CMake
    # ties a custom command to any target that lists one of its OUTPUT files
    # as a source, so leaving them in duplicates the SyntaxFormula.atg
    # generation rule into every dependent module's own build.make; under a
    # parallel build (`make -jN`, e.g. `act pipinstall`) several targets then
    # run cococpp concurrently on the same output files, segfaulting it.
    set(AGRUM_BASE_INCLUDES_FOR_OTHER_MODULES ${AGRUM_BASE_INCLUDES})
    list(REMOVE_ITEM AGRUM_BASE_INCLUDES_FOR_OTHER_MODULES
            ${AGRUM_SOURCE_DIR}/agrum/base/core/math/cocoR/Parser.h
            ${AGRUM_SOURCE_DIR}/agrum/base/core/math/cocoR/Scanner.h)
    foreach (OPTION ${MODULES})
        if (BUILD_${OPTION} OR BUILD_ALL)
            message(STATUS "** aGrUM Notification:      (+) adding target for ${OPTION}")

            # BASE/BN are the two modules pyAgrum whole-archives once into core
            # _pyagrumcpp.so instead of relinking per leaf module (see
            # wrappers/pyagrum/CMakeLists.txt) -- computed once here since both
            # the export split below and the dependency handling further down
            # branch on it.
            set (_IS_BASE_OR_BN OFF)
            if (OPTION STREQUAL "BASE" OR OPTION STREQUAL "BN")
                set (_IS_BASE_OR_BN ON)
            endif ()

            # BUILD_SHARED_LIBS=ON + BUILD_PYTHON=ON only happens for the
            # conda-forge recipe; PyPI/wheelhouse always builds pyAgrum static,
            # so this branch's split-per-module behavior only ever runs there.
            if (OPTION STREQUAL "BASE")
                add_library (agrum${OPTION} ${AGRUM_${OPTION}_SOURCES} ${AGRUM_${OPTION}_C_SOURCES} ${AGRUM_${OPTION}_INCLUDES})
            else ()
                add_library (agrum${OPTION} ${AGRUM_${OPTION}_SOURCES} ${AGRUM_${OPTION}_C_SOURCES} ${AGRUM_${OPTION}_INCLUDES} ${AGRUM_BASE_INCLUDES_FOR_OTHER_MODULES})
            endif ()

            # lrslib (base/external/lrslib) is vendored third-party C code with
            # no export attributes; attach the generated .def only when agrumCN
            # will actually be a DLL (MSVC/MinGW under BUILD_SHARED_LIBS=ON or
            # standalone aGrUM) -- both linkers otherwise drop its untagged
            # symbols once any GUM_PUBLIC_CN tag switches them to
            # explicit-export mode.
            if (OPTION STREQUAL "CN" AND WIN32 AND (MSVC OR MINGW) AND (BUILD_SHARED_LIBS OR NOT BUILD_PYTHON))
                # The .def's LIBRARY line must name the DLL the linker actually
                # produces (agrumCN.dll on MSVC, libagrumCN.dll on MinGW) -- a
                # mismatch builds fine but leaves _cncpp.pyd's NEEDED entry
                # naming a nonexistent file, failing at import time not link
                # time.
                if (MINGW)
                    set (LRSLIB_DEF_LIBRARY_NAME "libagrumCN.dll")
                else ()
                    set (LRSLIB_DEF_LIBRARY_NAME "agrumCN.dll")
                endif ()
                configure_file (
                        ${AGRUM_SOURCE_DIR}/agrum/CN/polytope/lrslib_windows.def.in
                        ${AGRUM_BINARY_DIR}/agrum/CN/polytope/lrslib_windows.def
                        @ONLY)
                target_sources (agrumCN PRIVATE ${AGRUM_BINARY_DIR}/agrum/CN/polytope/lrslib_windows.def)
            endif ()

            # GUM_PUBLIC blanking applies to every module under BUILD_PYTHON,
            # not just BASE/BN: default visibility is public for pyAgrum builds,
            # so a plain-GUM_PUBLIC-tagged class would otherwise get re-exposed
            # by any module's .so/.pyd -- kept as a safety net even though every
            # header has since migrated to its own GUM_PUBLIC_<MODULE> name.
            if (BUILD_PYTHON)
                target_compile_definitions (agrum${OPTION} PRIVATE GUM_PUBLIC=)

                # GUM_SHARED_PUBLIC is deliberately never blanked here (unlike
                # GUM_PUBLIC): it is the producer/consumer split for symbols
                # pyAgrum's leaf modules do need (config.h.in) -- blanking it
                # would define the macro empty on agrumBASE's own target before
                # config.h.in's guard runs, silently compiling every
                # GUM_SHARED_PUBLIC symbol (KNML, CachedContingencyCounter,
                # IndependenceTest, ...) as hidden-visibility, i.e. undefined
                # symbol at leaf dlopen time.

                # GUM_PUBLIC_<MODULE> blanking for the remaining 7 modules,
                # static build only: BASE is excluded (uses GUM_SHARED_PUBLIC
                # instead), BN is excluded too since -- like BASE -- it is
                # whole-archived into core rather than linked normally, crossing
                # that same DLL boundary to every other leaf even under
                # BUILD_SHARED_LIBS=OFF; blanking BN here used to short-circuit
                # config.h.in's guard and leave every GUM_PUBLIC_BN-tagged
                # symbol unexported (LNK2019). Skipped entirely under
                # BUILD_SHARED_LIBS=ON, where each module is a genuine separate
                # DLL needing its own dllexport.
                # LIST_OF_MODULES (computed above, exported to parent scope) lists all 8.
                if (NOT BUILD_SHARED_LIBS)
                    foreach (BLANK_MODULE ${LIST_OF_MODULES})
                        if (NOT BLANK_MODULE STREQUAL "BASE" AND NOT BLANK_MODULE STREQUAL "BN")
                            target_compile_definitions (agrum${OPTION} PRIVATE GUM_PUBLIC_${BLANK_MODULE}=)
                        endif ()
                    endforeach ()
                endif ()
            endif ()

            # GUM_SHARED_EXPORTING marks agrumBASE as sole producer of
            # GUM_SHARED_PUBLIC symbols (config.h.in); unconditional (not gated
            # on BUILD_PYTHON) so BASE sees its own flag even in the TU defining
            # it, avoiding the C4273 gap PYGUM_SHARED_EXPORTING hit when gated
            # that way (GUM_PUBLIC.md §13).
            #
            # AGRUM_BASE_EXPORTING exists solely so GUM_COCOR_PUBLIC
            # (config.h.in) has a flag exclusive to compiling agrumBASE's own
            # object files -- GUM_SHARED_EXPORTING isn't exclusive to BASE
            # (core's SWIG wrap TU also defines it), which wrongly matched
            # core's own CoCo/R re-instantiation of another module's grammar
            # when tried as the detection flag (LNK2019 on Windows, tried and
            # reverted, see the GUM_SHARED_EXPORTING comment above).
            if (OPTION STREQUAL "BASE")
                target_compile_definitions (agrum${OPTION} PRIVATE GUM_SHARED_EXPORTING AGRUM_BASE_EXPORTING)
            else ()
            # Producer flag for GUM_PUBLIC_<MODULE> (config.h.in): only wired
            # for real dllexport under BUILD_SHARED_LIBS=ON, else the blanking
            # guard above wins.
                target_compile_definitions (agrum${OPTION} PRIVATE AGRUM_${OPTION}_EXPORTING)
            endif ()

            # AGRUM_BUILD_SHARED_LIBS (config.h.in, CompilOptions.agrum.cmake)
            # tells GUM_SHARED_PUBLIC/GUM_PUBLIC_<MODULE> whether a real DLL
            # boundary exists: pyAgrum forces BUILD_SHARED_LIBS=OFF for every
            # agrum${OPTION} even though core _pyagrumcpp.so is always SHARED
            # one level up, so every module must see the same signal core's own
            # SWIG wrap TU gets, or producer/consumer tagging disagrees (LNK2005
            # for an inline-vs-exported mismatch, or LNK2019 when BASE's plain
            # definition never reaches core's export table).
            if (BUILD_PYTHON)
                target_compile_definitions (agrum${OPTION} PRIVATE AGRUM_BUILD_SHARED_LIBS)
            endif ()

            # PYGUM_SHARED_EXPORTING marks agrumBASE/agrumBN as true owner of
            # every PYGUM_SHARED_PUBLIC symbol (config.h.in), gated on
            # BUILD_PYTHON AND _IS_BASE_OR_BN AND NOT BUILD_SHARED_LIBS: under
            # static build BASE+BN are whole-archived into one link unit and BN
            # may legitimately dllexport BASE's symbols too; under
            # BUILD_SHARED_LIBS=ON they are two real separate DLLs, so BN must
            # dllimport BASE's symbols instead -- leaving this flag on BN there
            # caused LNK2019/LNK2001 once BASE/BN became separate libraries.
            if (BUILD_PYTHON AND _IS_BASE_OR_BN AND NOT BUILD_SHARED_LIBS)
                target_compile_definitions (agrum${OPTION} PRIVATE PYGUM_SHARED_EXPORTING)
            endif ()

            target_include_directories (agrum${OPTION} PRIVATE ${AGRUM_SOURCE_DIR};${AGRUM_BINARY_DIR})
            target_include_directories (agrum${OPTION} INTERFACE $<INSTALL_INTERFACE:include>)
            target_include_directories (agrum${OPTION} INTERFACE $<BUILD_INTERFACE:${AGRUM_SOURCE_DIR}>)
            target_include_directories (agrum${OPTION} INTERFACE $<BUILD_INTERFACE:${AGRUM_BINARY_DIR}>)

            target_compile_features (agrum${OPTION} PUBLIC cxx_std_20)

            set_target_properties (agrum${OPTION} PROPERTIES POSITION_INDEPENDENT_CODE ON)
            set_target_properties (agrum${OPTION} PROPERTIES DEBUG_POSTFIX "-dbg")
            set_target_properties (agrum${OPTION} PROPERTIES VERSION ${AGRUM_VERSION} SOVERSION ${AGRUM_VERSION_MAJOR})

            export (TARGETS agrum${OPTION} FILE agrum${OPTION}-targets.cmake)

            # handle dependencies
            foreach (DEP ${${OPTION}_DEPS})
                if (BUILD_PYTHON AND NOT _IS_BASE_OR_BN AND NOT BUILD_SHARED_LIBS)
                    # Leaf pyAgrum modules (PRM/MRF/CN/ID/CM) reach BASE/BN through core _pyagrumcpp.so
                    # only (see comment above): do NOT link agrum${DEP} here at all. For a STATIC
                    # library, target_link_libraries still adds the dependency to the *direct*
                    # consumer's link line even when PRIVATE -- static libs have no link step of
                    # their own, so CMake must pass their transitive deps down for symbol
                    # resolution regardless of PRIVATE/PUBLIC -- so PRIVATE alone still re-embeds
                    # agrumBASE/agrumBN's object code into the leaf .pyd on top of what _pyagrum's
                    # import lib already dllexports there (LNK2005 on MSVC). Headers stay reachable
                    # via the AGRUM_SOURCE_DIR PRIVATE include set above; unresolved symbols are
                    # resolved at the final .pyd link against _pyagrum (see
                    # wrappers/pyagrum/CMakeLists.txt).
                    #
                    # BUILD_SHARED_LIBS=ON is the exception to that skip: agrum${OPTION} here is a
                    # genuine standalone .so/.dylib (not embedded into core at all -- core links
                    # agrumBASE/agrumBN normally too, see wrappers/pyagrum/CMakeLists.txt), so it
                    # must resolve its own BASE/BN symbols the ordinary way, like every other
                    # non-python consumer below.
                else ()
                    target_link_libraries (agrum${OPTION} PUBLIC agrum${DEP})
                endif ()
            endforeach()

        endif ()
    endforeach ()
endmacro(buildFileListsWithModules)

# GUM_PUBLIC_<MODULE> dllexport/dllimport template (config.h.in), generated once
# per non-BASE module instead of hand-copied: a copy-paste typo or skipped
# branch here reproduces the exact class of LNK1181/LNK2019 bug this project
# spent module by module -- get the two right together, or reintroduce it.
# GUM_SHARED_PUBLIC (BASE's own macro) is structurally different and stays
# hand-written in config.h.in. Must run before src/CMakeLists.txt's
# configure_file(config.h.in ...).
set(_GUM_PUBLIC_MODULE_TEMPLATE [[
#ifndef GUM_PUBLIC_@@MOD@@
#  if defined(_WIN32) && defined(AGRUM_BUILD_SHARED_LIBS)
#    if defined(AGRUM_@@MOD@@_EXPORTING)
#      define GUM_PUBLIC_@@MOD@@ __declspec(dllexport)
#    else
#      define GUM_PUBLIC_@@MOD@@ __declspec(dllimport)
#    endif
#  elif defined(_WIN32)
     // No real DLL boundary here (BUILD_SHARED_LIBS=OFF): plain, undecorated
     // symbol for every TU; dllexport instead (tried, reverted) forces MSVC to
     // eagerly instantiate every implicit special member of a tagged class in
     // every static library that merely includes its header, colliding as
     // LNK2005 duplicate definitions across libraries linked into the same
     // binary.
#    define GUM_PUBLIC_@@MOD@@
#  elif defined(__GNUC__) || defined(__clang__)
#    define GUM_PUBLIC_@@MOD@@ __attribute__((visibility("default")))
#  else
#    define GUM_PUBLIC_@@MOD@@
#  endif
#endif
]])

macro(generateGumPublicModuleMacros)
    set(GUM_PUBLIC_MODULE_MACROS "")
    foreach (_gum_module ${MODULES})
        if (NOT _gum_module STREQUAL "BASE")
            string(REPLACE "@@MOD@@" "${_gum_module}" _gum_module_block "${_GUM_PUBLIC_MODULE_TEMPLATE}")
            string(APPEND GUM_PUBLIC_MODULE_MACROS "${_gum_module_block}\n")
        endif ()
    endforeach ()
    # PARENT_SCOPE: wrappers/pyagrum/CMakeLists.txt is a sibling
    # add_subdirectory(), not a descendant of src/'s scope, and needs this
    # variable too for its own configure_file(config.h.in).
    set(GUM_PUBLIC_MODULE_MACROS ${GUM_PUBLIC_MODULE_MACROS} PARENT_SCOPE)
endmacro(generateGumPublicModuleMacros)
