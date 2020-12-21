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
 * @file Template Implementations of the IMDDI datastructure learner
 * @brief
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 */
// =======================================================
#include <agrum/tools/core/math/math.h>
#include <agrum/tools/core/priorityQueue.h>
#include <agrum/tools/core/types.h>
// =======================================================
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/datastructure/imddi.h>
// =======================================================
#include <agrum/tools/variables/labelizedVariable.h>
// =======================================================


namespace gum {

  // ############################################################################
  // Constructor & destructor.
  // ############################################################################

  // ============================================================================
  // Variable Learner constructor
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  IMDDI< AttributeSelection, isScalar >::IMDDI(
     MultiDimFunctionGraph< double >* target,
     double                           attributeSelectionThreshold,
     double                           pairSelectionThreshold,
     Set< const DiscreteVariable* >   attributeListe,
     const DiscreteVariable*          learnedValue) :
      IncrementalGraphLearner< AttributeSelection, isScalar >(
         target, attributeListe, learnedValue),
      lg__(&(this->model_), pairSelectionThreshold), nbTotalObservation__(0),
      attributeSelectionThreshold__(attributeSelectionThreshold) {
    GUM_CONSTRUCTOR(IMDDI);
    addLeaf__(this->root_);
  }

  // ============================================================================
  // Reward Learner constructor
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  IMDDI< AttributeSelection, isScalar >::IMDDI(
     MultiDimFunctionGraph< double >* target,
     double                           attributeSelectionThreshold,
     double                           pairSelectionThreshold,
     Set< const DiscreteVariable* >   attributeListe) :
      IncrementalGraphLearner< AttributeSelection, isScalar >(
         target, attributeListe, new LabelizedVariable("Reward", "", 2)),
      lg__(&(this->model_), pairSelectionThreshold), nbTotalObservation__(0),
      attributeSelectionThreshold__(attributeSelectionThreshold) {
    GUM_CONSTRUCTOR(IMDDI);
    addLeaf__(this->root_);
  }

  // ============================================================================
  // Reward Learner constructor
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  IMDDI< AttributeSelection, isScalar >::~IMDDI() {
    GUM_DESTRUCTOR(IMDDI);
    for (HashTableIteratorSafe< NodeId, AbstractLeaf* > leafIter =
            leafMap__.beginSafe();
         leafIter != leafMap__.endSafe();
         ++leafIter)
      delete leafIter.val();
  }


