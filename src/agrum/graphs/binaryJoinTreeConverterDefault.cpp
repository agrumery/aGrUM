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
 * @brief An algorithm for converting a join tree into a binary join tree
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/config.h>

#include <agrum/core/priorityQueue.h>
#include <agrum/graphs/binaryJoinTreeConverterDefault.h>

namespace gum {

  /// default constructor
  BinaryJoinTreeConverterDefault::BinaryJoinTreeConverterDefault() {
    // for debugging purposes
    GUM_CONSTRUCTOR(BinaryJoinTreeConverterDefault);
  }

  /// destructor
  BinaryJoinTreeConverterDefault::~BinaryJoinTreeConverterDefault() {
    GUM_DESTRUCTOR(BinaryJoinTreeConverterDefault);
  }

  /** @brief a function used to mark the nodes belonging to a given
   * connected component */
  void BinaryJoinTreeConverterDefault::__markConnectedComponent(
      const CliqueGraph &JT, NodeId root, NodeProperty<bool> &mark) const {
    // we mark the nodes in a depth first search manner. To avoid a recursive
    // algorithm, use a vector to simulate a stack of nodes to inspect.
    // stack => depth first search
    std::vector<NodeId> nodes_to_inspect;
    nodes_to_inspect.reserve(JT.sizeNodes());

    // the idea to populate the marks is to use the stack: root is
    // put onto the stack. Then, while the stack is not empty, remove
    // the top of the stack and mark it and put into the stack its
    // adjacent nodes.
    nodes_to_inspect.push_back(root);

    while (!nodes_to_inspect.empty()) {
      // process the top of the stack
      NodeId current_node = nodes_to_inspect.back();
      nodes_to_inspect.pop_back();

      // only process the node if it has not been processed yet (actually,
      // this should not occur unless the clique graph is not singly connected)

      if (!mark[current_node]) {
        mark[current_node] = true;

        // put the neighbors onto the stack
        for (const auto neigh : JT.neighbours(current_node))
          if (!mark[neigh])
            nodes_to_inspect.push_back(neigh);
      }
    }
  }

  /// returns the domain size of the union of two cliques
  float BinaryJoinTreeConverterDefault::__combinedSize(
      const NodeSet &nodes1, const NodeSet &nodes2,
      const NodeProperty<Size> &domain_sizes) const {
    float result = 1;

    for (const auto node : nodes1)
      result *= domain_sizes[node];

    for (const auto node : nodes2)
      if (!nodes1.exists(node))
        result *= domain_sizes[node];

    return result;
  }

  /// returns all the roots considered for all the connected components
  const NodeSet &BinaryJoinTreeConverterDefault::roots() const { return __roots; }

