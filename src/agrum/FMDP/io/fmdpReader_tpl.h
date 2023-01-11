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


/** @file
 * @brief Templates implementation of FMDP/io/FMDPReader.h classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {


  /* ============================================================================
   */
  /* ===                           GUM_FMDP_WRITER === */
  /* ============================================================================
   */
  template < typename GUM_SCALAR >
  INLINE FMDPReader< GUM_SCALAR >::FMDPReader(FMDP< GUM_SCALAR >* FMDP,
                                              const std::string&  filename) {
    GUM_CONSTRUCTOR(FMDPReader);
  }

  template < typename GUM_SCALAR >
  INLINE FMDPReader< GUM_SCALAR >::~FMDPReader() {
    GUM_DESTRUCTOR(FMDPReader);
  }

} /* namespace gum */


#endif   // DOXYGEN_SHOULD_SKIP_THIS
