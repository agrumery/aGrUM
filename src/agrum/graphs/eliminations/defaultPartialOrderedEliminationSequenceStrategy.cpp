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
#include <agrum/graphs/eliminations/defaultPartialOrderedEliminationSequenceStrategy.h>

namespace gum {

  /// default constructor (uses an empty graph)
  DefaultPartialOrderedEliminationSequenceStrategy::
      DefaultPartialOrderedEliminationSequenceStrategy(float theRatio,
                                                       float theThreshold)
      : __graph(0), __modalities(0), __subsets(0), __simplicial_set(0),
        __simplicial_ratio(theRatio), __simplicial_threshold(theThreshold),
        __provide_fill_ins(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultPartialOrderedEliminationSequenceStrategy);
  }

  /// constructor for an a priori non empty graph
  DefaultPartialOrderedEliminationSequenceStrategy::
      DefaultPartialOrderedEliminationSequenceStrategy(
          UndiGraph *graph, const NodeProperty<Size> *modal,
          const List<NodeSet> *subsets, float ratio, float threshold)
      : __graph(0), __modalities(0), __subsets(0), __simplicial_set(0),
        __simplicial_ratio(ratio), __simplicial_threshold(threshold),
        __provide_fill_ins(false) {
    // for debugging purposes
    GUM_CONSTRUCTOR(DefaultPartialOrderedEliminationSequenceStrategy);
    setGraph(graph, modal, subsets);
  }

  /// copy constructor
  DefaultPartialOrderedEliminationSequenceStrategy::
      DefaultPartialOrderedEliminationSequenceStrategy(
          const DefaultPartialOrderedEliminationSequenceStrategy &from)
      : __graph(0), __modalities(0), __subsets(0), __simplicial_set(0),
        __simplicial_ratio(from.__simplicial_ratio),
        __simplicial_threshold(from.__simplicial_threshold),
        __provide_fill_ins(from.__provide_fill_ins) {
    GUM_CONS_CPY(DefaultPartialOrderedEliminationSequenceStrategy);

    if (__graph) {
      setGraph(from.__graph, from.__modalities, from.__subsets);
      __subset_iter = from.__subset_iter;
      __nodeset = from.__nodeset;
    }
  }

  /// destructor
  DefaultPartialOrderedEliminationSequenceStrategy::
      ~DefaultPartialOrderedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR(DefaultPartialOrderedEliminationSequenceStrategy);

    if (__simplicial_set)
      delete __simplicial_set;
  }

  /// sets a new graph to be triangulated
  void DefaultPartialOrderedEliminationSequenceStrategy::setGraph(
      UndiGraph *graph, const NodeProperty<Size> *modal,
      const List<NodeSet> *subsets) {
    // check that both the graph, the modalities and the subsets are different
    // from 0 or else that both are equal to 0
    unsigned char nb = 0;

    if (graph)
      ++nb;

    if (modal)
      ++nb;

    if (subsets)
      ++nb;

    if (nb && (nb != 3))
      GUM_ERROR(GraphError, "DefaultPartialOrderedEliminationSequenceStrategy needs "
                            "valid graphs, domain sizes and partial ordering");

    // check that each node has a domain size
    for (const auto node : graph->nodes())
      if (!modal->exists(node))
        GUM_ERROR(
            GraphError,
            "DefaultPartialOrderedEliminationSequenceStrategy needs  domain sizes");

    // avoid empty modifications
    if ((graph != __graph) || (modal != __modalities) || (subsets != __subsets)) {
      // remove, if any, the current graph and its simplicial set
      clear();

      // assign a new graph
      __graph = graph;
      __modalities = modal;
      __subsets = subsets;

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

        // initialize properly the set of nodes that can be currently eliminated
        __subset_iter = __subsets->cbegin();

        __nodeset = *__subset_iter;
      }
    }
  }

  /// clears the sequence (to prepare, for instance, a new elimination sequence)
  void DefaultPartialOrderedEliminationSequenceStrategy::clear() {
    __graph = 0;
    __modalities = 0;
    __log_modalities.clear();
    __log_weights.clear();
    __subsets = 0;
    __subset_iter.clear();
    __nodeset.clear();

    if (__simplicial_set)
      delete __simplicial_set;

    __simplicial_set = 0;
  }

  /// returns the best possible node to be eliminated
  NodeId DefaultPartialOrderedEliminationSequenceStrategy::__nodeToEliminate(
      const PriorityQueue<NodeId, float> &possibleNodes) {
    bool found = false;
    float min_score = 0;
    NodeId best_node = 0;

    for (const auto node : __nodeset)
      try {
        float score = possibleNodes.priority(node);

        if (!found || (score < min_score)) {
          found = true;
          min_score = score;
          best_node = node;
        }
      } catch (NotFound &) {
      }

    if (!found) {
      GUM_ERROR(NotFound, "no possible node to eliminate");
    }

    return best_node;
  }

  /// returns the new node to be eliminated within the triangulation algorithm
  NodeId DefaultPartialOrderedEliminationSequenceStrategy::nextNodeToEliminate() {
    // if there is no simplicial set, send an exception
    if (!__graph)
      GUM_ERROR(NotFound, "the graph is empty");

    if (__nodeset.empty()) {
      GUM_ERROR(NotFound, "no node is admissible");
    }

    // select a node to be eliminated: try simplicial nodes, then almost
    // simplicial nodes, then quasi-simplicial nodes
    // note that if __graph != 0, __simplicial_set has been allocated
    try {
      return __nodeToEliminate(__simplicial_set->allSimplicialNodes());
    } catch (NotFound &) {
    }

    try {
      return __nodeToEliminate(__simplicial_set->allAlmostSimplicialNodes());
    } catch (NotFound &) {
    }

    try {
      return __nodeToEliminate(__simplicial_set->allQuasiSimplicialNodes());
    } catch (NotFound &) {
    }

    // here: select the node through Kjaerulff's heuristic
    auto iter = __nodeset.begin();
    float min_score = __log_weights[*iter];
    NodeId best_node = *iter;

    for (++iter; iter != __nodeset.end(); ++iter) {
      float score = __log_weights[*iter];

      if (score < min_score) {
        min_score = score;
        best_node = *iter;
      }
    }

    return best_node;
  }

  /** @brief if the elimination sequence is able to compute fill-ins, we indicate
   * whether we want this feature to be activated */
  void DefaultPartialOrderedEliminationSequenceStrategy::askFillIns(bool do_it) {
    __provide_fill_ins = do_it;

    if (__simplicial_set)
      __simplicial_set->setFillIns(__provide_fill_ins);
  }

  /** @brief indicates whether the new fill-ins generated by a new eliminated
   * node, if needed, will be computed by the elimination sequence, or need be
   * computed by the triangulation itself. */
  bool DefaultPartialOrderedEliminationSequenceStrategy::providesFillIns() const {
    return __provide_fill_ins;
  }

  /** @brief indicates whether the elimination sequence updates by itself the
   * graph after a node has been eliminated */
  bool
  DefaultPartialOrderedEliminationSequenceStrategy::providesGraphUpdate() const {
    return true;
  }

  /// performs all the graph/fill-ins updates provided
  void DefaultPartialOrderedEliminationSequenceStrategy::eliminationUpdate(
      const NodeId id) {
    // check whether we can do something
    if (__simplicial_set) {
      __simplicial_set->makeClique(id);
      __simplicial_set->eraseClique(id);

      // remove the node from __nodeset
      __nodeset.erase(id);

      if (__nodeset.empty()) {
        ++__subset_iter;

        if (__subset_iter != __subsets->cend())
          __nodeset = *__subset_iter;
      }
    }
  }

  /** @brief in case fill-ins are provided, this function returns the fill-ins
   * generated after the last node elimination */
  const EdgeSet &DefaultPartialOrderedEliminationSequenceStrategy::fillIns() {
    if (!__provide_fill_ins || !__simplicial_set)
      return EliminationSequenceStrategy::fillIns();
    else
      return __simplicial_set->fillIns();
  }

  /** @brief creates a new elimination sequence of the same type as the current
   * object, but this sequence contains only an empty graph */
  DefaultPartialOrderedEliminationSequenceStrategy *
  DefaultPartialOrderedEliminationSequenceStrategy::newFactory() const {
    return new DefaultPartialOrderedEliminationSequenceStrategy(
        __simplicial_ratio, __simplicial_threshold);
  }

} /* namespace gum */
