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
 * @brief Generic BFS-based path-finding algorithms for aGrUM graphs.
 *
 * All functions live in namespace gum::graph and are constrained by the
 * concepts defined in graphConcepts.h :
 *
 *   GUM_DiGraphable    — directed graphs  (DiGraph, DAG, …)
 *   GUM_UndiGraphable  — undirected graphs (UndiGraph, …)
 *   GUM_MixedGraphable — mixed graphs     (MixedGraph, PDAG)
 *
 * Class methods in DiGraph / UndiGraph / MixedGraph delegate to these
 * free functions wherever the class satisfies the required concept.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_PATH_FINDING_H
#define GUM_GRAPH_PATH_FINDING_H

#include <optional>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/graphs/graphConcepts.h>

namespace gum::graph {

  namespace detail {
    /// Reconstruct the BFS path [n1 … n2] from the predecessor mark table.
    inline std::vector< NodeId >
        reconstructPath(const NodeProperty< NodeId >& mark, NodeId n1, NodeId n2);
  }   // namespace detail

  // =========================================================================
  // Directed graphs (GUM_DiGraphable)
  // =========================================================================

  /**
   * @brief Shortest directed path from @p n1 to @p n2 (BFS, arc direction).
   *
   * Returns the sequence of nodes [n1, …, n2], or std::nullopt if no directed
   * path exists.
   */
  template < GUM_DiGraphable G >
  std::optional< std::vector< NodeId > > directedPath(const G& g, NodeId n1, NodeId n2);

  /**
   * @brief Shortest path from @p n1 to @p n2 ignoring arc orientation (BFS).
   *
   * Traverses both parents and children — shortest path in the skeleton.
   * Returns std::nullopt if no path exists.
   */
  template < GUM_DiGraphable G >
  std::optional< std::vector< NodeId > > directedUnorientedPath(const G& g, NodeId n1, NodeId n2);

  /**
   * @brief Returns true if there is a directed path from @p from to @p to.
   *
   * Returns false immediately if @p from does not exist in @p g.
   */
  template < GUM_DiGraphable G >
  bool hasDirectedPath(const G& g, NodeId from, NodeId to);

  // =========================================================================
  // Undirected graphs (GUM_UndiGraphable)
  // =========================================================================

  /**
   * @brief Shortest undirected path from @p n1 to @p n2 (BFS).
   *
   * Returns the sequence of nodes [n1, …, n2], or std::nullopt if no path
   * exists.
   */
  template < GUM_UndiGraphable G >
  std::optional< std::vector< NodeId > > undirectedPath(const G& g, NodeId n1, NodeId n2);

  /**
   * @brief Returns true if an undirected path exists between @p n1 and @p n2.
   */
  template < GUM_UndiGraphable G >
  bool hasUndirectedPath(const G& g, NodeId n1, NodeId n2);

  /**
   * @brief Returns true if an undirected path from @p n1 to @p n2 exists
   *        that avoids all nodes in @p except.
   */
  template < GUM_UndiGraphable G >
  bool hasUndirectedPath(const G& g, NodeId n1, NodeId n2, const NodeSet& except);

  /**
   * @brief Returns true if an undirected path exists from any node in @p n1
   *        to any node in @p n2, avoiding nodes in @p except.
   */
  template < GUM_UndiGraphable G >
  bool hasUndirectedPath(const G& g, const NodeSet& n1, const NodeSet& n2, const NodeSet& except);

  // =========================================================================
  // Mixed graphs (GUM_MixedGraphable)
  // =========================================================================

  /**
   * @brief Shortest mixed-oriented path from @p n1 to @p n2.
   *
   * Follows arcs forward and edges in both directions.
   * Returns std::nullopt if no such path exists.
   */
  template < GUM_MixedGraphable G >
  std::optional< std::vector< NodeId > > mixedOrientedPath(const G& g, NodeId n1, NodeId n2);

  /**
   * @brief Returns true if a mixed-oriented path from @p n1 to @p n2 exists.
   */
  template < GUM_MixedGraphable G >
  bool hasMixedOrientedPath(const G& g, NodeId n1, NodeId n2);

  /**
   * @brief Shortest path ignoring all orientations in a mixed graph.
   *
   * Traverses edges (both directions), parents, and children.
   * Returns std::nullopt if no path exists.
   */
  template < GUM_MixedGraphable G >
  std::optional< std::vector< NodeId > > mixedUnorientedPath(const G& g, NodeId n1, NodeId n2);

}   // namespace gum::graph

#include <agrum/base/graphs/algorithms/generic/pathFinding_tpl.h>

#endif   // GUM_GRAPH_PATH_FINDING_H