  // ############################################################################
  // Incrementals methods
  // ############################################################################

  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::addObservation(
     const Observation* obs) {
    nbTotalObservation__++;
    IncrementalGraphLearner< AttributeSelection, isScalar >::addObservation(obs);
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::updateNodeWithObservation_(
     const Observation* newObs, NodeId currentNodeId) {
    IncrementalGraphLearner< AttributeSelection,
                             isScalar >::updateNodeWithObservation_(newObs,
                                                                    currentNodeId);
    if (this->nodeVarMap_[currentNodeId] == this->value_)
      lg__.updateLeaf(leafMap__[currentNodeId]);
  }


  // ============================================================================
  // Updates the tree after a new observation has been added
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::updateGraph() {
    varOrder__.clear();

    // First xe initialize the node set which will give us the scores
    Set< NodeId > currentNodeSet;
    currentNodeSet.insert(this->root_);

    // Then we initialize the pool of variables to consider
    VariableSelector vs(this->setOfVars_);
    for (vs.begin(); vs.hasNext(); vs.next()) {
      updateScore__(vs.current(), this->root_, vs);
    }

    // Then, until there's no node remaining
    while (!vs.isEmpty()) {
      // We select the best var
      const DiscreteVariable* selectedVar = vs.select();
      varOrder__.insert(selectedVar);

      // Then we decide if we update each node according to this var
      updateNodeSet__(currentNodeSet, selectedVar, vs);
    }

    // If there are remaining node that are not leaves after we establish the
    // var order
    // these nodes are turned into leaf.
    for (SetIteratorSafe< NodeId > nodeIter = currentNodeSet.beginSafe();
         nodeIter != currentNodeSet.endSafe();
         ++nodeIter)
      this->convertNode2Leaf_(*nodeIter);


    if (lg__.needsUpdate()) lg__.update();
  }


  // ############################################################################
  // Updating methods
  // ############################################################################


  // ###################################################################
  // Select the most relevant variable
  //
  // First parameter is the set of variables among which the most
  // relevant one is choosed
  // Second parameter is the set of node the will attribute a score
  // to each variable so that we choose the best.
  // ###################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::updateScore__(
     const DiscreteVariable* var, NodeId nody, VariableSelector& vs) {
    if (!this->nodeId2Database_[nody]->isTestRelevant(var)) return;
    double weight = (double)this->nodeId2Database_[nody]->nbObservation()
                    / (double)this->nbTotalObservation__;
    vs.updateScore(var,
                   weight * this->nodeId2Database_[nody]->testValue(var),
                   weight * this->nodeId2Database_[nody]->testOtherCriterion(var));
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::downdateScore__(
     const DiscreteVariable* var, NodeId nody, VariableSelector& vs) {
    if (!this->nodeId2Database_[nody]->isTestRelevant(var)) return;
    double weight = (double)this->nodeId2Database_[nody]->nbObservation()
                    / (double)this->nbTotalObservation__;
    vs.downdateScore(var,
                     weight * this->nodeId2Database_[nody]->testValue(var),
                     weight
                        * this->nodeId2Database_[nody]->testOtherCriterion(var));
  }


  // ============================================================================
  // For each node in the given set, this methods checks whether or not
  // we should installed the given variable as a test.
  // If so, the node is updated
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::updateNodeSet__(
     Set< NodeId >&          nodeSet,
     const DiscreteVariable* selectedVar,
     VariableSelector&       vs) {
    Set< NodeId > oldNodeSet(nodeSet);
    nodeSet.clear();
    for (SetIteratorSafe< NodeId > nodeIter = oldNodeSet.beginSafe();
         nodeIter != oldNodeSet.endSafe();
         ++nodeIter) {
      if (this->nodeId2Database_[*nodeIter]->isTestRelevant(selectedVar)
          && this->nodeId2Database_[*nodeIter]->testValue(selectedVar)
                > attributeSelectionThreshold__) {
        this->transpose_(*nodeIter, selectedVar);

        // Then we subtract the from the score given to each variables the
        // quantity given by this node
        for (vs.begin(); vs.hasNext(); vs.next()) {
          downdateScore__(vs.current(), *nodeIter, vs);
        }

        // And finally we add all its child to the new set of nodes
        // and updates the remaining var's score
        for (Idx modality = 0;
             modality < this->nodeVarMap_[*nodeIter]->domainSize();
             ++modality) {
          NodeId sonId = this->nodeSonsMap_[*nodeIter][modality];
          nodeSet << sonId;

          for (vs.begin(); vs.hasNext(); vs.next()) {
            updateScore__(vs.current(), sonId, vs);
          }
        }
      } else {
        nodeSet << *nodeIter;
      }
    }
  }


  // ============================================================================
  // Insert a new node with given associated database, var and maybe sons
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeId IMDDI< AttributeSelection, isScalar >::insertLeafNode_(
     NodeDatabase< AttributeSelection, isScalar >* nDB,
     const DiscreteVariable*                       boundVar,
     Set< const Observation* >*                    obsSet) {
    NodeId currentNodeId =
       IncrementalGraphLearner< AttributeSelection, isScalar >::insertLeafNode_(
          nDB, boundVar, obsSet);

    addLeaf__(currentNodeId);

    return currentNodeId;
  }


  // ============================================================================
  // Changes var associated to a node
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::chgNodeBoundVar_(
     NodeId currentNodeId, const DiscreteVariable* desiredVar) {
    if (this->nodeVarMap_[currentNodeId] == this->value_)
      removeLeaf__(currentNodeId);

    IncrementalGraphLearner< AttributeSelection, isScalar >::chgNodeBoundVar_(
       currentNodeId, desiredVar);

    if (desiredVar == this->value_) addLeaf__(currentNodeId);
  }


  // ============================================================================
  // Remove node from graph
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::removeNode_(NodeId currentNodeId) {
    if (this->nodeVarMap_[currentNodeId] == this->value_)
      removeLeaf__(currentNodeId);
    IncrementalGraphLearner< AttributeSelection, isScalar >::removeNode_(
       currentNodeId);
  }


  // ============================================================================
  // Add leaf to aggregator
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::addLeaf__(NodeId currentNodeId) {
    leafMap__.insert(currentNodeId,
                     new ConcreteLeaf< AttributeSelection, isScalar >(
                        currentNodeId,
                        this->nodeId2Database_[currentNodeId],
                        &(this->valueAssumed_)));
    lg__.addLeaf(leafMap__[currentNodeId]);
  }


  // ============================================================================
  // Remove leaf from aggregator
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::removeLeaf__(NodeId currentNodeId) {
    lg__.removeLeaf(leafMap__[currentNodeId]);
    delete leafMap__[currentNodeId];
    leafMap__.erase(currentNodeId);
  }


  // ============================================================================
  // Computes the Reduced and Ordered Function Graph  associated to this ordered
  // tree
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::updateFunctionGraph() {
    //      if( lg__.needsUpdate() || this->needUpdate_ ){
    rebuildFunctionGraph__();
    this->needUpdate_ = false;
    //      }
  }


  // ============================================================================
  // Performs the leaves merging
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  void IMDDI< AttributeSelection, isScalar >::rebuildFunctionGraph__() {
    // *******************************************************************************************************
    // Mise à jour de l'aggregateur de feuille
    lg__.update();

    // *******************************************************************************************************
    // Reinitialisation du Graphe de Décision
    this->target_->clear();
    for (auto varIter = varOrder__.beginSafe(); varIter != varOrder__.endSafe();
         ++varIter)
      this->target_->add(**varIter);
    this->target_->add(*this->value_);

    HashTable< NodeId, NodeId > toTarget;

    // *******************************************************************************************************
    // Insertion des feuilles
    HashTable< NodeId, AbstractLeaf* > treeNode2leaf = lg__.leavesMap();
    HashTable< AbstractLeaf*, NodeId > leaf2DGNode;
    for (HashTableConstIteratorSafe< NodeId, AbstractLeaf* > treeNodeIter =
            treeNode2leaf.cbeginSafe();
         treeNodeIter != treeNode2leaf.cendSafe();
         ++treeNodeIter) {
      if (!leaf2DGNode.exists(treeNodeIter.val()))
        leaf2DGNode.insert(treeNodeIter.val(),
                           insertLeafInFunctionGraph__(treeNodeIter.val(),
                                                       Int2Type< isScalar >()));

      toTarget.insert(treeNodeIter.key(), leaf2DGNode[treeNodeIter.val()]);
    }

    // *******************************************************************************************************
    // Insertion des noeuds internes (avec vérification des possibilités de
    // fusion)
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter =
            varOrder__.rbeginSafe();
         varIter != varOrder__.rendSafe();
         --varIter) {
      for (Link< NodeId >* curNodeIter = this->var2Node_[*varIter]->list();
           curNodeIter;
           curNodeIter = curNodeIter->nextLink()) {
        NodeId* sonsMap = static_cast< NodeId* >(
           SOA_ALLOCATE(sizeof(NodeId) * (*varIter)->domainSize()));
        for (Idx modality = 0; modality < (*varIter)->domainSize(); ++modality)
          sonsMap[modality] =
             toTarget[this->nodeSonsMap_[curNodeIter->element()][modality]];
        toTarget.insert(
           curNodeIter->element(),
           this->target_->manager()->addInternalNode(*varIter, sonsMap));
      }
    }

    // *******************************************************************************************************
    // Polish
    this->target_->manager()->setRootNode(toTarget[this->root_]);
    this->target_->manager()->clean();
  }


