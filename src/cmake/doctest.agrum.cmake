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
    # file(GENERATE), unlike file(WRITE), only touches the output when its
    # content actually changes -- file(WRITE) reruns unconditionally on every
    # configure (i.e. every `act test`) and bumps every stub's mtime, forcing
    # a full rebuild even when no test file changed.
    file(GENERATE OUTPUT ${test_stub_file} CONTENT "// Auto-generated file - do not edit manually\n#include <${test_file}>\n")
    list(APPEND TEST_INCLUDES_FILES ${test_stub_file})

    # Derive the doctest [Module][Suite] tags from the test file's own path
    # instead of requiring a #define GUM_CURRENT_SUITE/GUM_CURRENT_MODULE pair
    # in every test file: module = directory name, suite = filename stem.
    string(REGEX MATCH "^module_([A-Za-z0-9]+)/" _unused "${test_file}")
    set(module_tag "${CMAKE_MATCH_1}")
    if (module_tag STREQUAL "BASE")
        # GUMBASE: "BASE" is already #define'd by lrslib (src/agrum/base/external/lrslib/)
        set(module_tag "GUMBASE")
    endif ()

    get_filename_component(suite_name "${test_file}" NAME_WE)
    string(REGEX REPLACE "TestSuite$" "" suite_name "${suite_name}")

    set_source_files_properties(${test_stub_file} PROPERTIES
        COMPILE_DEFINITIONS "GUM_CURRENT_MODULE=${module_tag};GUM_CURRENT_SUITE=${suite_name}")
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
