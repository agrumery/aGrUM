/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 *IMarkovRandomFieldwork.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/graphicalModels/UGmodel.h>

namespace gum {
  INLINE
  const UndiGraph& UGmodel::graph() const { return graph_; }

  INLINE
  Size UGmodel::size() const { return graph().size(); }

  INLINE
  Size UGmodel::sizeEdges() const { return graph_.sizeEdges(); }

  INLINE const EdgeSet& UGmodel::edges() const { return graph_.edges(); }

  INLINE bool UGmodel::existsEdge(const NodeId node1, const NodeId node2) const {
    return graph_.existsEdge(node1, node2);
  }

  INLINE bool UGmodel::existsEdge(const std::string& name1, const std::string& name2) const {
    return existsEdge(idFromName(name1), idFromName(name2));
  }

  INLINE const NodeSet& UGmodel::neighbours(const NodeId id) const { return graph_.neighbours(id); }
  INLINE const NodeSet& UGmodel::neighbours(const std::string& name) const {
    return neighbours(idFromName(name));
  }

  INLINE bool UGmodel::exists(NodeId node) const { return graph_.exists(node); }

  INLINE const NodeGraphPart& UGmodel::nodes() const { return (NodeGraphPart&)graph_; }

  INLINE bool UGmodel::isIndependent(NodeId X, NodeId Y, const NodeSet& Z) const {
    return !graph().hasUndirectedPath(X, Y, Z);
  }

  INLINE bool UGmodel::isIndependent(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const {
    return !graph().hasUndirectedPath(X, Y, Z);
  }

} /* namespace gum */
