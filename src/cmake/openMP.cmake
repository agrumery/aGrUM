###
#to use openMP (pragmas ignored otherwise)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fopenmp")
SET(CMAKE_CXX_FLAGS -fopenmp)
SET(CMAKE_EXE_LINKER_FLAGS -lgomp)
###

