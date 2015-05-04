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
 * @brief Inline implementation of Base classes for mixed graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

namespace gum {

  INLINE MixedGraph &MixedGraph::operator=(const MixedGraph &g) {
    // avoid self assigment
    if (this != &g) {
      // remove the old graph properly
      EdgeGraphPart::clearEdges();
      ArcGraphPart::clearArcs();
      NodeGraphPart::clearNodes();

      // fill the new graph
      NodeGraphPart::operator=(g);
      EdgeGraphPart::operator=(g);
      ArcGraphPart::operator=(g);
    }

    return *this;
  }

  INLINE void MixedGraph::clear() {
    EdgeGraphPart::clearEdges();
    ArcGraphPart::clearArcs();
    NodeGraphPart::clearNodes();
  }

  INLINE void MixedGraph::eraseNode(const NodeId id) {
    EdgeGraphPart::unvirtualizedEraseNeighbours(id);
    ArcGraphPart::unvirtualizedEraseParents(id);
    ArcGraphPart::unvirtualizedEraseChildren(id);
    NodeGraphPart::eraseNode(id);
  }

  INLINE bool MixedGraph::operator==(const MixedGraph &p) const {
    return ArcGraphPart::operator==(p) && EdgeGraphPart::operator==(p) &&
           NodeGraphPart::operator==(p);
  }

  INLINE bool MixedGraph::operator!=(const MixedGraph &p) const {
    return !operator==(p);
  }

} /* namespace gum */
