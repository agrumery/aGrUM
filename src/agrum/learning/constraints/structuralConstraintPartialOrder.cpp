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
/** @file
 * @brief the structural constraint imposing a partial order over nodes
 *
 * In DBNs, it is forbidden to add arcs from nodes at time slice t to nodes at
 * time slice s, where s < t. This class allows for taking this kind of constraint
 * into account by imposing a partial order over the nodes: arcs (X,Y) can then
 * only be added if X <= Y in the partial order.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/constraints/structuralConstraintPartialOrder.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/constraints/structuralConstraintPartialOrder.inl>
#endif /* GUM_NO_INLINE */


namespace gum {


  namespace learning {


    /// default constructor
    StructuralConstraintPartialOrder::StructuralConstraintPartialOrder () {
      GUM_CONSTRUCTOR ( StructuralConstraintPartialOrder );
    }


    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintPartialOrder::StructuralConstraintPartialOrder
    ( const NodeProperty<unsigned int>& order ) {
      for ( auto iter = order.cbegin(); iter != order.cend (); ++iter ) {
        _DiGraph__graph.addNode ( iter.key () );
      }
      _PartialOrder__order = order;

      GUM_CONSTRUCTOR ( StructuralConstraintPartialOrder );
    }


    /// constructor starting with a given graph
    StructuralConstraintPartialOrder::StructuralConstraintPartialOrder
    ( const DiGraph& graph,
      const NodeProperty<unsigned int>& order ) :
      StructuralConstraintDiGraph ( graph ) {
      // check that each node has an appropriate time slice
      if ( order.size () != graph.size () ) {
        GUM_ERROR ( SizeError,
                    "the graph and the partial order do not have the same size" );
      }
      for ( const auto id : graph ) {
        if ( ! order.exists ( id ) ) {
          GUM_ERROR ( InvalidNode, "there exists a node in the graph that does "
                      "not belong to the partial order" );
        }
      }
      _PartialOrder__order = order;

      GUM_CONSTRUCTOR ( StructuralConstraintPartialOrder );
    }


    /// copy constructor
    StructuralConstraintPartialOrder::StructuralConstraintPartialOrder
    ( const StructuralConstraintPartialOrder& from ) :
      StructuralConstraintDiGraph ( from ),
      _PartialOrder__order ( from._PartialOrder__order ),
      _PartialOrder__default_slice ( from._PartialOrder__default_slice ) {
      GUM_CONS_CPY ( StructuralConstraintPartialOrder );
    }


    /// move constructor
    StructuralConstraintPartialOrder::StructuralConstraintPartialOrder
    ( StructuralConstraintPartialOrder&& from ) :
      StructuralConstraintDiGraph ( std::move ( from ) ),
      _PartialOrder__order ( std::move ( from._PartialOrder__order ) ),
      _PartialOrder__default_slice
      ( std::move ( from._PartialOrder__default_slice ) ) {
      GUM_CONS_MOV ( StructuralConstraintPartialOrder );
    }


    /// destructor
    StructuralConstraintPartialOrder::~StructuralConstraintPartialOrder () {
      GUM_DESTRUCTOR ( StructuralConstraintPartialOrder );
    }


    /// copy operator
    StructuralConstraintPartialOrder&
    StructuralConstraintPartialOrder::operator=
    ( const StructuralConstraintPartialOrder& from ) {
      if ( this != &from ) {
        StructuralConstraintDiGraph::operator= ( from );
        _PartialOrder__order = from._PartialOrder__order;
        _PartialOrder__default_slice = from._PartialOrder__default_slice;
      }
      return *this;
    }


    /// move operator
    StructuralConstraintPartialOrder&
    StructuralConstraintPartialOrder::operator=
    ( StructuralConstraintPartialOrder&& from ) {
      if ( this != &from ) {
        StructuralConstraintDiGraph::operator= ( std::move ( from ) );
        _PartialOrder__order = std::move ( from._PartialOrder__order );
        _PartialOrder__default_slice =
          std::move ( from._PartialOrder__default_slice );
      }
      return *this;
    }


  } /* namespace learning */


} /* namespace gum */

