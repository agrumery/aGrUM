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
 * @brief The Prim algorithm for computing min cost spanning trees or forests
 *
 * @author Jean-Philippe DUBUS & Christophe GONZALES(_at_AMU)
 */

#include <agrum/agrum.h>

#include <agrum/base/graphs/algorithms/spanningForestPrim.h>

namespace gum {

  /// Default constructor
  SpanningForestPrim::SpanningForestPrim(const UndiGraph*             graph,
                                         const EdgeProperty< float >* cost) :
      SpanningForest(), _graph_(*graph), _costTable_(*cost), _spanning_tree_cost_(0),
      _require_computation_(true) {
    if (!graph || !cost) { GUM_ERROR(GraphError, "invalid null graph or edge cost pointer") }

    // for debugging purposes
    GUM_CONSTRUCTOR(SpanningForestPrim);
  }

  // copy constructor
  SpanningForestPrim::SpanningForestPrim(const SpanningForestPrim& from) :
      SpanningForest(), _graph_(from._graph_), _costTable_(from._costTable_),
      _edgesToExplore_(from._edgesToExplore_), _spanning_tree_(from._spanning_tree_),
      _spanning_tree_cost_(from._spanning_tree_cost_),
      _require_computation_(from._require_computation_) {
    // for debugging purposes
    GUM_CONS_CPY(SpanningForestPrim);
  }

  // destructor
  SpanningForestPrim::~SpanningForestPrim() {
    // for debugging purposes
    GUM_DESTRUCTOR(SpanningForestPrim);
  }

  /// Returns the cost of the spanning forest
  float SpanningForestPrim::costOfSpanningForest() {
    if (_require_computation_) _compute_();

    return _spanning_tree_cost_;
  }

  /// Returns the edges in a min cost spanning forest
  const EdgeSet& SpanningForestPrim::edgesInSpanningForest() {
    if (_require_computation_) _compute_();

    return _spanning_tree_.edges();
  }

  /// Construct the spanning forest
  const UndiGraph& SpanningForestPrim::spanningForest() {
    if (_require_computation_) _compute_();

    return _spanning_tree_;
  }

  /// compute the spanning forest
  void SpanningForestPrim::_compute_() {
    // compute a spanning tree in every connected component
    for (const auto node: _graph_.nodes()) {
      if (!_spanning_tree_.existsNode(node)) { _computeInAComponent_(node); }
    }

    // indicate that everything was computed
    _require_computation_ = false;
  }

  /// compute a spanning tree
  void SpanningForestPrim::_computeInAComponent_(const NodeId id) {
    // add the node to the spanning tree
    _spanning_tree_.addNodeWithId(id);

    // explore its neighborhood
    _exploreNode_(id);

    // get the next nodes to link to the current spanning tree nodes

    while (!_edgesToExplore_.empty()) {
      const Edge   edge   = _edgesToExplore_.pop();
      const NodeId first  = edge.first();
      const NodeId second = edge.second();

      // consider only the edges that have one extremal node not in the spanning
      // tree as those that can be added to the tree

      if (!_spanning_tree_.existsNode(first)) {
        // add the edge to the spanning tree
        _spanning_tree_.addNodeWithId(first);
        _spanning_tree_.addEdge(first, second);
        _spanning_tree_cost_ += _costTable_[edge];

        // We must explore the first node's neighborhood
        _exploreNode_(first);
      } else if (!_spanning_tree_.existsNode(second)) {
        // add the edge to the spanning tree
        _spanning_tree_.addNodeWithId(second);
        _spanning_tree_.addEdge(first, second);
        _spanning_tree_cost_ += _costTable_[edge];

        // We must explore the second node
        _exploreNode_(second);
      }
    }
  }

  /// explore the neighborhood of a node belonging to the spanning tree
  void SpanningForestPrim::_exploreNode_(const NodeId id) {
    // add its neighbors  _edgesToExplore_ to indicate that they are
    // tensor next nodes to explore
    for (const auto node: _graph_.neighbours(id)) {
      if (!_spanning_tree_.existsNode(node)) {
        Edge edge(node, id);
        _edgesToExplore_.insert(edge, _costTable_[edge]);
      }
    }
  }

} /* namespace gum */
