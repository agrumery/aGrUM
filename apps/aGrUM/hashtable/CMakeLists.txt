cmake_minimum_required(VERSION 3.13)
project(EXAMPLE)

find_package(aGrUM CONFIG REQUIRED)

file(GLOB EXAMPLE_SOURCE ${EXAMPLE_SOURCE_DIR}/*.cpp)
file(GLOB EXAMPLE_INCLUDE ${EXAMPLE_SOURCE_DIR}/*.h)

add_executable (example ${EXAMPLE_SOURCE})
target_link_libraries(example agrumBASE)

install (TARGETS example DESTINATION bin)

