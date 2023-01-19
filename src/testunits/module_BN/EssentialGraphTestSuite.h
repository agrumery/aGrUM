/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/algorithms/essentialGraph.h>

namespace gum_tests {
  class [[maybe_unused]] EssentialGraphTestSuite: public CxxTest::TestSuite {
    public:
    GUM_TEST(Chain) {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c");
      auto eg = gum::EssentialGraph(bn);

      TS_ASSERT_EQUALS(eg.size(), 3u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 0u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 2u)
    }

    GUM_TEST(Vstructure) {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b;c->b");
      auto eg = gum::EssentialGraph(bn);
      GUM_TRACE_VAR(eg.pdag());

      TS_ASSERT_EQUALS(eg.size(), 3u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 2u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 0u)
    }
    GUM_TEST(CaseD) {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b;c1->b;c2->b;a->c1;a->c2");
      auto eg = gum::EssentialGraph(bn);

      TS_ASSERT_EQUALS(eg.size(), 4u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 3u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 2u)
    }

    GUM_TEST(Notebook1) {
      auto bn = gum::BayesNet< float >::fastPrototype(
         "A->B->C->D;E->B;F->G->D;F->H->I;E->J->K->I->M;K->L");
      auto eg = gum::EssentialGraph(bn);

      TS_ASSERT_EQUALS(eg.size(), 13u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 8u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 5u)
    }
    GUM_TEST(Notebook2) {
      auto bn = gum::BayesNet< float >::fastPrototype("A->B;C->B;C->D;B->D;A->C");
      auto eg = gum::EssentialGraph(bn);

      TS_ASSERT_EQUALS(eg.size(), 4u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 0u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 5u)
    }
    GUM_TEST(Notebook3) {
      auto bn = gum::BayesNet< float >::fastPrototype("Z->X->U;Y->X;Y->W");
      auto eg = gum::EssentialGraph(bn);

      TS_ASSERT_EQUALS(eg.size(), 5u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 3u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 1u)
    }
    GUM_TEST(Skeleton) {
      auto bn   = gum::BayesNet< float >::fastPrototype("Z->X->U;Y->X;Y->W");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();

      TS_ASSERT_EQUALS(skel.size(), 5u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 1u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 3u)
    }
    GUM_TEST(NonRegression1) {
      auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2<-0");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();

      TS_ASSERT_EQUALS(skel.size(), 3u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 3u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 0u)
    }
    GUM_TEST(NonRegression2) {
      auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2<-0;3->1");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();

      TS_ASSERT_EQUALS(skel.size(), 4u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 0u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 4u)
    }
    GUM_TEST(NonRegression3) {
      auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2->3<-4<-2<-5");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();
      TS_ASSERT_EQUALS(skel.size(), 6u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 2u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 4u)
    }
  };

}   // namespace gum_tests
