/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
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

#include <agrum/tools/core/types.h>
#include <agrum/tools/core/utils_misc.h>
#include <agrum/tools/core/hashFunc.h>
#include <agrum/tools/core/bijection.h>
#include <agrum/tools/core/set.h>
#include <agrum/tools/core/debug.h>
#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/core/refPtr.h>
#include <agrum/tools/core/list.h>
#include <agrum/tools/core/OMPThreads.h>

#include <agrum/tools/graphs/parts/edgeGraphPart.h>
#include <agrum/tools/graphs/parts/arcGraphPart.h>
#include <agrum/tools/graphs/parts/nodeGraphPart.h>

#include <agrum/tools/graphs/diGraph.h>
#include <agrum/tools/graphs/DAG.h>
#include <agrum/tools/graphs/undiGraph.h>
#include <agrum/tools/graphs/mixedGraph.h>
#include <agrum/tools/graphs/cliqueGraph.h>
#include <agrum/tools/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/tools/variables/discreteVariable.h>
#include <agrum/tools/multidim/implementations/multiDimInterface.h>
#include <agrum/tools/multidim/implementations/multiDimAdressable.h>
#include <agrum/tools/multidim/instantiation.h>
#include <agrum/tools/multidim/implementations/multiDimContainer.h>
#include <agrum/tools/multidim/implementations/multiDimDecorator.h>
#include <agrum/tools/multidim/implementations/multiDimImplementation.h>
#include <agrum/tools/multidim/implementations/multiDimWithOffset.h>
#include <agrum/tools/multidim/implementations/multiDimReadOnly.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyAND.h>

#include <agrum/tools/graphicalModels/graphicalModel.h>
#include <agrum/tools/graphicalModels/DAGmodel.h>
#include <agrum/tools/graphicalModels/UGmodel.h>
#include <agrum/tools/graphicalModels/variableNodeMap.h>

#include <agrum/tools/core/sequence.h>
#include <agrum/tools/variables/variable.h>
#include <agrum/tools/variables/discreteVariable.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/variables/rangeVariable.h>
#include <agrum/tools/variables/IDiscretizedVariable.h>
#include <agrum/tools/variables/discretizedVariable.h>
#include <agrum/tools/graphs/graphElements.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/multidim/implementations/multiDimArray.h>

#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFragment.h>

#include <agrum/MN/IMarkovNet.h>
#include <agrum/MN/MarkovNet.h>
#include <agrum/MN/inference/ShaferShenoyMNInference.h>
#include <agrum/MN/io/UAI/UAIMNReader.h>
#include <agrum/MN/io/UAI/UAIMNWriter.h>

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
#include <agrum/BN/io/UAI/UAIBNReader.h>
#include <agrum/BN/io/UAI/UAIBNWriter.h>

#include <agrum/tools/core/approximations/IApproximationSchemeConfiguration.h>
#include <agrum/tools/core/approximations/approximationScheme.h>

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

#include <agrum/BN/algorithms/divergence/BNdistance.h>
#include <agrum/BN/algorithms/divergence/exactBNdistance.h>
#include <agrum/BN/algorithms/divergence/GibbsBNdistance.h>

#include <agrum/tools/core/signal/listener.h>
#include <agrum/tools/graphs/parts/listeners/diGraphListener.h>

#include <agrum/CN/credalNet.h>
#include <agrum/CN/tools/varMod2BNsMap.h>
#include <agrum/CN/inference/inferenceEngine.h>
#include <agrum/CN/inference/multipleInferenceEngine.h>
#include <agrum/CN/inference/CNMonteCarloSampling.h>
#include <agrum/CN/inference/CNLoopyPropagation.h>

#include <agrum/tools/database/databaseTable.h>
#include <agrum/BN/learning/BNLearner.h>
#include <agrum/BN/database/BNDatabaseGenerator.h>

#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/inference/tools/influenceDiagramInference.h>
#include <agrum/ID/inference/ShaferShenoyLIMIDInference.h>
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
%import <agrum/tools/core/inline.h>

