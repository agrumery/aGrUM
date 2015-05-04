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
 * @brief inline implementations of simplicial set
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <sstream>
#include <limits>
#include <cmath>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease IDE parser
#include <agrum/graphs/eliminations/defaultPartialOrderedEliminationSequenceStrategy.h>

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===  CLASS FOR RETRIEVING SIMPLICIAL, ALMOST AND QUASI SIMPLICIAL NODES === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// indicates whether a given node is a simplicial node

  INLINE
  bool SimplicialSet::isSimplicial(const NodeId id) {
    // update the list to which the node belongs if needed
    if (__changed_status.contains(id))
      __updateList(id);

    // check if the node belongs to the simplicial list
    return __simplicial_nodes.contains(id);
  }

  /// gets a simplicial node

  INLINE
  NodeId SimplicialSet::bestSimplicialNode() {
    if (!hasSimplicialNode()) {
      GUM_ERROR(NotFound, "No simplicial node could be found");
    }

    return __simplicial_nodes.top();
  }

  /// gets an almost simplicial node

  INLINE
  NodeId SimplicialSet::bestAlmostSimplicialNode() {
    if (!hasAlmostSimplicialNode()) {
      GUM_ERROR(NotFound, "no almost simplicial node could be found");
    }

    return __almost_simplicial_nodes.top();
  }

  /// gets a quasi simplicial node

  INLINE
  NodeId SimplicialSet::bestQuasiSimplicialNode() {
    if (!hasQuasiSimplicialNode()) {
      GUM_ERROR(NotFound, "no quasi simplicial node could be found");
    }

    return __quasi_simplicial_nodes.top();
  }

  /// put all the nodes in their appropriate list

  INLINE
  void SimplicialSet::__updateAllNodes() {
    // check if a node can enter the simplicial list
    for (auto iter = __changed_status.beginSafe(); // safe iterator needed here
         iter != __changed_status.endSafe(); ++iter) {
      __updateList(*iter);
    }
  }

  /// returns all the simplicial nodes

  INLINE
  const PriorityQueue<NodeId, float> &SimplicialSet::allSimplicialNodes() {
    __updateAllNodes();
    return __simplicial_nodes;
  }

  /// returns all the almost simplicial nodes

  INLINE
  const PriorityQueue<NodeId, float> &SimplicialSet::allAlmostSimplicialNodes() {
    __updateAllNodes();
    return __almost_simplicial_nodes;
  }

  /// returns all the quasi simplicial nodes

  INLINE
  const PriorityQueue<NodeId, float> &SimplicialSet::allQuasiSimplicialNodes() {
    __updateAllNodes();
    return __quasi_simplicial_nodes;
  }

  /// sets/unset the fill-ins storage in the standard triangulation procedure

  INLINE
  void SimplicialSet::setFillIns(bool b) { __we_want_fill_ins = b; }

  INLINE
  const EdgeSet &SimplicialSet::fillIns() const { return __fill_ins_list; }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
