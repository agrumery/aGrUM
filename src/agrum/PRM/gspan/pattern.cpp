/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Implementation of the Pattern class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/gspan/pattern.h>

#ifdef GUM_NO_INLINE
#include <agrum/PRM/gspan/pattern.inl>
#endif // GUM_NO_INLINE

namespace gum {
  namespace prm {
    namespace gspan {

      Pattern::Pattern(const Pattern &source) : DiGraph(), __last(0) {
        GUM_CONS_CPY(Pattern);
        NodeProperty<NodeId> node_map;

        for (NodeId node = 1; node <= source.size(); ++node) {
          node_map.insert(node,
                          addNode(const_cast<LabelData &>(source.label(node))));
        }

        for (const auto edge : source.code().codes)
          addArc(node_map[edge->i], node_map[edge->j],
                 const_cast<LabelData &>(
                     source.label(node_map[edge->i], node_map[edge->j])));
      }

      bool Pattern::isMinimal() {
        for (const auto node : nodes()) {
          for (const auto next : parents(node)) {
            Size u = label(node).id;
            Size v = label(next).id;
            EdgeCode edge_code(1, 2, u, label(next, node).id, v);

            if (edge_code < *(code().codes.front())) {
              return false;
            } else if (edge_code == (*code().codes.front())) {
              if (__expandCodeIsMinimal(node, next)) {
                return false;
              }
            }
          }

          for (const auto next : children(node)) {
            Size u = label(node).id;
            Size v = label(next).id;
            EdgeCode edge_code(1, 2, u, label(node, next).id, v);

            if (edge_code < *(code().codes.front())) {
              return false;
            } else if (edge_code == (*code().codes.front())) {
              if (__expandCodeIsMinimal(node, next)) {
                return false;
              }
            }
          }
        }

        return true;
      }

      std::string Pattern::toDot(size_t name) const {
        std::stringstream sBuff;
        sBuff << "digraph " << name << " {\n";

        for (const auto arc : arcs()) {
          sBuff << label(arc.tail()).id << " -> ";
          sBuff << label(arc.head()).id << ";\n";
        }

        sBuff << "}\n";
        return sBuff.str();
      }

      bool Pattern::__expandCodeIsMinimal(NodeId u, NodeId v) {
        Bijection<NodeId, NodeId> node_map;
        Pattern p;
        node_map.insert(u, p.addNode(label(u)));
        node_map.insert(v, p.addNode(label(v)));

        try {
          p.addArc(1, 2, label(u, v));
        } catch (NotFound &) {
          p.addArc(1, 2, label(v, u));
        }

        for (const auto nei : children(u))
          if (nei != v)
            if (__rec(p, node_map, u, nei))
              return true;

        for (const auto nei : parents(u))
          if (nei != v)
            if (__rec(p, node_map, u, nei))
              return true;

        for (const auto nei : children(v))
          if (nei != u)
            if (__rec(p, node_map, v, nei))
              return true;

        for (const auto nei : parents(v))
          if (nei != u)
            if (__rec(p, node_map, v, nei))
              return true;

        return false;
      }

      bool Pattern::__rec(Pattern &p, Bijection<NodeId, NodeId> &node_map, NodeId u,
                          NodeId v) {
        if (node_map.existsFirst(v)) {
          if (node_map.second(u) < node_map.second(v)) {
            // Invalid forward edge
            return false;
          } else if ((p.existsArc(node_map.second(u), node_map.second(v))) or
                     (p.existsArc(node_map.second(v), node_map.second(u)))) {
            // Duplicate arc !
            return false;
          }
        } else {
          node_map.insert(v, p.addNode(label(v)));
        }

        // Retrieving arc label data
        LabelData *data = 0;

        try {
          data = &(label(u, v));
        } catch (NotFound &) {
          data = &(label(v, u));
        }

        // Adding arc
        try {
          p.addArc(node_map.second(u), node_map.second(v), *data);
        } catch (OperationNotAllowed &) {
          // Invalid neighbor
          if (node_map.second(u) < node_map.second(v)) {
            p.remove(node_map.second(v));
            node_map.eraseFirst(v);
          }

          return false;
        }

        // Check if this is minimal or if equal find another growth
        size_t depth = p.code().codes.size() - 1;

        if (*(p.code().codes.back()) < *(code().codes[depth])) {
          return true;
        } else if (*(p.code().codes.back()) == *(code().codes[depth])) {
          std::list<NodeId> r_path;
          p.rightmostPath(r_path);

          for (const auto node : r_path) {
            for (const auto nei : children(node_map.first(node)))
              if (__rec(p, node_map, node_map.first(node), nei))
                return true;

            for (const auto nei : parents(node_map.first(node)))
              if (__rec(p, node_map, node_map.first(node), nei))
                return true;
          }
        }

        if (p.code().codes.back()->isForward())
          node_map.eraseFirst(v);

        p.pop_back();
        return false;
      }

