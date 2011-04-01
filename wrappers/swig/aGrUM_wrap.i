/* INCLUDES */
%{
#define SWIG_FILE_WITH_INIT
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
#include <agrum/core/set.h>
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/refPtr.h>
#include <agrum/core/list.h>
#include <agrum/graphs/diGraph.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/cliqueGraph.h>
#include <agrum/graphs/defaultTriangulation.h>
#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/multiDimInterface.h>
#include <agrum/multidim/multiDimAdressable.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimContainer.h>
#include <agrum/multidim/multiDimDecorator.h>
#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimWithOffset.h>
#include <agrum/multidim/multiDimReadOnly.h>
#include <agrum/multidim/multiDimNoisyORCompound.h>
#include <agrum/graphicalModels/variableNodeMap.h>

#include <agrum/core/sequence.h>
#include <agrum/multidim/variable.h>
#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/rangeVariable.h>
#include <agrum/multidim/discretizedVariable.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/BN/io/DSL/DSLReader.h>

#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/Gibbs.h>

#include <agrum/BN/algorithms/divergence/defaultKL.h>
#include <agrum/BN/algorithms/divergence/bruteForceKL.h>

#include <agrum/core/signal/listener.h>
#include <agrum/graphs/diGraphListener.h>

%}

%include "std_vector.i"
%include "std_string.i"

/* DIRECTORS (for cross language polymorphism) */
%feature("director") gum::Potential; //add the __disown__() method to Potential
%feature("nodirector") gum::MultiDimContainer::copyFrom;
%feature("director") gum::LabelizedVariable;
%feature("director") gum::DiscretizedVariable;
%feature("director") gum::LazyPropagation;
%feature("director") gum::Gibbs;

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
%import <agrum/core/inline.h>

%import <agrum/core/types.h>
%include <agrum/core/exceptions.h>
%include <agrum/core/sequence.h>
%include <agrum/core/bijection.h>

%include <agrum/multidim/variable.h>
%include <agrum/multidim/discreteVariable.h>
%include <agrum/multidim/labelizedVariable.h>
%include <agrum/multidim/rangeVariable.h>
%include <agrum/multidim/discretizedVariable.h>

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
%include <agrum/multidim/multiDimNoisyORCompound.h>
%include <agrum/multidim/potential.h>
%include <agrum/multidim/multiDimArray.h>

%import <agrum/core/refPtr.h>
%include <agrum/core/list.h>

%import <agrum/graphicalModels/variableNodeMap.h>
%import <agrum/BN/abstractBayesNet.h>
%include <agrum/BN/BayesNet.h>

%include <agrum/BN/inference/BayesNetInference.h>
%include <agrum/BN/inference/lazyPropagation.h>
%include <agrum/BN/inference/Gibbs.h>

%import <agrum/BN/algorithms/divergence/defaultKL.h>
%include <agrum/BN/algorithms/divergence/bruteForceKL.h>

%import <agrum/core/signal/listener.h>
%import <agrum/graphs/diGraphListener.h>
%import <agrum/BN/io/BIF/BIFReader.h>



/* CLASS EXTENSIONS */
%extend gum::DiscreteVariable {
    gum::LabelizedVariable& toLabelizedVar()
    {
        return dynamic_cast<gum::LabelizedVariable &>(*(self));
    }

    gum::RangeVariable& toRangeVar()
    {
        return dynamic_cast<gum::RangeVariable &>(*(self));
    }

    gum::DiscretizedVariable<float>& toDiscretizedVar()
    {
        return dynamic_cast<gum::DiscretizedVariable<float> &>(*(self));
    }
}


%extend gum::BayesNet {
    BayesNet(std::string name) {
        gum::BayesNet<T_DATA> *bn = new gum::BayesNet<T_DATA>();
        bn->setProperty("name", name);
        return bn;
    }
}


%extend gum::List {
    void append(Val val)
    {
        self->insert(val);
    }

    void push_front(Val val)
    {
        self->push_front(val);
    }

    void push_back(Val val)
    {
        self->push_back(val);
    }

    void remove(Val val)
    {
        self->eraseByVal(val);
    }

    void eradicate(Val val)
    {
        self->eraseAllVal(val);
    }

    const Val __getitem__(unsigned int i)
    {
        return (*(self))[i];
    }

    bool exists(Val val) const
    {
        return self->exists(val);
    }
}


/* TEMPLATES INSTANTIATIONS */
%template(Vector_float) std::vector<float>;
%template(Sequence_node) gum::Sequence<gum::NodeId>;
%template(Sequence_string) gum::Sequence<std::string>;
%template(DiscretizedVar) gum::DiscretizedVariable<float>;
%template(MultiDimContainer_float) gum::MultiDimContainer<float>;
%template(MultiDimImplementation_float) gum::MultiDimImplementation<float>;
%template(MultiDimDecorator_float) gum::MultiDimDecorator<float>;
%template(MultiDimWithOffset_float) gum::MultiDimWithOffset<float>;
%template(MultiDimArray_float) gum::MultiDimArray<float>;
%template(Potential_float) gum::Potential<float>;
%template(BaseListPotentials_float) gum::ListBase< const gum::Potential<float> *>;
%template(ListPotentials_float) gum::List< const gum::Potential<float> *>;
%template(AbstractBayesNet_float) gum::AbstractBayesNet<float>;
%template(BayesNet_float) gum::BayesNet<float>;
%template(BayesNetInference_float) gum::BayesNetInference<float>;
%template(LazyPropagation_float) gum::LazyPropagation<float>;
%template(Gibbs_float) gum::Gibbs<float>;
%template(BruteForceKL_float) gum::BruteForceKL<float>;

%template(Vector_double) std::vector<double>;
%template(MultiDimContainer_double) gum::MultiDimContainer<double>;
%template(MultiDimImplementation_double) gum::MultiDimImplementation<double>;
%template(MultiDimDecorator_double) gum::MultiDimDecorator<double>;
%template(MultiDimWithOffset_double) gum::MultiDimWithOffset<double>;
%template(MultiDimArray_double) gum::MultiDimArray<double>;
%template(Potential_double) gum::Potential<double>;
%template(BaseListPotentials_double)  gum::ListBase< const gum::Potential<double> *>;
%template(ListPotentials_double) gum::List< const gum::Potential<double> *>;
%template(AbstractBayesNet_double) gum::AbstractBayesNet<double>;
%template(BayesNet_double) gum::BayesNet<double>;
%template(BayesNetInference_double) gum::BayesNetInference<double>;
%template(LazyPropagation_double) gum::LazyPropagation<double>;
%template(Gibbs_double) gum::Gibbs<double>;
%template(BruteForceKL_double) gum::BruteForceKL<double>;
