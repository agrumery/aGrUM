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
 * @brief Generic node-reachability algorithms for aGrUM graphs.
 *
 * All functions live in namespace gum::graph and are constrained by the
 * concepts defined in graphConcepts.h :
 *
 *   GUM_DiGraphable    — ancestors, descendants, family
 *   GUM_UndiGraphable  — chainComponents
 *   GUM_NodeGraphable  — connectedComponents (Di/Undi/Mixed/Marked via if constexpr)
 *   GUM_MixedGraphable — chainComponent, boundary
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_REACHABILITY_H
#define GUM_GRAPH_REACHABILITY_H

#include <agrum/agrum.h>

#include <agrum/base/graphs/graphConcepts.h>

namespace gum::graph {

  namespace detail {
    /**
     * @brief The nodes reached from the nodes of @p ids through at least one step of
     *        @p next (a function NodeId -> NodeSet: the parents, the children...).
     *
     * A node of @p ids belongs to the result only if it is reached from a node of
     * @p ids. With directed steps only (arcs), such a walk always contains a path
     * or a directed cycle. O(n + m).
     */
    template < typename NEXT >
    NodeSet directedClosure(const NodeSet& ids, NEXT next);

    /**
     * @brief The nodes with a path (no repeated node, at least one step of
     *        @p predecessors) to a node of @p ids.
     *
     * Same as directedClosure, but for steps which may be undirected edges: a node
     * of @p ids belongs to the result only if it has a path to another node of
     * @p ids, or lies on a cycle; going back and forth through the same undirected
     * edge is not a cycle. O((n + m) (1 + sum over the nodes v of ids of the
     * number of predecessors of v)).
     */
    template < typename PREDECESSORS >
    NodeSet pathClosure(const NodeSet& ids, PREDECESSORS predecessors);
  }   // namespace detail

  // =========================================================================
  // Directed graphs (GUM_DiGraphable)
  // =========================================================================

  /**
   * @brief Returns the set of all ancestors of @p id: the nodes with a
   *        directed path (at least one arc) to @p id. @p id belongs to it
   *        only if it is on a directed cycle.
   */
  template < GUM_DiGraphable G >
  NodeSet ancestors(const G& g, NodeId id);

  /**
   * @brief Returns the ancestors of the nodes of @p ids: the union of their
   *        ancestors. A node of @p ids belongs to it only if it has a directed
   *        path to a node of @p ids (on a cycle, or ancestor of another one).
   */
  template < GUM_DiGraphable G >
  NodeSet ancestors(const G& g, const NodeSet& ids);

  /**
   * @brief Returns the set of all descendants of @p id: the nodes with a
   *        directed path (at least one arc) from @p id. @p id belongs to it
   *        only if it is on a directed cycle.
   */
  template < GUM_DiGraphable G >
  NodeSet descendants(const G& g, NodeId id);

  /**
   * @brief Returns the descendants of the nodes of @p ids: the union of their
   *        descendants. A node of @p ids belongs to it only if it has a directed
   *        path from a node of @p ids (on a cycle, or descendant of another one).
   */
  template < GUM_DiGraphable G >
  NodeSet descendants(const G& g, const NodeSet& ids);

  /// @brief Returns the family of @p id : { id } ∪ parents(id).
  template < GUM_DiGraphable G >
  NodeSet family(const G& g, NodeId id);

  /// @brief Returns the union of families of all nodes in @p ids.
  template < GUM_DiGraphable G >
  NodeSet family(const G& g, const NodeSet& ids);

  // =========================================================================
  // Undirected graphs (GUM_UndiGraphable)
  // =========================================================================

  /**
   * @brief Returns a node-to-component-id mapping for the chain components
   *        of @p g (connected components of the edge-only subgraph).
   *
   * Arc orientations are ignored; only edges are traversed.
   * Component ids are assigned in order of first encounter.
   */
  template < GUM_UndiGraphable G >
  NodeProperty< NodeId > chainComponents(const G& g);

  /**
   * @brief Returns a node-to-component-id mapping for the (weakly) connected
   *        components of @p g.
   *
   * For directed graphs, arcs are treated as undirected (weak connectivity).
   * For mixed graphs, both edges and arcs are traversed.
   * For marked graphs, the edges are traversed whatever their marks.
   * Component ids are assigned in order of first encounter.
   *
   * @tparam G Must satisfy GUM_DiGraphable, GUM_UndiGraphable (or both), or
   *         GUM_MarkedGraphable.
   */
  template < GUM_NodeGraphable G >
  NodeProperty< NodeId > connectedComponents(const G& g);

  /**
   * @brief Returns true iff some node in @p A can reach some node in @p B
   *        via undirected edges.
   *
   * Returns false immediately if @p A or @p B is empty, or if both sets
   * share at least one node (trivially connected).
   *
   * @tparam G Any GUM_UndiGraphable graph.
   * @param g The undirected (or mixed) graph — only edges are traversed.
   * @param A First node set.
   * @param B Second node set.
   * @return `true` iff there exists an undirected path between some `a ∈ A`
   *         and some `b ∈ B` in @p g.
   */
  template < GUM_UndiGraphable G >
  bool areConnected(const G& g, const NodeSet& A, const NodeSet& B);

  // =========================================================================
  // Mixed graphs (GUM_MixedGraphable)
  // =========================================================================

  /**
   * @brief Returns the chain component of @p node in @p g.
   *
   * The chain component is the connected component of the undirected subgraph
   * (edges only) containing @p node.  Arc orientations are ignored.
   */
  template < GUM_MixedGraphable G >
  NodeSet chainComponent(const G& g, NodeId node);

  /**
   * @brief Returns the anterior nodes of the nodes of @p ids: the nodes with a
   *        path made of arcs (followed in their direction) and edges to a node
   *        of @p ids.
   *
   * As for ancestors, a node of @p ids belongs to the result only if it has such
   * a path (at least one step) to a node of @p ids: to another one, or on a cycle
   * (going back and forth through the same edge is not a cycle).
   */
  template < GUM_MixedGraphable G >
  NodeSet anteriors(const G& g, const NodeSet& ids);

  /// @brief Returns the boundary of @p node: neighbours ∪ parents ∪ children.
  template < GUM_MixedGraphable G >
  NodeSet boundary(const G& g, NodeId node);

}   // namespace gum::graph

#include <agrum/base/graphs/algorithms/generic/reachability_tpl.h>

#endif   // GUM_GRAPH_REACHABILITY_H
