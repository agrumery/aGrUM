
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
 *IBayesNet.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN (@LIP6)
 */

#include <agrum/tools/core/math/math.h>
#include <agrum/tools/graphicalModels/DAGmodel.h>

namespace gum {
  INLINE
  const DAG& DAGmodel::dag() const { return _dag; }

  INLINE
  Size DAGmodel::size() const { return dag().size(); }

  INLINE
  Size DAGmodel::sizeArcs() const { return _dag.sizeArcs(); }

  INLINE const ArcSet& DAGmodel::arcs() const { return _dag.arcs(); }

  INLINE const NodeSet& DAGmodel::parents(const NodeId id) const {
    return _dag.parents(id);
  }
  INLINE const NodeSet& DAGmodel::parents(const std::string& name) const {
    return parents(idFromName(name));
  }

  INLINE const NodeSet& DAGmodel::children(const NodeId id) const {
    return _dag.children(id);
  }
  INLINE const NodeSet& DAGmodel::children(const std::string& name) const {
    return _dag.children(idFromName(name));
  }

  INLINE const NodeGraphPart& DAGmodel::nodes() const {
    return (NodeGraphPart&)_dag;
  }
} /* namespace gum */
