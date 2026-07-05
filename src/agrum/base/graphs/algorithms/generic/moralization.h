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
 * @brief Moralization algorithms for aGrUM directed and mixed graphs.
 *
 * Both functions are constrained by GUM_DiGraphable and adapt their behaviour
 * at compile time via `if constexpr (GUM_MixedGraphable<G>)`:
 *
 * **moralGraph**
 *   - Pure directed graph (DAG): arcs → edges, then marry co-parents per node.
 *   - Mixed graph (PDAG / chain graph): arcs + edges → edges, then marry
 *     co-parents per *chain component* (nodes linked by undirected edges are
 *     treated as a single unit).
 *
 * **moralizedAncestralGraph**
 *   - Pure directed graph: BFS backward via parents only.
 *   - Mixed graph: BFS backward via parents *and* bidirectionally via edges,
 *     then moralizes the collected subgraph with `moralGraph`.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_MORALIZATION_H
#define GUM_GRAPH_MORALIZATION_H

#include <agrum/agrum.h>

#include <agrum/base/graphs/mixedGraph.h>

namespace gum::graph {

  /// @cond INTERNAL
  /// Add an edge between every pair of nodes in @p parents in @p g.
  inline void _marryParents_(const NodeSet& parents, UndiGraph& g);

  /// @endcond

  /**
   * @brief Returns the moral graph of @p g.
   *
   * For a **directed** graph (DAG):
   *   - every arc u→v becomes edge {u, v}
   *   - for every node v, an edge is added between every pair of parents
   *     ("marrying the parents")
   *
   * For a **mixed** graph (PDAG / chain graph):
   *   - every arc u→v and every edge u--v become edge {u, v}
   *   - for every *chain component* (maximal set of nodes connected by
   *     undirected edges), an edge is added between every pair of parents
   *     of any node in that component
   *
   * @tparam G Any GUM_DiGraphable graph.
   * @param g The graph to moralize.
   * @return The moral graph as an UndiGraph with the same node set as @p g.
   */
  template < GUM_DiGraphable G >
  UndiGraph moralGraph(const G& g);

  /**
   * @brief Returns the moralized ancestral graph of @p query in @p g.
   *
   * For a **directed** graph (DAG):
   *   Collects @p query and all ancestors reachable by following arcs
   *   backward, then moralizes the resulting subgraph.
   *
   * For a **mixed** graph (PDAG / chain graph):
   *   Collects @p query and all nodes reachable by following arcs backward
   *   *and* undirected edges in both directions, then moralizes with the
   *   chain-component rule.
   *
   * @tparam G Any GUM_DiGraphable graph.
   * @param g The graph.
   * @param query The set of nodes whose moralized ancestral graph is needed.
   * @return The moralized ancestral graph as an UndiGraph.
   */
  template < GUM_DiGraphable G >
  UndiGraph moralizedAncestralGraph(const G& g, const NodeSet& query);

}   // namespace gum::graph

#include <agrum/base/graphs/algorithms/generic/moralization_tpl.h>

#endif   // GUM_GRAPH_MORALIZATION_H
