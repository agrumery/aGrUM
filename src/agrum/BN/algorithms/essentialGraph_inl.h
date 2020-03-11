
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
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
