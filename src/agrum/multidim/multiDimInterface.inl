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
 * @brief Inline implementation of gumMultiDimInterface
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

// to ease IDE parser
#include <agrum/multidim/multiDimInterface.h>

namespace gum {

  // Iterator pointing at the beginning of the Sequence of variables.
  INLINE
  MultiDimInterface::iterator MultiDimInterface::begin() const {
    return variablesSequence().begin();
  }

  // Iterator pointing at the rbeginning of the Sequence of variables.
  INLINE
  MultiDimInterface::iterator MultiDimInterface::rbegin() const {
    return variablesSequence().rbegin();
  }

  // Constant reference on the iterator pointing at the end of the Sequence
  // of variables.
  INLINE
  const MultiDimInterface::iterator &MultiDimInterface::end() const {
    return variablesSequence().end();
  }

  // Constant reference on the iterator pointing at the rend of the Sequence
  // of variables.
  INLINE
  const MultiDimInterface::iterator &MultiDimInterface::rend() const {
    return variablesSequence().rend();
  }

  // Swap two variables in this multidim.
  INLINE
  void MultiDimInterface::swap(const DiscreteVariable &x,
                               const DiscreteVariable &y) {
    if (not contains(x)) {
      GUM_ERROR(NotFound, "could not find the variable");
    }

    if (contains(y)) {
      GUM_ERROR(DuplicateElement, "variable already in MultiDim");
    }

    if (x.domainSize() != y.domainSize()) {
      GUM_ERROR(OperationNotAllowed, "incompatible variables");
    }

    _swap(&x, &y);
  }

} /* namespace gum */
