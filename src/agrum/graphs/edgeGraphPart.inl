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
 *   GNU General Public License for more defirsts.                         *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief Inline implementation of classes for undirected edge sets
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */


namespace gum {

  INLINE EdgeGraphPart& EdgeGraphPart::operator=(const EdgeGraphPart& s ) {
    // avoid self assignment
    if (this!=&s) {
      __edges=s.__edges;
      __neighbours=s.__neighbours;
    }
    return *this;
  }

  INLINE bool EdgeGraphPart::emptyEdges() const {
    return __edges.empty();
  }

  INLINE Size EdgeGraphPart::sizeEdges() const {
    return __edges.size();
  }

  INLINE const EdgeSet& EdgeGraphPart::edges() const {
    return __edges;
  }

  INLINE bool EdgeGraphPart::existsEdge( const Edge& edge ) const {
    return __edges.contains( edge );
  }

  INLINE bool
  EdgeGraphPart::existsEdge( const NodeId first,const NodeId second ) const {
    return __edges.contains( Edge( first,second ) );
  }

  INLINE void EdgeGraphPart::__checkNeighbours( const NodeId id ) const {
    if ( ! __neighbours.exists( id ) ) {
      __neighbours.insert( id,__empty_edge_set );
    }
  }

  INLINE void EdgeGraphPart::insertEdge( const Edge& edge ) {
    __edges.insert( edge );
    __checkNeighbours( edge.first() );
    __checkNeighbours( edge.second() );
    __neighbours[edge.first()].insert( edge );
    __neighbours[edge.second()].insert( edge );
    GUM_EMIT2( onEdgeAdded,edge.first(),edge.second() );
  }

  INLINE void EdgeGraphPart::insertEdge( const NodeId first,const NodeId second ) {
    insertEdge( Edge( first,second ) );
  }

  INLINE void EdgeGraphPart::eraseEdge( const Edge& edge ) {
    if ( existsEdge( edge ) ) {
      // ASSUMING first and second exists in __neighbours (if not, it is an error)

      // here we need to create a copy of edge because when function
      // eraseNeighbours is called, after __neighbours.erase is called, "edge"
      // does not exist anymore. Hence, the following erases and GUM_EMIT2 may
      // segfault.
      Edge edge1 = edge;
      
      __neighbours[edge1.first()].erase( edge1 );
      __neighbours[edge1.second()].erase( edge1 );
      __edges.erase( edge1 );
      GUM_EMIT2( onEdgeDeleted,edge1.first(),edge1.second() );
    }
  }

  INLINE void EdgeGraphPart::eraseEdge( const NodeId first,const NodeId second ) {
    eraseEdge( Edge( first,second ) );
  }

  INLINE const EdgeSet& EdgeGraphPart::neighbours( const NodeId id ) const {
    __checkNeighbours( id );
    return __neighbours[id];
  }

  INLINE void EdgeGraphPart::eraseNeighbours( const NodeId id ) {
    if ( __neighbours.exists( id ) ) {
      const EdgeSet& set=neighbours( id );
      
      for ( EdgeSetIterator iter=set.begin();iter!=set.end();++iter ) {
        eraseEdge( *iter );
      }
    }
  }

  INLINE void EdgeGraphPart::clearEdges() {
    if ( onEdgeDeleted.hasListener() ) {
      EdgeSet tmp=__edges;

      __edges.clear();
      __neighbours.clear();

      for ( EdgeSetIterator iter=tmp.begin();iter!=tmp.end();++iter )
        GUM_EMIT2( onEdgeDeleted, iter->first(),iter->second() );
    }
    else {
      __edges.clear();
      __neighbours.clear();
    }
  }

  INLINE const EdgeSetIterator EdgeGraphPart::beginEdges() const {
    return __edges.begin();
  }

  INLINE const EdgeSetIterator& EdgeGraphPart::endEdges() const {
    return __edges.end();
  }

  INLINE bool EdgeGraphPart::operator==( const EdgeGraphPart& p ) const {
    return __edges==p.__edges;
  }

  INLINE bool EdgeGraphPart::operator!=( const EdgeGraphPart& p ) const {
    return __edges!=p.__edges;
  }


} /* namespace gum */

