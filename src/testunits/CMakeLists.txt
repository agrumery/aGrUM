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

  ADD_EXECUTABLE(${NAME} ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp ${ARGN} classes/DummyDiscreteVar.cpp  classes/DV_DummyListener.cpp)

  ADD_TEST(${NAME} ${NAME})
ENDMACRO(ADD_CXXTEST)

#########################################################################################

include_directories(cxxtest)
include_directories(classes)
include_directories(${AGRUM_SRC_DIR})
include_directories(${AGRUM_BINARY_DIR}/agrum)

link_directories(${AGRUM_BINARY_DIR}/lib})


# file(GLOB AGRUM_TESTS *Suite.h)
file(GLOB AGRUM_TESTS
 ArcTestSuite.h
 AVLSearchTreeTestSuite.h
 BayesBallsTestSuite.h
 BayesNetGeneratorTestSuite.h
 BayesNetTestSuite.h
 BijectionTestSuite.h
# BIFReaderTestSuite.h # <---- ERROR
# BIFWriterTestSuite.h
 BinTreeNodeTestSuite.h
 BinTreeNode4AVLTestSuite.h
 BinSearchTreeTestSuite.h
 CliqueGraphTestSuite.h
 CompareInferencesTestSuite.h
 CrossMultiDimTestSuite.h
 DAGTestSuite.h
 DatabaseTestSuite.h  # <------------------------- Memory leak : faire qqchose pour XML with missing value
# DefaultTriangulationTestSuite.h # <---- ERROR
 DiGraphTestSuite.h
 DiscretizedVariableListenerTestSuite.h
 DiscretizedVariableTestSuite.h
 EdgeTestSuite.h
 EliminationSequenceTriangulationTestSuite.h
 GibbsInferenceBNTestSuite.h
 HashTableTestSuite.h
 InstantiationTestSuite.h
 LabelizedVariableListenerTestSuite.h
 LabelizedVariableTestSuite.h
 LazyInferenceBNTestSuite.h     #<-----------------------MEMORY LEAK
 ListBaseTestSuite.h
 ListTestSuite.h
 MixedGraphTestSuite.h
 MultiDimAggregatorTestSuite.h
 MultiDimBucketTestSuite.h
 MultiDimNoisyORTestSuite.h
 MultiDimTestSuite.h
 PotentialTestSuite.h
 RangeVariableTestSuite.h
 RefPtrTestSuite.h
 SequenceTestSuite.h
 SetTestSuite.h
 ShaferShenoyInferenceTestSuite.h  #<-----------------------MEMORY LEAK
 SimpleCPTGeneratorTestSuite.h
 TorcsTestSuite.h   #<-----------------------MEMORY LEAK
 UndiGraphTestSuite.h
 ValueEliminationTestSuite.h
 VariableNodeMapTestSuite.h
)

add_cxxtest(test ${AGRUM_TESTS})

target_link_libraries(test agrum)
