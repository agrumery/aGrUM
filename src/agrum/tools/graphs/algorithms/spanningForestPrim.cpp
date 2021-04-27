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
 * @brief The Prim algorithm for computing min cost spanning trees or forests
 *
 * @author Jean-Philippe DUBUS & Christophe GONZALES(@AMU)
 */

#include <agrum/agrum.h>

#include <agrum/tools/graphs/algorithms/spanningForestPrim.h>

namespace gum {

  /// Default constructor
  SpanningForestPrim::SpanningForestPrim(const UndiGraph*             graph,
                                         const EdgeProperty< float >* cost) :
      SpanningForest(),
      graph__(*graph), costTable__(*cost), spanning_tree_cost__(0),
      require_computation__(true) {
    if (!graph || !cost) {
      GUM_ERROR(GraphError, "invalid null graph or edge cost pointer");
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(SpanningForestPrim);
  }

  // copy constructor
  SpanningForestPrim::SpanningForestPrim(const SpanningForestPrim& from) :
      SpanningForest(), graph__(from.graph__), costTable__(from.costTable__),
      edgesToExplore__(from.edgesToExplore__),
      spanning_tree__(from.spanning_tree__),
      spanning_tree_cost__(from.spanning_tree_cost__),
      require_computation__(from.require_computation__) {
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
    if (require_computation__) compute__();

    return spanning_tree_cost__;
  }

  /// Returns the edges in a min cost spanning forest
  const EdgeSet& SpanningForestPrim::edgesInSpanningForest() {
    if (require_computation__) compute__();

    return spanning_tree__.edges();
  }

  /// Construct the spanning forest
  const UndiGraph& SpanningForestPrim::spanningForest() {
    if (require_computation__) compute__();

    return spanning_tree__;
  }

  /// compute the spanning forest
  void SpanningForestPrim::compute__() {
    // compute a spanning tree in every connected component
    for (const auto node: graph__.nodes()) {
      if (!spanning_tree__.existsNode(node)) { computeInAComponent__(node); }
    }

    // indicate that everything was computed
    require_computation__ = false;
  }

  /// compute a spanning tree
  void SpanningForestPrim::computeInAComponent__(const NodeId id) {
    // add the node to the spanning tree
    spanning_tree__.addNodeWithId(id);

    // explore its neighborhood
    exploreNode__(id);

    // get the next nodes to link to the current spanning tree nodes

    while (!edgesToExplore__.empty()) {
      const Edge   edge   = edgesToExplore__.pop();
      const NodeId first  = edge.first();
      const NodeId second = edge.second();

      // consider only the edges that have one extremal node not in the spanning
      // tree as those that can be added to the tree

      if (!spanning_tree__.existsNode(first)) {
        // add the edge to the spanning tree
        spanning_tree__.addNodeWithId(first);
        spanning_tree__.addEdge(first, second);
        spanning_tree_cost__ += costTable__[edge];

        // We must explore the first node's neighborhood
        exploreNode__(first);
      } else if (!spanning_tree__.existsNode(second)) {
        // add the edge to the spanning tree
        spanning_tree__.addNodeWithId(second);
        spanning_tree__.addEdge(first, second);
        spanning_tree_cost__ += costTable__[edge];

        // We must explore the second node
        exploreNode__(second);
      }
    }
  }

  /// explore the neighborhood of a node belonging to the spanning tree
  void SpanningForestPrim::exploreNode__(const NodeId id) {
    // add its neighbors edgesToExplore__ to indicate that they are
    // potential next nodes to explore
    for (const auto node: graph__.neighbours(id)) {
      if (!spanning_tree__.existsNode(node)) {
        Edge edge(node, id);
        edgesToExplore__.insert(edge, costTable__[edge]);
      }
    }
  }

} /* namespace gum */
