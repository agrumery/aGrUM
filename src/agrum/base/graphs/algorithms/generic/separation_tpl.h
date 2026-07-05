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
 *   useful, but WITHOUT ANY WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,      *
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

#include <agrum/base/graphs/algorithms/generic/separation.h>

namespace gum::graph {

  template < GUM_DiGraphable G >
  bool dSeparated(const G& g, NodeId X, NodeId Y, const NodeSet& Z) {
    NodeSet qX;
    qX.insert(X);
    return !graph::dConnected(g, qX, Z).exists(Y);
  }

  template < GUM_DiGraphable G >
  bool dSeparated(const G& g, const NodeSet& X, const NodeSet& Y, const NodeSet& Z) {
    if (!(X * Y).empty()) GUM_ERROR(InvalidArgument, "NodeSets X and Y must be disjoint.")
    return (graph::dConnected(g, X, Z) * Y).empty();
  }

  template < GUM_MixedGraphable G >
  bool cSeparated(const G& g, NodeId X, NodeId Y, const NodeSet& Z) {
    NodeSet query{Z};
    query.insert(X);
    query.insert(Y);
    auto moral = moralizedAncestralGraph(g, query);
    for (const auto node: Z)
      if (moral.existsNode(node)) moral.eraseNode(node);
    return !moral.hasUndirectedPath(X, Y);
  }

  template < GUM_MixedGraphable G >
  bool cSeparated(const G& g, const NodeSet& X, const NodeSet& Y, const NodeSet& Z) {
    if (!(X * Y).empty()) GUM_ERROR(InvalidArgument, "NodeSets X and Y must be disjoint.")

    NodeSet query{Z};
    query += X;
    query += Y;
    auto moral = moralizedAncestralGraph(g, query);
    for (const auto node: Z)
      if (moral.existsNode(node)) moral.eraseNode(node);

    const auto cc = moral.chainComponents();

    NodeSet Xcc, Ycc;
    for (const auto node: X)
      if (moral.existsNode(node)) Xcc.insert(cc[node]);
    for (const auto node: Y)
      if (moral.existsNode(node)) Ycc.insert(cc[node]);

    return (Xcc * Ycc).empty();
  }

}   // namespace gum::graph
