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
 * @brief
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
// =======================================================
#include <queue>
// =======================================================
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/core/multiPriorityQueue.h>
#include <agrum/tools/core/types.h>
#include <agrum/tools/core/utils_random.h>
// =======================================================
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/datastructure/incrementalGraphLearner.h>
// =======================================================
#include <agrum/tools/variables/discreteVariable.h>
// =======================================================

namespace gum {

  // ============================================================================
  /// @name Constructor & destructor.
  // ============================================================================

  // ############################################################################
  /**
   * Default constructor
   * @param target : the output diagram usable by the outside
   * @param attributesSet : set of variables from which we try to describe the
   * learned function
   * @param learnVariable : if we tried to learn a the behaviour of a variable
   * given variable given another set of variables, this is the one. If we are
   * learning a function of real value, this is just a computationnal trick
   * (and is to be deprecated)
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  IncrementalGraphLearner< AttributeSelection, isScalar >::IncrementalGraphLearner(
     MultiDimFunctionGraph< double >* target,
     Set< const DiscreteVariable* >   varList,
     const DiscreteVariable*          value) :
      target_(target),
      setOfVars_(varList), value_(value) {
    GUM_CONSTRUCTOR(IncrementalGraphLearner);

    for (auto varIter = setOfVars_.cbeginSafe(); varIter != setOfVars_.cendSafe(); ++varIter)
      var2Node_.insert(*varIter, new LinkedList< NodeId >());
    var2Node_.insert(value_, new LinkedList< NodeId >());

    model_.addNode();
    this->root_
       = insertLeafNode_(new NodeDatabase< AttributeSelection, isScalar >(&setOfVars_, value_),
                         value_,
                         new Set< const Observation* >());
  }


  // ############################################################################
  /// Default destructor
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  IncrementalGraphLearner< AttributeSelection, isScalar >::~IncrementalGraphLearner() {
    for (auto nodeIter = nodeId2Database_.beginSafe(); nodeIter != nodeId2Database_.endSafe();
         ++nodeIter)
      delete nodeIter.val();

    for (auto nodeIter = nodeSonsMap_.beginSafe(); nodeIter != nodeSonsMap_.endSafe(); ++nodeIter)
      SOA_DEALLOCATE(nodeIter.val(), sizeof(NodeId) * nodeVarMap_[nodeIter.key()]->domainSize());

    for (auto varIter = var2Node_.beginSafe(); varIter != var2Node_.endSafe(); ++varIter)
      delete varIter.val();

    for (auto nodeIter = leafDatabase_.beginSafe(); nodeIter != leafDatabase_.endSafe(); ++nodeIter)
      delete nodeIter.val();

    _clearValue_();

    GUM_DESTRUCTOR(IncrementalGraphLearner);
  }


  // ============================================================================
  /// @name New Observation insertion methods
  // ============================================================================

  // ############################################################################
  /**
   * Inserts a new observation
   * @param the new observation to learn
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void IncrementalGraphLearner< AttributeSelection, isScalar >::addObservation(
     const Observation* newObs) {
    _assumeValue_(newObs);

    // The we go across the tree
    NodeId currentNodeId = root_;

    while (nodeSonsMap_.exists(currentNodeId)) {
      // On each encountered node, we update the database
      updateNodeWithObservation_(newObs, currentNodeId);

      // The we select the next to go throught
      currentNodeId = nodeSonsMap_[currentNodeId][_branchObs_(newObs, nodeVarMap_[currentNodeId])];
    }

    // On final insertion into the leave we reach
    updateNodeWithObservation_(newObs, currentNodeId);
    leafDatabase_[currentNodeId]->insert(newObs);
  }


  // ============================================================================
  /// @name New Observation insertion methods
  // ============================================================================

  // ############################################################################
  /// If a new modality appears to exists for given variable,
  /// call this method to turn every associated node to this variable into leaf.
  /// Graph has then indeed to be revised
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void IncrementalGraphLearner< AttributeSelection, isScalar >::updateVar(
     const DiscreteVariable* var) {
    Link< NodeId >* nodIter = var2Node_[var]->list();
    Link< NodeId >* nni     = nullptr;
    while (nodIter) {
      nni = nodIter->nextLink();
      convertNode2Leaf_(nodIter->element());
      nodIter = nni;
    }
  }


  // ############################################################################
  /**
   * From the given sets of node, selects randomly one and installs it
   * on given node. Chechks of course if node's current variable is not in that
   * set first.
   * @param nody : the node we update
   * @param bestVar : the set of interessting vars to be installed here
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void IncrementalGraphLearner< AttributeSelection, isScalar >::updateNode_(
     NodeId                          updatedNode,
     Set< const DiscreteVariable* >& varsOfInterest) {
    // If this node has no interesting variable, we turn it into a leaf
    if (varsOfInterest.empty()) {
      convertNode2Leaf_(updatedNode);
      return;
    }

    // If this node has already one of the best variable intalled as test, we
    // move on
    if (nodeVarMap_.exists(updatedNode) && varsOfInterest.exists(nodeVarMap_[updatedNode])) {
      return;
    }

    // In any other case we have to install variable as best test
    Idx randy = randomValue(varsOfInterest.size()), basc = 0;
    SetConstIteratorSafe< const DiscreteVariable* > varIter;
    for (varIter = varsOfInterest.cbeginSafe(), basc = 0;
         varIter != varsOfInterest.cendSafe() && basc < randy;
         ++varIter, basc++)
      ;

    transpose_(updatedNode, *varIter);
  }


  // ############################################################################
  /// Turns the given node into a leaf if not already so
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void IncrementalGraphLearner< AttributeSelection, isScalar >::convertNode2Leaf_(
     NodeId currentNodeId) {
    if (nodeVarMap_[currentNodeId] != value_) {
      leafDatabase_.insert(currentNodeId, new Set< const Observation* >());

      // Resolving potential sons issue
      for (Idx modality = 0; modality < nodeVarMap_[currentNodeId]->domainSize(); ++modality) {
        NodeId sonId = nodeSonsMap_[currentNodeId][modality];
        convertNode2Leaf_(sonId);
        (*leafDatabase_[currentNodeId]) = (*leafDatabase_[currentNodeId]) + *(leafDatabase_[sonId]);
        removeNode_(sonId);
      }

      SOA_DEALLOCATE(nodeSonsMap_[currentNodeId],
                     sizeof(NodeId) * nodeVarMap_[currentNodeId]->domainSize());
      nodeSonsMap_.erase(currentNodeId);

      chgNodeBoundVar_(currentNodeId, value_);
    }
  }


  // ############################################################################
  /// Installs given variable to the given node, ensuring that the variable
  /// is not present in its subtree
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void IncrementalGraphLearner< AttributeSelection, isScalar >::transpose_(
     NodeId                  currentNodeId,
     const DiscreteVariable* desiredVar) {
    // **************************************************************************************
    // Si le noeud courant contient déjà la variable qu'on souhaite lui amener
    // Il n'y a rien à faire
    if (nodeVarMap_[currentNodeId] == desiredVar) { return; }

    // **************************************************************************************
    // Si le noeud courant est terminal,
    // Il faut artificiellement insérer un noeud liant à la variable
    if (nodeVarMap_[currentNodeId] == value_) {
      // We turned this leaf into an internal node.
      // This mean that we'll need to install children leaves for each value of
      // desiredVar

      // First We must prepare these new leaves NodeDatabases and Sets<const
      // Observation*>
      NodeDatabase< AttributeSelection, isScalar >** dbMap
         = static_cast< NodeDatabase< AttributeSelection, isScalar >** >(SOA_ALLOCATE(
            sizeof(NodeDatabase< AttributeSelection, isScalar >*) * desiredVar->domainSize()));
      Set< const Observation* >** obsetMap = static_cast< Set< const Observation* >** >(
         SOA_ALLOCATE(sizeof(Set< const Observation* >*) * desiredVar->domainSize()));
      for (Idx modality = 0; modality < desiredVar->domainSize(); ++modality) {
        dbMap[modality]    = new NodeDatabase< AttributeSelection, isScalar >(&setOfVars_, value_);
        obsetMap[modality] = new Set< const Observation* >();
      }
      for (SetIteratorSafe< const Observation* > obsIter
           = leafDatabase_[currentNodeId]->beginSafe();
           leafDatabase_[currentNodeId]->endSafe() != obsIter;
           ++obsIter) {
        dbMap[_branchObs_(*obsIter, desiredVar)]->addObservation(*obsIter);
        obsetMap[_branchObs_(*obsIter, desiredVar)]->insert(*obsIter);
      }

      // Then we can install each new leaves (and put in place the sonsMap)
      NodeId* sonsMap
         = static_cast< NodeId* >(SOA_ALLOCATE(sizeof(NodeId) * desiredVar->domainSize()));
      for (Idx modality = 0; modality < desiredVar->domainSize(); ++modality)
        sonsMap[modality] = insertLeafNode_(dbMap[modality], value_, obsetMap[modality]);

      // Some necessary clean up
      SOA_DEALLOCATE(dbMap,
                     sizeof(NodeDatabase< AttributeSelection, isScalar >*)
                        * desiredVar->domainSize());
      SOA_DEALLOCATE(obsetMap, sizeof(Set< const Observation* >*) * desiredVar->domainSize());

      // And finally we can turn the node into an internal node associated to
      // desiredVar
      chgNodeBoundVar_(currentNodeId, desiredVar);
      nodeSonsMap_.insert(currentNodeId, sonsMap);

      return;
    }

    // *************************************************************************************
    // Remains the general case where currentNodeId is an internal node.

    // First we ensure that children node use desiredVar as variable
    for (Idx modality = 0; modality < nodeVarMap_[currentNodeId]->domainSize(); ++modality)
      transpose_(nodeSonsMap_[currentNodeId][modality], desiredVar);

    //        Sequence<NodeDatabase<AttributeSelection, isScalar>*>
    //        sonsNodeDatabase =
    //        nodeId2Database_[currentNodeId]->splitOnVar(desiredVar);
    NodeId* sonsMap
       = static_cast< NodeId* >(SOA_ALLOCATE(sizeof(NodeId) * desiredVar->domainSize()));

    // Then we create the new mapping
    for (Idx desiredVarModality = 0; desiredVarModality < desiredVar->domainSize();
         ++desiredVarModality) {
      NodeId* grandSonsMap = static_cast< NodeId* >(
         SOA_ALLOCATE(sizeof(NodeId) * nodeVarMap_[currentNodeId]->domainSize()));
      NodeDatabase< AttributeSelection, isScalar >* sonDB
         = new NodeDatabase< AttributeSelection, isScalar >(&setOfVars_, value_);
      for (Idx currentVarModality = 0;
           currentVarModality < nodeVarMap_[currentNodeId]->domainSize();
           ++currentVarModality) {
        grandSonsMap[currentVarModality]
           = nodeSonsMap_[nodeSonsMap_[currentNodeId][currentVarModality]][desiredVarModality];
        sonDB->operator+=((*nodeId2Database_[grandSonsMap[currentVarModality]]));
      }

      sonsMap[desiredVarModality]
         = insertInternalNode_(sonDB, nodeVarMap_[currentNodeId], grandSonsMap);
    }

    // Finally we clean the old remaining nodes
    for (Idx currentVarModality = 0; currentVarModality < nodeVarMap_[currentNodeId]->domainSize();
         ++currentVarModality) {
      removeNode_(nodeSonsMap_[currentNodeId][currentVarModality]);
    }

    // We suppress the old sons map and remap to the new one
    SOA_DEALLOCATE(nodeSonsMap_[currentNodeId],
                   sizeof(NodeId) * nodeVarMap_[currentNodeId]->domainSize());
    nodeSonsMap_[currentNodeId] = sonsMap;

    chgNodeBoundVar_(currentNodeId, desiredVar);
  }


  // ############################################################################
  /**
   * inserts a new node in internal graph
   * @param nDB : the associated database
   * @param boundVar : the associated variable
   * @return the newly created node's id
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeId IncrementalGraphLearner< AttributeSelection, isScalar >::insertNode_(
     NodeDatabase< AttributeSelection, isScalar >* nDB,
     const DiscreteVariable*                       boundVar) {
    NodeId newNodeId = model_.addNode();
    nodeVarMap_.insert(newNodeId, boundVar);
    nodeId2Database_.insert(newNodeId, nDB);
    var2Node_[boundVar]->addLink(newNodeId);

    needUpdate_ = true;

    return newNodeId;
  }


  // ############################################################################
  /**
   * inserts a new internal node in internal graph
   * @param nDB : the associated database
   * @param boundVar : the associated variable
   * @param sonsMap : a table giving node's sons node
   * @return the newly created node's id
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeId IncrementalGraphLearner< AttributeSelection, isScalar >::insertInternalNode_(
     NodeDatabase< AttributeSelection, isScalar >* nDB,
     const DiscreteVariable*                       boundVar,
     NodeId*                                       sonsMap) {
    NodeId newNodeId = this->insertNode_(nDB, boundVar);
    nodeSonsMap_.insert(newNodeId, sonsMap);
    return newNodeId;
  }


  // ############################################################################
  /**
   * inserts a new leaf node in internal graohs
   * @param nDB : the associated database
   * @param boundVar : the associated variable
   * @param obsSet : the set of observation this leaf retains
   * @return the newly created node's id
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  NodeId IncrementalGraphLearner< AttributeSelection, isScalar >::insertLeafNode_(
     NodeDatabase< AttributeSelection, isScalar >* nDB,
     const DiscreteVariable*                       boundVar,
     Set< const Observation* >*                    obsSet) {
    NodeId newNodeId = this->insertNode_(nDB, boundVar);
    leafDatabase_.insert(newNodeId, obsSet);
    return newNodeId;
  }


  // ############################################################################
  /**
   * Changes the associated variable of a node
   * @param chgedNodeId : the node to change
   * @param desiredVar : its new associated variable
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void IncrementalGraphLearner< AttributeSelection, isScalar >::chgNodeBoundVar_(
     NodeId                  currentNodeId,
     const DiscreteVariable* desiredVar) {
    if (nodeVarMap_[currentNodeId] == desiredVar) return;

    var2Node_[nodeVarMap_[currentNodeId]]->searchAndRemoveLink(currentNodeId);
    var2Node_[desiredVar]->addLink(currentNodeId);
    nodeVarMap_[currentNodeId] = desiredVar;

    if (nodeVarMap_[currentNodeId] != value_ && leafDatabase_.exists(currentNodeId)) {
      delete leafDatabase_[currentNodeId];
      leafDatabase_.erase(currentNodeId);
    }

    if (nodeVarMap_[currentNodeId] == value_ && !leafDatabase_.exists(currentNodeId)) {
      leafDatabase_.insert(currentNodeId, new Set< const Observation* >());
    }

    needUpdate_ = true;
  }


  // ############################################################################
  /**
   * Removes a node from the internal graph
   * @param removedNodeId : the node to remove
   */
  // ############################################################################
  template < TESTNAME AttributeSelection, bool isScalar >
  void IncrementalGraphLearner< AttributeSelection, isScalar >::removeNode_(NodeId currentNodeId) {
    // Retriat de l'id
    model_.eraseNode(currentNodeId);

    // Retrait du vecteur fils
    if (nodeSonsMap_.exists(currentNodeId)) {
      SOA_DEALLOCATE(nodeSonsMap_[currentNodeId],
                     sizeof(NodeId) * nodeVarMap_[currentNodeId]->domainSize());
      nodeSonsMap_.erase(currentNodeId);
    }

    if (leafDatabase_.exists(currentNodeId)) {
      delete leafDatabase_[currentNodeId];
      leafDatabase_.erase(currentNodeId);
    }

    // Retrait de la variable
    var2Node_[nodeVarMap_[currentNodeId]]->searchAndRemoveLink(currentNodeId);
    nodeVarMap_.erase(currentNodeId);

    // Retrait du NodeDatabase
    delete nodeId2Database_[currentNodeId];
    nodeId2Database_.erase(currentNodeId);

    needUpdate_ = true;
  }
}   // namespace gum
