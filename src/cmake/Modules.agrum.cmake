#first option is BUILD_ALL
option(BUILD_ALL "" OFF)

#creating all options
foreach(OPTION ${MODULES})
  option(BUILD_${OPTION} "build module ${OPTION}" OFF)
endforeach()

############### DEPENDENCIES BETWEEN OPTIONS ################
message(STATUS "** aGrUM Notification: Checking dependencies ...")
set(CHECK_DEPS 1)
while ( ${CHECK_DEPS} EQUAL 1)
  set(CHECK_DEPS 0)

  foreach(OPTION ${MODULES})
    if (BUILD_${OPTION})
      if (${OPTION}_DEPS)
        foreach(DEP ${${OPTION}_DEPS})
          if (BUILD_${DEP})
          else ()
            set(BUILD_${DEP} "ON")
            set(CHECK_DEPS 1)
            message(STATUS "  (+) adding ${DEP}")
          endif()
        endforeach()
      endif()
    endif()
  endforeach()
endwhile()

############### CONSTRAINTS BETWEEN OPTIONDS ################
# we want that
#  -if BUILD_ALL=ON, other ON is non important (just FYI because nothing to do),
#  -if there is at least one module = OFF, then BUILD_ALL is OFF
#  -if no module is ON then BUILD_ALL is ON
set(NBR_OPTIONS 0)
set(TOTAL_OPTIONS 0)
foreach(OPTION ${MODULES})
  math(EXPR TOTAL_OPTIONS "${TOTAL_OPTIONS}+1")
  if (BUILD_${OPTION})
    math(EXPR NBR_OPTIONS "${NBR_OPTIONS}+1")
  endif()
endforeach()

if (NBR_OPTIONS EQUAL 0 OR NBR_OPTIONS EQUAL TOTAL_OPTIONS)
  set(BUILD_ALL "ON")
else()
  set(BUILD_ALL "OFF")
endif()

# modules are all options (except ALL) + BASE module
set(LIST_OF_MODULES "BASE" ${MODULES})

# creating the lists of files by module
foreach(MODULE ${LIST_OF_MODULES})
  set(AGRUM_${MODULE}_SOURCES "")
  set(AGRUM_${MODULE}_INCLUDES "")
  set(AGRUM_${MODULE}_INLINES "")
  set(AGRUM_${MODULE}_TEMPLATES "")
  set(AGRUM_${MODULE}_C_SOURCES "")
  foreach(DIR ${${MODULE}_DIRS})
    file(GLOB_RECURSE LOOP_SRC ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/${DIR}/*.cpp)
    set(AGRUM_${MODULE}_SOURCES ${AGRUM_${MODULE}_SOURCES} ${LOOP_SRC})

    file(GLOB_RECURSE LOOP_HEADER ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/${DIR}/*.h)
    set(AGRUM_${MODULE}_INCLUDES ${AGRUM_${MODULE}_INCLUDES} ${LOOP_HEADER})
  endforeach()
endforeach()

#credal networks has a special case for C files
if (BUILD_CN OR BUILD_ALL)
  file(GLOB_RECURSE AGRUM_CN_C_SOURCES ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/external/lrslib/lrslib.c ${AGRUM_SOURCE_DIR}/agrum/external/lrslib/lrsmp.c)
endif()

# we always add the BASE module
set(AGRUM_SOURCES ${AGRUM_BASE_SOURCES})
set(AGRUM_C_SOURCES ${AGRUM_BASE_C_SOURCES})
set(AGRUM_INCLUDES ${AGRUM_BASE_INCLUDES})

if(BUILD_ALL)
  message(STATUS "** aGrUM Notification: Building all")
else()
  message(STATUS "** aGrUM Notification: Building specific modules")
endif()

#this macro has to be executed when recolt of module file lists is finished (after CocoR targets for instance)
macro(buildFileListsWithModules)
  message(STATUS "** aGrUM Notification: Generating files lists")
  foreach(OPTION ${MODULES})
    if (BUILD_${OPTION} OR BUILD_ALL)
      message(STATUS "** aGrUM Notification:      (+) adding ${OPTION}")
      set(AGRUM_SOURCES ${AGRUM_SOURCES} ${AGRUM_${OPTION}_SOURCES})
      set(AGRUM_INCLUDES ${AGRUM_INCLUDES} ${AGRUM_${OPTION}_INCLUDES})
      set(AGRUM_C_SOURCES ${AGRUM_C_SOURCES} ${AGRUM_${OPTION}_C_SOURCES})
    endif()
  endforeach()
endmacro(buildFileListsWithModules)
