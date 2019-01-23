/* INCLUDES */
%{
#include <iostream>
#include <iomanip>
#include <string>
#include <ostream>
#include <sstream>
#include <vector>
#include <limits>
#include <utility>
#include <new>
#include <exception>
#include <typeinfo>

#include <agrum/core/types.h>
#include <agrum/core/utils_misc.h>
#include <agrum/core/hashFunc.h>
#include <agrum/core/bijection.h>
#include <agrum/core/set.h>
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/refPtr.h>
#include <agrum/core/list.h>
#include <agrum/core/OMPThreads.h>

#include <agrum/graphs/parts/edgeGraphPart.h>
#include <agrum/graphs/parts/arcGraphPart.h>
#include <agrum/graphs/parts/nodeGraphPart.h>

#include <agrum/graphs/diGraph.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/graphs/cliqueGraph.h>
#include <agrum/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/multidim/implementations/multiDimInterface.h>
#include <agrum/multidim/implementations/multiDimAdressable.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/implementations/multiDimContainer.h>
#include <agrum/multidim/implementations/multiDimDecorator.h>
#include <agrum/multidim/implementations/multiDimImplementation.h>
#include <agrum/multidim/implementations/multiDimWithOffset.h>
#include <agrum/multidim/implementations/multiDimReadOnly.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/graphicalModels/variableNodeMap.h>

#include <agrum/core/sequence.h>
#include <agrum/variables/variable.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/rangeVariable.h>
#include <agrum/variables/IDiscretizedVariable.h>
#include <agrum/variables/discretizedVariable.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/implementations/multiDimArray.h>

#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/BayesNet.h>

#include <agrum/BN/algorithms/essentialGraph.h>
#include <agrum/BN/algorithms/MarkovBlanket.h>
#include <agrum/BN/algorithms/structuralComparator.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/BN/io/DSL/DSLReader.h>
#include <agrum/BN/io/DSL/DSLWriter.h>
#include <agrum/BN/io/net/netReader.h>
#include <agrum/BN/io/net/netWriter.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNWriter.h>
#include <agrum/PRM/o3prm/O3prmBNReader.h>
#include <agrum/PRM/o3prm/O3prmBNWriter.h>
#include <agrum/BN/io/UAI/UAIReader.h>
#include <agrum/BN/io/UAI/UAIWriter.h>

#include <agrum/core/approximations/IApproximationSchemeConfiguration.h>
#include <agrum/core/approximations/approximationScheme.h>

#include <agrum/BN/inference/tools/BayesNetInference.h>

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/variableElimination.h>

#include <agrum/BN/inference/GibbsSampling.h>
#include <agrum/BN/inference/importanceSampling.h>
#include <agrum/BN/inference/weightedSampling.h>
#include <agrum/BN/inference/MonteCarloSampling.h>
#include <agrum/BN/inference/loopySamplingInference.h>

#include <agrum/BN/inference/loopyBeliefPropagation.h>

#include <agrum/BN/algorithms/divergence/KL.h>
#include <agrum/BN/algorithms/divergence/bruteForceKL.h>
#include <agrum/BN/algorithms/divergence/GibbsKL.h>

#include <agrum/core/signal/listener.h>
#include <agrum/graphs/parts/listeners/diGraphListener.h>

#include <agrum/CN/credalNet.h>
#include <agrum/CN/varMod2BNsMap.h>
#include <agrum/CN/inferenceEngine.h>
#include <agrum/CN/multipleInferenceEngine.h>
#include <agrum/CN/CNMonteCarloSampling.h>
#include <agrum/CN/CNLoopyPropagation.h>

#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/BNLearner.h>
#include <agrum/learning/database/BNDatabaseGenerator.h>

#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/inference/IInfluenceDiagramInference.h>
#include <agrum/ID/inference/influenceDiagramInference.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDReader.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDWriter.h>
#include <agrum/ID/generator/influenceDiagramGenerator.h>
%}


%include "typemaps.i"
%include "std_vector.i"
%include "std_string.i"

namespace std {
    %template ( Vector) vector<double>;
    %template ( Vector_int) vector<unsigned int>;
    %template ( Vector_size) vector<gum::Size>;
    %template ( Vector_string) vector<string>;
}

/* CLASS EXTENSIONS */
%extend gum::DiscreteVariable {
  gum::LabelizedVariable toLabelizedVar() {
    return gum::LabelizedVariable(* dynamic_cast<gum::LabelizedVariable*>(self ));
  }

  gum::RangeVariable toRangeVar() {
    return gum::RangeVariable(* dynamic_cast<gum::RangeVariable*>(self ));
  }

  gum::DiscretizedVariable<double> toDiscretizedVar() {
    return gum::DiscretizedVariable<double>(* dynamic_cast<gum::DiscretizedVariable<double> *> (self ));
  }
}

