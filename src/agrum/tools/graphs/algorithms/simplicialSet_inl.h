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
 * @brief inline implementations of simplicial set
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <limits>
#include <sstream>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease IDE parser
#  include <agrum/tools/core/math/math.h>
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
    if (changed_status__.contains(id)) updateList__(id);

    // check if the node belongs to the simplicial list
    return simplicial_nodes__.contains(id);
  }


  /// gets a simplicial node
  INLINE
  NodeId SimplicialSet::bestSimplicialNode() {
    if (!hasSimplicialNode()) {
      GUM_ERROR(NotFound, "No simplicial node could be found");
    }

    return simplicial_nodes__.top();
  }


  /// gets an almost simplicial node
  INLINE
  NodeId SimplicialSet::bestAlmostSimplicialNode() {
    if (!hasAlmostSimplicialNode()) {
      GUM_ERROR(NotFound, "no almost simplicial node could be found");
    }

    return almost_simplicial_nodes__.top();
  }


  /// gets a quasi simplicial node
  INLINE
  NodeId SimplicialSet::bestQuasiSimplicialNode() {
    if (!hasQuasiSimplicialNode()) {
      GUM_ERROR(NotFound, "no quasi simplicial node could be found");
    }

    return quasi_simplicial_nodes__.top();
  }


  /// put all the nodes in their appropriate list
  INLINE
  void SimplicialSet::updateAllNodes__() {
    // check if a node can enter the simplicial list
    for (auto iter = changed_status__.beginSafe();   // safe iterator needed here
         iter != changed_status__.endSafe();
         ++iter) {
      updateList__(*iter);
    }
  }


  /// returns all the simplicial nodes
  INLINE
  const PriorityQueue< NodeId, double >& SimplicialSet::allSimplicialNodes() {
    updateAllNodes__();
    return simplicial_nodes__;
  }


  /// returns all the almost simplicial nodes
  INLINE
  const PriorityQueue< NodeId, double >&
     SimplicialSet::allAlmostSimplicialNodes() {
    updateAllNodes__();
    return almost_simplicial_nodes__;
  }


  /// returns all the quasi simplicial nodes
  INLINE
  const PriorityQueue< NodeId, double >& SimplicialSet::allQuasiSimplicialNodes() {
    updateAllNodes__();
    return quasi_simplicial_nodes__;
  }


  /// sets/unset the fill-ins storage in the standard triangulation procedure
  INLINE
  void SimplicialSet::setFillIns(bool b) { we_want_fill_ins__ = b; }


  /// returns the set of fill-ins
  INLINE
  const EdgeSet& SimplicialSet::fillIns() const { return fill_ins_list__; }


} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
