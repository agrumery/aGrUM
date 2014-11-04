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
    void ITI<AttributeSelection, isScalar>::__updateNodeWithObservation( const Observation* newObs, NodeId currentNodeId ){
       IncrementalGraphLearner<AttributeSelection, isScalar>::_updateNodeWithObservation( newObs, currentNodeId );
       __staleTable[currentNodeId] = true;
    }


    // ============================================================================
    // Updates the tree after a new observation has been added
    // ============================================================================
    template <TESTNAME AttributeSelection, bool isScalar >
    void ITI<AttributeSelection, isScalar>::updateGraph(){

    }



    // ============================================================================
    // Insert a new node with given associated database, var and maybe sons
    // ============================================================================
    template < TESTNAME AttributeSelection, bool isScalar >
    void ITI<AttributeSelection, isScalar>::_insertNode( NodeDatabase<AttributeSelection, isScalar>* nDB, const DiscreteVariable* boundVar, NodeId* sonsMap ){
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
    void IncrementalGraphLearner<AttributeSelection, isScalar>::_removeNode( NodeId currentNodeId ){
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

     __target->clear();
     for( auto varIter = __varOrder.beginSafe(); varIter != __varOrder.endSafe(); ++varIter )
       __target->add(**varIter);
     __target->add(*__value);

     HashTable<NodeId, NodeId> toTarget;
     __mergeLeaves(toTarget, false);


     for( auto nodeIter = __var2Node[__value]->cbeginSafe(); nodeIter != __var2Node[__value]->cendSafe(); ++nodeIter ){
       GUM_SCALAR* probDist = __nodeId2Database[*nodeIter]->probDist();
       NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*__value->domainSize()) );
       for(Idx modality = 0; modality < __value->domainSize(); ++modality )
         sonsMap[modality] = __target->manager()->addTerminalNode( probDist[modality] );
       toTarget.insert(*nodeIter, __target->manager()->nodeRedundancyCheck( __value, sonsMap ) );
       MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( probDist, sizeof(GUM_SCALAR)*__value->domainSize());
     }

     for( auto varIter = __varOrder.rbeginSafe(); varIter != __varOrder.rendSafe(); --varIter ) {

       for( auto nodeIter = __var2Node[*varIter]->cbeginSafe(); nodeIter != __var2Node[*varIter]->cendSafe(); ++nodeIter ){
         NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*(*varIter)->domainSize()) );
         for(Idx modality = 0; modality < (*varIter)->domainSize(); ++modality ){

           sonsMap[modality] = toTarget[__nodeSonsMap[*nodeIter][modality]];
         }
         toTarget.insert(*nodeIter, __target->manager()->nodeRedundancyCheck( *varIter, sonsMap ) );
       }

     }
     __target->manager()->setRootNode( toTarget[__root] );
     __target->manager()->clean();
  }
} // end gum namespace
