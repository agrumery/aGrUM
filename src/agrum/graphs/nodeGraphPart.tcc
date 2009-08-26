/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *   test $Id: $                                                           *
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
 * @brief Implementation of the NodeGraphPart
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  template <typename VAL> INLINE
  typename Property<VAL>::onNodes
    NodeGraphPart::nodesProperty( VAL( *f )( const NodeId& ), Size size ) const {
    return __nodes.hashMap( f,size );
  }

  template <typename VAL> INLINE
  typename Property<VAL>::onNodes
    NodeGraphPart::nodesProperty( const VAL& val, Size size ) const {
    return __nodes.hashMap( val,size );
  }

  template <typename VAL> INLINE
  List<VAL> NodeGraphPart::listMapNodes( VAL( *f )( const NodeId& ) ) const {
    return __nodes.listMap( f );
  }

  template<typename T> void
  NodeGraphPart::populateNodesFromProperty
  ( const typename Property<T>::onNodes& h ) {
    clear();

    for ( typename Property<T>::onNodes::const_iterator iter = h.begin();
          iter != h.end(); ++iter ) {
      NodeId new_one=iter.key();

      if ( __nodes.contains( new_one ) )
        GUM_ERROR( DuplicateElement,"in the graph" );

      insertNode(new_one);
    }
  }


} /* namespace gum */


#endif    // DOXYGEN_SHOULD_SKIP_THIS
