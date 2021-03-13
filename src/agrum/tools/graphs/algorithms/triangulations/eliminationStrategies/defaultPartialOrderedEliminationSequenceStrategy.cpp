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
 * @brief An efficient unconstrained elimination sequence algorithm
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/agrum.h>
#include <agrum/tools/core/math/math_utils.h>

#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/defaultPartialOrderedEliminationSequenceStrategy.h>
#include <agrum/tools/graphs/undiGraph.h>

namespace gum {

  /// default constructor (uses an empty graph)
  DefaultPartialOrderedEliminationSequenceStrategy::
     DefaultPartialOrderedEliminationSequenceStrategy(double theRatio,
                                                      double theThreshold) :
      simplicial_ratio__(theRatio),
      simplicial_threshold__(theThreshold) {
    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultPartialOrderedEliminationSequenceStrategy);
  }

  /// constructor for an a priori non empty graph
  DefaultPartialOrderedEliminationSequenceStrategy::
     DefaultPartialOrderedEliminationSequenceStrategy(
        UndiGraph*                  graph,
        const NodeProperty< Size >* dom_sizes,
        const List< NodeSet >*      subsets,
        double                      ratio,
        double                      threshold) :
      simplicial_ratio__(ratio),
      simplicial_threshold__(threshold) {
    setGraph(graph, dom_sizes);
    setPartialOrder(subsets);

    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultPartialOrderedEliminationSequenceStrategy);
  }

  /// copy constructor
  DefaultPartialOrderedEliminationSequenceStrategy::
     DefaultPartialOrderedEliminationSequenceStrategy(
        const DefaultPartialOrderedEliminationSequenceStrategy& from) :
      PartialOrderedEliminationSequenceStrategy(from),
      // no need to set log_weights__ because the copy of the simplicial set
      // will set it properly
      simplicial_set__(new SimplicialSet(*from.simplicial_set__,
                                         graph_,
                                         &log_domain_sizes_,
                                         &log_weights__,
                                         false)),
      simplicial_ratio__(from.simplicial_ratio__),
      simplicial_threshold__(from.simplicial_threshold__),
      provide_fill_ins__(from.provide_fill_ins__) {
    // for debugging purposes
    GUM_CONS_CPY(DefaultPartialOrderedEliminationSequenceStrategy);
  }

  /// move constructor
  DefaultPartialOrderedEliminationSequenceStrategy::
     DefaultPartialOrderedEliminationSequenceStrategy(
        DefaultPartialOrderedEliminationSequenceStrategy&& from) :
      PartialOrderedEliminationSequenceStrategy(std::move(from)),
      log_weights__(std::move(from.log_weights__)),
      simplicial_set__(from.simplicial_set__),
      simplicial_ratio__(from.simplicial_ratio__),
      simplicial_threshold__(from.simplicial_threshold__),
      provide_fill_ins__(from.provide_fill_ins__) {
    simplicial_set__->replaceLogWeights(&from.log_weights__, &log_weights__);
    from.simplicial_set__ = nullptr;

    // for debugging purposes
    GUM_CONS_MOV(DefaultPartialOrderedEliminationSequenceStrategy);
  }

  /// destructor
  DefaultPartialOrderedEliminationSequenceStrategy::
     ~DefaultPartialOrderedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR(DefaultPartialOrderedEliminationSequenceStrategy);

    if (simplicial_set__) delete simplicial_set__;
  }

  /// create a new simplicial set suited for the current graph
  void DefaultPartialOrderedEliminationSequenceStrategy::createSimplicialSet__() {
    // remove the old simplicial set, if any
    if (simplicial_set__ != nullptr) {
      delete simplicial_set__;
      simplicial_set__ = nullptr;
    }

    if (graph_ != nullptr) {
      // create a simplicial set suited for the graph
      simplicial_set__ = new SimplicialSet(graph_,
                                           &log_domain_sizes_,
                                           &log_weights__,
                                           simplicial_ratio__,
                                           simplicial_threshold__);

      simplicial_set__->setFillIns(provide_fill_ins__);
    }
  }

  /// sets a new graph to be triangulated
  bool DefaultPartialOrderedEliminationSequenceStrategy::setGraph(
     UndiGraph* graph, const NodeProperty< Size >* domain_sizes) {
    if (PartialOrderedEliminationSequenceStrategy::setGraph(graph, domain_sizes)) {
      createSimplicialSet__();
      return true;
    }

    return false;
  }

  /// clears the sequence (to prepare, for instance, a new elimination sequence)
  void DefaultPartialOrderedEliminationSequenceStrategy::clear() {
    PartialOrderedEliminationSequenceStrategy::clear();
    log_weights__.clear();

    if (simplicial_set__ != nullptr) {
      delete simplicial_set__;
      simplicial_set__ = nullptr;
    }
  }

  /// returns the best possible node to be eliminated
  NodeId DefaultPartialOrderedEliminationSequenceStrategy::nodeToEliminate__(
     const PriorityQueue< NodeId, double >& possibleNodes) {
    bool   found = false;
    double min_score = 0;
    NodeId best_node = 0;

    for (const auto node: nodeset_) {
      try {
        double score = possibleNodes.priority(node);

        if (!found || (score < min_score)) {
          found = true;
          min_score = score;
          best_node = node;
        }
      } catch (NotFound&) {}
    }

    if (!found) { GUM_ERROR(NotFound, "no possible node to eliminate"); }

    return best_node;
  }

  /// returns the new node to be eliminated within the triangulation algorithm
  NodeId DefaultPartialOrderedEliminationSequenceStrategy::nextNodeToEliminate() {
    // if there is no simplicial set, send an exception
    if (graph_ == nullptr) GUM_ERROR(NotFound, "the graph is empty");

    if (partial_order_needed_)
      GUM_ERROR(NotFound,
                "the partial order does not cover all the nodes "
                "of the graph");

    if (nodeset_.empty()) { GUM_ERROR(NotFound, "no node is admissible"); }

    // select a node to be eliminated: try simplicial nodes, then almost
    // simplicial nodes, then quasi-simplicial nodes
    // note that if graph_ != nullptr, simplicial_set__ has been allocated
    try {
      return nodeToEliminate__(simplicial_set__->allSimplicialNodes());
    } catch (NotFound&) {}

    try {
      return nodeToEliminate__(simplicial_set__->allAlmostSimplicialNodes());
    } catch (NotFound&) {}

    try {
      return nodeToEliminate__(simplicial_set__->allQuasiSimplicialNodes());
    } catch (NotFound&) {}

    // here: select the node through Kjaerulff's heuristic
    auto   iter = nodeset_.cbegin();
    double min_score = log_weights__[*iter];
    NodeId best_node = *iter;

    for (++iter; iter != nodeset_.cend(); ++iter) {
      double score = log_weights__[*iter];

      if (score < min_score) {
        min_score = score;
        best_node = *iter;
      }
    }

    return best_node;
  }

  /** @brief if the elimination sequence is able to compute fill-ins, we
   * indicatewhether we want this feature to be activated */
  void DefaultPartialOrderedEliminationSequenceStrategy::askFillIns(bool do_it) {
    provide_fill_ins__ = do_it;

    if (simplicial_set__) simplicial_set__->setFillIns(provide_fill_ins__);
  }

  /** @brief indicates whether the new fill-ins generated by a new eliminated
   * node, if needed, will be computed by the elimination sequence, or need be
   * computed by the triangulation itself. */
  bool DefaultPartialOrderedEliminationSequenceStrategy::providesFillIns() const {
    return provide_fill_ins__;
  }

  /** @brief indicates whether the elimination sequence updates by itself the
   * graph after a node has been eliminated */
  bool DefaultPartialOrderedEliminationSequenceStrategy::providesGraphUpdate()
     const {
    return true;
  }

  /// performs all the graph/fill-ins updates provided
  void DefaultPartialOrderedEliminationSequenceStrategy::eliminationUpdate(
     const NodeId id) {
    // check whether we can do something
    if (simplicial_set__ != nullptr) {
      simplicial_set__->makeClique(id);
      simplicial_set__->eraseClique(id);

      if (!partial_order_needed_) {
        // remove the node from nodeset_
        nodeset_.erase(id);

        if (nodeset_.empty()) {
          // go to the next non-empty subset
          for (++subset_iter_; subset_iter_ != subsets_->cend(); ++subset_iter_) {
            for (const auto node: *subset_iter_) {
              if (graph_->existsNode(node)) { nodeset_.insert(node); }
            }
            if (!nodeset_.empty()) break;
          }
        }
      }
    }
  }

  /** @brief in case fill-ins are provided, this function returns the fill-ins
   * generated after the last node elimination */
  const EdgeSet& DefaultPartialOrderedEliminationSequenceStrategy::fillIns() {
    if (!provide_fill_ins__ || (simplicial_set__ == nullptr))
      return EliminationSequenceStrategy::fillIns();
    else
      return simplicial_set__->fillIns();
  }

  /** @brief creates a new elimination sequence of the same type as the current
   * object, but this sequence contains only an empty graph */
  DefaultPartialOrderedEliminationSequenceStrategy*
     DefaultPartialOrderedEliminationSequenceStrategy::newFactory() const {
    return new DefaultPartialOrderedEliminationSequenceStrategy(
       simplicial_ratio__, simplicial_threshold__);
  }

  /// virtual copy constructor
  DefaultPartialOrderedEliminationSequenceStrategy*
     DefaultPartialOrderedEliminationSequenceStrategy::copyFactory() const {
    return new DefaultPartialOrderedEliminationSequenceStrategy(*this);
  }

} /* namespace gum */
