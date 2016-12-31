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
 * @brief Outlined implementation of ArgMaxSet
 *
 * @author Jean-Christophe MAGNAN
 */

#include <agrum/core/argMaxSet.h>


namespace gum {

  // ###########################################################################
  // CNL
  // ###########################################################################

  // ============================================================================
  // Constructor
  // ============================================================================
  template <typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ>
  ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>::ArgMaxSet() {
    GUM_CONSTRUCTOR( ArgMaxSet );
    __argMaxSeq = new Sequence<GUM_SCALAR_SEQ>();
  }

  // ============================================================================
  // Constructor
  // ============================================================================
  template <typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ>
  ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>::ArgMaxSet(
      const GUM_SCALAR_VAL& val, const GUM_SCALAR_SEQ& elem ) {
    GUM_CONSTRUCTOR( ArgMaxSet );
    __argMaxSeq = new Sequence<GUM_SCALAR_SEQ>();
    __argMaxSeq->insert( elem );
    __val = val;
  }

  // ============================================================================
  // Copy Constructor
  // ============================================================================
  template <typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ>
  ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>::ArgMaxSet(
      const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& src ) {
    GUM_CONS_CPY( ArgMaxSet );
    __argMaxSeq = new Sequence<GUM_SCALAR_SEQ>();
    this->operator+=( src );
    __val = src.value();
  }

  template <typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ>
  ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>&
  ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>::
  operator=( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& src ) {
    this->__argMaxSeq->clear();
    this->operator+=( src );
    __val = src.value();
    return *this;
  }

  // ============================================================================
  // Destructor
  // ============================================================================
  template <typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ>
  ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>::~ArgMaxSet() {
    GUM_DESTRUCTOR( ArgMaxSet )
    delete __argMaxSeq;
  }

  // ###########################################################################
  // Operators
  // ###########################################################################

  // ============================================================================
  // Ajout d'un élément
  // ============================================================================
  template <typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ>
  ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>&
  ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>::
  operator+=( const GUM_SCALAR_SEQ& elem ) {
    __argMaxSeq->insert( elem );
    return *this;
  }

  // ============================================================================
  // Use to insert the content of another set inside this one
  // ============================================================================
  template <typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ>
  ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>&
  ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>::
  operator+=( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& src ) {
    for ( auto iter = src.beginSafe(); iter != src.endSafe(); ++iter )
      if ( !__argMaxSeq->exists( *iter ) ) __argMaxSeq->insert( *iter );
    return *this;
  }

  // ============================================================================
  // Compares two ArgMaxSet to check if they are equals
  // ============================================================================
  template <typename GUM_SCALAR_VAL, typename GUM_SCALAR_SEQ>
  bool ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>::
  operator==( const ArgMaxSet<GUM_SCALAR_VAL, GUM_SCALAR_SEQ>& compared ) const {
    if ( __val != compared.value() ) return false;
    for ( auto iter = compared.beginSafe(); iter != compared.endSafe(); ++iter )
      if ( !__argMaxSeq->exists( *iter ) ) return false;
    for ( auto iter = this->beginSafe(); iter != this->endSafe(); ++iter )
      if ( !compared.exists( *iter ) ) return false;
    return true;
  }

}  // End of namespace gum
