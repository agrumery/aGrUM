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
 * @brief Source implementation of Base classes for undirected graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/base/graphs/algorithms/generic/pathFinding.h>
#include <agrum/base/graphs/undiGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/undiGraph_inl.h>
#endif   // GU%_NO_INLINE
#include <cstdio>
#include <iostream>

#include "graphElements.h"

namespace gum {

  UndiGraph UndiGraph::completeGraph(int n) {
    UndiGraph g;
    g.addNodes(n);

    for (int j = 0; j < n; ++j) {
      for (int k = j + 1; k < n; ++k) {
        g.addEdge(j, k);
      }
    }
    return g;
  }

  UndiGraph::UndiGraph(Size nodes_size,
                       bool nodes_resize_policy,
                       Size edges_size,
                       bool edges_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy),
      EdgeGraphPart(edges_size, edges_resize_policy) {
    GUM_CONSTRUCTOR(UndiGraph);
  }

  UndiGraph::UndiGraph(const UndiGraph& g) : NodeGraphPart(g), EdgeGraphPart(g) {
    GUM_CONS_CPY(UndiGraph);
  }

  UndiGraph::UndiGraph(UndiGraph&& g) : NodeGraphPart(std::move(g)), EdgeGraphPart(std::move(g)) {
    GUM_CONS_MOV(UndiGraph);
  }

  UndiGraph::~UndiGraph() {
    GUM_DESTRUCTOR(UndiGraph);
    ;
  }

  std::string UndiGraph::toString() const {
    std::string s = NodeGraphPart::toString();
    s += " , ";
    s += EdgeGraphPart::toString();
    return s;
  }

  std::string UndiGraph::toDot() const {
    std::stringstream output;
    std::stringstream nodeStream;
    std::stringstream edgeStream;
    List< NodeId >    treatedNodes;
    output << "digraph \"no_name\" {\nedge [dir=none]\n";
    nodeStream << "node [shape = ellipse];\n";
    std::string tab = "  ";

    for (const auto node: nodes()) {
      nodeStream << std::format("{}{}{};", tab, node, dotNodeLabel(node));

      if (neighbours(node).size() > 0)
        for (const auto nei: neighbours(node))
          if (!treatedNodes.exists(nei))
            edgeStream << std::format("{}{} -> {};\n", tab, node, nei);

      treatedNodes.insert(node);
    }

    output << nodeStream.str() << '\n' << edgeStream.str() << '\n' << "}\n";

    return output.str();
  }

  /// returns the partial graph formed by the nodes given in parameter
  UndiGraph UndiGraph::partialUndiGraph(NodeSet nodes) {
    UndiGraph partialGraph;

    for (const auto node: nodes) {
      partialGraph.addNodeWithId(node);

      for (const auto nei: neighbours(node))
        if (nodes.contains(nei) && partialGraph.existsNode(nei)) partialGraph.addEdge(node, nei);
    }

    return partialGraph;
  }

  /// for friendly displaying the content of undirected graphs
  std::ostream& operator<<(std::ostream& stream, const UndiGraph& g) {
    stream << g.toString();
    return stream;
  }

} /* namespace gum */
