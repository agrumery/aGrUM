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
* @file
* @brief
*
* @author Jean-Christophe Magnan
*/
// =======================================================
#include <cmath>
// =======================================================
#include <agrum/core/types.h>
#include <agrum/core/multiPriorityQueue.h>
// =======================================================
#include <agrum/FMDP/learning/datastructure/incrementalGraphLearner.h>
#include <agrum/FMDP/learning/core/chiSquare.h>
// =======================================================
#include <agrum/variables/discreteVariable.h>
// =======================================================

#define ALLOCATE(x) SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x,y) SmallObjectAllocator::instance().deallocate(x,y)

namespace gum {

  // ############################################################################
  // Constructors / Destructors
  // ############################################################################

    // ============================================================================
    // Default constructor.
    // ============================================================================
    template < bool isScalar >
    IncrementalGraphLearner<isScalar>::IncrementalGraphLearner( MultiDimDecisionGraph<double>* target,
                                                                double dependenceThreshold,
                                                                double similarityThreshold,
                                                                Set<const DiscreteVariable*> varList,
                                                                const DiscreteVariable* value,
                                                                bool rewardMode ) : __target(target),
                                                                                    __setOfVars(varList),
                                                                                    __value(value),
                                                                                    __nbTotalObservation(0),
                                                                                    __dependenceThreshold(dependenceThreshold),
                                                                                    __similarityThreshold(similarityThreshold),
                                                                                    __isReward(rewardMode)
    {
      GUM_CONSTRUCTOR(IncrementalGraphLearner);

      __model.addNode();
      __root = __model.addNode();
      __nodeId2Database.insert( __root, new NodeDatabase(&__setOfVars, __value));
      __nodeVarMap.insert( __root, __value );

      for( auto varIter = __setOfVars.cbeginSafe(); varIter != __setOfVars.cendSafe(); ++varIter )
          __var2Node.insert( *varIter, new List<NodeId>());

      __var2Node.insert( __value, new List<NodeId>());
      __var2Node[__value]->insert(__root);

    }


    // ============================================================================
    // Default destructor.
    // ============================================================================
    template < bool isScalar >
    IncrementalGraphLearner::~IncrementalGraphLearner(){

      for( auto nodeIter = __nodeId2Database.beginSafe(); nodeIter != __nodeId2Database.endSafe(); ++nodeIter )
        delete nodeIter.val();

      for( auto nodeIter = __nodeSonsMap.beginSafe(); nodeIter != __nodeSonsMap.endSafe(); ++nodeIter )
        DEALLOCATE( nodeIter.val(), sizeof(NodeId)*__nodeVarMap[nodeIter.key()]->domainSize());

      for( auto varIter = __var2Node.beginSafe(); varIter != __var2Node.endSafe(); ++varIter )
          delete varIter.val();

      GUM_DESTRUCTOR(IncrementalGraphLearner);
    }



  // ############################################################################
  // Incrementals methods
  // ############################################################################

    // ============================================================================
    // Adds a new observation to the structure
    // ============================================================================
    template < bool isScalar >
    void IncrementalGraphLearner::addObservation(const Observation *newObs ){

      // First we increase the total number of observation added
      __nbTotalObservation++;

      // The we go across the tree
      NodeId currentNodeId = __root;

      while( __nodeSonsMap.exists(currentNodeId) ){

        // On each encountered node, we update the database
        __nodeId2Database[currentNodeId]->addObservation( newObs );

        // The we select the next to go throught
        currentNodeId = __nodeSonsMap[currentNodeId][newObs->modality( __nodeVarMap[currentNodeId] )];
      }

      // On final insertion into the leave we reach
      __nodeId2Database[currentNodeId]->addObservation( newObs );

    }


