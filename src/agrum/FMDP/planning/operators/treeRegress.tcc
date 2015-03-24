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
#include <agrum/FMDP/planning/operators/treeRegress.h>
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
              const MultiDimDecisionGraph< GUM_SCALAR, TerminalNodePolicy>* qAction,
              const Bijection<const DiscreteVariable*, const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy> *> pxi,
              const Sequence<const DiscreteVariable*> xip,
              const GUM_SCALAR neutral ) :  __vFunc( qAction ),
                                            __pxi(pxi),
                                            __xip(xip),
                                            __neutral(neutral),
                                            __combine(),
                                            __project() {
      GUM_CONSTRUCTOR(TreeRegress);

      __rd = new MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>();

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
    MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy> *
    TreeRegress<GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy>::compute(){

      __rd->manager()->setRootNode( __xPlorePxi(__pxi.second( __xip.atPos(0) ), __pxi.second( __xip.atPos(0) )->root(), 0) );

      return __rd;
    }



    /// Main recursion function, called every time we move on a node to determine what we have to do
    template <typename GUM_SCALAR,
              template <typename> class COMBINEOPERATOR,
              template <typename> class PROJECTOPERATOR,
              template <typename> class TerminalNodePolicy>
    NodeId
    TreeRegress<GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy>::__xPlorePxi(
                        const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy> *xPloredTree,
                        NodeId currentNodeId,
                        Idx seqPos ){

      if( xPloredTree->isTerminalNode(currentNodeId) || __xip.exists(xPloredTree->node(currentNodeId)->nodeVar()) ){
        __curLeafMap.insert(xPloredTree, currentNodeId);
        NodeId nody;
        if( seqPos == __xip.size() - 1 ){
          nody = __rd->manager()->addTerminalNode( __xPloreVFunc( __vFunc->root() ) );
        } else {
          const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>* nextTree = __pxi.second( __xip.atPos(seqPos + 1) );
          nody = __xPlorePxi( nextTree, nextTree->root(), seqPos + 1);
        }
        __curLeafMap.erase(xPloredTree);
        return nody;
      }

      const InternalNode* currentNode = xPloredTree->node(currentNodeId);

      if( !__rd->variablesSequence().exists(currentNode->nodeVar()) )
        __rd->add(*(currentNode->nodeVar()));

      if( __context.exists(currentNode->nodeVar()) )
        return __xPlorePxi( xPloredTree, currentNode->son( __context[currentNode->nodeVar()] ), seqPos);

      NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE( sizeof(NodeId)*currentNode->nodeVar()->domainSize() ) );
      for( Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda ){
        __context.insert(currentNode->nodeVar(), moda);
        sonsMap[moda] = __xPlorePxi( xPloredTree, currentNode->son( moda ), seqPos);
        __context.erase(currentNode->nodeVar());
      }
      return __checkRedundancy(currentNode->nodeVar(), sonsMap);
    }



    template <typename GUM_SCALAR,
              template <typename> class COMBINEOPERATOR,
              template <typename> class PROJECTOPERATOR,
              template <typename> class TerminalNodePolicy>
    GUM_SCALAR
    TreeRegress<GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy>::__xPloreVFunc( NodeId currentNodeId ){

      if( __vFunc->isTerminalNode(currentNodeId) ){
        GUM_SCALAR val = __vFunc->nodeValue(currentNodeId);
        for( HashTableConstIteratorSafe<const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>*, NodeId> leafIter = __curLeafMap.cbeginSafe();
             leafIter != __curLeafMap.cendSafe(); ++leafIter ){
          if( __context.exists( __pxi.first(leafIter.key()) ) ){
            if( leafIter.key()->isTerminalNode(leafIter.val()) )
              val = __combine(val, leafIter.key()->nodeValue(leafIter.val()));
            else {
              const InternalNode* currentNode = leafIter.key()->node(leafIter.val());
              NodeId sonId = currentNode->son( __context[currentNode->nodeVar()] );
              val = __combine(val, leafIter.key()->nodeValue(sonId));
            }
          }
        }
        return val;
      }

      const InternalNode* currentNode = __vFunc->node(currentNodeId);
      GUM_SCALAR val = __neutral;
      for( Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda ){
        __context.insert(currentNode->nodeVar(), moda);
        val = __project( val, __xPloreVFunc( currentNode->son( moda ) ));
        __context.erase(currentNode->nodeVar());
      }
      return val;

    }

    template <typename GUM_SCALAR,
              template <typename> class COMBINEOPERATOR,
              template <typename> class PROJECTOPERATOR,
              template <typename> class TerminalNodePolicy>
    NodeId
    TreeRegress<GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy>::__checkRedundancy( const DiscreteVariable* var, NodeId* sonsMap ){
      bool diff = false;
      for(Idx moda = 1; moda < var->domainSize() && !diff; ++moda)
        if(sonsMap[0] != sonsMap[moda])
          diff = true;

      if(!diff){
        NodeId zero = sonsMap[0];
        DEALLOCATE(sonsMap, sizeof(NodeId)*var->domainSize());
        return zero;
      }

      return __rd->manager()->addNonTerminalNode(var, sonsMap);
    }

} // namespace gum

