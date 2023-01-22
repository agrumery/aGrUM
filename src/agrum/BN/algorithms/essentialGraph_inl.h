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


/** @file
 * @brief Inline implementation of the class building the essential Graph from a
 * DAGmodel
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

// to ease parsing by IDE
#include <agrum/BN/algorithms/essentialGraph.h>

namespace gum {

  INLINE PDAG EssentialGraph::pdag() const { return _pdag_; }

  INLINE const NodeSet& EssentialGraph::parents(NodeId id) const { return _pdag_.parents(id); }

  INLINE const NodeSet& EssentialGraph::children(NodeId id) const { return _pdag_.children(id); }

  INLINE NodeSet EssentialGraph::parents(const NodeSet& ids) const { return _pdag_.parents(ids); }

  INLINE NodeSet EssentialGraph::children(const NodeSet& ids) const { return _pdag_.children(ids); }

  INLINE const NodeSet& EssentialGraph::neighbours(NodeId id) const {
    return _pdag_.neighbours(id);
  }


  INLINE Size EssentialGraph::sizeArcs() const { return _pdag_.sizeArcs(); }

  INLINE const ArcSet& EssentialGraph::arcs() const { return _pdag_.arcs(); }

  INLINE Size EssentialGraph::sizeEdges() const { return _pdag_.sizeEdges(); }

  INLINE const EdgeSet& EssentialGraph::edges() const { return _pdag_.edges(); }

  INLINE Size EssentialGraph::sizeNodes() const { return _pdag_.sizeNodes(); }

  INLINE Size EssentialGraph::size() const { return _pdag_.size(); }

  INLINE const NodeGraphPart& EssentialGraph::nodes() const { return _pdag_.nodes(); }
}   // namespace gum
