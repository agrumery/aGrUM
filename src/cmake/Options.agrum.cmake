# list all of options
set(OPTIONS "BN" "CN" "FMDP" "ID" "PRM")

#first option is BUILD_ALL
option(BUILD_ALL "" OFF)

#creating all options
foreach(OPTION ${OPTIONS})
  option(BUILD_${OPTION} "build module ${OPTION}" OFF)
endforeach()


############### CONSTRAINTS BETWEEN OPTIONDS ################
# we want that
#  -if BUILD_ALL=ON, other ON is non important (just FYI because nothing to do),
#  -if there is at least one module = OFF, then BUILD_ALL is OFF
#  -if no module is ON then BUILD_ALL is ON
set(NBR_OPTIONS 0)
set(TOTAL_OPTIONS 0)
foreach(OPTION ${OPTIONS})
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

############### DEPENDENCIES BETWEEN OPTIONS ################
if (BUILD_CN)
  set(BUILD_BN "ON")
endif()

if (BUILD_PRM)
  set(BUILD_BN "ON")
endif()