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
 * @brief Class for Meek Rules
 *
 * @author Mahdi Hadj Ali and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/agrum.h>

#include <agrum/tools/graphs/algorithms/MeekRules.h>

namespace gum {

  /// default constructor
  MeekRules::MeekRules() { GUM_CONSTRUCTOR(MeekRules); }

  /// destructor
  MeekRules::~MeekRules() { GUM_DESTRUCTOR(MeekRules); }

  /// Propagates MeekRules in a MixedGraph
  MixedGraph MeekRules::propagates(const MixedGraph& graph) {
    _choices_.clear();
    return _propagates_(graph);
  }

  /// Propagates the orientation of a MixedGraph (no double-headed arcs) and return a PDAG.
  PDAG MeekRules::propagatesToPDAG(const MixedGraph& mg) {
    _choices_.clear();

    MixedGraph graph = _propagates_(mg);

    // Resolve double-headed arc while avoiding cycle creation.
    _orientDoubleHeadedArcs_(graph);

    // cycle have been resolved above, so we can safely convert to PDAG
    PDAG pdag;
    for (auto node: graph) {
      pdag.addNodeWithId(node);
    }
    for (const Edge& edge: graph.edges()) {
      pdag.addEdge(edge.first(), edge.second());
    }
    for (const Arc& arc: graph.arcs()) {
      pdag.addArc(arc.tail(), arc.head());
    }
    return pdag;
  }

  /// Propagates the orientation of a MixedGraph and return a DAG.
  DAG MeekRules::propagatesToDAG(const gum::MixedGraph& mg) {
    _choices_.clear();
    // Orient all remaining  edges into arcs
    MixedGraph graph = _propagates_(mg);

    // complete remaining edges
    _complete_(graph);

    // Resolve double-headed arc while avoiding cycle creation.
    _orientDoubleHeadedArcs_(graph);


    DAG dag;
    for (auto node: graph.nodes()) {
      dag.addNodeWithId(node);
    }
    for (const Arc& arc: graph.arcs()) {
      dag.addArc(arc.tail(), arc.head());
    }
    return dag;
  }

  /// Orient double-headed arcs while avoiding cycles
  void MeekRules::_orientDoubleHeadedArcs_(MixedGraph& mg) {
    gum::ArcSet L;   // create a set of all double-headed arcs
    for (gum::NodeId x: mg.nodes())
      for (NodeId y: mg.parents(x))
        // If there is a mutual parent-child relationship, add the arc to the set
        if (mg.parents(y).contains(x)) {
          if (x > y) {
            continue;   // Avoid duplicate arcs by considering only one direction
          } else {
            L.insert(gum::Arc(x, y));
          }
        }

    // If there are double-headed arcs
    if (not L.empty()) {
      while (true) {
        bool withdrawFlag_L = false;
        for (auto arc: ArcSet(L)) {
          bool tail_head        = _existsDirectedPath_(mg, arc.tail(), arc.head());
          bool head_tail        = _existsDirectedPath_(mg, arc.head(), arc.tail());
          bool withdrawFlag_arc = false;

          // Case 1: There is already a path from tail to head and no path from head to tail
          if (tail_head && !head_tail) {
            // Erase the arc from head to tail to avoid cycles
            mg.eraseArc(Arc(arc.head(), arc.tail()));
            withdrawFlag_arc = true;

            // Case 2: There is already a path from head to tail and no path from tail to head
          } else if (!tail_head && head_tail) {
            // Erase the arc from tail to head to avoid cycles
            mg.eraseArc(Arc(arc.tail(), arc.head()));
            withdrawFlag_arc = true;

            // Case 3: There is no path between tail and head
          } else if (!tail_head && !head_tail) {
            // Choose an arbitrary orientation and erase the corresponding arc
            mg.eraseArc(Arc(arc.head(), arc.tail()));
            withdrawFlag_arc = true;
          }

          // Remove the arc from the set if it was processed
          if (withdrawFlag_arc) {
            L.erase(arc);
            withdrawFlag_L = true;
          }
        }
        // If all double-headed arcs are processed, exit the loop
        if (L.empty()) { break; }

        // If no arcs were withdrawn, erase an arbitrary double arc in the graph (the first one in
        // L). Hoping the situation will improve. ┐(￣ヘ￣)┌ If we arrive here, it's because the
        // double-headed arc creates cycles in both directions
        if (!withdrawFlag_L) {
          auto it  = L.begin();
          auto arc = *it;
          mg.eraseArc(Arc(arc.head(), arc.tail()));
          mg.eraseArc(Arc(arc.tail(), arc.head()));
          L.erase(arc);
        }
      }
    }
  }

  MixedGraph MeekRules::_propagates_(const MixedGraph& mg) {
    MixedGraph graph(mg);
    // Propagates existing orientations thanks to Meek rules
    bool newOrientation = true;
    while (newOrientation) {
      newOrientation = false;
      for (NodeId x: graph.nodes()) {
        if (!graph.parents(x).empty()) {
          newOrientation |= _applyMeekRules_(graph, x);
        }
      }
    }
    return graph;
  }

  void MeekRules::_complete_(MixedGraph& graph) {
    // Propagates existing orientations thanks to rules
    bool newOrientation = true;
    while (newOrientation) {
      newOrientation = false;
      for (NodeId x: graph.nodes()) {
        if (!graph.parents(x).empty()) {
          newOrientation |= _applyMeekRules_(graph, x);
        }
      }
    }

    // GUM_TRACE(graph.toDot())
    //  Orient remaining edges
    _propagatesOrientationInChainOfRemainingEdges_(graph);
  }

  /// Propagates the orientation from a node to its neighbours
  bool MeekRules::_applyMeekRules_(MixedGraph& graph, NodeId xj) {
    bool       res        = false;
    const auto neighbours = graph.neighbours(xj);
    for (auto& xi: neighbours) {
      bool i_j = _isOrientable_(graph, xi, xj);
      bool j_i = _isOrientable_(graph, xj, xi);
      if (i_j || j_i) {
        // GUM_SL_EMIT(xi, xj, "Removing Edge", "line 660")
        graph.eraseEdge(Edge(xi, xj));
        res = true;
      }
      if (i_j) {
        graph.eraseEdge(Edge(xi, xj));
        graph.addArc(xi, xj);
        _applyMeekRules_(graph, xj);
      }
      if (j_i) {
        graph.eraseEdge(Edge(xi, xj));
        graph.addArc(xj, xi);
        _applyMeekRules_(graph, xi);
      }
      if (i_j && j_i) {
        // GUM_TRACE(" + add arc (" << xi << "," << xj << ")")
        _choices_.emplace_back(xi, xj);
      }
    }
    return res;
  }

  bool MeekRules::_isOrientable_(const MixedGraph& graph, NodeId xi, NodeId xj) const {
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
  void MeekRules::_propagatesOrientationInChainOfRemainingEdges_(MixedGraph& essentialGraph) {
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
          if (_applyMeekRules_(essentialGraph, next)) {
            continue;
          } else {
            if (!essentialGraph.existsArc(next,
                                          n)) {   // Checking that we're not creating a
                                                  // doubly-oriented arc by adding a "random" arc.
              essentialGraph.eraseEdge(Edge(n, next));
              essentialGraph.addArc(n, next);
              _choices_.emplace_back(n, next);
              // GUM_TRACE(" + add arc (" << n << "->" << next << ")")
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

}   // namespace gum
