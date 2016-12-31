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
 * @brief Source implementation of Base classes for oriented graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/graphs/diGraph.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/diGraph_inl.h>
#endif  // GUM_NOINLINE

namespace gum {

  DiGraph::DiGraph( Size nodes_size,
                    bool nodes_resize_policy,
                    Size arcs_size,
                    bool arcs_resize_policy )
      : NodeGraphPart( nodes_size, nodes_resize_policy )
      , ArcGraphPart( arcs_size, arcs_resize_policy )
      , __mutableTopologicalOrder( nullptr ) {
    GUM_CONSTRUCTOR( DiGraph );
  }

  DiGraph::DiGraph( const DiGraph& g )
      : NodeGraphPart( g )
      , ArcGraphPart( g )
      , __mutableTopologicalOrder( nullptr ) {
    GUM_CONS_CPY( DiGraph );
    if ( g.__mutableTopologicalOrder != nullptr ) {
      __mutableTopologicalOrder =
          new Sequence<NodeId>( *( g.__mutableTopologicalOrder ) );
    }
  }

  DiGraph::~DiGraph() {
    GUM_DESTRUCTOR( DiGraph );
    if ( __mutableTopologicalOrder != nullptr ) {
      delete __mutableTopologicalOrder;
    }
  }

  const std::string DiGraph::toString() const {
    std::string s = NodeGraphPart::toString();
    s += " , ";
    s += ArcGraphPart::toString();
    return s;
  }

  const std::string DiGraph::toDot( const std::string& name ) const {
    std::stringstream strBuff;
    std::string       tab = "     ";
    strBuff << "digraph " << name << " {" << std::endl;

    for ( const auto node : nodes() )
      strBuff << tab << node << ";" << std::endl;

    strBuff << std::endl;

    for ( const auto& arc : arcs() )
      strBuff << tab << arc.tail() << " -> " << arc.head() << ";" << std::endl;

    strBuff << "}" << std::endl << std::endl;
    return strBuff.str();
  }

  /// for friendly displaying the content of directed graphs
  std::ostream& operator<<( std::ostream& stream, const DiGraph& g ) {
    stream << g.toString();
    return stream;
  }

  const Sequence<NodeId>& DiGraph::topologicalOrder( bool clear ) const {
    if ( clear || ( __mutableTopologicalOrder ==
                    nullptr ) ) {  // we have to call _topologicalOrder
      if ( __mutableTopologicalOrder == nullptr ) {
        __mutableTopologicalOrder = new Sequence<NodeId>();
      } else {
        // clear is True
        __mutableTopologicalOrder->clear();
      }

      __topologicalOrder();
    }

    return *__mutableTopologicalOrder;
  }

  void DiGraph::__topologicalOrder() const {
    auto dag = *this;
    auto roots = std::vector<NodeId>();

    for ( const auto node : dag.nodes() ) {
      if ( dag.parents( node ).empty() ) {
        roots.push_back( node );
      }
    }

    while ( roots.size() ) {
      if ( __mutableTopologicalOrder->exists( roots.back() ) ) {
        GUM_ERROR( InvalidDirectedCycle,
                   "cycles prevent the creation of a topological ordering." );
      }
      __mutableTopologicalOrder->insert( roots.back() );
      roots.pop_back();

      while ( dag.children( __mutableTopologicalOrder->back() ).size() ) {
        auto back = __mutableTopologicalOrder->back();
        auto child = *( dag.children( back ).begin() );
        dag.eraseArc( Arc( back, child ) );

        if ( dag.parents( child ).empty() ) {
          roots.push_back( child );
        }
      }
    }

    GUM_ASSERT( dag.sizeArcs() == ( gum::Size )( 0 ) );
    GUM_ASSERT( __mutableTopologicalOrder->size() == dag.size() );
  }
} /* namespace gum */
