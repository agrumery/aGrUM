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
/** @file
 * @brief A generic class to instantiate a subset of variables of a MultiDim table
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/config.h>
#include <typeinfo>


namespace gum {


  /// constructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimPartialInstantiation<T_DATA,TABLE>::MultiDimPartialInstantiation () {
    /// for debugging purposes
    GUM_CONSTRUCTOR( MultiDimPartialInstantiation );
  }


  /// copy constructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimPartialInstantiation<T_DATA,TABLE>::MultiDimPartialInstantiation
  ( const MultiDimPartialInstantiation<T_DATA,TABLE>& from ) {
    /// for debugging purposes
    GUM_CONS_CPY( MultiDimPartialInstantiation );
  }


  /// destructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimPartialInstantiation<T_DATA,TABLE>::~MultiDimPartialInstantiation() {
    /// for debugging purposes
    GUM_DESTRUCTOR( MultiDimPartialInstantiation );
  }


  /// virtual constructor
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimPartialInstantiation<T_DATA,TABLE>*
  MultiDimPartialInstantiation<T_DATA,TABLE>::newFactory() const {
    return new MultiDimPartialInstantiation<T_DATA,TABLE> ( *this );
  }

  
  /// copy operator
  template< typename T_DATA, template<typename> class TABLE >
  MultiDimPartialInstantiation<T_DATA,TABLE>&
  MultiDimPartialInstantiation<T_DATA,TABLE>::operator=
  ( const MultiDimPartialInstantiation<T_DATA,TABLE>& from ) {
    return *this;
  }


  /// creates and returns the partial instantiation of the table
  template< typename T_DATA, template<typename> class TABLE >
  TABLE<T_DATA>*
  MultiDimPartialInstantiation<T_DATA,TABLE>::instantiate
  ( const TABLE<T_DATA>& table,
    const HashTable<const DiscreteVariable *,Idx>& inst_vars ) {
    MultiDimImplementation<T_DATA>* new_impl =
      partialInstantiation ( table, inst_vars );
    return new TABLE<T_DATA> ( new_impl );
  }

  /// creates and returns the partial instantiation of the table
  template< typename T_DATA, template<typename> class TABLE >
  void
  MultiDimPartialInstantiation<T_DATA,TABLE>::instantiate
  ( TABLE<T_DATA>& container,
    const TABLE<T_DATA>& table,
    const HashTable<const DiscreteVariable *,Idx>& inst_vars ) {
    MultiDimImplementation<T_DATA>* new_impl =
      partialInstantiation ( table, inst_vars );
    container = *new_impl;
    delete new_impl;
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
