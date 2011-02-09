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
  include("../testunits/testList.cmake")
  add_agrum_test(test  ${AGRUM_TESTS})
  target_link_libraries(test ${LIBAGRUM})
ELSE(PYTHONINTERP_FOUND)
    message(WARNING "Python not found : you cannot generate tests !")
ENDIF(PYTHONINTERP_FOUND)
