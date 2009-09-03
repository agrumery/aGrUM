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
 * @brief Source implementation of classes for undirected edge sets
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/graphs/edgeGraphPart.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/edgeGraphPart.inl>
#endif //GUM_NOINLINE


namespace gum {


  ///////////////////// EdgeGraphPart
  EdgeGraphPart::EdgeGraphPart( Size edges_size ,
                                bool edges_resize_policy ) :
      __edges( edges_size , edges_resize_policy ) {
    GUM_CONSTRUCTOR( EdgeGraphPart );
  }

  EdgeGraphPart::EdgeGraphPart( const EdgeGraphPart& s ):
      __edges( s.__edges ),
      __neighbours( s.__neighbours ) {
    GUM_CONS_CPY( EdgeGraphPart );
  }

  EdgeGraphPart::~EdgeGraphPart() {
    GUM_DESTRUCTOR( EdgeGraphPart );
  }

  const std::string EdgeGraphPart::toString() const {
    std::stringstream s;
    std::string res;
    bool first = true;
    s << "{";

    for ( EdgeIterator it = __edges.begin();it != __edges.end();++it ) {
      if ( first ) {
        first = false;
      } else {
        s << ",";
      }

      s << *it;
    }

    s << "}";

    s >> res;
    return res;
  }

  const std::vector<NodeId>
  EdgeGraphPart::undirectedPath( const NodeId n1, const NodeId n2 ) const {
    // not recursive version => use a FIFO for simulating the recursion
    List<NodeId> nodeFIFO;
    nodeFIFO.pushBack( n2 );

    // mark[node] = predecessor if visited, else mark[node] does not exist
    Property<NodeId>::onNodes mark;
    mark.insert( n2, n2 );

    NodeId current;

    while ( ! nodeFIFO.empty() ) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      // check the neighbour //////////////////////////////////////////////
      const EdgeSet& set = neighbours( current );

      for ( EdgeIterator ite = set.begin();ite != set.end();++ite ) {
        NodeId new_one = ite->other( current );

        if ( mark.exists( new_one ) ) // if this node is already marked, stop
          continue;

        mark.insert( new_one, current );

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

    GUM_ERROR( NotFound, "no path found" );

  }

  std::ostream& operator<< ( std::ostream& stream, const EdgeGraphPart& set ) {
    stream << set.toString();
    return stream;
  }


  // STATIC OBJECT
  // static definition of __empty_edge_set
  const EdgeSet EdgeGraphPart::__empty_edge_set;


} /* namespace gum */


// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
