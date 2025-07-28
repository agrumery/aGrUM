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
 * @brief Implementation of the Pattern class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/PRM/gspan/pattern.h>

#ifdef GUM_NO_INLINE
#  include <agrum/PRM/gspan/pattern_inl.h>
#endif   // GUM_NO_INLINE

namespace gum::prm::gspan {

  Pattern::Pattern(const Pattern& source) : DiGraph() {
    GUM_CONS_CPY(Pattern);
    NodeProperty< NodeId > node_map;

    for (NodeId node = 1; node <= source.size(); ++node) {
      node_map.insert(node, addNodeWithLabel(const_cast< LabelData& >(source.label(node))));
    }

    for (const auto& edge: source.code().codes)
      addArc(node_map[edge->i],
             node_map[edge->j],
             const_cast< LabelData& >(source.label(node_map[edge->i], node_map[edge->j])));
  }

  bool Pattern::isMinimal() {
    for (const auto node: nodes()) {
      for (const auto next: parents(node)) {
        Size     u = label(node).id;
        Size     v = label(next).id;
        EdgeCode edge_code(1, 2, u, label(next, node).id, v);

        if (edge_code < *(code().codes.front())) {
          return false;
        } else if (edge_code == (*code().codes.front())) {
          if (_expandCodeIsMinimal_(node, next)) { return false; }
        }
      }

      for (const auto next: children(node)) {
        Size     u = label(node).id;
        Size     v = label(next).id;
        EdgeCode edge_code(1, 2, u, label(node, next).id, v);

        if (edge_code < *(code().codes.front())) {
          return false;
        } else if (edge_code == (*code().codes.front())) {
          if (_expandCodeIsMinimal_(node, next)) { return false; }
        }
      }
    }

    return true;
  }

  std::string Pattern::toDot(size_t name) const {
    std::stringstream sBuff;
    sBuff << "digraph " << name << " {\n";

    for (const auto& arc: arcs()) {
      sBuff << label(arc.tail()).id << " -> ";
      sBuff << label(arc.head()).id << ";\n";
    }

    sBuff << "}\n";
    return sBuff.str();
  }

  bool Pattern::_expandCodeIsMinimal_(NodeId u, NodeId v) {
    Bijection< NodeId, NodeId > node_map;
    Pattern                     p;
    node_map.insert(u, p.addNodeWithLabel(label(u)));
    node_map.insert(v, p.addNodeWithLabel(label(v)));

    try {
      p.addArc(1, 2, label(u, v));
    } catch (NotFound const&) { p.addArc(1, 2, label(v, u)); }

    for (const auto nei: children(u))
      if (nei != v)
        if (_rec_(p, node_map, u, nei)) return true;

    for (const auto nei: parents(u))
      if (nei != v)
        if (_rec_(p, node_map, u, nei)) return true;

    for (const auto nei: children(v))
      if (nei != u)
        if (_rec_(p, node_map, v, nei)) return true;

    for (const auto nei: parents(v))
      if (nei != u)
        if (_rec_(p, node_map, v, nei)) return true;

    return false;
  }

  bool Pattern::_rec_(Pattern& p, Bijection< NodeId, NodeId >& node_map, NodeId u, NodeId v) {
    if (node_map.existsFirst(v)) {
      if (node_map.second(u) < node_map.second(v)) {
        // Invalid forward edge
        return false;
      } else if ((p.existsArc(node_map.second(u), node_map.second(v)))
                 || (p.existsArc(node_map.second(v), node_map.second(u)))) {
        // Duplicate arc !
        return false;
      }
    } else {
      node_map.insert(v, p.addNodeWithLabel(label(v)));
    }

    // Retrieving arc label data
    LabelData* data = 0;

    try {
      data = &(label(u, v));
    } catch (NotFound const&) { data = &(label(v, u)); }

    // Adding arc
    try {
      p.addArc(node_map.second(u), node_map.second(v), *data);
    } catch (OperationNotAllowed const&) {
      // Invalid neighbor
      if (node_map.second(u) < node_map.second(v)) {
        p.remove(node_map.second(v));
        node_map.eraseFirst(v);
      }

      return false;
    }

    // Check if this is minimal or if equal find another growth
    if (size_t depth = p.code().codes.size() - 1;
        *(p.code().codes.back()) < *(code().codes[depth])) {
      return true;
    } else if (*(p.code().codes.back()) == *(code().codes[depth])) {
      std::list< NodeId > r_path;
      p.rightmostPath(r_path);

      for (const auto node: r_path) {
        for (const auto nei: children(node_map.first(node)))
          if (_rec_(p, node_map, node_map.first(node), nei)) return true;

        for (const auto nei: parents(node_map.first(node)))
          if (_rec_(p, node_map, node_map.first(node), nei)) return true;
      }
    }

    if (p.code().codes.back()->isForward()) node_map.eraseFirst(v);

    p.pop_back();
    return false;
  }

  bool Pattern::_not_rec_(Pattern&                     p,
                          Bijection< NodeId, NodeId >& node_map,
                          NodeId                       a_u,
                          NodeId                       a_v) {
    std::vector< std::pair< NodeId, NodeId > > stack;
    stack.emplace_back(a_u, a_v);
    NodeId u = 0;
    NodeId v = 0;

    while (!stack.empty()) {
      bool go = true;
      u       = stack.back().first;
      v       = stack.back().second;
      stack.pop_back();

      if ((u == 0) && (v == 0)) {
        p.pop_back();
      } else {
        if (node_map.existsFirst(v)) {
          if (node_map.second(u) < node_map.second(v)) {
            // Invalid forward edge
            go = false;
          } else if ((p.existsArc(node_map.second(u), node_map.second(v)))
                     || (p.existsArc(node_map.second(v), node_map.second(u)))) {
            // Duplicate arc !
            go = false;
          }
        } else {
          node_map.insert(v, p.addNodeWithLabel(label(v)));
        }

        if (go) {
          // Retrieving arc label data
          LabelData* data = 0;

          try {
            data = &(label(u, v));
          } catch (NotFound const&) { data = &(label(v, u)); }

          // Adding arc
          try {
            p.addArc(node_map.second(u), node_map.second(v), *data);
          } catch (OperationNotAllowed const&) {
            // Invalid neighbor
            if (node_map.second(u) < node_map.second(v)) {
              p.remove(node_map.second(v));
              node_map.eraseFirst(v);
            }

            go = false;
          }

          if (go) {
            // Check if this is minimal or if equal find another growth
            if (size_t depth = p.code().codes.size() - 1;
                *(p.code().codes.back()) < *(code().codes[depth])) {
              return true;
            } else if (*(p.code().codes.back()) == *(code().codes[depth])) {
              std::list< NodeId > r_path;
              p.rightmostPath(r_path);
              stack.emplace_back((NodeId)0, (NodeId)0);

              for (const auto node: r_path) {
                for (const auto nei: children(node)) {
                  stack.emplace_back(node_map.first(node), nei);
                }

                for (const auto nei: parents(node)) {
                  stack.emplace_back(node_map.first(node), nei);
                }
              }
            }

            if (p.code().codes.back()->isForward()) node_map.eraseFirst(v);
          }
        }
      }
    }

    return false;
  }

}   // namespace gum::prm::gspan
