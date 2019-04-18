
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
 * @brief Outlined implementation for utilities for aGrUM.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/core/utils_misc.h>

namespace std {
  std::ostream& operator<<(std::ostream& os, const std::tuple<>&) {
    return os << "()";
  }
}   // namespace std

namespace gum {

  bool Memcmp(const void* const _in, const void* const _out, unsigned long size) {
    unsigned char* in = (unsigned char*)_in;
    unsigned char* out = (unsigned char*)_out;

    for (unsigned long i = 0; i < size; ++i)
      if (*(in++) != *(out++)) return false;

    return true;
  }

  void __atexit() {
#ifdef GUM_DEBUG_MODE
    gum::__debug__::__atexit();
#endif
  }

} /* namespace gum */
