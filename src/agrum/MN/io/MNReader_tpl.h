
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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


/** @file
 * @brief Templates implementation of bns/io/gumMNReader.h classes.
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =========================================================================*/
  /* ===                           GUM_BN_WRITER === */
  /* =========================================================================*/
  template < typename GUM_SCALAR >
  INLINE MNReader< GUM_SCALAR >::MNReader(MarkovNet< GUM_SCALAR >* MN,
                                          const std::string&      filename) {
    GUM_CONSTRUCTOR(MNReader);
  }

  template < typename GUM_SCALAR >
  INLINE MNReader< GUM_SCALAR >::~MNReader() {
    GUM_DESTRUCTOR(MNReader);
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS