# add *.cpp to AGRUM_SOURCES
file(GLOB_RECURSE AGRUM_SOURCES RELATIVE ${AGRUM_SOURCE_DIR}/agrum/ *.cpp)
file(GLOB_RECURSE AGRUM_INCLUDES RELATIVE ${AGRUM_SOURCE_DIR}/agrum/ *.h)
file(GLOB_RECURSE AGRUM_INLINES RELATIVE ${AGRUM_SOURCE_DIR}/agrum/ *.inl)
file(GLOB_RECURSE AGRUM_TEMPLATES RELATIVE ${AGRUM_SOURCE_DIR}/agrum/ *.tcc)

## macro in src/cmake/FlexBison.cmake
#FlexBisonCppTarget(BIFparser BN/io BIF)
FlexBisonTarget(XmlCsvParser learning parseXmlCsv)

###################################################################################
# Include binary directory to include lexers and parsers
include_directories(${AGRUM_BINARY_DIR}/agrum)

###################################################################################
add_library(agrum SHARED ${AGRUM_SOURCES})
set_target_properties(agrum PROPERTIES VERSION ${AGRUM_VERSION} SOVERSION ${AGRUM_VERSION_MAJOR})

install(CODE "MESSAGE(\"\n\n************************\")")
install(CODE "MESSAGE(\"* Installing Agrum ... *\")")
install(CODE "MESSAGE(\"************************\n\n\")")
install(DIRECTORY . DESTINATION include/agrum FILES_MATCHING PATTERN ".svn" EXCLUDE PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.tcc" PATTERN "*.inl")
install(TARGETS agrum DESTINATION lib)
