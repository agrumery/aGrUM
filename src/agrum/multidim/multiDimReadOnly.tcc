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
 * @brief Implementation of the gum::MultiDimReadOnly class.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
// ============================================================================
namespace gum {
// ==============================================================================
// Default constructor
// ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimReadOnly<T_DATA>::MultiDimReadOnly(): MultiDimImplementation<T_DATA>() {
    GUM_CONSTRUCTOR( MultiDimReadOnly ) ;
  }

// ==============================================================================
// Default constructor
// ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimReadOnly<T_DATA>::MultiDimReadOnly( const MultiDimReadOnly<T_DATA>& from ) : MultiDimImplementation<T_DATA>( from ) {
    GUM_CONS_CPY( MultiDimReadOnly );
  }

// ==============================================================================
// destructor
// ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimReadOnly<T_DATA>::~MultiDimReadOnly() {
    GUM_DESTRUCTOR( MultiDimReadOnly );
  }

// ============================================================================
// @throw OperationNotAllowed Raised because this is a read only table.
// ============================================================================
  template<typename T_DATA> INLINE
  void
  MultiDimReadOnly<T_DATA>::set( const Instantiation& i,
                                 const T_DATA& value ) const {
    GUM_ERROR( OperationNotAllowed,"Write access to an aggregator" );
  }

// ============================================================================
// @throw OperationNotAllowed Raised because this is a read only table.
// ============================================================================
  template <typename T_DATA>
  void
  MultiDimReadOnly<T_DATA>::fill( const T_DATA& ) const {
    GUM_ERROR( OperationNotAllowed,"Write access to an aggregator" );
  }

// ============================================================================
// @throw OperationNotAllowed Raised because this is a read only table.
// ============================================================================
  template <typename T_DATA>
  T_DATA&
  MultiDimReadOnly<T_DATA>::_get( const Instantiation &i ) const  {
    GUM_ERROR( OperationNotAllowed,"No (unconst) access to an aggregator" );
  }
// ============================================================================
} /* namespace gum */
// ============================================================================
