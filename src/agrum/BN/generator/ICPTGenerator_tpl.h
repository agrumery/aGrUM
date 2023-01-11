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
 * @brief Source implementation of gumCPTGenerator.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/BN/generator/ICPTGenerator.h>

namespace gum {

  // Default constructor.
  template < typename GUM_SCALAR >
  ICPTGenerator< GUM_SCALAR >::ICPTGenerator() {
    GUM_CONSTRUCTOR(ICPTGenerator);
  }

  // Destructor.
  template < typename GUM_SCALAR >
  ICPTGenerator< GUM_SCALAR >::~ICPTGenerator() {
    GUM_DESTRUCTOR(ICPTGenerator);
  }

} /* namespace gum */
