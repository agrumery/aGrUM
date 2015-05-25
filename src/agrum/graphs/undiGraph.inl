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
 *   GNU General Public License for more deseconds.                        *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief Inline implementation of Base classes for undirected graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

// to ease parser
#include <agrum/graphs/undiGraph.h>

namespace gum {

  INLINE void UndiGraph::insertEdge(const NodeId first, const NodeId second) {
    addEdge(first, second);
  }
  INLINE void UndiGraph::addEdge(const NodeId first, const NodeId second) {
    if (!exists(first)) {
      GUM_ERROR(InvalidNode, "first node");
    }

    if (!exists(second)) {
      GUM_ERROR(InvalidNode, "second node");
    }

    EdgeGraphPart::addEdge(second, first);
  }

  INLINE void UndiGraph::clear() {
    EdgeGraphPart::clearEdges();
    NodeGraphPart::clearNodes();
  }

  INLINE UndiGraph &UndiGraph::operator=(const UndiGraph &g) {
    if (this != &g) {
      UndiGraph::clear();
      NodeGraphPart::operator=(g);
      EdgeGraphPart::operator=(g);
    }

    return *this;
  }

  INLINE void UndiGraph::eraseNode(const NodeId id) {
    // warning: to remove the edges adjacent to id, use the unvirtualized versions
    // of edge removals
    EdgeGraphPart::unvirtualizedEraseNeighbours(id);
    NodeGraphPart::eraseNode(id);
  }

  INLINE bool UndiGraph::operator==(const UndiGraph &p) const {
    return EdgeGraphPart::operator==(p) && NodeGraphPart::operator==(p);
  }

  INLINE bool UndiGraph::operator!=(const UndiGraph &p) const {
    return !operator==(p);
  }

} /* namespace gum */
