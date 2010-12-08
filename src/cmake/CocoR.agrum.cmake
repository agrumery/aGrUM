set(RM cmake -E remove)

macro(CocoRTarget DIRNAME ARG_PREFIX ARG_NAMESPACE)
	ADD_CUSTOM_COMMAND(
		OUTPUT ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h
		COMMAND ${COCOR_EXECUTABLE} -namespace ${ARG_NAMESPACE} -frames ${AGRUM_SOURCE_DIR}/agrum/core/cocoR ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${ARG_PREFIX}.atg -o ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/
		COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h.old
		COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp.old
		COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h.old
		COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp.old
		DEPENDS ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${ARG_PREFIX}.atg ${AGRUM_SOURCE_DIR}/agrum/core/cocoR/Scanner.frame ${AGRUM_SOURCE_DIR}/agrum/core/cocoR/Parser.frame ${AGRUM_SOURCE_DIR}/agrum/core/cocoR/Copyright.frame
	)
  SET(AGRUM_SOURCES ${AGRUM_SOURCES} ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h)

  # Since parser does not exists yet when cmake is run, mark it as generated
  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp GENERATED)
  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h GENERATED)
  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp GENERATED)
  SET_SOURCE_FILES_PROPERTIES(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h GENERATED)
endmacro(CocoRTarget)

