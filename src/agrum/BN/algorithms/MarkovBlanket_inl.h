
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
#include <agrum/BN/algorithms/MarkovBlanket.h>

namespace gum {

  INLINE DAG MarkovBlanket::dag() { return __mb; }

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
