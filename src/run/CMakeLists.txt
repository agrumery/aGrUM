include_directories(${AGRUM_SRC_DIR})
include_directories(${AGRUM_BINARY_DIR}/agrum)

link_directories(${AGRUM_BINARY_DIR}/lib})
add_executable(agrumDemo agrumDemo.cpp)
target_link_libraries(agrumDemo agrum)