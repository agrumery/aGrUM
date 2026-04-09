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
 * @brief Generic minimal conditioning set algorithm for directed graphs.
 *
 * `minimalCondSet(g, target, soids)` computes the smallest subset S of
 * `soids` such that P(target | soids) = P(target | S) in a DAG context,
 * i.e. the nodes of `soids` that are actually "relevant" to target via
 * d-connected paths.
 *
 * Constrained by GUM_DiGraphable — works on any directed graph whose nodes
 * expose `parents()` and `children()`.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_MINIMAL_CONDITIONING_SET_H
#define GUM_GRAPH_MINIMAL_CONDITIONING_SET_H

#include <agrum/agrum.h>
#include <agrum/base/graphs/graphConcepts.h>
#include <agrum/base/graphs/graphElements.h>

namespace gum::graph {

  /// @cond INTERNAL

  // Forward declarations for mutual recursion.
  template < GUM_DiGraphable G >
  void _mcsVisitUp_(const G&       g,
                    NodeId         node,
                    const NodeSet& soids,
                    NodeSet&       minimal,
                    NodeSet&       visitedUp,
                    NodeSet&       visitedDn);

  template < GUM_DiGraphable G >
  void _mcsVisitDn_(const G&       g,
                    NodeId         node,
                    const NodeSet& soids,
                    NodeSet&       minimal,
                    NodeSet&       visitedUp,
                    NodeSet&       visitedDn);

  /**
   * Upward visit: if @p node is in @p soids, add it to @p minimal.
   * Otherwise continue upward via parents and downward via children.
   */
  template < GUM_DiGraphable G >
  void _mcsVisitUp_(const G&       g,
                    NodeId         node,
                    const NodeSet& soids,
                    NodeSet&       minimal,
                    NodeSet&       visitedUp,
                    NodeSet&       visitedDn) {
    if (visitedUp.contains(node)) return;
    visitedUp << node;

    if (soids.contains(node)) {
      minimal << node;
    } else {
      for (const auto par: g.parents(node))
        _mcsVisitUp_(g, par, soids, minimal, visitedUp, visitedDn);
      for (const auto chi: g.children(node))
        _mcsVisitDn_(g, chi, soids, minimal, visitedUp, visitedDn);
    }
  }

  /**
   * Downward visit: if @p node is in @p soids, add it and continue upward
   * via parents; otherwise continue downward via children.
   */
  template < GUM_DiGraphable G >
  void _mcsVisitDn_(const G&       g,
                    NodeId         node,
                    const NodeSet& soids,
                    NodeSet&       minimal,
                    NodeSet&       visitedUp,
                    NodeSet&       visitedDn) {
    if (visitedDn.contains(node)) return;
    visitedDn << node;

    if (soids.contains(node)) {
      minimal << node;
      for (const auto par: g.parents(node))
        _mcsVisitUp_(g, par, soids, minimal, visitedUp, visitedDn);
    } else {
      for (const auto chi: g.children(node))
        _mcsVisitDn_(g, chi, soids, minimal, visitedUp, visitedDn);
    }
  }

  /// @endcond


  /**
   * @brief Returns the minimal subset of @p soids that d-connects @p target.
   *
   * Computes the smallest S ⊆ @p soids such that
   * P(@p target | @p soids) = P(@p target | S) in the graph @p g.
   *
   * @tparam G Any GUM_DiGraphable graph.
   * @param g     The directed graph.
   * @param target The query node.
   * @param soids  The candidate conditioning set.
   * @return The minimal conditioning subset.
   */
  template < GUM_DiGraphable G >
  NodeSet minimalCondSet(const G& g, NodeId target, const NodeSet& soids) {
    if (soids.contains(target)) return NodeSet({target});

    NodeSet res, visitedUp, visitedDn;
    visitedUp << target;
    visitedDn << target;

    for (const auto par: g.parents(target))
      _mcsVisitUp_(g, par, soids, res, visitedUp, visitedDn);
    for (const auto chi: g.children(target))
      _mcsVisitDn_(g, chi, soids, res, visitedUp, visitedDn);

    return res;
  }

  /**
   * @brief Returns the minimal subset of @p soids that d-connects all @p targets.
   *
   * Union of `minimalCondSet(g, t, soids)` over each `t ∈ targets`.
   *
   * @tparam G Any GUM_DiGraphable graph.
   */
  template < GUM_DiGraphable G >
  NodeSet minimalCondSet(const G& g, const NodeSet& targets, const NodeSet& soids) {
    NodeSet res;
    for (const auto node: targets)
      res += minimalCondSet(g, node, soids);
    return res;
  }

}   // namespace gum::graph

#endif   // GUM_GRAPH_MINIMAL_CONDITIONING_SET_H
