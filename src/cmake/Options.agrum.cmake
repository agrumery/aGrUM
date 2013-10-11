
#first option is BUILD_ALL
option(BUILD_ALL "" OFF)

#creating all options
foreach(OPTION ${MODULES})
  option(BUILD_${OPTION} "build module ${OPTION}" OFF)
endforeach()

############### DEPENDENCIES BETWEEN OPTIONS ################
message(STATUS "Checking dependencies :")
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
