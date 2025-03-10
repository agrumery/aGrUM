/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/**
 * @file
 * @brief Implementation of the EdgeCode class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/PRM/gspan/edgeCode.h>

#ifdef GUM_NO_INLINE
#  include <agrum/PRM/gspan/edgeCode_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {
  namespace prm {
    namespace gspan {

      std::ostream& operator<<(std::ostream& out, const EdgeCode& code) {
        out << "(" << code.i << ", " << code.j << ", " << code.l_i << ", ";
        out << code.l_ij << ", " << code.l_j << ")";
        return out;
      }

    } /* namespace gspan */
  } /* namespace prm */
} /* namespace gum */
