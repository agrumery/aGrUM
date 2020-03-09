
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


/** @file
 * @brief A generic interface to combine and project efficiently MultiDim tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  // default constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombineAndProject< GUM_SCALAR, TABLE >::MultiDimCombineAndProject() {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimCombineAndProject);
  }

  // copy constructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombineAndProject< GUM_SCALAR, TABLE >::MultiDimCombineAndProject(
     const MultiDimCombineAndProject< GUM_SCALAR, TABLE >&) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimCombineAndProject);
  }

  // destructor
  template < typename GUM_SCALAR, template < typename > class TABLE >
  MultiDimCombineAndProject< GUM_SCALAR, TABLE >::~MultiDimCombineAndProject() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimCombineAndProject);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
