/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief inline implementations of simplicial set
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <limits>
#include <sstream>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease IDE parser
#  include <agrum/tools/core/math/math_utils.h>
#  include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/defaultPartialOrderedEliminationSequenceStrategy.h>

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
