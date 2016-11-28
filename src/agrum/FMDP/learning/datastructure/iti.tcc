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
* @file Template Implementations of the ITI datastructure learner
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
#include <agrum/FMDP/learning/datastructure/iti.h>
// =======================================================
#include <agrum/variables/labelizedVariable.h>
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
  template <TESTNAME AttributeSelection, bool isScalar>
  ITI<AttributeSelection, isScalar>::ITI(
      MultiDimFunctionGraph<double>* target,
      double                         attributeSelectionThreshold,
      Set<const DiscreteVariable*>   attributeListe,
      const DiscreteVariable*        learnedValue )
      : IncrementalGraphLearner<AttributeSelection, isScalar>(
            target, attributeListe, learnedValue )
      , __nbTotalObservation( 0 )
      , __attributeSelectionThreshold( attributeSelectionThreshold ) {
    GUM_CONSTRUCTOR( ITI );
    __staleTable.insert( this->_root, false );
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
  template <TESTNAME AttributeSelection, bool isScalar>
  ITI<AttributeSelection, isScalar>::ITI(
      MultiDimFunctionGraph<double>* target,
      double                         attributeSelectionThreshold,
      Set<const DiscreteVariable*>   attributeListe )
      : IncrementalGraphLearner<AttributeSelection, isScalar>(
            target, attributeListe, new LabelizedVariable( "Reward", "", 2 ) )
      , __nbTotalObservation( 0 )
      , __attributeSelectionThreshold( attributeSelectionThreshold ) {
    GUM_CONSTRUCTOR( ITI );
    __staleTable.insert( this->_root, false );
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
  template <TESTNAME AttributeSelection, bool isScalar>
  void
  ITI<AttributeSelection, isScalar>::addObservation( const Observation* obs ) {
    __nbTotalObservation++;
    IncrementalGraphLearner<AttributeSelection, isScalar>::addObservation( obs );
  }

  // ############################################################################
  /**
   * Will update internal graph's NodeDatabase of given node with the new
   * observation
   * @param newObs
   * @param currentNodeId
   */
  // ############################################################################
  template <TESTNAME AttributeSelection, bool isScalar>
  void ITI<AttributeSelection, isScalar>::_updateNodeWithObservation(
      const Observation* newObs, NodeId currentNodeId ) {
    IncrementalGraphLearner<AttributeSelection,
                            isScalar>::_updateNodeWithObservation( newObs,
                                                                   currentNodeId );
    __staleTable[currentNodeId] = true;
  }


  // ============================================================================
  /// @name Graph Structure update methods
  // ============================================================================

  // ############################################################################
  /// Updates the internal graph after a new observation has been added
  // ############################################################################
  template <TESTNAME AttributeSelection, bool isScalar>
  void ITI<AttributeSelection, isScalar>::updateGraph() {

    std::vector<NodeId> filo;
    filo.push_back( this->_root );
    HashTable<NodeId, Set<const DiscreteVariable*>*> potentialVars;
    potentialVars.insert( this->_root,
                          new Set<const DiscreteVariable*>( this->_setOfVars ) );


    while ( !filo.empty() ) {

      NodeId currentNodeId = filo.back();
      filo.pop_back();

      // First we look for the best var to install on the node
      double                       bestValue = __attributeSelectionThreshold;
      Set<const DiscreteVariable*> bestVars;

      for ( auto varIter = potentialVars[currentNodeId]->cbeginSafe();
            varIter != potentialVars[currentNodeId]->cendSafe();
            ++varIter )
        if ( this->_nodeId2Database[currentNodeId]->isTestRelevant( *varIter ) ) {
          double varValue =
              this->_nodeId2Database[currentNodeId]->testValue( *varIter );
          if ( varValue >= bestValue ) {
            if ( varValue > bestValue ) {
              bestValue = varValue;
              bestVars.clear();
            }
            bestVars.insert( *varIter );
          }
        }

      // Then We installed Variable a test on that node
      this->_updateNode( currentNodeId, bestVars );

      // The we move on the children if needed
      if ( this->_nodeVarMap[currentNodeId] != this->_value ) {
        for ( Idx moda = 0; moda < this->_nodeVarMap[currentNodeId]->domainSize();
              moda++ ) {
          Set<const DiscreteVariable*>* itsPotentialVars =
              new Set<const DiscreteVariable*>( *potentialVars[currentNodeId] );
          itsPotentialVars->erase( this->_nodeVarMap[currentNodeId] );
          NodeId sonId = this->_nodeSonsMap[currentNodeId][moda];
          if ( __staleTable[sonId] ) {
            filo.push_back( sonId );
            potentialVars.insert( sonId, itsPotentialVars );
          }
        }
      }
    }

    for ( HashTableIteratorSafe<NodeId, Set<const DiscreteVariable*>*> nodeIter =
              potentialVars.beginSafe();
          nodeIter != potentialVars.endSafe();
          ++nodeIter )
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
  template <TESTNAME AttributeSelection, bool isScalar>
  NodeId ITI<AttributeSelection, isScalar>::_insertNode(
      NodeDatabase<AttributeSelection, isScalar>* nDB,
      const DiscreteVariable* boundVar ) {
    NodeId n = IncrementalGraphLearner<AttributeSelection, isScalar>::_insertNode(
        nDB, boundVar );
    __staleTable.insert( n, true );
    return n;
  }


  // ############################################################################
  /**
   * Changes the associated variable of a node
   * @param chgedNodeId : the node to change
   * @param desiredVar : its new associated variable
   */
  // ############################################################################
  template <TESTNAME AttributeSelection, bool isScalar>
  void ITI<AttributeSelection, isScalar>::_chgNodeBoundVar(
      NodeId currentNodeId, const DiscreteVariable* desiredVar ) {
    if ( this->_nodeVarMap[currentNodeId] != desiredVar ) {
      __staleTable[currentNodeId] = true;
      IncrementalGraphLearner<AttributeSelection, isScalar>::_chgNodeBoundVar(
          currentNodeId, desiredVar );
    }
  }


  // ############################################################################
  /**
   * Removes a node from the internal graph
   * @param removedNodeId : the node to remove
   */
  // ############################################################################
  template <TESTNAME AttributeSelection, bool isScalar>
  void ITI<AttributeSelection, isScalar>::_removeNode( NodeId currentNodeId ) {
    IncrementalGraphLearner<AttributeSelection, isScalar>::_removeNode(
        currentNodeId );
    __staleTable.erase( currentNodeId );
  }


  // ============================================================================
  /// @name Function Graph Updating methods
  // ============================================================================

  // ############################################################################
  /// Updates target to currently learned graph structure
  // ############################################################################
  template <TESTNAME AttributeSelection, bool isScalar>
  void ITI<AttributeSelection, isScalar>::updateFunctionGraph() {

    this->_target->clear();
    this->_target->manager()->setRootNode(
        this->__insertNodeInFunctionGraph( this->_root ) );
  }


  // ############################################################################
  /**
   * Inserts an internal node in the target
   * @param the source node in internal graph
   * @return the mathcing node id in the target
   */
  // ############################################################################
  template <TESTNAME AttributeSelection, bool isScalar>
  NodeId ITI<AttributeSelection, isScalar>::__insertNodeInFunctionGraph(
      NodeId currentNodeId ) {

    if ( this->_nodeVarMap[currentNodeId] == this->_value ) {
      NodeId nody = __insertTerminalNode( currentNodeId );
      return nody;
    }

    if ( !this->_target->variablesSequence().exists(
             this->_nodeVarMap[currentNodeId] ) ) {
      this->_target->add( *( this->_nodeVarMap[currentNodeId] ) );
    }

    NodeId nody = this->_target->manager()->addInternalNode(
        this->_nodeVarMap[currentNodeId] );
    for ( Idx moda = 0; moda < this->_nodeVarMap[currentNodeId]->domainSize();
          ++moda ) {
      NodeId son = this->__insertNodeInFunctionGraph(
          this->_nodeSonsMap[currentNodeId][moda] );
      this->_target->manager()->setSon( nody, moda, son );
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
  template <TESTNAME AttributeSelection, bool isScalar>
  NodeId
  ITI<AttributeSelection, isScalar>::__insertTerminalNode( NodeId currentNodeId,
                                                           Int2Type<false> ) {


    if ( !this->_target->variablesSequence().exists( this->_value ) )
      this->_target->add( *( this->_value ) );

    double tot = this->_nodeId2Database[currentNodeId]->nbObservation();
    if ( !tot ) return this->_target->manager()->addTerminalNode( 0.0 );

    NodeId* sonsMap = static_cast<NodeId*>(
        SOA_ALLOCATE( sizeof( NodeId ) * this->_value->domainSize() ) );
    for ( Idx modality = 0; modality < this->_value->domainSize(); ++modality ) {
      double newVal = 0.0;
      newVal =
          (double)this->_nodeId2Database[currentNodeId]->effectif( modality ) /
          (double)tot;
      sonsMap[modality] = this->_target->manager()->addTerminalNode( newVal );
    }
    NodeId nody =
        this->_target->manager()->addInternalNode( this->_value, sonsMap );
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
  template <TESTNAME AttributeSelection, bool isScalar>
  NodeId
  ITI<AttributeSelection, isScalar>::__insertTerminalNode( NodeId currentNodeId,
                                                           Int2Type<true> ) {
    double value = 0.0;
    for ( auto valIter = this->_nodeId2Database[currentNodeId]->cbeginValues();
          valIter != this->_nodeId2Database[currentNodeId]->cendValues();
          ++valIter ) {
      value += (double)valIter.key() * valIter.val();
    }
    if ( this->_nodeId2Database[currentNodeId]->nbObservation() )
      value /= (double)this->_nodeId2Database[currentNodeId]->nbObservation();
    NodeId nody = this->_target->manager()->addTerminalNode( value );
    return nody;
  }
}  // end gum namespace
