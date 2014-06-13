/*********************************************************************************
 *  Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES          *
 *  {prenom.nom}_at_lip6.fr                                                      *
 *                                                                               *
 *  This program is free software; you can redistribute it and/or modify         *
 *  it under the terms of the GNU General Public LiceDG2NodeIde as published by  *
 *  the Free Software Foundation; either version 2 of the LiceDG2NodeIde, or     *
 *  (at your option) any later version.                                          *
 *                                                                               *
 *  This program is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
 *  GNU General Public LiceDG2NodeIde for more details.                          *
 *                                                                               *
 *  You should have received a copy of the GNU General Public LiceDG2NodeIde     *
 *  along with this program; if not, write to the                                *
 *  Free Software Foundation, Inc.,                                              *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                     *
 *********************************************************************************/
/**
* @file
* @brief
*
* @author Jean-Christophe Magnan
*/

// =======================================================
#include <agrum/core/types.h>
#include <agrum/core/multiPriorityQueue.h>
// =======================================================
#include <agrum/FMDP/learning/decision graph/IMDDI.h>
// =======================================================
#include <agrum/variables/discreteVariable.h>
// =======================================================

namespace gum {

    // ############################################################################
    // Constructors / Destructors
    // ############################################################################

        // ============================================================================
        // Default constructor.
        // ============================================================================
        template< typename GUM_SCALAR>
        IMDDI<GUM_SCALAR>::IMDDI( MultiDimDecisionGraph<GUM_SCALAR>* target,
                                  double dependenceThreshold,
                                  Set<const DiscreteVariable*> varList,
                                  const DiscreteVariable* value ): __target(target),
                                                                   __nbTotalObservation(0),
                                                                   __dependenceThreshold(dependenceThreshold),
                                                                   __setOfVars(varList),
                                                                   __value(value)
        {
            GUM_CONSTRUCTOR(IMDDI);

            __model.insertNode();
            __root = __model.insertNode();
            __nodeId2Database.insert( __root, new NodeDatabase<GUM_SCALAR>(&__setOfVars, __value));
            __nodeVarMap.insert( __root, __value );

        }


        // ============================================================================
        // Default destructor.
        // ============================================================================
        template <typename GUM_SCALAR>
        IMDDI<GUM_SCALAR>::~IMDDI(){

            for(HashTableIteratorSafe<NodeId, NodeDatabase<GUM_SCALAR>*> nodeIter = __nodeId2Database.beginSafe();
                    nodeIter != __nodeId2Database.endSafe(); ++nodeIter )
                delete *nodeIter;

            for(HashTableIteratorSafe<NodeId, NodeDatabase<GUM_SCALAR>*> nodeIter = __nodeSonsMap.beginSafe();
                    nodeIter != __nodeSonsMap.endSafe(); ++nodeIter )
                MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( *nodeIter, sizeof(NodeId)*__nodeVarMap[nodeIter.key()]);

            GUM_DESTRUCTOR(IMDDI);
        }



    // ############################################################################
    // Incrementals methods
    // ############################################################################

        // ============================================================================
        // Adds a new observation to the structure
        // ============================================================================
        template <typename GUM_SCALAR>
        void IMDDI<GUM_SCALAR>::addObservation( const Observation<GUM_SCALAR>* newObs ){
            
            // First we increase the total number of observation added
            __nbTotalObservation++;
            
            // The we go across the tree
            NodeId currentNodeId = __root;

            while( __nodeSonsMap.exists(currentNodeId) ){
                
                // On each encountered node, we update the database
                __nodeId2Database[currentNodeId]->addObservation( newObs );
                
                // The we select the next to go throught
                currentNodeId = (*__nodeSonsMap[currentNodeId])[newObs->modality( __nodeVarMap[currentNodeId] )];
            }

            // On final insertion into the leave we reach
            __nodeId2Database[currentNodeId]->addObservation( newObs );

        }


