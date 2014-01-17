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

macro(CocoRTarget DIRNAME ARG_PREFIX ARG_NAMESPACE ARG_MODULE)
  message(STATUS "** aGrUM notification : CocoR Target : ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME} ${ARG_PREFIX} ${ARG_NAMESPACE} ${ARG_MODULE}")
  ADD_CUSTOM_COMMAND(
    OUTPUT agrum/${DIRNAME}/Parser.cpp agrum/${DIRNAME}/Parser.h agrum/${DIRNAME}/Scanner.cpp agrum/${DIRNAME}/Scanner.h
    COMMAND ${COCOR_EXECUTABLE} -namespace ${ARG_NAMESPACE} -frames ${AGRUM_SOURCE_DIR}/agrum/core/cocoR ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${ARG_PREFIX}.atg -o ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h.old
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp.old
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h.old
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp.old
    DEPENDS ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${ARG_PREFIX}.atg ${AGRUM_SOURCE_DIR}/agrum/core/cocoR/Scanner.frame ${AGRUM_SOURCE_DIR}/agrum/core/cocoR/Parser.frame ${AGRUM_SOURCE_DIR}/agrum/core/cocoR/Copyright.frame
  )
  set(AGRUM_${ARG_MODULE}_SOURCES  agrum/${DIRNAME}/Parser.cpp agrum/${DIRNAME}/Scanner.cpp ${AGRUM_${ARG_MODULE}_SOURCES})
  set(AGRUM_${ARG_MODULE}_INCLUDES agrum/${DIRNAME}/Parser.h agrum/${DIRNAME}/Scanner.h ${AGRUM_${ARG_MODULE}_INCLUDES})

  SET_SOURCE_FILES_PROPERTIES(agrum/${DIRNAME}/Parser.cpp GENERATED)
  SET_SOURCE_FILES_PROPERTIES(agrum/${DIRNAME}/Parser.h GENERATED)
  SET_SOURCE_FILES_PROPERTIES(agrum/${DIRNAME}/Scanner.cpp GENERATED)
  SET_SOURCE_FILES_PROPERTIES(agrum/${DIRNAME}/Scanner.h GENERATED)
endmacro(CocoRTarget)
# Since parser does not exists yet when cmake is run, mark it as generated