  // ============================================================================
  // Performs the leaves merging
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeId IMDDI< AttributeSelection, isScalar >::insertLeafInFunctionGraph__(
     AbstractLeaf* leaf, Int2Type< true >) {
    double value = 0.0;
    for (Idx moda = 0; moda < leaf->nbModa(); moda++) {
      value += (double)leaf->effectif(moda) * this->valueAssumed_.atPos(moda);
    }
    if (leaf->total()) value /= (double)leaf->total();
    return this->target_->manager()->addTerminalNode(value);
  }


  // ============================================================================
  // Performs the leaves merging
  // ============================================================================
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeId IMDDI< AttributeSelection, isScalar >::insertLeafInFunctionGraph__(
     AbstractLeaf* leaf, Int2Type< false >) {
    NodeId* sonsMap = static_cast< NodeId* >(
       SOA_ALLOCATE(sizeof(NodeId) * this->value_->domainSize()));
    for (Idx modality = 0; modality < this->value_->domainSize(); ++modality) {
      double newVal = 0.0;
      if (leaf->total())
        newVal = (double)leaf->effectif(modality) / (double)leaf->total();
      sonsMap[modality] = this->target_->manager()->addTerminalNode(newVal);
    }
    return this->target_->manager()->addInternalNode(this->value_, sonsMap);
  }
}   // namespace gum
