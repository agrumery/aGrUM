/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) et Christophe GONZALES(_at_AMU)
 * (_at_AMU) info_at_agrum_dot_org
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
 *DAGModel.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/graphicalModels/DAGmodel.h>

namespace gum {
  INLINE
  const DAG& DAGmodel::dag() const { return dag_; }

  INLINE
  Size DAGmodel::size() const { return dag().size(); }

  INLINE
  Size DAGmodel::sizeArcs() const { return dag_.sizeArcs(); }

  INLINE const ArcSet& DAGmodel::arcs() const { return dag_.arcs(); }

  INLINE bool DAGmodel::existsArc(const NodeId tail, const NodeId head) const {
    return dag_.existsArc(tail, head);
  }

  INLINE bool DAGmodel::existsArc(const std::string& nametail, const std::string& namehead) const {
    return existsArc(idFromName(nametail), idFromName(namehead));
  }

  INLINE const NodeSet& DAGmodel::parents(const NodeId id) const { return dag_.parents(id); }

  INLINE const NodeSet& DAGmodel::parents(const std::string& name) const {
    return parents(idFromName(name));
  }

  INLINE NodeSet DAGmodel::family(const NodeId id) const { return dag_.family(id); }

  INLINE NodeSet DAGmodel::family(const std::string& name) const {
    return dag_.family(idFromName(name));
  }

  INLINE const NodeSet& DAGmodel::children(const NodeId id) const { return dag_.children(id); }
  INLINE const NodeSet& DAGmodel::children(const std::string& name) const {
    return dag_.children(idFromName(name));
  }

  INLINE NodeSet DAGmodel::children(const NodeSet& ids) const { return dag_.children(ids); }

  INLINE NodeSet DAGmodel::children(const std::vector< std::string >& names) const {
    return children(nodeset(names));
  }

  INLINE NodeSet DAGmodel::parents(const NodeSet& ids) const { return dag_.children(ids); }

  INLINE NodeSet DAGmodel::parents(const std::vector< std::string >& names) const {
    return parents(nodeset(names));
  }

  INLINE bool DAGmodel::exists(NodeId node) const { return dag_.exists(node); }

  INLINE const NodeGraphPart& DAGmodel::nodes() const { return (NodeGraphPart&)dag_; }

  INLINE Sequence< NodeId > DAGmodel::topologicalOrder() const { return dag().topologicalOrder(); }

  INLINE NodeSet DAGmodel::descendants(const NodeId id) const { return dag().descendants(id); }

  INLINE NodeSet DAGmodel::descendants(const std::string& name) const {
    return descendants(idFromName(name));
  }

  INLINE NodeSet DAGmodel::ancestors(const NodeId id) const { return dag().ancestors(id); }

  INLINE NodeSet DAGmodel::ancestors(const std::string& name) const {
    return ancestors(idFromName(name));
  }


  INLINE UndiGraph
     DAGmodel::moralizedAncestralGraph(const std::vector< std::string >& nodenames) const {
    return moralizedAncestralGraph(nodeset(nodenames));
  }


  INLINE UndiGraph DAGmodel::moralizedAncestralGraph(const NodeSet& nodes) const {
    return dag().moralizedAncestralGraph(nodes);
  }

  INLINE bool DAGmodel::isIndependent(NodeId X, NodeId Y, const NodeSet& Z) const {
    return dag().dSeparation(X, Y, Z);
  }

  INLINE bool DAGmodel::isIndependent(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const {
    return dag().dSeparation(X, Y, Z);
  }
} /* namespace gum */
