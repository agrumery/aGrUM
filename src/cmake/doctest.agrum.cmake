# doctest configuration for aGrUM tests

set(GUMTESTPATH ${AGRUM_SOURCE_DIR}/testunits/gumtest)
set(DOCTESTPATH ${AGRUM_SOURCE_DIR}/testunits/doctest)

# Collect all test files
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/testunits/testList.cmake)
    include(testunits/testList.cmake)
else ()
    file(GLOB_RECURSE AGRUM_TESTS
            RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/testunits/
            ${CMAKE_CURRENT_SOURCE_DIR}/testunits/module_*/*TestSuite.h)
endif ()

# Generate a file that includes all test headers
# This is necessary because doctest registers tests at compile time via macros
set(TEST_INCLUDES_FILE "${AGRUM_BINARY_DIR}/gumTestIncludes.cpp")
file(WRITE ${TEST_INCLUDES_FILE} "// Auto-generated file - includes all test headers\n")
file(APPEND ${TEST_INCLUDES_FILE} "// Do not edit manually\n\n")
foreach (test_file ${AGRUM_TESTS})
    file(APPEND ${TEST_INCLUDES_FILE} "#include <${test_file}>\n")
endforeach ()

# Add the main.cpp and the generated includes file
add_executable(gumTest EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/testunits/gumtest/main.cpp
    ${TEST_INCLUDES_FILE}
)

target_include_directories(gumTest PRIVATE
    "${DOCTESTPATH}"
    "${GUMTESTPATH}"
    "${AGRUM_SOURCE_DIR}/testunits"
)

add_test(NAME gumTest COMMAND gumTest)

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

# doctest command-line options:
# Run all tests:           ./gumTest
# List all tests:          ./gumTest --list-test-cases
# Run tests by filter:     ./gumTest --test-case="*BayesNet*"
# Run tests by tag:        ./gumTest --test-suite="*[BN]*"
# Run specific module:     ./gumTest --test-case="*[BASE]*"
