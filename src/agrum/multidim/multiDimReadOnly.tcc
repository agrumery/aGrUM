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

namespace gum {

  // Default constructor

  template <typename GUM_SCALAR>
  INLINE MultiDimReadOnly<GUM_SCALAR>::MultiDimReadOnly()
      : MultiDimImplementation<GUM_SCALAR>() {
    GUM_CONSTRUCTOR(MultiDimReadOnly);
  }

  // Default constructor

  template <typename GUM_SCALAR>
  INLINE MultiDimReadOnly<GUM_SCALAR>::MultiDimReadOnly(
      const MultiDimReadOnly<GUM_SCALAR> &from)
      : MultiDimImplementation<GUM_SCALAR>(from) {
    GUM_CONS_CPY(MultiDimReadOnly);
  }

  // destructor

  template <typename GUM_SCALAR>
  INLINE MultiDimReadOnly<GUM_SCALAR>::~MultiDimReadOnly() {
    GUM_DESTRUCTOR(MultiDimReadOnly);
  }

  // @throw OperationNotAllowed Raised because this is a read only table.

  template <typename GUM_SCALAR>
  INLINE void MultiDimReadOnly<GUM_SCALAR>::set(const Instantiation &i,
                                                const GUM_SCALAR &value) const {
    GUM_ERROR(OperationNotAllowed, "Write access to an aggregator");
  }

  // @throw OperationNotAllowed Raised because this is a read only table.

  template <typename GUM_SCALAR>
  void MultiDimReadOnly<GUM_SCALAR>::fill(const GUM_SCALAR &) const {
    GUM_ERROR(OperationNotAllowed, "Write access to an aggregator");
  }

  // @throw OperationNotAllowed Raised because this is a read only table.

  template <typename GUM_SCALAR>
  GUM_SCALAR &MultiDimReadOnly<GUM_SCALAR>::_get(const Instantiation &i) const {
    GUM_ERROR(OperationNotAllowed, "No (unconst) access to an aggregator");
  }

} /* namespace gum */
