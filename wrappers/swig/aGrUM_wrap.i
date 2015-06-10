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
#include <agrum/core/utils.h>
#include <agrum/core/hashFunc.h>
#include <agrum/core/bijection.h>
#include <agrum/core/set.h>
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/refPtr.h>
#include <agrum/core/list.h>
#include <agrum/core/OMPThreads.h>

#include <agrum/graphs/edgeGraphPart.h>
#include <agrum/graphs/arcGraphPart.h>
#include <agrum/graphs/nodeGraphPart.h>

#include <agrum/graphs/diGraph.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/graphs/cliqueGraph.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/multidim/multiDimInterface.h>
#include <agrum/multidim/multiDimAdressable.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimContainer.h>
#include <agrum/multidim/multiDimDecorator.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimWithOffset.h>
#include <agrum/multidim/multiDimReadOnly.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/graphicalModels/variableNodeMap.h>

#include <agrum/core/sequence.h>
#include <agrum/variables/variable.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/rangeVariable.h>
#include <agrum/variables/discretizedVariable.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/utilityTable.h>
#include <agrum/multidim/multiDimArray.h>

#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/BayesNet.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/BN/io/DSL/DSLReader.h>
#include <agrum/BN/io/DSL/DSLWriter.h>
#include <agrum/BN/io/net/netReader.h>
#include <agrum/BN/io/net/netWriter.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNWriter.h>

#include <agrum/core/algorithms/approximationScheme/IApproximationSchemeConfiguration.h>
#include <agrum/core/algorithms/approximationScheme/approximationScheme.h>

#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/GibbsInference.h>

#include <agrum/BN/algorithms/divergence/KL.h>
#include <agrum/BN/algorithms/divergence/bruteForceKL.h>
#include <agrum/BN/algorithms/divergence/GibbsKL.h>

#include <agrum/core/signal/listener.h>
#include <agrum/graphs/listeners/diGraphListener.h>

#include <agrum/CN/credalNet.h>
#include <agrum/CN/varMod2BNsMap.h>
#include <agrum/CN/inferenceEngine.h>
#include <agrum/CN/multipleInferenceEngine.h>
#include <agrum/CN/CNMonteCarloSampling.h>
#include <agrum/CN/CNLoopyPropagation.h>

#include <agrum/learning/BNLearner.h>

#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/inference/IInfluenceDiagramInference.h>
#include <agrum/ID/inference/influenceDiagramInference.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDReader.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDWriter.h>
#include <agrum/ID/generator/influenceDiagramGenerator.h>
%}

/* CLASS EXTENSIONS */
%extend gum::DiscreteVariable {
  gum::LabelizedVariable & toLabelizedVar() {
    return dynamic_cast<gum::LabelizedVariable&> ( * ( self ) );
  }

  gum::RangeVariable & toRangeVar() {
    return dynamic_cast<gum::RangeVariable&> ( * ( self ) );
  }

  gum::DiscretizedVariable<double>& toDiscretizedVar() {
    return dynamic_cast<gum::DiscretizedVariable<double> &> ( * ( self ) );
  }
}

%extend gum::LazyPropagation<double> {
  void addHardEvidence(const gum::NodeId id,gum::Idx val) {
    return self->gum::BayesNetInference<double>::addHardEvidence(id,val);
  }
}

%extend gum::GibbsInference<double> {
  void addHardEvidence(const gum::NodeId id,gum::Idx val) {
    return self->gum::BayesNetInference<double>::addHardEvidence(id,val);
  }
}


%define ADD_NODEGRAPHPART_API(classname)
%extend classname {
  using gum::NodeGraphPart::addNode;
  using gum::NodeGraphPart::existsNode;
  using gum::NodeGraphPart::size;
  using gum::NodeGraphPart::empty;
}
%enddef
ADD_NODEGRAPHPART_API(gum::DiGraph)
ADD_NODEGRAPHPART_API(gum::UndiGraph)
ADD_NODEGRAPHPART_API(gum::MixedGraph)

%define ADD_EDGEGRAPHPART_API(classname)
%extend classname {
  using gum::EdgeGraphPart::eraseEdge;
  using gum::EdgeGraphPart::existsEdge;
  using gum::EdgeGraphPart::eraseParents;
  using gum::EdgeGraphPart::eraseChildren;
  using gum::EdgeGraphPart::sizeEdges;
  using gum::EdgeGraphPart::emptyEdges;
  using gum::EdgeGraphPart::eraseNeighbours;
}
%enddef
ADD_EDGEGRAPHPART_API(gum::UndiGraph)
ADD_EDGEGRAPHPART_API(gum::MixedGraph)

%define ADD_ARCGRAPHPART_API(classname)
%extend classname {
  using gum::ArcGraphPart::eraseArc;
  using gum::ArcGraphPart::existsArc;
  using gum::ArcGraphPart::eraseParents;
  using gum::ArcGraphPart::eraseChildren;
  using gum::ArcGraphPart::sizeArcs;
  using gum::ArcGraphPart::emptyArcs;
}
%enddef
ADD_ARCGRAPHPART_API(gum::DiGraph);
ADD_ARCGRAPHPART_API(gum::MixedGraph);



