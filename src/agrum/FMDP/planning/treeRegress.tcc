/****************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                                *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ****************************************************************************/
/**
* @file
* @brief Class used to compute the operation between two decision diagrams
*
* @author Jean-Christophe Magnan
*/

// =======================================================================================
#include <agrum/FMDP/planning/treeRegress.h>
#include <agrum/multidim/decisionGraphUtilities/internalNode.h>
// =======================================================================================

#define ALLOCATE(x) SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x,y) SmallObjectAllocator::instance().deallocate(x,y)

namespace gum {

    /* ***************************************************************************************************************************** */
    /* CONSTRUCTOR                                                                                                                   */
    /* ***************************************************************************************************************************** */
    template <typename GUM_SCALAR,
              template <typename> class COMBINEOPERATOR,
              template <typename> class PROJECTOPERATOR,
              template <typename> class TerminalNodePolicy>
    TreeRegress<GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy>::TreeRegress(
        Set<const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>*> treeSet,
        const GUM_SCALAR neutral ) : __treeSet( treeSet ),
                                     __neutral(neutral),
                                     __combine(),
                                     __project(){
      GUM_CONSTRUCTOR(TreeRegress);
      __rd = new MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>();
      /*__nbVar = 0;
      __default = nullptr;
      __primedVars = primedVars;
      __targetVar = targetVar;*/
    }



    /* ***************************************************************************************************************************** */
    /* DESTRUCTOR                                                                                                                   */
    /* ***************************************************************************************************************************** */
    template <typename GUM_SCALAR,
              template <typename> class COMBINEOPERATOR,
              template <typename> class PROJECTOPERATOR,
              template <typename> class TerminalNodePolicy>
    TreeRegress<GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy>::~TreeRegress( ){

      GUM_DESTRUCTOR(TreeRegress);

    }



    /* ***************************************************************************************************************************** */
    /* Compute                                                                                                                       */
    /*                                                                                                                               */
    /* This function is the main function. To be call every time an operation between the two given Decision Graphs is required      */
    /* ***************************************************************************************************************************** */
    template <typename GUM_SCALAR,
              template <typename> class COMBINEOPERATOR,
              template <typename> class PROJECTOPERATOR,
              template <typename> class TerminalNodePolicy>
    MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy> * TreeRegress<GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy>::compute(){

      NodeId root = __compute( __DG1->, (Idx)0 - 1, "" );
      __rd->manager()->setRootNode( root );

      if(__nbVar != 0 )
        DEALLOCATE( varInst, sizeof(Idx)*__nbVar );

      __rd->erase(*__targetVar);

      return __rd;
    }



