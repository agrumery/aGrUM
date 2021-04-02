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
 * @brief Source implementation of Base classes for directed acyclic graphs
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */
#include <agrum/tools/graphs/DAG.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/DAG_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  // diamond structure require to explicitly initialize NodeGraphPart
  DAG::DAG(Size nodes_size,
           bool nodes_resize_policy,
           Size arcs_size,
           bool arcs_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy),
      DiGraph(nodes_size, nodes_resize_policy, arcs_size, arcs_resize_policy) {
    GUM_CONSTRUCTOR(DAG);
  }

  // diamond structure require to explicitly initialize NodeGraphPart
  DAG::DAG(const DAG& g) : NodeGraphPart(g), DiGraph(g) { GUM_CONS_CPY(DAG); }

  DAG::~DAG() { GUM_DESTRUCTOR(DAG); }


  UndiGraph DAG::moralGraph() const {
    UndiGraph moralgraph;
    moralgraph.populateNodes(*this);
    // transform the arcs into edges
    for (const auto& arc: arcs())
      moralgraph.addEdge(arc.first(), arc.second());

    // marry the parents
    for (const auto node: nodes()) {
      const auto& par = parents(node);

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


  UndiGraph DAG::moralizedAncestralGraph(const NodeSet& nodes) const {
    UndiGraph res;
    NodeSet   tmp{nodes};

    // findings all nodes
    while (!tmp.empty()) {
      auto current = *(tmp.begin());
      tmp.erase(current);

      res.addNodeWithId(current);
      for (auto next: parents(current))
        if (!tmp.contains(next) && !res.exists(next)) tmp.insert(next);
    }

    // finding all edges and moralizing
    for (auto current: res)
      for (auto father: parents(current)) {
        res.addEdge(current,
                    father);   // addEdge does not complain if edge already exists
        for (auto other_father: parents(current))
          if (other_father != father) res.addEdge(father, other_father);
      }

    return res;
  }

  bool DAG::dSeparation(NodeId X, NodeId Y, const NodeSet& Z) const {
    NodeSet cumul{Z};
    cumul << X << Y;
    auto g = moralizedAncestralGraph(cumul);
    for (auto node: Z)
      g.eraseNode(node);
    return !g.hasUndirectedPath(X, Y);
  }

  bool
     DAG::dSeparation(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const {
    if (!(X * Y).empty())
      GUM_ERROR(InvalidArgument,
                "NodeSets " << X << ", " << Y << " should have no intersection")

    NodeSet cumul{Z};
    cumul += X;
    cumul += Y;
    auto g = moralizedAncestralGraph(cumul);
    for (auto node: Z)
      g.eraseNode(node);
    auto cc = g.nodes2ConnectedComponent();

    NodeSet Xcc, Ycc;
    for (const auto node: X)
      if (g.existsNode(node))   // it may be in Z too
        if (!Xcc.exists(cc[node])) Xcc.insert(cc[node]);
    for (const auto node: Y)
      if (g.existsNode(node))   // it may be in Z too
        if (!Ycc.exists(cc[node])) Ycc.insert(cc[node]);

    return (Xcc * Ycc).empty();
  }
} /* namespace gum */
