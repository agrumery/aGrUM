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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/multidim/FunctionGraphUtilities/internalNode.h>
#include <agrum/multidim/FunctionGraphUtilities/operators/treeOperator.h>

#define ALLOCATE( x ) SmallObjectAllocator::instance().allocate( x )
#define DEALLOCATE( x, y ) SmallObjectAllocator::instance().deallocate( x, y )

namespace gum {

  template <typename GUM_SCALAR,
            template <typename> class COMBINEOPERATOR,
            template <typename> class TerminalNodePolicy>
  INLINE
  TreeOperator<GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy>::TreeOperator(
      const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* dt1,
      const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* dt2 )
      : __dt1( dt1 )
      , __dt2( dt2 )
      , __combine() {
    GUM_CONSTRUCTOR( TreeOperator );

    __rd =
        MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::getTreeInstance();
  }

  template <typename GUM_SCALAR,
            template <typename> class COMBINEOPERATOR,
            template <typename> class TerminalNodePolicy>
  INLINE
  TreeOperator<GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy>::TreeOperator(
      const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* dt1,
      const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* dt2,
      const HashTable<const DiscreteVariable*, Idx>                givenContext )
      : __dt1( dt1 )
      , __dt2( dt2 )
      , __combine()
      , __context( givenContext ) {
    GUM_CONSTRUCTOR( TreeOperator );

    __rd =
        MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>::getTreeInstance();
  }

  template <typename GUM_SCALAR,
            template <typename> class COMBINEOPERATOR,
            template <typename> class TerminalNodePolicy>
  INLINE TreeOperator<GUM_SCALAR,
                      COMBINEOPERATOR,
                      TerminalNodePolicy>::~TreeOperator() {

    GUM_DESTRUCTOR( TreeOperator );
  }

  // This function is the main function. To be call every time an operation
  // between the two given Function Graphs is required
  template <typename GUM_SCALAR,
            template <typename> class COMBINEOPERATOR,
            template <typename> class TerminalNodePolicy>
  INLINE MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>*
         TreeOperator<GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy>::compute() {

    __rd->manager()->setRootNode( __xPloreDT1( __dt1->root() ) );

    return __rd;
  }

  // Main recursion function, called every time we move on a node to determine
  // what we have to do
  template <typename GUM_SCALAR,
            template <typename> class COMBINEOPERATOR,
            template <typename> class TerminalNodePolicy>
  INLINE NodeId
         TreeOperator<GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy>::__xPloreDT1(
      NodeId currentNodeId ) {

    if ( __dt1->isTerminalNode( currentNodeId ) ) {
      __curDT1Leaf = currentNodeId;
      return __xPloreDT2( __dt2->root() );
    }

    const InternalNode* currentNode = __dt1->node( currentNodeId );

    if ( !__rd->variablesSequence().exists( currentNode->nodeVar() ) )
      __rd->add( *( currentNode->nodeVar() ) );

    NodeId* sonsMap = static_cast<NodeId*>(
        ALLOCATE( sizeof( NodeId ) * currentNode->nodeVar()->domainSize() ) );
    for ( Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda ) {
      __context.insert( currentNode->nodeVar(), moda );
      sonsMap[moda] = __xPloreDT1( currentNode->son( moda ) );
      __context.erase( currentNode->nodeVar() );
    }
    return __checkRedundancy( currentNode->nodeVar(), sonsMap );
  }

  template <typename GUM_SCALAR,
            template <typename> class COMBINEOPERATOR,
            template <typename> class TerminalNodePolicy>
  INLINE NodeId
         TreeOperator<GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy>::__xPloreDT2(
      NodeId currentNodeId ) {

    if ( __dt2->isTerminalNode( currentNodeId ) )
      return __rd->manager()->addTerminalNode( __combine(
          __dt1->nodeValue( __curDT1Leaf ), __dt2->nodeValue( currentNodeId ) ) );

    const InternalNode* currentNode = __dt2->node( currentNodeId );

    if ( !__rd->variablesSequence().exists( currentNode->nodeVar() ) )
      __rd->add( *( currentNode->nodeVar() ) );

    if ( __context.exists( currentNode->nodeVar() ) )
      return __xPloreDT2( currentNode->son( __context[currentNode->nodeVar()] ) );

    NodeId* sonsMap = static_cast<NodeId*>(
        ALLOCATE( sizeof( NodeId ) * currentNode->nodeVar()->domainSize() ) );
    for ( Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda ) {
      __context.insert( currentNode->nodeVar(), moda );
      sonsMap[moda] = __xPloreDT2( currentNode->son( moda ) );
      __context.erase( currentNode->nodeVar() );
    }
    return __checkRedundancy( currentNode->nodeVar(), sonsMap );
  }

  template <typename GUM_SCALAR,
            template <typename> class COMBINEOPERATOR,
            template <typename> class TerminalNodePolicy>
  INLINE NodeId
         TreeOperator<GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy>::__checkRedundancy(
      const DiscreteVariable* var, NodeId* sonsMap ) {
    bool diff = false;
    for ( Idx moda = 1; moda < var->domainSize() && !diff; ++moda )
      if ( sonsMap[0] != sonsMap[moda] ) diff = true;

    if ( !diff ) {
      NodeId zero = sonsMap[0];
      DEALLOCATE( sonsMap, sizeof( NodeId ) * var->domainSize() );
      return zero;
    }

    return __rd->manager()->addInternalNode( var, sonsMap );
  }

}  // namespace gum
