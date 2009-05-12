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
 * @brief Source implementation of Base classes for undirected graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/graphs/undiGraph.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/undiGraph.inl>
#endif //GUM_NOINLINE


namespace gum {


  UndiGraph::UndiGraph( Size nodes_size ,
                        bool nodes_resize_policy,
                        Size edges_size ,
                        bool edges_resize_policy ) :
    NodeGraphPart( nodes_size,nodes_resize_policy ),
    EdgeGraphPart( edges_size,edges_resize_policy ) {
    GUM_CONSTRUCTOR( UndiGraph );
  }

  UndiGraph::UndiGraph( const UndiGraph& g ) :
    NodeGraphPart( g ),EdgeGraphPart( g ) {
    GUM_CONS_CPY( UndiGraph );
  }

  UndiGraph::~UndiGraph() {
    GUM_DESTRUCTOR( UndiGraph );
  }

  bool UndiGraph::hasUndirectedCycle() const {
    // not recursive version ...
    List<NodeId> nodeFile;
    Property<NodeId>::onNodes predecessor=nodesProperty(( NodeId ) 0 );

    for ( NodeSetIterator iteFirst=nodes().begin();
          iteFirst!=nodes().end();++iteFirst ) {
      if ( predecessor[*iteFirst]==( NodeId ) 0 ) {
        // we found a not-already-visited node
        nodeFile.insert( *iteFirst );
        predecessor[*iteFirst]=*iteFirst;

        while ( ! nodeFile.empty() ) {
          NodeId current=nodeFile.front();nodeFile.popFront();
          // check the neighbour //////////////////////////////////////////////
          const EdgeSet& set=neighbours( current );

          for ( EdgeSetIterator ite=set.begin();ite!=set.end();++ite ) {
            NodeId new_one=ite->other( current );

            if ( new_one==*iteFirst )
              continue; // the father of all nodes in the search

            if ( predecessor[new_one]==current )
              continue; // the direct parent in the search

            if ( predecessor[new_one]!=( NodeId ) 0 ) {
              return true; // we found an undirectedcycle !!
            }

            predecessor[new_one]=current;

            nodeFile.pushBack( new_one );
          }
        }
      }
    }

    return false;
  }

  const std::string UndiGraph::toString() const {
    std::string s=NodeGraphPart::toString();
    s+=" , ";
    s+=EdgeGraphPart::toString();
    return s;
  }

  /// for friendly displaying the content of directed graphs
  std::ostream& operator<< ( std::ostream& stream, const UndiGraph& g ) {
    stream<<g.toString();
    return stream;
  }


} /* namespace gum */

  
