/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <agrum/PRM/utils_prm.h>

namespace gum {
  namespace prm {

    // Decompose a string in a vector of strings using "." as separators.
    void decomposePath(const std::string& path, std::vector< std::string >& v) {
      size_t prev   = 0;
      size_t length = 0;
      size_t idx_1  = path.find(".");
      size_t idx_2  = path.find(PRMObject::LEFT_CAST());

      if (idx_2 == std::string::npos) {
        // ignore safe names
        size_t idx = idx_1;

        while (idx != std::string::npos) {
          length = idx - prev;
          v.push_back(path.substr(prev, length));
          prev = idx + 1;
          idx  = path.find(".", prev);
        }
      } else {
        size_t tmp = 0;

        while (idx_1 != std::string::npos) {
          if (idx_1 < idx_2) {
            length = idx_1 - prev;
            v.push_back(path.substr(prev, length));
            prev  = idx_1 + 1;
            idx_1 = path.find(".", prev);
          } else if (idx_2 < idx_1) {
            tmp   = path.find(PRMObject::RIGHT_CAST(), idx_2);
            idx_1 = path.find(".", tmp);
            idx_2 = path.find(PRMObject::LEFT_CAST(), tmp);
          }
        }
      }

      v.push_back(path.substr(prev, std::string::npos));
    }

    NodeId nextNodeId() {
      static NodeId id = 0;
      return ++id;
    }

  } /* namespace prm */
}   // namespace gum
