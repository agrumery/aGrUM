/*********************************************************************************
 * Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES           *
 * {prenom.nom}_at_lip6.fr                                                       *
 *                                                                               *
 * This program is free software; you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by          *
 * the Free Software Foundation; either version 2 of the License, or             *
 * (at your option) any later version.                                           *
 *                                                                               *
 * This program is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  *
 * GNU General Public License for more details.                                  *
 *                                                                               *
 * You should have received a copy of the GNU General Public License             *
 * along with this program; if not, write to the                                 *
 * Free Software Foundation, Inc.,                                               *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                      *
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
#include <agrum/core/types.h>
#include <agrum/core/priorityQueue.h>
// =======================================================
#include <agrum/FMDP/learning/datastructure/iti.h>
#include <agrum/FMDP/learning/core/chiSquare.h>
// =======================================================
#include <agrum/variables/discreteVariable.h>
// =======================================================

#define ALLOCATE(x) SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x,y) SmallObjectAllocator::instance().deallocate(x,y)

namespace gum {

  // ==========================================================================
  /// @name Constructor & destructor.
  // ==========================================================================
  /// @{

    // ###################################################################
    /// Variable Learner constructor
    // ###################################################################
  template <TESTNAME AttributeSelection, bool isScalar >
    ITI<AttributeSelection, isScalar>::ITI ( MultiDimDecisionGraph<double>* target,
            double attributeSelectionThreshold,
            Set<const DiscreteVariable*> attributeListe,
            const DiscreteVariable* learnedValue ) : IncrementalGraphLearner( target, attributeListe, learnedValue),
                                                     __nbTotalObservation(0),
                                                     __attributeSelectionThreshold(attributeSelectionThreshold)
    {GUM_CONSTRUCTOR(ITI);}

    // ###################################################################
    /// Reward Learner constructor
    // ###################################################################
    template <TESTNAME AttributeSelection, bool isScalar >
    ITI<AttributeSelection, isScalar>::ITI ( MultiDimDecisionGraph<double>* target,
            double attributeSelectionThreshold,
            Set<const DiscreteVariable*> attributeListe ) : IncrementalGraphLearner( target, attributeListe ),
                                                            __nbTotalObservation(0),
                                                            __attributeSelectionThreshold(attributeSelectionThreshold)
    {GUM_CONSTRUCTOR(ITI);}



  // ############################################################################
  // Incrementals methods
  // ############################################################################
    template <TESTNAME AttributeSelection, bool isScalar >
    void ITI<AttributeSelection, isScalar>::addObservation ( const Observation* obs ){
      __nbTotalObservation++;
      IncrementalGraphLearner<AttributeSelection, isScalar>::addObservation(obs);
    }

    template <TESTNAME AttributeSelection, bool isScalar >
    void ITI<AttributeSelection, isScalar>::_updateNodeWithObservation( const Observation* newObs, NodeId currentNodeId ){
       IncrementalGraphLearner<AttributeSelection, isScalar>::_updateNodeWithObservation( newObs, currentNodeId );
       __staleTable[currentNodeId] = true;
    }


    // ============================================================================
    // Updates the tree after a new observation has been added
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    void ITI<AttributeSelection, isScalar>::updateGraph(){

      std::vector<NodeId> filo;
      filo.push_back( _root );
      HashTable<NodeId, Set<const DiscreteVariable*>* > potentialVars;
      potentialVars.insert( _root, new Set<const DiscreteVariable*>(_setOfVars));


      while( !filo.empty() ){

        NodeId currentNodeId = filo.back();
        filo.pop_back();

        // First we look for the best var to install on the node
        double bestValue = __attributeSelectionThreshold;
        Set<const DiscreteVariable*> bestVars;

        for( auto varIter = potentialVars[currentNodeId]->cbeginSafe(); varIter != potentialVars[currentNodeId]->cendSafe(); ++varIter )
          if( _nodeId2Database[currentNodeId]->isTestRelevant() ){
            double varValue = _nodeId2Database[currentNodeId]->testValue();
            if( varValue >= bestValue ) {
              if( varValue > bestValue ){
                bestValue = varValue;
                bestVars.clear();
              }
              bestVars.insert( *varIter );
            }
          }

        // Then We installed Variable a test on that node
        _updateNode( currentNodeId, bestVars );

        // The we move on the children if needed
        if( _nodeVarMap.exists(currentNodeId)) {
          Set<const DiscreteVariable* >* itsPotentialVars = new Set<const DiscreteVariable*>(potentialVars[currentNodeId]);
          itsPotentialVars >> _nodeVarMap[currentNodeId];
          for(Idx moda = 0; moda < _nodeVarMap[currentNodeId]->domainSize(); moda++ )
            if( __staleTable[_nodeSonsMap[currentNodeId][moda]] ) {
                filo.push_back( _nodeSonsMap[currentNodeId][moda] );
                potentialVars.insert( _nodeSonsMap[currentNodeId][moda], itsPotentialVars);
            }
        }
      }
      for( auto nodeIter = potentialVars.beginSafe(); nodeIter != potentialVars.endSafe(); ++nodeIter )
        if( nodeIter.val() )
          delete nodeIter.val();
    }



    // ============================================================================
    // Insert a new node with given associated database, var and maybe sons
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    NodeId ITI<AttributeSelection, isScalar>::_insertNode( NodeDatabase<AttributeSelection, isScalar>* nDB, const DiscreteVariable* boundVar, NodeId* sonsMap ){
      NodeId n = IncrementalGraphLearner<AttributeSelection, isScalar>::_insertNode(nDB, boundVar, sonsMap);
      __staleTable.insert(n, true);      
      return n;
    }



    // ============================================================================
    // Changes var associated to a node
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void ITI<AttributeSelection, isScalar>::_chgNodeBoundVar( NodeId currentNodeId, const DiscreteVariable* desiredVar ){
      IncrementalGraphLearner<AttributeSelection, isScalar>::_chgNodeBoundVar( currentNodeId, desiredVar );
      __staleTable[currentNodeId] = true;
    }



    // ============================================================================
    // Remove node from graph
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void ITI<AttributeSelection, isScalar>::_removeNode( NodeId currentNodeId ){
      IncrementalGraphLearner<AttributeSelection, isScalar>::_removeNode( currentNodeId );
      __staleTable.erase(currentNodeId);
    }



  // ############################################################################
  // Updating methods
  // ############################################################################


    // ============================================================================
    // Computes the Reduced and Ordered Decision Graph  associated to this ordered tree
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    void ITI<AttributeSelection, isScalar>::updateDecisionGraph(){

      _target->clear();

      __insertNodeInDecisionGraph( _root );

      if( !_nodeVarMap.exists(_root))
        return;

      std::vector<NodeId> filo;
      filo.push_back( _root );

      while(!filo.empty()){

        NodeId currentNode = filo.back();
        filo.pop_back();

        for(Idx moda = 0; moda < _nodeVarMap[currentNode]->domainSize(); moda++ ){
          toTarget.insert( _nodeSonsMap[currentNode][moda], __insertNodeInDecisionGraph( _nodeSonsMap[currentNode][moda]) );
          _target->manager()->setSon(currentNode, moda, _nodeSonsMap[currentNode][moda] );
          if( _nodeVarMap.exists(_nodeSonsMap[currentNode][moda]))
            filo.push_back(_nodeSonsMap[currentNode][moda]);
        }
      }


    }


    // ============================================================================
    // Computes the Reduced and Ordered Decision Graph  associated to this ordered tree
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    NodeId ITI<AttributeSelection, isScalar>::__insertNodeInDecisionGraph( NodeId currentNodeId ){

      if( !_nodeVarMap.exists(currentNodeId))
        return __insertTerminalNode(currentNodeId);
      if( !_target->variablesSequence().exists(_nodeVarMap[currentNodeId]))
        _target->add(*(_nodeVarMap[currentNodeId]));
      return _target->manager()->addNonTerminalNode( _nodeVarMap[currentNodeId] );
    }


    // ============================================================================
    // Computes the Reduced and Ordered Decision Graph  associated to this ordered tree
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    NodeId ITI<AttributeSelection, isScalar>::__insertTerminalNode( NodeId currentNodeId ){
      insertTerminalNode(currentNodeId, Int2Type<isScalar>());
    }


    // ============================================================================
    // Computes the Reduced and Ordered Decision Graph  associated to this ordered tree
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    NodeId ITI<AttributeSelection, isScalar>::__insertTerminalNode( NodeId currentNodeId, Int2Type<false> ){

      double* probDist = _nodeId2Database[currentNodeId]->effectif();
      double tot = _nodeId2Database[currentNodeId]->nbObservation();
      NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*__value->domainSize()) );
      for(Idx modality = 0; modality < _value->domainSize(); ++modality )
        sonsMap[modality] = _target->manager()->addTerminalNode( probDist[modality]/tot );
      MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( probDist, sizeof(GUM_SCALAR)*_value->domainSize());
      return _target->manager()->addNonTerminalNode( _value, sonsMap );
    }


    // ============================================================================
    // Computes the Reduced and Ordered Decision Graph  associated to this ordered tree
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    NodeId ITI<AttributeSelection, isScalar>::__insertTerminalNode( NodeId currentNodeId, Int2Type<true> ){
      return _target->manager()->addTerminalNode( _nodeId2Database[currentNodeId]->reward());
    }
} // end gum namespace
