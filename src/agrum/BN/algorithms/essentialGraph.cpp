/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


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
#endif   // GUM_NOINLINE

namespace gum {
  EssentialGraph::EssentialGraph(const DAGmodel& m) : _dagmodel_(&m) { _buildEssentialGraph_(); }

  EssentialGraph::EssentialGraph(const DAGmodel& m, const MixedGraph& mg) :
      _dagmodel_(&m), _mg_(mg) {}
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
    _mg_.clear();
    if (_dagmodel_ == nullptr) return;

    for (const auto& node: _dagmodel_->nodes()) {
      _mg_.addNodeWithId(node);
    }
    for (const auto& arc: _dagmodel_->arcs()) {
      _mg_.addArc(arc.tail(), arc.head());
    }

    std::vector< Arc > v;
    do {
      v.clear();
      for (const auto x: _dagmodel_->topologicalOrder())
        for (const auto y: _mg_.children(x))
          if (!_strongly_protected_(x, y)) v.emplace_back(x, y);

      for (const auto& arc: v) {
        _mg_.eraseArc(arc);
        _mg_.addEdge(arc.tail(), arc.head());
      }
    } while (!v.empty());
  }

  bool EssentialGraph::_strongly_protected_(NodeId a, NodeId b) {
    // testing a->b from
    // A Characterization of Markov Equivalence Classes for Acyclic Digraphs (2001)
    //  Steen A. Andersson, David Madigan, and Michael D. Perlman*

    // condition (a)
    for (const auto& c: _mg_.parents(a)) {
      if (!_mg_.existsArc(c, b)) { return true; }
    }


    for (const auto& c: _mg_.parents(b)) {
      if (c == a) { continue; }
      // condition (c)
      if (_mg_.existsArc(a, c)) { return true; }

      // condition (b) knowing that a can not be a parent of c (condition below)
      if (!_mg_.existsEdge(a, c) && !_mg_.existsArc(c, a)) { return true; }
    }

    // condition (d)
    bool oneFound = false;
    for (const auto& c: _mg_.parents(b)) {
      if (c == a) { continue; }
      // condition (d)
      if (_mg_.existsEdge(c, a)) {
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
      for (const auto node: _mg_.nodes()) {
        nodeStream << tab << node << "[label=\"" << _dagmodel_->variable(node).name() << "\"];";

        for (const auto nei: _mg_.neighbours(node))
          if (!treatedNodes.exists(nei))
            edgeStream << tab << node << " -> " << nei << " [dir=none];" << std::endl;

        for (const auto chi: _mg_.children(node))
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
