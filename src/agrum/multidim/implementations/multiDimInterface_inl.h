
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief Inline implementation of gumMultiDimInterface
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */

// to ease IDE parser
#include <agrum/multidim/implementations/multiDimInterface.h>

namespace gum {
  // Swap two variables in this multidim.
  INLINE
  void MultiDimInterface::replace(const DiscreteVariable& x,
                                  const DiscreteVariable& y) {
    if (!contains(x)) { GUM_ERROR(NotFound, "could not find the variable"); }

    if (contains(y)) {
      GUM_ERROR(DuplicateElement, "variable " << y << " already in MultiDim");
    }

    if (x.domainSize() != y.domainSize()) {
      GUM_ERROR(OperationNotAllowed, "incompatible variables");
    }

    _replace(&x, &y);
  }

} /* namespace gum */
