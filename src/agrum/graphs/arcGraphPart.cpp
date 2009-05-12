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
 * @brief Source implementation of classes for directed edge sets
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/graphs/arcGraphPart.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/arcGraphPart.inl>
#endif //GUM_NOINLINE


namespace gum {


  ///////////////////// ArcGraphPart
  ArcGraphPart::ArcGraphPart( Size arcs_size ,
                              bool arcs_resize_policy ) :
    __arcs( arcs_size,arcs_resize_policy ) {
    GUM_CONSTRUCTOR( ArcGraphPart );
  }

  ArcGraphPart::ArcGraphPart( const ArcGraphPart& s ):
    __arcs( s.__arcs ),
    __parents( s.__parents ),
    __children( s.__children ) {
    GUM_CONS_CPY( ArcGraphPart );
  }

  ArcGraphPart::~ArcGraphPart() {
    GUM_DESTRUCTOR( ArcGraphPart );
  }

  const std::string ArcGraphPart::toString() const {
    std::stringstream s;
    std::string res;
    bool first=true;
    s<<"{";

    for ( ArcSetIterator it=__arcs.begin();it!=__arcs.end();++it ) {
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
  ArcGraphPart::directedPath( const NodeId n1,const NodeId n2 ) const {
    // not recursive version ...
    List<NodeId> nodeFile;
    // mark[node] contains 0 if not visited
    // mark[node]=predecessor if visited
    Property<NodeId>::onNodes mark;
    mark[n1]=n1;
    nodeFile.pushBack( n1 );
    NodeId current;

    while ( ! nodeFile.empty() ) {
      current=nodeFile.front();nodeFile.popFront();
      // check the children  //////////////////////////////////////////////
      const ArcSet& set=children( current );

      for ( ArcSetIterator ite=set.begin();ite!=set.end();++ite ) {
        NodeId new_one=ite->head();

        if ( mark[new_one]!=0 ) continue; // if this node is already marked, stop

        mark[new_one]=current;

        if ( new_one==n2 ) break;

        nodeFile.pushBack( new_one );
      }
    }

    if ( mark[n2] ==0 ) GUM_ERROR( NotFound,"no path found" );

    std::vector<NodeId> v;

    for ( current=n2;current!=n1;current=mark[current] )  v.push_back( current );

    v.push_back( n1 );

    return v;
  }

  const std::vector<NodeId>
  ArcGraphPart::directedUnorientedPath( const NodeId n1,const NodeId n2 ) const {
    // not recursive version ...
    List<NodeId> nodeFile;
    // mark[node] contains 0 if not visited
    // mark[node]=predecessor if visited
    Property<NodeId>::onNodes mark;//=nodes().hashMapNodes(( NodeId )0 );
    mark[n1]=n1;
    nodeFile.pushBack( n1 );
    NodeId current;

    while ( ! nodeFile.empty() ) {
      current=nodeFile.front();nodeFile.popFront();
      // check the children //////////////////////////////////////////////
      const ArcSet& set_children=children( current );

      for ( ArcSetIterator ite=set_children.begin();ite!=set_children.end();++ite ) {
        NodeId new_one=ite->head();

        if ( mark[new_one]!=0 ) continue; // if this node is already marked, stop

        mark[new_one]=current;

        if ( new_one==n2 ) break;

        nodeFile.pushBack( new_one );
      }

      // check the parent //////////////////////////////////////////////
      const ArcSet& set_parent=parents( current );

      for ( ArcSetIterator ite=set_parent.begin();ite!=set_parent.end();++ite ) {
        NodeId new_one=ite->tail();

        if ( mark[new_one]!=0 ) continue; // if this node is already marked, stop

        mark[new_one]=current;

        if ( new_one==n2 ) break;

        nodeFile.pushBack( new_one );
      }
    }

    if ( mark[n2] ==0 ) GUM_ERROR( NotFound,"no path found" );

    std::vector<NodeId> v;

    for ( current=n2;current!=n1;current=mark[current] )  v.push_back( current );

    v.push_back( n1 );

    return v;
  }


  std::ostream& operator<< ( std::ostream& stream, const ArcGraphPart& set ) {
    stream<<set.toString();
    return stream;
  }



  // STATIC OBJECT
  // static definition of __empty_arc_set
  ArcSet ArcGraphPart::__empty_arc_set;


} /* namespace gum */

