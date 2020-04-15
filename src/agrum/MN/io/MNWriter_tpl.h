
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
 * @brief Templates implementation of bns/io/gumMNWriter.h classes.
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =========================================================================*/
  /* ===                           GUM_BN_WRITER                         === */
  /* =========================================================================*/
  template < typename GUM_SCALAR >
  INLINE MNWriter< GUM_SCALAR >::MNWriter() {
    GUM_CONSTRUCTOR(MNWriter);
  }

  template < typename GUM_SCALAR >
  INLINE MNWriter< GUM_SCALAR >::~MNWriter() {
    GUM_DESTRUCTOR(MNWriter);
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
