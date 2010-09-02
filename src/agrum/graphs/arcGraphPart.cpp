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
    __arcs( s.__arcs ) {
    GUM_CONS_CPY( ArcGraphPart );

    // copy the sets of parents
    const Property<NodeSet*>::onNodes& pars = s.__parents;
    __parents.resize ( pars.capacity () );
    for ( Property<NodeSet*>::onNodes::const_iterator iter = pars.begin();
          iter != pars.end(); ++iter ) {
      NodeSet* newpar = new NodeSet ( **iter );
      __parents.insert ( iter.key (), newpar );
    }

    // copy the sets of children
    const Property<NodeSet*>::onNodes& children = s.__children;
    __children.resize ( children.capacity () );
    for ( Property<NodeSet*>::onNodes::const_iterator iter = children.begin();
          iter != children.end(); ++iter ) {
      NodeSet* newchildren = new NodeSet ( **iter );
      __children.insert ( iter.key (), newchildren );
    }

    if ( onArcAdded.hasListener() ) {
      for ( ArcSetIterator iter = __arcs.begin(); iter != __arcs.end(); ++iter ) {
        GUM_EMIT2( onArcAdded, iter->tail(), iter->head() );
      }
    }
  }


  ArcGraphPart::~ArcGraphPart() {
    GUM_DESTRUCTOR( ArcGraphPart );
    // be sure to deallocate all the parents and children sets
    clearArcs ();
  }

  
  void ArcGraphPart::clearArcs() {
    for ( Property<NodeSet*>::onNodes::const_iterator iter = __parents.begin();
          iter != __parents.end(); ++iter ) {
      delete *iter;
    }
    __parents.clear();

    for ( Property<NodeSet*>::onNodes::const_iterator iter = __children.begin();
          iter != __children.end(); ++iter ) {
      delete *iter;
    }
    __children.clear();

    // we need this copy only if at least one onArcDeleted listener exists
    if ( onArcDeleted.hasListener() ) {
      ArcSet tmp = __arcs;
      __arcs.clear();
      for ( ArcSetIterator iter = tmp.begin();iter != tmp.end();++iter )
        GUM_EMIT2( onArcDeleted, iter->tail(), iter->head() );
    }
    else {
      __arcs.clear();
    }
  }


  ArcGraphPart& ArcGraphPart::operator=( const ArcGraphPart& s ) {
    // avoid self assignment
    if ( this != &s ) {
      clearArcs ();

      __arcs = s.__arcs;

      // copy the sets of parents
      const Property<NodeSet*>::onNodes& pars = s.__parents;
      __parents.resize ( pars.capacity () );
      for ( Property<NodeSet*>::onNodes::const_iterator iter = pars.begin();
            iter != pars.end(); ++iter ) {
        NodeSet* newpar = new NodeSet ( **iter );
        __parents.insert ( iter.key (), newpar );
      }

      // copy the sets of children
      const Property<NodeSet*>::onNodes& children = s.__children;
      __children.resize ( children.capacity () );
      for ( Property<NodeSet*>::onNodes::const_iterator iter = children.begin();
            iter != children.end(); ++iter ) {
        NodeSet* newchildren = new NodeSet ( **iter );
        __children.insert ( iter.key (), newchildren );
      }

      if ( onArcAdded.hasListener() ) {
        for ( ArcSetIterator iter = __arcs.begin(); iter != __arcs.end(); ++iter ) {
          GUM_EMIT2( onArcAdded, iter->tail(), iter->head() );
        }
      }
    }

    return *this;
  }

  
  const std::string ArcGraphPart::toString() const {
    std::stringstream s;
    bool first=true;
    s<<"{";

    for ( ArcSetIterator it=__arcs.begin();it!=__arcs.end();++it ) {
      if ( first ) {
        first=false;
      }
      else {
        s<<",";
      }

      s<<*it;
    }

    s<<"}";
    
    return s.str();
  }
  

  const std::vector<NodeId>
  ArcGraphPart::directedPath( const NodeId n1,const NodeId n2 ) const {
    // not recursive version => use a FIFO for simulating the recursion
    List<NodeId> nodeFIFO;
    nodeFIFO.pushBack( n2 );

    // mark[node] = successor if visited, else mark[node] does not exist
    Property<NodeId>::onNodes mark;
    mark.insert (n2,n2);
    
    NodeId current;
    
    while ( ! nodeFIFO.empty() ) {
      current=nodeFIFO.front();
      nodeFIFO.popFront();

      // check the parents  //////////////////////////////////////////////
      const NodeSet& set = parents( current );

      for ( NodeSetIterator ite=set.begin();ite!=set.end();++ite ) {
        NodeId new_one = *ite;

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
  ArcGraphPart::directedUnorientedPath( const NodeId n1,const NodeId n2 ) const {
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

      // check the parents //////////////////////////////////////////////
      const NodeSet& set_parent = parents( current );

      for ( NodeSetIterator ite=set_parent.begin();ite!=set_parent.end();++ite ) {
        NodeId new_one = *ite;

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
      const NodeSet& set_children = children( current );

      for ( NodeSetIterator ite=set_children.begin();
            ite!=set_children.end();++ite ) {
        NodeId new_one = *ite;

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


  std::ostream& operator<< ( std::ostream& stream, const ArcGraphPart& set ) {
    stream<<set.toString();
    return stream;
  }


} /* namespace gum */