        // ============================================================================
        // Updates the tree after a new observation has been added
        // ============================================================================
        template <typename GUM_SCALAR>
        void IMDDI<GUM_SCALAR>::updateOrderedTree(){

            // First xe initialize the node set which will give us the scores
            Set<NodeId> currentNodeSet;
            currentNodeSet.insert( __root );

            // Then we initialize the pool of variables to consider
            Set<const DiscreteVariable*> remainingVars(__setOfVars);

            MultiPriorityQueue<const DiscreteVariable*, double, std::greater<double>> remainingVarsScore;
            for( SetIteratorSafe<const DiscreteVariable*> varIter = remainingVars.beginSafe();
                    varIter != remainingVars.endSafe(); ++varIter )
                remainingVarsScore.insert(*varIter, __score(*varIter, __root));

            // Then, until there's no node remaining
            while( !remainingVars.empty() ){

                // We select the best var
                const DiscreteVariable* selectedVar = remainingVarsScore.pop();

                // Then we decide if we update each node according to this var
                __updateNodeSet( selectedVar, currentNodeSet );

            }

            // If there are remaining node that are not leaves after we establish the var order
            // these nodes are turned into leaf.
            for(SetIteratorSafe<NodeId> nodeIter = currentNodeSet.beginSafe(); nodeIter != currentNodeSet.endSafe(); ++nodeIter )
                __turnIntoLeaf(*nodeIter);
        }



    // ############################################################################
    // Updating methods
    // ############################################################################

        // ============================================================================
        // Select the most relevant variable
        //
        // First parameter is the set of variables among which the most
        // relevant one is choosed
        // Second parameter is the set of node the will attribute a score
        // to each variable so that we choose the best.
        // ============================================================================
        template <typename GUM_SCALAR>
        INLINE
        double IMDDI<GUM_SCALAR>::__score( const DiscreteVariable* var, NodeId nody){

            double weight = (double)__nodeId2Database[nody]->nbObservation() / (double) this->__nbTotalObservation;
            return weight*__nodeId2Database[nody]->pValue( var );
        }


        // ============================================================================
        // For each node in the given set, this methods checks whether or not
        // we should installed the given variable as a test.
        // If so, the node is updated
        // ============================================================================
        template <typename GUM_SCALAR>
        void IMDDI<GUM_SCALAR>::__updateNodeSet( Set<NodeId>& nodeSet,
                                                 const DiscreteVariable* selectedVar,
                                                 MultiPriorityQueue<const DiscreteVariable*, double, std::greater<double>>& remainingVarsScore ){

            Set<NodeId> oldNodeSet(nodeSet);
            nodeSet.clear();
            for(SetIteratorSafe<NodeId> nodeIter = oldNodeSet.beginSafe(); nodeIter != oldNodeSet.endSafe(); ++nodeIter ){

                if( __nodeId2Database[*nodeIter]->pValue( selectedVar) > __dependenceThreshold ){

                    __transpose(*nodeIter, selectedVar);

                    // Then we subtract the from the score given to each variables the quantity given by this node
                    for( HashTableConstIteratorSafe<const DiscreteVariable*, std::vector<Size>>
                            varIter = remainingVarsScore.allValues().cbeginSafe();
                            varIter != remainingVarsScore.allValues().cendSafe();
                            ++varIter ) {
                        double newPriority = remainingVarsScore.priority( varIter.key() ) - __score( varIter.key(), *nodeIter );
                        remainingVarsScore.setPriority(varIter.key(), newPriority);
                    }

                    // And finally we add all its child to the new set of nodes
                    // and updates the remaining var's score
                    for( Idx  modality = 0; modality < __nodeVarMap[*nodeIter]->domainSize(); ++modality){
                        nodeSet << __nodeSonsMap[*nodeIter][modality];

                        for( HashTableConstIteratorSafe<const DiscreteVariable*, std::vector<Size>>
                                varIter = remainingVarsScore.allValues().cbeginSafe();
                                varIter != remainingVarsScore.allValues().cendSafe();
                                ++varIter ) {
                            double newPriority = remainingVarsScore.priority( varIter.key() )
                                    + __score( varIter.key(), __nodeSonsMap[*nodeIter][modality] );
                            remainingVarsScore.setPriority(varIter.key(), newPriority);
                        }
                    }
                } else {
                    nodeSet << *nodeIter;
                }
            }
        }


        // ============================================================================
        // Bring the desiredVar to the given node
        // ============================================================================
        template <typename GUM_SCALAR>
        void IMDDI<GUM_SCALAR>::__transpose(NodeId currentNodeId,
                                  const DiscreteVariable* desiredVar ){

            // Si le noeud courant contient déjà la variable qu'on souhaite lui amener
            // Il n'y a rien à faire
            if( __nodeVarMap[currentNodeId] == desiredVar )
                return;

            // Si le noeud courant est terminal,
            // Il faut artificiellement insérer un noeud liant à la variable
            if( __nodeVarMap[currentNodeId] == __value ){

                Sequence<NodeDatabase<GUM_SCALAR>*> sonsNodeDatabase = __nodeId2Database[currentNodeId]->splitonVar(desiredVar);
                NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*desiredVar->domainSize()) );

