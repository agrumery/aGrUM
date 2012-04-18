/*********************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES  	*
 *   {prenom.nom}_at_lip6.fr                                                                           	*
 *                                                                                                                     	*
 *   This program is free software; you can redistribute it and/or modify         	*
 *   it under the terms of the GNU General Public License as published by      	*
 *   the Free Software Foundation; either version 2 of the License, or            	*
 *   (at your option) any later version.                                                             	*
 *                                                                                                                     	*
 *   This program is distributed in the hope that it will be useful,                     	*
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          	*
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         	*
 *   GNU General Public License for more details.                                             	*
 *                                                                                                                     	*
 *   You should have received a copy of the GNU General Public License         	*
 *   along with this program; if not, write to the                                               	*
 *   Free Software Foundation, Inc.,                                                                	*
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                     	*
 ********************************************************************************/
/**
* @file
* @brief Files containing recursions functions for operators data.
*
* @author Jean-Christophe Magnan
*/

#ifdef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION


// =============================================================================================
#include <agrum/randomVariables/discreteVariable.h>
#include <agrum/multidim/multiDimDecisionDiagramBase.h>
#include <agrum/multidim/patterns/o4DDMiscellaneous.h>
// =============================================================================================

namespace gum {
  
// =============================================================================================
/// Main recursion function, called every time we move on a node to determine what we have to do
// =============================================================================================
// A key is used for prunning uneccesary operations since once a node has been visited in a given context, there's no use to revisit him,
// the result will be the same node, so we just have to do an association context - node.
// The context consists in :
//              _ Leader node we are visiting.
//              _ Follower node we are visiting.
//              _ For all retrograde variables, if it has been instanciated before, current modality instanciated, meaning :
//                      _ 0 means the variable hasn't be instanciated yet,
//                      _ From 1 to domainSize + 1 means that current modality index of variable is value - 1,
//                      _ domainSize + 2 means variable is on default mode.
// A key - node association is made each time we create a node in resulting diagram.
// Since GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION is a corner step in algorithm ( meaning each time we explore a node we go trought
// this function ), check only have to be at the beginning of that function.
template<typename T>
NodeId
GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( const MultiDimDecisionDiagramBase<T>* leader,
                                                                                      const MultiDimDecisionDiagramBase<T>* follower,
                                                                                      OperatorData<T>& opData,
                                                                                      const DiscreteVariable* leaderParentVar,
                                                                                      std::string tabu  ) {

    if ( opData.explorationTable->exists( opData.conti.contextKey() ) )
        return ( *( opData.explorationTable ) )[ opData.conti.contextKey() ];

    NodeId newNode = 0;


    // ******************************************************************************************************
    // If both current nodes are terminal,
    // we only have to compute the resulting value
    if ( leader->isTerminalNode( opData.conti.getLeaderNode() ) && follower->isTerminalNode( opData.conti.getFollowerNode() ) ) {
        // We have to compute new valueand we insert a new node in diagram with this value, ...
        newNode = opData.factory->addTerminalNode( GUM_MULTI_DIM_OPERATOR( leader->unsafeNodeValue( opData.conti.getLeaderNode() ), follower->unsafeNodeValue( opData.conti.getFollowerNode() ) ) );

        // And ensure that if we get back to those node we won't explore them again
        opData.explorationTable->insert( opData.conti.contextKey(), newNode );
    }
    // *******************************************************************************************************

    // ******************************************************************************************************
    // If not, first we ensure that all retrograde variable have been considered
    if ( !follower->isTerminalNode( opData.conti.getFollowerNode() ) )
        if ( opData.retrogradeVarTable->exists( opData.conti.getFollowerNode() ) ) {

            Idx indexFin = opData.factory->variablesSequence().size();
            if ( !leader->isTerminalNode( opData.conti.getLeaderNode() ) )
                indexFin = opData.factory->variablesSequence().pos( leader->unsafeNodeVariable( opData.conti.getLeaderNode() ) );

            Idx indexDebut = 0;
            if ( leaderParentVar != NULL ) 
                indexDebut = opData.factory->variablesSequence().pos( leaderParentVar ) +1;

            if ( indexFin != indexDebut )
                for ( Idx i = indexDebut; i < indexFin; ++i ) {
                    const DiscreteVariable* preneededVar = opData.factory->variablesSequence().atPos( i );
                    if ( (*opData.retrogradeVarTable)[ opData.conti.getFollowerNode() ]->exists( preneededVar ) && ( opData.conti.variableModality( preneededVar ) == 0 ) ) {

                        const std::vector< bool >* usedModalities = follower->variableUsedModalities( preneededVar );
                        if ( usedModalities == NULL )
                            continue;

                        std::vector< NodeId > sonsIds( preneededVar->domainSize(), 0);
                        Idx nbExploredModalities = 0;
                        for ( Idx indexModality = 0; indexModality < preneededVar->domainSize(); indexModality++ ) {
                            if ( (*usedModalities)[ indexModality ] ) {
                                opData.conti.chgVarModality( preneededVar, indexModality + 1 );
                                sonsIds[ indexModality ] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, preneededVar, tabu  );
                                ++nbExploredModalities;
                            }
                        }

                        NodeId defaultSon = 0;
                        if ( nbExploredModalities != preneededVar->domainSize() ) {
                            opData.conti.chgVarModality( preneededVar, preneededVar->domainSize() + 1 );
                            defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, preneededVar, tabu  );
                        }

                        newNode = insertNonTerminalNode( opData, preneededVar, sonsIds, defaultSon );

                        opData.conti.chgVarModality( preneededVar, 0 );
                        opData.explorationTable->insert(  opData.conti.contextKey(), newNode );

                        return newNode;
                    }
                }
        }
    // *******************************************************************************************************

    // *******************************************************************************************************
    // If only one of the current node is terminal,
    // we have to pursue deeper on the other diagram
    if ( leader->isTerminalNode( opData.conti.getLeaderNode() ) && !follower->isTerminalNode( opData.conti.getFollowerNode() ) )
        newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( leader, follower, opData, leaderParentVar, tabu  );


    if ( !leader->isTerminalNode( opData.conti.getLeaderNode() ) && follower->isTerminalNode( opData.conti.getFollowerNode() ) )
        newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( leader, follower, opData, leaderParentVar, tabu  );
    // *******************************************************************************************************


    // *******************************************************************************************************
    // Remains the case where both nodes aren't terminal
    if ( !leader->isTerminalNode( opData.conti.getLeaderNode() ) && !follower->isTerminalNode( opData.conti.getFollowerNode() ) ) {

        //==========================================================
        // First we have to determine the position of current node
        // associated variable in the resulting diagram
        int indexLeader = -1;
        int indexFollower = -1;

        if ( opData.factory->variablesSequence().exists( leader->unsafeNodeVariable( opData.conti.getLeaderNode() ) ) )
            indexLeader = opData.factory->variablesSequence().pos(  leader->unsafeNodeVariable( opData.conti.getLeaderNode() ) );
        if ( opData.factory->variablesSequence().exists( follower->unsafeNodeVariable( opData.conti.getFollowerNode() ) ) )
            indexFollower = opData.factory->variablesSequence().pos(  follower->unsafeNodeVariable( opData.conti.getFollowerNode() ) );
        if ( indexLeader == -1 || indexFollower == -1 )
            GUM_ERROR( FatalError, "Unable to compute MultiDimDecisionDiagramBase");
        //==========================================================

        //==========================================================
        // If one variable preceeds the other on final diagram, we
        // have to go down on this one
        if ( indexLeader > indexFollower )
            newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( leader, follower, opData, leaderParentVar, tabu  );

        if ( indexLeader < indexFollower )
            newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( leader, follower, opData, leaderParentVar, tabu );
        //==========================================================

        //==========================================================
        // If not - meaning it's the same variable - we have to go
        // down on both
        if ( indexLeader == indexFollower ) {

            const std::vector< NodeId >* leaderSonsMap = leader->unsafeNodeSons( opData.conti.getLeaderNode() );
            const std::vector< NodeId >* followerSonsMap = follower->unsafeNodeSons( opData.conti.getFollowerNode() );

            NodeId leaderCurrentNode = opData.conti.getLeaderNode();
            NodeId followerCurrentNode = opData.conti.getFollowerNode();
            std::vector< NodeId > sonsIds( leaderSonsMap->size(), 0);
            for ( Idx indexModality = 0; indexModality < leader->unsafeNodeVariable( leaderCurrentNode )->domainSize(); indexModality++ ) {
                if ( (*leaderSonsMap)[ indexModality ] == 0 )
                    opData.conti.setLeaderNode( leader->unsafeNodeDefaultSon( leaderCurrentNode ) );
                else
                    opData.conti.setLeaderNode( (*leaderSonsMap)[ indexModality ] );

                if ( (*followerSonsMap)[ indexModality ] == 0 )
                    opData.conti.setFollowerNode( follower->unsafeNodeDefaultSon( followerCurrentNode ) );
                else
                    opData.conti.setFollowerNode( (*followerSonsMap)[ indexModality ] );

                sonsIds[ indexModality ] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leader->unsafeNodeVariable( leaderCurrentNode ), tabu  );
            }

            NodeId defaultSon = 0;
            if ( leader->unsafeHasNodeDefaultSon( leaderCurrentNode ) && follower->unsafeHasNodeDefaultSon( followerCurrentNode ) ) {
                opData.conti.setLeaderNode( leader->unsafeNodeDefaultSon( leaderCurrentNode ) );
                opData.conti.setFollowerNode( follower->unsafeNodeDefaultSon( followerCurrentNode ) );
                defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leader->unsafeNodeVariable( leaderCurrentNode ), tabu  );
            }

            newNode = insertNonTerminalNode( opData, leader->unsafeNodeVariable( leaderCurrentNode ), sonsIds, defaultSon );

            opData.conti.setLeaderNode( leaderCurrentNode );
            opData.conti.setFollowerNode( followerCurrentNode );
            opData.explorationTable->insert(  opData.conti.contextKey(), newNode );

        }
    }
    // *******************************************************************************************************

    return newNode;
}