%define ADD_APPROXIMATIONSCHEME_API(classname)
%extend classname {
  using gum::ApproximationScheme::setVerbosity;
  using gum::ApproximationScheme::setEpsilon;
  using gum::ApproximationScheme::setMinEpsilonRate;
  using gum::ApproximationScheme::setMaxIter;
  using gum::ApproximationScheme::setMaxTime;
  using gum::ApproximationScheme::setPeriodSize;
  using gum::ApproximationScheme::setBurnIn;

  using gum::ApproximationScheme::verbosity;
  using gum::ApproximationScheme::epsilon;
  using gum::ApproximationScheme::minEpsilonRate;
  using gum::ApproximationScheme::maxIter;
  using gum::ApproximationScheme::maxTime;
  using gum::ApproximationScheme::periodSize;
  using gum::ApproximationScheme::burnIn;

  using gum::ApproximationScheme::nbrIterations;
  using gum::ApproximationScheme::currentTime;

  using gum::ApproximationScheme::messageApproximationScheme;
  using gum::ApproximationScheme::history;
}
%enddef
ADD_APPROXIMATIONSCHEME_API(gum::GibbsInference<double>)
ADD_APPROXIMATIONSCHEME_API(gum::GibbsKL<double>)
ADD_APPROXIMATIONSCHEME_API(%arg(gum::credal::CNMonteCarloSampling<double,gum::LazyPropagation<double> >))
ADD_APPROXIMATIONSCHEME_API(gum::credal::CNLoopyPropagation<double>)

%extend gum::learning::BNLearner {
  using gum::IApproximationSchemeConfiguration::messageApproximationScheme;

  using gum::learning::genericBNLearner::setMaxTime;
  using gum::learning::genericBNLearner::maxTime;
  using gum::learning::genericBNLearner::currentTime;

  using gum::learning::genericBNLearner::learnDAG;
  using gum::learning::genericBNLearner::names;
  using gum::learning::genericBNLearner::modalities;
  using gum::learning::genericBNLearner::idFromName;
  using gum::learning::genericBNLearner::nameFromId;
  using gum::learning::genericBNLearner::useScoreAIC;
  using gum::learning::genericBNLearner::useScoreBD;
  using gum::learning::genericBNLearner::useScoreBDeu;
  using gum::learning::genericBNLearner::useScoreBIC;
  using gum::learning::genericBNLearner::useScoreK2;
  using gum::learning::genericBNLearner::useScoreLog2Likelihood;
  using gum::learning::genericBNLearner::setAprioriWeight;
  using gum::learning::genericBNLearner::useNoApriori;
  using gum::learning::genericBNLearner::useAprioriSmoothing;
  using gum::learning::genericBNLearner::useAprioriDirichlet;
  using gum::learning::genericBNLearner::useGreedyHillClimbing;
  using gum::learning::genericBNLearner::useLocalSearchWithTabuList;
  using gum::learning::genericBNLearner::useK2;
  using gum::learning::genericBNLearner::setMaxIndegree;
  using gum::learning::genericBNLearner::setSliceOrder;
  using gum::learning::genericBNLearner::addForbiddenAr;
  using gum::learning::genericBNLearner::eraseForbiddenArc;
  using gum::learning::genericBNLearner::addMandatoryArc;
  using gum::learning::genericBNLearner::addMandatoryArc;
  using gum::learning::genericBNLearner::eraseMandatoryArc;

  /*using gum::learning::genericBNLearner::setInitialDAG;*/
}

%include "forUsing.i"

%include "std_vector.i"
%include "std_string.i"

namespace std {
  %template ( Vector_double) vector<double>;
  %template ( Vector_string) vector<string>;
}


/* EXCEPTION HANDLING */
%exceptionclass std::bad_cast;
%exceptionclass gum::Exception;
%exceptionclass gum::GraphError;
%exceptionclass gum::IdError;
%exceptionclass gum::OperationNotAllowed;
%exceptionclass gum::ReferenceError;
%exceptionclass gum::DefaultInLabel;
%exceptionclass gum::Circuit;
%exceptionclass gum::InvalidNode;
%exceptionclass gum::InvalidEdge;
%exceptionclass gum::DuplicateElement;
%exceptionclass gum::OutOfBounds;
%exceptionclass gum::OutOfLowerBound;
%exceptionclass gum::OutOfUpperBound;
%exceptionclass gum::IOError;
%exceptionclass std::exception;



/* WRAPPED HEADERS (pyAgrum API) */
%import <agrum/config.h>
%import <agrum/core/inline.h>

%import <agrum/core/types.h>
%include <agrum/core/exceptions.h>
%include <agrum/core/sequence.h>
%include <agrum/core/utils_random.h>

%include <agrum/core/OMPThreads.h>

