###
# the infrastructure for parallelizing algorithms

# checks for the existence of the different kinds of threads used by aGrUM,
# i.e., openMP and STL threads

# all the flags that will be used for multithreading
set(GUM_THREADS_C_FLAGS "")
set(GUM_THREADS_CXX_FLAGS "")
set(GUM_THREADS_SHARED_LINKER_FLAGS "")

### openOMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set(GUM_THREADS_C_FLAGS "${OpenMP_C_FLAGS}")
    set(GUM_THREADS_CXX_FLAGS "${OpenMP_CXX_FLAGS}")
endif ()

### STL threads
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads)

if (UNIX)
    set(GUM_THREADS_CXX_FLAGS "${GUM_THREADS_CXX_FLAGS} -pthread")
    set(GUM_THREADS_SHARED_LINKER_FLAGS "${GUM_THREADS_SHARED_LINKER_FLAGS} -lpthread")
endif (UNIX)

# specifies which kind of thread should be used by default by aGrUM
# the _USE_ are required for swig
if ("${CMAKE_GUM_THREADS}" STREQUAL "omp" AND OPENMP_FOUND)
    set(GUM_THREADS_C_FLAGS "${GUM_THREADS_C_FLAGS} -DGUM_THREADS_USE_OMP=1 -DGUM_THREADS=threadsOMP")
    set(GUM_THREADS_CXX_FLAGS "${GUM_THREADS_CXX_FLAGS} -DGUM_THREADS_USE_OMP=1 -DGUM_THREADS=threadsOMP")
else ()
    set(GUM_THREADS_C_FLAGS "${GUM_THREADS_C_FLAGS} -DGUM_THREADS_USE_STL=1 -DGUM_THREADS=threadsSTL")
    set(GUM_THREADS_CXX_FLAGS "${GUM_THREADS_CXX_FLAGS} -DGUM_THREADS_USE_STL=1 -DGUM_THREADS=threadsSTL")
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GUM_THREADS_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GUM_THREADS_CXX_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${GUM_THREADS_SHARED_LINKER_FLAGS}")
