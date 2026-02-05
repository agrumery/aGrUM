/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/









/* TO AVOID NAMESPACE CLASHES */
%rename(OMP_getMaxNumberOfThreads) gum::threadsOMP::getMaxNumberOfThreads;
%rename(OMP_getNumberOfThreads) gum::threadsOMP::getNumberOfThreads;
%rename(OMP_getNumberOfLogicalProcessors) gum::threadsOMP::getNumberOfLogicalProcessors;
%rename(OMP_setNumberOfThreads) gum::threadsOMP::setNumberOfThreads;

%rename(STL_getMaxNumberOfThreads) gum::threadsSTL::getMaxNumberOfThreads;
%rename(STL_getNumberOfThreads) gum::threadsSTL::getNumberOfThreads;
%rename(STL_getNumberOfLogicalProcessors) gum::threadsSTL::getNumberOfLogicalProcessors;
%rename(STL_setNumberOfThreads) gum::threadsSTL::setNumberOfThreads;

/* INCLUDES */
%{
#include <agrum/base.h>
%}


%include "typemaps.i"
%include "std_vector.i"
%include "std_string.i"

%include "std_unique_ptr.i"
%unique_ptr(gum::DiscreteVariable) // to handle gum::fastVariable

namespace std {
    %template ( Vector) vector<double>;
    %template ( Vector_uint) vector<unsigned int>;
    %template ( Vector_int) vector<int>;
    %template ( Vector_string) vector<string>;
}


%ignore *::newFactory;

/* CLASS EXTENSIONS */
%extend gum::DiscreteVariable {
  gum::LabelizedVariable asLabelizedVar() {
    if (self->varType()!=gum::VarType::LABELIZED) {
      GUM_ERROR(gum::OperationNotAllowed,"impossible cast as gum.LabelizedVar for "<<*self);
    }
    return *static_cast<gum::LabelizedVariable * >(self);
  }

  gum::RangeVariable asRangeVar() {
    if (self->varType() != gum::VarType::RANGE)
      GUM_ERROR(gum::OperationNotAllowed,"impossible cast as gum.RangeVar for "<<*self);
    return *static_cast<gum::RangeVariable*>(self);
  }

  gum::IntegerVariable asIntegerVar() {
    if (self->varType()!=gum::VarType::INTEGER) {
      GUM_ERROR(gum::OperationNotAllowed, "impossible cast as gum.IntegerVar for " << *self);
    }
    return *static_cast<gum::IntegerVariable*>(self );
  }

  gum::NumericalDiscreteVariable asNumericalDiscreteVar() {
    if (self->varType()!=gum::VarType::NUMERICAL) {
      GUM_ERROR(gum::OperationNotAllowed, "impossible cast as gum.NumericalDiscreteVar for " << *self);
    }
    return *static_cast<gum::NumericalDiscreteVariable *>(self);
  }

  gum::DiscretizedVariable<double> asDiscretizedVar() {
    if (self->varType()!=gum::VarType::DISCRETIZED) {
      GUM_ERROR(gum::OperationNotAllowed, "impossible cast as gum.DiscretizedVar for " << *self);
    }
    return *static_cast<gum::DiscretizedVariable<double> *>(self);
  }
}


%include "forUsing.i"

/* EXCEPTION HANDLING */
%exceptionclass std::bad_cast;

%exceptionclass gum::Exception;

%exceptionclass gum::CPTError;
%exceptionclass gum::DefaultInLabel;
%exceptionclass gum::DuplicateElement;
%exceptionclass gum::DuplicateLabel;
%exceptionclass gum::FatalError;
%exceptionclass gum::FormatNotFound;
%exceptionclass gum::GraphError;
%exceptionclass gum::IOError;
%exceptionclass gum::InvalidArc;
%exceptionclass gum::InvalidArgument;
%exceptionclass gum::InvalidArgumentsNumber;
%exceptionclass gum::InvalidDirectedCycle;
%exceptionclass gum::InvalidEdge;
%exceptionclass gum::InvalidNode;
%exceptionclass gum::DatabaseError;
%exceptionclass gum::MissingValueInDatabase;
%exceptionclass gum::MissingVariableInDatabase;
%exceptionclass gum::NoChild;
%exceptionclass gum::NoNeighbour;
%exceptionclass gum::NoParent;
%exceptionclass gum::NotFound;
%exceptionclass gum::NullElement;
%exceptionclass gum::OperationNotAllowed;
%exceptionclass gum::OutOfBounds;
%exceptionclass gum::ArgumentError;
%exceptionclass gum::SizeError;
%exceptionclass gum::SyntaxError;
%exceptionclass gum::UndefinedElement;
%exceptionclass gum::UndefinedIteratorKey;
%exceptionclass gum::UndefinedIteratorValue;
%exceptionclass gum::UnknownLabelInDatabase;

%rename gum::Exception GumException;
%ignore gum::GraphicalModel::spaceCplxToStream(std::stringstream& s, double dSize, int dim, Size usedMem);

/* WRAPPED HEADERS (pyAgrum API) */
%import <agrum/config.h>
%import <agrum/base/core/inline.h>

