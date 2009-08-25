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
 * @brief Inline implementation of classes for directed edge sets
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */


namespace gum {

  INLINE  ArcGraphPart& ArcGraphPart::operator=( const ArcGraphPart& s ) {
    // avoid self assignment
    if (this!=&s) {
      __arcs=s.__arcs;
      __parents=s.__parents;
      __children=s.__children;
    }
    return *this;
  }

  INLINE bool ArcGraphPart::emptyArcs() const {
    return __arcs.empty();
  }

  INLINE Size ArcGraphPart::sizeArcs() const {
    return __arcs.size();
  }

  INLINE const ArcSet& ArcGraphPart::arcs() const {
    return __arcs;
  }

  INLINE bool ArcGraphPart::existsArc( const Arc& arc ) const {
    return __arcs.contains( arc );
  }

  INLINE bool ArcGraphPart::existsArc( const NodeId tail,
                                       const NodeId head ) const {
    return __arcs.contains( Arc( tail,head ) );
  }
  
  INLINE void ArcGraphPart::__checkParents( const NodeId id ) const {
    if ( ! __parents.exists( id ) ) {
      __parents.insert( id,__empty_arc_set );
    }
  }

  INLINE void ArcGraphPart::__checkChildren( const NodeId id ) const {
    if ( ! __children.exists( id ) ) {
      __children.insert( id,__empty_arc_set );
    }
  }

  INLINE const ArcSet& ArcGraphPart::parents( const NodeId id ) const {
    __checkParents( id );
    return __parents[id];
  }

  INLINE const ArcSet& ArcGraphPart::children( const NodeId id ) const {
    __checkChildren( id );
    return __children[id];
  }

  INLINE const ArcSetIterator ArcGraphPart::beginArcs() const {
    return __arcs.begin();
  }

  INLINE const ArcSetIterator& ArcGraphPart::endArcs() const {
    return __arcs.end();
  }

  INLINE void ArcGraphPart::insertArc( const Arc& arc ) {
    __arcs.insert( arc );
    __checkParents( arc.head() );
    __checkChildren( arc.tail() );
    __parents[arc.head()].insert( arc );
    __children[arc.tail()].insert( arc );
    GUM_EMIT2( onArcAdded,arc.tail(),arc.head() );
  }

  INLINE void ArcGraphPart::insertArc( const NodeId tail,const NodeId head ) {
    insertArc( Arc( tail,head ) );
  }

  INLINE void ArcGraphPart::eraseArc( const Arc& arc ) {
    // ASSUMING tail and head exists in __parents anf __children
    // (if not, it is an error)
    if ( existsArc( arc ) ) {
      // here we need to create a copy of arc because when function eraseParents
      // is called, after __parents.erase is called, the arc does not exist
      // anymore. Hence, the following erases and GUM_EMIT2 may segfault.
     Arc arc1 = arc;
      __parents[arc1.head()].erase( arc1 );
      __children[arc1.tail()].erase( arc1 );
      __arcs.erase( arc1 );
      GUM_EMIT2( onArcDeleted,arc1.tail(),arc1.head() );
    }
  }

  INLINE void ArcGraphPart::eraseArc( NodeId tail,NodeId head ) {
    eraseArc( Arc( tail,head ) );
  }

  INLINE void ArcGraphPart::clearArcs() {
    // we need this copy only if at least one onArcDeleted listener exists
    if ( onArcDeleted.hasListener() ) {
      ArcSet tmp=__arcs;

      __arcs.clear();
      __parents.clear();
      __children.clear();
      
      for ( ArcSetIterator iter=tmp.begin();iter!=tmp.end();++iter )
        GUM_EMIT2( onArcDeleted, iter->tail(),iter->head() );
    }
    else {
      __arcs.clear();
      __parents.clear();
      __children.clear();
    }
  }  

  INLINE void ArcGraphPart::_eraseSetOfArcs( const ArcSet& set ) {
    for ( ArcSetIterator iter=set.begin();iter!=set.end();++iter )
      eraseArc( *iter );
  }

  INLINE void ArcGraphPart::eraseParents( const NodeId id ) {
    if ( __parents.exists( id ) ) _eraseSetOfArcs( __parents[id] );
  }

  INLINE void ArcGraphPart::eraseChildren( const NodeId id ) {
    if ( __children.exists( id ) ) _eraseSetOfArcs( __children[id] );
  }

  INLINE bool ArcGraphPart::operator==( const ArcGraphPart& p ) const {
    return __arcs==p.__arcs;
  }

  INLINE bool ArcGraphPart::operator!=( const ArcGraphPart& p ) const {
    return __arcs!=p.__arcs;
  }



} /* namespace gum */
