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
#pragma once


/** @file
 * @brief Inline implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  /// returns an elimination ordering compatible with the triangulated graph
  INLINE
  const std::vector< NodeId >& StaticTriangulation::eliminationOrder() {
    if (!_has_triangulation_) _triangulate_();

    return _elim_order_;
  }

  /** @brief returns the index of a given node in the elimination order
   * (0 = first node eliminated) */
  INLINE
  Idx StaticTriangulation::eliminationOrder(const NodeId id) {
    if (!_has_triangulation_) _triangulate_();

    return _reverse_elim_order_[id];
  }

  /** @brief returns the number of a given node in the elimination order
   * (0 = first node eliminated) */
  INLINE
  const NodeProperty< NodeId >& StaticTriangulation::reverseEliminationOrder() {
    if (!_has_triangulation_) _triangulate_();

    return _reverse_elim_order_;
  }

  /// returns the elimination tree of a compatible ordering
  INLINE
  const CliqueGraph& StaticTriangulation::eliminationTree() {
    if (!_has_elimination_tree_) _computeEliminationTree_();

    return _elim_tree_;
  }

  /// returns a compatible junction tree
  INLINE
  const CliqueGraph& StaticTriangulation::junctionTree() {
    // checks if junctionTree already exists
    if (!_has_junction_tree_) {
      _junction_tree_     = &(junction_tree_strategy_->junctionTree());
      _has_junction_tree_ = true;
    }

    return *_junction_tree_;
  }

  /// returns a junction tree of maximal prime subgraphs
  INLINE
  const CliqueGraph& StaticTriangulation::maxPrimeSubgraphTree() {
    if (!_has_max_prime_junction_tree_) _computeMaxPrimeJunctionTree_();

    return _max_prime_junction_tree_;
  }

  /** @brief returns the Id of the maximal prime subgraph created by the
   * elimination of a given node during the triangulation process */
  INLINE
  NodeId StaticTriangulation::createdMaxPrimeSubgraph(const NodeId id) {
    if (!_has_max_prime_junction_tree_) _computeMaxPrimeJunctionTree_();

    return _node_2_max_prime_clique_[id];
  }

  /** @brief returns the Id of the clique created by the
   * elimination of a given node during the triangulation process */
  INLINE
  NodeId StaticTriangulation::createdJunctionTreeClique(const NodeId id) {
    return junction_tree_strategy_->createdClique(id);
  }

  /** @brief returns the Ids of the cliques of the junction tree created by the
   * elimination of the nodes */
  INLINE
  const NodeProperty< NodeId >& StaticTriangulation::createdJunctionTreeCliques() {
    return junction_tree_strategy_->createdCliques();
  }

  /// sets/unset the fill-ins storage in the standard triangulation procedure
  INLINE
  void StaticTriangulation::setFillIns(bool b) {
    _we_want_fill_ins_ = b;
    elimination_sequence_strategy_->askFillIns(b);
  }

  /// returns the graph to be triangulated
  INLINE
  const UndiGraph* StaticTriangulation::originalGraph() const { return _original_graph_; }

  /// returns the elimination sequence strategy used by the triangulation
  INLINE EliminationSequenceStrategy& StaticTriangulation::eliminationSequenceStrategy() const {
    return *elimination_sequence_strategy_;
  }

  /// returns the junction tree strategy used by the triangulation
  INLINE JunctionTreeStrategy& StaticTriangulation::junctionTreeStrategy() const {
    return *junction_tree_strategy_;
  }

  /// indicates wether minimality is required
  INLINE bool StaticTriangulation::isMinimalityRequired() const { return _minimality_required_; }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