// =========================================================================================================================================================
/// Called when we investigate a node of the first diagram only
// =========================================================================================================================================================
template< typename T >
NodeId
GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( const MultiDimDecisionDiagramBase<T>* leader,
        const MultiDimDecisionDiagramBase<T>* follower,
        OperatorData<T>& opData,
        const DiscreteVariable* leaderParentVar,
        std::string tabu ) {

    NodeId newNode = 0;

    Idx nbExploredModalities = 0;
    NodeId leaderCurrentNode = opData.conti.getLeaderNode();
    const DiscreteVariable* leaderCurrentVar = leader->unsafeNodeVariable( leaderCurrentNode );
    bool isIndeedRetorgrade = opData.conti.isRetrogradeVar( leaderCurrentVar );
    const std::vector< NodeId >* sonsMap = leader->unsafeNodeSons( leaderCurrentNode );

    const std::vector< bool >* usedModalities = follower->variableUsedModalities( leaderCurrentVar );
    std::vector< NodeId > sonsIds( sonsMap->size(), 0);

    // ********************************************************************************************************
    // For each value the current var take on this node, we have to do our computation
    for ( Idx sonKey = 0; sonKey < leaderCurrentVar->domainSize(); sonKey++ ) {
        NodeId sonId = (*sonsMap)[ sonKey ];
        if ( sonId != 0 || ( usedModalities != NULL && (*usedModalities)[ sonKey ] ) ) {

            // But we have to indicates to possible node on follower diagram, the current value of the var
            if ( isIndeedRetorgrade )
                opData.conti.chgVarModality( leaderCurrentVar, sonKey + 1 );
            if ( sonId != 0 )
                opData.conti.setLeaderNode( sonId );
            else
                opData.conti.setLeaderNode( leader->unsafeNodeDefaultSon( leaderCurrentNode ) );
            sonsIds[ sonKey ] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderCurrentVar, tabu  );

            ++nbExploredModalities;
        }
    }
    //*********************************************************************************************************

    // ********************************************************************************************************
    // Then, if not all possible value of that node have been investigate (meaning we have a default arc)
    // we have to look on second diagram every value that can still take this var (meaning value not taken
    // on this leader node and the defautl one )
    NodeId defaultSon = 0;

    if ( nbExploredModalities != leaderCurrentVar->domainSize() ) {
        opData.conti.setLeaderNode( leader->unsafeNodeDefaultSon( leaderCurrentNode ) );
        if ( isIndeedRetorgrade )
            opData.conti.chgVarModality( leaderCurrentVar, leaderCurrentVar->domainSize() + 1 );

        defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderCurrentVar, tabu  );
    }

    //*********************************************************************************************************

    // ********************************************************************************************************
    // And we finally add this node to our resulting graph
    newNode = insertNonTerminalNode( opData, leader->unsafeNodeVariable(leaderCurrentNode), sonsIds, defaultSon );

    opData.conti.setLeaderNode( leaderCurrentNode );
    if ( isIndeedRetorgrade )
        opData.conti.chgVarModality( leaderCurrentVar, 0 );
    opData.explorationTable->insert(  opData.conti.contextKey(), newNode );

    return newNode;
}



