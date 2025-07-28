/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief Base class for all elimination sequence algorithm that impose a given
 * partial ordering on the nodes elimination sequence, that is, the set of all
 * the nodes is divided into several subsets. Within each subset, any ordering
 * can be chosen. But all the nodes of the first subset must be eliminated
 * before the nodes of the second, which must be eliminated before those of the
 * third subset, and so on.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/graphs/algorithms/triangulations/eliminationStrategies/partialOrderedEliminationSequenceStrategy.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/algorithms/triangulations/eliminationStrategies/partialOrderedEliminationSequenceStrategy_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  /// default constructor
  PartialOrderedEliminationSequenceStrategy::PartialOrderedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_CONSTRUCTOR(PartialOrderedEliminationSequenceStrategy);
  }

  /// constructor for a (tensorly) non empty graph
  PartialOrderedEliminationSequenceStrategy::PartialOrderedEliminationSequenceStrategy(
      UndiGraph*                  graph,
      const NodeProperty< Size >* dom_sizes,
      const List< NodeSet >*      subsets) {
    setGraph(graph, dom_sizes);
    setPartialOrder(subsets);

    // for debugging purposes
    GUM_CONSTRUCTOR(PartialOrderedEliminationSequenceStrategy);
  }

  /// copy constructor
  PartialOrderedEliminationSequenceStrategy::PartialOrderedEliminationSequenceStrategy(
      const PartialOrderedEliminationSequenceStrategy& from) :
      EliminationSequenceStrategy(from), subsets_(from.subsets_), subset_iter_(from.subset_iter_),
      nodeset_(from.nodeset_), partial_order_needed_(from.partial_order_needed_) {
    // for debugging purposes
    GUM_CONS_CPY(PartialOrderedEliminationSequenceStrategy);
  }

  /// move constructor
  PartialOrderedEliminationSequenceStrategy::PartialOrderedEliminationSequenceStrategy(
      PartialOrderedEliminationSequenceStrategy&& from) :
      EliminationSequenceStrategy(std::move(from)), subsets_(from.subsets_),
      subset_iter_(from.subset_iter_), nodeset_(std::move(from.nodeset_)),
      partial_order_needed_(from.partial_order_needed_) {
    from.partial_order_needed_ = true;

    // for debugging purposes
    GUM_CONS_MOV(PartialOrderedEliminationSequenceStrategy);
  }

  /// destructor
  PartialOrderedEliminationSequenceStrategy::~PartialOrderedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR(PartialOrderedEliminationSequenceStrategy);
  }

  /// sets a new graph to be triangulated
  bool PartialOrderedEliminationSequenceStrategy::setGraph(
      UndiGraph*                  graph,
      const NodeProperty< Size >* domain_sizes) {
    if (EliminationSequenceStrategy::setGraph(graph, domain_sizes)) {
      setPartialOrder(subsets_);
      return true;
    }
    return false;
  }

  /// indicate whether a partial ordering is compatible with the current graph
  bool PartialOrderedEliminationSequenceStrategy::isPartialOrderNeeded_(
      const List< NodeSet >* subsets) const {
    if ((graph_ == nullptr) || (subsets == nullptr)) return true;

    // determine the set of nodes in the subsets that belong to the graph
    NodeSet nodes_found(graph_->size() / 2);
    for (const auto& nodes: *subsets) {
      for (const auto node: nodes) {
        if (graph_->existsNode(node)) { nodes_found.insert(node); }
      }
    }

    // check that the size of nodes_found is equal to that of the graph
    return nodes_found.size() != graph_->size();
  }

  /// sets a new partial order
  bool PartialOrderedEliminationSequenceStrategy::setPartialOrder(const List< NodeSet >* subsets) {
    // check that the partial order contains all the nodes of the graph
    partial_order_needed_ = isPartialOrderNeeded_(subsets);

    if (!partial_order_needed_) {
      subsets_ = subsets;

      // initialize properly the set of nodes that can be currently eliminated:
      // find the first subset that contains some node(s) of the graph
      nodeset_.clear();
      for (subset_iter_ = subsets_->cbegin(); subset_iter_ != subsets_->cend(); ++subset_iter_) {
        for (const auto node: *subset_iter_) {
          if (graph_->existsNode(node)) { nodeset_.insert(node); }
        }
        if (!nodeset_.empty()) return true;
      }
    }

    return false;
  }

  /// clears the sequence (to prepare, for instance, a new elimination sequence)
  void PartialOrderedEliminationSequenceStrategy::clear() {
    EliminationSequenceStrategy::clear();
    subsets_ = nullptr;
    nodeset_.clear();
    partial_order_needed_ = true;
  }

} /* namespace gum */