%include "forUsing.i"

/* EXCEPTION HANDLING */
%exceptionclass std::bad_cast;

%exceptionclass gum::Exception;

%exceptionclass gum::DefaultInLabel;
%exceptionclass gum::DuplicateElement;
%exceptionclass gum::DuplicateLabel;
%exceptionclass gum::EmptyBSTree;
%exceptionclass gum::EmptySet;
%exceptionclass gum::FatalError;
%exceptionclass gum::FormatNotFound;
%exceptionclass gum::GraphError;
%exceptionclass gum::IOError;
%exceptionclass gum::IdError;
%exceptionclass gum::InvalidArc;
%exceptionclass gum::InvalidArgument;
%exceptionclass gum::InvalidArgumentsNumber;
%exceptionclass gum::InvalidDirectedCycle;
%exceptionclass gum::InvalidEdge;
%exceptionclass gum::InvalidNode;
%exceptionclass gum::MissingValueInDatabase;
%exceptionclass gum::MissingVariableInDatabase;
%exceptionclass gum::NoChild;
%exceptionclass gum::NoNeighbour;
%exceptionclass gum::NoParent;
%exceptionclass gum::NotFound;
%exceptionclass gum::NullElement;
%exceptionclass gum::OperationNotAllowed;
%exceptionclass gum::OutOfBounds;
%exceptionclass gum::OutOfLowerBound;
%exceptionclass gum::OutOfUpperBound;
%exceptionclass gum::ReferenceError;
%exceptionclass gum::SizeError;
%exceptionclass gum::SyntaxError;
%exceptionclass gum::UndefinedElement;
%exceptionclass gum::UndefinedIteratorKey;
%exceptionclass gum::UndefinedIteratorValue;
%exceptionclass gum::UnknownLabelInDatabase;

%rename gum::Exception GumException;

/* WRAPPED HEADERS (pyAgrum API) */
%import <agrum/config.h>
%import <agrum/core/inline.h>

%import <agrum/core/types.h>
%include <agrum/core/set.h>
%include <agrum/core/exceptions.h>
%include <agrum/core/sequence.h>
%include <agrum/core/utils_random.h>

%include <agrum/core/OMPThreads.h>

%include <agrum/variables/variable.h>
%include <agrum/variables/discreteVariable.h>
%include <agrum/variables/labelizedVariable.h>
%include <agrum/variables/rangeVariable.h>
%include <agrum/variables/IDiscretizedVariable.h>
%include <agrum/variables/discretizedVariable.h>

%include <agrum/graphs/graphElements.h>

%import <agrum/graphs/parts/edgeGraphPart.h>
%import <agrum/graphs/parts/arcGraphPart.h>
%import <agrum/graphs/parts/nodeGraphPart.h>

%include <agrum/graphs/diGraph.h>
%include <agrum/graphs/DAG.h>
%include <agrum/graphs/undiGraph.h>
%include <agrum/graphs/mixedGraph.h>
%include <agrum/graphs/cliqueGraph.h>
%import <agrum/graphs/algorithms/triangulations/defaultTriangulation.h>
%import <agrum/graphs/algorithms/triangulations/triangulation.h>

%import <agrum/multidim/implementations/multiDimInterface.h>
%import <agrum/multidim/implementations/multiDimAdressable.h>
%import <agrum/multidim/implementations/multiDimContainer.h>
%import <agrum/multidim/implementations/multiDimDecorator.h>
%import <agrum/multidim/implementations/multiDimImplementation.h>
%import <agrum/multidim/imple                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 gum::credal::CredalNet<double>;
%template ( CNMonteCarloSampling ) gum::credal::CNMonteCarloSampling<double>;
%template ( CNLoopyPropagation ) gum::credal::CNLoopyPropagation<double>;

%template ( InfluenceDiagram) gum::InfluenceDiagram<double>;
%template ( InfluenceDiagramInference) gum::InfluenceDiagramInference<double>;

%template ( BNLearner) gum::learning::BNLearner<double>;
%template ( BNDatabaseGenerator) gum::learning::BNDatabaseGenerator<double>;

/* for debug */
namespace gum {
  void statsObj(void);
}
%{
namespace gum {
  void statsObj(void) {
#ifdef GUM_DEBUG_MODE
    gum::__debug__::__atexit();
#else
    //std::cout<<"Stats on aGrUM objects only available in debug mode"<<std::endl;
#endif // GUM_DEBUG_MODE
  }
}
%}
