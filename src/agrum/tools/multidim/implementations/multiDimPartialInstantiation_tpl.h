/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Implementation of MultiDimPartialInstantiation.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/agrum.h>
#include <typeinfo>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimPartialInstantiation< GUM_SCALAR, TABLE >::MultiDimPartialInstantiation() {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimPartialInstantiation);
  }

  // copy constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimPartialInstantiation< GUM_SCALAR, TABLE >::MultiDimPartialInstantiation(
     const MultiDimPartialInstantiation< GUM_SCALAR, TABLE >& from) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimPartialInstantiation);
  }

  // destructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimPartialInstantiation< GUM_SCALAR, TABLE >::~MultiDimPartialInstantiation() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimPartialInstantiation);
  }

  // virtual constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimPartialInstantiation< GUM_SCALAR, TABLE >*
     MultiDimPartialInstantiation< GUM_SCALAR, TABLE >::newFactory() const {
    return new MultiDimPartialInstantiation< GUM_SCALAR, TABLE >(*this);
  }

  // copy operator
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimPartialInstantiation< GUM_SCALAR, TABLE >&
     MultiDimPartialInstantiation< GUM_SCALAR, TABLE >::operator=(
        const MultiDimPartialInstantiation< GUM_SCALAR, TABLE >& from) {
    return *this;
  }

  // creates and returns the partial instantiation of the table
  template < typename GUM_SCALAR, template < typename > class TABLE >
  TABLE< GUM_SCALAR >* MultiDimPartialInstantiation< GUM_SCALAR, TABLE >::instantiate(
     const TABLE< GUM_SCALAR >&                       table,
     const HashTable< const DiscreteVariable*, Idx >& inst_vars) {
    MultiDimImplementation< GUM_SCALAR >* new_impl = partialInstantiation(table, inst_vars);
    return new TABLE< GUM_SCALAR >(new_impl);
  }

  // creates and returns the partial instantiation of the table
  template < typename GUM_SCALAR, template < typename > class TABLE >
  void MultiDimPartialInstantiation< GUM_SCALAR, TABLE >::instantiate(
     TABLE< GUM_SCALAR >&                             container,
     const TABLE< GUM_SCALAR >&                       table,
     const HashTable< const DiscreteVariable*, Idx >& inst_vars) {
    MultiDimImplementation< GUM_SCALAR >* new_impl = partialInstantiation(table, inst_vars);
    container                                      = *new_impl;
    delete new_impl;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
