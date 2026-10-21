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


#include <string>

#include <agrum/base/graphicalModels/inference/junctionTreeRoots.h>
#include <agrum/BN/BayesNet.h>

#include <testunits/gumtest/AgrumTestSuite.h>

namespace gum_tests {

  struct JunctionTreeRootsTestSuite {};

  GUM_TEST(OneRootPerComponent) {
    // A is binary, every other variable has 5 values
    auto bn = gum::BayesNet< double >::fastPrototype("A->B;C[5]->D[5];D->E[5];F[5]");
    auto id = [&bn](const std::string& name) { return bn.idFromName(name); };

    // component 1: {A,B} (domain 10); component 2: {C,D} (25) -- {D,E} (125); component 3: {F}
    gum::JunctionTree jt;
    const auto        ab = jt.addNode(gum::NodeSet{id("A"), id("B")});
    const auto        cd = jt.addNode(gum::NodeSet{id("C"), id("D")});
    const auto        de = jt.addNode(gum::NodeSet{id("D"), id("E")});
    const auto        f  = jt.addNode(gum::NodeSet{id("F")});
    jt.addEdge(cd, de);

    // without preferred cliques: the smallest clique of each component
    CHECK_EQ(gum::junctionTreeRoots(jt, bn, gum::NodeSet()), gum::NodeSet({ab, cd, f}));

    // a preferred clique wins in its component, even if it is larger
    CHECK_EQ(gum::junctionTreeRoots(jt, bn, gum::NodeSet{de}), gum::NodeSet({ab, de, f}));

    // the empty junction tree has no root
    CHECK(gum::junctionTreeRoots(gum::JunctionTree(), bn, gum::NodeSet()).empty());
  }

  GUM_TEST(RootsOfLongChainAndLargeCliques) {
    // a long chain (no recursion depth issue) of cliques whose domain sizes overflow gum::Size
    const int   n = 20000;
    std::string proto;
    for (int i = 0; i < 30; ++i)
      proto += "X" + std::to_string(i) + "[10000];";
    auto bn = gum::BayesNet< double >::fastPrototype(proto);

    gum::NodeSet large;                        // 10000^30 values
    for (int i = 0; i < 30; ++i)
      large.insert(bn.idFromName("X" + std::to_string(i)));
    gum::NodeSet small{bn.idFromName("X0")};   // 10000 values

    gum::JunctionTree jt;
    gum::NodeId       previous = jt.addNode(large);
    gum::NodeId       smallest = previous;
    for (int i = 1; i < n; ++i) {
      const auto clique = jt.addNode((i == n / 2) ? small : large);
      if (i == n / 2) smallest = clique;
      jt.addEdge(previous, clique);
      previous = clique;
    }
    CHECK_EQ(gum::junctionTreeRoots(jt, bn, gum::NodeSet()), gum::NodeSet{smallest});
  }
}   // namespace gum_tests
