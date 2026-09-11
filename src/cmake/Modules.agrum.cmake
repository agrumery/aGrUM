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

if (NBR_OPTIONS EQUAL 0 OR NBR_OPTIONS EQUAL TOTAL_OPTIONS)
    set(BUILD_ALL "ON")
else ()
    set(BUILD_ALL "OFF")
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
            # in *any* module's .so/.pyd, not just the core's. Currently a no-op (0
            # standalone GUM_PUBLIC tag anywhere in src/agrum/ -- see GUM_PUBLIC.md,
            # Phase 6 not started), but scoping it to _IS_BASE_OR_BN only would silently
            # leave a future leaf module's GUM_PUBLIC-only symbols exported under
            # pyAgrum, contrary to the documented GUM_PUBLIC.md §3.1 semantics.
            if (BUILD_PYTHON)
                target_compile_definitions (agrum${OPTION} PRIVATE GUM_PUBLIC=)
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