%include <agrum/variables/variable.h>
%include <agrum/variables/discreteVariable.h>
%include <agrum/variables/labelizedVariable.h>
%include <agrum/variables/rangeVariable.h>
%include <agrum/variables/discretizedVariable.h>

%include <agrum/graphs/graphElements.h>

%import <agrum/graphs/edgeGraphPart.h>
%import <agrum/graphs/arcGraphPart.h>
%import <agrum/graphs/nodeGraphPart.h>

%include <agrum/graphs/diGraph.h>
%include <agrum/graphs/DAG.h>
%include <agrum/graphs/undiGraph.h>
%include <agrum/graphs/mixedGraph.h>
%include <agrum/graphs/cliqueGraph.h>
%import <agrum/graphs/triangulations/defaultTriangulation.h>

%include <agrum/multidim/multiDimInterface.h>
%import <agrum/multidim/multiDimAdressable.h>
%include <agrum/multidim/instantiation.h>
%include <agrum/multidim/multiDimContainer.h>
%include <agrum/multidim/multiDimDecorator.h>
%include <agrum/multidim/multiDimImplementation.h>
%include <agrum/multidim/multiDimWithOffset.h>
%import <agrum/multidim/multiDimReadOnly.h>
%include <agrum/multidim/ICIModels/multiDimNoisyORCompound.h>
%include <agrum/multidim/ICIModels/multiDimNoisyAND.h>
%include <agrum/multidim/potential.h>
%include <agrum/multidim/utilityTable.h>
%include <agrum/multidim/multiDimArray.h>

%import <agrum/core/refPtr.h>
%include <agrum/core/list.h>

%import <agrum/graphicalModels/variableNodeMap.h>
%include <agrum/graphicalModels/DAGmodel.h>
%include <agrum/BN/IBayesNet.h>
%include <agrum/BN/BayesNet.h>

%import <agrum/core/algorithms/approximationScheme/IApproximationSchemeConfiguration.h>
%import <agrum/core/algorithms/approximationScheme/approximationScheme.h>

%include <agrum/BN/inference/BayesNetInference.h>
%include <agrum/BN/inference/lazyPropagation.h>
%include <agrum/BN/inference/GibbsInference.h>

%import <agrum/BN/algorithms/divergence/KL.h>
%include <agrum/BN/algorithms/divergence/bruteForceKL.h>
%include <agrum/BN/algorithms/divergence/GibbsKL.h>

%import <agrum/core/signal/listener.h>
%import <agrum/graphs/listeners/diGraphListener.h>
%import <agrum/BN/io/BIF/BIFReader.h>

%import <agrum/learning/BNLearnUtils/genericBNLearner.h>
%include <agrum/learning/BNLearner.h>

%include <agrum/CN/credalNet.h>
%include <agrum/CN/varMod2BNsMap.h>
%import <agrum/CN/inferenceEngine.h>
%import <agrum/CN/multipleInferenceEngine.h>
%include <agrum/CN/CNMonteCarloSampling.h>
%include <agrum/CN/CNLoopyPropagation.h>

%include <agrum/ID/influenceDiagram.h>
%include <agrum/ID/inference/influenceDiagramInference.h>


/* TEMPLATES INSTANTIATIONS */

%template ( randomDistribution_double ) gum::randomDistribution<double>;

%template ( DiscretizedVariable_double ) gum::DiscretizedVariable<double>;

%template ( MultiDimContainer_double ) gum::MultiDimContainer<double>;
%template ( MultiDimImplementation_double ) gum::MultiDimImplementation<double>;
%template ( MultiDimDecorator_double ) gum::MultiDimDecorator<double>;
%template ( MultiDimWithOffset_double ) gum::MultiDimWithOffset<double>;
%template ( MultiDimArray_double ) gum::MultiDimArray<double>;

%template ( Potential_double ) gum::Potential<double>;
%template ( UtilityTable_double ) gum::UtilityTable<double>;

%template (IBayesNet_double ) gum::IBayesNet<double>;
%template ( BayesNet_double ) gum::BayesNet<double>;
%template ( BayesNetInference_double ) gum::BayesNetInference<double>;
%template ( LazyPropagation_double ) gum::LazyPropagation<double>;
%template ( GibbsInference_double ) gum::GibbsInference<double>;

%template ( BruteForceKL_double ) gum::BruteForceKL<double>;
%template ( GibbsKL_double ) gum::GibbsKL<double>;

%template ( CredalNet_double ) gum::credal::CredalNet<double>;
%template ( CNMonteCarloSampling_double ) gum::credal::CNMonteCarloSampling<double, gum::LazyPropagation<double> >;
%template ( CNLoopyPropagation_double ) gum::credal::CNLoopyPropagation<double>;

%template ( InfluenceDiagram_double) gum::InfluenceDiagram<double>;
%template ( InfluenceDiagramInference_double) gum::InfluenceDiagramInference<double>;

%template ( BNLearner_double) gum::learning::BNLearner<double>;
