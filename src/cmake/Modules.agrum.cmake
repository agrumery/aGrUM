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

            # BASE/BN are the two modules pyAgrum embeds once into core _pyagrum.so (whole-archived,
            # see wrappers/pyagrum/CMakeLists.txt) instead of re-linking per leaf module -- computed
            # once here since both the export split below and the dependency handling further down
            # branch on it.
            set (_IS_BASE_OR_BN OFF)
            if (OPTION STREQUAL "BASE" OR OPTION STREQUAL "BN")
                set (_IS_BASE_OR_BN ON)
            endif ()

            # pyAgrum forces static linkage project-wide (ActBuilderPyAgrum.check_consistency,
            # "Static library forced for [[pyAgrum]] target.") -- BASE/BN stay static here too:
            # they carry process-global state (e.g. CompleteProjectionRegister4MultiDim, used by
            # Tensor::min/max/sum/product) that must be a SINGLE instance shared by every pyAgrum
            # SWIG extension (_pyagrum.so, _mrf.so, _id.so, ...). Rather than making agrumBASE/BN
            # themselves shared libraries, they are embedded once into core pyagrum (_pyagrum.so,
            # built SHARED -- see wrappers/pyagrum/CMakeLists.txt), and every other extension links
            # against that single _pyagrum.so instead of re-embedding its own copy of BASE/BN.
            add_library (agrum${OPTION} ${AGRUM_${OPTION}_SOURCES} ${AGRUM_${OPTION}_C_SOURCES} ${AGRUM_${OPTION}_INCLUDES} ${AGRUM_BASE_INCLUDES})

            # GUM_PUBLIC blanking applies to every module under BUILD_PYTHON, not just
            # BASE/BN: -fvisibility=hidden is already active project-wide for pyAgrum
            # builds (CompilOptions.agrum.cmake), so a class tagged GUM_PUBLIC only
            # (public C++ API, not needed by pyAgrum -- as opposed to PYGUM_PUBLIC/
            # PYGUM_SHARED_PUBLIC) would otherwise still get re-exposed by that macro
            # in *any* module's .so/.pyd, not just the core's. Every module has since
            # been migrated to its own GUM_PUBLIC_<MODULE> name, so no header uses
            # plain GUM_PUBLIC anymore; this blanking is kept as a safety net so a
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
                # KNML::clear from pyagrum.id), silent on macOS's lazy binding.

                # Same blanking for the remaining 7 modules' still-placeholder
                # GUM_PUBLIC_<MODULE> names (config.h.in) -- BASE excluded, it no longer
                # uses that family (renamed to GUM_SHARED_PUBLIC above: BASE is the one
                # module with no dependencies of its own, so unlike BN/PRM/..., a single
                # unqualified name is safe for it -- see config.h.in's comment on
                # GUM_SHARED_PUBLIC for why this doesn't generalize to the other 7).
                # LIST_OF_MODULES (computed above, exported to parent scope) lists all 8.
                foreach (BLANK_MODULE ${LIST_OF_MODULES})
                    if (NOT BLANK_MODULE STREQUAL "BASE")
                        target_compile_definitions (agrum${OPTION} PRIVATE GUM_PUBLIC_${BLANK_MODULE}=)
                    endif ()
                endforeach ()
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
            if (OPTION STREQUAL "BASE")
                target_compile_definitions (agrum${OPTION} PRIVATE GUM_SHARED_EXPORTING)
            else ()
                # Placeholder producer flag for the not-yet-rolled-out GUM_PUBLIC_<MODULE>
                # scaffolding (config.h.in) -- unused until that module's tags are migrated.
                target_compile_definitions (agrum${OPTION} PRIVATE AGRUM_${OPTION}_EXPORTING)
            endif ()

            # AGRUM_BUILD_SHARED_LIBS (config.h.in, CompilOptions.agrum.cmake) tells
            # GUM_SHARED_PUBLIC/GUM_PUBLIC_<MODULE> whether there is an actual DLL
            # boundary to cross. It is set globally when BUILD_SHARED_LIBS=ON (aGrUM's
            # own standalone shared build), but pyAgrum forces BUILD_SHARED_LIBS=OFF
            # unconditionally (every agrum${OPTION} here is a plain static library) even
            # though a real shared boundary still exists one level up: core _pyagrum.so
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
            # into core _pyagrum (and core's own SWIG wrap TU, see
            # wrappers/pyagrum/CMakeLists.txt) -- may dllexport them. Every other consumer
            # (leaf modules PRM/CN/ID/MRF/CM) sees dllimport instead, so it references
            # core's exported copy instead of emitting its own duplicate definition
            # (LNK2005 on MSVC). Unlike GUM_PUBLIC above, this stays BASE/BN-only: it is
            # specifically the whole-archive producer/consumer split, which only BASE/BN
            # need -- a leaf module's own symbols use PYGUM_PUBLIC (unconditional
            # dllexport, no split) instead of PYGUM_SHARED_PUBLIC.
            if (BUILD_PYTHON AND _IS_BASE_OR_BN)
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
                if (BUILD_PYTHON AND NOT _IS_BASE_OR_BN)
                    # Leaf pyAgrum modules (PRM/MRF/CN/ID/CM) reach BASE/BN through core _pyagrum.so
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
                else ()
                    target_link_libraries (agrum${OPTION} PUBLIC agrum${DEP})
                endif ()
            endforeach()

        endif ()
    endforeach ()
endmacro(buildFileListsWithModules)
