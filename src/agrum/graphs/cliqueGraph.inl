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
 * @brief inline source of basic clique graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parser in IDE
#include <agrum/graphs/cliqueGraph.h>

namespace gum {

  /// copy operator
  INLINE CliqueGraph &CliqueGraph::operator=(const CliqueGraph &g) {
    if (this != &g) {
      UndiGraph::operator=(g);
      __cliques = g.__cliques;
      __separators = g.__separators;
    }

    return *this;
  }

  INLINE void CliqueGraph::insertEdge(const NodeId first, const NodeId second) {
    addEdge(first, second);
  }
  INLINE void CliqueGraph::addEdge(const NodeId first, const NodeId second) {
    Edge edge(first, second);

    if (!existsEdge(edge)) {
      // create the edge in the graph
      UndiGraph::addEdge(first, second);

      // create the separator
      __separators.insert(edge, __cliques[first] * __cliques[second]);
    }
  }

  /// removes an edge (and its separator) from the clique graph

  INLINE void CliqueGraph::eraseEdge(const Edge &edge) {
    if (existsEdge(edge)) {
      __separators.erase(edge);
      UndiGraph::eraseEdge(edge);
    }
  }

  /// adds a new clique to the graph

  INLINE NodeId CliqueGraph::insertNode(const NodeSet &clique) {
    return addNode(clique);
  }
  INLINE NodeId CliqueGraph::addNode(const NodeSet &clique) {
    // create the new node in the graph
    NodeId new_node = UndiGraph::addNode();

    // update the set of nodes of the clique
    __cliques.insert(new_node, clique);
    return new_node;
  }

  INLINE NodeId CliqueGraph::insertNode() { return addNode(); }
  INLINE NodeId CliqueGraph::addNode() { return addNode(NodeSet()); }

  /// adds a new clique to the graph

  INLINE void CliqueGraph::insertNode(const NodeId id, const NodeSet &clique) {
    addNode(id, clique);
  }
  INLINE void CliqueGraph::addNode(const NodeId id, const NodeSet &clique) {
    // create the new node in the graph
    UndiGraph::addNode(id);

    // update the set of nodes of the clique
    __cliques.insert(id, clique);
  }

  INLINE void CliqueGraph::insertNode(const NodeId id) { addNode(id); }
  INLINE void CliqueGraph::addNode(const NodeId id) { addNode(id, NodeSet()); }

  /// removes a given clique from the clique graph

  INLINE void CliqueGraph::eraseNode(const NodeId id) {
    // check if the node belongs to the graph
    if (!exists(id))
      return;

    // remove the separators
    auto nei = neighbours(id);
    for (auto iter = nei.beginSafe(); iter != nei.endSafe();
         ++iter) // safe iterator needed here
      eraseEdge(Edge(*iter, id));

    // erase the clique set
    __cliques.erase(id);

    // erase the node and its neighbours from the graph
    UndiGraph::eraseNode(id);
  }

  /// returns the set of nodes included into a given clique

  INLINE const NodeSet &CliqueGraph::clique(const NodeId clique) const {
    return __cliques[clique];
  }

  /// returns the id of a clique containing the node the id of which is in argument

  INLINE NodeId CliqueGraph::container(const NodeId id) const {
    for (const auto &elt : __cliques)
      if (elt.second.contains(id))
        return elt.first;

    GUM_ERROR(NotFound, "This node belongs to no clique");
  }

  /// function used to update the __separators when __clique/edges are modified

  INLINE void CliqueGraph::__updateSeparators(const NodeId id1) {
    for (const auto nei : neighbours(id1))
      __separators[Edge(nei, id1)] = __cliques[id1] * __cliques[nei];
  }

  /// changes the set of nodes included into a given clique and returns the new set

  INLINE void CliqueGraph::setClique(const NodeId id, const NodeSet &new_clique) {
    // get the current clique set
    __cliques[id] = new_clique;
    __updateSeparators(id);
  }

  /// returns the separator included in a given edge

  INLINE const NodeSet &CliqueGraph::separator(const Edge &edge) const {
    return __separators[edge];
  }

  /// returns the separator included in an edge specified by its extremities

  INLINE const NodeSet &CliqueGraph::separator(const NodeId node1,
                                               const NodeId node2) const {
    return separator(Edge(node1, node2));
  }

  /// indicates whether the graph is a join tree

  INLINE bool CliqueGraph::isJoinTree() const {
    return (!hasUndirectedCycle() && hasRunningIntersection());
  }

  /// removes all the nodes from the graph (as well as their adjacent edges/arcs)

  INLINE void CliqueGraph::clear() {
    UndiGraph::clear();
    __cliques.clear();
    __separators.clear();
  }

  /// removes all the edges from the clique graph

  INLINE void CliqueGraph::clearEdges() {
    UndiGraph::clearEdges();
    __separators.clear();
  }

  /// checks whether two clique graphs are different

  INLINE bool CliqueGraph::operator!=(const CliqueGraph &from) const {
    return (!operator==(from));
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
