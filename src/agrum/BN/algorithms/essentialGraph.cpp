/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */
#include <agrum/BN/algorithms/essentialGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/BN/algorithms/essentialGraph_inl.h>
#endif   // GUM_NOINLINE

namespace gum {
  EssentialGraph::EssentialGraph(const DAGmodel& m) : dagmodel__(&m) {
    buildEssentialGraph__();
  }

  EssentialGraph::EssentialGraph(const DAGmodel& m, const MixedGraph& mg) :
      dagmodel__(&m), mg__(mg) {}
  EssentialGraph::EssentialGraph(const EssentialGraph& g) {
    dagmodel__ = g.dagmodel__;
    buildEssentialGraph__();
  }
  EssentialGraph& EssentialGraph::operator=(const EssentialGraph& g) {
    if (&g != this) {
      dagmodel__ = g.dagmodel__;
      buildEssentialGraph__();
    }
    return *this;
  }

  EssentialGraph::~EssentialGraph() = default;

  void EssentialGraph::buildEssentialGraph__() {
    mg__.clear();
    if (dagmodel__ == nullptr) return;

    for (const auto& node: dagmodel__->nodes()) {
      mg__.addNodeWithId(node);
    }
    for (const auto& arc: dagmodel__->arcs()) {
      mg__.addArc(arc.tail(), arc.head());
    }

    std::vector< Arc > v;
    do {
      v.clear();
      for (const auto x: dagmodel__->topologicalOrder())
        for (const auto y: mg__.children(x))
          if (!strongly_protected__(x, y)) v.emplace_back(x, y);

      for (const auto& arc: v) {
        mg__.eraseArc(arc);
        mg__.addEdge(arc.tail(), arc.head());
      }
    } while (!v.empty());
  }

  bool EssentialGraph::strongly_protected__(NodeId a, NodeId b) {
    // testing a->b from
    // A Characterization of Markov Equivalence Classes for Acyclic Digraphs (2001)
    //  Steen A. Andersson, David Madigan, and Michael D. Perlman*

    // condition (a)
    for (const auto& c: mg__.parents(a)) {
      if (!mg__.existsArc(c, b)) { return true; }
    }


    for (const auto& c: mg__.parents(b)) {
      if (c == a) { continue; }
      // condition (c)
      if (mg__.existsArc(a, c)) { return true; }

      // condition (b) knowing that a can not be a parent of c (condition below)
      if (!mg__.existsEdge(a, c) && !mg__.existsArc(c, a)) { return true; }
    }

    // condition (d)
    bool oneFound = false;
    for (const auto& c: mg__.parents(b)) {
      if (c == a) { continue; }
      // condition (d)
      if (mg__.existsEdge(c, a)) {
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
    if (dagmodel__ != nullptr) {
      for (const auto node: mg__.nodes()) {
        nodeStream << tab << node << "[label=\""
                   << dagmodel__->variable(node).name() << "\"];";

        for (const auto nei: mg__.neighbours(node))
          if (!treatedNodes.exists(nei))
            edgeStream << tab << node << " -> " << nei << " [dir=none];"
                       << std::endl;

        for (const auto chi: mg__.children(node))
          edgeStream << tab << node << " -> " << chi << " [color=red];"
                     << std::endl;

        treatedNodes.insert(node);
      }
    }
    output << nodeStream.str() << std::endl
           << edgeStream.str() << std::endl
           << "}" << std::endl;

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
