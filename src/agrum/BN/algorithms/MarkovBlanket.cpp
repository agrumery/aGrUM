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
 * @brief Source implementation of the class building the Markov Blanket from a
 * DAGmodel and a node (id or name)
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/BN/algorithms/MarkovBlanket.h>

#ifdef GUM_NO_INLINE
#include <agrum/BN/algorithms/MarkovBlanket_inl.h>
#endif  // GUM_NOINLINE

namespace gum {
  MarkovBlanket::MarkovBlanket( const DAGmodel& m, NodeId id )
      : __dag( m )
      , __node( id ) {
    __buildMarkovBlanket();
  }

  MarkovBlanket::MarkovBlanket( const DAGmodel& m, const std::string& name )
      : MarkovBlanket( m, m.idFromName( name ) ) {}

  MarkovBlanket::~MarkovBlanket() {}

  void MarkovBlanket::__buildMarkovBlanket() {
    if ( !__dag.nodes().exists( __node ) )
      GUM_ERROR( InvalidArgument, "Node " << __node << " does not exist." );

    __mb.addNode( __node );
    for ( const auto& parent : __dag.dag().parents( __node ) ) {
      __mb.addNode( parent );
      __mb.addArc( parent, __node );
    }
    for ( const auto& child : __dag.dag().children( __node ) ) {
      __mb.addNode( child );
      __mb.addArc( __node, child );
      for ( const auto& opar : __dag.dag().parents( child ) ) {
        if ( opar != __node ) {
          if ( !__mb.nodes().exists( opar ) ) __mb.addNode( opar );
          __mb.addArc( opar, child );
        }
      }
    }
  }


  std::string MarkovBlanket::toDot( void ) const {
    std::stringstream output;
    std::stringstream nodeStream;
    std::stringstream edgeStream;
    List<NodeId>      treatedNodes;
    output << "digraph \""
           << "no_name\" {" << std::endl;
    nodeStream << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for ( const auto node : __mb.nodes() ) {

      nodeStream << tab << node << "[label=\"" << __dag.variable( node ).name()
                 << "\"";
      if ( node == __node ) {
        nodeStream << ", color=red";
      }
      nodeStream << "];";

      for ( const auto chi : __mb.children( node ) )
        edgeStream << tab << node << " -> " << chi << ";" << std::endl;
    }

    output << nodeStream.str() << std::endl
           << edgeStream.str() << std::endl
           << "}" << std::endl;

    return output.str();
  }
}  // namespace gum
