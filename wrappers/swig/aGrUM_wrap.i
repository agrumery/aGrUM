/*%include "includes.i"*/
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

#include <agrum/graphs/diGraph.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/cliqueGraph.h>
#include <agrum/graphs/defaultTriangulation.h>
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

#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/GibbsInference.h>

#include <agrum/BN/algorithms/divergence/KL.h>
#include <agrum/BN/algorithms/divergence/bruteForceKL.h>
#include <agrum/BN/algorithms/approximationScheme.h>
#include <agrum/BN/algorithms/divergence/GibbsKL.h>

#include <agrum/core/signal/listener.h>
#include <agrum/graphs/diGraphListener.h>

#include <agrum/CN/credalNet.h>
#include <agrum/CN/varMod2BNsMap.h>
#include <agrum/CN/inferenceEngine.h>
#include <agrum/CN/multipleInferenceEngine.h>
#include <agrum/CN/CNMonteCarloSampling.h>
#include <agrum/CN/CNLoopyPropagation.h>
%}

%typemap ( out ) std::vector<double> {
  std::vector<double> vOut = $1;
  int iLen = vOut.size();
  $result = PyList_New ( iLen );

  for ( unsigned int i = 0; i < iLen; i++ ) {
    double fVal = vOut.at ( i );
    PyObject* o = PyFloat_FromDouble ( ( double ) fVal );
    PyList_SetItem ( $result, i, o );
  }
}

%include "std_vector.i"

namespace std {
  %template ( Vector_double ) vector<double>;
}


%include "std_string.i"

/* DIRECTORS (for cross language polymorphism) */
%feature ( "director" ) gum::Potential; //add the __disown__() method to Potential
%feature ( "nodirector" ) gum::MultiDimContainer::copyFrom;
%feature ( "director" ) gum::LabelizedVariable;
%feature ( "director" ) gum::DiscretizedVariable;
%feature ( "director" ) gum::LazyPropagation;
%feature ( "director" ) gum::GibbsInference;

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
%exceptionclass std::exception;



/* WRAPPED HEADERS (pyAgrum API) */
%include <agrum/config.h>
%import <agrum/core/inline.h>

%import <agrum/core/types.h>
%include <agrum/core/exceptions.h>
%include <agrum/core/sequence.h>
//%include <agrum/core/set.h>
%include <agrum/core/utils_random.h>

%include <agrum/core/OMPThreads.h>

%include <agrum/variables/variable.h>
%include <agrum/variables/discreteVariable.h>
%include <agrum/variables/labelizedVariable.h>
%include <agrum/variables/rangeVariable.h>
%include <agrum/variables/discretizedVariable.h>

%include <agrum/graphs/graphElements.h>
%import <agrum/graphs/nodeGraphPart.h>
%import <agrum/graphs/arcGraphPart.h>
%include <agrum/graphs/diGraph.h>
%import <agrum/graphs/DAG.h>
%import <agrum/graphs/undiGraph.h>
%import <agrum/graphs/cliqueGraph.h>
%import <agrum/graphs/defaultTriangulation.h>

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
%include <agrum/multidim/multiDimArray.h>

%import <agrum/core/refPtr.h>
%include <agrum/core/list.h>

%import <agrum/graphicalModels/variableNodeMap.h>
%include <agrum/graphicalModels/DAGmodel.h>
%include <agrum/BN/IBayesNet.h>
%include <agrum/BN/BayesNet.h>

%include <agrum/BN/inference/BayesNetInference.h>
%include <agrum/BN/inference/lazyPropagation.h>
%import <agrum/BN/algorithms/approximationScheme.h>
%include <agrum/BN/inference/GibbsInference.h>

%import <agrum/BN/algorithms/divergence/KL.h>
%include <agrum/BN/algorithms/divergence/bruteForceKL.h>
%include <agrum/BN/algorithms/divergence/GibbsKL.h>

%import <agrum/core/signal/listener.h>
%import <agrum/graphs/diGraphListener.h>
%import <agrum/BN/io/BIF/BIFReader.h>

%include <agrum/CN/credalNet.h>
%include <agrum/CN/varMod2BNsMap.h>
%include <agrum/CN/inferenceEngine.h>
%include <agrum/CN/multipleInferenceEngine.h>
%include <agrum/CN/CNMonteCarloSampling.h>
%include <agrum/CN/CNLoopyPropagation.h>

/* CLASS EXTENSIONS */
%extend gum::DiscreteVariable {
  gum::LabelizedVariable & toLabelizedVar() {
    return dynamic_cast<gum::LabelizedVariable&> ( * ( self ) );
  }

  gum::RangeVariable & toRangeVar() {
    return dynamic_cast<gum::RangeVariable&> ( * ( self ) );
  }

  gum::DiscretizedVariable<float>& toDiscretizedVar() {
    return dynamic_cast<gum::DiscretizedVariable<float> &> ( * ( self ) );
  }
}

