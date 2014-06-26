
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
 * @brief Template methods of MultiDimDecisionGraph.
 *
 * @author Jean-Christophe Magnan
 *
 */
#include <agrum/multidim/multiDimDecisionGraph.h>

namespace gum {

// ############################################################################
// Internal Nodes methods
// ############################################################################
    // ============================================================================
    /// Constructors
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::InternalNode(){
      __nodeVar = nullptr;
      __nodeParents = nullptr;
    }

    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::InternalNode(const DiscreteVariable* v){
      _setNodeVar(v);
      __nodeParents = nullptr;
    }

    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::InternalNode(const DiscreteVariable* v, NodeId* sons){
        __nodeVar = v;
        __nodeSons = sons;
        __nodeParents = nullptr;
    }

    // ============================================================================
    /// Destructors
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::~InternalNode(){
        if( __nodeVar != nullptr )
            deallocateNodeSons(__nodeVar, __nodeSons);
        MultiDimDecisionGraph<GUM_SCALAR>::_deletePICL(__nodeParents);
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void* MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::operator new(size_t t){
      return soa.allocate(sizeof(InternalNode));
    }

    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::operator delete(void* in){
      soa.deallocate(in, sizeof(InternalNode));
    }

    // ============================================================================
    /// Allocates a table of nodeid of the size given in parameter
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    NodeId* MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::allocateNodeSons( const DiscreteVariable* v ){
//        std::cout << "Internal Node 92" << std::endl;
      NodeId* sons = static_cast<NodeId*>( soa.allocate( sizeof(NodeId)*v->domainSize() ) );
      for( gum::Idx i = 0; i < v->domainSize(); ++i)
        sons[i] = 0;
      return sons;
    }

    // ============================================================================
    /// Deallocates a NodeSons table
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::deallocateNodeSons(const DiscreteVariable* v, NodeId* s){
        soa.deallocate(s, sizeof(NodeId)*v->domainSize());
    }

    // ============================================================================
    /// Node handlers
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::setNode(const DiscreteVariable* v, NodeId* sons){
        if( __nodeVar != nullptr )
            deallocateNodeSons(__nodeVar, __nodeSons);
        __nodeVar = v;
        __nodeSons = sons;
    }

    // ============================================================================
    /// Var handlers
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::setNodeVar(const DiscreteVariable* v){
        if( __nodeVar != nullptr )
            deallocateNodeSons(__nodeVar, __nodeSons);
        _setNodeVar(v);
    }

    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::_setNodeVar(const DiscreteVariable* v){
        __nodeVar = v;
        __nodeSons = allocateNodeSons( v );
    }

    // ============================================================================
    /// Sons handlers
    // ============================================================================
//    template<typename GUM_SCALAR>
//    INLINE
//    void MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::setSons(NodeId* sons){

//    }

    // ============================================================================
    /// Parent handlers
    // ============================================================================
    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::addParent( NodeId parent, Idx modality ){
        MultiDimDecisionGraph::_addElemToPICL(&__nodeParents, parent, modality );
    }

    template<typename GUM_SCALAR>
    INLINE
    void MultiDimDecisionGraph<GUM_SCALAR>::InternalNode::removeParent( NodeId parent, Idx modality ){
        MultiDimDecisionGraph::_removeElemFromPICL(&__nodeParents, parent, modality );
    }

    //PICLElem*& parents() const {return __parentList;}
} // namespace gum
