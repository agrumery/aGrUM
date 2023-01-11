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
#include <agrum/BN/algorithms/MarkovBlanket.h>

namespace gum {

  INLINE DAG MarkovBlanket::dag() const { return _mb_; }

  INLINE const NodeSet& MarkovBlanket::parents(NodeId id) const { return _mb_.parents(id); }

  INLINE const NodeSet& MarkovBlanket::children(NodeId id) const { return _mb_.children(id); }

  INLINE NodeSet MarkovBlanket::parents(const NodeSet& ids) const { return _mb_.parents(ids); }

  INLINE NodeSet MarkovBlanket::children(const NodeSet& ids) const { return _mb_.children(ids); }

  INLINE Size MarkovBlanket::sizeArcs() const { return _mb_.sizeArcs(); }

  INLINE const ArcSet& MarkovBlanket::arcs() const { return _mb_.arcs(); }

  INLINE Size MarkovBlanket::sizeNodes() const { return _mb_.sizeNodes(); }

  INLINE Size MarkovBlanket::size() const { return _mb_.size(); }

  INLINE const NodeGraphPart& MarkovBlanket::nodes() const { return _mb_.nodes(); }
}   // namespace gum