%extend gum::List {
  void append ( Val val ) {
    self->insert ( val );
  }

  void push_front ( Val val ) {
    self->push_front ( val );
  }

  void push_back ( Val val ) {
    self->push_back ( val );
  }

  void remove ( Val val ) {
    self->eraseByVal ( val );
  }

  void eradicate ( Val val ) {
    self->eraseAllVal ( val );
  }

  const Val __getitem__ ( unsigned int i ) {
    return ( * ( self ) ) [i];
  }

  bool exists ( Val val ) const
  {
    return self->exists ( val );
  }
}
%extend gum::LazyPropagation {
  using gum::BayesNetInference::addHardEvidence;
}

%extend gum::GibbsInference {
  using gum::BayesNetInference::addHardEvidence;

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
%extend gum::GibbsKL {
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

%extend gum::credal::InferenceEngine {
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

%extend gum::credal::MultipleInferenceEngine {
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setVerbosity;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setEpsilon;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setMinEpsilonRate;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setMaxIter;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setMaxTime;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setPeriodSize;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setBurnIn;

  using gum::credal::InferenceEngine<PYGUM_SCALAR>::verbosity;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::epsilon;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::minEpsilonRate;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::maxIter;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::maxTime;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::periodSize;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::burnIn;

  using gum::credal::InferenceEngine<PYGUM_SCALAR>::nbrIterations;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::currentTime;

  using gum::credal::InferenceEngine<PYGUM_SCALAR>::messageApproximationScheme;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::history;
}

%extend gum::credal::CNMonteCarloSampling {
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::setVerbosity;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::setEpsilon;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::setMinEpsilonRate;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::setMaxIter;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::setMaxTime;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::setPeriodSize;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::setBurnIn;

  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::verbosity;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::epsilon;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::minEpsilonRate;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::maxIter;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::maxTime;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::periodSize;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::InferenceEngine::burnIn;

  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::nbrIterations;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::currentTime;

  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::messageApproximationScheme;
  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::history;

  using gum::credal::MultipleInferenceEngine<PYGUM_SCALAR, BNInferenceEngine>::insertEvidenceFile;
}

%extend gum::credal::CNLoopyPropagation {
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setVerbosity;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setEpsilon;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setMinEpsilonRate;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setMaxIter;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setMaxTime;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setPeriodSize;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::setBurnIn;

  using gum::credal::InferenceEngine<PYGUM_SCALAR>::verbosity;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::epsilon;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::minEpsilonRate;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::maxIter;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::maxTime;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::periodSize;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::burnIn;

  using gum::credal::InferenceEngine<PYGUM_SCALAR>::nbrIterations;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::currentTime;

  using gum::credal::InferenceEngine<PYGUM_SCALAR>::messageApproximationScheme;
  using gum::credal::InferenceEngine<PYGUM_SCALAR>::history;
}

/* TEMPLATES INSTANTIATIONS */

%template ( randomDistribution_double ) gum::randomDistribution<double>;

%template ( DiscretizedVar ) gum::DiscretizedVariable<float>;

%template ( MultiDimContainer_double ) gum::MultiDimContainer<double>;
%template ( MultiDimImplementation_double ) gum::MultiDimImplementation<double>;
%template ( MultiDimDecorator_double ) gum::MultiDimDecorator<double>;
%template ( MultiDimWithOffset_double ) gum::MultiDimWithOffset<double>;
%template ( MultiDimArray_double ) gum::MultiDimArray<double>;

%template ( Potential_double ) gum::Potential<double>;

%template (IBayesNet_double ) gum::IBayesNet<double>;
%template ( BayesNet_double ) gum::BayesNet<double>;
%template ( BayesNetInference_double ) gum::BayesNetInference<double>;
%template ( LazyPropagation_double ) gum::LazyPropagation<double>;
%template ( GibbsInference_double ) gum::GibbsInference<double>;

%template ( BruteForceKL_double ) gum::BruteForceKL<double>;
%template ( GibbsKL_double ) gum::GibbsKL<double>;

%template ( CredalNet_double ) gum::credal::CredalNet<double>;
%template ( CNInferenceEngine_double ) gum::credal::InferenceEngine<double>;
%template ( CNMultipleInferenceEngine_double ) gum::credal::MultipleInferenceEngine<double, gum::LazyPropagation<double> >;
%template ( CNMonteCarloSampling_double ) gum::credal::CNMonteCarloSampling<double, gum::LazyPropagation<double> >;
%template ( CNLoopyPropagation_double ) gum::credal::CNLoopyPropagation<double>;
