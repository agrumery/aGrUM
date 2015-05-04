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
 * @brief An algorithms producing a junction given the elimination tree
 * produced by the triangulation algorithm
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/config.h>
#include <agrum/graphs/defaultJunctionTreeStrategy.h>
#include <agrum/graphs/triangulations/staticTriangulation.h>

namespace gum {

  /// default constructor
  DefaultJunctionTreeStrategy::DefaultJunctionTreeStrategy()
      : __triangulation(0), __has_junction_tree(true) {
    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultJunctionTreeStrategy);
  }

  /// destructor
  DefaultJunctionTreeStrategy::~DefaultJunctionTreeStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR(DefaultJunctionTreeStrategy);
  }

  /// virtual copy constructor
  DefaultJunctionTreeStrategy *DefaultJunctionTreeStrategy::newFactory() const {
    return new DefaultJunctionTreeStrategy;
  }

  /// indicates whether the junction tree strategy needs fill-ins to work properly
  bool DefaultJunctionTreeStrategy::requiresFillIns() const { return false; }

  /// assign the triangulation to the junction tree strategy
  void DefaultJunctionTreeStrategy::setTriangulation(StaticTriangulation *tr) {
    __triangulation = tr;
    __has_junction_tree = false;
    __junction_tree.clear();
    __node_2_junction_clique.clear();
  }

  /// returns, for each node, the clique which was created by its deletion
  const NodeProperty<NodeId> &DefaultJunctionTreeStrategy::createdCliques() {
    // compute the junction tree only if it does not already exist
    if (!__has_junction_tree)
      __computeJunctionTree();

    return __node_2_junction_clique;
  }

  /** @brief returns the Id of the clique created by the
   * elimination of a given node during the triangulation process */
  NodeId DefaultJunctionTreeStrategy::createdClique(const NodeId id) {
    // compute the junction tree only if it does not already exist
    if (!__has_junction_tree)
      __computeJunctionTree();

    return __node_2_junction_clique[id];
  }

  /// returns the junction tree asked by the triangulation
  const CliqueGraph &DefaultJunctionTreeStrategy::junctionTree() {
    // compute the junction tree only if it does not already exist
    if (!__has_junction_tree)
      __computeJunctionTree();

    return __junction_tree;
  }

  /// computes a junction tree from an elimination tree
  void DefaultJunctionTreeStrategy::__computeJunctionTree() {
    // if no triangulation is assigned to the strategy, do nothing
    if (!__triangulation)
      return;

    // get the elimination tree
    const CliqueGraph &elim_tree = __triangulation->eliminationTree();

    // copy the elimination tree into the junction tree
    __junction_tree = elim_tree;

    // mark all the edges of the junction tree to false
    EdgeProperty<bool> mark = __junction_tree.edgesProperty(false);

    // create a vector indicating by which clique a given clique has been
    // substituted during the transformation from the elimination tree to the
    // junction tree. Assume that clique J of the elmination tree has been
    // substituted by clique K of the elimination tree, and that clique J
    // (resp. K) was the jth (resp. kth) one created during the triangulation
    // process. Then, in the vector below, substitution[j] = k.
    const std::vector<NodeId> &elim_order = __triangulation->eliminationOrder();
    std::vector<unsigned int> substitution(elim_order.size());

    for (unsigned int i = 0; i < substitution.size(); ++i)
      substitution[i] = i;

    // for all cliques C_i, from the last one created to the first one, check if
    // there exists a parent C_j (a neighbour created before C_i) such that
    // |C_j| = |C_i| + 1 and such that the edge is not marked. In this case, this
    // means that C_j contains C_i. Hence, we should remove C_i, and link all of
    // its neighbours to C_j. These links will be marked to true as no such
    // neighbour can be included in C_j (and conversely).
    // NodeSetIterator iter;

    for (unsigned int i = elim_order.size() - 1; i < elim_order.size(); --i) {
      NodeId C_i = i;
      unsigned int card_C_i = __junction_tree.clique(C_i).size();

      // search for C_j such that |C_j| = [C_i| + 1
      NodeId C_j = C_i;

      for (const auto C_jj : __junction_tree.neighbours(C_i))
        if ((i > C_jj) && !mark[Edge(C_i, C_jj)] &&
            (__junction_tree.clique(C_jj).size() == card_C_i + 1)) {
          // ok, here we found a parent such that |C_jj| = [C_i| + 1
          C_j = C_jj;
          __junction_tree.eraseEdge(Edge(C_i, C_j));
          break;
        }

      // if we found a C_j, link the neighbours of C_i to C_j
      if (C_j != C_i) {
        for (const auto nei : __junction_tree.neighbours(C_i)) {
          __junction_tree.addEdge(C_j, nei);
          mark.insert(Edge(C_j, nei), true);
        }

        substitution[i] = C_j;
        __junction_tree.eraseNode(C_i);
      }
    }

    // compute the transitive closure of vector substitution
    for (unsigned int i = 0; i < substitution.size(); ++i)
      substitution[i] = substitution[substitution[i]];

    // using the transitive closure of vector substitution, compute for each node
    // the clique of the junction tree that was created by its elimination during
    // the triangulation
    for (unsigned int i = 0; i < elim_order.size(); ++i) {
      __node_2_junction_clique.insert(elim_order[i], substitution[i]);
    }

    __has_junction_tree = true;
  }

  /// resets the current junction tree strategy data structures
  void DefaultJunctionTreeStrategy::clear() {
    __has_junction_tree = false;
    __junction_tree.clear();
    __node_2_junction_clique.clear();
  }

} /* namespace gum */
