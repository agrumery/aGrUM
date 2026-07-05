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

#include <agrum/base/graphs/algorithms/generic/minimalConditioningSet.h>

namespace gum::graph {

  template < GUM_DiGraphable G >
  void _mcsVisitUp_(const G&       g,
                    NodeId         node,
                    const NodeSet& soids,
                    NodeSet&       minimal,
                    NodeSet&       visitedUp,
                    NodeSet&       visitedDn) {
    if (visitedUp.contains(node)) return;
    visitedUp << node;

    if (soids.contains(node)) {
      minimal << node;
    } else {
      for (const auto par: g.parents(node))
        _mcsVisitUp_(g, par, soids, minimal, visitedUp, visitedDn);
      for (const auto chi: g.children(node))
        _mcsVisitDn_(g, chi, soids, minimal, visitedUp, visitedDn);
    }
  }

  template < GUM_DiGraphable G >
  void _mcsVisitDn_(const G&       g,
                    NodeId         node,
                    const NodeSet& soids,
                    NodeSet&       minimal,
                    NodeSet&       visitedUp,
                    NodeSet&       visitedDn) {
    if (visitedDn.contains(node)) return;
    visitedDn << node;

    if (soids.contains(node)) {
      minimal << node;
      for (const auto par: g.parents(node))
        _mcsVisitUp_(g, par, soids, minimal, visitedUp, visitedDn);
    } else {
      for (const auto chi: g.children(node))
        _mcsVisitDn_(g, chi, soids, minimal, visitedUp, visitedDn);
    }
  }

  template < GUM_DiGraphable G >
  NodeSet minimalCondSet(const G& g, NodeId target, const NodeSet& soids) {
    if (soids.contains(target)) return NodeSet({target});

    NodeSet res, visitedUp, visitedDn;
    visitedUp << target;
    visitedDn << target;

    for (const auto par: g.parents(target))
      _mcsVisitUp_(g, par, soids, res, visitedUp, visitedDn);
    for (const auto chi: g.children(target))
      _mcsVisitDn_(g, chi, soids, res, visitedUp, visitedDn);

    return res;
  }

  template < GUM_DiGraphable G >
  NodeSet minimalCondSet(const G& g, const NodeSet& targets, const NodeSet& soids) {
    NodeSet res;
    for (const auto node: targets)
      res += minimalCondSet(g, node, soids);
    return res;
  }

}   // namespace gum::graph
