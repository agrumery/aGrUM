
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


/**
 * @file
 * @brief  Interface-like class encapsulating basic functionalities for a
 *IMarkovNetwork.
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 */

#include <agrum/tools/core/math/math.h>
#include <agrum/tools/graphicalModels/UGmodel.h>

namespace gum {
  INLINE
  const UndiGraph& UGmodel::graph() const { return _graph; }

  INLINE
  Size UGmodel::size() const { return graph().size(); }

  INLINE
  Size UGmodel::sizeEdges() const { return _graph.sizeEdges(); }

  INLINE const EdgeSet& UGmodel::edges() const { return _graph.edges(); }

  INLINE const NodeSet& UGmodel::neighbours(const NodeId id) const {
    return _graph.neighbours(id);
  }
  INLINE const NodeSet& UGmodel::neighbours(const std::string& name) const {
    return neighbours(idFromName(name));
  }

  INLINE const NodeGraphPart& UGmodel::nodes() const {
    return (NodeGraphPart&)_graph;
  }
} /* namespace gum */