    // ============================================================================
    // Bring the desiredVar to the given node
    // ============================================================================
    template < bool isScalar >
    void IncrementalGraphLearner::_transpose( NodeId currentNodeId,
                                              const DiscreteVariable* desiredVar ){

      // **************************************************************************************
      // Si le noeud courant contient déjà la variable qu'on souhaite lui amener
      // Il n'y a rien à faire
      if( __nodeVarMap[currentNodeId] == desiredVar )
        return;

      // **************************************************************************************
      // Si le noeud courant est terminal,
      // Il faut artificiellement insérer un noeud liant à la variable
      if( __nodeVarMap[currentNodeId] == __value ){

        Sequence<NodeDatabase*> sonsNodeDatabase = __nodeId2Database[currentNodeId]->splitOnVar(desiredVar);
        NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*desiredVar->domainSize()) );

        for( Idx modality = 0; modality < desiredVar->domainSize(); ++modality ){
          NodeId newNodeId = __model.addNode();
          __nodeVarMap.insert(newNodeId, __value);
          __nodeId2Database.insert(newNodeId, sonsNodeDatabase.atPos(modality));
          __var2Node[__value]->insert(newNodeId);
          sonsMap[modality] = newNodeId;
        }

        __var2Node[desiredVar]->insert(currentNodeId);
        __var2Node[__value]->eraseByVal(currentNodeId);

        __nodeVarMap[currentNodeId] = desiredVar;
        __nodeSonsMap.insert( currentNodeId, sonsMap);