%import <agrum/tools/core/types.h>
%include <agrum/tools/core/set.h>
%include <agrum/tools/core/exceptions.h>
%include <agrum/tools/core/sequence.h>
%include <agrum/tools/core/utils_random.h>

%include <agrum/tools/core/OMPThreads.h>

%include <agrum/tools/variables/variable.h>
%include <agrum/tools/variables/discreteVariable.h>
%include <agrum/tools/variables/labelizedVariable.h>
%include <agrum/tools/variables/rangeVariable.h>
%include <agrum/tools/variables/IDiscretizedVariable.h>
%include <agrum/tools/variables/discretizedVariable.h>

%include <agrum/tools/graphs/graphElements.h>

%import <agrum/tools/graphs/parts/edgeGraphPart.h>
%import <agrum/tools/graphs/parts/arcGraphPart.h>
%import <agrum/tools/graphs/parts/nodeGraphPart.h>

%include <agrum/tools/graphs/undiGraph.h>
%include <agrum/tools/graphs/diGraph.h>
%include <agrum/tools/graphs/DAG.h>
%include <agrum/tools/graphs/mixedGraph.h>
%include <agrum/tools/graphs/cliqueGraph.h>
%import <agrum/tools/graphs/algorithms/triangulations/defaultTriangulation.h>
%import <agrum/tools/graphs/algorithms/triangulations/triangulation.h>


%import <agrum/tools/multidim/implementations/multiDimInterface.h>
%import <agrum/tools/multidim/implementations/multiDimAdressable.h>
%import <agrum/tools/multidim/implementations/multiDimContainer.h>
%import <agrum/tools/multidim/implementations/multiDimDecorator.h>
%import <agrum/tools/multidim/implementations/multiDimImplementation.h>
%import <agrum/tools/multidim/implementations/multiDimWithOffset.h>
%import <agrum/tools/multidim/implementations/multiDimReadOnly.h>
%import <agrum/tools/multidim/implementations/multiDimArray.h>

%include <agrum/tools/multidim/ICIModels/multiDimNoisyORCompound.h>
%include <agrum/tools/multidim/ICIModels/multiDimNoisyAND.h>

%include <agrum/tools/multidim/instantiation.h>
%include <agrum/tools/multidim/potential.h>

%import <agrum/tools/core/refPtr.h>
%include <agrum/tools/core/list.h>

%import <agrum/tools/graphicalModels/variableNodeMap.h>
%include <agrum/tools/graphicalModels/graphicalModel.h>
%include <agrum/tools/graphicalModels/DAGmodel.h>
%include <agrum/tools/graphicalModels/UGmodel.h>

%include <agrum/BN/IBayesNet.h>
%include <agrum/BN/BayesNet.h>
%include <agrum/BN/BayesNetFragment.h>

%include <agrum/MN/IMarkovNet.h>
%include <agrum/MN/MarkovNet.h>
%include <agrum/MN/inference/ShaferShenoyMNInference.h>


%include <agrum/BN/algorithms/essentialGraph.h>
%include <agrum/BN/algorithms/MarkovBlanket.h>
%include <agrum/BN/algorithms/structuralComparator.h>

%import <agrum/tools/core/approximations/IApproximationSchemeConfiguration.h>
%include <agrum/tools/core/approximations/approximationScheme.h>

%import <agrum/BN/inference/tools/relevantPotentialsFinderType.h>

%ignore gum::BayesNetInference;
%include <agrum/BN/inference/tools/BayesNetInference.h>
%include <agrum/BN/inference/lazyPropagation.h>
%include <agrum/BN/inference/ShaferShenoyInference.h>
%include <agrum/BN/inference/variableElimination.h>

%include <agrum/BN/inference/GibbsSampling.h>
%include <agrum/BN/inference/importanceSampling.h>
%include <agrum/BN/inference/weightedSampling.h>
%include <agrum/BN/inference/MonteCarloSampling.h>
%include <agrum/BN/inference/loopySamplingInference.h>

%include <agrum/BN/inference/loopyBeliefPropagation.h>

