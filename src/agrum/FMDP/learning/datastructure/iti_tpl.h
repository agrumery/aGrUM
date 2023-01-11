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
 * @file Template Implementations of the ITI datastructure learner
 * @brief
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
// =======================================================
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/core/priorityQueue.h>
#include <agrum/tools/core/types.h>
// =======================================================
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/datastructure/iti.h>
// =======================================================
#include <agrum/tools/variables/labelizedVariable.h>
// =======================================================


namespace gum {

  // ==========================================================================
  /// @name Constructor & destructor.
  // ==========================================================================

  // ###################################################################
  /**
   * ITI constructor for functions describing the behaviour of one variable
   * according to a set of other variable such as conditionnal probabilities
   * @param target : the MultiDimFunctionGraph in which we load the structure
   * @param attributeSelectionThreshold : threshold under which a node is not
   * installed (pe-pruning)
   * @param temporaryAPIfix : Issue in API in regard to IMDDI
   * @param attributeListe : Set of vars on which we rely to explain the
   * behaviour of learned variable
   * @param learnedValue : the variable from which we try to learn the behaviour
   */
  // ###################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  ITI< AttributeSelection, isScalar >::ITI(MultiDimFunctionGraph< double >* target,
                                           double attributeSelectionThreshold,
                                           Set< const DiscreteVariable* > attributeListe,
                                           const DiscreteVariable*        learnedValue) :
      IncrementalGraphLearner< AttributeSelection, isScalar >(target, attributeListe, learnedValue),
      _nbTotalObservation_(0), _attributeSelectionThreshold_(attributeSelectionThreshold) {
    GUM_CONSTRUCTOR(ITI);
    _staleTable_.insert(this->root_, false);
  }

  // ###################################################################
  /**
   * ITI constructeur for real functions. We try to predict the output of a
   * function f given a set of variable
   * @param target : the MultiDimFunctionGraph in which we load the structure
   * @param attributeSelectionThreshold : threshold under which a node is not
   * installed (pe-pruning)
   * @param temporaryAPIfix : Issue in API in regard to IMDDI
   * @param attributeListeSet of vars on which we rely to explain the
   * behaviour of learned function
   */
  // ###################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  ITI< AttributeSelection, isScalar >::ITI(MultiDimFunctionGraph< double >* target,
                                           double attributeSelectionThreshold,
                                           Set< const DiscreteVariable* > attributeListe) :
      IncrementalGraphLearner< AttributeSelection, isScalar >(
         target,
         attributeListe,
         new LabelizedVariable("Reward", "", 2)),
      _nbTotalObservation_(0), _attributeSelectionThreshold_(attributeSelectionThreshold) {
    GUM_CONSTRUCTOR(ITI);
    _staleTable_.insert(this->root_, false);
  }


  // ==========================================================================
  /// @name New Observation insertion methods
  // ==========================================================================

  // ############################################################################
  /**
   * Inserts a new observation
   * @param the new observation to learn
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void ITI< AttributeSelection, isScalar >::addObservation(const Observation* obs) {
    _nbTotalObservation_++;
    IncrementalGraphLearner< AttributeSelection, isScalar >::addObservation(obs);
  }

  // ############################################################################
  /**
   * Will update internal graph's NodeDatabase of given node with the new
   * observation
   * @param newObs
   * @param currentNodeId
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void ITI< AttributeSelection, isScalar >::updateNodeWithObservation_(const Observation* newObs,
                                                                       NodeId currentNodeId) {
    IncrementalGraphLearner< AttributeSelection, isScalar >::updateNodeWithObservation_(
       newObs,
       currentNodeId);
    _staleTable_[currentNodeId] = true;
  }


  // ============================================================================
  /// @name Graph Structure update methods
  // ============================================================================

  // ############################################################################
  /// Updates the internal graph after a new observation has been added
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void ITI< AttributeSelection, isScalar >::updateGraph() {
    std::vector< NodeId > filo;
    filo.push_back(this->root_);
    HashTable< NodeId, Set< const DiscreteVariable* >* > potentialVars;
    potentialVars.insert(this->root_, new Set< const DiscreteVariable* >(this->setOfVars_));


    while (!filo.empty()) {
      NodeId currentNodeId = filo.back();
      filo.pop_back();

      // First we look for the best var to install on the node
      double                         bestValue = _attributeSelectionThreshold_;
      Set< const DiscreteVariable* > bestVars;

      for (auto varIter = potentialVars[currentNodeId]->cbeginSafe();
           varIter != potentialVars[currentNodeId]->cendSafe();
           ++varIter)
        if (this->nodeId2Database_[currentNodeId]->isTestRelevant(*varIter)) {
          double varValue = this->nodeId2Database_[currentNodeId]->testValue(*varIter);
          if (varValue >= bestValue) {
            if (varValue > bestValue) {
              bestValue = varValue;
              bestVars.clear();
            }
            bestVars.insert(*varIter);
          }
        }

      // Then We installed Variable a test on that node
      this->updateNode_(currentNodeId, bestVars);

      // The we move on the children if needed
      if (this->nodeVarMap_[currentNodeId] != this->value_) {
        for (Idx moda = 0; moda < this->nodeVarMap_[currentNodeId]->domainSize(); moda++) {
          Set< const DiscreteVariable* >* itsPotentialVars
             = new Set< const DiscreteVariable* >(*potentialVars[currentNodeId]);
          itsPotentialVars->erase(this->nodeVarMap_[currentNodeId]);
          NodeId sonId = this->nodeSonsMap_[currentNodeId][moda];
          if (_staleTable_[sonId]) {
            filo.push_back(sonId);
            potentialVars.insert(sonId, itsPotentialVars);
          }
        }
      }
    }

    for (HashTableIteratorSafe< NodeId, Set< const DiscreteVariable* >* > nodeIter
         = potentialVars.beginSafe();
         nodeIter != potentialVars.endSafe();
         ++nodeIter)
      delete nodeIter.val();
  }


  // ############################################################################
  /**
   * inserts a new node in internal graohs
   * @param nDB : the associated database
   * @param boundVar : the associated variable
   * @return the newly created node's id
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeId ITI< AttributeSelection, isScalar >::insertNode_(
     NodeDatabase< AttributeSelection, isScalar >* nDB,
     const DiscreteVariable*                       boundVar) {
    NodeId n = IncrementalGraphLearner< AttributeSelection, isScalar >::insertNode_(nDB, boundVar);
    _staleTable_.insert(n, true);
    return n;
  }


  // ############################################################################
  /**
   * Changes the associated variable of a node
   * @param chgedNodeId : the node to change
   * @param desiredVar : its new associated variable
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void ITI< AttributeSelection, isScalar >::chgNodeBoundVar_(NodeId                  currentNodeId,
                                                             const DiscreteVariable* desiredVar) {
    if (this->nodeVarMap_[currentNodeId] != desiredVar) {
      _staleTable_[currentNodeId] = true;
      IncrementalGraphLearner< AttributeSelection, isScalar >::chgNodeBoundVar_(currentNodeId,
                                                                                desiredVar);
    }
  }


  // ############################################################################
  /**
   * Removes a node from the internal graph
   * @param removedNodeId : the node to remove
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void ITI< AttributeSelection, isScalar >::removeNode_(NodeId currentNodeId) {
    IncrementalGraphLearner< AttributeSelection, isScalar >::removeNode_(currentNodeId);
    _staleTable_.erase(currentNodeId);
  }


  // ============================================================================
  /// @name Function Graph Updating methods
  // ============================================================================

  // ############################################################################
  /// Updates target to currently learned graph structure
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void ITI< AttributeSelection, isScalar >::updateFunctionGraph() {
    this->target_->clear();
    this->target_->manager()->setRootNode(this->_insertNodeInFunctionGraph_(this->root_));
  }


  // ############################################################################
  /**
   * Inserts an internal node in the target
   * @param the source node in internal graph
   * @return the mathcing node id in the target
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeId ITI< AttributeSelection, isScalar >::_insertNodeInFunctionGraph_(NodeId currentNodeId) {
    if (this->nodeVarMap_[currentNodeId] == this->value_) {
      NodeId nody = _insertTerminalNode_(currentNodeId);
      return nody;
    }

    if (!this->target_->variablesSequence().exists(this->nodeVarMap_[currentNodeId])) {
      this->target_->add(*(this->nodeVarMap_[currentNodeId]));
    }

    NodeId nody = this->target_->manager()->addInternalNode(this->nodeVarMap_[currentNodeId]);
    for (Idx moda = 0; moda < this->nodeVarMap_[currentNodeId]->domainSize(); ++moda) {
      NodeId son = this->_insertNodeInFunctionGraph_(this->nodeSonsMap_[currentNodeId][moda]);
      this->target_->manager()->setSon(nody, moda, son);
    }

    return nody;
  }


  // ############################################################################
  /**
   * Insert a terminal node in the target.
   * This function is called if we're learning a real value function.
   * Inserts then a single value in target.
   * @param the source node in the learned graph
   * @return the matching node in the target
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeId ITI< AttributeSelection, isScalar >::_insertTerminalNode_(NodeId currentNodeId,
                                                                   Int2Type< false >) {
    if (!this->target_->variablesSequence().exists(this->value_))
      this->target_->add(*(this->value_));

    Size tot = this->nodeId2Database_[currentNodeId]->nbObservation();
    if (tot == Size(0)) return this->target_->manager()->addTerminalNode(0.0);

    NodeId* sonsMap
       = static_cast< NodeId* >(SOA_ALLOCATE(sizeof(NodeId) * this->value_->domainSize()));
    for (Idx modality = 0; modality < this->value_->domainSize(); ++modality) {
      double newVal = 0.0;
      newVal = (double)this->nodeId2Database_[currentNodeId]->effectif(modality) / (double)tot;
      sonsMap[modality] = this->target_->manager()->addTerminalNode(newVal);
    }
    NodeId nody = this->target_->manager()->addInternalNode(this->value_, sonsMap);
    return nody;
  }


  // ############################################################################
  /**
   * Insert a terminal node in the target.
   * This function is called if we're learning the behaviour of a variable.
   * Inserts then this variable and the relevant value beneath into target.
   * @param the source node in the learned graph
   * @return the matching node in the target
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeId ITI< AttributeSelection, isScalar >::_insertTerminalNode_(NodeId currentNodeId,
                                                                   Int2Type< true >) {
    double value = 0.0;
    for (auto valIter = this->nodeId2Database_[currentNodeId]->cbeginValues();
         valIter != this->nodeId2Database_[currentNodeId]->cendValues();
         ++valIter) {
      value += (double)valIter.key() * valIter.val();
    }
    if (this->nodeId2Database_[currentNodeId]->nbObservation())
      value /= (double)this->nodeId2Database_[currentNodeId]->nbObservation();
    NodeId nody = this->target_->manager()->addTerminalNode(value);
    return nody;
  }
}   // namespace gum
