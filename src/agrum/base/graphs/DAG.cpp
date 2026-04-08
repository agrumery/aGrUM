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


/** @file
 * @brief Source implementation of Base classes for directed acyclic graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/algorithms/generic/moralization.h>
#include <agrum/base/graphs/algorithms/generic/separation.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/DAG_inl.h>
#endif   // GU%_NO_INLINE

namespace gum {

  // diamond structure require to explicitly initialize NodeGraphPart
  DAG::DAG(Size nodes_size, bool nodes_resize_policy, Size arcs_size, bool arcs_resize_policy) :
      NodeGraphPart(nodes_size, nodes_resize_policy),
      DiGraph(nodes_size, nodes_resize_policy, arcs_size, arcs_resize_policy) {
    GUM_CONSTRUCTOR(DAG);
  }

  // diamond structure require to explicitly initialize NodeGraphPart
  DAG::DAG(const DAG& g) : NodeGraphPart(g), DiGraph(g) { GUM_CONS_CPY(DAG); }

  // diamond structure require to explicitly initialize NodeGraphPart
  DAG::DAG(DAG&& g) : NodeGraphPart(std::move(g)), DiGraph(std::move(g)) { GUM_CONS_MOV(DAG); }

  DAG::~DAG() { GUM_DESTRUCTOR(DAG); }

  UndiGraph DAG::moralGraph() const { return graph::moralGraph(*this); }

  UndiGraph DAG::moralizedAncestralGraph(const NodeSet& nodes) const {
    return graph::moralizedAncestralGraph(*this, nodes);
  }

  bool DAG::dSeparation(NodeId X, NodeId Y, const NodeSet& Z) const {
    return graph::dSeparated(*this, X, Y, Z);
  }

  bool DAG::dSeparation(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const {
    return graph::dSeparated(*this, X, Y, Z);
  }

  // visit the nodes and add some of node from soids in minimal
  void DAG::_minimalCondSetVisitUp_(NodeId         node,
                                    const NodeSet& soids,
                                    NodeSet&       minimal,
                                    NodeSet&       alreadyVisitedUp,
                                    NodeSet&       alreadyVisitedDn) const {
    if (alreadyVisitedUp.contains(node)) return;
    alreadyVisitedUp << node;

    if (soids.contains(node)) {
      minimal << node;
    } else {
      for (auto fath: parents(node))
        _minimalCondSetVisitUp_(fath, soids, minimal, alreadyVisitedUp, alreadyVisitedDn);
      for (auto chil: children(node))
        _minimalCondSetVisitDn_(chil, soids, minimal, alreadyVisitedUp, alreadyVisitedDn);
    }
  }

  // visit the nodes and add some of node from soids in minimal
  void DAG::_minimalCondSetVisitDn_(NodeId         node,
                                    const NodeSet& soids,
                                    NodeSet&       minimal,
                                    NodeSet&       alreadyVisitedUp,
                                    NodeSet&       alreadyVisitedDn) const {
    if (alreadyVisitedDn.contains(node)) return;
    alreadyVisitedDn << node;

    if (soids.contains(node)) {
      minimal << node;
      for (auto fath: parents(node))
        _minimalCondSetVisitUp_(fath, soids, minimal, alreadyVisitedUp, alreadyVisitedDn);
    } else {
      for (auto chil: children(node))
        _minimalCondSetVisitDn_(chil, soids, minimal, alreadyVisitedUp, alreadyVisitedDn);
    }
  }

  NodeSet DAG::minimalCondSet(NodeId target, const NodeSet& soids) const {
    if (soids.contains(target)) return NodeSet({target});

    NodeSet res;
    NodeSet alreadyVisitedUp;
    NodeSet alreadyVisitedDn;
    alreadyVisitedDn << target;
    alreadyVisitedUp << target;

    for (auto fath: parents(target))
      _minimalCondSetVisitUp_(fath, soids, res, alreadyVisitedUp, alreadyVisitedDn);
    for (auto chil: children(target))
      _minimalCondSetVisitDn_(chil, soids, res, alreadyVisitedUp, alreadyVisitedDn);
    return res;
  }

  NodeSet DAG::minimalCondSet(const NodeSet& targets, const NodeSet& soids) const {
    NodeSet res;
    for (auto node: targets) {
      res += minimalCondSet(node, soids);
    }
    return res;
  }
} /* namespace gum */
