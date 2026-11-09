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


#ifndef TESTSUITE_RANDOM_GRAPHS_H
#define TESTSUITE_RANDOM_GRAPHS_H

#include <utility>
#include <vector>

#include <agrum/base/graphs/DAG.h>

#include <agrum/base/core/utils_random.h>

namespace gum_tests {

  /// a random order of the nodes 0..n-1
  inline std::vector< gum::NodeId > randomOrder(gum::Size n) {
    std::vector< gum::NodeId > order;
    for (gum::NodeId i = 0; i < n; ++i)
      order.push_back(i);
    for (gum::Size i = n; i > 1; --i)
      std::swap(order[i - 1], order[gum::randomValue(i)]);
    return order;
  }

  /// random DAG: the arcs follow a random order of the nodes
  inline gum::DAG randomDAG(gum::Size n, double density) {
    const auto order = randomOrder(n);
    gum::DAG   dag;
    dag.addNodes(n);
    for (gum::Size i = 0; i < n; ++i)
      for (gum::Size j = i + 1; j < n; ++j)
        if (gum::randomProba() < density) dag.addArc(order[i], order[j]);
    return dag;
  }

  /// random subset of the nodes of dag, without x and y
  inline gum::NodeSet randomSet(const gum::DAG& dag, gum::NodeId x, gum::NodeId y) {
    gum::NodeSet z;
    for (const auto node: dag.nodes())
      if (node != x && node != y && gum::randomProba() < 0.3) z.insert(node);
    return z;
  }

}   // namespace gum_tests

#endif   // TESTSUITE_RANDOM_GRAPHS_H
