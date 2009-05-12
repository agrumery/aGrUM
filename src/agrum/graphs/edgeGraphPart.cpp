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
      __edges( edges_size ,edges_resize_policy ) {
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
    bool first=true;
    s<<"{";

    for ( EdgeSetIterator it=__edges.begin();it!=__edges.end();++it ) {
      if ( first ) {
        first=false;
      } else {
        s<<",";
      }

      s<<*it;
    }

    s<<"}";

    s >> res;
    return res;
  }

  const std::vector<NodeId>
  EdgeGraphPart::undirectedPath( const NodeId n1,const NodeId n2 ) const {
    // not recursive version ...
    List<NodeId> nodeFile;
    // mark[node] contains 0 if not visited
    // mark[node]=predecessor if visited
    // Instead of NEW_NODE_PROPERTY((NodeId)0), we prefer to use a
    // LAZY GUM_NODE_PROPERTY, using HashTable::getWithDefault method (instead
    // of HashTable::operator[])
    Property<NodeId>::onNodes mark; //=NEW_NODE_PROPERTY((NodeId)0)
    mark.getWithDefault( n1,n1 );
    nodeFile.pushBack( n1 );
    NodeId current;

    while ( ! nodeFile.empty() ) {
      current=nodeFile.front();nodeFile.popFront();
      // check the neighbour //////////////////////////////////////////////
      const EdgeSet& set=neighbours( current );

      for ( EdgeSetIterator ite=set.begin();ite!=set.end();++ite ) {
        NodeId new_one=ite->other( current );

        if ( 0!=mark.getWithDefault( new_one,( NodeId ) 0 ) )
          continue; // if this node is already marked, stop

        mark[new_one]=current;

        if ( new_one==n2 ) break;

        nodeFile.pushBack( new_one );
      }
    }

    if ( 0==mark.getWithDefault( n2,( NodeId )0 ) )
      GUM_ERROR( NotFound,"no path found" );

    std::vector<NodeId> v;

    for ( current=n2;current!=n1;current=mark[current] )  v.push_back( current );

    v.push_back( n1 );

    return v;
  }

  std::ostream& operator<< ( std::ostream& stream, const EdgeGraphPart& set ) {
    stream<<set.toString();
    return stream;
  }


  // STATIC OBJECT
  // static definition of __empty_edge_set
  EdgeSet EdgeGraphPart::__empty_edge_set;


} /* namespace gum */


