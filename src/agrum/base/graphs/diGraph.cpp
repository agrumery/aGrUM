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
 * @brief Source implementation of Base classes for oriented graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/base/graphs/algorithms/generic/pathFinding.h>
#include <agrum/base/graphs/diGraph.h>
#include <agrum/base/graphs/graphElements.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/diGraph_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  DiGraph DiGraph::completeGraph(int n) {
    DiGraph g;
    g.addNodes(n);

    for (int j = 0; j < n; ++j) {
      for (int k = j + 1; k < n; ++k) {
        g.addArc(j, k);
      }
    }
    return g;
  }

  DiGraph::DiGraph(Size nodes_size,
                   bool nodes_resize_policy,
                   Size arcs_size,
                   bool arcs_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy), ArcGraphPart(arcs_size, arcs_resize_policy) {
    GUM_CONSTRUCTOR(DiGraph)
  }

  DiGraph::DiGraph(const DiGraph& g) : NodeGraphPart(g), ArcGraphPart(g) { GUM_CONS_CPY(DiGraph) }

  DiGraph::DiGraph(DiGraph&& g) : NodeGraphPart(std::move(g)), ArcGraphPart(std::move(g)) {
    GUM_CONS_MOV(DiGraph)
  }

  DiGraph::~DiGraph() { GUM_DESTRUCTOR(DiGraph) }

  std::string DiGraph::toString() const {
    std::string s = NodeGraphPart::toString();
    s += " , ";
    s += ArcGraphPart::toString();
    return s;
  }

  std::string DiGraph::toDot() const {
    std::stringstream strBuff;
    std::string       tab = "     ";
    strBuff << "digraph {" << std::endl;

    for (const auto node: nodes()) {
      strBuff << tab << node << dotNodeLabel(node) << ";" << std::endl;
    }

    strBuff << std::endl;

    for (const auto& arc: arcs())
      strBuff << tab << arc.tail() << " -> " << arc.head() << ";" << std::endl;

    strBuff << "}" << std::endl << std::endl;
    return strBuff.str();
  }

  /// for friendly displaying the content of directed graphs
  std::ostream& operator<<(std::ostream& stream, const DiGraph& g) {
    stream << g.toString();
    return stream;
  }

  bool DiGraph::hasDirectedPath(const NodeId from, const NodeId to) const {
    return graph::hasDirectedPath(*this, from, to);
  }

} /* namespace gum */