      bool Pattern::__not_rec(Pattern &p, Bijection<NodeId, NodeId> &node_map,
                              NodeId a_u, NodeId a_v) {
        std::vector<std::pair<NodeId, NodeId>> stack;
        std::vector<size_t> rec_call;
        stack.push_back(std::make_pair(a_u, a_v));
        NodeId u = 0;
        NodeId v = 0;
        bool go = true;

        while (not stack.empty()) {
          ///////////////////////////////////
          std::stringstream stack_str;
          stack_str << "(";

          for (size_t idx = 0; idx < stack.size(); ++idx) {
            stack_str << stack[idx] << ", ";
          }

          GUM_TRACE_VAR(stack_str.str());
          GUM_TRACE_VAR(code());
          GUM_TRACE_VAR(p.code());
          GUM_TRACE_VAR(node_map.size());
          std::stringstream sBuff;

          for (auto iter = node_map.begin(); iter != node_map.end(); ++iter)
            sBuff << "(" << iter.first() << ", " << iter.second() << ") ";

          GUM_TRACE(sBuff.str());
          GUM_TRACE_VAR(stack.back().first);
          GUM_TRACE_VAR(stack.back().second);
          ///////////////////////////////////
          go = true;
          u = stack.back().first;
          v = stack.back().second;
          stack.pop_back();

          if ((u == 0) and (v == 0)) {
            p.pop_back();
          } else {
            if (node_map.existsFirst(v)) {
              if (node_map.second(u) < node_map.second(v)) {
                // Invalid forward edge
                go = false;
              } else if ((p.existsArc(node_map.second(u), node_map.second(v))) or
                         (p.existsArc(node_map.second(v), node_map.second(u)))) {
                // Duplicate arc !
                go = false;
              }
            } else {
              node_map.insert(v, p.addNode(label(v)));
            }

            if (go) {
              // Retrieving arc label data
              LabelData *data = 0;

              try {
                data = &(label(u, v));
              } catch (NotFound &) {
                data = &(label(v, u));
              }

              // Adding arc
              try {
                p.addArc(node_map.second(u), node_map.second(v), *data);
              } catch (OperationNotAllowed &) {
                // Invalid neighbor
                if (node_map.second(u) < node_map.second(v)) {
                  p.remove(node_map.second(v));
                  node_map.eraseFirst(v);
                }

                go = false;
              }

              if (go) {
                // Check if this is minimal or if equal find another growth
                size_t depth = p.code().codes.size() - 1;

                if (*(p.code().codes.back()) < *(code().codes[depth])) {
                  return true;
                } else if (*(p.code().codes.back()) == *(code().codes[depth])) {
                  std::list<NodeId> r_path;
                  p.rightmostPath(r_path);
                  stack.push_back(std::make_pair((NodeId)0, (NodeId)0));

                  for (const auto node : r_path) {
                    for (const auto nei : children(node)) {
                      stack.push_back(std::make_pair(node_map.first(node), nei));
                      ++(rec_call.back());
                    }

                    for (const auto nei : parents(node)) {
                      stack.push_back(std::make_pair(node_map.first(node), nei));
                      ++(rec_call.back());
                    }
                  }
                }

                if (p.code().codes.back()->isForward())
                  node_map.eraseFirst(v);
              }
            }
          }
        }

        return false;
      }

    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */
