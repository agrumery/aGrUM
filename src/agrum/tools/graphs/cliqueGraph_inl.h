/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parser in IDE
#  include <agrum/tools/graphs/cliqueGraph.h>

namespace gum {

  /// copy operator
  INLINE CliqueGraph& CliqueGraph::operator=(const CliqueGraph& g) {
    if (this != &g) {
      UndiGraph::operator=(g);
      _cliques_    = g._cliques_;
      _separators_ = g._separators_;
    }

    return *this;
  }

  INLINE void CliqueGraph::addEdge(NodeId first, NodeId second) {
    Edge edge(first, second);

    if (!existsEdge(edge)) {
      // create the edge in the graph
      UndiGraph::addEdge(first, second);

      // create the separator
      _separators_.insert(edge, _cliques_[first] * _cliques_[second]);
    }
  }

  /// removes an edge (and its separator) from the clique graph

  INLINE void CliqueGraph::eraseEdge(const Edge& edge) {
    if (existsEdge(edge)) {
      _separators_.erase(edge);
      UndiGraph::eraseEdge(edge);
    }
  }

  /// adds a new clique to the graph

  INLINE NodeId CliqueGraph::addNode(const NodeSet& clique) {
    // create the new node in the graph
    NodeId new_node = UndiGraph::addNode();

    // update the set of nodes of the clique
    _cliques_.insert(new_node, clique);
    return new_node;
  }

  INLINE NodeId CliqueGraph::addNode() { return addNode(NodeSet()); }

  /// adds a new clique to the graph

  INLINE void CliqueGraph::addNodeWithId(const NodeId id, const NodeSet& clique) {
    // create the new node in the graph
    UndiGraph::addNodeWithId(id);

    // update the set of nodes of the clique
    _cliques_.insert(id, clique);
  }

  INLINE void CliqueGraph::addNodeWithId(const NodeId id) { addNodeWithId(id, NodeSet()); }

  /// removes a given clique from the clique graph

  INLINE void CliqueGraph::eraseNode(const NodeId id) {
    // check if the node belongs to the graph
    if (!exists(id)) return;

    // remove the separators
    auto nei = neighbours(id);
    for (auto iter = nei.beginSafe(); iter != nei.endSafe(); ++iter)   // safe iterator needed here
      eraseEdge(Edge(*iter, id));

    // erase the clique set
    _cliques_.erase(id);

    // erase the node and its neighbours from the graph
    UndiGraph::eraseNode(id);
  }

  /// returns the set of nodes included into a given clique

  INLINE const NodeSet& CliqueGraph::clique(const NodeId clique) const { return _cliques_[clique]; }

  /// returns the id of a clique containing the node the id of which is in
  /// argument

  INLINE NodeId CliqueGraph::container(const NodeId id) const {
    for (const auto& elt: _cliques_)
      if (elt.second.contains(id)) return elt.first;

    GUM_ERROR(NotFound, "This node belongs to no clique")
  }

  /// function used to update the  _separators_ when  _clique_/edges are modified

  INLINE void CliqueGraph::_updateSeparators_(const NodeId id1) {
    for (const auto nei: neighbours(id1))
      _separators_[Edge(nei, id1)] = _cliques_[id1] * _cliques_[nei];
  }

  /// changes the set of nodes included into a given clique and returns the new
  /// set

  INLINE void CliqueGraph::setClique(const NodeId id, const NodeSet& new_clique) {
    // get the current clique set
    _cliques_[id] = new_clique;
    _updateSeparators_(id);
  }

  /// returns the separator included in a given edge

  INLINE const NodeSet& CliqueGraph::separator(const Edge& edge) const {
    return _separators_[edge];
  }

  /// returns the separator included in an edge specified by its extremities

  INLINE const NodeSet& CliqueGraph::separator(const NodeId node1, const NodeId node2) const {
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
    _cliques_.clear();
    _separators_.clear();
  }

  /// removes all the edges from the clique graph

  INLINE void CliqueGraph::clearEdges() {
    UndiGraph::clearEdges();
    _separators_.clear();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
