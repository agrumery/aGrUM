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
#include <agrum/graphs/mixedGraph.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/mixedGraph.inl>
#endif //GUM_NOINLINE


namespace gum {


  MixedGraph::MixedGraph( Size nodes_size ,
                          bool nodes_resize_policy,
                          Size arcs_size ,
                          bool arcs_resize_policy,
                          Size edges_size ,
                          bool edges_resize_policy ) :
    NodeGraphPart( nodes_size,nodes_resize_policy ),
    UndiGraph( edges_size,edges_resize_policy ),
    DiGraph( arcs_size,arcs_resize_policy )  {
    GUM_CONSTRUCTOR( MixedGraph );
  }

  MixedGraph::MixedGraph( const MixedGraph& g ) :
    NodeGraphPart( g ),UndiGraph( g ),DiGraph( g )  {
    GUM_CONS_CPY( MixedGraph );
  }

  MixedGraph::~MixedGraph() {
    GUM_DESTRUCTOR( MixedGraph );
  }

  const std::string MixedGraph::toString() const {
    std::string s=NodeGraphPart::toString();
    s+=" , ";
    s+=ArcGraphPart::toString();
    s+=" , ";
    s+=EdgeGraphPart::toString();
    return s;
  }

  const std::vector<NodeId>
  MixedGraph::mixedOrientedPath( const NodeId n1,const NodeId n2 ) const {
    // not recursive version => use a FIFO for simulating the recursion
    List<NodeId> nodeFIFO;
    nodeFIFO.pushBack( n2 );
    
    // mark[node] = successor if visited, else mark[node] does not exist
    Property<NodeId>::onNodes mark;
    mark.insert ( n2,n2 );
   
    NodeId current;

    while ( ! nodeFIFO.empty() ) {
      current=nodeFIFO.front();
      nodeFIFO.popFront();

      // check the neighbours //////////////////////////////////////////////
      const EdgeSet& set_neighbour=neighbours( current );

      for ( EdgeSetIterator ite=set_neighbour.begin();
            ite!=set_neighbour.end();++ite ) {
        NodeId new_one=ite->other( current );

        if ( mark.exists ( new_one ) ) // if the node has already been visited
          continue;                    // do not check it again

        mark.insert( new_one, current );

        if ( new_one == n1 ) {
          std::vector<NodeId> v;

          for ( current=n1; current!=n2; current=mark[current] )
            v.push_back( current );
          v.push_back( n2 );
          
          return v;
        }

        nodeFIFO.pushBack( new_one );
      }

      // check the parents  //////////////////////////////////////////////
      const ArcSet& set = parents( current );

      for ( ArcSetIterator ite=set.begin();ite!=set.end();++ite ) {
        NodeId new_one=ite->tail();

        if ( mark.exists(new_one) ) // if this node is already marked, do not
          continue;                 // check it again

        mark.insert(new_one, current);

        if ( new_one == n1 ) {
          std::vector<NodeId> v;

          for ( current = n1; current != n2; current = mark[current] )
            v.push_back( current );
          v.push_back( n2 );

          return v;
        }
  
        nodeFIFO.pushBack( new_one );
      }
    }

    GUM_ERROR( NotFound,"no path found" );
  }

  const std::vector<NodeId>
  MixedGraph::mixedUnorientedPath( const NodeId n1,const NodeId n2 ) const {
    // not recursive version => use a FIFO for simulating the recursion
    List<NodeId> nodeFIFO;
    nodeFIFO.pushBack( n2 );

    // mark[node] = successor if visited, else mark[node] does not exist
    Property<NodeId>::onNodes mark;
    mark.insert ( n2,n2 );

    NodeId current;

    while ( ! nodeFIFO.empty() ) {
      current=nodeFIFO.front();
      nodeFIFO.popFront();

      // check the neighbours //////////////////////////////////////////////
      const EdgeSet& set_neighbour=neighbours( current );

      for ( EdgeSetIterator ite=set_neighbour.begin();
            ite!=set_neighbour.end();++ite ) {
        NodeId new_one=ite->other( current );

        if ( mark.exists ( new_one ) ) // if the node has already been visited
          continue;                    // do not check it again

        mark.insert( new_one, current );

        if ( new_one == n1 ) {
          std::vector<NodeId> v;

          for ( current=n1; current!=n2; current=mark[current] )
            v.push_back( current );
          v.push_back( n2 );
          
          return v;
        }

        nodeFIFO.pushBack( new_one );
      }

      // check the parents //////////////////////////////////////////////
      const ArcSet& set_parent = parents( current );

      for ( ArcSetIterator ite=set_parent.begin();ite!=set_parent.end();++ite ) {
        NodeId new_one=ite->tail();

        if ( mark.exists ( new_one ) ) // the node has already been visited
          continue; 

        mark.insert( new_one, current );

        if ( new_one==n1 ) {
          std::vector<NodeId> v;

          for ( current=n1; current!=n2; current=mark[current] )
            v.push_back( current );
          v.push_back( n2 );

          return v;
        }

        nodeFIFO.pushBack( new_one );
      }

      // check the children //////////////////////////////////////////////
      const ArcSet& set_children = children( current );

      for ( ArcSetIterator ite=set_children.begin();
            ite!=set_children.end();++ite ) {
        NodeId new_one=ite->head();

        if ( mark.exists ( new_one ) ) // the node has already been visited
          continue; 

        mark.insert( new_one, current );

        if ( new_one == n1 ) {
          std::vector<NodeId> v;

          for ( current=n1; current!=n2; current=mark[current] )
            v.push_back( current );
          v.push_back( n2 );

          return v;
        }

        nodeFIFO.pushBack( new_one );
      }

    }

    GUM_ERROR( NotFound,"no path found" );
  }


  /// for friendly displaying the content of directed graphs
  std::ostream& operator<< ( std::ostream& stream, const MixedGraph& g ) {
    stream<<g.toString();
    return stream;
  }
  

} /* namespace gum */

