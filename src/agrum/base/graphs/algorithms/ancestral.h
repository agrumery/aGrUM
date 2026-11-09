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
 * @brief Algorithms for ancestral graphs (marked graphs without circle): ancestors and anterior
 * nodes, ancestrality, maximality and m-separation; representative graph of a PAG.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_ANCESTRAL_H
#define GUM_GRAPH_ANCESTRAL_H

#include <agrum/agrum.h>

#include <agrum/base/graphs/markedGraph.h>
#include <agrum/base/graphs/PDAG.h>

namespace gum::graph {

  /**
   * @brief The ancestors of the nodes of @p x in @p g, @p x included: the nodes with a directed
   * path (made of edges ->) to a node of @p x (Richardson and Spirtes 2002, Sec. 2.4). O(n + m).
   */
  GUM_SHARED_PUBLIC NodeSet ancestors(const MarkedGraph& g, const NodeSet& x);

  /**
   * @brief The anterior nodes of the nodes of @p x in @p g, @p x included: the nodes with a path
   * made of edges -- and -> (pointing towards @p x) to a node of @p x (Richardson and Spirtes 2002,
   * Sec. 2.4). O(n + m).
   */
  GUM_SHARED_PUBLIC NodeSet anteriors(const MarkedGraph& g, const NodeSet& x);

  /**
   * @brief True if @p g is an ancestral graph (Richardson and Spirtes 2002, Sec. 3.1): no circle,
   * for every node a, a is not anterior to a parent or a spouse of a, and a node with an undirected
   * edge has neither parent nor spouse. O(n (n + m)).
   */
  GUM_SHARED_PUBLIC bool isAncestral(const MarkedGraph& g);

  /**
   * @brief True if the ancestral graph @p g is maximal: two non adjacent nodes a and b are
   * m-separated by ant({a, b}) \ {a, b} (no primitive inducing path between them, Richardson and
   * Spirtes 2002, Theorem 4.2 with S and L empty). O(n^2 (n + m)).
   * @warning @p g is supposed ancestral.
   */
  GUM_SHARED_PUBLIC bool isMaximal(const MarkedGraph& g);

  /**
   * @brief True if @p x and @p y are m-separated by @p z in the ancestral graph @p g: no path
   * between them on which every non collider is out of @p z and every collider is anterior to
   * @p z (Richardson and Spirtes 2002, Sec. 3.4). Search of the reachable (node, arrowhead at the
   * node) pairs from @p x, O(n + m). As graph::dSeparated, the nodes of @p x and @p y which belong
   * to @p z are observed, hence separated from any node: the result is the one for x \ z and
   * y \ z.
   * @warning @p g is supposed ancestral (without circle).
   * @throw InvalidArgument if @p x and @p y are not disjoint.
   */
  GUM_SHARED_PUBLIC bool
      mSeparation(const MarkedGraph& g, const NodeSet& x, const NodeSet& y, const NodeSet& z);

  /**
   * @brief The circle component of the marked graph @p g: its edges o-o, as undirected edges of a
   * PDAG over all the nodes of @p g. O(n + m).
   */
  GUM_SHARED_PUBLIC PDAG circleComponent(const MarkedGraph& g);

  /**
   * @brief The representative graph of the marked graph @p g (a PAG) for an orientation of its
   * circle component: every edge o-> becomes ->, every edge --o becomes <-, and every edge o-o
   * gets the orientation of @p circleOrientation (Zhang 2008, Theorem 2). The other edges are
   * kept. O(n + m).
   *
   * When @p g is a valid PAG and @p circleOrientation is a DAG without unshielded collider (e.g. a
   * consistent extension of circleComponent(g)), the result is a MAG of the Markov equivalence
   * class of @p g.
   *
   * @throw InvalidArgument if the arcs of @p circleOrientation are not exactly an orientation of
   * the edges o-o of @p g
   *
   * @par References
   * - J. Zhang. On the completeness of orientation rules for causal discovery in the presence of
   *   latent confounders and selection bias. Artificial Intelligence 172(16-17):1873-1896, 2008.
   *   doi:10.1016/j.artint.2008.08.001.
   */
  GUM_SHARED_PUBLIC MarkedGraph representativeGraph(const MarkedGraph& g,
                                                    const DAG&         circleOrientation);

}   // namespace gum::graph

#endif   // GUM_GRAPH_ANCESTRAL_H
