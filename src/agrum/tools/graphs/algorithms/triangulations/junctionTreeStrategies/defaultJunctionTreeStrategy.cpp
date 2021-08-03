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
 * @brief An algorithms producing a junction given the elimination tree
 * produced by the triangulation algorithm
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <numeric>

#include <agrum/agrum.h>
#include <agrum/tools/graphs/algorithms/triangulations/junctionTreeStrategies/defaultJunctionTreeStrategy.h>
#include <agrum/tools/graphs/algorithms/triangulations/staticTriangulation.h>

namespace gum {

  // default constructor
  DefaultJunctionTreeStrategy::DefaultJunctionTreeStrategy() {
    GUM_CONSTRUCTOR(DefaultJunctionTreeStrategy);
  }

  // copy constructor
  DefaultJunctionTreeStrategy::DefaultJunctionTreeStrategy(
     const DefaultJunctionTreeStrategy& from) :
      JunctionTreeStrategy(from),
      _has_junction_tree_(from._has_junction_tree_), _junction_tree_(from._junction_tree_),
      _node_2_junction_clique_(from._node_2_junction_clique_) {
    GUM_CONS_CPY(DefaultJunctionTreeStrategy);
  }

  // move constructor
  DefaultJunctionTreeStrategy::DefaultJunctionTreeStrategy(DefaultJunctionTreeStrategy&& from) :
      JunctionTreeStrategy(std::move(from)), _has_junction_tree_(from._has_junction_tree_),
      _junction_tree_(std::move(from._junction_tree_)),
      _node_2_junction_clique_(std::move(from._node_2_junction_clique_)) {
    GUM_CONS_MOV(DefaultJunctionTreeStrategy);
  }

  // destructor
  DefaultJunctionTreeStrategy::~DefaultJunctionTreeStrategy() {
    GUM_DESTRUCTOR(DefaultJunctionTreeStrategy);
    ;
  }

  // clone factory
  DefaultJunctionTreeStrategy* DefaultJunctionTreeStrategy::newFactory() const {
    return new DefaultJunctionTreeStrategy;
  }

  // virtual copy constructor
  DefaultJunctionTreeStrategy*
     DefaultJunctionTreeStrategy::copyFactory(StaticTriangulation* tr) const {
    if (tr == nullptr) {
      return new DefaultJunctionTreeStrategy(*this);
    } else {
      // here, we need to assign new triangulation "tr" to the new strategy

      // if there was already a triangulation associated with the current
      // strategy, 2 cases can obtain:
      // 1/ both  _triangulation_ and tr point toward the same graph to be
      //    triangulated. In this case, no need to recompute anything
      // 2/ they point toward different graphs. Then, we must indicate that
      //    the new strategy has not computed anything yet
      if ((triangulation_ != nullptr) && (tr->originalGraph() == triangulation_->originalGraph())) {
        auto new_strategy            = new DefaultJunctionTreeStrategy(*this);   // case 1/
        new_strategy->triangulation_ = tr;
        return new_strategy;
      } else {   // case 2/
        auto new_strategy = new DefaultJunctionTreeStrategy;
        new_strategy->setTriangulation(tr);
        return new_strategy;
      }
    }
  }

  // indicates whether the junction tree strategy needs fill-ins to work
  // properly
  bool DefaultJunctionTreeStrategy::requiresFillIns() const { return false; }

  // assign the triangulation to the junction tree strategy
  void DefaultJunctionTreeStrategy::setTriangulation(StaticTriangulation* tr) {
    clear();
    triangulation_ = tr;
  }

  // returns, for each node, the clique which was created by its deletion
  const NodeProperty< NodeId >& DefaultJunctionTreeStrategy::createdCliques() {
    // compute the junction tree only if it does not already exist
    if (!_has_junction_tree_) _computeJunctionTree_();

    return _node_2_junction_clique_;
  }

