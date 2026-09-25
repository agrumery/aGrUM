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

# CACHE FORCE (not a plain set()): this recomputed value must be visible
# outside this scope -- wrappers/pyagrum/CMakeLists.txt (a sibling
# add_subdirectory(), not a descendant of this include()'d file's directory
# scope) checks BUILD_ALL too, to decide whether to build a leaf module's
# SWIG extension when no BUILD_<MODULE> flag was passed explicitly at all
# (e.g. a plain `cmake -DBUILD_PYTHON=ON`, as conda-forge-style recipes do,
# bypassing act's own explicit -DBUILD_ALL=ON). A plain set() here used to
# stay local to src/'s scope and never reach that check.
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

# ticpp is an internal dependency used only by BN readers: exclude from BASE,
# compile directly into BN so its symbols stay hidden and unexported.
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
    foreach (OPTION ${MODULES})
        if (BUILD_${OPTION} OR BUILD_ALL)
            message(STATUS "** aGrUM Notification:      (+) adding target for ${OPTION}")

            # BASE/BN are the two modules pyAgrum embeds once into core _pyagrumcpp.so (whole-archived,
            # see wrappers/pyagrum/CMakeLists.txt) instead of re-linking per leaf module -- computed
            # once here since both the export split below and the dependency handling further down
            # branch on it.
            set (_IS_BASE_OR_BN OFF)
            if (OPTION STREQUAL "BASE" OR OPTION STREQUAL "BN")
                set (_IS_BASE_OR_BN ON)
            endif ()

            # BUILD_SHARED_LIBS=ON combined with BUILD_PYTHON=ON is exercised only
            # by the conda-forge recipe (see the BUILD_SHARED_LIBS option comment
            # in the root CMakeLists.txt) -- PyPI/wheelhouse always builds pyAgrum
            # static, so this branch's split-per-module behavior only ever runs
            # there.
            add_library (agrum${OPTION} ${AGRUM_${OPTION}_SOURCES} ${AGRUM_${OPTION}_C_SOURCES} ${AGRUM_${OPTION}_INCLUDES} ${AGRUM_BASE_INCLUDES})

            # lrslib (base/external/lrslib) is vendored third-party C code with zero
            # export attributes on its declarations -- see the .def file's own header
            # comment for why it must not be tagged directly. Attach the .def only when
            # agrumCN will actually be a DLL for MSVC/MinGW to apply it to: standalone
            # aGrUM's own BUILD_SHARED_LIBS=ON default, or this chantier's pyAgrum
            # BUILD_SHARED_LIBS=ON exercise. Never under pyAgrum's normal static build,
            # where agrumCN is a plain archive and both linkers reject a .def input.
            # MSVC and MinGW alike need it: GNU ld's "auto-export everything" fallback
            # only applies when NO symbol in the link is explicitly dllexport-tagged --
            # agrumCN's own GUM_PUBLIC_CN-tagged classes are (BUILD_SHARED_LIBS=ON, see
            # the blanking guard below), which switches ld to explicit-only mode and
            # drops lrslib's untagged C symbols (e.g. checkindex, LNK/undefined
            # reference from LrsWrapper_tpl.h) same as MSVC without the .def.
            if (OPTION STREQUAL "CN" AND WIN32 AND (MSVC OR MINGW) AND (BUILD_SHARED_LIBS OR NOT BUILD_PYTHON))
                target_sources (agrumCN PRIVATE ${AGRUM_SOURCE_DIR}/agrum/CN/polytope/lrslib_windows.def)
            endif ()

            # GUM_PUBLIC blanking applies to every module under BUILD_PYTHON, not just
            # BASE/BN: default visibility is "visible" for pyAgrum builds (no
            # -fvisibility=hidden project-wide anymore, see CompilOptions.agrum.cmake
            # and the note below at the LIST_OF_MODULES loop), so a class tagged
            # GUM_PUBLIC only (public C++ API, not needed by pyAgrum -- as opposed to
            # PYGUM_PUBLIC/PYGUM_SHARED_PUBLIC) would otherwise still get re-exposed by
            # that macro in *any* module's .so/.pyd, not just the core's. Every module
            # has since been migrated to its own GUM_PUBLIC_<MODULE> name, so no header
            # uses plain GUM_PUBLIC anymore; this blanking is kept as a safety net so a
            # future GUM_PUBLIC use (before it gets its own per-module name) can never
            # leak a symbol into pyAgrum by accident, regardless of which module adds it.
            if (BUILD_PYTHON)
                target_compile_definitions (agrum${OPTION} PRIVATE GUM_PUBLIC=)

                # GUM_SHARED_PUBLIC is deliberately NEVER blanked here, unlike GUM_PUBLIC
                # above: it is not a "not needed by pyAgrum" tag, it is the producer/
                # consumer split for symbols pyAgrum's leaf modules DO need (config.h.in),
                # exactly like PYGUM_SHARED_PUBLIC -- which is also never blanked anywhere
                # in this file. Blanking it here would define the macro empty on agrumBASE's
                # own target before config.h.in's #ifndef guard ever runs, so BASE --
                # GUM_SHARED_PUBLIC's own producer, see config.h.in -- would silently compile
                # every GUM_SHARED_PUBLIC-tagged symbol (KNML, CachedContingencyCounter,
                # IndependenceTest, Score/Prior/ParamEstimator/GraphChange, ...) as
                # hidden-visibility on every platform, GUM_SHARED_EXPORTING below
                # notwithstanding: undefined symbol at leaf-module dlopen time (e.g.
                # KNML::clear from pyagrum.influence_diagram), silent on macOS's lazy binding.

                # Same blanking for the remaining 7 modules' GUM_PUBLIC_<MODULE>
                # names (config.h.in) -- BASE excluded, it no longer uses that
                # family (renamed to GUM_SHARED_PUBLIC above). Only applied under
                # BUILD_SHARED_LIBS=OFF (pyAgrum's normal static build): there it's
                # a harmless safety net (config.h.in's plain-undecorated fallback
                # applies anyway, no real DLL boundary between agrum<MODULE> and
                # its own leaf .pyd) -- EXCEPT for BN, which -- like BASE -- is
                # whole-archived into core _pyagrumcpp.pyd rather than linked
                # normally into its own leaf .pyd, so it crosses that same real
                # DLL boundary to every OTHER leaf module even under
                # BUILD_SHARED_LIBS=OFF. The loop below used to blank
                # GUM_PUBLIC_BN= on agrumBN's own target too (it only excluded
                # "BASE", not the module currently being configured), silently
                # short-circuiting config.h.in's #ifndef guard before
                # AGRUM_BN_EXPORTING ever got a chance to apply dllexport --
                # every GUM_PUBLIC_BN-tagged class (BarrenNodesFinder,
                # IBNLearner, GreedyHillClimbing, DAG2BNLearner, Score*,
                # StructuralConstraint*...) compiled as a plain, unexported
                # symbol, invisible to every leaf .pyd's dllimport reference
                # (LNK2019, confirmed on CI 2026-09-26 -- see
                # md_docs/backShared.md). Harmless on GNU/Linux/macOS, where the
                # compiler's own default visibility is "visible" now that
                # -fvisibility=hidden is gone project-wide for static pyAgrum
                # builds (CompilOptions.agrum.cmake, commit 65d695b8b, same
                # night): that flag turned out fundamentally incompatible with
                # this project's _tpl.h convention -- it hid vague-linkage
                # symbols (typeinfo/vtable) of template classes even under an
                # explicit GUM_PUBLIC_<MODULE> tag, confirmed via readelf -sW,
                # with #pragma GCC visibility push(default) around the explicit
                # instantiation confirmed as a dead end (still WEAK HIDDEN) --
                # see md_docs/backShared.md for the full investigation. Do not
                # reintroduce it without solving that first. Catastrophic on
                # MSVC, which never exports anything without an
                # explicit dllexport. Skipped under BUILD_SHARED_LIBS=ON, where
                # each agrum<MODULE> is now a genuine separate DLL/.so and needs
                # its own GUM_PUBLIC_<MODULE>-tagged symbols actually exported --
                # blanking would define the macro empty on the command line,
                # short-circuiting config.h.in's #ifndef guard and permanently
                # hiding that module's own AGRUM_<MODULE>_EXPORTING dllexport
                # (LNK1181: no .lib produced at all once every tagged symbol is
                # hidden, or LNK2019 for symbols still referenced cross-module,
                # e.g. gum::Separation from CM, referenced by CausalImpact).
                # LIST_OF_MODULES (computed above, exported to parent scope) lists all 8.
                if (NOT BUILD_SHARED_LIBS)
                    foreach (BLANK_MODULE ${LIST_OF_MODULES})
                        if (NOT BLANK_MODULE STREQUAL "BASE" AND NOT BLANK_MODULE STREQUAL "BN")
                            target_compile_definitions (agrum${OPTION} PRIVATE GUM_PUBLIC_${BLANK_MODULE}=)
                        endif ()
                    endforeach ()
                endif ()
            endif ()

            # GUM_SHARED_EXPORTING marks agrumBASE as the sole producer of
            # GUM_SHARED_PUBLIC-tagged symbols (config.h.in) for the Windows
            # dllexport/dllimport split. Unconditional (not gated on BUILD_PYTHON),
            # precisely to avoid the C4273 gap PYGUM_SHARED_EXPORTING hit when it was
            # gated that way (GUM_PUBLIC.md §13): BASE must see its own exporting flag
            # even in the TU that defines it, regardless of build flavor. BASE-only,
            # not AGRUM_${OPTION}_EXPORTING for every module, because only BASE
            # currently has a module-specific macro retagged and rolled out -- see
            # config.h.in's GUM_SHARED_PUBLIC comment for why the other 7 modules need
            # their own distinct name (not this one) once their turn comes.
            #
            # AGRUM_BASE_EXPORTING (separate from GUM_SHARED_EXPORTING above) exists
            # solely so GUM_COCOR_PUBLIC (config.h.in) has a flag that is genuinely
            # exclusive to compiling agrumBASE's own object files. GUM_SHARED_EXPORTING
            # is NOT exclusive to BASE: wrappers/pyagrum/CMakeLists.txt deliberately
            # also defines it on core's own SWIG wrap TU, because that TU whole-
            # archives agrumBASE and is a legitimate second "owner" of every
            # GUM_SHARED_PUBLIC symbol. Using GUM_SHARED_EXPORTING as GUM_COCOR_PUBLIC's
            # BASE-detection flag (tried 2026-09-26) made core's wrap TU match that
            # branch too whenever it locally reinstantiates a DIFFERENT module's
            # CoCo/R grammar (e.g. PRM's o3prm Parser, via GUM_NO_EXTERN_TEMPLATE_CLASS)
            # -- wrongly tagging it GUM_SHARED_PUBLIC/dllexport instead of falling
            # through to the GUM_PUBLIC_BN fallback, leaving 47 PRM AST symbols
            # (O3Type, O3Label, O3Position, ...) unresolved: LNK2019 on
            # windows_pyagrum_2022_{py310,py314} and windows_conda_forge (see
            # md_docs/backShared.md). AGRUM_BASE_EXPORTING has no such second owner.
            if (OPTION STREQUAL "BASE")
                target_compile_definitions (agrum${OPTION} PRIVATE GUM_SHARED_EXPORTING AGRUM_BASE_EXPORTING)
            else ()
                # Producer flag for GUM_PUBLIC_<MODULE> (config.h.in), wired for real
                # dllexport/dllimport on Windows under BUILD_SHARED_LIBS=ON -- see the
                # blanking guard above, which must stay skipped in that case or this
                # flag's dllexport gets short-circuited back to nothing.
                target_compile_definitions (agrum${OPTION} PRIVATE AGRUM_${OPTION}_EXPORTING)
            endif ()

            # AGRUM_BUILD_SHARED_LIBS (config.h.in, CompilOptions.agrum.cmake) tells
            # GUM_SHARED_PUBLIC/GUM_PUBLIC_<MODULE> whether there is an actual DLL
            # boundary to cross. It is set globally when BUILD_SHARED_LIBS=ON (aGrUM's
            # own standalone shared build), but pyAgrum forces BUILD_SHARED_LIBS=OFF
            # unconditionally (every agrum${OPTION} here is a plain static library) even
            # though a real shared boundary still exists one level up: core _pyagrumcpp.so
            # is always built SHARED (wrappers/pyagrum/CMakeLists.txt) and whole-archives
            # BASE/BN, while leaf .pyd's link against it as a genuine DLL. Every
            # agrum${OPTION} target must see the same signal core's own SWIG wrap TU
            # already gets (wrappers/pyagrum/CMakeLists.txt), or the split becomes
            # inconsistent between "this is the defining TU" and "this is a mere
            # declarer": BASE's own compilation would emit a plain, undecorated
            # definition (empty-macro fallback) while core's wrap TU still claims
            # dllexport for the same symbol -- an inline-vs-exported mismatch MSVC
            # reports as LNK2005 (e.g. _hashTableLog2_, genuinely inline there --
            # AGRUM_INLINE defaults ON off MinGW/Debug, CompilOptions.agrum.cmake) -- or
            # BASE's plain definition never reaches core's real export table at all,
            # leaving a leaf .pyd's correct dllimport with nothing to resolve against
            # (LNK2019, e.g. the SortedPriorityQueue end-marker statics).
            if (BUILD_PYTHON)
                target_compile_definitions (agrum${OPTION} PRIVATE AGRUM_BUILD_SHARED_LIBS)
            endif ()

            # PYGUM_SHARED_EXPORTING marks agrumBASE/agrumBN as the true owner of every
            # PYGUM_SHARED_PUBLIC-tagged symbol (config.h.in) for the Windows dllexport/
            # dllimport split: only these two targets' own object files -- whole-archived
            # into core _pyagrumcpp (and core's own SWIG wrap TU, see
            # wrappers/pyagrum/CMakeLists.txt) -- may dllexport them. Every other consumer
            # (leaf modules PRM/CN/ID/MRF/CM/KTBN) sees dllimport instead, so it references
            # core's/BN's exported copy instead of emitting its own duplicate definition
            # (LNK2005 on MSVC). Unlike GUM_PUBLIC above, this stays BASE/BN-only: it is
            # specifically the whole-archive producer/consumer split, which only BASE/BN
            # need -- a leaf module's own symbols use PYGUM_PUBLIC (unconditional dllexport,
            # no split) instead.
            #
            # NOT BUILD_SHARED_LIBS is deliberate here: this flag also feeds GUM_SHARED_PUBLIC
            # (config.h.in's `defined(GUM_FOR_SWIG) && defined(PYGUM_SHARED_EXPORTING)` half of
            # its dllexport condition), BASE's own real macro now that BASE is fully retagged
            # (no more PYGUM_SHARED_PUBLIC sites left). Under BUILD_SHARED_LIBS=OFF, BASE+BN
            # truly are one link unit (whole-archived into core), so BN claiming dllexport for
            # BASE-owned GUM_SHARED_PUBLIC symbols is correct and necessary (same reasoning as
            # PYGUM_SHARED_PUBLIC always was). Under BUILD_SHARED_LIBS=ON, BASE and BN are two
            # real, separate DLLs/.so's (Modules.agrum.cmake's per-module add_library() above,
            # normal target_link_libraries() in the dependency loop below) -- BN must see
            # dllimport for BASE's GUM_SHARED_PUBLIC symbols like any other consumer, not
            # dllexport. Leaving this flag posed on BN there made every BASE-owned
            # GUM_SHARED_PUBLIC symbol BN's own .obj files call into (e.g. GammaLog2::
            # _small_values_, the _static_*_end_* sentinels) resolve to a local dllexport
            # declaration with no matching definition in agrumBN itself -- LNK2019/LNK2001,
            # confirmed on CI (pipeline 2882904035) once BASE/BN actually became separate
            # libraries. BASE itself does not need this flag under BUILD_SHARED_LIBS=ON either
            # (GUM_SHARED_EXPORTING, unconditional below, already covers it) -- so this whole
            # block is now scoped to the one case where BASE+BN genuinely share a link unit.
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

