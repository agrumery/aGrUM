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
#include <agrum/core/multiPriorityQueue.h>
// =======================================================
#include <agrum/FMDP/learning/IMDDI.h>
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
        IMDDI::IMDDI( MultiDimDecisionGraph<GUM_SCALAR>* target, double learningThreshold ): __target(target),
                                               __intermediate(new MultiDimDecisionGraph<GUM_SCALAR>()),
                                               __nbTotalObservation(0){
            GUM_CONSTRUCTOR(IMDDI);
        }


        // ============================================================================
        // Default destructor.
        // ============================================================================
        template < typename GUM_SCALAR>
        IMDDI::~IMDDI(){
            delete __intermediate;

            for(HashTableIteratorSafe<NodeId, NodeDatabase<GUM_SCALAR>*> nodeIter = __nodeId2Database.beginSafe();
                    nodeIter != __nodeId2Database.endSafe(); ++nodeIter )
                delete *nodeIter;

            GUM_DESTRUCTOR(IMDDI);
        }



    // ############################################################################
    // Incrementals methods
    // ############################################################################

        // ============================================================================
        // Adds a new observation to the structure
        // ============================================================================
        template <typename GUM_SCALAR>
        void IMDDI::addObservation( const Observation* newObs ){
            
            // First we increase the total number of observation added
            __nbTotalObservation++;
            
            // The we go across the tree
            NodeId currentNodeId = __intermediate->root();
            while( /* qqch*/ ){
                
                // On each encountered node, we update the database
                __nodeId2Database[currentNodeId]->addObservation( newObs );
                
                // The we select the next to go throught
                currentNodeId = __intermediate->node( currentNodeId )->son( newObs->modality( __intermediate->node( currentNodeId )->nodeVar() ) );
            }
        }


        // ============================================================================
        // Updates the tree after a new observation has been added
        // ============================================================================
        template <typename GUM_SCALAR>
        void IMDDI::updateOrderedTree(){

            // First xe initialize the node set which will give us the scores
            Set<NodeId> currentNodeSet;
            currentNodeSet.insert( __intermediate->root() );

            // Then we initialize the pool of variables to consider
            Set<const DiscreteVariable*> remainingVars(__setOfVars);

            PriorityQueue<const DiscreteVariable*, double, std::greater> remainingVarsScore;
            for( SetIteratorSafe<const DiscreteVariable*> varIter = remainingVars.beginSafe();
                    varIter != remainingVars.endSafe(); ++varIter )
                remainingVarsScore.insert(__score(*varIter, __intermediate->root()), *varIter);

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
        const DiscreteVariable* IMDDI::__score( const DiscreteVariable* var, NodeId nody){

            double weight = (double)__nodeId2Database[nody]->nbObservation() / (double) this->__nbTotalObservation;
            return weight*__nodeId2Database[nody]->pValue( var );
        }
} // end gum namespace
