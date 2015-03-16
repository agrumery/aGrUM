
include_directories(${AGRUM_INCLUDE_DIRS})
add_definitions(${AGRUM_DEFINITIONS})
set (CMAKE_CXX_FLAGS "${AGRUM_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
link_directories(${AGRUM_LIBRARY_DIRS})

