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
 * @brief Inline implementation of CDG.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

namespace gum {
  namespace prm {

    INLINE
    CDG::CDG( const PRM& prm ) {
      GUM_CONSTRUCTOR( CDG );
      __buildGraph( prm );
    }

    INLINE
    CDG::CDG( const CDG& source ):
      __graph( source.__graph ), __modalitites( source.__modalitites ),
      __elt_map( source.__elt_map ) {
      GUM_CONS_CPY( CDG );

      for ( NodeMap::const_iterator iter = source.__node_map.begin(); iter != source.__node_map.end(); ++iter ) {
        __node_map.insert( iter.key(), new HashTable<const ClassElement*, NodeId>( **iter ) );
      }
    }

    INLINE
    const DAG&
    CDG::dag() const { return __graph; }

    INLINE
    const CDG::EltPair&
    CDG::get( NodeId id ) const { return *( __elt_map[id] ); }

    INLINE
    NodeId
    CDG::get( const ClassElementContainer& c, const ClassElement& elt ) const {
      return ( *( __node_map[&c] ) )[&elt];
    }

    INLINE
    const NodeProperty<unsigned int>&
    CDG::modalities() const { return __modalitites; }

    INLINE
    void
    CDG::__addNode( const ClassElementContainer* c, const ClassElement& elt ) {
      switch ( elt.elt_type() ) {
        case ClassElement::prm_attribute:
        case ClassElement::prm_aggregate: {
          NodeId id = __graph.insertNode();
          __elt_map.insert( id, new CDG::EltPair( c, &elt ) );
          __node_map[c]->insert( &elt, id );
          __modalitites.insert( id, elt.type().variable().domainSize() );
          break;
        }

        default: { /* do nothing */ break; }
      }
    }

  } /* namespace prm */
} /* namespace gum */

