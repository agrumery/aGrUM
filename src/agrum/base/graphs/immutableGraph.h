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


/** @file
 * @brief Read-only bases of the immutable graphs (CPDAG, MPDAG, MAG)
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_IMMUTABLE_GRAPH_H
#define GUM_IMMUTABLE_GRAPH_H

#include <optional>
#include <string>

#include <agrum/agrum.h>

#include <agrum/base/graphs/DAG.h>

namespace gum {

  namespace dot {
    class DotOptions;
  }   // namespace dot

  /** @class ImmutableGraph
   * @brief The read access shared by the immutable graphs: a graph whose invariants are global
   * (adding an arc or an edge would in general break them), kept by value and built only by the
   * factories of the derived class.
   *
   * \ingroup graph_group
   *
   * neighbours() is not part of this base: its meaning depends on the kind of graph (the undirected
   * neighbours of a mixed graph, the nodes linked by a tail-tail edge in a MAG).
   *
   * @tparam GRAPH the stored graph (ChainGraph, PDAG, MarkedGraph)
   */
  template < typename GRAPH >
  class ImmutableGraph {
    public:
    // ############################################################################
    /// @name Read access
    // ############################################################################
    /// @{

    /// the stored graph
    const GRAPH& graph() const;

    /// the nodes of the graph
    const NodeGraphPart& nodes() const;

    /// the edges of the graph
    const EdgeSet& edges() const;

    /// the number of nodes
    Size size() const;

    /// the number of edges
    Size sizeEdges() const;

    /// true if the graph has no node
    bool empty() const;

    bool existsNode(NodeId node) const;
    bool existsEdge(NodeId first, NodeId second) const;

    /// the name of node id, or "<id>" if it has no name
    std::string nameFromId(NodeId id) const;

    /// the id of the node with the given name, or std::nullopt
    std::optional< NodeId > idFromName(const std::string& name) const;

    /// true if node id has a name
    bool hasName(NodeId id) const;

    /// the connected components: each node is mapped to the root of its component
    NodeProperty< NodeId > connectedComponents() const;

    /// the undirected graph with the same nodes (ids and names) and adjacencies. O(n + m).
    UndiGraph skeleton() const;

    /// @}

    /// equality of the graphs
    bool operator==(const ImmutableGraph< GRAPH >& g) const;

    /// a string representation
    std::string toString() const;

    protected:
    /// trusted constructor: g satisfies the invariants of the derived class
    explicit ImmutableGraph(GRAPH&& g);

    ImmutableGraph(const ImmutableGraph< GRAPH >& g);
    ImmutableGraph(ImmutableGraph< GRAPH >&& g);
    ~ImmutableGraph();
    ImmutableGraph< GRAPH >& operator=(const ImmutableGraph< GRAPH >& g);
    ImmutableGraph< GRAPH >& operator=(ImmutableGraph< GRAPH >&& g);

    GRAPH graph_;
  };

  /** @class ImmutableMixedGraph
   * @brief The read access shared by the immutable mixed graphs which represent a set of Markov
   * equivalent DAGs (CPDAG, MPDAG).
   *
   * \ingroup graph_group
   *
   * The represented DAGs are Markov equivalent (same skeleton and v-structures, Verma and Pearl
   * 1990): the separation of the graph is the d-separation of any of them. It is computed on a
   * consistent extension, kept from the construction.
   *
   * @tparam GRAPH the stored graph (ChainGraph, PDAG)
   */
  template < typename GRAPH >
  class ImmutableMixedGraph: public ImmutableGraph< GRAPH > {
    public:
    // ############################################################################
    /// @name Read access
    // ############################################################################
    /// @{

    /// the arcs of the graph (the arcs with the same orientation in all the represented DAGs)
    const ArcSet& arcs() const;

    /// the parents of a node
    const NodeSet& parents(NodeId node) const;

    /// the children of a node
    const NodeSet& children(NodeId node) const;

    /// the undirected neighbours of a node
    const NodeSet& neighbours(NodeId node) const;

    /// the number of arcs
    Size sizeArcs() const;

    bool existsArc(NodeId tail, NodeId head) const;

    /// @}

    // ############################################################################
    /// @name Represented DAGs
    // ############################################################################
    /// @{

    /// a represented DAG (a consistent extension), kept from the construction
    DAG consistentExtension() const;

    /// d-separation of X and Y given Z in every represented DAG
    bool dSeparation(NodeId X, NodeId Y, const NodeSet& Z) const;

    /// d-separation of X and Y given Z in every represented DAG
    bool dSeparation(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const;

    /// @}

    /// a dot representation, the arcs being drawn with the color of the configuration key
    /// `dot.compelledArcColor`
    std::string toDot() const;

    /// a dot representation, with the options given by the caller (which win over the color of
    /// the arcs)
    std::string toDot(const dot::DotOptions& options) const;

    protected:
    /// trusted constructor: extension is a consistent extension of g
    ImmutableMixedGraph(GRAPH&& g, DAG&& extension);

    ImmutableMixedGraph(const ImmutableMixedGraph< GRAPH >& g);
    ImmutableMixedGraph(ImmutableMixedGraph< GRAPH >&& g);
    ~ImmutableMixedGraph();
    ImmutableMixedGraph< GRAPH >& operator=(const ImmutableMixedGraph< GRAPH >& g);
    ImmutableMixedGraph< GRAPH >& operator=(ImmutableMixedGraph< GRAPH >&& g);

    DAG extension_;
  };

}   // namespace gum

#include <agrum/base/graphs/immutableGraph_tpl.h>

#endif   // GUM_IMMUTABLE_GRAPH_H
