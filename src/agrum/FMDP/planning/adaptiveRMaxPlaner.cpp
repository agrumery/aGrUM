/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
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


/**
 * @file
 * @brief Template implementation of AdaptiveRMaxPlaner classes.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe GONZALES(@AMU)
 */

// =========================================================================
#include <queue>
#include <vector>
//#include <algorithm>
//#include <utility>
// =========================================================================
#include <agrum/tools/core/math/math.h>
#include <agrum/tools/core/functors.h>
#include <agrum/tools/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/tools/multidim/instantiation.h>
#include <agrum/tools/multidim/potential.h>
// =========================================================================
#include <agrum/FMDP/planning/adaptiveRMaxPlaner.h>
// =========================================================================

/// For shorter line and hence more comprehensive code purposes only
#define RECASTED(x) reinterpret_cast< const MultiDimFunctionGraph< double >* >(x)

namespace gum {

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                Constructors / Destructors **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Default constructor
  // ===========================================================================
  AdaptiveRMaxPlaner::AdaptiveRMaxPlaner(IOperatorStrategy< double >* opi,
                                         double                   discountFactor,
                                         double                   epsilon,
                                         const ILearningStrategy* learner,
                                         bool                     verbose) :
      StructuredPlaner(opi, discountFactor, epsilon, verbose),
      IDecisionStrategy(), fmdpLearner__(learner), initialized__(false) {
    GUM_CONSTRUCTOR(AdaptiveRMaxPlaner);
  }

  // ===========================================================================
  // Default destructor
  // ===========================================================================
  AdaptiveRMaxPlaner::~AdaptiveRMaxPlaner() {
    GUM_DESTRUCTOR(AdaptiveRMaxPlaner);

    for (HashTableIteratorSafe< Idx, StatesCounter* > scIter =
            counterTable__.beginSafe();
         scIter != counterTable__.endSafe();
         ++scIter)
      delete scIter.val();
  }

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                     Planning Methods **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ==========================================================================
  // Initializes data structure needed for making the planning
  // ==========================================================================
  void AdaptiveRMaxPlaner::initialize(const FMDP< double >* fmdp) {
    if (!initialized__) {
      StructuredPlaner::initialize(fmdp);
      IDecisionStrategy::initialize(fmdp);
      for (auto actionIter = fmdp->beginActions();
           actionIter != fmdp->endActions();
           ++actionIter) {
        counterTable__.insert(*actionIter, new StatesCounter());
        initializedTable__.insert(*actionIter, false);
      }
      initialized__ = true;
    }
  }

  // ===========================================================================
  // Performs a value iteration
  // ===========================================================================
  void AdaptiveRMaxPlaner::makePlanning(Idx nbStep) {
    makeRMaxFunctionGraphs__();

    StructuredPlaner::makePlanning(nbStep);

    clearTables__();
  }

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                   Value Iteration Methods **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Performs a single step of value iteration
  // ===========================================================================
  void AdaptiveRMaxPlaner::initVFunction_() {
    vFunction_->manager()->setRootNode(
       vFunction_->manager()->addTerminalNode(0.0));
    for (auto actionIter = fmdp_->beginActions();
         actionIter != fmdp_->endActions();
         ++actionIter)
      vFunction_ = this->operator_->add(
         vFunction_, RECASTED(this->fmdp_->reward(*actionIter)), 1);
  }

  // ===========================================================================
  // Performs a single step of value iteration
  // ===========================================================================
  MultiDimFunctionGraph< double >* AdaptiveRMaxPlaner::valueIteration_() {
    // *****************************************************************************************
    // Loop reset
    MultiDimFunctionGraph< double >* newVFunction =
       operator_->getFunctionInstance();
    newVFunction->copyAndReassign(*vFunction_, fmdp_->mapMainPrime());

    // *****************************************************************************************
    // For each action
    std::vector< MultiDimFunctionGraph< double >* > qActionsSet;
    for (auto actionIter = fmdp_->beginActions();
         actionIter != fmdp_->endActions();
         ++actionIter) {
      MultiDimFunctionGraph< double >* qAction =
         evalQaction_(newVFunction, *actionIter);

      // *******************************************************************************************
      // Next, we add the reward
      qAction = addReward_(qAction, *actionIter);

      qAction = this->operator_->maximize(
         actionsRMaxTable__[*actionIter],
         this->operator_->multiply(qAction, actionsBoolTable__[*actionIter], 1),
         2);

      qActionsSet.push_back(qAction);
    }
    delete newVFunction;

    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    newVFunction = maximiseQactions_(qActionsSet);

    return newVFunction;
  }

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                   Optimal Policy Evaluation Methods **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Evals the policy corresponding to the given value function
  // ===========================================================================
  void AdaptiveRMaxPlaner::evalPolicy_() {
    // *****************************************************************************************
    // Loop reset
    MultiDimFunctionGraph< double >* newVFunction =
       operator_->getFunctionInstance();
    newVFunction->copyAndReassign(*vFunction_, fmdp_->mapMainPrime());

    std::vector<
       MultiDimFunctionGraph< ArgMaxSet< double, Idx >, SetTerminalNodePolicy >* >
       argMaxQActionsSet;
    // *****************************************************************************************
    // For each action
    for (auto actionIter = fmdp_->beginActions();
         actionIter != fmdp_->endActions();
         ++actionIter) {
      MultiDimFunctionGraph< double >* qAction =
         this->evalQaction_(newVFunction, *actionIter);

      qAction = this->addReward_(qAction, *actionIter);

      qAction = this->operator_->maximize(
         actionsRMaxTable__[*actionIter],
         this->operator_->multiply(qAction, actionsBoolTable__[*actionIter], 1),
         2);

      argMaxQActionsSet.push_back(makeArgMax_(qAction, *actionIter));
    }
    delete newVFunction;

    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    MultiDimFunctionGraph< ArgMaxSet< double, Idx >, SetTerminalNodePolicy >*
       argMaxVFunction = argmaximiseQactions_(argMaxQActionsSet);

    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    extractOptimalPolicy_(argMaxVFunction);
  }

