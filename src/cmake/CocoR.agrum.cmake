set(RM ${CMAKE_COMMAND} -E remove)

find_program(COCOR_EXECUTABLE NAMES cococpp Coco)
if (COCOR_EXECUTABLE)
    message(STATUS "** aGrUM Notification: Coco/R found")
else ()
    message(STATUS "** aGrUM notification: Coco/R not found : you cannot change atg files !")
endif ()

macro(CocoRTarget DIRNAME ARG_PREFIX ARG_NAMESPACE MODULE)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h
            COMMAND ${COCOR_EXECUTABLE} -namespace ${ARG_NAMESPACE} -frames ${AGRUM_SOURCE_DIR}/agrum/base/core/cocoR ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${ARG_PREFIX}.atg -o ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/
            COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h.old
            COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp.old
            COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h.old
            COMMAND ${RM} -f ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp.old
            DEPENDS ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${ARG_PREFIX}.atg ${AGRUM_SOURCE_DIR}/agrum/base/core/cocoR/Scanner.frame ${AGRUM_SOURCE_DIR}/agrum/base/core/cocoR/Parser.frame ${AGRUM_SOURCE_DIR}/agrum/base/core/cocoR/Copyright.frame
    )

    set(_COCOR_SOURCES  ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.cpp)
    set(_COCOR_INCLUDES ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Parser.h ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/Scanner.h)
    set_source_files_properties(${_COCOR_SOURCES} ${_COCOR_INCLUDES} GENERATED)
    list(APPEND AGRUM_${MODULE}_SOURCES ${_COCOR_SOURCES})
    list(APPEND AGRUM_${MODULE}_INCLUDES ${_COCOR_INCLUDES})

    message(STATUS "** aGrUM notification:      (+) ${ARG_NAMESPACE} found")
endmacro()
# Since parser does not exists yet when cmake is run, mark it as generated
