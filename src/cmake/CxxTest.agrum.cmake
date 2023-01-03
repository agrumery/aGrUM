#enable_testing()
find_package(Python ${PYAGRUM_REQUIRED_PYTHON_VERSION} COMPONENTS Interpreter)

set(CXXTESTPATH ${AGRUM_SOURCE_DIR}/testunits/cxxtest-3.10-1)
set(GUMTESTPATH ${AGRUM_SOURCE_DIR}/testunits/gumtest)
set(CXXTESTGEN ${CXXTESTPATH}/cxxtestgen.py)

include_directories(${CXXTESTPATH})
include_directories(${GUMTESTPATH})
include_directories(${AGRUM_SOURCE_DIR}/testunits/cxxtest)
include_directories(${AGRUM_SOURCE_DIR}/testunits)

MACRO(ADD_CXXTEST NAME)
    IF (Python_FOUND)
        set(PATH_FILES "")
        foreach (part ${ARGN})
            set(PATH_FILES "testunits/${part}" ${PATH_FILES})
        endforeach (part ${ARGN})
        ADD_CUSTOM_COMMAND(
                OUTPUT ${AGRUM_BINARY_DIR}/${NAME}.cpp
                COMMAND
                ${Python_EXECUTABLE} ${CXXTESTGEN}
                --runner=AgrumErrorPrinter
                -o ${AGRUM_BINARY_DIR}/${NAME}.cpp ${ARGN}
                DEPENDS ${PATH_FILES}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testunits
        )
    ENDIF ()

    ADD_EXECUTABLE(${NAME} EXCLUDE_FROM_ALL ${AGRUM_BINARY_DIR}/${NAME}.cpp ${PATH_FILES})

    ADD_TEST(${NAME} ${NAME})
ENDMACRO(ADD_CXXTEST)


MACRO(add_agrum_test NAME)
    set(PATH_FILES "")
    foreach (part ${ARGN})
        set(PATH_FILES "testunits/${part}" ${PATH_FILES})
    endforeach (part ${ARGN})
    ADD_CUSTOM_COMMAND(
            OUTPUT ${AGRUM_BINARY_DIR}/${NAME}.cpp
            COMMAND
            ${Python_EXECUTABLE} ${CXXTESTGEN}
            --runner=AgrumErrorPrinter
            -o ${AGRUM_BINARY_DIR}/${NAME}.cpp ${ARGN}
            DEPENDS ${PATH_FILES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testunits
    )

    ADD_EXECUTABLE(${NAME} EXCLUDE_FROM_ALL ${AGRUM_BINARY_DIR}/${NAME}.cpp ${PATH_FILES})
ENDMACRO(add_agrum_test NAME)

IF (Python_FOUND)
    IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/testunits/testList.cmake)
        INCLUDE(testunits/testList.cmake)
    ELSE ()
        FILE(GLOB AGRUM_TESTS
                RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/testunits/
                ${CMAKE_CURRENT_SOURCE_DIR}/testunits/*TestSuite.h)
    ENDIF ()
    add_agrum_test(gumTest ${AGRUM_TESTS})
    IF (GUM_COVERAGE)
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            set_target_properties(gumTest PROPERTIES COMPILE_FLAGS " --coverage")
            target_link_libraries(gumTest ${LIBAGRUM} "gcov")
        else ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            message(FATAL_ERROR "** Configuration error: code coverage is only supported with GNU GCC.")
        endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    ELSE ()
        target_link_libraries(gumTest ${LIBAGRUM})
    ENDIF (GUM_COVERAGE)
ELSE ()
    message(WARNING "** aGrUM Warning: Python not found : you cannot generate tests !")
ENDIF ()