# ==========================================================================
# GUM_PUBLIC_<MODULE> dllexport/dllimport macros (config.h.in): identical
# for every non-BASE module, differing only in the module name -- generated
# once from this template instead of hand-copied per module. Adding a 9th
# module used to mean copy-pasting a ~30-line block and wiring its
# AGRUM_<MODULE>_EXPORTING flag by hand; any typo or skipped branch
# (dllexport vs dllimport, the WIN32 static-vs-shared split) reproduces the
# exact class of LNK1181/LNK2019 bug this project spent many commits
# chasing module by module (see md_docs/backShared.md). GUM_SHARED_PUBLIC
# (BASE's own macro) is structurally different and stays hand-written in
# config.h.in.
#
# Must run before src/CMakeLists.txt's configure_file(config.h.in ...), so
# it only needs MODULES (set by modules.txt, included earlier), not
# LIST_OF_MODULES (only ready once buildFileListsWithModules() runs, later).
# ==========================================================================
set(_GUM_PUBLIC_MODULE_TEMPLATE [[
#ifndef GUM_PUBLIC_@@MOD@@
#  if defined(_WIN32) && defined(AGRUM_BUILD_SHARED_LIBS)
#    if defined(AGRUM_@@MOD@@_EXPORTING)
#      define GUM_PUBLIC_@@MOD@@ __declspec(dllexport)
#    else
#      define GUM_PUBLIC_@@MOD@@ __declspec(dllimport)
#    endif
#  elif defined(_WIN32)
     // No real DLL boundary to cross (BUILD_SHARED_LIBS=OFF, see
     // CompilOptions.agrum.cmake): plain, undecorated symbol for every TU that
     // sees this class/function, producer and consumer alike -- exactly how it
     // already behaves on non-Windows platforms when not hidden. Using
     // dllexport here instead (tried first, reverted) forces MSVC to eagerly
     // instantiate and emit a full, non-weak copy of every implicit special
     // member (default ctor/dtor/copy/assign) of a tagged class in *every*
     // static library that merely includes its header, not just the one
     // library that actually owns it -- multiple such libraries linked into
     // the same final binary (e.g. a pyAgrum leaf .pyd linking both its own
     // agrum<MODULE>.lib and core's import library) then collide as
     // LNK2005 duplicate definitions. Plain (no decoration at all) keeps
     // ordinary lazy, COMDAT/weak instantiation -- safe to duplicate and let
     // the linker fold, exactly like any normal template or inline method.
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
endmacro(generateGumPublicModuleMacros)
