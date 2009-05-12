# FlexBisonTarget(Directory FileNameScheme)
macro(FlexBisonTarget TARGETNAME DIRNAME FILENAME)
  ################################################################################################################
  # Create target for the parser
  add_custom_target(${TARGETNAME} echo "\n\n ******************* \n Creating ${TARGETNAME} (${DIRNAME})\n ******************* \n")
  # Create custom command for flex/lex (note the outputs)
  add_custom_command(
    SOURCE ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${TARGETNAME}/${FILENAME}.lpp
    COMMAND ${FLEX_EXECUTABLE}
    ARGS -o${AGRUM_BINARY_DIR}/agrum/${FILENAME}_lex.cpp
          ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${TARGETNAME}/${FILENAME}.lpp
    TARGET ${TARGETNAME}
    OUTPUTS ${AGRUM_BINARY_DIR}/agrum/${FILENAME}_lex.cpp)

  # Create custom command for bison/yacc (note the DEPENDS)
  add_custom_command(
    SOURCE ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${TARGETNAME}/${FILENAME}.ypp
    COMMAND ${BISON_EXECUTABLE}
    ARGS -y ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${TARGETNAME}/${FILENAME}.ypp
          -o ${AGRUM_BINARY_DIR}/agrum/${FILENAME}.cpp
    TARGET ${TARGETNAME}
    DEPENDS ${AGRUM_BINARY_DIR}/agrum/${FILENAME}_lex.cpp
    OUTPUTS ${AGRUM_BINARY_DIR}/agrum/${FILENAME}.cpp)

  # Add parser to the list of sources
  set(AGRUM_SOURCES ${AGRUM_SOURCES} ${AGRUM_BINARY_DIR}/agrum/${FILENAME}.cpp ${AGRUM_BINARY_DIR}/agrum/${FILENAME}_lex.cpp)

  # Since parser does not exists yet when cmake is run, mark it as generated
  set_source_files_properties(${AGRUM_BINARY_DIR}/agrum/${FILENAME}.cpp GENERATED)
  set_source_files_properties(${AGRUM_BINARY_DIR}/agrum/${FILENAME}.hpp GENERATED)
  set_source_files_properties(${AGRUM_BINARY_DIR}/agrum/${FILENAME}_lex.cpp GENERATED)
endmacro (FlexBisonTarget)

# FlexBisonCppTarget(Directory FileNameScheme) for object-oriented version
macro(FlexBisonCppTarget TARGETNAME DIRNAME FILENAMEPREFIX)
  ################################################################################################################
  # Create target for the parser
  add_custom_target(${TARGETNAME} echo "\n\n ******************* \n Creating ${TARGETNAME} (${DIRNAME})\n ******************* \n")
  # Create custom command for flex/lex (note the outputs)
  add_custom_command(
    SOURCE ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${TARGETNAME}/${FILENAMEPREFIX}Scanner.lpp
    COMMAND ${FLEX_EXECUTABLE}
    ARGS -o${AGRUM_BINARY_DIR}/agrum/${FILENAMEPREFIX}Scanner.cpp
          ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${TARGETNAME}/${FILENAMEPREFIX}Scanner.lpp
    TARGET ${TARGETNAME}
    OUTPUTS ${AGRUM_BINARY_DIR}/agrum/${FILENAMEPREFIX}Scanner.cpp)

  # Create custom command for bison/yacc (note the DEPENDS)
  add_custom_command(
    SOURCE ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${TARGETNAME}/${FILENAMEPREFIX}Parser.ypp
    COMMAND ${BISON_EXECUTABLE}
    ARGS -y ${AGRUM_SOURCE_DIR}/agrum/${DIRNAME}/${TARGETNAME}/${FILENAMEPREFIX}Parser.ypp
          -o ${AGRUM_BINARY_DIR}/agrum/${FILENAMEPREFIX}Parser.cpp
    TARGET ${TARGETNAME}
    DEPENDS ${AGRUM_BINARY_DIR}/agrum/${FILENAMEPREFIX}Scanner.cpp
    OUTPUTS ${AGRUM_BINARY_DIR}/agrum/${FILENAMEPREFIX}Parser.cpp)

  # Add parser to the list of sources
  set(AGRUM_SOURCES ${AGRUM_SOURCES} ${AGRUM_BINARY_DIR}/agrum/${FILENAMEPREFIX}Parser.cpp ${AGRUM_BINARY_DIR}/agrum/${FILENAMEPREFIX}Scanner.cpp)

  # Since parser does not exists yet when cmake is run, mark it as generated
  set_source_files_properties(${AGRUM_BINARY_DIR}/agrum/${FILENAMEPREFIX}Parser.cpp GENERATED)
  set_source_files_properties(${AGRUM_BINARY_DIR}/agrum/${FILENAMEPREFIX}Parser.hpp GENERATED)
  set_source_files_properties(${AGRUM_BINARY_DIR}/agrum/${FILENAMEPREFIX}Scanner.cpp GENERATED)
endmacro (FlexBisonCppTarget)