
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Inline implementation of the class building the essential Graph from a
 * DAGmodel
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */

// to ease parsing by IDE
#include <agrum/BN/algorithms/essentialGraph.h>

namespace gum {

  INLINE MixedGraph EssentialGraph::mixedGraph() { return mg__; }

  INLINE const NodeSet& EssentialGraph::parents(const NodeId id) const {
    return mg__.parents(id);
  }

  INLINE const NodeSet& EssentialGraph::children(const NodeId id) const {
    return mg__.children(id);
  }

  INLINE const NodeSet& EssentialGraph::neighbours(const NodeId id) const {
    return mg__.neighbours(id);
  }


  INLINE Size EssentialGraph::sizeArcs() const { return mg__.sizeArcs(); }

  INLINE const ArcSet& EssentialGraph::arcs() const { return mg__.arcs(); }

  INLINE Size EssentialGraph::sizeEdges() const { return mg__.sizeEdges(); }

  INLINE const EdgeSet& EssentialGraph::edges() const { return mg__.edges(); }

  INLINE Size EssentialGraph::sizeNodes() const { return mg__.sizeNodes(); }

  INLINE Size EssentialGraph::size() const { return mg__.size(); }

  INLINE const NodeGraphPart& EssentialGraph::nodes() const {
    return mg__.nodes();
  }
}   // namespace gum
