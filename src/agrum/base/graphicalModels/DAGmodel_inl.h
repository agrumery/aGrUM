/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/**
 * @file
 * @brief  Interface-like class encapsulating basic functionalities for a
 *DAGModel.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/graphicalModels/DAGmodel.h>

#include <agrum/base/core/math/math_utils.h>

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

  INLINE bool DAGmodel::exists(const std::string& name) const {
    try {
      return exists(idFromName(name));
    } catch ([[maybe_unused]] gum::NotFound& e) { return false; }
  }

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

  INLINE NodeSet DAGmodel::minimalCondSet(NodeId target, const NodeSet& soids) const {
    return dag_.minimalCondSet(target, soids);
  }

  INLINE NodeSet DAGmodel::minimalCondSet(const NodeSet& targets, const NodeSet& soids) const {
    return dag_.minimalCondSet(targets, soids);
  }

  INLINE NodeSet DAGmodel::minimalCondSet(const std::string&                target,
                                          const std::vector< std::string >& soids) const {
    return dag_.minimalCondSet(idFromName(target), nodeset(soids));
  }

  INLINE NodeSet DAGmodel::minimalCondSet(const std::vector< std::string >& targets,
                                          const std::vector< std::string >& soids) const {
    return dag_.minimalCondSet(nodeset(targets), nodeset(soids));
  }


} /* namespace gum */
