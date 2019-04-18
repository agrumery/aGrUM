
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
 * @brief A generic class to combine efficiently several MultiDim tables.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  // Constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombination< GUM_SCALAR, TABLE >::MultiDimCombination() {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimCombination);
  }

  // Copy constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombination< GUM_SCALAR, TABLE >::MultiDimCombination(
     const MultiDimCombination< GUM_SCALAR, TABLE >& from) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimCombination);
  }

  // Destructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombination< GUM_SCALAR, TABLE >::~MultiDimCombination() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimCombination);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
