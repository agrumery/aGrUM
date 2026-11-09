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
 * @brief Generic algorithms on chordal graphs: lexicographic breadth-first search, maximum
 * cardinality search and perfect elimination orderings.
 *
 * All functions live in namespace gum::graph and work on the undirected edges of the graph
 * (GUM_UndiGraphable): for a mixed graph, the arcs are ignored.
 *
 * A graph is chordal if every cycle of length at least 4 has a chord. A perfect elimination
 * ordering (v1, ..., vn) is an ordering of the nodes such that, for every i, the neighbours of vi
 * among v(i+1), ..., vn form a clique: a graph is chordal iff it has one (Fulkerson and Gross 1965;
 * Rose, Tarjan and Lueker 1976). The reverse of the visit order of a lexicographic breadth-first
 * search (Rose, Tarjan and Lueker 1976) or of a maximum cardinality search (Tarjan and Yannakakis
 * 1984) of a chordal graph is a perfect elimination ordering.
 *
 * @par References
 * - D. J. Rose, R. E. Tarjan, G. S. Lueker. Algorithmic aspects of vertex elimination on graphs.
 *   SIAM Journal on Computing 5(2):266-283, 1976. doi:10.1137/0205021.
 * - R. E. Tarjan, M. Yannakakis. Simple linear-time algorithms to test chordality of graphs, test
 *   acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs. SIAM Journal on
 * Computing 13(3):566-579, 1984. doi:10.1137/0213035.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_CHORDALITY_H
#define GUM_GRAPH_CHORDALITY_H

#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/sequence.h>
#include <agrum/base/graphs/graphConcepts.h>

namespace gum::graph {

  /**
   * @brief Visit order of a lexicographic breadth-first search (LexBFS) of the undirected edges of
   * @p g.
   *
   * At each step, the visited node is an unvisited node whose label (the decreasing list of the
   * visit steps of its visited neighbours) is lexicographically the largest, the ties being broken
   * by the order of the nodes in g.nodes(). Partition refinement implementation in O(n + m)
   * (Rose, Tarjan and Lueker 1976). The reverse of the returned sequence is a perfect elimination
   * ordering iff @p g is chordal.
   *
   * @param g the graph.
   * @param first a clique of @p g, whose nodes are visited first and in this order (always a valid
   * LexBFS: the label of the next node of a clique is the largest possible one).
   * @throw InvalidArgument if @p first is not a clique of @p g.
   */
  template < GUM_UndiGraphable G >
  Sequence< NodeId > lexBFS(const G& g, const std::vector< NodeId >& first = {});

  /**
   * @brief Visit order of a maximum cardinality search (MCS) of the undirected edges of @p g.
   *
   * At each step, the visited node is an unvisited node with the largest number of visited
   * neighbours. Bucket implementation in O(n + m) (Tarjan and Yannakakis 1984). The reverse of the
   * returned sequence is a perfect elimination ordering iff @p g is chordal.
   */
  template < GUM_UndiGraphable G >
  Sequence< NodeId > maximumCardinalitySearch(const G& g);

  /**
   * @brief Returns true if @p order is a perfect elimination ordering of the undirected edges of
   * @p g: for every node v, its neighbours placed after v in @p order form a clique.
   *
   * Tarjan and Yannakakis (1984) test: it is enough to check that the later neighbours of v other
   * than the first of them, p, are neighbours of p. O(n + m) adjacency tests.
   *
   * @throw InvalidArgument if @p order is not an ordering of the nodes of @p g.
   */
  template < GUM_UndiGraphable G >
  bool isPerfectEliminationOrdering(const G& g, const Sequence< NodeId >& order);

  /**
   * @brief Returns true if the undirected edges of @p g form a chordal graph, i.e. if every cycle
   * of length at least 4 has a chord.
   *
   * Tests whether the reverse of a maximum cardinality search is a perfect elimination ordering
   * (Tarjan and Yannakakis 1984). O(n + m).
   */
  template < GUM_UndiGraphable G >
  bool isChordal(const G& g);

}   // namespace gum::graph

#include <agrum/base/graphs/algorithms/generic/chordality_tpl.h>

#endif   // GUM_GRAPH_CHORDALITY_H
