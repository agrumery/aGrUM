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


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/BN/BayesNet.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DAGModel
#define GUM_CURRENT_MODULE BN

namespace gum_tests {
  struct DAGModelTestSuite {
    public:
    static void testEquality() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c;a->c");

      CHECK(bn.hasSameStructure(bn));
      CHECK(bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("a->b->c;a->c")));
      CHECK(!bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("b->a->c;b->c")));
      CHECK(!bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("b->a")));
      CHECK(!bn.hasSameStructure(gum::BayesNet< double >::fastPrototype("b->a")));
      CHECK(!bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("b->a->d")));

      CHECK(bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("a->b[1,5]->c;a->c")));
      CHECK(
          bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("a ->b  [1,5]->c ;  a->c  ")));
      CHECK(bn.hasSameStructure(
          gum::BayesNet< float >::fastPrototype("a ->b  [1,\n5]->c ;\n  a->c  ")));
    }   // namespace gum_tests

    static void testAncestors() {
      auto bn = gum::BayesNet< float >::fastPrototype("A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J");
      CHECK((bn.descendants(6)) == (gum::NodeSet({0, 1, 4, 5})));
      CHECK((bn.descendants("G")) == (gum::NodeSet({0, 1, 4, 5})));

      CHECK((bn.descendants(1)) == (gum::NodeSet()));
      CHECK((bn.descendants("B")) == (gum::NodeSet()));

      CHECK((bn.ancestors(1)) == (gum::NodeSet({0, 2, 6, 8, 9})));
      CHECK((bn.ancestors("B")) == (gum::NodeSet({0, 2, 6, 8, 9})));

      CHECK((bn.ancestors(9)) == (gum::NodeSet()));
      CHECK((bn.ancestors("J")) == (gum::NodeSet()));
    }

    static void testMoralizedAncestralGraph() {
      auto bn = gum::BayesNet< float >::fastPrototype("A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J");

      auto g = bn.moralizedAncestralGraph({"I", "J"});
      CHECK((g.nodes().asNodeSet()) == (gum::NodeSet({8, 9})));
      CHECK((g.edges()) == (gum::EdgeSet({})));

      g = bn.moralizedAncestralGraph({"A"});
      CHECK((g.nodes().asNodeSet()) == (gum::NodeSet({0, 6, 8})));
      CHECK((g.edges()) == (gum::EdgeSet({gum::Edge(0, 6), gum::Edge(6, 8)})));
      // just for the fun
      CHECK((g.edges()) == (gum::EdgeSet({gum::Edge(8, 6), gum::Edge(6, 0)})));

      // V-structure
      g = bn.moralizedAncestralGraph({"C"});
      CHECK((g.nodes().asNodeSet()) == (gum::NodeSet({2, 8, 9})));
      CHECK((g.edges()) == (gum::EdgeSet({gum::Edge(2, 8), gum::Edge(2, 9), gum::Edge(8, 9)})));

      g = bn.moralizedAncestralGraph({"A", "D", "I", "H"});
      CHECK((g.nodes().asNodeSet()) == (gum::NodeSet({0, 2, 3, 6, 7, 8, 9})));
      CHECK((g.edges())
            == (gum::EdgeSet({gum::Edge(2, 8),
                              gum::Edge(2, 9),
                              gum::Edge(8, 9),
                              gum::Edge(2, 3),
                              gum::Edge(3, 7),
                              gum::Edge(0, 6),
                              gum::Edge(6, 8)})));

      g = bn.moralizedAncestralGraph({"F", "B", "E", "H"});
      CHECK((g.nodes().asNodeSet()) == (gum::NodeSet({0, 1, 2, 3, 4, 5, 6, 7, 8, 9})));
      CHECK((g.edges())
            == (gum::EdgeSet({gum::Edge(2, 8),
                              gum::Edge(2, 9),
                              gum::Edge(8, 9),
                              gum::Edge(2, 3),
                              gum::Edge(3, 7),
                              gum::Edge(0, 6),
                              gum::Edge(6, 8),
                              gum::Edge(0, 5),
                              gum::Edge(0, 1),
                              gum::Edge(3, 4),
                              gum::Edge(0, 2),
                              gum::Edge(0, 3),
                              gum::Edge(0, 4),
                              gum::Edge(1, 2)})));
    }

    static void testIndependence() {
      auto bn = gum::BayesNet< float >::fastPrototype("A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J");

      CHECK(bn.isIndependent("I", "J", {}));
      CHECK(!bn.isIndependent("I", "J", {"C"}));
      CHECK(!bn.isIndependent("I", "J", {"H"}));
      CHECK(bn.isIndependent("I", "J", {"F"}));
      CHECK(!bn.isIndependent("I", "J", {"E"}));
      CHECK(!bn.isIndependent("I", "J", {"E", "G"}));
      CHECK(!bn.isIndependent("I", "J", {"E", "G", "H"}));

      CHECK(!bn.isIndependent("I", "H", {}));
      CHECK(bn.isIndependent("I", "H", {"C"}));
      CHECK(bn.isIndependent("I", "H", {"C", "B"}));
      CHECK(!bn.isIndependent("I", "H", {"C", "E"}));
      CHECK(!bn.isIndependent("I", "H", {"C", "E", "B"}));
      CHECK(bn.isIndependent("I", "H", {"C", "E", "B", "G"}));
    }

    static void testMultiIndependence() {
      auto bn = gum::BayesNet< float >::fastPrototype("A->B->C<-F;C->G;D->B->E");

      CHECK_THROWS_AS(bn.isIndependent(std::vector< std::string >{"A", "E"}, {"A", "G"}, {"F"}),
                      const gum::InvalidArgument&);

      // clang-format off
      CHECK( bn.isIndependent(std::vector<std::string>{"A"}        , {"D"}        , {}));
      CHECK(!bn.isIndependent(std::vector<std::string>{"A"}        , {"D"}        , {"B"}));
      CHECK(!bn.isIndependent(std::vector<std::string>{"A","B"}    , {"D"}        , {"B"}));
      CHECK(!bn.isIndependent(std::vector<std::string>{"A"}        , {"D","B"}    , {"B"}));
      CHECK(!bn.isIndependent(std::vector<std::string>{"A"}        , {"D"}        , {"E"}));
      CHECK(!bn.isIndependent(std::vector<std::string>{"A"}        , {"D"}        , {"G"}));
      CHECK(!bn.isIndependent(std::vector<std::string>{"A","E"}    , {"F","G"}    , {}));
      CHECK( bn.isIndependent(std::vector<std::string>{"A","E"}    , {"F","G"}    , {"B"}));
      CHECK(!bn.isIndependent(std::vector<std::string>{"A","E"}    , {"F","G"}    , {"C","D"}));
      CHECK(!bn.isIndependent(std::vector<std::string>{"A","E","C"}, {"F","G"}    , {"C","D"}));
      CHECK(!bn.isIndependent(std::vector<std::string>{"A","E"}    , {"F","G","C"}, {"C","D"}));
      // clang-format on
    }
  };

  GUM_TEST_ACTIF(Equality)
  GUM_TEST_ACTIF(Ancestors)
  GUM_TEST_ACTIF(MoralizedAncestralGraph)
  GUM_TEST_ACTIF(Independence)
  GUM_TEST_ACTIF(MultiIndependence)
}   // namespace gum_tests
