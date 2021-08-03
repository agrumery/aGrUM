/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/tools/graphs/mixedGraph.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/mixedGraph_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  MixedGraph::MixedGraph(Size nodes_size,
                         bool nodes_resize_policy,
                         Size arcs_size,
                         bool arcs_resize_policy,
                         Size edges_size,
                         bool edges_resize_policy) :
      // Note that we need to initialize the NodeGraphPart by ourselves
      // because
      // it is a virtual inherited class (see C++ FAQ Lite #25.12 for details)
      NodeGraphPart(nodes_size, nodes_resize_policy),
      UndiGraph(edges_size, edges_resize_policy),
      DiGraph(arcs_size, arcs_resize_policy) {   // for debugging purposes
    GUM_CONSTRUCTOR(MixedGraph);
  }

  MixedGraph::MixedGraph(const UndiGraph& g) : NodeGraphPart(g), UndiGraph(g), DiGraph() {
    GUM_CONSTRUCTOR(MixedGraph);
  }

  MixedGraph::MixedGraph(const DiGraph& g) : NodeGraphPart(g), UndiGraph(), DiGraph(g) {
    GUM_CONSTRUCTOR(MixedGraph);
  }

  MixedGraph::MixedGraph(const MixedGraph& g) :
      NodeGraphPart(g), UndiGraph(g), DiGraph(g) {   // for debugging purposes
    GUM_CONS_CPY(MixedGraph);
  }

  MixedGraph::~MixedGraph() {   // for debugging purposes
    GUM_DESTRUCTOR(MixedGraph);
  }

  std::string MixedGraph::toString() const {
    std::string s = NodeGraphPart::toString();
    s += " , ";
    s += ArcGraphPart::toString();
    s += " , ";
    s += EdgeGraphPart::toString();
    return s;
  }

  std::vector< NodeId > MixedGraph::mixedOrientedPath(NodeId n1, NodeId n2) const {
    std::vector< NodeId > v;
    // not recursive version => use a FIFO for simulating the recursion
    List< NodeId > node_fifo;
    node_fifo.pushBack(n2);

    // mark[node] = successor if visited, else mark[node] does not exist
    NodeProperty< NodeId > mark;
    mark.insert(n2, n2);

    NodeId current;
    while (!node_fifo.empty()) {
      current = node_fifo.front();
      node_fifo.popFront();

      // check the neighbours
      for (const auto new_one: neighbours(current)) {
        if (mark.exists(new_one))   // if the node has already been visited
          continue;                 // do not check it again

        mark.insert(new_one, current);

        if (new_one == n1) {
          for (current = n1; current != n2; current = mark[current])
            v.push_back(current);
          v.push_back(n2);
          return v;
        }

        node_fifo.pushBack(new_one);
      }

      // check the parents
      for (const auto new_one: parents(current)) {
        if (mark.exists(new_one))   // if this node is already marked, do not
          continue;                 // check it again

        mark.insert(new_one, current);

        if (new_one == n1) {
          for (current = n1; current != n2; current = mark[current])
            v.push_back(current);
          v.push_back(n2);
          return v;
        }

        node_fifo.pushBack(new_one);
      }
    }

    return v;
  }

  std::vector< NodeId > MixedGraph::mixedUnorientedPath(NodeId n1, NodeId n2) const {
    std::vector< NodeId > v;
    // not recursive version => use a FIFO for simulating the recursion
    List< NodeId > node_fifo;
    node_fifo.pushBack(n2);

    // mark[node] = successor if visited, else mark[node] does not exist
    NodeProperty< NodeId > mark;
    mark.insert(n2, n2);

    NodeId current;

    while (!node_fifo.empty()) {
      current = node_fifo.front();
      node_fifo.popFront();

      // check the neighbours
      for (const auto new_one: neighbours(current)) {
        if (mark.exists(new_one))   // if the node has already been visited
          continue;                 // do not check it again

        mark.insert(new_one, current);

        if (new_one == n1) {
          for (current = n1; current != n2; current = mark[current])
            v.push_back(current);
          v.push_back(n2);
          return v;
        }

        node_fifo.pushBack(new_one);
      }

      // check the parents
      for (const auto new_one: parents(current)) {
        if (mark.exists(new_one))   // the node has already been visited
          continue;

        mark.insert(new_one, current);
        if (new_one == n1) {
          for (current = n1; current != n2; current = mark[current])
            v.push_back(current);
          v.push_back(n2);
          return v;
        }
        node_fifo.pushBack(new_one);
      }

      // check the children
      for (const auto new_one: children(current)) {
        if (mark.exists(new_one))   // the node has already been visited
          continue;

        mark.insert(new_one, current);

        if (new_one == n1) {
          for (current = n1; current != n2; current = mark[current])
            v.push_back(current);
          v.push_back(n2);
          return v;
        }

        node_fifo.pushBack(new_one);
      }
    }

    return v;
  }

  std::string MixedGraph::toDot() const {
    std::stringstream output;
    std::stringstream nodeStream;
    std::stringstream edgeStream;
    List< NodeId >    treatedNodes;
    output << "digraph \""
           << "no_name\" {" << std::endl;
    nodeStream << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for (const auto node: nodes()) {
      nodeStream << tab << node << ";";

      for (const auto nei: neighbours(node))
        if (!treatedNodes.exists(nei))
          edgeStream << tab << node << " -> " << nei << " [dir=none];" << std::endl;

      for (const auto chi: children(node))
        edgeStream << tab << node << " -> " << chi << ";" << std::endl;

      treatedNodes.insert(node);
    }

    output << nodeStream.str() << std::endl << edgeStream.str() << std::endl << "}" << std::endl;

    return output.str();
  }

  NodeSet MixedGraph::adjacents(const NodeId id) const {
    return neighbours(id) + parents(id) + children(id);
  }

  /// for friendly displaying the content of directed graphs
  std::ostream& operator<<(std::ostream& stream, const MixedGraph& g) {
    stream << g.toString();
    return stream;
  }


} /* namespace gum */
