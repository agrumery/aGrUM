/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Source implementation of Base classes for undirected graphs
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */
#include <agrum/tools/graphs/undiGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/undiGraph_inl.h>
#endif   // GUM_NOINLINE
#include "graphElements.h"
#include <cstdio>
#include <iostream>

namespace gum {

  UndiGraph::UndiGraph(Size nodes_size,
                       bool nodes_resize_policy,
                       Size edges_size,
                       bool edges_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy),
      EdgeGraphPart(edges_size, edges_resize_policy) {
    GUM_CONSTRUCTOR(UndiGraph);
  }

  UndiGraph::UndiGraph(const UndiGraph& g) : NodeGraphPart(g), EdgeGraphPart(g) {
    GUM_CONS_CPY(UndiGraph);
  }

  UndiGraph::~UndiGraph() {
    GUM_DESTRUCTOR(UndiGraph);
    ;
  }

  bool UndiGraph::hasUndirectedCycle() const {
    List< std::pair< NodeId, NodeId > > open_nodes;
    NodeProperty< bool >                examined_nodes = nodesProperty(false);
    std::pair< NodeId, NodeId >         thePair;
    NodeId                              current, from_current;

    for (const auto node: nodes()) {
      // check if the node has already been examined (if this is not the case,
      // this means that we are on a new connected component)
      if (!examined_nodes[node]) {
        // indicates that we are examining a new node
        examined_nodes[node] = true;

        // check recursively all the nodes of node's connected component
        thePair.first  = node;
        thePair.second = node;
        open_nodes.insert(thePair);

        while (!open_nodes.empty()) {
          // get a node to propagate
          thePair = open_nodes.front();
          open_nodes.popFront();

          current      = thePair.first;
          from_current = thePair.second;

          // check the neighbours
          for (const auto new_node: neighbours(current))

            // avoid to check the node we are coming from
            if (new_node != from_current) {
              if (examined_nodes[new_node])
                return true;
              else {
                examined_nodes[new_node] = true;
                thePair.first            = new_node;
                thePair.second           = current;
                open_nodes.insert(thePair);
              }
            }
        }
      }
    }

    return false;
  }

  std::string UndiGraph::toString() const {
    std::string s = NodeGraphPart::toString();
    s += " , ";
    s += EdgeGraphPart::toString();
    return s;
  }

  std::string UndiGraph::toDot() const {
    std::stringstream output;
    std::stringstream nodeStream;
    std::stringstream edgeStream;
    List< NodeId >    treatedNodes;
    output << "digraph \""
           << "no_name\" {" << std::endl
           << "edge [dir=none]" << std::endl;
    nodeStream << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for (const auto node: nodes()) {
      nodeStream << tab << node << ";";

      if (neighbours(node).size() > 0)
        for (const auto nei: neighbours(node))
          if (!treatedNodes.exists(nei))
            edgeStream << tab << node << " -> " << nei << ";" << std::endl;

      treatedNodes.insert(node);
    }

    output << nodeStream.str() << std::endl << edgeStream.str() << std::endl << "}" << std::endl;

    return output.str();
  }

  /// returns the partial graph formed by the nodes given in parameter
  UndiGraph UndiGraph::partialUndiGraph(NodeSet nodes) {
    UndiGraph partialGraph;

    for (const auto node: nodes) {
      partialGraph.addNodeWithId(node);

      for (const auto nei: neighbours(node))
        if (nodes.contains(nei) && partialGraph.existsNode(nei)) partialGraph.addEdge(node, nei);
    }

    return partialGraph;
  }

  NodeProperty< NodeId > UndiGraph::nodes2ConnectedComponent() const {
    NodeProperty< NodeId > res;

    NodeId numCC = 0;
    for (const auto node: nodes()) {
      if (res.exists(node)) continue;
      NodeSet nodes{node};
      while (!nodes.empty()) {
        auto actual = *(nodes.begin());
        nodes.erase(actual);
        res.insert(actual, numCC);
        for (const auto nei: neighbours(actual)) {
          if (!res.exists(nei))
            if (!nodes.exists(nei)) nodes.insert(nei);
        }
      }
      numCC += 1;
    }

    return res;
  }

  /// for friendly displaying the content of undirected graphs
  std::ostream& operator<<(std::ostream& stream, const UndiGraph& g) {
    stream << g.toString();
    return stream;
  }

} /* namespace gum */