  // returns the Id of the clique created by the elimination of a given
  // node during the triangulation process */
  NodeId DefaultJunctionTreeStrategy::createdClique(const NodeId id) {
    // compute the junction tree only if it does not already exist
    if (!_has_junction_tree_) _computeJunctionTree_();

    return _node_2_junction_clique_[id];
  }

  // returns the junction tree asked by the triangulation
  const CliqueGraph& DefaultJunctionTreeStrategy::junctionTree() {
    // compute the junction tree only if it does not already exist
    if (!_has_junction_tree_) _computeJunctionTree_();

    return _junction_tree_;
  }

  // resets the current junction tree strategy data structures
  void DefaultJunctionTreeStrategy::clear() {
    _has_junction_tree_ = false;
    _junction_tree_.clear();
    _node_2_junction_clique_.clear();
  }

  // computes a junction tree from an elimination tree
  void DefaultJunctionTreeStrategy::_computeJunctionTree_() {
    // if no triangulation is assigned to the strategy, we cannot compute
    // the junction tree, so raise an exception
    if (triangulation_ == nullptr)
      GUM_ERROR(UndefinedElement,
                "No triangulation has been assigned to "
                "the DefaultJunctionTreeStrategy")

    // copy the elimination tree into the junction tree
    _junction_tree_ = triangulation_->eliminationTree();

    // mark all the edges of the junction tree to false
    EdgeProperty< bool > mark = _junction_tree_.edgesProperty(false);

    // create a vector indicating by which clique a given clique has been
    // substituted during the transformation from the elimination tree to the
    // junction tree. Assume that clique J of the elimination tree has been
    // substituted by clique K of the elimination tree, and that clique J
    // (resp. K) was the jth (resp. kth) one created during the triangulation
    // process. Then, in the vector below, substitution[j] = k.
    const std::vector< NodeId >& elim_order = triangulation_->eliminationOrder();

    auto                  size = elim_order.size();
    std::vector< NodeId > substitution(size);
    std::iota(substitution.begin(), substitution.end(), 0);

    // for all cliques C_i, from the last one created to the first one, check
    // if there exists a parent C_j (a neighbor created before C_i) such that
    // |C_j| = |C_i| + 1 and such that the edge is not marked. In this case,
    // this means that C_j contains C_i. Hence, we should remove C_i, and link
    // all of its neighbors to C_j. These links will be marked to true as no
    // such neighbor can be included in C_j (and conversely).
    if (size > 0) {
      for (auto i = size; i >= 1; --i) {
        const NodeId C_i             = NodeId(i - 1);
        const auto   card_C_i_plus_1 = _junction_tree_.clique(C_i).size() + 1;

        // search for C_j such that |C_j| = [C_i| + 1
        NodeId C_j = C_i;

        for (const auto C_jj: _junction_tree_.neighbours(C_i))
          if ((C_i > C_jj) && !mark[Edge(C_jj, C_i)]
              && (_junction_tree_.clique(C_jj).size() == card_C_i_plus_1)) {
            // ok, here we found a parent such that |C_jj| = [C_i| + 1
            C_j = C_jj;
            _junction_tree_.eraseEdge(Edge(C_j, C_i));
            break;
          }

        // if we found a C_j, link the neighbors of C_i to C_j
        if (C_j != C_i) {
          for (const auto nei: _junction_tree_.neighbours(C_i)) {
            _junction_tree_.addEdge(C_j, nei);
            mark.insert(Edge(C_j, nei), true);
          }

          // remove C_i
          substitution[C_i] = C_j;
          _junction_tree_.eraseNode(C_i);
        }
      }
    }

    // compute the transitive closure of vector substitution
    for (std::size_t i = 0; i < size; ++i)
      substitution[i] = substitution[substitution[i]];

    // using the transitive closure of vector substitution, compute for each
    // node the clique of the junction tree that was created by its
    // elimination during the triangulation
    for (NodeId i = NodeId(0); i < size; ++i) {
      _node_2_junction_clique_.insert(elim_order[i], substitution[i]);
    }

    _has_junction_tree_ = true;
  }

} /* namespace gum */