    /// Main recursion function, called every time we move on a node to determine what we have to do

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
    template <typename GUM_SCALAR,
              template <typename> class COMBINEOPERATOR,
              template <typename> class PROJECTOPERATOR,
              template <typename> class TerminalNodePolicy>
    NodeId
    TreeRegress<GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy>::__compute(  O4DGContext & currentSituation,
                                                                                        Idx lastInstVarPos,
                                                                                        std::string tab) {

      NodeId newNode = 0;
//      std::cout << "Now computing " << currentSituation.DG1Node() << " " << currentSituation.DG2Node() << std::endl;

      // ******************************************************************************************************
      // If both current nodes are terminal,
      // we only have to compute the resulting value
      if ( __DG1->isTerminalNode ( currentSituation.DG1Node() ) && __DG2->isTerminalNode ( currentSituation.DG2Node() ) ) {

        // We have to compute new valueand we insert a new node in diagram with this value, ...
        GUM_SCALAR newVal = __neutral;
        GUM_SCALAR tempVal = __combine( __DG1->nodeValue(currentSituation.DG1Node()),
                                        __DG2->nodeValue(currentSituation.DG2Node()));
        for( Idx targetModa = 0; targetModa < __targetVar->domainSize(); ++targetModa)
          newVal = __project( newVal, tempVal );
        return __rd->manager()->addTerminalNode( newVal );
      }


      // ******************************************************************************************************



      // ******************************************************************************************************
      // If not,
      // we'll have to do some exploration

      //First we ensure that we hadn't already visit this pair of node under hte same circumstances

      short int* dg1NeededVar = __DG1InstantiationNeeded.exists( currentSituation.DG1Node() )?__DG1InstantiationNeeded[ currentSituation.DG1Node() ]:__default;
      Idx dg1CurrentVarPos = __DG1->isTerminalNode( currentSituation.DG1Node() )?__nbVar:__rd->variablesSequence().pos( __DG1->node( currentSituation.DG1Node() )->nodeVar() );
      short int* dg2NeededVar = __DG2InstantiationNeeded.exists( currentSituation.DG2Node() )?__DG2InstantiationNeeded[ currentSituation.DG2Node() ]:__default;
      Idx dg2CurrentVarPos = __DG2->isTerminalNode( currentSituation.DG2Node() )?__nbVar:__rd->variablesSequence().pos( __DG2->node( currentSituation.DG2Node() )->nodeVar() );

//        std::cout << "MDGO 348" << std::endl;
      short int* instNeeded = static_cast<short int*>( ALLOCATE( sizeof(short int)*__nbVar ) );

      for( Idx i = 0; i < __nbVar; i++ ){

        instNeeded[i] = dg1NeededVar[i] + dg2NeededVar[i];
      }

      double curSitKey = currentSituation.key( instNeeded );

      if ( __explorationTable.exists ( curSitKey ) ){
        DEALLOCATE( instNeeded, sizeof(short int)*__nbVar);

        return __explorationTable[ curSitKey ];
      }

      // ====================================================

      NodeId origDG1 = currentSituation.DG1Node(), origDG2 = currentSituation.DG2Node();

      const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>* leaddg = nullptr;
      NodeId leadNodeId = 0;
      Idx leadVarPos = __rd->variablesSequence().size();
      typedef void (O4DGContext::*SetNodeFunction)(const NodeId&);
      SetNodeFunction leadFunction = nullptr;

      bool sameVar = false;

      if ( ! __DG1->isTerminalNode ( currentSituation.DG1Node() ) ){

        if( currentSituation.varModality( dg1CurrentVarPos ) != 0 ){
          // If var associated to current node has already been instanciated, we have to jump it
          currentSituation.setDG1Node( __DG1->node( currentSituation.DG1Node() )->son( currentSituation.varModality( dg1CurrentVarPos ) - 1 ) );

          newNode = __compute(currentSituation, lastInstVarPos, tab );
          __explorationTable.insert( curSitKey, newNode);
          currentSituation.setDG1Node(origDG1);
          currentSituation.setDG2Node(origDG2);

          DEALLOCATE( instNeeded, sizeof(short int)*__nbVar);

          return newNode;
        }

        leaddg = __DG1;
        leadNodeId = currentSituation.DG1Node();
        leadVarPos = dg1CurrentVarPos;
        leadFunction = &O4DGContext::setDG1Node;
      }

      if ( ! __DG2->isTerminalNode ( currentSituation.DG2Node() ) ){

        if( currentSituation.varModality( dg2CurrentVarPos ) != 0 ){
          // If var associated to current node has already been instanciated, we have to jump it
          currentSituation.setDG2Node( __DG2->node( currentSituation.DG2Node() )->son( currentSituation.varModality( dg2CurrentVarPos ) - 1 ) );

          newNode = __compute(currentSituation, lastInstVarPos, tab );
          __explorationTable.insert( curSitKey, newNode);
          currentSituation.setDG1Node(origDG1);
          currentSituation.setDG2Node(origDG2);

          DEALLOCATE( instNeeded, sizeof(short int)*__nbVar);

          return newNode;
        }

        if( leadVarPos == dg2CurrentVarPos ){
          sameVar = true;
        }

        if( leadVarPos > dg2CurrentVarPos){
          leaddg = __DG2;
          leadNodeId = currentSituation.DG2Node();
          leadVarPos = dg2CurrentVarPos;
          leadFunction = &O4DGContext::setDG2Node;
        }
      }

      // ====================================================
      // Anticipated Exploration

      // Before exploring nodes, we have to ensure that every anticipated exploration is done
      for ( Idx varPos = lastInstVarPos + 1; varPos < leadVarPos; ++varPos ) {

        if( instNeeded[ varPos ] ){

          const DiscreteVariable* curVar = __rd->variablesSequence().atPos( varPos );
//                std::cout << "MDGO 433" << std::endl;
          NodeId* sonsIds = static_cast<NodeId*>( ALLOCATE( sizeof(NodeId)*curVar->domainSize() ) );

          for ( Idx modality = 0; modality < curVar->domainSize(); modality++ ) {

            currentSituation.chgVarModality ( varPos, modality + 1 );

            sonsIds[ modality ] = __compute( currentSituation, varPos, tab );
          }

          newNode = __rd->manager()->nodeRedundancyCheck( curVar, sonsIds );

          __explorationTable.insert ( curSitKey, newNode );
          currentSituation.chgVarModality ( varPos, 0 );
          currentSituation.setDG1Node(origDG1);
          currentSituation.setDG2Node(origDG2);

          DEALLOCATE( instNeeded, sizeof(short int)*__nbVar);

          return newNode;
        }
      }

      // ====================================================
      // Terminal Exploration
      if( sameVar && __DG1->node(origDG1)->nodeVar() == __targetVar ){
//        std::cout << "Same here!" << std::endl;
        GUM_SCALAR newVal = __neutral;
        for( Idx targetModa = 0; targetModa < __targetVar->domainSize(); ++targetModa)
          newVal = __project( newVal, __combine( __DG1->nodeValue( __DG1->node(origDG1)->son(targetModa) ),
                                                 __DG2->nodeValue( __DG2->node(origDG2)->son(targetModa) ) ) );
        NodeId newNode = __rd->manager()->addTerminalNode( newVal );
        __explorationTable.insert ( curSitKey, newNode );
        DEALLOCATE( instNeeded, sizeof(short int)*__nbVar);
        return newNode;
      }
      if( __DG1->isTerminalNode(origDG1) ){
//        std::cout << "DG1 term";
        if( __DG2->node(origDG2)->nodeVar() == __targetVar ){
//          std::cout << "&& DG2 target" << std::endl;
          GUM_SCALAR newVal = __neutral;
          for( Idx targetModa = 0; targetModa < __targetVar->domainSize(); ++targetModa)
            newVal = __project( newVal, __combine( __DG1->nodeValue( origDG1 ),
                                                   __DG2->nodeValue( __DG2->node(origDG2)->son(targetModa) ) ) );
          NodeId newNode = __rd->manager()->addTerminalNode( newVal );
          __explorationTable.insert ( curSitKey, newNode );
          DEALLOCATE( instNeeded, sizeof(short int)*__nbVar);
          return newNode;
        }
//        std::cout << std::endl;
      } else {
        if( __DG1->node(origDG1)->nodeVar() == __targetVar && __DG2->isTerminalNode(origDG2) ){
//          std::cout << "DG1 target && DG2 term" << std::endl;
          GUM_SCALAR newVal = __neutral;
          for( Idx targetModa = 0; targetModa < __targetVar->domainSize(); ++targetModa)
            newVal = __project( newVal, __combine( __DG1->nodeValue( __DG1->node(origDG1)->son(targetModa) ),
                                                   __DG2->nodeValue( origDG2 ) ) );
          NodeId newNode = __rd->manager()->addTerminalNode( newVal );
          __explorationTable.insert ( curSitKey, newNode );
          DEALLOCATE( instNeeded, sizeof(short int)*__nbVar);
          return newNode;
        }
      }


      // ====================================================
      // Normal Exploration

      // If only one of the current node is terminal,
      // we have to pursue deeper on the other diagram
      if( sameVar ){

        // If so - meaning it's the same variable - we have to go
        // down on both
        const InternalNode* dg1Node = __DG1->node( origDG1 );
        const InternalNode* dg2Node = __DG2->node( origDG2 );

        const DiscreteVariable* curVar = dg1Node->nodeVar();
        Idx varPos = __rd->variablesSequence().pos( curVar );
//            std::cout << "MDGO 469" << std::endl;
        NodeId* sonsIds = static_cast<NodeId*>( ALLOCATE( sizeof(NodeId)*curVar->domainSize() ) );

        for ( Idx modality = 0; modality < curVar->domainSize(); modality++ ) {

          currentSituation.chgVarModality ( varPos, modality + 1 );
          currentSituation.setDG1Node( dg1Node->son( modality ) );
          currentSituation.setDG2Node( dg2Node->son( modality ) );

          sonsIds[ modality ] = __compute( currentSituation, varPos, tab );
        }

        newNode = __rd->manager()->nodeRedundancyCheck( curVar, sonsIds );

        __explorationTable.insert ( curSitKey, newNode );
        currentSituation.chgVarModality ( varPos, 0 );
        currentSituation.setDG1Node(origDG1);
        currentSituation.setDG2Node(origDG2);

        DEALLOCATE( instNeeded, sizeof(short int)*__nbVar);

        return newNode;
      }
      // ====================================================
      else {


        const InternalNode* leaddgNode = leaddg->node( leadNodeId );

        const DiscreteVariable* curVar = leaddgNode->nodeVar();
//            std::cout << "MDGO 499" << std::endl;
        NodeId* sonsIds = static_cast<NodeId*>( ALLOCATE( sizeof(NodeId)*curVar->domainSize() ) );

        for ( Idx modality = 0; modality < curVar->domainSize(); modality++ ) {
          currentSituation.chgVarModality ( leadVarPos, modality + 1 );
          (currentSituation.*leadFunction)( leaddgNode->son( modality ) );

          sonsIds[ modality ] = __compute( currentSituation, leadVarPos, tab );
        }

        newNode = __rd->manager()->nodeRedundancyCheck( curVar, sonsIds );

        __explorationTable.insert ( curSitKey, newNode );
        currentSituation.chgVarModality ( leadVarPos, 0 );
        currentSituation.setDG1Node(origDG1);
        currentSituation.setDG2Node(origDG2);

        DEALLOCATE( instNeeded, sizeof(short int)*__nbVar);

        return newNode;
      }
    }

} // namespace gum

