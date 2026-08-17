###
# the infrastructure for parallelizing algorithms
set (GUM_THREADS "stl" CACHE STRING "threads flavor std/omp")
find_package(Threads REQUIRED)
if (GUM_THREADS MATCHES "stl")
elseif (GUM_THREADS MATCHES "omp")
  find_package(OpenMP)
else ()
  message(SEND_ERROR "Unknown thread flavor: ${GUM_THREADS}")
endif ()
