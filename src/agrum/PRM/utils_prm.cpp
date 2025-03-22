/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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
