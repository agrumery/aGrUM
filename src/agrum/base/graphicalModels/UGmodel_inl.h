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
 *IMarkovRandomFieldwork.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/base/graphicalModels/UGmodel.h>

#include <agrum/base/core/math/math_utils.h>

namespace gum {
  INLINE
  const UndiGraph& UGmodel::internalGraph() const { return graph_; }

  INLINE
  UndiGraph UGmodel::graph() const {
    UndiGraph g = graph_;
    _nameNodes_(g);
    return g;
  }

  INLINE
  Size UGmodel::size() const { return graph_.size(); }

  INLINE
  Size UGmodel::sizeEdges() const { return graph_.sizeEdges(); }

  INLINE const EdgeSet& UGmodel::edges() const { return graph_.edges(); }

  INLINE bool UGmodel::existsEdge(const NodeId node1, const NodeId node2) const {
    return graph_.existsEdge(node1, node2);
  }

  INLINE bool UGmodel::existsEdge(std::string_view name1, std::string_view name2) const {
    return existsEdge(idFromName(name1), idFromName(name2));
  }

  INLINE const NodeSet& UGmodel::neighbours(const NodeId id) const { return graph_.neighbours(id); }

  INLINE const NodeSet& UGmodel::neighbours(std::string_view name) const {
    return neighbours(idFromName(name));
  }

  INLINE bool UGmodel::exists(NodeId node) const { return graph_.exists(node); }

  INLINE bool UGmodel::exists(std::string_view name) const {
    try {
      return exists(idFromName(name));
    } catch ([[maybe_unused]] gum::NotFound& e) { return false; }
  }

  INLINE const NodeGraphPart& UGmodel::nodes() const {
    return static_cast< const NodeGraphPart& >(graph_);
  }

  INLINE bool UGmodel::isIndependent(NodeId X, NodeId Y, const NodeSet& Z) const {
    return !graph_.hasUndirectedPath(X, Y, Z);
  }

  INLINE bool UGmodel::isIndependent(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const {
    return !graph_.hasUndirectedPath(X, Y, Z);
  }

  INLINE NodeProperty< NodeId > UGmodel::connectedComponents() const {
    return graph_.connectedComponents();
  }

  INLINE NodeSet UGmodel::family(const NodeId id) const {
    // family is the set of the note and its parent. So for a completely undirected model, it is
    // just the node.
    return gum::NodeSet{id};
  }

  INLINE NodeSet UGmodel::family(std::string_view name) const { return family(idFromName(name)); }
} /* namespace gum */
