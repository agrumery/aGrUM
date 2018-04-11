set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package (Threads)

if (UNIX)
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
   set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lpthread")
endif(UNIX)

