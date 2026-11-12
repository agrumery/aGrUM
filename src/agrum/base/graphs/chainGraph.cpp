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
 * @brief Outlined implementation of chain graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <agrum/base/core/dot/dotElements.h>
#include <agrum/base/graphs/algorithms/generic/cycleDetection.h>
#include <agrum/base/graphs/algorithms/generic/moralization.h>
#include <agrum/base/graphs/algorithms/generic/pathFinding.h>
#include <agrum/base/graphs/algorithms/generic/separation.h>
#include <agrum/base/graphs/chainGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/chainGraph_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  namespace {
    /// the check of the constructors from a PDAG or a mixed graph (whose constructor PDAG(g)
    /// already checks the directed cycles and the double connections)
    void checkNoPartiallyDirectedCycle(const MixedGraph& g) {
      if (graph::hasPartiallyDirectedCycle(g))
        GUM_ERROR(InvalidPartiallyDirectedCycle,
                  "A chain graph can not contain a partially directed cycle")
    }
  }   // namespace

  // diamond structure require to explicitly initialize NodeGraphPart
  ChainGraph::ChainGraph(Size nodes_size,
                         bool nodes_resize_policy,
                         Size arcs_size,
                         bool arcs_resize_policy,
                         Size edges_size,
                         bool edges_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy), PDAG(nodes_size,
                                                           nodes_resize_policy,
                                                           arcs_size,
                                                           arcs_resize_policy,
                                                           edges_size,
                                                           edges_resize_policy) {
    GUM_CONSTRUCTOR(ChainGraph);
  }

  ChainGraph::ChainGraph(const UndiGraph& g) : NodeGraphPart(g), PDAG(g) {
    GUM_CONSTRUCTOR(ChainGraph);
  }

  ChainGraph::ChainGraph(const DAG& g) : NodeGraphPart(g), PDAG(g) { GUM_CONSTRUCTOR(ChainGraph); }

  ChainGraph::ChainGraph(const PDAG& g) : NodeGraphPart(g), PDAG(g) {
    checkNoPartiallyDirectedCycle(g);
    GUM_CONSTRUCTOR(ChainGraph);
  }

  ChainGraph::ChainGraph(const MixedGraph& g) : NodeGraphPart(g), PDAG(g) {
    checkNoPartiallyDirectedCycle(g);
    GUM_CONSTRUCTOR(ChainGraph);
  }

  ChainGraph::ChainGraph(const ChainGraph& g) : NodeGraphPart(g), PDAG(g) {
    GUM_CONS_CPY(ChainGraph);
  }

  ChainGraph::ChainGraph(ChainGraph&& g) : NodeGraphPart(std::move(g)), PDAG(std::move(g)) {
    GUM_CONS_MOV(ChainGraph);
  }

  ChainGraph::~ChainGraph() { GUM_DESTRUCTOR(ChainGraph); }

  bool ChainGraph::hasMixedReallyOrientedPath(NodeId n1, NodeId n2) const {
    return graph::hasMixedWalkWithArc(*this, n1, n2);
  }

  UndiGraph ChainGraph::moralGraph() const { return graph::moralGraph(*this); }

  UndiGraph ChainGraph::moralizedAncestralGraph(const NodeSet& nodes) const {
    return graph::moralizedAncestralGraph(*this, nodes);
  }

  bool ChainGraph::cSeparation(NodeId X, NodeId Y, const NodeSet& Z) const {
    return graph::cSeparated(*this, X, Y, Z);
  }

  bool ChainGraph::cSeparation(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const {
    return graph::cSeparated(*this, X, Y, Z);
  }

  std::string ChainGraph::toDot() const { return toDot(dot::DotOptions()); }

  std::string ChainGraph::toDot(const dot::DotOptions& options) const {
    const auto     style = dot::DotStyle::read();
    dot::DotWriter writer(true);
    dot::writePreamble(writer, style, options);

    // the nodes without undirected edge, then one cluster for each chain component
    NodeSet treatedNodes;
    for (const auto node: nodes()) {
      if (neighbours(node).empty()) {
        writeDotNode(writer, style, options, node);
        treatedNodes.insert(node);
      }
    }

    int cluster = 0;
    for (const auto node: nodes()) {
      if (treatedNodes.exists(node)) continue;
      writer.beginSubgraph("cluster_" + std::to_string(cluster++),
                           {{"style", "filled"}, {"color", style.chainComponentBgColor}});
      // rank=same is ignored by dot on a cluster: it goes to an anonymous inner subgraph
      writer.beginSubgraph("", {{"rank", "same"}});
      for (const auto cc: chainComponent(node)) {
        writeDotNode(writer, style, options, cc);
        treatedNodes.insert(cc);
      }
      writer.endSubgraph();
      writer.endSubgraph();
    }

    for (const auto& arc: arcs())
      writer.edge(arc.tail(), arc.head(), dot::arcAttributes(options.arc(arc.tail(), arc.head())));

    for (const auto& edge: edges()) {
      auto attributes = dot::arcAttributes(options.edge(edge.first(), edge.second()));
      attributes.insert(attributes.begin(), {"dir", "none"});
      writer.edge(edge.first(), edge.second(), attributes);
    }

    return writer.str();
  }

} /* namespace gum */
