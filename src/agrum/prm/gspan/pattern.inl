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
/**
 * @file
 * @brief Inline implementation of the Pattern class.
 *
 * @author Lionel TORTI
 */

namespace gum {
  namespace prm {
    namespace gspan {

      INLINE
      Pattern::Pattern():
        DiGraph(), __last( 0 ) {
        GUM_CONSTRUCTOR( Pattern );
      }

      INLINE
      Pattern::~Pattern() {
        GUM_DESTRUCTOR( Pattern );
      }

      INLINE
      NodeId
      Pattern::insertNode( LabelData& l ) {
        DiGraph::insertNode( size() + 1 );
        __node_map.insert( size(), &l );
        __last = &l;
        return size();
      }

      INLINE
      LabelData&
      Pattern::label( NodeId node ) {
        try {
          return *( __node_map[node] );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "node not found in this Pattern" );
        }
      }

      INLINE
      const LabelData&
      Pattern::label( NodeId node ) const {
        try {
          return *( __node_map[node] );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "node not found in this Pattern" );
        }
      }

      INLINE
      LabelData&
      Pattern::lastAdded() {
        if ( __last )
          return *__last;

        GUM_ERROR( OperationNotAllowed, "there are no LabelData yet" );
      }

      INLINE
      const LabelData&
      Pattern::lastAdded() const {
        if ( __last )
          return *__last;

        GUM_ERROR( OperationNotAllowed, "there are no LabelData yet" );
      }

      INLINE
      LabelData&
      Pattern::label( NodeId i, NodeId j ) {
        try {
          return *( __arc_map[Arc( i, j )].first );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "arc not found in this Pattern" );
        }
      }

      INLINE
      const LabelData&
      Pattern::label( NodeId i, NodeId j ) const {
        try {
          return *( __arc_map[Arc( i, j )].first );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "arc not found in this Pattern" );
        }
      }

      INLINE
      LabelData&
      Pattern::label( const Arc& arc ) {
        try {
          return *( __arc_map[arc].first );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "arc not found in this Pattern" );
        }
      }

      INLINE
      const LabelData&
      Pattern::label( const Arc& arc ) const {
        try {
          return *( __arc_map[arc].first );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "arc not found in this Pattern" );
        }
      }

      INLINE
      void
      Pattern::insertArc( NodeId i, NodeId j, LabelData& l ) {
        if ( not( DiGraph::exists( i ) and DiGraph::exists( j ) ) ) {
          GUM_ERROR( NotFound, "node not found in this pattern" );
        }

        EdgeCode* edge = new EdgeCode( i, j, __node_map[i]->id, l.id, __node_map[j]->id );

        if ( ( code().codes.size() == 0 ) or
             ( DFSCode::validNeighbors( code().codes.back(), edge ) ) ) {
          DiGraph::insertArc( i, j );
          __arc_map.insert( Arc( i, j ), std::make_pair( &l, edge ) );
          code().codes.push_back( edge );
        } else {
          delete edge;
          GUM_ERROR( OperationNotAllowed, "illegal arc considering neighborhood restriction" );
        }
      }

      INLINE
      bool
      Pattern::exists( NodeId id ) const {
        return DiGraph::exists( id );
      }

      INLINE
      bool
      Pattern::exists( NodeId tail, NodeId head ) const {
        return DiGraph::existsArc( tail, head );
      }

      INLINE
      Size
      Pattern::size() const { return DiGraph::size(); }

      INLINE
      Size
      Pattern::sizeArcs() const { return DiGraph::sizeArcs(); }

      INLINE
      void
      Pattern::rightmostPath( std::list<NodeId>& r_path ) const {
        r_path.push_back( size() );

        while ( r_path.front() != 1 ) {
          const NodeSet& parents = DiGraph::parents( r_path.front() );

          for ( NodeSetIterator arc = parents.begin();
                arc != parents.end(); ++arc ) {
            if ( *arc < r_path.front() ) {
              r_path.push_front( *arc );
              break;
            }
          }
        }
      }

      INLINE
      Pattern::NodeIterator
      Pattern::beginNodes() const { return DiGraph::beginNodes(); }

      INLINE
      const Pattern::NodeIterator&
      Pattern::endNodes() const { return DiGraph::endNodes(); }

      INLINE
      Pattern::ArcIterator
      Pattern::beginArcs() const { return DiGraph::beginArcs(); }

      INLINE
      const Pattern::ArcIterator&
      Pattern::endArcs() const { return DiGraph::endArcs(); }

      INLINE
      NeighborIterator
      Pattern::beginNeighbors( NodeId node ) const {
        return NeighborIterator( DiGraph::parents( node ), DiGraph::children( node ) );
      }

      INLINE
      DFSCode&
      Pattern::code() { return __code; }

      INLINE
      const DFSCode&
      Pattern::code() const { return __code; }

      INLINE
      EdgeCode&
      Pattern::edgeCode( NodeId tail, NodeId head ) {
        try {
          return *( __arc_map[Arc( tail, head )].second );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "arc not found in Pattern" );
        }
      }

      INLINE
      EdgeCode&
      Pattern::edgeCode( const Arc& arc ) {
        try {
          return *( __arc_map[arc].second );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "arc not found in Pattern" );
        }
      }

      INLINE
      const EdgeCode&
      Pattern::edgeCode( NodeId tail, NodeId head ) const {
        try {
          return *( __arc_map[Arc( tail, head )].second );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "arc not found in Pattern" );
        }
      }

      INLINE
      const EdgeCode&
      Pattern::edgeCode( const Arc& arc ) const {
        try {
          return *( __arc_map[arc].second );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "arc not found in Pattern" );
        }
      }

      INLINE
      void
      Pattern::pop_back() {
        EdgeCode* edge = __code.codes.back();
        __code.codes.pop_back();

        if ( edge->isForward() ) {
          __node_map.erase( edge->j );
          __arc_map.erase( Arc( edge->i, edge->j ) );
          DiGraph::eraseArc( Arc( edge->i, edge->j ) );
          DiGraph::eraseNode( edge->j );
        } else {
          __arc_map.erase( Arc( edge->i, edge->j ) );
          DiGraph::eraseArc( Arc( edge->i, edge->j ) );
        }

        delete edge;
      }

      INLINE
      void
      Pattern::remove( NodeId node ) {
        if ( DiGraph::parents( node ).empty() and DiGraph::children( node ).empty() ) {
          DiGraph::eraseNode( node );
          __node_map.erase( node );
        } else {
          GUM_ERROR( OperationNotAllowed, "the given node has neighbors" );
        }
      }


