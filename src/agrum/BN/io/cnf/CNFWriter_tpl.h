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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing in IDE
#  include <agrum/BN/io/cnf/CNFWriter.h>

namespace gum {

  /* =========================================================================*/
  /* ===                           GUM_BN_WRITER === */
  /* =========================================================================*/
  // Default constructor.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE CNFWriter< GUM_SCALAR, IApproximationPolicy >::CNFWriter() {
    GUM_CONSTRUCTOR(CNFWriter);
  }

  // Default destructor.
  template < typename GUM_SCALAR, template < class > class IApproximationPolicy >
  INLINE CNFWriter< GUM_SCALAR, IApproximationPolicy >::~CNFWriter() {
    GUM_DESTRUCTOR(CNFWriter);
  }

} /*namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
