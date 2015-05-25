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
 * @brief An efficient unconstrained elimination sequence algorithm
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <cmath>

#include <agrum/config.h>

#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/eliminations/defaultEliminationSequenceStrategy.h>

namespace gum {

  /// default constructor (uses an empty graph)
  DefaultEliminationSequenceStrategy::DefaultEliminationSequenceStrategy(
      float theRatio, float theThreshold)
      : __graph(0), __modalities(0), __simplicial_set(0),
        __simplicial_ratio(theRatio), __simplicial_threshold(theThreshold),
        __provide_fill_ins(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultEliminationSequenceStrategy);
  }

  /// constructor for an a priori non empty graph
  DefaultEliminationSequenceStrategy::DefaultEliminationSequenceStrategy(
      UndiGraph *graph, const NodeProperty<Size> *modal, float ratio,
      float threshold)
      : __graph(nullptr), __modalities(nullptr), __simplicial_set(0),
        __simplicial_ratio(ratio), __simplicial_threshold(threshold),
        __provide_fill_ins(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultEliminationSequenceStrategy);
    setGraph(graph, modal);
  }

  /// copy constructor
  DefaultEliminationSequenceStrategy::DefaultEliminationSequenceStrategy(
      const DefaultEliminationSequenceStrategy &from)
      : __graph(0), __modalities(0), __simplicial_set(0),
        __simplicial_ratio(from.__simplicial_ratio),
        __simplicial_threshold(from.__simplicial_threshold),
        __provide_fill_ins(from.__provide_fill_ins) {
    GUM_CONS_CPY(DefaultEliminationSequenceStrategy);

    if (__graph) {
      setGraph(from.__graph, from.__modalities);
    }
  }

  /// destructor
  DefaultEliminationSequenceStrategy::~DefaultEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR(DefaultEliminationSequenceStrategy);

    if (__simplicial_set)
      delete __simplicial_set;
  }

  /// sets a new graph to be triangulated
  void
  DefaultEliminationSequenceStrategy::setGraph(UndiGraph *graph,
                                               const NodeProperty<Size> *modal) {
    // check that both the graph and the modalities are different from 0
    // or else that both are equal to 0
    if ((!graph && modal) || (graph && !modal)) {
      GUM_ERROR(
          GraphError,
          "DefaultEliminationSequenceStrategy needs valid graphs and domain sizes");
    }

    // check that each node has a domain size
    for (const auto node : graph->nodes())
      if (!modal->exists(node))
        GUM_ERROR(GraphError,
                  "DefaultEliminationSequenceStrategy needs domain sizes");

    // avoid empty modifications
    if ((graph != __graph) || (modal != __modalities)) {
      // remove, if any, the current graph and its simplicial set
      clear();

      // assign a new graph
      __graph = graph;
      __modalities = modal;

      if (graph) {
        // compute the log of the modalities
        __log_modalities.resize(__graph->sizeNodes() / 2);

        for (const auto node : graph->nodes())
          __log_modalities.insert(node, std::log((*modal)[node]));

        // creation du simplicial set
        __simplicial_set =
            new SimplicialSet(__graph, &__log_modalities, &__log_weights,
                              __simplicial_ratio, __simplicial_threshold);

        __simplicial_set->setFillIns(__provide_fill_ins);
      }
    }
  }

  /// clears the sequence (to prepare, for instance, a new elimination sequence)
  void DefaultEliminationSequenceStrategy::clear() {
    __graph = 0;
    __modalities = 0;
    __log_modalities.clear();
    __log_weights.clear();

    if (__simplicial_set)
      delete __simplicial_set;

    __simplicial_set = 0;
  }

  /// returns the new node to be eliminated within the triangulation algorithm
  NodeId DefaultEliminationSequenceStrategy::nextNodeToEliminate() {
    // if there is no simplicial set, send an exception
    if (!__graph) {
      GUM_ERROR(NotFound, "the graph is empty");
    }

    // select a node to be eliminated: try simplicial nodes, then almost
    // simplicial nodes, then quasi-simplicial nodes
    // note that if __graph != 0, __simplicial_set has been allocated
    if (__simplicial_set->hasSimplicialNode())
      return __simplicial_set->bestSimplicialNode();
    else if (__simplicial_set->hasAlmostSimplicialNode())
      return __simplicial_set->bestAlmostSimplicialNode();
    else if (__simplicial_set->hasQuasiSimplicialNode())
      return __simplicial_set->bestQuasiSimplicialNode();
    else {
      // here: select the node through Kjaerulff's heuristic
      auto iter_heuristic = __log_weights.begin();

      if (iter_heuristic == __log_weights.end())
        GUM_ERROR(NotFound, "there exists no more node to eliminate");

      float min_weight = iter_heuristic.val();
      NodeId removable_node = iter_heuristic.key();

      for (++iter_heuristic; iter_heuristic != __log_weights.end(); ++iter_heuristic)
        if (iter_heuristic.val() < min_weight) {
          removable_node = iter_heuristic.key();
          min_weight = iter_heuristic.val();
        }

      return removable_node;
    }
  }

  /** @brief if the elimination sequence is able to compute fill-ins, we indicate
   * whether we want this feature to be activated */
  void DefaultEliminationSequenceStrategy::askFillIns(bool do_it) {
    __provide_fill_ins = do_it;

    if (__simplicial_set)
      __simplicial_set->setFillIns(__provide_fill_ins);
  }

  /** @brief indicates whether the new fill-ins generated by a new eliminated
   * node, if needed, will be computed by the elimination sequence, or need be
   * computed by the triangulation itself. */
  bool DefaultEliminationSequenceStrategy::providesFillIns() const {
    return __provide_fill_ins;
  }

  /** @brief indicates whether the elimination sequence updates by itself the
   * graph after a node has been eliminated */
  bool DefaultEliminationSequenceStrategy::providesGraphUpdate() const {
    return true;
  }

  /// performs all the graph/fill-ins updates provided
  void DefaultEliminationSequenceStrategy::eliminationUpdate(const NodeId id) {
    if (__simplicial_set) {
      __simplicial_set->makeClique(id);
      __simplicial_set->eraseClique(id);
    }
  }

  /** @brief in case fill-ins are provided, this function returns the fill-ins
   * generated after the last node elimination */
  const EdgeSet &DefaultEliminationSequenceStrategy::fillIns() {
    if (!__provide_fill_ins || !__simplicial_set)
      return EliminationSequenceStrategy::fillIns();
    else
      return __simplicial_set->fillIns();
  }

  /** @brief creates a new elimination sequence of the same type as the current
   * object, but this sequence contains only an empty graph */
  DefaultEliminationSequenceStrategy *
  DefaultEliminationSequenceStrategy::newFactory() const {
    return new DefaultEliminationSequenceStrategy(__simplicial_ratio,
                                                  __simplicial_threshold);
  }

} /* namespace gum */
