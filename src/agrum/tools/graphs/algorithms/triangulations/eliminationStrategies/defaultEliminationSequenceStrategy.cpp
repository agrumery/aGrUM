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
#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/defaultEliminationSequenceStrategy.h>

namespace gum {

  /// default constructor (uses an empty graph)
  DefaultEliminationSequenceStrategy::DefaultEliminationSequenceStrategy(
     double theRatio,
     double theThreshold) :
      simplicial_ratio__(theRatio),
      simplicial_threshold__(theThreshold) {
    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultEliminationSequenceStrategy);
  }

  /// constructor for an a priori non empty graph
  DefaultEliminationSequenceStrategy::DefaultEliminationSequenceStrategy(
     UndiGraph*                  graph,
     const NodeProperty< Size >* domain_sizes,
     double                      ratio,
     double                      threshold) :
      simplicial_ratio__(ratio),
      simplicial_threshold__(threshold) {
    setGraph(graph, domain_sizes);

    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultEliminationSequenceStrategy);
  }

  /// copy constructor
  DefaultEliminationSequenceStrategy::DefaultEliminationSequenceStrategy(
     const DefaultEliminationSequenceStrategy& from) :
      UnconstrainedEliminationSequenceStrategy(from),
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
    GUM_CONS_CPY(DefaultEliminationSequenceStrategy);
  }

  /// move constructor
  DefaultEliminationSequenceStrategy::DefaultEliminationSequenceStrategy(
     DefaultEliminationSequenceStrategy&& from) :
      UnconstrainedEliminationSequenceStrategy(std::move(from)),
      log_weights__(std::move(from.log_weights__)),
      simplicial_set__(from.simplicial_set__),
      simplicial_ratio__(from.simplicial_ratio__),
      simplicial_threshold__(from.simplicial_threshold__),
      provide_fill_ins__(from.provide_fill_ins__) {
    simplicial_set__->replaceLogWeights(&from.log_weights__, &log_weights__);
    from.simplicial_set__ = nullptr;

    // for debugging purposes
    GUM_CONS_MOV(DefaultEliminationSequenceStrategy);
  }

  /// destructor
  DefaultEliminationSequenceStrategy::~DefaultEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR(DefaultEliminationSequenceStrategy);

    if (simplicial_set__ != nullptr) delete simplicial_set__;
  }

  /** @brief creates a new elimination sequence of the same type as the current
   * object, but this sequence contains only an empty graph */
  DefaultEliminationSequenceStrategy*
     DefaultEliminationSequenceStrategy::newFactory() const {
    return new DefaultEliminationSequenceStrategy(simplicial_ratio__,
                                                  simplicial_threshold__);
  }

  /// virtual copy constructor
  DefaultEliminationSequenceStrategy*
     DefaultEliminationSequenceStrategy::copyFactory() const {
    return new DefaultEliminationSequenceStrategy(*this);
  }

  /// create a new simplicial set suited for the current graph
  void DefaultEliminationSequenceStrategy::createSimplicialSet__() {
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
  bool DefaultEliminationSequenceStrategy::setGraph(
     UndiGraph*                  graph,
     const NodeProperty< Size >* domain_sizes) {
    if (UnconstrainedEliminationSequenceStrategy::setGraph(graph, domain_sizes)) {
      createSimplicialSet__();
      return true;
    }

    return false;
  }

  /// clears the sequence (to prepare, for instance, a new elimination sequence)
  void DefaultEliminationSequenceStrategy::clear() {
    UnconstrainedEliminationSequenceStrategy::clear();

    log_weights__.clear();
    if (simplicial_set__ != nullptr) {
      delete simplicial_set__;
      simplicial_set__ = nullptr;
    }
  }

  /// returns the new node to be eliminated within the triangulation algorithm
  NodeId DefaultEliminationSequenceStrategy::nextNodeToEliminate() {
    // if there is no simplicial set, send an exception
    if (graph_ == nullptr) { GUM_ERROR(NotFound, "the graph is empty"); }

    // select a node to be eliminated: try simplicial nodes, then almost
    // simplicial nodes, then quasi-simplicial nodes
    // note that if graph__ != 0, simplicial_set__ has been allocated
    if (simplicial_set__->hasSimplicialNode())
      return simplicial_set__->bestSimplicialNode();
    else if (simplicial_set__->hasAlmostSimplicialNode())
      return simplicial_set__->bestAlmostSimplicialNode();
    else if (simplicial_set__->hasQuasiSimplicialNode())
      return simplicial_set__->bestQuasiSimplicialNode();
    else {
      // here: select the node through Kjaerulff's heuristic
      auto iter_heuristic = log_weights__.cbegin();

      if (iter_heuristic == log_weights__.cend())
        GUM_ERROR(NotFound, "there exists no more node to eliminate")

      double min_weight     = iter_heuristic.val();
      NodeId removable_node = iter_heuristic.key();
      for (++iter_heuristic; iter_heuristic != log_weights__.cend();
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
    provide_fill_ins__ = do_it;

    if (simplicial_set__ != nullptr)
      simplicial_set__->setFillIns(provide_fill_ins__);
  }

  /** @brief indicates whether the new fill-ins generated by a new eliminated
   * node, if needed, will be computed by the elimination sequence, or need be
   * computed by the triangulation itself. */
  bool DefaultEliminationSequenceStrategy::providesFillIns() const {
    return provide_fill_ins__;
  }

  /** @brief indicates whether the elimination sequence updates by itself the
   * graph after a node has been eliminated */
  bool DefaultEliminationSequenceStrategy::providesGraphUpdate() const {
    return true;
  }

  /// performs all the graph/fill-ins updates provided
  void DefaultEliminationSequenceStrategy::eliminationUpdate(const NodeId id) {
    if (simplicial_set__ != nullptr) {
      simplicial_set__->makeClique(id);
      simplicial_set__->eraseClique(id);
    }
  }

  /** @brief in case fill-ins are provided, this function returns the fill-ins
   * generated after the last node elimination */
  const EdgeSet& DefaultEliminationSequenceStrategy::fillIns() {
    if (!provide_fill_ins__ || (simplicial_set__ == nullptr))
      return UnconstrainedEliminationSequenceStrategy::fillIns();
    else
      return simplicial_set__->fillIns();
  }

} /* namespace gum */
