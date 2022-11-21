/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Source implementation of Base classes for partially directed acyclic graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/tools/graphs/PDAG.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/PDAG_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  // diamond structure require to explicitly initialize NodeGraphPart
  PDAG::PDAG(Size nodes_size,
             bool nodes_resize_policy,
             Size arcs_size,
             bool arcs_resize_policy,
             Size edges_size,
             bool edges_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy),
      MixedGraph(nodes_size,
                 nodes_resize_policy,
                 arcs_size,
                 arcs_resize_policy,
                 edges_size,
                 edges_resize_policy) {
    GUM_CONSTRUCTOR(PDAG);
  }

  PDAG::PDAG(const UndiGraph& g) : NodeGraphPart(g), MixedGraph(g) { GUM_CONSTRUCTOR(PDAG); }

  PDAG::PDAG(const DAG& g) : NodeGraphPart(g), MixedGraph(g) { GUM_CONSTRUCTOR(PDAG); }

  PDAG::PDAG(const PDAG& g) : NodeGraphPart(g), MixedGraph(g) { GUM_CONS_CPY(PDAG); }

  PDAG::~PDAG() { GUM_DESTRUCTOR(PDAG); }


  UndiGraph PDAG::moralGraph() const {
    UndiGraph moralgraph;
    moralgraph.populateNodes(*this);
    // transform the arcs into edges
    for (const auto& arc: arcs())
      moralgraph.addEdge(arc.first(), arc.second());

    // marry the parents
    NodeSet already;
    for (const auto node: nodes()) {
      if (already.contains(node)) continue;

      already.insert(node);
      auto par = parents(node);
      for (const auto nei: neighbours(node)) {
        already.insert(nei);
        par += parents(nei);
      }

      for (auto it1 = par.begin(); it1 != par.end(); ++it1) {
        auto it2 = it1;
        for (++it2; it2 != par.end(); ++it2) {
          // will automatically check if this edge already exists
          moralgraph.addEdge(*it1, *it2);
        }
      }
    }
    return moralgraph;
  }
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

  void rec_ancestral(const PDAG& graph, PDAG& ancestral, NodeId nod) {
    for (const auto par: graph.parents(nod)) {
      if (!ancestral.existsNode(par)) {
        ancestral.addNodeWithId(par);
        rec_ancestral(graph, ancestral, par);
      }
      ancestral.addArc(par, nod);
    }
    for (const auto nei: graph.neighbours(nod)) {
      if (!ancestral.existsNode(nei)) {
        ancestral.addNodeWithId(nei);
        rec_ancestral(graph, ancestral, nei);
      }
      ancestral.addEdge(nei, nod);
    }
  }

  UndiGraph PDAG::moralizedAncestralGraph(const NodeSet& nodes) const {
    PDAG ancestral;
    for (const auto n: nodes)
      rec_ancestral(*this, ancestral, n);

    return ancestral.moralGraph();
  }

  bool PDAG::cSeparation(NodeId X, NodeId Y, const NodeSet& Z) const {
    NodeSet cumul{Z};
    cumul << X << Y;
    auto g = moralizedAncestralGraph(cumul);
    for (auto node: Z)
      g.eraseNode(node);
    return !g.hasUndirectedPath(X, Y);
  }

  bool PDAG::cSeparation(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const {
    if (!(X * Y).empty())
      GUM_ERROR(InvalidArgument, "NodeSets " << X << ", " << Y << " should have no intersection")

    NodeSet cumul{Z};
    cumul += X;
    cumul += Y;
    auto g = moralizedAncestralGraph(cumul);
    for (auto node: Z)
      g.eraseNode(node);
    auto cc = g.nodes2ConnectedComponent();

    NodeSet Xcc;
    NodeSet Ycc;
    for (const auto node: X)
      if (g.existsNode(node) && !Xcc.exists(cc[node]))   // it may be in Z too
        Xcc.insert(cc[node]);
    for (const auto node: Y)
      if (g.existsNode(node) && !Ycc.exists(cc[node]))   // it may be in Z too
        Ycc.insert(cc[node]);
    return (Xcc * Ycc).empty();
  }
} /* namespace gum */
