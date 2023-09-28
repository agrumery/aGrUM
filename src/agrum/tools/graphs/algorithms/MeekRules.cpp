/**
*
*   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
* @brief source implementations of simplicial set
*
* @author Mahdi HADJ ALI(_at_LIP6) and Pierre-Henri WUILLEMIN(_at_LIP6)
*/
#include <agrum/agrum.h>
#include <agrum/tools/graphs/mixedGraph.h>
#include <agrum/tools/graphs/algorithms/MeekRules.h>

namespace gum{

  /// default constructor
  MeekRules::MeekRules(){ GUM_CONSTRUCTOR(MeekRules);}

  /// destructor
  MeekRules::~MeekRules(){
    GUM_DESTRUCTOR(MeekRules);
  }

  /// Propagates MeekRules in a MixedGraph
  MixedGraph MeekRules::propagatesOrientations(MixedGraph graph, std::vector< Arc > _latentCouples_) {
    const Sequence< NodeId > order = graph.topologicalOrder();

    // Propagates existing orientations thanks to Meek rules
    bool newOrientation = true;
    while (newOrientation) {
      newOrientation = false;
      for (NodeId x: order) {
        if (!graph.parents(x).empty()) {
          newOrientation |= propagatesRemainingOrientableEdges_(graph, x, _latentCouples_);
        }
      }
    }
    return graph;
  }

  MixedGraph MeekRules::orientAllEdges(MixedGraph graph, std::vector< Arc > _latentCouples_){

    const Sequence< NodeId > order = graph.topologicalOrder();

    //Propagates existing orientations thanks to rules
    bool newOrientation = true;
    while (newOrientation) {
      newOrientation = false;
      for (NodeId x: order) {
        if (!graph.parents(x).empty()) {
          newOrientation |= propagatesRemainingOrientableEdges_(graph, x, _latentCouples_);
        }
      }
    }

    //GUM_TRACE(graph.toDot())
    // Orient remaining edges
    propagatesOrientationInChainOfRemainingEdges_(graph, _latentCouples_);

    return graph;
  }

  /// Propagates the orientation from a node to its neighbours
  bool MeekRules::propagatesRemainingOrientableEdges_(gum::MixedGraph& graph,
                                                      NodeId xj,
                                                      std::vector< Arc > _latentCouples_) {
    bool       res        = false;
    const auto neighbours = graph.neighbours(xj);
    for (auto& xi: neighbours) {
      bool i_j = isOrientable_(graph, xi, xj);
      bool j_i = isOrientable_(graph, xj, xi);
      if (i_j || j_i) {
        // GUM_SL_EMIT(xi, xj, "Removing Edge", "line 660")
        graph.eraseEdge(Edge(xi, xj));
        res = true;
      }
      if (i_j) {
        graph.eraseEdge(Edge(xi, xj));
        graph.addArc(xi, xj);
        propagatesRemainingOrientableEdges_(graph, xj, _latentCouples_);
      }
      if (j_i) {
        graph.eraseEdge(Edge(xi, xj));
        graph.addArc(xj, xi);
        propagatesRemainingOrientableEdges_(graph, xi, _latentCouples_);
      }
      if (i_j && j_i) {
        // GUM_TRACE(" + add arc (" << xi << "," << xj << ")")
        _latentCouples_.emplace_back(xi, xj);
      }
    }
    return res;
  }

  bool MeekRules::isOrientable_(const MixedGraph& graph, NodeId xi, NodeId xj) const {
    // no cycle
    if (_existsDirectedPath_(graph, xj, xi)) {
      // GUM_TRACE("cycle(" << xi << "-" << xj << ")")
      return false;
    }
    // R1
    if (!(graph.parents(xi) - graph.boundary(xj)).empty()) {
      // GUM_TRACE("R1(" << xi << "-" << xj << ")")
      return true;
    }
    // R2
    if (_existsDirectedPath_(graph, xi, xj)) {
      // GUM_TRACE("R2(" << xi << "-" << xj << ")")
      return true;
    }
    // R3
    int nbr = 0;
    for (const auto p: graph.parents(xj)) {
      if (!graph.mixedOrientedPath(xi, p).empty()) {
        nbr += 1;
        if (nbr == 2) {
          // GUM_TRACE("R3(" << xi << "-" << xj << ")")
          return true;
        }
      }
    }
    return false;
  }

  /// Arbitrary propagation if we can't propagate thanks to MeekRules
  void MeekRules::propagatesOrientationInChainOfRemainingEdges_(MixedGraph& essentialGraph, std::vector< Arc > _latentCouples_) {
    // then decide the orientation for remaining edges
    while (!essentialGraph.edges().empty()) {
      const auto& edge               = *(essentialGraph.edges().begin());
      NodeId      root               = edge.first();
      Size        size_children_root = essentialGraph.children(root).size();
      NodeSet     visited;
      NodeSet     stack{root};

      // check the best root for the set of neighbours
      while (!stack.empty()) {
        NodeId next = *(stack.begin());
        stack.erase(next);
        if (visited.contains(next)) continue;
        if (essentialGraph.children(next).size() > size_children_root) {
          size_children_root = essentialGraph.children(next).size();
          root               = next;
        }
        for (const auto n: essentialGraph.neighbours(next))
          if (!stack.contains(n) && !visited.contains(n)) stack.insert(n);
        visited.insert(next);
      }

      // orientation now
      visited.clear();
      stack.clear();
      stack.insert(root);
      while (!stack.empty()) {
        // GUM_TRACE("stack : " << stack)
        NodeId next = *(stack.begin());
        stack.erase(next);
        // GUM_TRACE("next : " << next)
        if (visited.contains(next)) continue;
        const auto nei = essentialGraph.neighbours(next);
        for (const auto n: nei) {
          // GUM_TRACE("n : "<< n)
          if (!stack.contains(n) && !visited.contains(n)) stack.insert(n);
          // GUM_TRACE(" + amap reasonably orientation for " << n << "->" << next);
          if (propagatesRemainingOrientableEdges_(essentialGraph, next, _latentCouples_)) {
            continue;
          } else {
            if (!essentialGraph.existsArc(next,
                                          n)) {   // Checking that we're not creating a
                                                  // doubly-oriented arc by adding a "random" arc.
              essentialGraph.eraseEdge(Edge(n, next));
              essentialGraph.addArc(n, next);
              //GUM_TRACE(" + add arc (" << n << "->" << next << ")")
            }
          }
        }
        visited.insert(next);
      }
    }
  }

  bool MeekRules::_existsDirectedPath_(const MixedGraph& graph, const NodeId n1, const NodeId n2) {
    // not recursive version => use a FIFO for simulating the recursion
    List< NodeId > nodeFIFO;
    // mark[node] = successor if visited, else mark[node] does not exist
    Set< NodeId > mark;

    mark.insert(n2);
    nodeFIFO.pushBack(n2);

    NodeId current;

    while (!nodeFIFO.empty()) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      // check the parents
      for (const auto new_one: graph.parents(current)) {
        if (graph.existsArc(current,
                            new_one))   // if there is a double arc, pass
          continue;

        if (new_one == n1) { return true; }

        if (mark.exists(new_one))   // if this node is already marked, do not
          continue;                 // check it again

        mark.insert(new_one);
        nodeFIFO.pushBack(new_one);
      }
    }

    return false;
  }

} ///namespace gum