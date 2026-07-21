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

# Generate one translation unit per test header instead of concatenating them
# all into a single .cpp file. doctest registers tests at compile time via
# macros that generate identifiers from __LINE__/__COUNTER__; those are only
# guaranteed unique within a single translation unit, so a unity build risks
# identifier collisions between unrelated test files that happen to share a
# line number (observed with Clang >= 22, where doctest falls back to
# __LINE__ instead of __COUNTER__). Compiling each header as its own .cpp
# keeps every TU independent while still linking into a single gumTest binary.
set(TEST_INCLUDES_DIR "${AGRUM_BINARY_DIR}/gumTestIncludes")
file(MAKE_DIRECTORY ${TEST_INCLUDES_DIR})
set(TEST_INCLUDES_FILES "")
foreach (test_file ${AGRUM_TESTS})
    string(REPLACE "/" "_" test_stub_name "${test_file}")
    set(test_stub_file "${TEST_INCLUDES_DIR}/${test_stub_name}.cpp")
    file(WRITE ${test_stub_file} "// Auto-generated file - do not edit manually\n#include <${test_file}>\n")
    list(APPEND TEST_INCLUDES_FILES ${test_stub_file})
endforeach ()

# Add the main.cpp and the generated per-test translation units
add_executable(gumTest EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/testunits/gumtest/main.cpp
    ${TEST_INCLUDES_FILES}
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
