
/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of the MultiDimFunctionGraph InternalNode class.
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_INTERNAL_NODE_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_INTERNAL_NODE_H

// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// ============================================================================
#include <agrum/graphs/graphElements.h>
// ============================================================================
#include <agrum/multidim/FunctionGraphUtilities/link.h>
// ============================================================================
#include <agrum/variables/discreteVariable.h>
// ============================================================================

namespace gum {

  struct Parent {
    NodeId parentId;
    Idx modality;

    Parent( NodeId pi, Idx m )
        : parentId( pi )
        , modality( m ) {}
    Parent( const Parent& src )
        : parentId( src.parentId )
        , modality( src.modality ) {}

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate( p, sizeof( Parent ) );
    }

    bool operator==( const Parent& comp ) const {
      if ( comp.parentId == this->parentId && comp.modality == this->modality )
        return true;
      return false;
    }
    bool operator!=( const Parent& comp ) const { return !( comp == *this ); }

    Parent& operator=( const Parent& src ) {
      this->parentId = src.parentId;
      this->modality = src.modality;
      return *this;
    }
  };

  /**
   * @class InternalNode internalNode.h
   * <agrum/multidim/FunctionGraphUtilities/internalNode.h>
   *
   * @brief Structure used to represent a node internal structure
   *
   * @ingroup multidim_group
   */
  class InternalNode {

    // ============================================================================
    /// Variable associated to such node
    // ============================================================================
    const DiscreteVariable* __nodeVar;

    // ============================================================================
    /**
     * Table of sons of the node.
     * Each son is bound to a modality of the variable.
     * So those modalities are used has indexes for that table.
     * _____________________________
     * |      |      |      |      |
     * | son1 | son2 | son3 | son4 |
     * |______|______|______|______|
     *    x1     x2     x3     x4
     */
    // ============================================================================
    NodeId* __nodeSons;

    LinkedList<Parent> __nodeParents;

    public:
    // #############################################################################################
    /// Constructors and Destructors
    // #############################################################################################
    /// @{

    // ============================================================================
    /// Default Constructor
    /// Creates an empty node with no variable attached.
    // ============================================================================
    InternalNode();

    // ============================================================================
    /// Constructor
    /// Creates a node and attached the specified variable.
    /// Initializes the sons.
    // ============================================================================
    InternalNode( const DiscreteVariable* v );

    // ============================================================================
    /// Constructor
    /// Creates a node and attached the specified variable.
    /// Also attached the given on map (and will handle it by itself especially
    /// deallocate it)
    ///
    /// @warning You'd better known what you're doing if you're using this one.
    /// sons must have the size of domainSize of v or the program will fail!
    // ============================================================================
    InternalNode( const DiscreteVariable* v, NodeId* sons );

    // ============================================================================
    /// Destructors
    // ============================================================================
    ~InternalNode();

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate( p, sizeof( InternalNode ) );
    }

    /// @}

    // #############################################################################################
    /// Node handlers
    // #############################################################################################
    /// @{

    // ============================================================================
    /// Allows you to respecify the node, changing its attached variable as well
    /// as its son map
    ///
    /// @warning You'd better known what you're doing if you're using this one.
    /// sons must have the size of domainSize of v or the program will fail!
    // ============================================================================
    void setNode( const DiscreteVariable* v, NodeId* sons );

    /// @}

    // #############################################################################################
    /// Var handlers
    // #############################################################################################
    /// @{

    void setNodeVar( const DiscreteVariable* v );

    void _setNodeVar( const DiscreteVariable* v );

    const DiscreteVariable* nodeVar() const { return __nodeVar; }

    /// @}

    // #############################################################################################
    /// Sons handlers
    // #############################################################################################
    /// @{

    void setSon( Idx modality, NodeId son ) { __nodeSons[modality] = son; }

    NodeId son( Idx modality ) const { return __nodeSons[modality]; }


    Idx nbSons() const { return __nodeVar->domainSize(); }

    /// @}

    // #############################################################################################
    /// Parent handlers
    // #############################################################################################
    /// @{

    void addParent( NodeId parent, Idx modality );

    void removeParent( NodeId parent, Idx modality );

    Link<Parent>* parents() { return __nodeParents.list(); }

    const Link<Parent>* parents() const { return __nodeParents.list(); }

    /// @}

    // ============================================================================
    /// Allocates a table of nodeid of the size given in parameter
    // ============================================================================
    static NodeId* allocateNodeSons( const DiscreteVariable* v );

    // ============================================================================
    /// Deallocates a NodeSons table
    // ============================================================================
    static void deallocateNodeSons( const DiscreteVariable* v, NodeId* s );
  };

}  // End of namespace gum

#endif /* GUM_MULTI_DIM_FUNCTION_GRAPH_INTERNAL_NODE_H */
