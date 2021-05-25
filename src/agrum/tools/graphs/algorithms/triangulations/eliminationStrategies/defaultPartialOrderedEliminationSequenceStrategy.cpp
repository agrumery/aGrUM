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
       _simplicial_ratio_(theRatio),
       _simplicial_threshold_(theThreshold) {
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
       _simplicial_ratio_(ratio),
       _simplicial_threshold_(threshold) {
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
      // no need to set  _log_weights_ because the copy of the simplicial set
      // will set it properly
       _simplicial_set_(new SimplicialSet(*from. _simplicial_set_,
                                         graph_,
                                         &log_domain_sizes_,
                                         & _log_weights_,
                                         false)),
       _simplicial_ratio_(from. _simplicial_ratio_),
       _simplicial_threshold_(from. _simplicial_threshold_),
       _provide_fill_ins_(from. _provide_fill_ins_) {
    // for debugging purposes
    GUM_CONS_CPY(DefaultPartialOrderedEliminationSequenceStrategy);
  }

  /// move constructor
  DefaultPartialOrderedEliminationSequenceStrategy::
     DefaultPartialOrderedEliminationSequenceStrategy(
        DefaultPartialOrderedEliminationSequenceStrategy&& from) :
      PartialOrderedEliminationSequenceStrategy(std::move(from)),
       _log_weights_(std::move(from. _log_weights_)),
       _simplicial_set_(from. _simplicial_set_),
       _simplicial_ratio_(from. _simplicial_ratio_),
       _simplicial_threshold_(from. _simplicial_threshold_),
       _provide_fill_ins_(from. _provide_fill_ins_) {
     _simplicial_set_->replaceLogWeights(&from. _log_weights_, & _log_weights_);
    from. _simplicial_set_ = nullptr;

    // for debugging purposes
    GUM_CONS_MOV(DefaultPartialOrderedEliminationSequenceStrategy);
  }

  /// destructor
  DefaultPartialOrderedEliminationSequenceStrategy::
     ~DefaultPartialOrderedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR(DefaultPartialOrderedEliminationSequenceStrategy);

    if ( _simplicial_set_) delete  _simplicial_set_;
  }

  /// create a new simplicial set suited for the current graph
  void DefaultPartialOrderedEliminationSequenceStrategy:: _createSimplicialSet_() {
    // remove the old simplicial set, if any
    if ( _simplicial_set_ != nullptr) {
      delete  _simplicial_set_;
       _simplicial_set_ = nullptr;
    }

    if (graph_ != nullptr) {
      // create a simplicial set suited for the graph
       _simplicial_set_ = new SimplicialSet(graph_,
                                           &log_domain_sizes_,
                                           & _log_weights_,
                                            _simplicial_ratio_,
                                            _simplicial_threshold_);

       _simplicial_set_->setFillIns( _provide_fill_ins_);
    }
  }

  /// sets a new graph to be triangulated
  bool DefaultPartialOrderedEliminationSequenceStrategy::setGraph(
     UndiGraph*                  graph,
     const NodeProperty< Size >* domain_sizes) {
    if (PartialOrderedEliminationSequenceStrategy::setGraph(graph, domain_sizes)) {
       _createSimplicialSet_();
      return true;
    }

    return false;
  }

  /// clears the sequence (to prepare, for instance, a new elimination sequence)
  void DefaultPartialOrderedEliminationSequenceStrategy::clear() {
    PartialOrderedEliminationSequenceStrategy::clear();
     _log_weights_.clear();

    if ( _simplicial_set_ != nullptr) {
      delete  _simplicial_set_;
       _simplicial_set_ = nullptr;
    }
  }

  /// returns the best possible node to be eliminated
  NodeId DefaultPartialOrderedEliminationSequenceStrategy:: _nodeToEliminate_(
     const PriorityQueue< NodeId, double >& possibleNodes) {
    bool   found     = false;
    double min_score = 0;
    NodeId best_node = 0;

    for (const auto node: nodeset_) {
      try {
        double score = possibleNodes.priority(node);

        if (!found || (score < min_score)) {
          found     = true;
          min_score = score;
          best_node = node;
        }
      } catch (NotFound&) {}
    }

    if (!found) { GUM_ERROR(NotFound, "no possible node to eliminate") }

    return best_node;
  }

  /// returns the new node to be eliminated within the triangulation algorithm
  NodeId DefaultPartialOrderedEliminationSequenceStrategy::nextNodeToEliminate() {
    // if there is no simplicial set, send an exception
    if (graph_ == nullptr) GUM_ERROR(NotFound, "the graph is empty")

    if (partial_order_needed_)
      GUM_ERROR(NotFound,
                "the partial order does not cover all the nodes "
                "of the graph");

    if (nodeset_.empty()) { GUM_ERROR(NotFound, "no node is admissible") }

    // select a node to be eliminated: try simplicial nodes, then almost
    // simplicial nodes, then quasi-simplicial nodes
    // note that if graph_ != nullptr,  _simplicial_set_ has been allocated
    try {
      return  _nodeToEliminate_( _simplicial_set_->allSimplicialNodes());
    } catch (NotFound&) {}

    try {
      return  _nodeToEliminate_( _simplicial_set_->allAlmostSimplicialNodes());
    } catch (NotFound&) {}

    try {
      return  _nodeToEliminate_( _simplicial_set_->allQuasiSimplicialNodes());
    } catch (NotFound&) {}

    // here: select the node through Kjaerulff's heuristic
    auto   iter      = nodeset_.cbegin();
    double min_score =  _log_weights_[*iter];
    NodeId best_node = *iter;

    for (++iter; iter != nodeset_.cend(); ++iter) {
      double score =  _log_weights_[*iter];

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
     _provide_fill_ins_ = do_it;

    if ( _simplicial_set_)  _simplicial_set_->setFillIns( _provide_fill_ins_);
  }

  /** @brief indicates whether the new fill-ins generated by a new eliminated
   * node, if needed, will be computed by the elimination sequence, or need be
   * computed by the triangulation itself. */
  bool DefaultPartialOrderedEliminationSequenceStrategy::providesFillIns() const {
    return  _provide_fill_ins_;
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
    if ( _simplicial_set_ != nullptr) {
       _simplicial_set_->makeClique(id);
       _simplicial_set_->eraseClique(id);

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
    if (! _provide_fill_ins_ || ( _simplicial_set_ == nullptr))
      return EliminationSequenceStrategy::fillIns();
    else
      return  _simplicial_set_->fillIns();
  }

  /** @brief creates a new elimination sequence of the same type as the current
   * object, but this sequence contains only an empty graph */
  DefaultPartialOrderedEliminationSequenceStrategy*
     DefaultPartialOrderedEliminationSequenceStrategy::newFactory() const {
    return new DefaultPartialOrderedEliminationSequenceStrategy(
        _simplicial_ratio_,
        _simplicial_threshold_);
  }

  /// virtual copy constructor
  DefaultPartialOrderedEliminationSequenceStrategy*
     DefaultPartialOrderedEliminationSequenceStrategy::copyFactory() const {
    return new DefaultPartialOrderedEliminationSequenceStrategy(*this);
  }

} /* namespace gum */
