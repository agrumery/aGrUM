set(RM cmake -E remove)

find_program(COCOR_EXECUTABLE cococpp)
if(COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")
  find_program(COCOR_EXECUTABLE2 Coco)
  if(COCOR_EXECUTABLE2 STREQUAL "COCOR_EXECUTABLE2-NOTFOUND")
    message(STATUS "** aGrUM notification : Coco/R not found : you cannot change atg files !")
  else(COCOR_EXECUTABLE2 STREQUAL "COCOR_EXECUTABLE2-NOTFOUND")
    set(COCOR_EXECUTABLE "${COCOR_EXECUTABLE2}")
  endif(COCOR_EXECUTABLE2 STREQUAL "COCOR_EXECUTABLE2-NOTFOUND")
endif(COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")

macro(CocoRTarget DIRNAME ARG_PREFIX ARG_NAMESPACE MODULE)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h
    COMMAND ${COCOR_EXECUTABLE} -namespace ${ARG_NAMESPACE} -frames ${AGRUM_SOURCE_DIR}/agrum/core/cocoR ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${ARG_PREFIX}.atg -o ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h.old
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp.old
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h.old
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp.old
    DEPENDS ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${ARG_PREFIX}.atg ${AGRUM_SOURCE_DIR}/agrum/core/cocoR/Scanner.frame ${AGRUM_SOURCE_DIR}/agrum/core/cocoR/Parser.frame ${AGRUM_SOURCE_DIR}/agrum/core/cocoR/Copyright.frame
  )
  set(AGRUM_${MODULE}_SOURCES  ${AGRUM_${MODULE}_SOURCES}  ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp)
  set(AGRUM_${MODULE}_INCLUDES ${AGRUM_${MODULE}_INCLUDES} ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h   ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h)

  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp GENERATED)
  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h GENERATED)
  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp GENERATED)
  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h GENERATED)
endmacro(CocoRTarget)
# Since parser does not exists yet when cmake is run, mark it as generated
