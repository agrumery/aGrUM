
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>


namespace gum_tests {
  class DAGModelTestSuite: public CxxTest::TestSuite {
    public:
    void testEquality() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c;a->c");

      TS_ASSERT(bn.hasSameStructure(bn));
      TS_ASSERT(bn.hasSameStructure(
         gum::BayesNet< float >::fastPrototype("a->b->c;a->c")));
      TS_ASSERT(!bn.hasSameStructure(
         gum::BayesNet< float >::fastPrototype("b->a->c;b->c")));
      TS_ASSERT(
         !bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("b->a")));
      TS_ASSERT(
         !bn.hasSameStructure(gum::BayesNet< double >::fastPrototype("b->a")));
      TS_ASSERT(
         !bn.hasSameStructure(gum::BayesNet< float >::fastPrototype("b->a->d")));
    }

    void testAncestors() {
      auto bn = gum::BayesNet< float >::fastPrototype(
         "A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J");
      TS_ASSERT_EQUALS(bn.descendants(6), gum::NodeSet({0, 1, 4, 5}));
      TS_ASSERT_EQUALS(bn.descendants("G"), gum::NodeSet({0, 1, 4, 5}));

      TS_ASSERT_EQUALS(bn.descendants(1), gum::NodeSet());
      TS_ASSERT_EQUALS(bn.descendants("B"), gum::NodeSet());

      TS_ASSERT_EQUALS(bn.ancestors(1), gum::NodeSet({0, 2, 6, 8, 9}));
      TS_ASSERT_EQUALS(bn.ancestors("B"), gum::NodeSet({0, 2, 6, 8, 9}));

      TS_ASSERT_EQUALS(bn.ancestors(9), gum::NodeSet());
      TS_ASSERT_EQUALS(bn.ancestors("J"), gum::NodeSet());
    }

    void testMoralizedAncestralGraph() {
      auto bn = gum::BayesNet< float >::fastPrototype(
         "A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J");

      auto g = bn.moralizedAncestralGraph({"I", "J"});
      TS_ASSERT_EQUALS(g.nodes().asNodeSet(), gum::NodeSet({8, 9}));
      TS_ASSERT_EQUALS(g.edges(), gum::EdgeSet({}));

      g = bn.moralizedAncestralGraph({"A"});
      TS_ASSERT_EQUALS(g.nodes().asNodeSet(), gum::NodeSet({0, 6, 8}));
      TS_ASSERT_EQUALS(g.edges(),
                       gum::EdgeSet({gum::Edge(0, 6), gum::Edge(6, 8)}));
      // just for the fun
      TS_ASSERT_EQUALS(g.edges(),
                       gum::EdgeSet({gum::Edge(8, 6), gum::Edge(6, 0)}));

      // V-structure
      g = bn.moralizedAncestralGraph({"C"});
      TS_ASSERT_EQUALS(g.nodes().asNodeSet(), gum::NodeSet({2, 8, 9}));
      TS_ASSERT_EQUALS(
         g.edges(),
         gum::EdgeSet({gum::Edge(2, 8), gum::Edge(2, 9), gum::Edge(8, 9)}));

      g = bn.moralizedAncestralGraph({"A", "D", "I", "H"});
      TS_ASSERT_EQUALS(g.nodes().asNodeSet(), gum::NodeSet({0, 2, 3, 6, 7, 8, 9}));
      TS_ASSERT_EQUALS(g.edges(),
                       gum::EdgeSet({gum::Edge(2, 8),
                                     gum::Edge(2, 9),
                                     gum::Edge(8, 9),
                                     gum::Edge(2, 3),
                                     gum::Edge(3, 7),
                                     gum::Edge(0, 6),
                                     gum::Edge(6, 8)}));

      g = bn.moralizedAncestralGraph({"F", "B", "E", "H"});
      TS_ASSERT_EQUALS(g.nodes().asNodeSet(),
                       gum::NodeSet({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
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

    void testIndependence() {
      auto bn = gum::BayesNet< float >::fastPrototype(
         "A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J");

      TS_ASSERT(bn.isIndependent("I", "J", {}));
      TS_ASSERT(!bn.isIndependent("I", "J", {"C"}));
      TS_ASSERT(!bn.isIndependent("I", "J", {"H"}));
      TS_ASSERT(bn.isIndependent("I", "J", {"F"}));
      TS_ASSERT(!bn.isIndependent("I", "J", {"E"}));
      TS_ASSERT(!bn.isIndependent("I", "J", {"E", "G"}));
      TS_ASSERT(!bn.isIndependent("I", "J", {"E", "G", "H"}));

      TS_ASSERT(!bn.isIndependent("I", "H", {}));
      TS_ASSERT(bn.isIndependent("I", "H", {"C"}));
      TS_ASSERT(bn.isIndependent("I", "H", {"C","B"}));
      TS_ASSERT(!bn.isIndependent("I", "H", {"C","E"}));
      TS_ASSERT(!bn.isIndependent("I", "H", {"C","E","B"}));
      TS_ASSERT(bn.isIndependent("I", "H", {"C","E","B","G"}));
    }
  };
}   // namespace gum_tests
