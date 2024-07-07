#enable_testing()
find_package(Python ${PYAGRUM_REQUIRED_PYTHON_VERSION} COMPONENTS Interpreter)

set(CXXTESTPATH ${AGRUM_SOURCE_DIR}/testunits/cxxtest-3.10-1)
set(GUMTESTPATH ${AGRUM_SOURCE_DIR}/testunits/gumtest)
set(CXXTESTGEN ${CXXTESTPATH}/cxxtestgen.py)

macro(add_agrum_test NAME)
    set(PATH_FILES "")
    foreach (part ${ARGN})
        set(PATH_FILES "testunits/${part}" ${PATH_FILES})
    endforeach (part ${ARGN})
    add_custom_command(
            OUTPUT ${AGRUM_BINARY_DIR}/${NAME}.cpp
            COMMAND
            ${Python_EXECUTABLE} ${CXXTESTGEN}
            --runner=AgrumErrorPrinter
            -o ${AGRUM_BINARY_DIR}/${NAME}.cpp ${ARGN}
            DEPENDS ${PATH_FILES}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testunits
    )

    add_executable(${NAME} EXCLUDE_FROM_ALL ${AGRUM_BINARY_DIR}/${NAME}.cpp ${PATH_FILES})
    target_include_directories(${NAME} PRIVATE "${CXXTESTPATH};${GUMTESTPATH};${AGRUM_SOURCE_DIR}/testunits/cxxtest;${AGRUM_SOURCE_DIR}/testunits")
    add_test(NAME cxx_quick COMMAND ${NAME})
endmacro()

if (Python_FOUND)
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/testunits/testList.cmake)
        include(testunits/testList.cmake)
    else ()
        file(GLOB_RECURSE AGRUM_TESTS
                RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/testunits/
                ${CMAKE_CURRENT_SOURCE_DIR}/testunits/*TestSuite.h)
    endif ()
    add_agrum_test(gumTest ${AGRUM_TESTS})
    if (GUM_COVERAGE)
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            set_target_properties(gumTest PROPERTIES COMPILE_FLAGS " --coverage")
            target_link_libraries(gumTest ${LIBAGRUM} "gcov")
        else ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            message(FATAL_ERROR "** Configuration error: code coverage is only supported with GNU GCC.")
        endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    else ()
        target_link_libraries(gumTest ${LIBAGRUM})
    endif ()
else ()
    message(WARNING "** aGrUM Warning: Python not found : you cannot generate tests !")
endif ()
