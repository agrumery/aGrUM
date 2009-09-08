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
 * @brief Source implementation of nodes sets
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/graphs/nodeGraphPart.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/nodeGraphPart.inl>
#endif //GUM_NOINLINE


namespace gum {


  ///////////////////// NodeGraphPart
  NodeGraphPart::NodeGraphPart( Size holes_size ,
                                bool holes_resize_policy ) :
      __holes_size( holes_size ),
      __holes_resize_policy( holes_resize_policy ),
      __endIterator( this ),
      __max( 0 ) {
    __holes = 0;
    GUM_CONSTRUCTOR( NodeGraphPart );
    __updateEndIterator();
  }

  NodeGraphPart::NodeGraphPart( const NodeGraphPart& s ) :
      __holes_size( s.__holes_size ), __holes_resize_policy( s.__holes_resize_policy ), __endIterator( this ), __max( s.__max ) {
    __holes = 0;

    if ( s.__holes )
      __holes = new NodeSet( *s.__holes );

    __updateEndIterator();

    GUM_CONS_CPY( NodeGraphPart );
  }

  NodeGraphPart::~NodeGraphPart() {
    if ( __holes ) delete __holes;

    GUM_DESTRUCTOR( NodeGraphPart );
  }

  void NodeGraphPart::populateNodes( const NodeGraphPart& s ) {
    clear();// "virtual" flush of the nodes set
    __holes_size = s.__holes_size;
    __holes_resize_policy = s.__holes_resize_policy;

    if ( s.__holes )
      __holes = new NodeSet( *s.__holes );

    __max = s.__max;

    __updateEndIterator();
  }

  // id is assumed not to be already a hole
  void NodeGraphPart::__addHole( NodeId node ) {

    // the node exists
    if ( node == __max ) {  // we remove the max : no new hole and maybe a bunch of holes to remove
      __max--;

      if ( __holes ) {
        while ( __holes->contains( __max ) ) { // a bunch of holes to remove. We do not use inHoles for optimisation : not to repeat the test if (__holes) each time
          __holes->erase( __max );
          __max--;
        }

        if ( __holes->empty() ) {
          __holes->resize( __holes_size );
        }
      }

      __updateEndIterator();
    } else {
      if ( !__holes )
        __holes = new NodeSet( __holes_size, __holes_resize_policy );

      __holes->insert( node );
    }
  }

  std::string NodeGraphPart::toString() const {
    std::stringstream s;
    bool first = true;
    s << "{";

    for ( NodeId id = 1;id <= __max;id++ ) {
      if ( inHoles( id ) ) continue;

      if ( first ) {
        first = false;
      } else {
        s << ",";
      }

      s << id;
    }

    s << "}";

    return s.str();
  }

  std::ostream& operator<< ( std::ostream& stream, const NodeGraphPart& set ) {
    stream << set.toString();
    return stream;
  }

  void NodeGraphPart::insertNode( const NodeId id ) {
    bool isOK = false;

    if (id==(NodeId)0)
      GUM_ERROR(OutOfLowerBound,"id cannot be 0");
    
    if ( __max < id ) {
      if ( __max + 1 < id ) { // we have to add holes
        if ( ! __holes )
          __holes = new NodeSet();

        for ( NodeId i = __max + 1; i < id;i++ )
          __holes->insert( i );
      }

      __max = id;

      __updateEndIterator();

      isOK = true;
    } else {
      if ( inHoles( id ) ) {  // we fill a hole
        __eraseHole( id );

        isOK = true;
      }
    }

    if ( ! isOK ) {
      GUM_ERROR( DuplicateElement, "This id is already used" );
    } else {
      GUM_EMIT1( onNodeAdded, id );
    }
  }

  void NodeGraphPart::__clearNodes( void ) {
    if ( onNodeDeleted.hasListener() ) {
      for ( NodeId n = 1;n <= __max;n++ ) {
        if ( inHoles( n ) ) continue;

        GUM_EMIT1( onNodeDeleted, n );
      }
    }

__max = 0;
__updateEndIterator();

    delete( __holes );

    __holes = 0;
  }
} /* namespace gum */

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
