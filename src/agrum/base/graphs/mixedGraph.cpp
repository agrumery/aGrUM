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
#include <agrum/base/graphs/mixedGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/mixedGraph_inl.h>
#endif   // GU%_NO_INLINE

namespace gum {

  MixedGraph::MixedGraph(Size nodes_size,
                         bool nodes_resize_policy,
                         Size arcs_size,
                         bool arcs_resize_policy,
                         Size edges_size,
                         bool edges_resize_policy) :
      // Note that we need to initialize the NodeGraphPart by ourselves
      // because
      // it is a virtual inherited class (see C++ FAQ Lite #25.12 for details)
      NodeGraphPart(nodes_size, nodes_resize_policy), UndiGraph(edges_size, edges_resize_policy),
      DiGraph(arcs_size, arcs_resize_policy) {   // for debugging purposes
    GUM_CONSTRUCTOR(MixedGraph);
  }

  MixedGraph::MixedGraph(const UndiGraph& g) : NodeGraphPart(g), UndiGraph(g), DiGraph() {
    GUM_CONSTRUCTOR(MixedGraph);
  }

  MixedGraph::MixedGraph(const DiGraph& g) : NodeGraphPart(g), UndiGraph(), DiGraph(g) {
    GUM_CONSTRUCTOR(MixedGraph);
  }

  MixedGraph::MixedGraph(const MixedGraph& g) :
      NodeGraphPart(g), UndiGraph(g), DiGraph(g) {   // for debugging purposes
    GUM_CONS_CPY(MixedGraph);
  }

  MixedGraph::MixedGraph(MixedGraph&& g) :
      NodeGraphPart(std::move(g)), UndiGraph(std::move(g)), DiGraph(std::move(g)) {
    GUM_CONS_MOV(MixedGraph);
  }

  MixedGraph::~MixedGraph() {   // for debugging purposes
    GUM_DESTRUCTOR(MixedGraph);
  }

  std::string MixedGraph::toString() const {
    std::string s = NodeGraphPart::toString();
    s += " , ";
    s += ArcGraphPart::toString();
    s += " , ";
    s += EdgeGraphPart::toString();
    return s;
  }

  std::string MixedGraph::toDot() const {
    std::stringstream output;
    std::stringstream nodeStream;
    std::stringstream edgeStream;

    NodeSet treatedNodes;

    output << "digraph \"no_name\" {\n";
    nodeStream << "node [shape = ellipse];\n";
    std::string tab = "  ";

    for (const auto node: nodes()) {
      nodeStream << std::format("{}{}{};", tab, node, dotNodeLabel(node));

      for (const auto nei: neighbours(node))
        if (!treatedNodes.exists(nei))
          edgeStream << std::format("{}{} -> {} [dir=none];\n", tab, node, nei);

      for (const auto chi: children(node))
        edgeStream << std::format("{}{} -> {};\n", tab, node, chi);

      treatedNodes.insert(node);
    }

    output << nodeStream.str() << '\n' << edgeStream.str() << '\n' << "}\n";

    return output.str();
  }

  /// for friendly displaying the content of directed graphs
  std::ostream& operator<<(std::ostream& stream, const MixedGraph& g) {
    stream << g.toString();
    return stream;
  }


} /* namespace gum */