  // ===========================================================================
  //
  // ===========================================================================
  void AdaptiveRMaxPlaner::makeRMaxFunctionGraphs__() {
    rThreshold__ =
       fmdpLearner__->modaMax() * 5 > 30 ? fmdpLearner__->modaMax() * 5 : 30;
    rmax__ = fmdpLearner__->rMax() / (1.0 - this->discountFactor_);

    for (auto actionIter = this->fmdp()->beginActions();
         actionIter != this->fmdp()->endActions();
         ++actionIter) {
      std::vector< MultiDimFunctionGraph< double >* > rmaxs;
      std::vector< MultiDimFunctionGraph< double >* > boolQs;

      for (auto varIter = this->fmdp()->beginVariables();
           varIter != this->fmdp()->endVariables();
           ++varIter) {
        const IVisitableGraphLearner* visited = counterTable__[*actionIter];

        MultiDimFunctionGraph< double >* varRMax =
           this->operator_->getFunctionInstance();
        MultiDimFunctionGraph< double >* varBoolQ =
           this->operator_->getFunctionInstance();

        visited->insertSetOfVars(varRMax);
        visited->insertSetOfVars(varBoolQ);

        std::pair< NodeId, NodeId > rooty =
           visitLearner__(visited, visited->root(), varRMax, varBoolQ);
        varRMax->manager()->setRootNode(rooty.first);
        varRMax->manager()->reduce();
        varRMax->manager()->clean();
        varBoolQ->manager()->setRootNode(rooty.second);
        varBoolQ->manager()->reduce();
        varBoolQ->manager()->clean();

        rmaxs.push_back(varRMax);
        boolQs.push_back(varBoolQ);

        //          std::cout << RECASTED(this->fmdp_->transition(*actionIter,
        //          *varIter))->toDot() << std::endl;
        //          for( auto varIter2 =
        //          RECASTED(this->fmdp_->transition(*actionIter,
        //          *varIter))->variablesSequence().beginSafe(); varIter2 !=
        //          RECASTED(this->fmdp_->transition(*actionIter,
        //          *varIter))->variablesSequence().endSafe(); ++varIter2 )
        //              std::cout << (*varIter2)->name() << " | ";
        //          std::cout << std::endl;

        //          std::cout << varRMax->toDot() << std::endl;
        //          for( auto varIter =
        //          varRMax->variablesSequence().beginSafe(); varIter !=
        //          varRMax->variablesSequence().endSafe(); ++varIter )
        //              std::cout << (*varIter)->name() << " | ";
        //          std::cout << std::endl;

        //          std::cout << varBoolQ->toDot() << std::endl;
        //          for( auto varIter =
        //          varBoolQ->variablesSequence().beginSafe(); varIter !=
        //          varBoolQ->variablesSequence().endSafe(); ++varIter )
        //              std::cout << (*varIter)->name() << " | ";
        //          std::cout << std::endl;
      }

      //        std::cout << "Maximising" << std::endl;
      actionsRMaxTable__.insert(*actionIter, this->maximiseQactions_(rmaxs));
      actionsBoolTable__.insert(*actionIter, this->minimiseFunctions_(boolQs));
    }
  }

  // ===========================================================================
  //
  // ===========================================================================
  std::pair< NodeId, NodeId >
     AdaptiveRMaxPlaner::visitLearner__(const IVisitableGraphLearner* visited,
                                        NodeId currentNodeId,
                                        MultiDimFunctionGraph< double >* rmax,
                                        MultiDimFunctionGraph< double >* boolQ) {
    std::pair< NodeId, NodeId > rep;
    if (visited->isTerminal(currentNodeId)) {
      rep.first = rmax->manager()->addTerminalNode(
         visited->nodeNbObservation(currentNodeId) < rThreshold__ ? rmax__ : 0.0);
      rep.second = boolQ->manager()->addTerminalNode(
         visited->nodeNbObservation(currentNodeId) < rThreshold__ ? 0.0 : 1.0);
      return rep;
    }

    NodeId* rmaxsons = static_cast< NodeId* >(SOA_ALLOCATE(
       sizeof(NodeId) * visited->nodeVar(currentNodeId)->domainSize()));
    NodeId* bqsons = static_cast< NodeId* >(SOA_ALLOCATE(
       sizeof(NodeId) * visited->nodeVar(currentNodeId)->domainSize()));

    for (Idx moda = 0; moda < visited->nodeVar(currentNodeId)->domainSize();
         ++moda) {
      std::pair< NodeId, NodeId > sonp = visitLearner__(
         visited, visited->nodeSon(currentNodeId, moda), rmax, boolQ);
      rmaxsons[moda] = sonp.first;
      bqsons[moda] = sonp.second;
    }

    rep.first =
       rmax->manager()->addInternalNode(visited->nodeVar(currentNodeId), rmaxsons);
    rep.second =
       boolQ->manager()->addInternalNode(visited->nodeVar(currentNodeId), bqsons);
    return rep;
  }

  // ===========================================================================
  //
  // ===========================================================================
  void AdaptiveRMaxPlaner::clearTables__() {
    for (auto actionIter = this->fmdp()->beginActions();
         actionIter != this->fmdp()->endActions();
         ++actionIter) {
      delete actionsBoolTable__[*actionIter];
      delete actionsRMaxTable__[*actionIter];
    }
    actionsRMaxTable__.clear();
    actionsBoolTable__.clear();
  }

}   // end of namespace gum
