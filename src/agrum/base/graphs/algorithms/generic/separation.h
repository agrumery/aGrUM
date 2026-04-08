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
 *   useful, but WITHOUT ANY KIND, EXPRESS OR IMPLIED,          *
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
 * @brief d-Separation and c-Separation tests for aGrUM graphs.
 *
 * All functions live in namespace gum::graph and are constrained by the
 * concepts defined in graphConcepts.h :
 *
 *   GUM_DiGraphable    — dSeparated (DAGs)
 *   GUM_MixedGraphable — cSeparated (PDAGs / chain graphs)
 *
 * Both use the same three-step algorithm (Pearl 1988 / Lauritzen 1990):
 *   1. Moralize the ancestral subgraph of X ∪ Y ∪ Z.
 *   2. Remove all nodes of Z.
 *   3. X and Y are separated iff they are disconnected in the result.
 *
 * The difference lies in step 1:
 *   - d-separation: ancestral subgraph follows arcs only; moralization
 *     marries co-parents per individual node.
 *   - c-separation: ancestral subgraph follows arcs *and* edges; moralization
 *     marries co-parents per *chain component* (Lauritzen & Richardson 2002).
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_SEPARATION_H
#define GUM_GRAPH_SEPARATION_H

#include <agrum/agrum.h>
#include <agrum/base/core/exceptions.h>
#include <agrum/base/graphs/graphConcepts.h>
#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/graphs/algorithms/generic/moralization.h>

namespace gum::graph {

  /**
   * @brief Returns true iff @p X and @p Y are d-separated by @p Z in @p g.
   *
   * @tparam G Any GUM_DiGraphable graph (typically a DAG).
   * @param g The directed graph.
   * @param X Source node.
   * @param Y Target node.
   * @param Z Conditioning set (must not contain X or Y).
   * @return true if X ⊥ Y | Z in the d-separation sense.
   */
  template < GUM_DiGraphable G >
  bool dSeparated(const G& g, NodeId X, NodeId Y, const NodeSet& Z) {
    NodeSet query{Z};
    query.insert(X);
    query.insert(Y);
    auto moral = moralizedAncestralGraph(g, query);
    for (const auto node: Z)
      if (moral.existsNode(node)) moral.eraseNode(node);
    return !moral.hasUndirectedPath(X, Y);
  }

  /**
   * @brief Returns true iff @p X and @p Y are d-separated by @p Z in @p g.
   *
   * Set-valued version: d-separation holds iff every node in @p X is
   * disconnected from every node in @p Y after moralization and removal of Z.
   *
   * @tparam G Any GUM_DiGraphable graph (typically a DAG).
   * @param g The directed graph.
   * @param X Source node set (must be disjoint from Y).
   * @param Y Target node set (must be disjoint from X).
   * @param Z Conditioning set.
   * @return true if X ⊥ Y | Z in the d-separation sense.
   * @throw InvalidArgument if X and Y are not disjoint.
   */
  template < GUM_DiGraphable G >
  bool dSeparated(const G& g, const NodeSet& X, const NodeSet& Y, const NodeSet& Z) {
    if (!(X * Y).empty())
      GUM_ERROR(InvalidArgument, "NodeSets X and Y must be disjoint.")

    NodeSet query{Z};
    query += X;
    query += Y;
    auto moral = moralizedAncestralGraph(g, query);
    for (const auto node: Z)
      if (moral.existsNode(node)) moral.eraseNode(node);

    const auto cc = moral.chainComponents();

    NodeSet Xcc, Ycc;
    for (const auto node: X)
      if (moral.existsNode(node)) Xcc.insert(cc[node]);
    for (const auto node: Y)
      if (moral.existsNode(node)) Ycc.insert(cc[node]);

    return (Xcc * Ycc).empty();
  }

  /**
   * @brief Returns true iff @p X and @p Y are c-separated by @p Z in @p g.
   *
   * C-separation generalizes d-separation to mixed graphs (PDAGs / chain
   * graphs).  The ancestral subgraph is built by following arcs backward
   * *and* undirected edges bidirectionally; moralization then marries the
   * parents of each chain component rather than each individual node.
   *
   * @warning C-separation is well-defined for any PDAG but is statistically
   *   meaningful only for chain graphs (PDAGs with no partially directed
   *   cycles).
   *
   * @tparam G Any GUM_MixedGraphable graph (typically a PDAG or chain graph).
   * @param g The mixed graph.
   * @param X Source node.
   * @param Y Target node.
   * @param Z Conditioning set.
   * @return true if X ⊥ Y | Z in the c-separation sense.
   */
  template < GUM_MixedGraphable G >
  bool cSeparated(const G& g, NodeId X, NodeId Y, const NodeSet& Z) {
    NodeSet query{Z};
    query.insert(X);
    query.insert(Y);
    auto moral = moralizedAncestralGraph(g, query);
    for (const auto node: Z)
      if (moral.existsNode(node)) moral.eraseNode(node);
    return !moral.hasUndirectedPath(X, Y);
  }

  /**
   * @brief Returns true iff @p X and @p Y are c-separated by @p Z in @p g.
   *
   * Set-valued version: c-separation holds iff every node in @p X is
   * disconnected from every node in @p Y after moralization and removal of Z.
   *
   * @tparam G Any GUM_MixedGraphable graph (typically a PDAG or chain graph).
   * @param g The mixed graph.
   * @param X Source node set (must be disjoint from Y).
   * @param Y Target node set (must be disjoint from X).
   * @param Z Conditioning set.
   * @return true if X ⊥ Y | Z in the c-separation sense.
   * @throw InvalidArgument if X and Y are not disjoint.
   */
  template < GUM_MixedGraphable G >
  bool cSeparated(const G& g, const NodeSet& X, const NodeSet& Y, const NodeSet& Z) {
    if (!(X * Y).empty())
      GUM_ERROR(InvalidArgument, "NodeSets X and Y must be disjoint.")

    NodeSet query{Z};
    query += X;
    query += Y;
    auto moral = moralizedAncestralGraph(g, query);
    for (const auto node: Z)
      if (moral.existsNode(node)) moral.eraseNode(node);

    const auto cc = moral.chainComponents();

    NodeSet Xcc, Ycc;
    for (const auto node: X)
      if (moral.existsNode(node)) Xcc.insert(cc[node]);
    for (const auto node: Y)
      if (moral.existsNode(node)) Ycc.insert(cc[node]);

    return (Xcc * Ycc).empty();
  }

}   // namespace gum::graph

#endif   // GUM_GRAPH_SEPARATION_H
