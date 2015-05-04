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
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimPartialInstantiation<GUM_SCALAR, TABLE>::MultiDimPartialInstantiation() {
    /// for debugging purposes
    GUM_CONSTRUCTOR(MultiDimPartialInstantiation);
  }

  /// copy constructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimPartialInstantiation<GUM_SCALAR, TABLE>::MultiDimPartialInstantiation(
      const MultiDimPartialInstantiation<GUM_SCALAR, TABLE> &from) {
    /// for debugging purposes
    GUM_CONS_CPY(MultiDimPartialInstantiation);
  }

  /// destructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimPartialInstantiation<GUM_SCALAR, TABLE>::~MultiDimPartialInstantiation() {
    /// for debugging purposes
    GUM_DESTRUCTOR(MultiDimPartialInstantiation);
  }

  /// virtual constructor
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimPartialInstantiation<GUM_SCALAR, TABLE> *
  MultiDimPartialInstantiation<GUM_SCALAR, TABLE>::newFactory() const {
    return new MultiDimPartialInstantiation<GUM_SCALAR, TABLE>(*this);
  }

  /// copy operator
  template <typename GUM_SCALAR, template <typename> class TABLE>
  MultiDimPartialInstantiation<GUM_SCALAR, TABLE> &
      MultiDimPartialInstantiation<GUM_SCALAR, TABLE>::
      operator=(const MultiDimPartialInstantiation<GUM_SCALAR, TABLE> &from) {
    return *this;
  }

  /// creates and returns the partial instantiation of the table
  template <typename GUM_SCALAR, template <typename> class TABLE>
  TABLE<GUM_SCALAR> *MultiDimPartialInstantiation<GUM_SCALAR, TABLE>::instantiate(
      const TABLE<GUM_SCALAR> &table,
      const HashTable<const DiscreteVariable *, Idx> &inst_vars) {
    MultiDimImplementation<GUM_SCALAR> *new_impl =
        partialInstantiation(table, inst_vars);
    return new TABLE<GUM_SCALAR>(new_impl);
  }

  /// creates and returns the partial instantiation of the table
  template <typename GUM_SCALAR, template <typename> class TABLE>
  void MultiDimPartialInstantiation<GUM_SCALAR, TABLE>::instantiate(
      TABLE<GUM_SCALAR> &container, const TABLE<GUM_SCALAR> &table,
      const HashTable<const DiscreteVariable *, Idx> &inst_vars) {
    MultiDimImplementation<GUM_SCALAR> *new_impl =
        partialInstantiation(table, inst_vars);
    container = *new_impl;
    delete new_impl;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
