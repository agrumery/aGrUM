/*********************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                                                            *
 *                                                                                                                      *
 *   This program is free software; you can redistribute it and/or modify           *
 *   it under the terms of the GNU General Public License as published by       *
 *   the Free Software Foundation; either version 2 of the License, or              *
 *   (at your option) any later version.                                                              *
 *                                                                                                                      *
 *   This program is distributed in the hope that it will be useful,                     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                                             *
 *                                                                                                                      *
 *   You should have received a copy of the GNU General Public License          *
 *   along with this program; if not, write to the                                                *
 *   Free Software Foundation, Inc.,                                                                  *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                      *
 ********************************************************************************/
/**
* @file
* @brief Files containing recursions functions for operators data. This ones are for debug on improvement only, see its normal one for common use
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

#ifdef GUM_DECISION_GRAPH_OPERATOR_FUNCTION_DEFINITION
// =====================================================================
#include <agrum/variables/discreteVariable.h>
// =====================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
#include <agrum/multidim/patterns/DGUtility/o4DGMiscellaneous.h>
// =====================================================================


namespace gum {


/// Main recursion function, called every time we move on a new node
/// It determines what we have to do

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
// Since GUM_MULTI_DIM_DECISION_GRAPH_RECUR_FUNCTION is a corner step in algorithm ( meaning each time we explore a node we go trought
// this function ), check only have to be at the beginning of that function.
  template<typename GUM_SCALAR >
  NodeId
  GUM_MULTI_DIM_DECISION_GRAPH_RECUR_FUNCTION( DGNonOrderedOperatorData< GUM_SCALAR >& opData,
                                                 const DiscreteVariable* leaderParentVar,
                                                 std::string tabu ) {

    // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "PRUNNING EVALUATION : " );
    // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "KEY : " << opData.conti.contextKey() );
// // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "RECUR EXPLORATION STATUS : " << *(opData.explorationTable) );

    if ( opData.explorationTable->exists( opData.conti.contextKey() ) ) {
      // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "PRUNING!" );
      return ( *( opData.explorationTable ) )[ opData.conti.contextKey() ];
    }

    tabu  += "\t";
    NodeId newNode = 0;


    // ******************************************************************************************************
    // If both current nodes are terminal,
    // we only have to compute the resulting value
    if ( opData.DG1->isTerminalNode( opData.conti.getDG1Node() ) && opData.DG2->isTerminalNode( opData.conti.getDG2Node() ) ) {
      // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Ajout noeud terminal " << " | Noeud leader : " <<  opData.conti.getDG1Node() << " | Noeud follower : " <<  opData.conti.getDG2Node() );

      // We have to compute new valueand we insert a new node in diagram with this value, ...
      GUM_SCALAR leaderValue = opData.DG1->nodeValue( opData.conti.getDG1Node() );
      GUM_SCALAR followerValue = opData.DG2->nodeValue( opData.conti.getDG2Node() );
      GUM_SCALAR newVal = GUM_MULTI_DIM_OPERATOR( leaderValue, followerValue );
      newNode = opData.DG3->manager()->addTerminalNode( newVal );
      // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Leader value : "  << leaderValue  << " - Follower Value : " << followerValue << " | Resulting value : " << newVal << " - Resulting node : " << newNode );

      // And ensure that if we get back to those node we won't explore them again
      // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "KEY INSERTION : " );
      // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Context : " << opData.conti.toString() << "Ajout de : " << opData.conti.contextKey() << " -> " << newNode );
      opData.explorationTable->insert( opData.conti.contextKey(), newNode );
// // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  <<  "LEAF EXPLORATION STATUS : " << *(opData.explorationTable) );
    }

    // *******************************************************************************************************

    // ******************************************************************************************************
    // If not, first we ensure that all retrograde variable have been instantiated
    if ( !opData.DG2->isTerminalNode( opData.conti.getDG2Node() ) )
      if ( opData.conti.retrogradeVarTable->exists( opData.conti.getDG2Node() ) ) {

        // GUM_TRACE("\n" << tabu << "Preneeded Variable : " );

//        for ( SetIterator< const DiscreteVariable* > iterS = ( *opData.conti.retrogradeVarTable )[ opData.conti.getDG2Node() ]->begin();
//              iterS != ( *opData.conti.retrogradeVarTable )[ opData.conti.getDG2Node() ]->end(); ++iterS )
          // GUM_TRACE("\n" << tabu << ( *iterS )->toString() << " | " );


        Idx indexFin = opData.DG3->variablesSequence().size();

        if ( !opData.DG1->isTerminalNode( opData.conti.getDG1Node() ) )
          indexFin = opData.DG3->variablesSequence().pos( opData.DG1->node( opData.conti.getDG1Node() )->nodeVar );

        Idx indexDebut = 0;

        if ( leaderParentVar != nullptr ) {
          // GUM_TRACE("\n" << tabu << "ParentVar : " << leaderParentVar->toString() );
          indexDebut = opData.DG3->variablesSequence().pos( leaderParentVar ) +1;
        }

        if ( indexFin != indexDebut )
          for ( Idx i = indexDebut; i < indexFin; ++i ) {
            const DiscreteVariable* preneededVar = opData.DG3->variablesSequence().atPos( i );

            if ( ( *opData.conti.retrogradeVarTable )[ opData.conti.getDG2Node() ]->exists( preneededVar ) && ( opData.conti.variableModality( preneededVar ) == 0 ) ) {

              // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << " Action Taken! - Noeud leader : " << opData.conti.getDG1Node() << " - Noeud follower : " << opData.conti.getDG2Node() << " - Instantiated Variable : " << preneededVar->toString() );

              NodeId* sonsIds = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate( preneededVar->domainSize() * sizeof( NodeId ) ) );

              bool theSame = true;
              for ( Idx modality = 0; modality < preneededVar->domainSize(); modality++ ) {

                  opData.conti.chgVarModality( preneededVar, modality + 1 );
                  sonsIds[ modality ] = GUM_MULTI_DIM_DECISION_GRAPH_RECUR_FUNCTION( opData, preneededVar, tabu );
                  if( sonsIds[ modality ] != sonsIds[ 0 ] )
                      theSame = false;
              }

              if( theSame ){
                  newNode = sonsIds[0];
                  MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( sonsIds, preneededVar->domainSize() * sizeof( NodeId ) );
              } else {
                newNode = opData.DG3->manager()->checkIsomorphism( preneededVar, sonsIds );
                if( newNode == 0 )
                  newNode = opData.DG3->manager()->addNonTerminalNode( preneededVar, sonsIds );
              }

              // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "KEY INSERTION : " );
              opData.conti.chgVarModality( preneededVar, 0 );
              // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Context : " << opData.conti.toString() << "Ajout de : " << opData.conti.contextKey() << " -> " << newNode );
              opData.explorationTable->insert( opData.conti.contextKey(), newNode );
// // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "RETROGRADE EXPLORATION STATUS : " << *(opData.explorationTable) );

              // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Fin Action Taken! - Instantiated Variable : " << preneededVar->toString() << " - Resulting Node : " << newNode );

              return newNode;
            }
          }
      }

    // *******************************************************************************************************

    // *******************************************************************************************************
    // If only one of the current node is terminal,
    // we have to pursue deeper on the other diagram
    if ( opData.DG1->isTerminalNode( opData.conti.getDG1Node() ) && !opData.DG2->isTerminalNode( opData.conti.getDG2Node() ) )
      newNode = GUM_MULTI_DIM_DECISION_GRAPH_GO_DOWN_ON_FOLLOWER_FUNCTION( opData, leaderParentVar, tabu );


    if ( !opData.DG1->isTerminalNode( opData.conti.getDG1Node() ) && opData.DG2->isTerminalNode( opData.conti.getDG2Node() ) )
      newNode = GUM_MULTI_DIM_DECISION_GRAPH_GO_DOWN_ON_LEADER_FUNCTION( opData, leaderParentVar, tabu );

    // *******************************************************************************************************


    // *******************************************************************************************************
    // Remains the case where both nodes aren't terminal
    if ( !opData.DG1->isTerminalNode( opData.conti.getDG1Node() ) && !opData.DG2->isTerminalNode( opData.conti.getDG2Node() ) ) {

      //==========================================================
      // First we have to determine the position of current node
      // associated variable in the resulting diagram
      int indexLeader = -1;
      int indexFollower = -1;

      if ( opData.DG3->variablesSequence().exists( opData.DG1->node( opData.conti.getDG1Node() )->nodeVar ) )
        indexLeader = opData.DG3->variablesSequence().pos( opData.DG1->node( opData.conti.getDG1Node() )->nodeVar );

      if ( opData.DG3->variablesSequence().exists( opData.DG2->node( opData.conti.getDG2Node() )->nodeVar ) )
        indexFollower = opData.DG3->variablesSequence().pos( opData.DG2->node( opData.conti.getDG2Node() )->nodeVar );

      if ( indexLeader == -1 || indexFollower == -1 )
        GUM_ERROR( FatalError, "Unable to compute MultiDimDecisionDiagramBase" );

      //==========================================================

      //==========================================================
      // If one variable preceeds the other on final diagram, we
      // have to go down on this one
      if ( indexLeader > indexFollower )
        newNode = GUM_MULTI_DIM_DECISION_GRAPH_GO_DOWN_ON_FOLLOWER_FUNCTION( opData, leaderParentVar, tabu );

      if ( indexLeader < indexFollower )
        newNode = GUM_MULTI_DIM_DECISION_GRAPH_GO_DOWN_ON_LEADER_FUNCTION( opData, leaderParentVar, tabu );

      //==========================================================

      //==========================================================
      // If not - meaning it's the same variable - we have to go
      // down on both
      if ( indexLeader == indexFollower ) {
        // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Recur - Noeud leader : " << opData.conti.getDG1Node() << " - Noeud follower : " << opData.conti.getDG2Node() );

        NodeId leaderNodeId = opData.conti.getDG1Node();
        NodeId followerNodeId = opData.conti.getDG2Node();

        const typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* leaderNode = opData.DG1->node( leaderNodeId );
        const typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* followerNode = opData.DG2->node( followerNodeId );

        NodeId* sonsIds = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate( leaderNode->nodeVar->domainSize() * sizeof( NodeId ) ) );
        bool theSame = true;

        for ( Idx modality = 0; modality < leaderNode->nodeVar->domainSize(); modality++ ) {
          // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Recur - Descente sur fils numéro : " << modality );
          opData.conti.setDG1Node( leaderNode->nodeSons[ modality ] );
          opData.conti.setDG2Node( followerNode->nodeSons[ modality ] );

          sonsIds[ modality ] = GUM_MULTI_DIM_DECISION_GRAPH_RECUR_FUNCTION( opData, leaderNode->nodeVar, tabu );
          if( sonsIds[ modality ] != sonsIds[ 0 ] )
              theSame = false;
        }

        if( theSame ){
            newNode = sonsIds[0];
            MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( sonsIds, leaderNode->nodeVar->domainSize() * sizeof( NodeId ) );
        } else {
          newNode = opData.DG3->manager()->checkIsomorphism( leaderNode->nodeVar, sonsIds );
          if( newNode == 0 )
            newNode = opData.DG3->manager()->addNonTerminalNode( leaderNode->nodeVar, sonsIds );
        }

        // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "KEY INSERTION : " );
        opData.conti.setDG1Node( leaderNodeId );
        opData.conti.setDG2Node( followerNodeId );
        // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Context : " << opData.conti.toString() << "Ajout de : " << opData.conti.contextKey() << " -> " << newNode );
        opData.explorationTable->insert( opData.conti.contextKey(), newNode );
        // // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "EQUALS EXPLORATION STATUS : " << *(opData.explorationTable) );
        // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Fin Recur - Noeud leader : " << opData.conti.getDG1Node() << " - Noeud follower : " << opData.conti.getDG2Node() << " - Resulting Node : " << newNode );

      }
    }

    // *******************************************************************************************************

    return newNode;
  }




/// Called when we investigate a node of the first diagram only

  template< typename GUM_SCALAR >
  NodeId
  GUM_MULTI_DIM_DECISION_GRAPH_GO_DOWN_ON_LEADER_FUNCTION( DGNonOrderedOperatorData< GUM_SCALAR >& opData,
                                                             const DiscreteVariable* leaderParentVar,
                                                             std::string tabu ) {

    NodeId newNode = 0;
    // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "GoDownLeader  - Noeud Leader : " << opData.conti.getDG1Node() << " - Noeud Follower : " << opData.conti.getDG2Node() << " - Instantiated Variable : " << opData.DG1->node( opData.conti.getDG1Node() )->nodeVar->toString() );


    NodeId leaderCurrentNodeId = opData.conti.getDG1Node();
    const typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* leaderCurrentNode = opData.DG1->node( leaderCurrentNodeId );
    bool isIndeedRetorgrade = opData.conti.isRetrogradeVar( leaderCurrentNode->nodeVar );

    NodeId* sonsIds = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate( leaderCurrentNode->nodeVar->domainSize() * sizeof( NodeId ) ) );
    bool theSame = true;

    // ********************************************************************************************************
    // For each value the current var take on this node, we have to do our computation
    for ( Idx modality = 0; modality < leaderCurrentNode->nodeVar->domainSize(); modality++ ) {

        // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "GoDownLeader  - Descente sur fils numéro : " << modality );

        // But we have to indicates to possible node on follower diagram, the current value of the var
        if ( isIndeedRetorgrade )
          opData.conti.chgVarModality( leaderCurrentNode->nodeVar, modality + 1 );

        opData.conti.setDG1Node( leaderCurrentNode->nodeSons[modality] );

        sonsIds[ modality ] = GUM_MULTI_DIM_DECISION_GRAPH_RECUR_FUNCTION( opData, leaderCurrentNode->nodeVar, tabu );
        if( sonsIds[ modality ] != sonsIds[ 0 ] )
            theSame = false;
    }

    //*********************************************************************************************************

    // ********************************************************************************************************
    // And we finally add this node to our resulting graph
    if( theSame ){
      newNode = sonsIds[0];
      MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( sonsIds, leaderCurrentNode->nodeVar->domainSize() * sizeof( NodeId ) );
    } else {
      newNode = opData.DG3->manager()->checkIsomorphism( leaderCurrentNode->nodeVar, sonsIds );
      if( newNode == 0 )
        newNode = opData.DG3->manager()->addNonTerminalNode( leaderCurrentNode->nodeVar, sonsIds );
    }

    // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "KEY INSERTION : " );
    opData.conti.setDG1Node( leaderCurrentNodeId );

    if ( isIndeedRetorgrade )
      opData.conti.chgVarModality( leaderCurrentNode->nodeVar, 0 );

    // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Context : " << opData.conti.toString() << "Ajout de : " << opData.conti.contextKey() << " -> " << newNode );
    opData.explorationTable->insert( opData.conti.contextKey(), newNode );
// // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "GODOWNLEADER EXPLORATION STATUS : " << *(opData.explorationTable) );


    // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "GoDownLeader  - Fin GoDownLeader  - Noeud Leader : " << opData.conti.getDG1Node() << " - Noeud Follower : " << opData.conti.getDG2Node() << " - Resulting Node : " << newNode );


    return newNode;
  }




/// Called when we investigate a node of the second diagram only

  template< typename GUM_SCALAR >
  NodeId
  GUM_MULTI_DIM_DECISION_GRAPH_GO_DOWN_ON_FOLLOWER_FUNCTION( DGNonOrderedOperatorData< GUM_SCALAR >& opData,
                                                               const DiscreteVariable* leaderParentVar,
                                                               std::string tabu ) {


    // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "GoDownFollower - Noeud leader : " << opData.conti.getDG1Node() << " - Noeud follower : " << opData.conti.getDG2Node() << " - Instantiated Variable : " << opData.DG2->node( opData.conti.getDG2Node() )->nodeVar->toString() );

    NodeId newNode = 0;

    NodeId followerCurrentNodeId = opData.conti.getDG2Node();
    const typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* followerCurrentNode = opData.DG1->node( followerCurrentNodeId );
    bool isIndeedRetorgrade = opData.conti.isRetrogradeVar( followerCurrentNode->nodeVar );

    if ( isIndeedRetorgrade && opData.conti.variableModality( followerCurrentNode->nodeVar ) != 0 ) {
      // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "GoDownFollower  -  Context : " << opData.conti.toString() )
      // ***************************************************************************************************************
      // If var exists in leader diagram and has already been instantiate to its default value,
      // we have to do so in second diagram too
      Idx varModality = opData.conti.variableModality( followerCurrentNode->nodeVar ) - 1;

      // ***************************************************************************************************************
      // If var exists in leader diagram and has already been instantiate to a value,
      // we have to go down on this value
      // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "GoDownFollower  -  Saut par fils : " << opData.conti.variableModality( followerCurrentNode->nodeVar ) - 1 );

      // But we have to check if value has its arc for this nod
      opData.conti.setDG2Node( followerCurrentNode->nodeSons[ varModality ] );

      newNode = GUM_MULTI_DIM_DECISION_GRAPH_RECUR_FUNCTION( opData, leaderParentVar, tabu );

      opData.conti.setDG2Node( followerCurrentNodeId );
      // ****************************************************************************************************************
    } else {
      // ***************************************************************************************************************
      // If we aren't in one of the above cases, it means that leader diagram hasn't the variable pointed by follower current node.

      NodeId* sonsIds = static_cast<NodeId*>( MultiDimDecisionGraph<GUM_SCALAR>::soa.allocate( followerCurrentNode->nodeVar->domainSize() * sizeof( NodeId ) ) );
      bool theSame = true;

      // In that case we do our computation for all sons of that var
      for ( Idx modality = 0; modality < followerCurrentNode->nodeVar->domainSize(); modality++ ) {

          // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "GoDownFollower  -  Descente sur fils numéro : " << modality );
          opData.conti.setDG2Node( followerCurrentNode->nodeSons[modality] );
          sonsIds[ modality ] = GUM_MULTI_DIM_DECISION_GRAPH_RECUR_FUNCTION( opData, leaderParentVar, tabu );
          if( sonsIds[ modality ] != sonsIds[ 0 ] )
              theSame = false;
      }

      // And we had this node to our graph
      if( theSame ){
        newNode = sonsIds[0];
        MultiDimDecisionGraph<GUM_SCALAR>::soa.deallocate( sonsIds, followerCurrentNode->nodeVar->domainSize() * sizeof( NodeId ) );
      } else {
        newNode = opData.DG3->manager()->checkIsomorphism( followerCurrentNode->nodeVar, sonsIds );
        if( newNode == 0 )
          newNode = opData.DG3->manager()->addNonTerminalNode( followerCurrentNode->nodeVar, sonsIds );
      }

      // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "KEY INSERTION : " );
      opData.conti.setDG2Node( followerCurrentNodeId );
      // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Context : " << opData.conti.toString() << "Ajout de : " << opData.conti.contextKey() << " -> " << newNode );
      opData.explorationTable->insert( opData.conti.contextKey(), newNode );
// // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "GODOWNFOLLOWER EXPLORATION STATUS : " << *(opData.explorationTable) );

    }

    // ****************************************************************************************************************
    // GUM_TRACE("\n" << tabu << std::setprecision( 20 )  << "Fin GoDownFollower - Noeud leader : " << opData.conti.getDG1Node() << " - Noeud follower : " << opData.conti.getDG2Node() << " - Resulting Node : " << newNode );

    return newNode;
  }

}/* end of namespace GUM */

#endif /* GUM_DECISION_GRAPH_OPERATOR_FUNCTION_DEFINITION */