%import <agrum/base/core/types.h>
%import <agrum/base/core/staticInitializer.h>
%include <agrum/base/core/set.h>
%include <agrum/base/core/exceptions.h>
%include <agrum/base/core/sequence.h>
%import <agrum/base/core/threads/threadsOMP.h>
%import <agrum/base/core/threads/threadsSTL.h>
%include <agrum/base/core/threads/threads.h>
%import <agrum/base/core/threads/threadNumberManager.h>
%include <agrum/base/core/utils_random.h>


%include <agrum/base/variables/variable.h>
%include <agrum/base/variables/discreteVariable.h>
%include <agrum/base/variables/labelizedVariable.h>
%include <agrum/base/variables/rangeVariable.h>
%include <agrum/base/variables/integerVariable.h>
%include <agrum/base/variables/numericalDiscreteVariable.h>
%include <agrum/base/variables/IDiscretizedVariable.h>
%include <agrum/base/variables/discretizedVariable.h>
%include <agrum/base/variables/allDiscreteVariables.h>

%include <agrum/base/graphs/graphElements.h>

%import <agrum/base/graphs/parts/edgeGraphPart.h>
%import <agrum/base/graphs/parts/arcGraphPart.h>
%import <agrum/base/graphs/parts/nodeGraphPart.h>

%include <agrum/base/graphs/undiGraph.h>
%include <agrum/base/graphs/diGraph.h>
%include <agrum/base/graphs/DAG.h>
%include <agrum/base/graphs/mixedGraph.h>
%include <agrum/base/graphs/PDAG.h>
%include <agrum/base/graphs/cliqueGraph.h>
%import <agrum/base/graphs/algorithms/triangulations/defaultTriangulation.h>
%import <agrum/base/graphs/algorithms/triangulations/triangulation.h>
%include <agrum/base/graphs/algorithms/MeekRules.h>

%import <agrum/base/multidim/implementations/multiDimInterface.h>
%import <agrum/base/multidim/implementations/multiDimAdressable.h>
%ignore gum::MultiDimContainer::populate(std::initializer_list< GUM_SCALAR > l) const;
%import <agrum/base/multidim/implementations/multiDimContainer.h>
%import <agrum/base/multidim/implementations/multiDimDecorator.h>
%import <agrum/base/multidim/implementations/multiDimImplementation.h>
%import <agrum/base/multidim/implementations/multiDimWithOffset.h>
%import <agrum/base/multidim/implementations/multiDimReadOnly.h>
%import <agrum/base/multidim/implementations/multiDimArray.h>

%include <agrum/base/multidim/ICIModels/multiDimNoisyORCompound.h>
%include <agrum/base/multidim/ICIModels/multiDimNoisyAND.h>

%include <agrum/base/multidim/instantiation.h>
%include <agrum/base/multidim/tensor.h>

%import <agrum/base/core/refPtr.h>
%include <agrum/base/core/list.h>

%import <agrum/base/graphicalModels/variableNodeMap.h>
%include <agrum/base/graphicalModels/graphicalModel.h>
%include <agrum/base/graphicalModels/DAGmodel.h>
%include <agrum/base/graphicalModels/UGmodel.h>

%import <agrum/base/core/approximations/IApproximationSchemeConfiguration.h>
%include <agrum/base/core/approximations/approximationScheme.h>

%ignore gum::BayesNetInference;
%import <agrum/base/graphicalModels/inference/scheduler/scheduledInference.h>

%import <agrum/base/core/signal/listener.h>
%import <agrum/base/graphs/parts/listeners/diGraphListener.h>

%import <agrum/base/database/databaseTable.h>

/* TEMPLATES FOR FUNCTIONS */
%template ( fastVariable) gum::fastVariable<double>;

/* TEMPLATES FOR CLASSES */
%template ( randomDistribution ) gum::randomDistribution<double>;

%template ( DiscretizedVariable ) gum::DiscretizedVariable<double>;

%template ( MultiDimContainer ) gum::MultiDimContainer<double>;
%template ( MultiDimImplementation ) gum::MultiDimImplementation<double>;
%template ( MultiDimDecorator ) gum::MultiDimDecorator<double>;
%template ( MultiDimWithOffset ) gum::MultiDimWithOffset<double>;
%template ( MultiDimArray ) gum::MultiDimArray<double>;

%template ( Tensor ) gum::Tensor<double>;

%template ( PairMPE ) std::pair<gum::Instantiation, double>;

/* for debug */
namespace gum {
  void statsObj(void);
}
%{
namespace gum {
  void statsObj(void) {
#ifdef GUM_DEBUG_MODE
    gum::__debug__::_atexit_();
#else
    //std::cout<<"Stats on aGrUM objects only available in debug mode"<<std::endl;
#endif // GUM_DEBUG_MODE
  }
}
%}


/* for multithreading */
namespace gum {
  namespace multithreading {
    unsigned int getNumberOfThreads();
    unsigned int getMaxNumberOfThreads();
    unsigned int getNumberOfLogicalProcessors();
  }
}
%{
namespace gum {
  namespace multithreading {
    unsigned int getNumberOfThreads() {
      return gum::getNumberOfThreads();
    }

    unsigned int getMaxNumberOfThreads() {
      return gum::getMaxNumberOfThreads();
    }

    unsigned int getNumberOfLogicalProcessors() {
      return gum::getNumberOfLogicalProcessors();
    }
  }
}
%}
