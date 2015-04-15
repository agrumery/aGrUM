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
 * @brief Inline implementation of Base classes for oriented graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

// to ease parser in IDEs
#include <agrum/graphs/diGraph.h>

namespace gum {

  INLINE void DiGraph::insertArc(const NodeId tail, const NodeId head) {
    addArc(tail, head);
  }
  INLINE void DiGraph::addArc(const NodeId tail, const NodeId head) {
    if (!exists(head)) {
      GUM_ERROR(InvalidNode, "head node");
    }

    if (!exists(tail)) {
      GUM_ERROR(InvalidNode, "tail node");
    }

    ArcGraphPart::addArc(tail, head);
  }

  INLINE void DiGraph::clear() {
    ArcGraphPart::clearArcs();
    NodeGraphPart::clearNodes();
  }

  INLINE DiGraph &DiGraph::operator=(const DiGraph &g) {
    // avoid self assigment
    if (this != &g) {
      DiGraph::clear();
      NodeGraphPart::operator=(g);
      ArcGraphPart::operator=(g);
    }

    return *this;
  }

  INLINE void DiGraph::eraseNode(const NodeId id) {
    // warning: to remove the arcs adjacent to id, use the unvirtualized versions
    // of arc removals
    ArcGraphPart::unvirtualizedEraseParents(id);
    ArcGraphPart::unvirtualizedEraseChildren(id);

    NodeGraphPart::eraseNode(id);
  }

  INLINE bool DiGraph::operator==(const DiGraph &p) const {
    return ArcGraphPart::operator==(p) && NodeGraphPart::operator==(p);
  }

  INLINE bool DiGraph::operator!=(const DiGraph &p) const { return !operator==(p); }

} /* namespace gum */
