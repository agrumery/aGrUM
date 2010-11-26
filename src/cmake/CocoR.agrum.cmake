find_program(COCOR_EXECUTABLE cococpp)
if(COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")
    message(FATAL_ERROR "Please install Coco/R!")
endif(COCOR_EXECUTABLE STREQUAL "COCOR_EXECUTABLE-NOTFOUND")

macro(CocoRTarget DIRNAME PREFIX NAMESPACE)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h
    COMMAND ${COCOR_EXECUTABLE} ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${PREFIX}.atg -o ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h.old
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp.old
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h.old
    COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp.old
    DEPENDS ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${PREFIX}.atg ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.frame ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.frame
    )

  SET(AGRUM_SOURCES ${AGRUM_SOURCES} ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h)

  # Since parser does not exists yet when cmake is run, mark it as generated
  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp GENERATED)
  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h GENERATED)
  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp GENERATED)
  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h GENERATED)
endmacro(CocoRTarget)

