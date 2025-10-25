/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
#include <agrum/BN/algorithms/essentialGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/BN/algorithms/essentialGraph_inl.h>
#endif   // GU%_NO_INLINE

namespace gum {
  EssentialGraph::EssentialGraph(const DAGmodel& m) : _dagmodel_(&m) { _buildEssentialGraph_(); }

  EssentialGraph::EssentialGraph(const DAGmodel& m, const PDAG& mg) : _dagmodel_(&m), _pdag_(mg) {}

  EssentialGraph::EssentialGraph(const EssentialGraph& g) {
    _dagmodel_ = g._dagmodel_;
    _buildEssentialGraph_();
  }

  EssentialGraph& EssentialGraph::operator=(const EssentialGraph& g) {
    if (&g != this) {
      _dagmodel_ = g._dagmodel_;
      _buildEssentialGraph_();
    }
    return *this;
  }

  EssentialGraph::~EssentialGraph() = default;

  void EssentialGraph::_buildEssentialGraph_() {
    MixedGraph mg;   // during the process, the graph may not be a PDAG
    _pdag_.clear();
    if (_dagmodel_ == nullptr) return;

    for (const auto& node: _dagmodel_->nodes()) {
      mg.addNodeWithId(node);
      _pdag_.addNodeWithId(node);
    }
    for (const auto& arc: _dagmodel_->arcs()) {
      mg.addArc(arc.tail(), arc.head());
    }

    std::vector< Arc > v;
    do {
      v.clear();
      for (const auto x: _dagmodel_->topologicalOrder())
        for (const auto y: mg.children(x))
          if (!_strongly_protected_(mg, x, y)) v.emplace_back(x, y);

      for (const auto& arc: v) {
        mg.eraseArc(arc);
        mg.addEdge(arc.tail(), arc.head());
      }
    } while (!v.empty());

    for (const auto& arc: mg.arcs()) {
      _pdag_.addArc(arc.tail(), arc.head());
    }
    for (const auto& edge: mg.edges()) {
      _pdag_.addEdge(edge.first(), edge.second());
    }
  }

  bool EssentialGraph::_strongly_protected_(NodeId a, NodeId b) const {
    return _strongly_protected_(_pdag_, a, b);
  }

  bool EssentialGraph::_strongly_protected_(MixedGraph mg, NodeId a, NodeId b) {
    // testing a->b from
    // A Characterization of Markov Equivalence Classes for Acyclic Digraphs (2001)
    //  Steen A. Andersson, David Madigan, and Michael D. Perlman*

    // condition (a)
    for (const auto& c: mg.parents(a)) {
      if (!mg.existsArc(c, b)) { return true; }
    }


    for (const auto& c: mg.parents(b)) {
      if (c == a) { continue; }
      // condition (c)
      if (mg.existsArc(a, c)) { return true; }

      // condition (b) knowing that a can not be a parent of c (condition below)
      if (!mg.existsEdge(a, c) && !mg.existsArc(c, a)) { return true; }
    }

    // condition (d)
    bool oneFound = false;
    for (const auto& c: mg.parents(b)) {
      if (c == a) { continue; }
      // condition (d)
      if (mg.existsEdge(c, a)) {
        if (oneFound) {   // this is the second found
          return true;
        }
        oneFound = true;
      }
    }

    return false;
  }

  std::string EssentialGraph::toDot() const {
    std::stringstream output;
    std::stringstream nodeStream;
    std::stringstream edgeStream;
    List< NodeId >    treatedNodes;
    output << "digraph \""
           << "no_name\" {" << std::endl;
    nodeStream << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";
    if (_dagmodel_ != nullptr) {
      for (const auto node: _pdag_.nodes()) {
        nodeStream << tab << node << "[label=\"" << _dagmodel_->variable(node).name() << "\"];";

        for (const auto nei: _pdag_.neighbours(node))
          if (!treatedNodes.exists(nei))
            edgeStream << tab << node << " -> " << nei << " [dir=none];" << std::endl;

        for (const auto chi: _pdag_.children(node))
          edgeStream << tab << node << " -> " << chi << " [color=red];" << std::endl;

        treatedNodes.insert(node);
      }
    }
    output << nodeStream.str() << std::endl << edgeStream.str() << std::endl << "}" << std::endl;

    return output.str();
  }

  UndiGraph EssentialGraph::skeleton() const {
    UndiGraph skel;
    for (const auto& n: nodes())
      skel.addNodeWithId(n);
    for (const auto& edge: edges())
      skel.addEdge(edge.first(), edge.second());
    for (const auto& arc: arcs())
      skel.addEdge(arc.tail(), arc.head());
    return skel;
  }
}   // namespace gum
