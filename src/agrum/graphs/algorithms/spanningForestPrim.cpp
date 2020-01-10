
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief The Prim algorithm for computing min cost spanning trees or forests
 *
 * @author Jean-Philippe DUBUS and Christophe GONZALES
 */

#include <agrum/agrum.h>

#include <agrum/graphs/algorithms/spanningForestPrim.h>

namespace gum {

  /// Default constructor
  SpanningForestPrim::SpanningForestPrim(const UndiGraph*             graph,
                                         const EdgeProperty< float >* cost) :
      SpanningForest(),
      __graph(*graph), __costTable(*cost), __spanning_tree_cost(0),
      __require_computation(true) {
    if (!graph || !cost) {
      GUM_ERROR(GraphError, "invalid null graph or edge cost pointer");
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(SpanningForestPrim);
  }

  // copy constructor
  SpanningForestPrim::SpanningForestPrim(const SpanningForestPrim& from) :
      SpanningForest(), __graph(from.__graph), __costTable(from.__costTable),
      __edgesToExplore(from.__edgesToExplore),
      __spanning_tree(from.__spanning_tree),
      __spanning_tree_cost(from.__spanning_tree_cost),
      __require_computation(from.__require_computation) {
    // for debugging purposes
    GUM_CONS_CPY(SpanningForestPrim);
  }

  // destructor
  SpanningForestPrim::~SpanningForestPrim() {
    // for debugging purposes
    GUM_DESTRUCTOR(SpanningForestPrim);
  }

  /// Returns the cost of the spanning forest
  float SpanningForestPrim::costOfSpanningForest() {
    if (__require_computation) __compute();

    return __spanning_tree_cost;
  }

  /// Returns the edges in a min cost spanning forest
  const EdgeSet& SpanningForestPrim::edgesInSpanningForest() {
    if (__require_computation) __compute();

    return __spanning_tree.edges();
  }

  /// Construct the spanning forest
  const UndiGraph& SpanningForestPrim::spanningForest() {
    if (__require_computation) __compute();

    return __spanning_tree;
  }

  /// compute the spanning forest
  void SpanningForestPrim::__compute() {
    // compute a spanning tree in every connected component
    for (const auto node: __graph.nodes()) {
      if (!__spanning_tree.existsNode(node)) { __computeInAComponent(node); }
    }

    // indicate that everything was computed
    __require_computation = false;
  }

  /// compute a spanning tree
  void SpanningForestPrim::__computeInAComponent(const NodeId id) {
    // add the node to the spanning tree
    __spanning_tree.addNodeWithId(id);

    // explore its neighborhood
    __exploreNode(id);

    // get the next nodes to link to the current spanning tree nodes

    while (!__edgesToExplore.empty()) {
      const Edge   edge = __edgesToExplore.pop();
      const NodeId first = edge.first();
      const NodeId second = edge.second();

      // consider only the edges that have one extremal node not in the spanning
      // tree as those that can be added to the tree

      if (!__spanning_tree.existsNode(first)) {
        // add the edge to the spanning tree
        __spanning_tree.addNodeWithId(first);
        __spanning_tree.addEdge(first, second);
        __spanning_tree_cost += __costTable[edge];

        // We must explore the first node's neighborhood
        __exploreNode(first);
      } else if (!__spanning_tree.existsNode(second)) {
        // add the edge to the spanning tree
        __spanning_tree.addNodeWithId(second);
        __spanning_tree.addEdge(first, second);
        __spanning_tree_cost += __costTable[edge];

        // We must explore the second node
        __exploreNode(second);
      }
    }
  }

  /// explore the neighborhood of a node belonging to the spanning tree
  void SpanningForestPrim::__exploreNode(const NodeId id) {
    // add its neighbors __edgesToExplore to indicate that they are
    // potential next nodes to explore
    for (const auto node: __graph.neighbours(id)) {
      if (!__spanning_tree.existsNode(node)) {
        Edge edge(node, id);
        __edgesToExplore.insert(edge, __costTable[edge]);
      }
    }
  }

} /* namespace gum */
