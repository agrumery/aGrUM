/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Outlined implementation for utilities for aGrUM.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/core/utils_misc.h>

namespace std {
  std::ostream& operator<<(std::ostream& os, const std::tuple<>&) {
    return os << "()";
  }
}   // namespace std

namespace gum {

  bool Memcmp(const void* const in_, const void* const out_, unsigned long size) {
    unsigned char* in  = (unsigned char*)in_;
    unsigned char* out = (unsigned char*)out_;

    for (unsigned long i = 0; i < size; ++i)
      if (*(in++) != *(out++)) return false;

    return true;
  }

  void atexit__() {
#ifdef GUM_DEBUG_MODE
    gum::__debug__::atexit__();
#endif
  }

} /* namespace gum */
