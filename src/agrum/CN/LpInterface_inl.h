
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
 * @brief Class to include at least once this header
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */

#include <agrum/CN/LpInterface.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  INLINE Size
         HashFunc< credal::lp::LpCol >::castToSize(const credal::lp::LpCol& key) {
    return Size(key.id());
  }

  INLINE Size HashFunc< credal::lp::LpCol >::
              operator()(const credal::lp::LpCol& key) const {
    return (castToSize(key) * gum::HashFuncConst::gold) & this->_hash_mask;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
