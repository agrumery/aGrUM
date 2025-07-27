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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Inline implementation of the DFSTree class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/PRM/gspan/edgeGrowth.h>

namespace gum {
  namespace prm {
    namespace gspan {

      template < typename GUM_SCALAR >
      INLINE EdgeGrowth< GUM_SCALAR >::EdgeGrowth(NodeId     a_u,
                                                  LabelData* an_edge,
                                                  LabelData* a_l_v,
                                                  NodeId     a_v) :
          u(a_u), edge(an_edge), l_v(a_l_v), v(a_v), degree_list(new std::vector< NodeId >()) {
        GUM_CONSTRUCTOR(EdgeGrowth);
      }

      template < typename GUM_SCALAR >
      INLINE EdgeGrowth< GUM_SCALAR >::EdgeGrowth(const EdgeGrowth< GUM_SCALAR >& from) :
          u(from.u), edge(from.edge), v(from.v), matches(from.matches), iso_graph(from.iso_graph),
          degree_list(0), max_indep_set(from.max_indep_set) {
        GUM_CONS_CPY(EdgeGrowth);

        if (from.degree_list != 0) { degree_list = new std::vector< NodeId >(*(from.degree_list)); }
      }

      template < typename GUM_SCALAR >
      INLINE EdgeGrowth< GUM_SCALAR >::~EdgeGrowth() {
        GUM_DESTRUCTOR(EdgeGrowth);

        if (degree_list != 0) { delete degree_list; }
      }

      template < typename GUM_SCALAR >
      INLINE std::string EdgeGrowth< GUM_SCALAR >::toString() {
        std::stringstream str;
        str << u << "-" << edge << "-" << l_v << "-" << v;
        return str.str();
      }

      template < typename GUM_SCALAR >
      void EdgeGrowth< GUM_SCALAR >::insert(PRMInstance< GUM_SCALAR >* u,
                                            PRMInstance< GUM_SCALAR >* v) {
        NodeId id = iso_graph.addNode();
        degree_list->push_back(id);

        for (const auto& elt: matches) {
          if ((elt.second.first == u) || (elt.second.second == u) || (elt.second.first == v)
              || (elt.second.second == v)) {
            iso_graph.addEdge(elt.first, id);
          }
        }

        // The order between u and v is important ! DO NOT INVERSE IT !
        matches.insert(id, std::make_pair(u, v));
      }

    } /* namespace gspan */
  } /* namespace prm */
} /* namespace gum */
