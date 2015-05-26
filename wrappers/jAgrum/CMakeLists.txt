
set(JAVA_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/lib)
make_directory(${JAVA_OUTDIR})

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

include_directories (${AGRUM_SOURCE_DIR})
include_directories (${AGRUM_BINARY_DIR})

# Properties
set(SWIG_MODULE_jAgrum_EXTRA_DEPS "../swig/aGrUM_wrap.i")
set(CMAKE_SWIG_OUTDIR ${JAVA_OUTDIR})
set_source_files_properties(jAgrum.i PROPERTIES CPLUSPLUS ON)

# Build
swig_add_module (jAgrum java jAgrum.i)
#for now we do not take for pyAgrum the ${AGRUM_LEARNING_SOURCES}
swig_link_libraries (jAgrum ${LIBAGRUM})

set_target_properties(jAgrum PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${JAVA_OUTDIR})

#set_directory_properties(${CMAKE_CURRENT_BINARY_DIR} ADDITIONAL_MAKE_CLEAN_FILES pyAgrum.py)

#install(FILES ${JAVA_OUTDIR}/pyAgrum.py DESTINATION ${PYTHON_INSTALL})
#install(FILES ${JAVA_OUTDIR}/_pyAgrum.so DESTINATION ${PYTHON_INSTALL})

