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
 * @brief Implementation of the gumArcGraphPart
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/graphs/edgeGraphPart.h>


namespace gum {


  template <typename ASSENT> INLINE
  typename Property< ASSENT >::onEdges
    EdgeGraphPart::edgesProperty( ASSENT( *f )( const Edge& ), Size size ) const {
    return __edges.hashMap( f,size );
  }

  template <typename ASSENT> INLINE
  typename Property< ASSENT >::onEdges
    EdgeGraphPart::edgesProperty( const ASSENT& val, Size size ) const {
    return __edges.hashMap( val,size );
  }

  /// a method to create a list of Assent from a node list
  // ================================================================================
  template <typename ASSENT> INLINE
  List<ASSENT> EdgeGraphPart::listMapEdges( ASSENT( *f )( const Edge& ) ) const {
    return __edges.listMap( f );
  }


} /* namespace gum */

  
#endif    // DOXYGEN_SHOULD_SKIP_THIS
