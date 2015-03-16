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

    file(GLOB_RECURSE LOOP_INLINE ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/${DIR}/*.inl)
    set(AGRUM_${MODULE}_INLINES ${AGRUM_${MODULE}_INLINES} ${LOOP_INLINE})

    file(GLOB_RECURSE LOOP_TPL ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/${DIR}/*.tpl)
    set(AGRUM_${MODULE}_TEMPLATES ${AGRUM_${MODULE}_TEMPLATES} ${LOOP_TPL})
  endforeach()
endforeach()

#credal networks has a special case for C files
if (BUILD_CN OR BUILD_ALL)
  file(GLOB_RECURSE AGRUM_CN_C_SOURCES ${AGRUM_SOURCE_DIR} ${AGRUM_SOURCE_DIR}/agrum/core/math/lrslib/lrslib.c ${AGRUM_SOURCE_DIR}/agrum/core/math/lrslib/lrsmp.c)
endif()

# we always add the BASE module
set(AGRUM_SOURCES ${AGRUM_BASE_SOURCES})
set(AGRUM_C_SOURCES ${AGRUM_BASE_C_SOURCES})
set(AGRUM_INCLUDES ${AGRUM_BASE_INCLUDES})
set(AGRUM_INLINES ${AGRUM_BASE_INLINES})
set(AGRUM_TEMPLATES ${AGRUM_BASE_TEMPLATES})

if(BUILD_ALL)
  message(STATUS "Building all :")
else()
  message(STATUS "Building specific modules :")
endif()

foreach(OPTION ${MODULES})
  if (BUILD_${OPTION} OR BUILD_ALL)
    message(STATUS "  (*) building ${OPTION}")
    set(AGRUM_SOURCES ${AGRUM_SOURCES} ${AGRUM_${OPTION}_SOURCES})
    set(AGRUM_INCLUDES ${AGRUM_INCLUDES} ${AGRUM_${OPTION}_INCLUDES})
    set(AGRUM_INLINES ${AGRUM_INLINES} ${AGRUM_${OPTION}_INLINES})
    set(AGRUM_TEMPLATES ${AGRUM_TEMPLATES} ${AGRUM_${OPTION}_TEMPLATES})
    set(AGRUM_C_SOURCES ${AGRUM_C_SOURCES} ${AGRUM_${OPTION}_C_SOURCES})
  endif()
endforeach()
message("")
