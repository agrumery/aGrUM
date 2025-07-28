/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/BN/BayesNet.h>

namespace gum_tests {
  class [[maybe_unused]] DAGModelTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Equality) {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c;a->c");

      TS_ASSERT(bn.hasSameStructure(bn))
      TS_ASSERT(bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("a->b->c;a->c")))
      TS_ASSERT(!bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("b->a->c;b->c")))
      TS_ASSERT(!bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("b->a")))
      TS_ASSERT(!bn.hasSameStructure(gum::BayesNet< double >::fastPrototype("b->a")))
      TS_ASSERT(!bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("b->a->d")))

      TS_ASSERT(bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("a->b[1,5]->c;a->c")));
      TS_ASSERT(
          bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("a ->b  [1,5]->c ;  a->c  ")));
      TS_ASSERT(bn.hasSameStructure(
          gum::BayesNet< float >::fastPrototype("a ->b  [1,\n5]->c ;\n  a->c  ")));
    }

    GUM_ACTIVE_TEST(Ancestors) {
      auto bn = gum::BayesNet< float >::fastPrototype("A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J");
      TS_ASSERT_EQUALS(bn.descendants(6), gum::NodeSet({0, 1, 4, 5}))
      TS_ASSERT_EQUALS(bn.descendants("G"), gum::NodeSet({0, 1, 4, 5}))

      TS_ASSERT_EQUALS(bn.descendants(1), gum::NodeSet())
      TS_ASSERT_EQUALS(bn.descendants("B"), gum::NodeSet())

      TS_ASSERT_EQUALS(bn.ancestors(1), gum::NodeSet({0, 2, 6, 8, 9}))
      TS_ASSERT_EQUALS(bn.ancestors("B"), gum::NodeSet({0, 2, 6, 8, 9}))

      TS_ASSERT_EQUALS(bn.ancestors(9), gum::NodeSet())
      TS_ASSERT_EQUALS(bn.ancestors("J"), gum::NodeSet())
    }

    GUM_ACTIVE_TEST(MoralizedAncestralGraph) {
      auto bn = gum::BayesNet< float >::fastPrototype("A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J");

      auto g = bn.moralizedAncestralGraph({"I", "J"});
      TS_ASSERT_EQUALS(g.nodes().asNodeSet(), gum::NodeSet({8, 9}))
      TS_ASSERT_EQUALS(g.edges(), gum::EdgeSet({}))

      g = bn.moralizedAncestralGraph({"A"});
      TS_ASSERT_EQUALS(g.nodes().asNodeSet(), gum::NodeSet({0, 6, 8}))
      TS_ASSERT_EQUALS(g.edges(), gum::EdgeSet({gum::Edge(0, 6), gum::Edge(6, 8)}))
      // just for the fun
      TS_ASSERT_EQUALS(g.edges(), gum::EdgeSet({gum::Edge(8, 6), gum::Edge(6, 0)}))

      // V-structure
      g = bn.moralizedAncestralGraph({"C"});
      TS_ASSERT_EQUALS(g.nodes().asNodeSet(), gum::NodeSet({2, 8, 9}))
      TS_ASSERT_EQUALS(g.edges(),
                       gum::EdgeSet({gum::Edge(2, 8), gum::Edge(2, 9), gum::Edge(8, 9)}));

      g = bn.moralizedAncestralGraph({"A", "D", "I", "H"});
      TS_ASSERT_EQUALS(g.nodes().asNodeSet(), gum::NodeSet({0, 2, 3, 6, 7, 8, 9}))
      TS_ASSERT_EQUALS(g.edges(),
                       gum::EdgeSet({gum::Edge(2, 8),
                                     gum::Edge(2, 9),
                                     gum::Edge(8, 9),
                                     gum::Edge(2, 3),
                                     gum::Edge(3, 7),
                                     gum::Edge(0, 6),
                                     gum::Edge(6, 8)}));

      g = bn.moralizedAncestralGraph({"F", "B", "E", "H"});
      TS_ASSERT_EQUALS(g.nodes().asNodeSet(), gum::NodeSet({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}))
      TS_ASSERT_EQUALS(g.edges(),
                       gum::EdgeSet({gum::Edge(2, 8),
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
                                     gum::Edge(1, 2)}));
    }

    GUM_ACTIVE_TEST(Independence) {
      auto bn = gum::BayesNet< float >::fastPrototype("A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J");

      TS_ASSERT(bn.isIndependent("I", "J", {}))
      TS_ASSERT(!bn.isIndependent("I", "J", {"C"}))
      TS_ASSERT(!bn.isIndependent("I", "J", {"H"}))
      TS_ASSERT(bn.isIndependent("I", "J", {"F"}))
      TS_ASSERT(!bn.isIndependent("I", "J", {"E"}))
      TS_ASSERT(!bn.isIndependent("I", "J", {"E", "G"}))
      TS_ASSERT(!bn.isIndependent("I", "J", {"E", "G", "H"}))

      TS_ASSERT(!bn.isIndependent("I", "H", {}))
      TS_ASSERT(bn.isIndependent("I", "H", {"C"}))
      TS_ASSERT(bn.isIndependent("I", "H", {"C", "B"}))
      TS_ASSERT(!bn.isIndependent("I", "H", {"C", "E"}))
      TS_ASSERT(!bn.isIndependent("I", "H", {"C", "E", "B"}))
      TS_ASSERT(bn.isIndependent("I", "H", {"C", "E", "B", "G"}))
    }

    GUM_ACTIVE_TEST(MultiIndependence) {
      auto bn = gum::BayesNet< float >::fastPrototype("A->B->C<-F;C->G;D->B->E");

      TS_ASSERT_THROWS(bn.isIndependent(std::vector< std::string >{"A", "E"}, {"A", "G"}, {"F"}),
                       const gum::InvalidArgument&)

      // clang-format off
      TS_ASSERT( bn.isIndependent(std::vector<std::string>{"A"}        , {"D"}        , {}))
      TS_ASSERT(!bn.isIndependent(std::vector<std::string>{"A"}        , {"D"}        , {"B"}))
      TS_ASSERT(!bn.isIndependent(std::vector<std::string>{"A","B"}    , {"D"}        , {"B"}))
      TS_ASSERT(!bn.isIndependent(std::vector<std::string>{"A"}        , {"D","B"}    , {"B"}))
      TS_ASSERT(!bn.isIndependent(std::vector<std::string>{"A"}        , {"D"}        , {"E"}))
      TS_ASSERT(!bn.isIndependent(std::vector<std::string>{"A"}        , {"D"}        , {"G"}))
      TS_ASSERT(!bn.isIndependent(std::vector<std::string>{"A","E"}    , {"F","G"}    , {}))
      TS_ASSERT( bn.isIndependent(std::vector<std::string>{"A","E"}    , {"F","G"}    , {"B"}))
      TS_ASSERT(!bn.isIndependent(std::vector<std::string>{"A","E"}    , {"F","G"}    , {"C","D"}))
      TS_ASSERT(!bn.isIndependent(std::vector<std::string>{"A","E","C"}, {"F","G"}    , {"C","D"}))
      TS_ASSERT(!bn.isIndependent(std::vector<std::string>{"A","E"}    , {"F","G","C"}, {"C","D"}))
      // clang-format on
    }
  };
}   // namespace gum_tests