%import <agrum/BN/algorithms/divergence/BNdistance.h>
%include <agrum/BN/algorithms/divergence/exactBNdistance.h>
%include <agrum/BN/algorithms/divergence/GibbsBNdistance.h>

%import <agrum/tools/core/signal/listener.h>
%import <agrum/tools/graphs/parts/listeners/diGraphListener.h>
%import <agrum/BN/io/BIF/BIFReader.h>

%import <agrum/tools/database/databaseTable.h>
%import <agrum/BN/learning/BNLearnUtils/genericBNLearner.h>
%include <agrum/BN/learning/BNLearner.h>
%include <agrum/BN/database/BNDatabaseGenerator.h>

%include <agrum/CN/credalNet.h>
%include <agrum/CN/varMod2BNsMap.h>
%include <agrum/CN/inferenceEngine.h>
%include <agrum/CN/multipleInferenceEngine.h>
%include <agrum/CN/CNMonteCarloSampling.h>
%include <agrum/CN/CNLoopyPropagation.h>

%include <agrum/ID/influenceDiagram.h>
%include <agrum/ID/inference/ShaferShenoyLIMIDInference.h>

/* TEMPLATES INSTANTIATIONS */

%template ( randomDistribution ) gum::randomDistribution<double>;

%template ( DiscretizedVariable ) gum::DiscretizedVariable<double>;

%template ( MultiDimContainer ) gum::MultiDimContainer<double>;
%template ( MultiDimImplementation ) gum::MultiDimImplementation<double>;
%template ( MultiDimDecorator ) gum::MultiDimDecorator<double>;
%template ( MultiDimWithOffset ) gum::MultiDimWithOffset<double>;
%template ( MultiDimArray ) gum::MultiDimArray<double>;

%template ( Potential ) gum::Potential<double>;

%template (IBayesNet ) gum::IBayesNet<double>;
%template ( BayesNet ) gum::BayesNet<double>;
%template ( BayesNetFragment ) gum::BayesNetFragment<double>;

%template (IMarkovNet ) gum::IMarkovNet<double>;
%template ( MarkovNet ) gum::MarkovNet<double>;
%template ( ShaferShenoyMNInference ) gum::ShaferShenoyMNInference<double>;

//%template ( BayesNetInference ) gum::BayesNetInference<double>;
%template ( LazyPropagation ) gum::LazyPropagation<double>;
%template ( ShaferShenoyInference ) gum::ShaferShenoyInference<double>;
%template ( VariableElimination ) gum::VariableElimination<double>;

%template ( GibbsSampling ) gum::GibbsSampling<double>;
%template ( ImportanceSampling ) gum::ImportanceSampling<double>;
%template ( WeightedSampling ) gum::WeightedSampling<double>;
%template ( MonteCarloSampling ) gum::MonteCarloSampling<double>;
%template ( LoopyImportanceSampling ) gum::LoopySamplingInference<double,gum::ImportanceSampling>;
%template ( LoopyWeightedSampling ) gum::LoopySamplingInference<double,gum::WeightedSampling>;
%template ( LoopyGibbsSampling ) gum::LoopySamplingInference<double,gum::GibbsSampling>;
%template ( LoopyMonteCarloSampling ) gum::LoopySamplingInference<double,gum::MonteCarloSampling>;

%template ( LoopyBeliefPropagation ) gum::LoopyBeliefPropagation<double>;

%template ( ExactBNdistance ) gum::ExactBNdistance<double>;
%template ( GibbsBNdistance ) gum::GibbsBNdistance<double>;

%template ( CredalNet ) gum::credal::CredalNet<double>;
%template ( CNMonteCarloSampling ) gum::credal::CNMonteCarloSampling<double>;
%template ( CNLoopyPropagation ) gum::credal::CNLoopyPropagation<double>;

%template ( InfluenceDiagram) gum::InfluenceDiagram<double>;
%template ( ShaferShenoyLIMIDInference) gum::ShaferShenoyLIMIDInference<double>;

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