                for( Idx modality = 0; modality < desiredVar->domainSize(); ++modality ){
                    NodeId newNodeId = __model.insertNode();
                    __nodeVarMap.insert(newNodeId, __value);
                    __nodeId2Database.insert(newNodeId, sonsNodeDatabase.atPos(modality));
                    sonsMap[modality] = newNodeId;
                }

                __nodeVarMap[currentNodeId] = desiredVar;
                __nodeSonsMap.insert( currentNodeId, sonsMap);

                return;
            }

            // Remains the general case where currentNodeId is an internal node.

            // First we ensure that children node use desiredVar as variable
            for(Idx modality = 0; modality < __nodeVarMap[currentNodeId]->domainSize(); ++modality )
                __transpose( __nodeSonsMap[currentNodeId][modality], desiredVar );

            Sequence<NodeDatabase<GUM_SCALAR>*> sonsNodeDatabase = __nodeId2Database[currentNodeId]->splitonVar(desiredVar);
            NodeId* sonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*desiredVar->domainSize()) );

            // Then we create the new mapping
            for(Idx desiredVarModality = 0; desiredVarModality < desiredVar->domainSize(); ++desiredVarModality){
                NodeId* grandSonsMap = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate(sizeof(NodeId)*__nodeVarMap[currentNodeId]->domainSize()) );
                for(Idx currentVarModality = 0; currentVarModality < __nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality )
                    grandSonsMap[currentVarModality] = __nodeSonsMap[__nodeSonsMap[currentNodeId][currentVarModality]][desiredVarModality];

                NodeId newNodeId = __model.insertNode();
                __nodeVarMap.insert(newNodeId, __nodeVarMap[currentNodeId]);
                __nodeId2Database.insert(newNodeId, sonsNodeDatabase.atPos(desiredVarModality));
                sonsMap[desiredVarModality] = newNodeId;
            }

            // Finally we clean the old remaining nodes
            for(Idx currentVarModality = 0; currentVarModality < __nodeVarMap[currentNodeId]->domainSize(); ++currentVarModality ){
                NodeId sonId = __nodeSonsMap[currentNodeId][currentVarModality];

                // Retriat de l'id
                __model.eraseNode( sonId );

                // Retrait de la variable
                __nodeVarMap.erase( sonId );

                // Retrait du NodeDatabase
                delete __nodeId2Database[sonId];
                __nodeId2Database.erase(sonId);

                // Retrait du vecteur fils
                MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( __nodeSonsMap[sonId], sizeof(NodeId)*desiredVar->domainSize() );
                __nodeSonsMap.erase(sonId);
            }

            // We suppress the old sons map and remap to the new one
            MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( __nodeSonsMap[currentNodeId], sizeof(NodeId)*__nodeVarMap[currentNodeId]->domainSize() );
            __nodeSonsMap[currentNodeId] = sonsMap;

            __nodeVarMap[currentNodeId] = desiredVar;

        }


        template <typename GUM_SCALAR>
        void IMDDI<GUM_SCALAR>::__turnIntoLeaf(NodeId currentNodeId){

            if(__nodeVarMap[currentNodeId] == __value )
                return;

            //
            for(Idx modality = 0; modality < __nodeVarMap[currentNodeId]->domainSize(); ++modality) {
                NodeId sonId = __nodeSonsMap[currentNodeId][modality];

                __turnIntoLeaf(sonId);

                // Retriat de l'id
                __model.eraseNode( sonId );

                // Retrait du NodeDatabase
                delete __nodeId2Database[sonId];
                __nodeId2Database.erase(sonId);

                // Retrait du vecteur fils
                MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( __nodeSonsMap[sonId], sizeof(NodeId)*__nodeVarMap[sonId]->domainSize() );
                __nodeSonsMap.erase(sonId);

                // Retrait de la variable
                __nodeVarMap.erase( sonId );

            }


            MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( __nodeSonsMap[currentNodeId], sizeof(NodeId)*__nodeVarMap[currentNodeId]->domainSize() );
            __nodeSonsMap.erase(currentNodeId);

            __nodeVarMap[currentNodeId] = __value;
        }
} // end gum namespace