// NeighborIterator


      INLINE
      NeighborIterator::NeighborIterator( const NodeSet& parents,
                                          const NodeSet& children ):
        __parents( &parents ), __children( &children ),
        __parent_iterator( __parents->begin() ),
        __children_iterator( __children->begin() ),
        __iterator( 0 ), __end_iterator( 0 ) {
        GUM_CONSTRUCTOR( NeighborIterator );

        if ( __parents->empty() ) {
          __iterator = &__children_iterator;
          __end_iterator = &( __children->end() );
        } else {
          __iterator = &__parent_iterator;
          __end_iterator = &( __parents->end() );
        }
      }

      INLINE
      NeighborIterator::NeighborIterator( const NeighborIterator& from ):
        __parents( from.__parents ), __children( from.__children ),
        __parent_iterator( from.__parent_iterator ),
        __children_iterator( from.__children_iterator ),
        __iterator( 0 ), __end_iterator( from.__end_iterator ) {
        GUM_CONS_CPY( NeighborIterator );

        if ( from.__iterator == &( from.__parent_iterator ) ) {
          __iterator = &__parent_iterator;
        } else {
          __iterator = &__children_iterator;
        }
      }

      INLINE
      NeighborIterator::~NeighborIterator() {
        GUM_DESTRUCTOR( NeighborIterator );
      }

      INLINE
      NeighborIterator&
      NeighborIterator::operator++() {
        ++( *__iterator );

        if ( ( __iterator == ( &__parent_iterator ) ) and
             ( *__iterator ) == ( *__end_iterator ) ) {
          __iterator = ( &__children_iterator );
          __end_iterator = &( __children->end() );
        }

        return *this;
      }

      INLINE
      bool
      NeighborIterator::isEnd() const {
        return ( __iterator == ( &__children_iterator ) ) and
               ( __children_iterator == __children->end() );
      }

      INLINE
      bool
      NeighborIterator::operator!=( const NeighborIterator& from ) const {
        return ( *__iterator ) != *( from.__iterator );
      }

      INLINE
      bool
      NeighborIterator::operator==( const NeighborIterator& from ) const {
        return ( *__iterator ) == *( from.__iterator );
      }

      INLINE
      NeighborIterator&
      NeighborIterator::operator=( const NeighborIterator& from ) {
        __parents = from.__parents;
        __children = from.__children;
        __parent_iterator = from.__parent_iterator;
        __children_iterator = from.__children_iterator;

        if ( from.__iterator == &( from.__parent_iterator ) ) {
          __iterator = &__parent_iterator;
          __end_iterator = &( __parents->end() );
        } else {
          __iterator = &__children_iterator;
          __end_iterator = &( __children->end() );
        }

        return *this;
      }

      INLINE
      NodeId
      NeighborIterator::operator*() const { return **__iterator; }

    } /* namespace gspan */
  } /* namespace prm */
} /* namespace gum */

