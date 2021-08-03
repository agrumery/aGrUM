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
 * @brief implementation of the base class for all elimination sequence
 *algorithms
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/agrum.h>
#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/eliminationSequenceStrategy.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/eliminationSequenceStrategy_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  // an empty fill-ins set returned by default when we ask for a fill-ins set
  const EdgeSet& EliminationSequenceStrategy::_empty_fill_ins_() {
#ifdef GUM_DEBUG_MODE
    static bool first_use = true;
    if (first_use) {
      first_use = false;
      __debug__::_dec_creation_("Set", " __empty_edge_set", 0, "static variable correction", 0);
      __debug__::_dec_creation_("HashTable",
                                " __empty_edge_set",
                                0,
                                "static variable correction",
                                0);
    }
#endif
    static EdgeSet empty_fill_ins;
    return empty_fill_ins;
  }

  // default constructor
  EliminationSequenceStrategy::EliminationSequenceStrategy() {
    GUM_CONSTRUCTOR(EliminationSequenceStrategy);
  }

  // constructor for an a priori non empty graph
  EliminationSequenceStrategy::EliminationSequenceStrategy(
     UndiGraph*                  graph,
     const NodeProperty< Size >* domain_sizes) {
    EliminationSequenceStrategy::setGraph(graph, domain_sizes);

    GUM_CONSTRUCTOR(EliminationSequenceStrategy);
  }

  // copy constructor
  EliminationSequenceStrategy::EliminationSequenceStrategy(
     const EliminationSequenceStrategy& from) :
      graph_(from.graph_),
      domain_sizes_(from.domain_sizes_), log_domain_sizes_(from.log_domain_sizes_) {
    GUM_CONS_CPY(EliminationSequenceStrategy);
  }

  /// move constructor
  EliminationSequenceStrategy::EliminationSequenceStrategy(EliminationSequenceStrategy&& from) :
      graph_(from.graph_), domain_sizes_(from.domain_sizes_),
      log_domain_sizes_(std::move(from.log_domain_sizes_)) {
    GUM_CONS_MOV(EliminationSequenceStrategy);
  }

  // destructor
  EliminationSequenceStrategy::~EliminationSequenceStrategy() {
    GUM_DESTRUCTOR(EliminationSequenceStrategy);
  }

  // performs all the graph/fill-ins updates provided
  void EliminationSequenceStrategy::eliminationUpdate(const NodeId node) {}

  /** @brief in case fill-ins are provided, this function returns the fill-ins
   * due to all the nodes eliminated so far */
  const EdgeSet& EliminationSequenceStrategy::fillIns() { return _empty_fill_ins_(); }

  // clears the sequence (to prepare, for instance, a new elimination sequence)
  void EliminationSequenceStrategy::clear() {
    graph_        = nullptr;
    domain_sizes_ = nullptr;
    log_domain_sizes_.clear();
  }

  // sets a new graph to be triangulated
  bool EliminationSequenceStrategy::setGraph(UndiGraph*                  graph,
                                             const NodeProperty< Size >* dom_sizes) {
    // check that both the graph and the domain sizes are different from nullptr
    // or else that both are equal to nullptr
    if (((graph != nullptr) && (dom_sizes == nullptr))
        || ((graph == nullptr) && (dom_sizes != nullptr))) {
      GUM_ERROR(GraphError,
                "EliminationSequenceStrategy: one of the graph or the set of "
                "domain sizes is a null pointer.");
    }

    // check that each node has a domain size
    if (graph != nullptr) {
      for (const auto node: *graph)
        if (!dom_sizes->exists(node))
          GUM_ERROR(GraphError,
                    "EliminationSequenceStrategy needs a domain size "
                    "for every node in the graph.");
    }

    // avoid empty modifications
    if ((graph != graph_) || (dom_sizes != domain_sizes_)) {
      // remove, if any, the current graph
      clear();

      // assign a new graph
      graph_        = graph;
      domain_sizes_ = dom_sizes;

      if (graph_ != nullptr) {
        // compute the log of the modalities
        log_domain_sizes_.resize(graph_->sizeNodes() / 2);

        for (const auto node: *graph_)
          log_domain_sizes_.insert(node, std::log((*domain_sizes_)[node]));
      }

      return true;
    }

    return false;
  }

} /* namespace gum */
