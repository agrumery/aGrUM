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
        target_compile_options(gumTest PRIVATE --coverage)
        target_link_libraries(gumTest ${LIBAGRUM} "gcov")
    else ()
        message(FATAL_ERROR "** Configuration error: code coverage is only supported with GNU GCC.")
    endif ()
else ()
    target_link_libraries(gumTest ${LIBAGRUM})
endif ()

# doctest uses __COUNTER__ to generate unique test-case identifiers. Clang flags
# __COUNTER__ as a C2y extension (-Wc2y-extensions) even though it is universally
# supported and the warning is purely cosmetic. Suppress it for the test binary only.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_compile_options(gumTest PRIVATE -Wno-c2y-extensions)
endif ()

# doctest command-line options:
# Run all tests:           ./gumTest
# List all tests:          ./gumTest --list-test-cases
# Run tests by filter:     ./gumTest --test-case="*BayesNet*"
# Run tests by tag:        ./gumTest --test-suite="*[BN]*"
# Run specific module:     ./gumTest --test-case="*[BASE]*"
