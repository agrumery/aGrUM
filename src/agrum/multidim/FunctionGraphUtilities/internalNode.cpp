/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                              //
 *                                                                        //
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or    //
 *   (at your option) any later version.                                  //
 *                                                                        //
 *   This program is distributed in the hope that it will be useful,      //
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       //
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
 *   GNU General Public License for more details.                         //
 *                                                                        //
 *   You should have received a copy of the GNU General Public License    //
 *   along with this program; if not, write to the                        //
 *   Free Software Foundation, Inc.,                                      //
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.            //
 ***************************************************************************/

/**
 * @file
 * @brief Methods of the MultiDimFunctionGraph InternalNode class.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 * @author Jean-Christophe Magnan
 */

#include <agrum/multidim/FunctionGraphUtilities/internalNode.h>

#define ALLOCATE( x ) SmallObjectAllocator::instance().allocate( x )
#define DEALLOCATE( x, y ) SmallObjectAllocator::instance().deallocate( x, y )

namespace gum {

  // ============================================================================
  // Constructors
  // ============================================================================
  InternalNode::InternalNode() {
    GUM_CONSTRUCTOR( InternalNode )
    __nodeVar = nullptr;
  }

  InternalNode::InternalNode( const DiscreteVariable* v ) {
    GUM_CONSTRUCTOR( InternalNode )
    __setNodeVar( v );
  }

  InternalNode::InternalNode( const DiscreteVariable* v, NodeId* sons ) {
    GUM_CONSTRUCTOR( InternalNode )
    __nodeVar = v;
    __nodeSons = sons;
  }

  // ============================================================================
  // Destructors
  // ============================================================================
  InternalNode::~InternalNode() {
    GUM_DESTRUCTOR( InternalNode )
    if ( __nodeVar != nullptr ) deallocateNodeSons( __nodeVar, __nodeSons );
  }

  // ============================================================================
  // Allocates a table of nodeid of the size given in parameter
  // ============================================================================
  NodeId* InternalNode::allocateNodeSons( const DiscreteVariable* v ) {
    NodeId* sons =
        static_cast<NodeId*>( ALLOCATE( sizeof( NodeId ) * v->domainSize() ) );
    for ( gum::Idx i = 0; i < v->domainSize(); ++i )
      sons[i] = 0;
    return sons;
  }

  // ============================================================================
  // Deallocates a NodeSons table
  // ============================================================================
  void InternalNode::deallocateNodeSons( const DiscreteVariable* v, NodeId* s ) {
    DEALLOCATE( s, sizeof( NodeId ) * v->domainSize() );
  }

  // ============================================================================
  // Node handlers
  // ============================================================================
  void InternalNode::setNode( const DiscreteVariable* v, NodeId* sons ) {
    if ( __nodeVar != nullptr ) deallocateNodeSons( __nodeVar, __nodeSons );
    __nodeVar = v;
    __nodeSons = sons;
  }

  // ============================================================================
  // Var handlers
  // ============================================================================
  void InternalNode::setNodeVar( const DiscreteVariable* v ) {
    if ( __nodeVar != nullptr ) deallocateNodeSons( __nodeVar, __nodeSons );
    __setNodeVar( v );
  }

  void InternalNode::__setNodeVar( const DiscreteVariable* v ) {
    __nodeVar = v;
    __nodeSons = allocateNodeSons( v );
  }

  // ============================================================================
  // Parent handlers
  // ============================================================================
  void InternalNode::addParent( NodeId parent, Idx modality ) {
    __nodeParents.addLink( Parent( parent, modality ) );
  }

  void InternalNode::removeParent( NodeId parent, Idx modality ) {
    __nodeParents.searchAndRemoveLink( Parent( parent, modality ) );
  }

}  // namespace gum

#ifdef GUM_NO_INLINE
#include <agrum/multidim/FunctionGraphUtilities/internalNode_inl.h>
#endif /* GUM_NO_INLINE */
