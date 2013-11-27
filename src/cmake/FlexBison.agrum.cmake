set(MV cmake -E rename)

set(BISON_FLEX_EXECUTABLE "BISON-FLEX not found")
find_program(FLEX_EXECUTABLE flex)
if(FLEX_EXECUTABLE STREQUAL "FLEX_EXECUTABLE-NOTFOUND")
  message(STATUS "** aGrUM notification : flex not found : you cannot change lex/yacc files !")
else(FLEX_EXECUTABLE STREQUAL "FLEX_EXECUTABLE-NOTFOUND")
    find_program(BISON_EXECUTABLE bison)
    if(BISON_EXECUTABLE STREQUAL "BISON_EXECUTABLE-NOTFOUND")
        message(STATUS "** aGrUM notification : bison not found : you cannot change lex/yacc files !")
    else(BISON_EXECUTABLE STREQUAL "BISON_EXECUTABLE-NOTFOUND")
      set(BISON_FLEX_EXECUTABLE "ok")
    endif(BISON_EXECUTABLE STREQUAL "BISON_EXECUTABLE-NOTFOUND")
endif(FLEX_EXECUTABLE STREQUAL "FLEX_EXECUTABLE-NOTFOUND")

# FlexBisonTarget(Directory FileNameScheme)
macro(FlexBisonTarget DIRNAME FILENAME)
  add_custom_command(
    OUTPUT ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}_lex.cpp
    SOURCE ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.lpp
    DEPENDS ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.hpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.lpp
    COMMAND ${FLEX_EXECUTABLE}
      ARGS -o${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}_lex.cpp
           ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.lpp
  )

  # Add parser to the list of sources
  set(AGRUM_SOURCES ${AGRUM_SOURCES} ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}_lex.cpp)

  # Since parser does not exists yet when cmake is run, mark it as generated
  set_source_files_properties(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}_lex.cpp GENERATED)

  add_custom_command(
    OUTPUT ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.cpp
           ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.hpp
    SOURCE ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.ypp
    DEPENDS ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.ypp
    COMMAND ${BISON_EXECUTABLE}
      ARGS -y ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.ypp
    COMMAND ${MV} ${FILENAME}.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.cpp
    COMMAND ${MV} ${FILENAME}.hpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.hpp
  )

  # Add parser to the list of sources
  set(AGRUM_SOURCES ${AGRUM_SOURCES} ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.cpp ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.hpp)

  # Since parser does not exists yet when cmake is run, mark it as generated
  set_source_files_properties(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.cpp GENERATED)
  set_source_files_properties(${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${FILENAME}.hpp GENERATED)
endmacro (FlexBisonTarget)