  /// convert a clique and its adjacent cliques into a binary join tree
  void BinaryJoinTreeConverterDefault::__convertClique(
      CliqueGraph &JT, NodeId clique, NodeId from,
      const NodeProperty<Size> &domain_sizes) const {
    // get the neighbors of clique. If there are fewer than 3 neighbors,
    // there is nothing to do
    const NodeSet &neighbors = JT.neighbours(clique);

    if (neighbors.size() <= 2)
      return;

    if ((neighbors.size() == 3) && (clique != from))
      return;

    // here we need to transform the neighbors into a binary tree
    // create a vector with all the ids of the cliques to combine
    std::vector<NodeId> cliques;
    cliques.reserve(neighbors.size());

    for (const auto nei : neighbors)
      if (nei != from)
        cliques.push_back(nei);

    // create a vector indicating wether the elements in cliques contain
    // relevant information or not (during the execution of the for
    // loop below, a cell of vector cliques may actually contain only
    // trash data)
    std::vector<bool> is_cliques_relevant(cliques.size(), true);

    // for each pair of cliques (i,j), compute the size of the clique that would
    // result from the combination of clique i with clique j and store the result
    // into a priorityQueue
    std::pair<unsigned int, unsigned int> pair;

    PriorityQueue<std::pair<unsigned int, unsigned int>, float> queue;

    for (unsigned int i = 0; i < cliques.size(); ++i) {
      pair.first = i;
      const NodeSet &nodes1 = JT.separator(cliques[i], clique);

      for (unsigned int j = i + 1; j < cliques.size(); ++j) {
        pair.second = j;
        queue.insert(pair, __combinedSize(nodes1, JT.separator(cliques[j], clique),
                                          domain_sizes));
      }
    }

    // now parse the priority queue: the top element (i,j) gives the combination
    // to perform. When the result R has been computed, substitute i by R, remove
    // table j and recompute all the priorities of all the pairs (R,k) still
    // available.
    for (unsigned int k = 2; k < cliques.size(); ++k) {
      // get the combination to perform and do it
      pair = queue.pop();
      unsigned int ti = pair.first;
      unsigned int tj = pair.second;

      // create a new clique that will become adjacent to ti and tj
      // and remove the edges between ti, tj and clique
      const NodeSet &nodes1 = JT.separator(cliques[ti], clique);
      const NodeSet &nodes2 = JT.separator(cliques[tj], clique);
      NodeId new_node = JT.addNode(nodes1 + nodes2);
      JT.addEdge(cliques[ti], new_node);
      JT.addEdge(cliques[tj], new_node);
      JT.addEdge(clique, new_node);
      JT.eraseEdge(Edge(cliques[ti], clique));
      JT.eraseEdge(Edge(cliques[tj], clique));

      // substitute cliques[pair.first] by the result
      cliques[ti] = new_node;
      is_cliques_relevant[tj] = false; // now tj is no more a neighbor of clique

      // remove all the pairs involving tj in the priority queue

      for (unsigned int ind = 0; ind < tj; ++ind) {
        if (is_cliques_relevant[ind]) {
          pair.first = ind;
          queue.erase(pair);
        }
      }

      pair.first = tj;

      for (unsigned int ind = tj + 1; ind < cliques.size(); ++ind) {
        if (is_cliques_relevant[ind]) {
          pair.second = ind;
          queue.erase(pair);
        }
      }

      // update the "combined" size of all the pairs involving "new_node"
      {
        const NodeSet &nodes1 = JT.separator(cliques[ti], clique);
        pair.second = ti;
        float newsize;

        for (unsigned int ind = 0; ind < ti; ++ind) {
          if (is_cliques_relevant[ind]) {
            pair.first = ind;
            newsize = __combinedSize(nodes1, JT.separator(cliques[ind], clique),
                                     domain_sizes);
            queue.setPriority(pair, newsize);
          }
        }

        pair.first = ti;

        for (unsigned int ind = ti + 1; ind < cliques.size(); ++ind) {
          if (is_cliques_relevant[ind]) {
            pair.second = ind;
            newsize = __combinedSize(nodes1, JT.separator(cliques[ind], clique),
                                     domain_sizes);
            queue.setPriority(pair, newsize);
          }
        }
      }
    }
  }

  /// convert a whole connected component into a binary join tree
  void BinaryJoinTreeConverterDefault::__convertConnectedComponent(
      CliqueGraph &JT, NodeId current_node, NodeId from,
      const NodeProperty<Size> &domain_sizes, NodeProperty<bool> &mark) const {
    // first, indicate that the node has been marked (this avoids looping
    // if JT is not a tree
    mark[current_node] = true;

    // parse all the neighbors except nodes already converted and convert them
    for (const auto neigh : JT.neighbours(current_node)) {
      if (!mark[neigh]) {
        __convertConnectedComponent(JT, neigh, current_node, domain_sizes, mark);
      }
    }

    // convert the current node
    __convertClique(JT, current_node, from, domain_sizes);
  }

  /// computes the binary join tree
  CliqueGraph
  BinaryJoinTreeConverterDefault::convert(const CliqueGraph &JT,
                                          const NodeProperty<Size> &domain_sizes,
                                          const NodeSet &specified_roots) {
    // first, we copy the current clique graph. By default, this is what we
    // will return
    CliqueGraph binJT = JT;

    // check that there is no connected component without a root. In such a case,
    // assign an arbitrary root to it
    __roots = specified_roots;

    NodeProperty<bool> mark = JT.nodesProperty(false, JT.sizeNodes());

    // for each specified root, populate its connected component
    for (const auto root : specified_roots) {
      // check that the root has not already been marked
      // in this case, this means that more than one root has been specified
      // for a given connected component
      if (mark[root])
        GUM_ERROR(
            InvalidNode,
            "several roots have been specified for a given connected component");

      __markConnectedComponent(JT, root, mark);
    }

    // check that all nodes have been marked. If this is not the case, then
    // this means that we need to add new roots
    for (const auto &elt : mark)
      if (!elt.second) {
        __roots << elt.first;
        __markConnectedComponent(JT, elt.first, mark);
      }

    // here, we know that each connected component has one and only one root.
    // Now we can apply a recursive collect algorithm starting from root
    // that transforms each clique with more than 3 neighbors into a set of
    // cliques having at most 3 neighbors.
    NodeProperty<bool> mark2 = JT.nodesProperty(false, JT.sizeNodes());

    for (const auto root : __roots)
      __convertConnectedComponent(binJT, root, root, domain_sizes, mark2);

    // binJT is now a binary join tree, so we can return it
    return binJT;
  }

} /* namespace gum */
