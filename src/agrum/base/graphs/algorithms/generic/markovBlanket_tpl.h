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

#include <agrum/base/graphs/algorithms/generic/markovBlanket.h>

namespace gum::graph {

  template < GUM_DiGraphable G >
  DAG markovBlanket(const G& g, NodeId node, int level) {
    if (!g.exists(node))
      GUM_ERROR(InvalidArgument, "Node " << node << " does not exist in the graph.")
    if (level < 1) GUM_ERROR(InvalidArgument, "level must be >= 1")

    DAG mb;

    auto step = [&](NodeId n, NodeSet& next_frontier) {
      for (const auto par: g.parents(n)) {
        if (!mb.existsNode(par)) {
          mb.addNodeWithId(par);
          next_frontier.insert(par);
        }
        if (!mb.existsArc(par, n)) mb.addArc(par, n);
      }
      for (const auto chi: g.children(n)) {
        if (!mb.existsNode(chi)) {
          mb.addNodeWithId(chi);
          next_frontier.insert(chi);
        }
        if (!mb.existsArc(n, chi)) mb.addArc(n, chi);
        for (const auto opar: g.parents(chi)) {
          if (opar == n) continue;
          if (!mb.existsNode(opar)) {
            mb.addNodeWithId(opar);
            next_frontier.insert(opar);
          }
          if (!mb.existsArc(opar, chi)) mb.addArc(opar, chi);
        }
      }
    };

    mb.addNodeWithId(node);
    NodeSet frontier;
    step(node, frontier);

    for (int lv = 1; lv < level && !frontier.empty(); ++lv) {
      NodeSet next;
      for (const auto n: frontier)
        step(n, next);
      frontier = std::move(next);
    }

    return mb;
  }

}   // namespace gum::graph
