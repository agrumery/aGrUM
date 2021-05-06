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
 * @brief inline source of basic clique graphs
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parser in IDE
#  include <agrum/tools/graphs/cliqueGraph.h>

namespace gum {

  /// copy operator
  INLINE CliqueGraph& CliqueGraph::operator=(const CliqueGraph& g) {
    if (this != &g) {
      UndiGraph::operator=(g);
      cliques__          = g.cliques__;
      separators__       = g.separators__;
    }

    return *this;
  }

  INLINE void CliqueGraph::addEdge(const NodeId first, const NodeId second) {
    Edge edge(first, second);

    if (!existsEdge(edge)) {
      // create the edge in the graph
      UndiGraph::addEdge(first, second);

      // create the separator
      separators__.insert(edge, cliques__[first] * cliques__[second]);
    }
  }

  /// removes an edge (and its separator) from the clique graph

  INLINE void CliqueGraph::eraseEdge(const Edge& edge) {
    if (existsEdge(edge)) {
      separators__.erase(edge);
      UndiGraph::eraseEdge(edge);
    }
  }

  /// adds a new clique to the graph

  INLINE NodeId CliqueGraph::addNode(const NodeSet& clique) {
    // create the new node in the graph
    NodeId new_node = UndiGraph::addNode();

    // update the set of nodes of the clique
    cliques__.insert(new_node, clique);
    return new_node;
  }

  INLINE NodeId CliqueGraph::addNode() { return addNode(NodeSet()); }

  /// adds a new clique to the graph

  INLINE void CliqueGraph::addNode(const NodeId id, const NodeSet& clique) {
    // create the new node in the graph
    UndiGraph::addNodeWithId(id);

    // update the set of nodes of the clique
    cliques__.insert(id, clique);
  }

  INLINE void CliqueGraph::addNode(const NodeId id) { addNode(id, NodeSet()); }

  /// removes a given clique from the clique graph

  INLINE void CliqueGraph::eraseNode(const NodeId id) {
    // check if the node belongs to the graph
    if (!exists(id)) return;

    // remove the separators
    auto nei = neighbours(id);
    for (auto iter = nei.beginSafe(); iter != nei.endSafe();
         ++iter)   // safe iterator needed here
      eraseEdge(Edge(*iter, id));

    // erase the clique set
    cliques__.erase(id);

    // erase the node and its neighbours from the graph
    UndiGraph::eraseNode(id);
  }

  /// returns the set of nodes included into a given clique

  INLINE const NodeSet& CliqueGraph::clique(const NodeId clique) const {
    return cliques__[clique];
  }

  /// returns the id of a clique containing the node the id of which is in
  /// argument

  INLINE NodeId CliqueGraph::container(const NodeId id) const {
    for (const auto& elt: cliques__)
      if (elt.second.contains(id)) return elt.first;

    GUM_ERROR(NotFound, "This node belongs to no clique")
  }

  /// function used to update the separators__ when clique__/edges are modified

  INLINE void CliqueGraph::updateSeparators__(const NodeId id1) {
    for (const auto nei: neighbours(id1))
      separators__[Edge(nei, id1)] = cliques__[id1] * cliques__[nei];
  }

  /// changes the set of nodes included into a given clique and returns the new
  /// set

  INLINE void CliqueGraph::setClique(const NodeId id, const NodeSet& new_clique) {
    // get the current clique set
    cliques__[id] = new_clique;
    updateSeparators__(id);
  }

  /// returns the separator included in a given edge

  INLINE const NodeSet& CliqueGraph::separator(const Edge& edge) const {
    return separators__[edge];
  }

  /// returns the separator included in an edge specified by its extremities

  INLINE const NodeSet& CliqueGraph::separator(const NodeId node1,
                                               const NodeId node2) const {
    return separator(Edge(node1, node2));
  }

  /// indicates whether the graph is a join tree

  INLINE bool CliqueGraph::isJoinTree() const {
    return (!hasUndirectedCycle() && hasRunningIntersection());
  }

  /// removes all the nodes from the graph (as well as their adjacent
  /// edges/arcs)

  INLINE void CliqueGraph::clear() {
    UndiGraph::clear();
    cliques__.clear();
    separators__.clear();
  }

  /// removes all the edges from the clique graph

  INLINE void CliqueGraph::clearEdges() {
    UndiGraph::clearEdges();
    separators__.clear();
  }

  /// checks whether two clique graphs are different

  INLINE bool CliqueGraph::operator!=(const CliqueGraph& from) const {
    return (!operator==(from));
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
