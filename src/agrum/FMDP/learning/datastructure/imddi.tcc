/*********************************************************************************
 * Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES *
 * {prenom.nom}_at_lip6.fr *
 *                                                                               *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or *
 * (at your option) any later version. *
 *                                                                               *
 * This program is distributed in the hope that it will be useful, *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the *
 * GNU General Public License for more details. *
 *                                                                               *
 * You should have received a copy of the GNU General Public License *
 * along with this program; if not, write to the *
 * Free Software Foundation, Inc., *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. *
 *********************************************************************************/
/**
* @file Template Implementations of the IMDDI datastructure learner
* @brief
*
* @author Jean-Christophe Magnan
*/
// =======================================================
#include <cmath>
// =======================================================
#include <agrum/core/priorityQueue.h>
#include <agrum/core/types.h>
// =======================================================
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/datastructure/imddi.h>
// =======================================================
#include <agrum/variables/labelizedVariable.h>
// =======================================================


namespace gum {

  // ############################################################################
  // Constructor & destructor.
  // ############################################################################

  // ============================================================================
  // Variable Learner constructor
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  IMDDI<AttributeSelection, isScalar>::IMDDI(
      MultiDimFunctionGraph<double>* target,
      double                         attributeSelectionThreshold,
      double                         pairSelectionThreshold,
      Set<const DiscreteVariable*>   attributeListe,
      const DiscreteVariable*        learnedValue )
      : IncrementalGraphLearner<AttributeSelection, isScalar>(
            target, attributeListe, learnedValue )
      , __lg( &( this->_model ), pairSelectionThreshold )
      , __nbTotalObservation( 0 )
      , __attributeSelectionThreshold( attributeSelectionThreshold ) {
    GUM_CONSTRUCTOR( IMDDI );
    __addLeaf( this->_root );
  }

  // ============================================================================
  // Reward Learner constructor
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  IMDDI<AttributeSelection, isScalar>::IMDDI(
      MultiDimFunctionGraph<double>* target,
      double                         attributeSelectionThreshold,
      double                         pairSelectionThreshold,
      Set<const DiscreteVariable*>   attributeListe )
      : IncrementalGraphLearner<AttributeSelection, isScalar>(
            target, attributeListe, new LabelizedVariable( "Reward", "", 2 ) )
      , __lg( &( this->_model ), pairSelectionThreshold )
      , __nbTotalObservation( 0 )
      , __attributeSelectionThreshold( attributeSelectionThreshold ) {
    GUM_CONSTRUCTOR( IMDDI );
    __addLeaf( this->_root );
  }

  // ============================================================================
  // Reward Learner constructor
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  IMDDI<AttributeSelection, isScalar>::~IMDDI() {
    GUM_DESTRUCTOR( IMDDI );
    for ( HashTableIteratorSafe<NodeId, AbstractLeaf*> leafIter =
              __leafMap.beginSafe();
          leafIter != __leafMap.endSafe();
          ++leafIter )
      delete leafIter.val();
  }


  // ############################################################################
  // Incrementals methods
  // ############################################################################

  template <TESTNAME AttributeSelection, bool isScalar>
  void
  IMDDI<AttributeSelection, isScalar>::addObservation( const Observation* obs ) {
    __nbTotalObservation++;
    IncrementalGraphLearner<AttributeSelection, isScalar>::addObservation( obs );
  }

  template <TESTNAME AttributeSelection, bool isScalar>
  void IMDDI<AttributeSelection, isScalar>::_updateNodeWithObservation(
      const Observation* newObs, NodeId currentNodeId ) {

    IncrementalGraphLearner<AttributeSelection,
                            isScalar>::_updateNodeWithObservation( newObs,
                                                                   currentNodeId );
    if ( this->_nodeVarMap[currentNodeId] == this->_value )
      __lg.updateLeaf( __leafMap[currentNodeId] );
  }


