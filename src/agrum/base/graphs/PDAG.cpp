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
 * @brief Source implementation of Base classes for partially directed acyclic graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/base/graphs/algorithms/generic/moralization.h>
#include <agrum/base/graphs/algorithms/generic/separation.h>
#include <agrum/base/graphs/PDAG.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/PDAG_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  // diamond structure require to explicitly initialize NodeGraphPart
  PDAG::PDAG(Size nodes_size,
             bool nodes_resize_policy,
             Size arcs_size,
             bool arcs_resize_policy,
             Size edges_size,
             bool edges_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy), MixedGraph(nodes_size,
                                                                 nodes_resize_policy,
                                                                 arcs_size,
                                                                 arcs_resize_policy,
                                                                 edges_size,
                                                                 edges_resize_policy) {
    GUM_CONSTRUCTOR(PDAG);
  }

  PDAG::PDAG(const UndiGraph& g) : NodeGraphPart(g), MixedGraph(g) { GUM_CONSTRUCTOR(PDAG); }

  PDAG::PDAG(const DAG& g) : NodeGraphPart(g), MixedGraph(g) { GUM_CONSTRUCTOR(PDAG); }

  PDAG::PDAG(const MixedGraph& g) : NodeGraphPart(g), MixedGraph(g) { GUM_CONS_CPY(PDAG); }

  PDAG::PDAG(const PDAG& g) : NodeGraphPart(g), MixedGraph(g) { GUM_CONS_CPY(PDAG); }

  PDAG::PDAG(PDAG&& g) : NodeGraphPart(std::move(g)), MixedGraph(std::move(g)) {
    GUM_CONS_MOV(PDAG);
  }

  PDAG::~PDAG() { GUM_DESTRUCTOR(PDAG); }

  UndiGraph PDAG::moralGraph() const { return graph::moralGraph(*this); }

  bool rec_hasMixedReallyOrientedPath(const PDAG& gr,
                                      NodeSet&    marked,
                                      NodeId      node,
                                      NodeId      goal,
                                      bool        alreadyOriented) {
    if (node == goal) return alreadyOriented;
    if (marked.contains(node)) return false;
    marked.insert(node);
    for (const auto nod: gr.children(node))
      if (rec_hasMixedReallyOrientedPath(gr, marked, nod, goal, true)) return true;
    for (const auto nod: gr.neighbours(node))
      if (rec_hasMixedReallyOrientedPath(gr, marked, nod, goal, alreadyOriented)) return true;
    return false;
  }

  bool PDAG::hasMixedReallyOrientedPath(gum::NodeId n1, gum::NodeId n2) const {
    if (n1 == n2) return false;
    NodeSet marked;   // marked as already explored
    for (const auto nod: this->children(n1))
      if (rec_hasMixedReallyOrientedPath(*this, marked, nod, n2, true)) return true;

    for (const auto nod: this->neighbours(n1))
      if (rec_hasMixedReallyOrientedPath(*this, marked, nod, n2, false)) return true;

    return false;
  }

  UndiGraph PDAG::moralizedAncestralGraph(const NodeSet& nodes) const {
    return graph::moralizedAncestralGraph(*this, nodes);
  }

  bool PDAG::cSeparation(NodeId X, NodeId Y, const NodeSet& Z) const {
    return graph::cSeparated(*this, X, Y, Z);
  }

  bool PDAG::cSeparation(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const {
    return graph::cSeparated(*this, X, Y, Z);
  }

  std::string PDAG::toDot() const {
    std::stringstream output;
    List< NodeId >    treatedNodes;
    output << "digraph \""
           << "no_name\" {" << std::endl;

    std::string tab = "  ";
    output << tab << "rankdir = TD;" << std::endl;
    output << tab << "node [style=filled,fillcolor=white,color=black];" << std::endl;
    output << tab << "graph [style=filled,color=\"#F5F5F5\"];" << std::endl;

    output << std::endl;
    for (const auto node: nodes()) {
      if (neighbours(node).empty()) {
        output << tab << node << ";" << std::endl;
        treatedNodes.insert(node);
      }
    }
    output << std::endl;

    int cluster = 0;
    for (const auto node: nodes()) {
      if (!treatedNodes.exists(node)) {
        output << tab << "subgraph cluster_" << cluster++ << "{{" << std::endl;
        output << tab << tab << "rank=same;" << std::endl << tab << tab;
        for (const auto cc: chainComponent(node)) {
          output << cc << ";";
          treatedNodes.insert(cc);
        }
        output << std::endl << tab << "}}" << std::endl << std::endl;
      }
    }

    for (const auto node: nodes()) {
      for (const auto child: children(node)) {
        output << tab << node << "->" << child << ";\n";
      }
    }
    output << std::endl << tab << "edge [dir=none];" << std::endl;

    for (const auto node: nodes()) {
      for (const auto other: neighbours(node))
        if (other > node) output << tab << node << "->" << other << ";\n";
    }
    output << "}\n";
    return output.str();
  }

} /* namespace gum */
