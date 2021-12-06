###
# the infrastructure for parallelizing algorithms

# checks for the existence of the different kinds of threads used by aGrUM,
# i.e., openMP and STL threads

### openOMP
find_package (OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${OpenMP_C_FLAGS} ${CMAKE_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
endif ()

### STL threads
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package (Threads)

if (UNIX)
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
   set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lpthread")
endif(UNIX)


# specifies which kind of thread should be used by default by aGrUM
if ("${CMAKE_GUM_THREADS}" STREQUAL "omp" AND OPENMP_FOUND)
   set (GUM_THREADS "threadsOMP")
else ()
   set (GUM_THREADS "threadsSTL")
endif ()