  // ============================================================================
  // Updates the tree after a new observation has been added
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  void IMDDI<AttributeSelection, isScalar>::updateGraph() {

    __varOrder.clear();

    // First xe initialize the node set which will give us the scores
    Set<NodeId> currentNodeSet;
    currentNodeSet.insert( this->_root );

    // Then we initialize the pool of variables to consider
    VariableSelector vs( this->_setOfVars );
    for ( vs.begin(); vs.hasNext(); vs.next() ) {
      __updateScore( vs.current(), this->_root, vs );
    }

    // Then, until there's no node remaining
    while ( !vs.isEmpty() ) {

      // We select the best var
      const DiscreteVariable* selectedVar = vs.select();
      __varOrder.insert( selectedVar );

      // Then we decide if we update each node according to this var
      __updateNodeSet( currentNodeSet, selectedVar, vs );
    }

    // If there are remaining node that are not leaves after we establish the
    // var order
    // these nodes are turned into leaf.
    for ( SetIteratorSafe<NodeId> nodeIter = currentNodeSet.beginSafe();
          nodeIter != currentNodeSet.endSafe();
          ++nodeIter )
      this->_convertNode2Leaf( *nodeIter );


    if ( __lg.needsUpdate() ) __lg.update();
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
  template <TESTNAME AttributeSelection, bool isScalar>
  void IMDDI<AttributeSelection, isScalar>::__updateScore(
      const DiscreteVariable* var, NodeId nody, VariableSelector& vs ) {
    if ( !this->_nodeId2Database[nody]->isTestRelevant( var ) ) return;
    double weight = (double)this->_nodeId2Database[nody]->nbObservation() /
                    (double)this->__nbTotalObservation;
    vs.updateScore( var,
                    weight * this->_nodeId2Database[nody]->testValue( var ),
                    weight *
                        this->_nodeId2Database[nody]->testOtherCriterion( var ) );
  }

  template <TESTNAME AttributeSelection, bool isScalar>
  void IMDDI<AttributeSelection, isScalar>::__downdateScore(
      const DiscreteVariable* var, NodeId nody, VariableSelector& vs ) {
    if ( !this->_nodeId2Database[nody]->isTestRelevant( var ) ) return;
    double weight = (double)this->_nodeId2Database[nody]->nbObservation() /
                    (double)this->__nbTotalObservation;
    vs.downdateScore(
        var,
        weight * this->_nodeId2Database[nody]->testValue( var ),
        weight * this->_nodeId2Database[nody]->testOtherCriterion( var ) );
  }


  // ============================================================================
  // For each node in the given set, this methods checks whether or not
  // we should installed the given variable as a test.
  // If so, the node is updated
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  void IMDDI<AttributeSelection, isScalar>::__updateNodeSet(
      Set<NodeId>&            nodeSet,
      const DiscreteVariable* selectedVar,
      VariableSelector&       vs ) {

    Set<NodeId> oldNodeSet( nodeSet );
    nodeSet.clear();
    for ( SetIteratorSafe<NodeId> nodeIter = oldNodeSet.beginSafe();
          nodeIter != oldNodeSet.endSafe();
          ++nodeIter ) {

      if ( this->_nodeId2Database[*nodeIter]->isTestRelevant( selectedVar ) &&
           this->_nodeId2Database[*nodeIter]->testValue( selectedVar ) >
               __attributeSelectionThreshold ) {

        this->_transpose( *nodeIter, selectedVar );

        // Then we subtract the from the score given to each variables the
        // quantity given by this node
        for ( vs.begin(); vs.hasNext(); vs.next() ) {
          __downdateScore( vs.current(), *nodeIter, vs );
        }

        // And finally we add all its child to the new set of nodes
        // and updates the remaining var's score
        for ( Idx modality = 0;
              modality < this->_nodeVarMap[*nodeIter]->domainSize();
              ++modality ) {
          NodeId sonId = this->_nodeSonsMap[*nodeIter][modality];
          nodeSet << sonId;

          for ( vs.begin(); vs.hasNext(); vs.next() ) {
            __updateScore( vs.current(), sonId, vs );
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
  template <TESTNAME AttributeSelection, bool isScalar>
  NodeId IMDDI<AttributeSelection, isScalar>::_insertLeafNode(
      NodeDatabase<AttributeSelection, isScalar>* nDB,
      const DiscreteVariable*  boundVar,
      Set<const Observation*>* obsSet ) {

    NodeId currentNodeId =
        IncrementalGraphLearner<AttributeSelection, isScalar>::_insertLeafNode(
            nDB, boundVar, obsSet );

    __addLeaf( currentNodeId );

    return currentNodeId;
  }


  // ============================================================================
  // Changes var associated to a node
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  void IMDDI<AttributeSelection, isScalar>::_chgNodeBoundVar(
      NodeId currentNodeId, const DiscreteVariable* desiredVar ) {

    if ( this->_nodeVarMap[currentNodeId] == this->_value )
      __removeLeaf( currentNodeId );

    IncrementalGraphLearner<AttributeSelection, isScalar>::_chgNodeBoundVar(
        currentNodeId, desiredVar );

    if ( desiredVar == this->_value ) __addLeaf( currentNodeId );
  }


  // ============================================================================
  // Remove node from graph
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  void IMDDI<AttributeSelection, isScalar>::_removeNode( NodeId currentNodeId ) {
    if ( this->_nodeVarMap[currentNodeId] == this->_value )
      __removeLeaf( currentNodeId );
    IncrementalGraphLearner<AttributeSelection, isScalar>::_removeNode(
        currentNodeId );
  }


  // ============================================================================
  // Add leaf to aggregator
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  void IMDDI<AttributeSelection, isScalar>::__addLeaf( NodeId currentNodeId ) {

    __leafMap.insert( currentNodeId,
                      new ConcreteLeaf<AttributeSelection, isScalar>(
                          currentNodeId,
                          this->_nodeId2Database[currentNodeId],
                          &( this->_valueAssumed ) ) );
    __lg.addLeaf( __leafMap[currentNodeId] );
  }


  // ============================================================================
  // Remove leaf from aggregator
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  void IMDDI<AttributeSelection, isScalar>::__removeLeaf( NodeId currentNodeId ) {

    __lg.removeLeaf( __leafMap[currentNodeId] );
    delete __leafMap[currentNodeId];
    __leafMap.erase( currentNodeId );
  }


  // ============================================================================
  // Computes the Reduced and Ordered Function Graph  associated to this ordered
  // tree
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  void IMDDI<AttributeSelection, isScalar>::updateFunctionGraph() {

    //      if( __lg.needsUpdate() || this->_needUpdate ){
    __rebuildFunctionGraph();
    this->_needUpdate = false;
    //      }
  }


  // ============================================================================
  // Performs the leaves merging
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  void IMDDI<AttributeSelection, isScalar>::__rebuildFunctionGraph() {

    // *******************************************************************************************************
    // Mise à jour de l'aggregateur de feuille
    __lg.update();

    // *******************************************************************************************************
    // Reinitialisation du Graphe de Décision
    this->_target->clear();
    for ( auto varIter = __varOrder.beginSafe(); varIter != __varOrder.endSafe();
          ++varIter )
      this->_target->add( **varIter );
    this->_target->add( *this->_value );

    HashTable<NodeId, NodeId> toTarget;

    // *******************************************************************************************************
    // Insertion des feuilles
    HashTable<NodeId, AbstractLeaf*> treeNode2leaf = __lg.leavesMap();
    HashTable<AbstractLeaf*, NodeId> leaf2DGNode;
    for ( HashTableConstIteratorSafe<NodeId, AbstractLeaf*> treeNodeIter =
              treeNode2leaf.cbeginSafe();
          treeNodeIter != treeNode2leaf.cendSafe();
          ++treeNodeIter ) {

      if ( !leaf2DGNode.exists( treeNodeIter.val() ) )
        leaf2DGNode.insert( treeNodeIter.val(),
                            __insertLeafInFunctionGraph( treeNodeIter.val(),
                                                         Int2Type<isScalar>() ) );

      toTarget.insert( treeNodeIter.key(), leaf2DGNode[treeNodeIter.val()] );
    }

    // *******************************************************************************************************
    // Insertion des noeuds internes (avec vérification des possibilités de
    // fusion)
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              __varOrder.rbeginSafe();
          varIter != __varOrder.rendSafe();
          --varIter ) {
      for ( Link<NodeId>* curNodeIter = this->_var2Node[*varIter]->list();
            curNodeIter;
            curNodeIter = curNodeIter->nextLink() ) {
        NodeId* sonsMap = static_cast<NodeId*>(
            SOA_ALLOCATE( sizeof( NodeId ) * ( *varIter )->domainSize() ) );
        for ( Idx modality = 0; modality < ( *varIter )->domainSize(); ++modality )
          sonsMap[modality] =
              toTarget[this->_nodeSonsMap[curNodeIter->element()][modality]];
        toTarget.insert(
            curNodeIter->element(),
            this->_target->manager()->addInternalNode( *varIter, sonsMap ) );
      }
    }

    // *******************************************************************************************************
    // Polish
    this->_target->manager()->setRootNode( toTarget[this->_root] );
    this->_target->manager()->clean();
  }


  // ============================================================================
  // Performs the leaves merging
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  NodeId IMDDI<AttributeSelection, isScalar>::__insertLeafInFunctionGraph(
      AbstractLeaf* leaf, Int2Type<true> ) {


    double value = 0.0;
    for ( Idx moda = 0; moda < leaf->nbModa(); moda++ ) {
      value += (double)leaf->effectif( moda ) * this->_valueAssumed.atPos( moda );
    }
    if ( leaf->total() ) value /= (double)leaf->total();
    return this->_target->manager()->addTerminalNode( value );
  }


  // ============================================================================
  // Performs the leaves merging
  // ============================================================================
  template <TESTNAME AttributeSelection, bool isScalar>
  NodeId IMDDI<AttributeSelection, isScalar>::__insertLeafInFunctionGraph(
      AbstractLeaf* leaf, Int2Type<false> ) {

    NodeId* sonsMap = static_cast<NodeId*>(
        SOA_ALLOCATE( sizeof( NodeId ) * this->_value->domainSize() ) );
    for ( Idx modality = 0; modality < this->_value->domainSize(); ++modality ) {
      double newVal = 0.0;
      if ( leaf->total() )
        newVal = (double)leaf->effectif( modality ) / (double)leaf->total();
      sonsMap[modality] = this->_target->manager()->addTerminalNode( newVal );
    }
    return this->_target->manager()->addInternalNode( this->_value, sonsMap );
  }
}  // end gum namespace
