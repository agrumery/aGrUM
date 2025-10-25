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
 * @brief inline implementations of simplicial set
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <limits>
#include <sstream>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease IDE parser
#  include <agrum/base/graphs/algorithms/triangulations/eliminationStrategies/defaultPartialOrderedEliminationSequenceStrategy.h>

#  include <agrum/base/core/math/math_utils.h>

namespace gum {

  /* ===========================================================================
   */
  /* ===========================================================================
   */
  /* ===  CLASS FOR RETRIEVING SIMPLICIAL, ALMOST AND QUASI SIMPLICIAL NODES ===
   */
  /* ===========================================================================
   */
  /* ===========================================================================
   */

  /// indicates whether a given node is a simplicial node
  INLINE
  bool SimplicialSet::isSimplicial(const NodeId id) {
    // update the list to which the node belongs if needed
    if (_changed_status_.contains(id)) _updateList_(id);

    // check if the node belongs to the simplicial list
    return _simplicial_nodes_.contains(id);
  }

  /// gets a simplicial node
  INLINE
  NodeId SimplicialSet::bestSimplicialNode() {
    if (!hasSimplicialNode()) { GUM_ERROR(NotFound, "No simplicial node could be found") }

    return _simplicial_nodes_.top();
  }

  /// gets an almost simplicial node
  INLINE
  NodeId SimplicialSet::bestAlmostSimplicialNode() {
    if (!hasAlmostSimplicialNode()) {
      GUM_ERROR(NotFound, "no almost simplicial node could be found")
    }

    return _almost_simplicial_nodes_.top();
  }

  /// gets a quasi simplicial node
  INLINE
  NodeId SimplicialSet::bestQuasiSimplicialNode() {
    if (!hasQuasiSimplicialNode()) {
      GUM_ERROR(NotFound, "no quasi simplicial node could be found")
    }

    return _quasi_simplicial_nodes_.top();
  }

  /// put all the nodes in their appropriate list
  INLINE
  void SimplicialSet::_updateAllNodes_() {
    // check if a node can enter the simplicial list
    for (auto iter = _changed_status_.beginSafe();   // safe iterator needed here
         iter != _changed_status_.endSafe();
         ++iter) {
      _updateList_(*iter);
    }
  }

  /// returns all the simplicial nodes
  INLINE
  const PriorityQueue< NodeId, double >& SimplicialSet::allSimplicialNodes() {
    _updateAllNodes_();
    return _simplicial_nodes_;
  }

  /// returns all the almost simplicial nodes
  INLINE
  const PriorityQueue< NodeId, double >& SimplicialSet::allAlmostSimplicialNodes() {
    _updateAllNodes_();
    return _almost_simplicial_nodes_;
  }

  /// returns all the quasi simplicial nodes
  INLINE
  const PriorityQueue< NodeId, double >& SimplicialSet::allQuasiSimplicialNodes() {
    _updateAllNodes_();
    return _quasi_simplicial_nodes_;
  }

  /// sets/unset the fill-ins storage in the standard triangulation procedure
  INLINE
  void SimplicialSet::setFillIns(bool b) { _we_want_fill_ins_ = b; }

  /// returns the set of fill-ins
  INLINE
  const EdgeSet& SimplicialSet::fillIns() const { return _fill_ins_list_; }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
