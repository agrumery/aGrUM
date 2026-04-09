/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3imit.html)    *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 * @brief Generic Markov-blanket algorithm for directed graphs.
 *
 * `markovBlanket(g, node, level)` builds the level-@p level Markov blanket
 * of @p node in @p g as a DAG.  Level 1 (default): parents ∪ children ∪
 * co-parents (parents of children).  Level k > 1: iterates on newly added
 * nodes up to k times.
 *
 * Constrained by GUM_DiGraphable — works on any directed graph whose nodes
 * expose `parents()`, `children()`, and `exists()`.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_MARKOV_BLANKET_H
#define GUM_GRAPH_MARKOV_BLANKET_H

#include <agrum/agrum.h>
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/graphConcepts.h>
#include <agrum/base/graphs/graphElements.h>

namespace gum::graph {

  /**
   * @brief Returns the level-@p level Markov blanket of @p node in @p g as a DAG.
   *
   * The level-1 Markov blanket contains:
   *   - all parents of @p node,
   *   - all children of @p node,
   *   - all co-parents (other parents of each child of @p node).
   *
   * For level k > 1, the same construction is applied iteratively to all
   * newly added nodes at each level, stopping early if no new node is found.
   *
   * @tparam G Any GUM_DiGraphable graph (typically a DAG or BayesNet).
   * @param g     The source directed graph.
   * @param node  The node whose Markov blanket is computed.
   * @param level Depth of the blanket (must be >= 1).
   * @return A DAG containing the structural Markov blanket.
   * @throw InvalidArgument if @p node does not exist in @p g, or level < 1.
   */
  template < GUM_DiGraphable G >
  DAG markovBlanket(const G& g, NodeId node, int level = 1) {
    if (!g.exists(node))
      GUM_ERROR(InvalidArgument, "Node " << node << " does not exist in the graph.")
    if (level < 1) GUM_ERROR(InvalidArgument, "level must be >= 1")

    DAG mb;

    // Adds node n and its parents/children/co-parents to mb.
    // Returns true if any new node was inserted.
    auto build = [&](NodeId n) -> bool {
      bool changed = false;

      if (!mb.existsNode(n)) {
        mb.addNodeWithId(n);
        changed = true;
      }

      for (const auto par: g.parents(n)) {
        if (!mb.existsNode(par)) {
          mb.addNodeWithId(par);
          changed = true;
        }
        mb.addArc(par, n);
      }

      for (const auto chi: g.children(n)) {
        if (!mb.existsNode(chi)) {
          mb.addNodeWithId(chi);
          changed = true;
        }
        mb.addArc(n, chi);

        for (const auto opar: g.parents(chi)) {
          if (opar == n) continue;
          if (!mb.existsNode(opar)) {
            mb.addNodeWithId(opar);
            changed = true;
          }
          mb.addArc(opar, chi);
        }
      }

      return changed;
    };

    NodeSet processed{node};
    build(node);

    for (int lv = 1; lv < level; ++lv) {
      const NodeSet todo = mb.nodes().asNodeSet() - processed;
      bool          any  = false;
      for (const auto n: todo) {
        processed.insert(n);
        if (build(n)) any = true;
      }
      if (!any) break;
    }

    return mb;
  }

}   // namespace gum::graph

#endif   // GUM_GRAPH_MARKOV_BLANKET_H
