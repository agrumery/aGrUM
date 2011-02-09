find_package(Doxygen)

IF (DOXYGEN_FOUND)
	SET(DOXYGEN_INPUT ${AGRUM_BINARY_DIR}/agrum.doxy ${AGRUM_BINARY_DIR}/piedpage_agrum.html ${AGRUM_BINARY_DIR}/agrum.css)
	SET(DOXYGEN_OUTPUT_DIR "${AGRUM_BINARY_DIR}/docs/html")
	SET(DOXYGEN_OUTPUT "${DOXYGEN_OUTPUT_DIR}/index.html")

	configure_file("${AGRUM_SOURCE_DIR}/docs/agrum.css.in" "${AGRUM_BINARY_DIR}/agrum.css")
	configure_file("${AGRUM_SOURCE_DIR}/docs/piedpage_agrum.html.in" "${AGRUM_BINARY_DIR}/piedpage_agrum.html")
	configure_file("${AGRUM_SOURCE_DIR}/docs/agrum.doxy.in" "${AGRUM_BINARY_DIR}/agrum.doxy")

	add_custom_command(OUTPUT ${DOXYGEN_OUTPUT_DIR}
		COMMAND ${CMAKE_COMMAND}
		ARGS -E make_directory ${DOXYGEN_OUTPUT_DIR}
	)

	add_custom_command(
	 OUTPUT ${DOXYGEN_OUTPUT}
	 COMMAND ${CMAKE_COMMAND} -E echo_append "Building Documentation..."
	 COMMAND ${CMAKE_COMMAND} -E copy ${AGRUM_BINARY_DIR}/agrum.css ${AGRUM_BINARY_DIR}/docs/html/agrum.css
	 COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
	 COMMAND ${CMAKE_COMMAND} -E echo "Done."
	 WORKING_DIRECTORY ${AGRUM_BINARY_DIR}
	 DEPENDS ${DOXYGEN_INPUT} ${DOXYGEN_OUTPUT_DIR}
	 )

	add_custom_target(doc DEPENDS ${DOXYGEN_OUTPUT})
  add_dependencies(doc ${AGRUM_SOURCES})
ELSE (DOXYGEN_FOUND)
    message(WARNING "Doxygen not found : you cannot generate documentation !")
ENDIF (DOXYGEN_FOUND)