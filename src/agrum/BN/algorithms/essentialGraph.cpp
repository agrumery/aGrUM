/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief Source implementation of the class building the essential Graph from a
 * DAGmodel
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/BN/algorithms/essentialGraph.h>

#ifdef GUM_NO_INLINE
#include <agrum/BN/algorithms/essentialGraph_inl.h>
#endif  // GUM_NOINLINE

namespace gum {
  EssentialGraph::EssentialGraph(const DAGmodel& m)
      : __dagmodel(&m) {
    __buildEssentialGraph();
  }

  EssentialGraph::EssentialGraph(const DAGmodel& m, const MixedGraph& mg)
      : __dagmodel(&m)
      , __mg(mg) {}
  EssentialGraph::EssentialGraph(const EssentialGraph& g) {
    __dagmodel = g.__dagmodel;
    __buildEssentialGraph();
  }
  EssentialGraph& EssentialGraph::operator=(const EssentialGraph& g) {
    if (&g != this) {
      __dagmodel = g.__dagmodel;
      __buildEssentialGraph();
    }
    return *this;
  }

  EssentialGraph::~EssentialGraph() {}

  void EssentialGraph::__buildEssentialGraph() {
    __mg.clear();
    if (__dagmodel == nullptr) return;

    for (const auto& node : __dagmodel->nodes()) {
      __mg.addNode(node);
    }
    for (const auto& arc : __dagmodel->arcs()) {
      __mg.addArc(arc.tail(), arc.head());
    }

    std::vector< Arc > v;
    do {
      v.clear();
      for (const auto x : __dagmodel->topologicalOrder())
        for (const auto y : __mg.children(x))
          if (!__strongly_protected(x, y)) v.push_back(Arc(x, y));
      for (const auto& arc : v) {
        __mg.eraseArc(arc);
        __mg.addEdge(arc.tail(), arc.head());
      }
    } while (v.size() > 0);
  }

  bool EssentialGraph::__strongly_protected(NodeId a, NodeId b) {
    // testing a->b from
    // A Characterization of Markov Equivalence Classes for Acyclic Digraphs (2001)
    //  Steen A. Andersson, David Madigan, and Michael D. Perlman*

    // condition (c)
    if ((__mg.parents(a) - __mg.parents(b)).size() > 0) {
      return true;
    }

    NodeSet cs;
    for (const auto& c : __mg.parents(b)) {
      // condition (b) & (c)
      if (c == a) {
        continue;
      }
      if (!__mg.existsEdge(c, a)) {
        return true;
      } else {
        cs.insert(c);
      }
    }

    // condition (d)
    NodeSet ss(cs);
    if (cs.size() < 2) {
      return false;
    } else {
      for (const auto& i : cs) {
        ss = ss - __mg.neighbours(i);
        if (ss.size() < 2) {
          return false;
        }
      }
      return true;
    }
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
    if (__dagmodel != nullptr) {
      for (const auto node : __mg.nodes()) {
        nodeStream << tab << node << "[label=\""
                   << __dagmodel->variable(node).name() << "\"];";

        for (const auto nei : __mg.neighbours(node))
          if (!treatedNodes.exists(nei))
            edgeStream << tab << node << " -> " << nei << " [dir=none];"
                       << std::endl;

        for (const auto chi : __mg.children(node))
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
}  // namespace gum
