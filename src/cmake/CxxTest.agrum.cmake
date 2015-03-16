#enable_testing()
find_package(PythonInterp)

set(CXXTESTGEN ${CMAKE_SOURCE_DIR}/testunits/cxxtest/cxxtestgen.py)

include_directories(${AGRUM_SOURCE_DIR}/testunits/cxxtest)
include_directories(${AGRUM_SOURCE_DIR}/testunits)

MACRO(ADD_CXXTEST NAME)
  IF(PYTHONINTERP_FOUND)
    set(PATH_FILES "")
    foreach(part ${ARGN})
      set(PATH_FILES "testunits/${part}" ${PATH_FILES})
    endforeach(part ${ARGN})
    ADD_CUSTOM_COMMAND(
      OUTPUT ${AGRUM_BINARY_DIR}/${NAME}.cpp
      COMMAND
        ${PYTHON_EXECUTABLE} ${CXXTESTGEN}
        --runner=AgrumErrorPrinter
        -o ${AGRUM_BINARY_DIR}/${NAME}.cpp ${ARGN}
      DEPENDS ${PATH_FILES}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testunits
    )
  ENDIF(PYTHONINTERP_FOUND)

  ADD_EXECUTABLE(${NAME} EXCLUDE_FROM_ALL ${AGRUM_BINARY_DIR}/${NAME}.cpp ${PATH_FILES})

  ADD_TEST(${NAME} ${NAME})
ENDMACRO(ADD_CXXTEST)


MACRO(add_agrum_test NAME)
    set(PATH_FILES "")
    foreach(part ${ARGN})
      set(PATH_FILES "testunits/${part}" ${PATH_FILES})
    endforeach(part ${ARGN})
    ADD_CUSTOM_COMMAND(
      OUTPUT ${AGRUM_BINARY_DIR}/${NAME}.cpp
      COMMAND
        ${PYTHON_EXECUTABLE} ${CXXTESTGEN}
        --runner=AgrumErrorPrinter
        -o ${AGRUM_BINARY_DIR}/${NAME}.cpp ${ARGN}
      DEPENDS ${PATH_FILES}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testunits
    )

  ADD_EXECUTABLE(${NAME} EXCLUDE_FROM_ALL ${AGRUM_BINARY_DIR}/${NAME}.cpp ${PATH_FILES})
ENDMACRO(add_agrum_test NAME)

IF(PYTHONINTERP_FOUND)
  IF ( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/testunits/testList.cmake )
    INCLUDE ( testunits/testList.cmake )
  ELSE ()
# TODO : should be controlled using options (BUILD_...)
    FILE( GLOB AGRUM_TESTS
          RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/testunits/
          ${CMAKE_CURRENT_SOURCE_DIR}/testunits/*TestSuite.h )
  ENDIF ()
  add_agrum_test(gumTest  ${AGRUM_TESTS})
  set_target_properties(gumTest PROPERTIES COMPILE_FLAGS "-DGUM_RANDOMSEED=${GUM_RANDOMSEED}")
  target_link_libraries(gumTest ${LIBAGRUM})
ELSE(PYTHONINTERP_FOUND)
    message(WARNING "Python not found : you cannot generate tests !")
ENDIF(PYTHONINTERP_FOUND)
