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
 * @brief Source implementation of the class building the essential Graph from a
 * DAGmodel
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/base/core/dot/dotElements.h>
#include <agrum/base/graphs/algorithms/orientation.h>
#include <agrum/BN/algorithms/essentialGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/BN/algorithms/essentialGraph_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {
  namespace {
    /// the DAG of m with the names of the variables of m only
    DAG namedDAG(const DAGmodel& m) {
      const DAG model = m.dag();   // a copy: a temporary would not outlive the loops
      DAG       dag;
      for (const auto node: model.nodes()) {
        dag.addNodeWithId(node);
        dag.setName(node, m.variable(node).name());
      }
      for (const auto& arc: model.arcs())
        dag.addArc(arc.tail(), arc.head());
      return dag;
    }

    /// the PDAG pdag with the names of the variables of m only: its own names are dropped (renaming
    /// node by node would fail halfway on names permuted between its nodes)
    PDAG namedAfter(const PDAG& pdag, const DAGmodel& m) {
      MixedGraph g;
      for (const auto node: pdag.nodes()) {
        g.addNodeWithId(node);
        g.setName(node, m.variable(node).name());
      }
      for (const auto& arc: pdag.arcs())
        g.addArc(arc.tail(), arc.head());
      for (const auto& edge: pdag.edges())
        g.addEdge(edge.first(), edge.second());
      return PDAG(g);
    }
  }   // namespace

  EssentialGraph::EssentialGraph() : CPDAG(ChainGraph(), DAG()) {}

  EssentialGraph::EssentialGraph(const DAGmodel& m) : CPDAG(CPDAG::fromDAG(namedDAG(m))) {}

  EssentialGraph::EssentialGraph(const DAGmodel& m, const PDAG& pdag) :
      CPDAG(CPDAG::fromPDAG(namedAfter(pdag, m))) {}

  EssentialGraph::EssentialGraph(const PDAG& pdag) : CPDAG(CPDAG::fromPDAG(pdag)) {}

  EssentialGraph::EssentialGraph(const CPDAG& cpdag) : CPDAG(cpdag) {}

  EssentialGraph::~EssentialGraph() = default;

  std::string EssentialGraph::toDot() const { return toDot(dot::DotOptions()); }

  std::string EssentialGraph::toDot(const dot::DotOptions& options) const {
    // the look of a CPDAG (clusters, colored compelled arcs), with the names of the variables as
    // labels unless the caller gives other ones
    dot::DotOptions withLabels(options);
    for (const auto node: nodes()) {
      if (!hasName(node)) continue;   // the default label and tooltip of the graph
      const auto* given = options.node(node);
      const auto  name  = nameFromId(node);
      if (given == nullptr || given->label.empty()) withLabels.setNodeLabel(node, name);
      if (given == nullptr || given->tooltip.empty())
        withLabels.setNodeTooltip(node, "(" + std::to_string(node) + ") " + name);
    }
    return CPDAG::toDot(withLabels);
  }

  std::string graphToGUMString(const EssentialGraph& g, int indent) {
    return graph::detail::gumGraphToString(
        graph::detail::gumGraphContentOf(g.graph(), "EssentialGraph"),
        indent);
  }

  void saveGraphGUM(const EssentialGraph& g,
                    std::string_view      path,
                    std::optional< bool > binary,
                    int                   indent) {
    graph::detail::gumGraphToFile(graph::detail::gumGraphContentOf(g.graph(), "EssentialGraph"),
                                  path,
                                  binary,
                                  indent);
  }

  namespace {
    /// the essential graph read from a GUM content: checked to be a CPDAG
    EssentialGraph essentialGraphOf(const graph::detail::GumGraphContent& content) {
      auto pdag = graph::detail::gumGraphPDAGOf(content, "EssentialGraph");
      try {
        return EssentialGraph(pdag);
      } catch (const Exception&) { graph::detail::rethrowAsIOError("EssentialGraph"); }
    }
  }   // namespace

  template <>
  EssentialGraph graphFromGUMString< EssentialGraph >(std::string_view content) {
    return essentialGraphOf(graph::detail::gumGraphFromString(content));
  }

  template <>
  EssentialGraph loadGraphGUM< EssentialGraph >(std::string_view      path,
                                                std::optional< bool > binary) {
    return essentialGraphOf(graph::detail::gumGraphFromFile(path, binary));
  }

}   // namespace gum
