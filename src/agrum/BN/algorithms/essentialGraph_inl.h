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
#include <agrum/BN/algorithms/essentialGraph.h>

namespace gum {

  INLINE MixedGraph EssentialGraph::mixedGraph() { return __mg; }

  INLINE const NodeSet& EssentialGraph::parents(const NodeId id) const {
    return __mg.parents(id);
  }

  INLINE const NodeSet& EssentialGraph::children(const NodeId id) const {
    return __mg.children(id);
  }

  INLINE const NodeSet& EssentialGraph::neighbours(const NodeId id) const {
    return __mg.neighbours(id);
  }


  INLINE Size EssentialGraph::sizeArcs() const { return __mg.sizeArcs(); }

  INLINE const ArcSet& EssentialGraph::arcs() const { return __mg.arcs(); }

  INLINE Size EssentialGraph::sizeEdges() const { return __mg.sizeEdges(); }

  INLINE const EdgeSet& EssentialGraph::edges() const { return __mg.edges(); }

  INLINE Size EssentialGraph::sizeNodes() const { return __mg.sizeNodes(); }

  INLINE Size EssentialGraph::size() const { return __mg.size(); }

  INLINE const NodeGraphPart& EssentialGraph::nodes() const {
    return __mg.nodes();
  }
}   // namespace gum
