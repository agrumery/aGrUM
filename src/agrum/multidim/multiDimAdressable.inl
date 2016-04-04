/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Inline implementation for the abstract base class for all multi
 * dimensionnal containers.
 *
 * MultiDimAdressable is the abstract base class for all multi dimensionnal
 * adressable. Its purpose is to deal with slave Instantiation and notification
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

// to ease IDE parser
#include <agrum/multidim/multiDimAdressable.h>

namespace gum {

  INLINE
  MultiDimAdressable::MultiDimAdressable( MultiDimAdressable&& from )
      : MultiDimInterface( std::forward<MultiDimInterface>( from ) ) {
    GUM_CONS_MOV( MultiDimAdressable );
  }

  INLINE MultiDimAdressable::MultiDimAdressable() {
    GUM_CONSTRUCTOR( MultiDimAdressable );
  }

  INLINE
  MultiDimAdressable::MultiDimAdressable( const MultiDimAdressable& from ) {
    GUM_CONS_CPY( MultiDimAdressable );
  }

  INLINE MultiDimAdressable::~MultiDimAdressable() {
    GUM_DESTRUCTOR( MultiDimAdressable );
  }

  INLINE MultiDimAdressable& MultiDimAdressable::
  operator=( const MultiDimAdressable& from ) {
    GUM_OP_CPY( MultiDimAdressable );
    MultiDimInterface::operator=( from );
    return *this;
  }
  INLINE MultiDimAdressable& MultiDimAdressable::
  operator=( MultiDimAdressable&& from ) {
    GUM_OP_MOV( MultiDimAdressable );
    MultiDimInterface::operator=( std::forward<MultiDimInterface>( from ) );
    return *this;
  }

} /* namespace gum */
