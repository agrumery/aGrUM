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
 * @brief Inline implementation of the class building the essential Graph from a
 * DAGmodel
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

// to ease parsing by IDE
#include <agrum/BN/algorithms/MarkovBlanket.h>

namespace gum {

  INLINE DiGraph MarkovBlanket::mb() { return __mb; }

  INLINE const NodeSet& MarkovBlanket::parents(const NodeId id) const {
    return __mb.parents(id);
  }

  INLINE const NodeSet& MarkovBlanket::children(const NodeId id) const {
    return __mb.children(id);
  }

  INLINE Size MarkovBlanket::sizeArcs() const { return __mb.sizeArcs(); }

  INLINE const ArcSet& MarkovBlanket::arcs() const { return __mb.arcs(); }

  INLINE Size MarkovBlanket::sizeNodes() const { return __mb.sizeNodes(); }

  INLINE Size MarkovBlanket::size() const { return __mb.size(); }

  INLINE const NodeGraphPart& MarkovBlanket::nodes() const { return __mb.nodes(); }
}   // namespace gum