        return;
      }

      // *************************************************************************************
      // Remains the general case where currentNodeId is an internal node.

          // First we ensure that children node use desiredVar as variable
          for(Idx modality = 0; modality < __nodeVarMap[currentNodeId]->domainSize(); ++modality )
            _transpose( __nodeSonsMap[currentNodeId][modality], desiredVar );

          Sequence<NodeDatabase*> sonsNodeDatabase = __nodeId2Database[currentNodeId]->splitOnVar(desiredVar);
          NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*desiredVar->domainSize()) );

          // Then we create the new mapping
          for(Idx desiredVarModality = 0; desiredVarModality < desiredVar->domainSize(); ++desiredVarModality){
            NodeId* grandSonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*__nodeVarMap[currentNodeId]->domainSize()) );
            for(Idx currentVarModality = 0; currentVarModality < __nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality )
              grandSonsMap[currentVarModality] = __nodeSonsMap[__nodeSonsMap[currentNodeId][currentVarModality]][desiredVarModality];

            NodeId newNodeId = __model.addNode();
            __nodeVarMap.insert(newNodeId, __nodeVarMap[currentNodeId]);
            __nodeSonsMap.insert(newNodeId, grandSonsMap);
            __var2Node[__nodeVarMap[currentNodeId]]->insert(newNodeId);
            __nodeId2Database.insert(newNodeId, sonsNodeDatabase.atPos(desiredVarModality));
            sonsMap[desiredVarModality] = newNodeId;
          }

          // Finally we clean the old remaining nodes
          for(Idx currentVarModality = 0; currentVarModality < __nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality ){
            NodeId sonId = __nodeSonsMap[currentNodeId][currentVarModality];

            // Retriat de l'id
            __model.eraseNode( sonId );

            // Retrait de la variable
            __var2Node[__nodeVarMap[sonId]]->eraseByVal(sonId);
            __nodeVarMap.erase( sonId );

            // Retrait du NodeDatabase
            delete __nodeId2Database[sonId];
            __nodeId2Database.erase(sonId);

            // Retrait du vecteur fils
            DEALLOCATE( __nodeSonsMap[sonId], sizeof(NodeId)*desiredVar->domainSize() );
            __nodeSonsMap.erase(sonId);
          }

          // We suppress the old sons map and remap to the new one
          DEALLOCATE( __nodeSonsMap[currentNodeId], sizeof(NodeId)*__nodeVarMap[currentNodeId]->domainSize() );
          __nodeSonsMap[currentNodeId] = sonsMap;

          __var2Node[__nodeVarMap[currentNodeId]]->eraseByVal(currentNodeId);
          __var2Node[desiredVar]->insert(currentNodeId);

          __nodeVarMap[currentNodeId] = desiredVar;

    }



    // ============================================================================
    // Turn given node into a leaf, recurssively destroying its children
    // ============================================================================
    template < bool isScalar >
    void IncrementalGraphLearner::_convertNode2Leaf(NodeId currentNodeId){

      if(__nodeVarMap[currentNodeId] != __value ){

        //
        for(Idx modality = 0; modality < __nodeVarMap[currentNodeId]->domainSize(); ++modality) {
          NodeId sonId = __nodeSonsMap[currentNodeId][modality];

          _convertNode2Leaf(sonId);

          // Retriat de l'id
          __model.eraseNode( sonId );

          // Retrait du NodeDatabase
          delete __nodeId2Database[sonId];
          __nodeId2Database.erase(sonId);

          // Retrait de la variable
          __nodeVarMap.erase( sonId );

          // Retrait du fils de la liste des noeuds terminaux
          __var2Node[__value]->eraseByVal(sonId);
        }


        DEALLOCATE( __nodeSonsMap[currentNodeId], sizeof(NodeId)*__nodeVarMap[currentNodeId]->domainSize() );
        __nodeSonsMap.erase(currentNodeId);

        __var2Node[__nodeVarMap[currentNodeId]]->eraseByVal(currentNodeId);
        __var2Node[__value]->insert(currentNodeId);

        __nodeVarMap[currentNodeId] = __value;
      }
    }


    // ============================================================================
    // Computes the Reduced and Ordered Decision Graph  associated to this ordered tree
    // ============================================================================
    template < bool isScalar >
    void IncrementalGraphLearner::toDG(){

       __target->clear();
       for( auto varIter = __varOrder.beginSafe(); varIter != __varOrder.endSafe(); ++varIter )
         __target->add(**varIter);
       __target->add(*__value);

       HashTable<NodeId, NodeId> toTarget;
       __mergeLeaves(toTarget, false);


//       for( auto nodeIter = __var2Node[__value]->cbeginSafe(); nodeIter != __var2Node[__value]->cendSafe(); ++nodeIter ){
//         GUM_SCALAR* probDist = __nodeId2Database[*nodeIter]->probDist();
//         NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*__value->domainSize()) );
//         for(Idx modality = 0; modality < __value->domainSize(); ++modality )
//           sonsMap[modality] = __target->manager()->addTerminalNode( probDist[modality] );
//         toTarget.insert(*nodeIter, __target->manager()->nodeRedundancyCheck( __value, sonsMap ) );
//         MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( probDist, sizeof(GUM_SCALAR)*__value->domainSize());
//       }

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


    void
    IncrementalGraphLearner::__showMap( ){

        for( auto varIter = __nodeVarMap.beginSafe(); varIter != __nodeVarMap.endSafe(); ++varIter )
            std::cout << varIter.key() << " - " << varIter.val()->name() << " | ";
        std::cout << std::endl;

        for( auto nodeIter = __nodeSonsMap.beginSafe(); nodeIter != __nodeSonsMap.endSafe(); ++nodeIter ) {
            std::cout << nodeIter.key() << " : ";
            for( Idx i = 0; i < __nodeVarMap[nodeIter.key()]->domainSize(); ++i )
              std::cout << i << " -> " << __nodeSonsMap[nodeIter.key()][i] << " . ";
            std::cout << " | " << std::endl;
        }


        for(auto varLiter = __var2Node.beginSafe(); varLiter != __var2Node.endSafe(); ++varLiter ){
         std::cout << "Var " << varLiter.key()->name();
         List<NodeId>* mojo = varLiter.val();
         std::cout << " - Taille : " << mojo->size() << " - Liste : ";
         for( ListIteratorSafe<NodeId> nodeIter = mojo->beginSafe(); nodeIter != mojo->endSafe(); ++nodeIter)
             std::cout << *nodeIter << " | ";
         std::cout <<  std::endl;
        }

        std::cout << __target->toDot() << std::endl << "     " << std::endl;
    }
} // end gum namespace
