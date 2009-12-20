enable_testing()
find_package(PythonInterp)
set(CXXTESTGEN ${CMAKE_SOURCE_DIR}/testunits/cxxtest/cxxtestgen.py)

MACRO(ADD_CXXTEST NAME)
  IF(PYTHONINTERP_FOUND)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp
      COMMAND
        ${PYTHON_EXECUTABLE} ${CXXTESTGEN}
        --runner=AgrumErrorPrinter
        -o ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp ${ARGN}
      DEPENDS ${ARGN}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
  ENDIF(PYTHONINTERP_FOUND)

  ADD_EXECUTABLE(${NAME} ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp ${ARGN})

  ADD_TEST(${NAME} ${NAME})
ENDMACRO(ADD_CXXTEST)

#########################################################################################

include_directories(cxxtest)
include_directories(classes)

include_directories(${AGRUM_BINARY_DIR}/agrum)

link_directories(${AGRUM_BINARY_DIR}/lib})


#file(GLOB AGRUM_TESTS *Suite.h)
file(GLOB AGRUM_TESTS
       ArcTestSuite.h
       AVLSearchTreeTestSuite.h
       BayesBallsTestSuite.h
       BayesNetFactoryTestSuite.h
       BayesNetGeneratorTestSuite.h
       BayesNetTestSuite.h
       BijectionTestSuite.h
       BIFReaderTestSuite.h
       BIFWriterTestSuite.h
       BinSearchTreeTestSuite.h
       BinTreeNode4AVLTestSuite.h
       BinTreeNodeTestSuite.h
       CliqueGraphTestSuite.h
       CompareInferencesTestSuite.h
       CrossMultiDimTestSuite.h
       DAGTestSuite.h
       DatabaseTestSuite.h
       DefaultTriangulationTestSuite.h
       DiGraphTestSuite.h
       DiscretizedVariableTestSuite.h
       EdgeTestSuite.h
       GibbsInferenceBNTestSuite.h
       GraphListenerTestSuite.h
       HashTableTestSuite.h
       IncrementalTriangulationTestSuite.h
       InstantiationTestSuite.h
       LabelizedVariableTestSuite.h
       LazyInferenceBNTestSuite.h     #<-----------------------MEMORY LEAK
       ListBaseTestSuite.h
       ListTestSuite.h
       MixedGraphTestSuite.h
       MultiDimAggregatorTestSuite.h
       MultiDimBucketTestSuite.h
       MultiDimCombination.h
       MultiDimNoisyORTestSuite.h
       MultiDimOperatorsTestSuite.h
       MultiDimProjection.h
       MultiDimTestSuite.h
       NodeGraphPartTestSuite.h
       OrderedTriangulationTestSuite.h
       PartialOrderedTriangulationTestSuite.h
       PotentialTestSuite.h
       RangeVariableTestSuite.h
       RefPtrTestSuite.h
       SequenceTestSuite.h
       SetTestSuite.h
       ShaferShenoyInferenceTestSuite.h
       SimpleCPTGeneratorTestSuite.h
       SpanningForestPrimTestSuite.h
       TorcsTestSuite.h   #<-----------------------MEMORY LEAK
       UndiGraphTestSuite.h
       UtilityTestSuite.h
       ValueEliminationTestSuite.h
       VariableNodeMapTestSuite.h
)

add_cxxtest(test ${AGRUM_TESTS})

add_definitions("-DAGRUM_SRC_PATH=${AGRUM_SOURCE_DIR}")

target_link_libraries(test ${LIBAGRUM})
