###
# the infrastructure for parallelizing algorithms
set (GUM_THREADS "stl" CACHE STRING "threads flavor std/omp")
if (GUM_THREADS MATCHES "stl")
  find_package(Threads)
elseif (GUM_THREADS MATCHES "omp")
  find_package(OpenMP)
else ()
  message(SEND_ERROR "Unknown thread flavor: ${GUM_THREADS}")
endif ()
