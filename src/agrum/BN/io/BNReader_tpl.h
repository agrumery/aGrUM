
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
 * @brief Templates implementation of bns/io/gumBNReader.h classes.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =========================================================================*/
  /* ===                           GUM_BN_WRITTER === */
  /* =========================================================================*/
  template < typename GUM_SCALAR >
  INLINE BNReader< GUM_SCALAR >::BNReader(BayesNet< GUM_SCALAR >* bn,
                                          const std::string&      filename) {
    GUM_CONSTRUCTOR(BNReader);
  }

  template < typename GUM_SCALAR >
  INLINE BNReader< GUM_SCALAR >::~BNReader() {
    GUM_DESTRUCTOR(BNReader);
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
