/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Template implementation of FMDP/planning/StructuredPlaner.h classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

// =========================================================================
#include <queue>
#include <vector>
// #include <algorithm>
// #include <utility>
//  =========================================================================
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/core/functors.h>
// =========================================================================
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/tools/multidim/instantiation.h>
#include <agrum/tools/multidim/potential.h>
// =========================================================================
#include <agrum/FMDP/planning/structuredPlaner.h>
// =========================================================================

/// For shorter line and hence more comprehensive code purposes only
#define RECAST(x) reinterpret_cast< const MultiDimFunctionGraph< GUM_SCALAR >* >(x)

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
  template < typename GUM_SCALAR >
  INLINE StructuredPlaner< GUM_SCALAR >::StructuredPlaner(IOperatorStrategy< GUM_SCALAR >* opi,
                                                          GUM_SCALAR discountFactor,
                                                          GUM_SCALAR epsilon,
                                                          bool       verbose) :
      discountFactor_(discountFactor),
      operator_(opi), verbose_(verbose) {
    GUM_CONSTRUCTOR(StructuredPlaner);

    _threshold_    = epsilon;
    vFunction_     = nullptr;
    optimalPolicy_ = nullptr;
  }

  // ===========================================================================
  // Default destructor
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE StructuredPlaner< GUM_SCALAR >::~StructuredPlaner() {
    GUM_DESTRUCTOR(StructuredPlaner);

    if (vFunction_) { delete vFunction_; }

    if (optimalPolicy_) delete optimalPolicy_;

    delete operator_;
  }


  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                Datastructure access methods **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Initializes data structure needed for making the planning
  // ===========================================================================
  template < typename GUM_SCALAR >
  std::string StructuredPlaner< GUM_SCALAR >::optimalPolicy2String() {
    // ************************************************************************
    // Discarding the case where no \pi* have been computed
    if (!optimalPolicy_ || optimalPolicy_->root() == 0) return "NO OPTIMAL POLICY CALCULATED YET";

    // ************************************************************************
    // Initialisation

    // Declaration of the needed string stream
    std::stringstream output;
    std::stringstream terminalStream;
    std::stringstream nonTerminalStream;
    std::stringstream arcstream;

    // First line for the toDot
    output << std::endl << "digraph \" OPTIMAL POLICY \" {" << std::endl;

    // Form line for the internal node stream en the terminal node stream
    terminalStream << "node [shape = box];" << std::endl;
    nonTerminalStream << "node [shape = ellipse];" << std::endl;

    // For somme clarity in the final string
    std::string tab = "\t";

    // To know if we already checked a node or not
    Set< NodeId > visited;

    // FIFO of nodes to visit
    std::queue< NodeId > fifo;

    // Loading the FIFO
    fifo.push(optimalPolicy_->root());
    visited << optimalPolicy_->root();


    // ************************************************************************
    // Main loop
    while (!fifo.empty()) {
      // Node to visit
      NodeId currentNodeId = fifo.front();
      fifo.pop();

      // Checking if it is terminal
      if (optimalPolicy_->isTerminalNode(currentNodeId)) {
        // Get back the associated ActionSet
        ActionSet ase = optimalPolicy_->nodeValue(currentNodeId);

        // Creating a line for this node
        terminalStream << tab << currentNodeId << ";" << tab << currentNodeId << " [label=\""
                       << currentNodeId << " - ";

        // Enumerating and adding to the line the associated optimal actions
        for (SequenceIteratorSafe< Idx > valIter = ase.beginSafe(); valIter != ase.endSafe();
             ++valIter)
          terminalStream << fmdp_->actionName(*valIter) << " ";

        // Terminating line
        terminalStream << "\"];" << std::endl;
        continue;
      }

      // Either wise
      {
        // Geting back the associated internal node
        const InternalNode* currentNode = optimalPolicy_->node(currentNodeId);

        // Creating a line in internalnode stream for this node
        nonTerminalStream << tab << currentNodeId << ";" << tab << currentNodeId << " [label=\""
                          << currentNodeId << " - " << currentNode->nodeVar()->name() << "\"];"
                          << std::endl;

        // Going through the sons and agregating them according the the sons Ids
        HashTable< NodeId, LinkedList< Idx >* > sonMap;
        for (Idx sonIter = 0; sonIter < currentNode->nbSons(); ++sonIter) {
          if (!visited.exists(currentNode->son(sonIter))) {
            fifo.push(currentNode->son(sonIter));
            visited << currentNode->son(sonIter);
          }
          if (!sonMap.exists(currentNode->son(sonIter)))
            sonMap.insert(currentNode->son(sonIter), new LinkedList< Idx >());
          sonMap[currentNode->son(sonIter)]->addLink(sonIter);
        }

        // Adding to the arc stram
        for (auto sonIter = sonMap.beginSafe(); sonIter != sonMap.endSafe(); ++sonIter) {
          arcstream << tab << currentNodeId << " -> " << sonIter.key() << " [label=\" ";
          Link< Idx >* modaIter = sonIter.val()->list();
          while (modaIter) {
            arcstream << currentNode->nodeVar()->label(modaIter->element());
            if (modaIter->nextLink()) arcstream << ", ";
            modaIter = modaIter->nextLink();
          }
          arcstream << "\",color=\"#00ff00\"];" << std::endl;
          delete sonIter.val();
        }
      }
    }

    // Terminating
    output << terminalStream.str() << std::endl
           << nonTerminalStream.str() << std::endl
           << arcstream.str() << std::endl
           << "}" << std::endl;

    return output.str();
  }


  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                     Planning Methods **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Initializes data structure needed for making the planning
  // ===========================================================================
  template < typename GUM_SCALAR >
  void StructuredPlaner< GUM_SCALAR >::initialize(const FMDP< GUM_SCALAR >* fmdp) {
    fmdp_ = fmdp;

    // Determination of the threshold value
    _threshold_ *= (1 - discountFactor_) / (2 * discountFactor_);

    // Establishement of sequence of variable elemination
    for (auto varIter = fmdp_->beginVariables(); varIter != fmdp_->endVariables(); ++varIter)
      elVarSeq_ << fmdp_->main2prime(*varIter);

    // Initialisation of the value function
    vFunction_     = operator_->getFunctionInstance();
    optimalPolicy_ = operator_->getAggregatorInstance();
    _firstTime_    = true;
  }


  // ===========================================================================
  // Performs a value iteration
  // ===========================================================================
  template < typename GUM_SCALAR >
  void StructuredPlaner< GUM_SCALAR >::makePlanning(Idx nbStep) {
    if (_firstTime_) {
      this->initVFunction_();
      _firstTime_ = false;
    }

    // *****************************************************************************************
    // Main loop
    // *****************************************************************************************
    Idx        nbIte = 0;
    GUM_SCALAR gap   = _threshold_ + 1;
    while ((gap > _threshold_) && (nbIte < nbStep)) {
      ++nbIte;

      MultiDimFunctionGraph< GUM_SCALAR >* newVFunction = this->valueIteration_();

      // *****************************************************************************************
      // Then we compare new value function and the old one
      MultiDimFunctionGraph< GUM_SCALAR >* deltaV = operator_->subtract(newVFunction, vFunction_);
      gap                                         = 0;

      for (deltaV->beginValues(); deltaV->hasValue(); deltaV->nextValue())
        if (gap < fabs(deltaV->value())) gap = fabs(deltaV->value());
      delete deltaV;

      if (verbose_)
        std::cout << " ------------------- Fin itération n° " << nbIte << std::endl
                  << " Gap : " << gap << " - " << _threshold_ << std::endl;

      // *****************************************************************************************
      // And eventually we update pointers for next loop
      delete vFunction_;
      vFunction_ = newVFunction;
    }

    // *****************************************************************************************
    // Policy matching value function research
    // *****************************************************************************************
    this->evalPolicy_();
  }


  // ===========================================================================
  // Performs a single step of value iteration
  // ===========================================================================
  template < typename GUM_SCALAR >
  void StructuredPlaner< GUM_SCALAR >::initVFunction_() {
    vFunction_->copy(*(RECAST(fmdp_->reward())));
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
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >* StructuredPlaner< GUM_SCALAR >::valueIteration_() {
    // *****************************************************************************************
    // Loop reset
    MultiDimFunctionGraph< GUM_SCALAR >* newVFunction = operator_->getFunctionInstance();
    newVFunction->copyAndReassign(*vFunction_, fmdp_->mapMainPrime());

    // *****************************************************************************************
    // For each action
    std::vector< MultiDimFunctionGraph< GUM_SCALAR >* > qActionsSet;
    for (auto actionIter = fmdp_->beginActions(); actionIter != fmdp_->endActions(); ++actionIter) {
      MultiDimFunctionGraph< GUM_SCALAR >* qAction = this->evalQaction_(newVFunction, *actionIter);
      qActionsSet.push_back(qAction);
    }
    delete newVFunction;

    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    newVFunction = this->maximiseQactions_(qActionsSet);

    // *******************************************************************************************
    // Next, we eval the new function value
    newVFunction = this->addReward_(newVFunction);

    return newVFunction;
  }


  // ===========================================================================
  // Evals the q function for current fmdp action
  // ===========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     StructuredPlaner< GUM_SCALAR >::evalQaction_(const MultiDimFunctionGraph< GUM_SCALAR >* Vold,
                                                  Idx actionId) {
    // ******************************************************************************
    // Initialisation :
    // Creating a copy of last Vfunction to deduce from the new Qaction
    // And finding the first var to eleminate (the one at the end)

    return operator_->regress(Vold, actionId, this->fmdp_, this->elVarSeq_);
  }


  // ===========================================================================
  // Maximise the AAction to iobtain the vFunction
  // ===========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >* StructuredPlaner< GUM_SCALAR >::maximiseQactions_(
     std::vector< MultiDimFunctionGraph< GUM_SCALAR >* >& qActionsSet) {
    MultiDimFunctionGraph< GUM_SCALAR >* newVFunction = qActionsSet.back();
    qActionsSet.pop_back();

    while (!qActionsSet.empty()) {
      MultiDimFunctionGraph< GUM_SCALAR >* qAction = qActionsSet.back();
      qActionsSet.pop_back();
      newVFunction = operator_->maximize(newVFunction, qAction);
    }

    return newVFunction;
  }


  // ===========================================================================
  // Maximise the AAction to iobtain the vFunction
  // ===========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >* StructuredPlaner< GUM_SCALAR >::minimiseFunctions_(
     std::vector< MultiDimFunctionGraph< GUM_SCALAR >* >& qActionsSet) {
    MultiDimFunctionGraph< GUM_SCALAR >* newVFunction = qActionsSet.back();
    qActionsSet.pop_back();

    while (!qActionsSet.empty()) {
      MultiDimFunctionGraph< GUM_SCALAR >* qAction = qActionsSet.back();
      qActionsSet.pop_back();
      newVFunction = operator_->minimize(newVFunction, qAction);
    }

    return newVFunction;
  }


  // ===========================================================================
  // Updates the value function by multiplying by discount and adding reward
  // ===========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     StructuredPlaner< GUM_SCALAR >::addReward_(MultiDimFunctionGraph< GUM_SCALAR >* Vold,
                                                Idx                                  actionId) {
    // *****************************************************************************************
    // ... we multiply the result by the discount factor, ...
    MultiDimFunctionGraph< GUM_SCALAR >* newVFunction = operator_->getFunctionInstance();
    newVFunction->copyAndMultiplyByScalar(*Vold, this->discountFactor_);
    delete Vold;

    // *****************************************************************************************
    // ... and finally add reward
    newVFunction = operator_->add(newVFunction, RECAST(fmdp_->reward(actionId)));

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
  template < typename GUM_SCALAR >
  void StructuredPlaner< GUM_SCALAR >::evalPolicy_() {
    // *****************************************************************************************
    // Loop reset
    MultiDimFunctionGraph< GUM_SCALAR >* newVFunction = operator_->getFunctionInstance();
    newVFunction->copyAndReassign(*vFunction_, fmdp_->mapMainPrime());

    std::vector< MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* >
       argMaxQActionsSet;
    // *****************************************************************************************
    // For each action
    for (auto actionIter = fmdp_->beginActions(); actionIter != fmdp_->endActions(); ++actionIter) {
      MultiDimFunctionGraph< GUM_SCALAR >* qAction = this->evalQaction_(newVFunction, *actionIter);

      qAction = this->addReward_(qAction);

      argMaxQActionsSet.push_back(makeArgMax_(qAction, *actionIter));
    }
    delete newVFunction;


    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* argMaxVFunction
       = argmaximiseQactions_(argMaxQActionsSet);

    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    extractOptimalPolicy_(argMaxVFunction);
  }


  // ===========================================================================
  // Creates a copy of given in parameter decision Graph and replaces leaves
  // of that Graph by a pair containing value of the leaf and action to which
  // is bind this Graph (given in parameter).
  // ===========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >*
     StructuredPlaner< GUM_SCALAR >::makeArgMax_(const MultiDimFunctionGraph< GUM_SCALAR >* qAction,
                                                 Idx actionId) {
    MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* amcpy
       = operator_->getArgMaxFunctionInstance();

    // Insertion des nouvelles variables
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter
         = qAction->variablesSequence().beginSafe();
         varIter != qAction->variablesSequence().endSafe();
         ++varIter)
      amcpy->add(**varIter);

    HashTable< NodeId, NodeId > src2dest;
    amcpy->manager()->setRootNode(
       _recurArgMaxCopy_(qAction->root(), actionId, qAction, amcpy, src2dest));

    delete qAction;
    return amcpy;
  }


  // ==========================================================================
  // Recursion part for the createArgMaxCopy
  // ==========================================================================
  template < typename GUM_SCALAR >
  NodeId StructuredPlaner< GUM_SCALAR >::_recurArgMaxCopy_(
     NodeId                                                                        currentNodeId,
     Idx                                                                           actionId,
     const MultiDimFunctionGraph< GUM_SCALAR >*                                    src,
     MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* argMaxCpy,
     HashTable< NodeId, NodeId >&                                                  visitedNodes) {
    if (visitedNodes.exists(currentNodeId)) return visitedNodes[currentNodeId];

    NodeId nody;
    if (src->isTerminalNode(currentNodeId)) {
      ArgMaxSet< GUM_SCALAR, Idx > leaf(src->nodeValue(currentNodeId), actionId);
      nody = argMaxCpy->manager()->addTerminalNode(leaf);
    } else {
      const InternalNode* currentNode = src->node(currentNodeId);
      NodeId*             sonsMap     = static_cast< NodeId* >(
         SOA_ALLOCATE(sizeof(NodeId) * currentNode->nodeVar()->domainSize()));
      for (Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda)
        sonsMap[moda]
           = _recurArgMaxCopy_(currentNode->son(moda), actionId, src, argMaxCpy, visitedNodes);
      nody = argMaxCpy->manager()->addInternalNode(currentNode->nodeVar(), sonsMap);
    }
    visitedNodes.insert(currentNodeId, nody);
    return nody;
  }


  // ===========================================================================
  // Performs argmax_a Q(s,a)
  // ===========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >*
     StructuredPlaner< GUM_SCALAR >::argmaximiseQactions_(
        std::vector< MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >,
                                            SetTerminalNodePolicy >* >& qActionsSet) {
    MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* newVFunction
       = qActionsSet.back();
    qActionsSet.pop_back();

    while (!qActionsSet.empty()) {
      MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* qAction
         = qActionsSet.back();
      qActionsSet.pop_back();
      newVFunction = operator_->argmaximize(newVFunction, qAction);
    }

    return newVFunction;
  }

  // ===========================================================================
  // Creates a copy of given in parameter decision Graph and replaces leaves
  // of that Graph by a pair containing value of the leaf and action to which
  // is bind this Graph (given in parameter).
  // ===========================================================================
  template < typename GUM_SCALAR >
  void StructuredPlaner< GUM_SCALAR >::extractOptimalPolicy_(
     const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >*
        argMaxOptimalValueFunction) {
    optimalPolicy_->clear();

    // Insertion des nouvelles variables
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter
         = argMaxOptimalValueFunction->variablesSequence().beginSafe();
         varIter != argMaxOptimalValueFunction->variablesSequence().endSafe();
         ++varIter)
      optimalPolicy_->add(**varIter);

    HashTable< NodeId, NodeId > src2dest;
    optimalPolicy_->manager()->setRootNode(_recurExtractOptPol_(argMaxOptimalValueFunction->root(),
                                                                argMaxOptimalValueFunction,
                                                                src2dest));

    delete argMaxOptimalValueFunction;
  }


  // ==========================================================================
  // Recursion part for the createArgMaxCopy
  // ==========================================================================
  template < typename GUM_SCALAR >
  NodeId StructuredPlaner< GUM_SCALAR >::_recurExtractOptPol_(
     NodeId currentNodeId,
     const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >*
                                  argMaxOptVFunc,
     HashTable< NodeId, NodeId >& visitedNodes) {
    if (visitedNodes.exists(currentNodeId)) return visitedNodes[currentNodeId];

    NodeId nody;
    if (argMaxOptVFunc->isTerminalNode(currentNodeId)) {
      ActionSet leaf;
      _transferActionIds_(argMaxOptVFunc->nodeValue(currentNodeId), leaf);
      nody = optimalPolicy_->manager()->addTerminalNode(leaf);
    } else {
      const InternalNode* currentNode = argMaxOptVFunc->node(currentNodeId);
      NodeId*             sonsMap     = static_cast< NodeId* >(
         SOA_ALLOCATE(sizeof(NodeId) * currentNode->nodeVar()->domainSize()));
      for (Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda)
        sonsMap[moda] = _recurExtractOptPol_(currentNode->son(moda), argMaxOptVFunc, visitedNodes);
      nody = optimalPolicy_->manager()->addInternalNode(currentNode->nodeVar(), sonsMap);
    }
    visitedNodes.insert(currentNodeId, nody);
    return nody;
  }

  // ==========================================================================
  // Extract from an ArgMaxSet the associated ActionSet
  // ==========================================================================
  template < typename GUM_SCALAR >
  void StructuredPlaner< GUM_SCALAR >::_transferActionIds_(const ArgMaxSet< GUM_SCALAR, Idx >& src,
                                                           ActionSet& dest) {
    for (auto idi = src.beginSafe(); idi != src.endSafe(); ++idi)
      dest += *idi;
  }


}   // end of namespace gum