// =========================================================================================================================================================
/// Called when we investigate a node of the second diagram only
// =========================================================================================================================================================
template< typename T >
NodeId
GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( const MultiDimDecisionDiagramBase<T>* leader,
        const MultiDimDecisionDiagramBase<T>* follower,
        OperatorData<T>& opData,
        const DiscreteVariable* leaderParentVar,
        std::string tabu ) {

    NodeId newNode = 0;

    NodeId followerCurrentNode = opData.conti.getFollowerNode();
    const DiscreteVariable* followerCurrentVar = follower->unsafeNodeVariable( followerCurrentNode );
    bool isIndeedRetorgrade = opData.conti.isRetrogradeVar( followerCurrentVar );
    const std::vector< NodeId >* sonsMap = follower->unsafeNodeSons( followerCurrentNode );

    if ( isIndeedRetorgrade && opData.conti.variableModality( followerCurrentVar ) != 0 ) {
        // ***************************************************************************************************************
        // If var exists in leader diagram and has already been instantiate to its default value,
        // we have to do so in second diagram too
        Idx varModality = opData.conti.variableModality( followerCurrentVar ) - 1;
        if ( varModality ==  followerCurrentVar->domainSize() ) {
            opData.conti.setFollowerNode( follower->unsafeNodeDefaultSon( followerCurrentNode ) );
            newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, tabu  );

            // ***************************************************************************************************************
        } else {
            // ***************************************************************************************************************
            // If var exists in leader diagram and has already been instantiate to a value,
            // we have to go down on this value

            // But we have to check if value has its arc for this node
            if ( (*sonsMap)[ varModality ] != 0 )
                opData.conti.setFollowerNode(  (*sonsMap)[ varModality ] );
            else
                opData.conti.setFollowerNode( follower->unsafeNodeDefaultSon( followerCurrentNode ) );

            newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, tabu  );
        }
        opData.conti.setFollowerNode( followerCurrentNode );
        // ****************************************************************************************************************
    } else {
        // ***************************************************************************************************************
        // If we aren't in one of the above cases, it means that leader diagram hasn't the variable pointed by follower current node.
        std::vector< NodeId > sonsIds( sonsMap->size(), 0 );

        // In that case we do our computation for all sons of that var
        for ( std::vector<NodeId>::const_iterator iter = sonsMap->begin(); iter != sonsMap->end(); ++iter)
            if ( *iter != 0 ) {
                opData.conti.setFollowerNode( *iter );
                sonsIds[ std::distance( sonsMap->begin(), iter ) ] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, tabu  );
            }

        // and for its default son if it has one
        NodeId defaultSon = 0;
        if ( follower->unsafeHasNodeDefaultSon( followerCurrentNode ) ) {
            opData.conti.setFollowerNode( follower->unsafeNodeDefaultSon( followerCurrentNode ) );
            defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, tabu  );
        }

        // And we had this node to our graph
        newNode = insertNonTerminalNode( opData, followerCurrentVar, sonsIds, defaultSon );

        opData.conti.setFollowerNode( followerCurrentNode );
        opData.explorationTable->insert(  opData.conti.contextKey(), newNode );

    }
    // ****************************************************************************************************************

    return newNode;
}

}/* end of namespace GUM */

#endif /* GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION */
