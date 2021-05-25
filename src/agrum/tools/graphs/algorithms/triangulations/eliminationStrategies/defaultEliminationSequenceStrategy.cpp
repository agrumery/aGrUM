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
#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/defaultEliminationSequenceStrategy.h>

namespace gum {

  /// default constructor (uses an empty graph)
  DefaultEliminationSequenceStrategy::DefaultEliminationSequenceStrategy(
     double theRatio,
     double theThreshold) :
       _simplicial_ratio_(theRatio),
       _simplicial_threshold_(theThreshold) {
    GUM_CONSTRUCTOR(DefaultEliminationSequenceStrategy);
  }

  /// constructor for an a priori non empty graph
  DefaultEliminationSequenceStrategy::DefaultEliminationSequenceStrategy(
     UndiGraph*                  graph,
     const NodeProperty< Size >* domain_sizes,
     double                      ratio,
     double                      threshold) :
       _simplicial_ratio_(ratio),
       _simplicial_threshold_(threshold) {
    setGraph(graph, domain_sizes);

    GUM_CONSTRUCTOR(DefaultEliminationSequenceStrategy);
  }

  /// copy constructor
  DefaultEliminationSequenceStrategy::DefaultEliminationSequenceStrategy(
     const DefaultEliminationSequenceStrategy& from) :
      UnconstrainedEliminationSequenceStrategy(from),
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
    GUM_CONS_CPY(DefaultEliminationSequenceStrategy);
  }

  /// move constructor
  DefaultEliminationSequenceStrategy::DefaultEliminationSequenceStrategy(
     DefaultEliminationSequenceStrategy&& from) :
      UnconstrainedEliminationSequenceStrategy(std::move(from)),
       _log_weights_(std::move(from. _log_weights_)),
       _simplicial_set_(from. _simplicial_set_),
       _simplicial_ratio_(from. _simplicial_ratio_),
       _simplicial_threshold_(from. _simplicial_threshold_),
       _provide_fill_ins_(from. _provide_fill_ins_) {
     _simplicial_set_->replaceLogWeights(&from. _log_weights_, & _log_weights_);
    from. _simplicial_set_ = nullptr;

    GUM_CONS_MOV(DefaultEliminationSequenceStrategy);
  }

  /// destructor
  DefaultEliminationSequenceStrategy::~DefaultEliminationSequenceStrategy() {
    GUM_DESTRUCTOR(DefaultEliminationSequenceStrategy);

    if ( _simplicial_set_ != nullptr) delete  _simplicial_set_;
  }

  /** @brief creates a new elimination sequence of the same type as the current
   * object, but this sequence contains only an empty graph */
  DefaultEliminationSequenceStrategy*
     DefaultEliminationSequenceStrategy::newFactory() const {
    return new DefaultEliminationSequenceStrategy( _simplicial_ratio_,
                                                   _simplicial_threshold_);
  }

  /// virtual copy constructor
  DefaultEliminationSequenceStrategy*
     DefaultEliminationSequenceStrategy::copyFactory() const {
    return new DefaultEliminationSequenceStrategy(*this);
  }

  /// create a new simplicial set suited for the current graph
  void DefaultEliminationSequenceStrategy:: _createSimplicialSet_() {
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
  bool DefaultEliminationSequenceStrategy::setGraph(
     UndiGraph*                  graph,
     const NodeProperty< Size >* domain_sizes) {
    if (UnconstrainedEliminationSequenceStrategy::setGraph(graph, domain_sizes)) {
       _createSimplicialSet_();
      return true;
    }

    return false;
  }

  /// clears the sequence (to prepare, for instance, a new elimination sequence)
  void DefaultEliminationSequenceStrategy::clear() {
    UnconstrainedEliminationSequenceStrategy::clear();

     _log_weights_.clear();
    if ( _simplicial_set_ != nullptr) {
      delete  _simplicial_set_;
       _simplicial_set_ = nullptr;
    }
  }

  /// returns the new node to be eliminated within the triangulation algorithm
  NodeId DefaultEliminationSequenceStrategy::nextNodeToEliminate() {
    // if there is no simplicial set, send an exception
    if (graph_ == nullptr) { GUM_ERROR(NotFound, "the graph is empty") }

    // select a node to be eliminated: try simplicial nodes, then almost
    // simplicial nodes, then quasi-simplicial nodes
    // note that if  _graph_ != 0,  _simplicial_set_ has been allocated
    if ( _simplicial_set_->hasSimplicialNode())
      return  _simplicial_set_->bestSimplicialNode();
    else if ( _simplicial_set_->hasAlmostSimplicialNode())
      return  _simplicial_set_->bestAlmostSimplicialNode();
    else if ( _simplicial_set_->hasQuasiSimplicialNode())
      return  _simplicial_set_->bestQuasiSimplicialNode();
    else {
      // here: select the node through Kjaerulff's heuristic
      auto iter_heuristic =  _log_weights_.cbegin();

      if (iter_heuristic ==  _log_weights_.cend())
        GUM_ERROR(NotFound, "there exists no more node to eliminate")

      double min_weight     = iter_heuristic.val();
      NodeId removable_node = iter_heuristic.key();
      for (++iter_heuristic; iter_heuristic !=  _log_weights_.cend();
           ++iter_heuristic) {
        if (iter_heuristic.val() < min_weight) {
          removable_node = iter_heuristic.key();
          min_weight     = iter_heuristic.val();
        }
      }

      return removable_node;
    }
  }

  /** @brief if the elimination sequence is able to compute fill-ins, we
   * indicate
   * whether we want this feature to be activated */
  void DefaultEliminationSequenceStrategy::askFillIns(bool do_it) {
     _provide_fill_ins_ = do_it;

    if ( _simplicial_set_ != nullptr)
       _simplicial_set_->setFillIns( _provide_fill_ins_);
  }

  /** @brief indicates whether the new fill-ins generated by a new eliminated
   * node, if needed, will be computed by the elimination sequence, or need be
   * computed by the triangulation itself. */
  bool DefaultEliminationSequenceStrategy::providesFillIns() const {
    return  _provide_fill_ins_;
  }

  /** @brief indicates whether the elimination sequence updates by itself the
   * graph after a node has been eliminated */
  bool DefaultEliminationSequenceStrategy::providesGraphUpdate() const {
    return true;
  }

  /// performs all the graph/fill-ins updates provided
  void DefaultEliminationSequenceStrategy::eliminationUpdate(const NodeId id) {
    if ( _simplicial_set_ != nullptr) {
       _simplicial_set_->makeClique(id);
       _simplicial_set_->eraseClique(id);
    }
  }

  /** @brief in case fill-ins are provided, this function returns the fill-ins
   * generated after the last node elimination */
  const EdgeSet& DefaultEliminationSequenceStrategy::fillIns() {
    if (! _provide_fill_ins_ || ( _simplicial_set_ == nullptr))
      return UnconstrainedEliminationSequenceStrategy::fillIns();
    else
      return  _simplicial_set_->fillIns();
  }

} /* namespace gum */
