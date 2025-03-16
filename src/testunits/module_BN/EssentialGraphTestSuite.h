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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
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

#include <agrum/BN/algorithms/essentialGraph.h>
#include <agrum/BN/BayesNet.h>

namespace gum_tests {
  class [[maybe_unused]] EssentialGraphTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Chain) {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c");
      auto eg = gum::EssentialGraph(bn);

      TS_ASSERT_EQUALS(eg.size(), 3u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 0u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 2u)
    }

    GUM_ACTIVE_TEST(Vstructure) {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b;c->b");
      auto eg = gum::EssentialGraph(bn);

      TS_ASSERT_EQUALS(eg.size(), 3u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 2u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 0u)
    }

    GUM_ACTIVE_TEST(CaseD) {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b;c1->b;c2->b;a->c1;a->c2");
      auto eg = gum::EssentialGraph(bn);

      TS_ASSERT_EQUALS(eg.size(), 4u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 3u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 2u)
    }

    GUM_ACTIVE_TEST(Notebook1) {
      auto bn = gum::BayesNet< float >::fastPrototype(
          "A->B->C->D;E->B;F->G->D;F->H->I;E->J->K->I->M;K->L");
      auto eg = gum::EssentialGraph(bn);

      TS_ASSERT_EQUALS(eg.size(), 13u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 8u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 5u)
    }

    GUM_ACTIVE_TEST(Notebook2) {
      auto bn = gum::BayesNet< float >::fastPrototype("A->B;C->B;C->D;B->D;A->C");
      auto eg = gum::EssentialGraph(bn);

      TS_ASSERT_EQUALS(eg.size(), 4u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 0u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 5u)
    }

    GUM_ACTIVE_TEST(Notebook3) {
      auto bn = gum::BayesNet< float >::fastPrototype("Z->X->U;Y->X;Y->W");
      auto eg = gum::EssentialGraph(bn);

      TS_ASSERT_EQUALS(eg.size(), 5u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 3u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 1u)
    }

    GUM_ACTIVE_TEST(Skeleton) {
      auto bn   = gum::BayesNet< float >::fastPrototype("Z->X->U;Y->X;Y->W");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();

      TS_ASSERT_EQUALS(skel.size(), 5u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 1u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 3u)

      TS_ASSERT_EQUALS(eg.idFromName("Z"), 0u)
      TS_ASSERT_EQUALS(eg.idFromName("X"), 1u)
      TS_ASSERT_EQUALS(eg.idFromName("U"), 2u)
      TS_ASSERT_EQUALS(eg.idFromName("Y"), 3u)
      TS_ASSERT_EQUALS(eg.idFromName("W"), 4u)

      TS_ASSERT_EQUALS(eg.nameFromId(0u), "Z")
      TS_ASSERT_EQUALS(eg.nameFromId(1u), "X")
      TS_ASSERT_EQUALS(eg.nameFromId(2u), "U")
      TS_ASSERT_EQUALS(eg.nameFromId(3u), "Y")
      TS_ASSERT_EQUALS(eg.nameFromId(4u), "W")
    }

    GUM_ACTIVE_TEST(NonRegression1) {
      auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2<-0");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();

      TS_ASSERT_EQUALS(skel.size(), 3u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 3u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 0u)
    }

    GUM_ACTIVE_TEST(NonRegression2) {
      auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2<-0;3->1");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();

      TS_ASSERT_EQUALS(skel.size(), 4u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 0u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 4u)
    }

    GUM_ACTIVE_TEST(NonRegression3) {
      auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2->3<-4<-2<-5");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();
      TS_ASSERT_EQUALS(skel.size(), 6u)
      TS_ASSERT_EQUALS(eg.sizeEdges(), 2u)
      TS_ASSERT_EQUALS(eg.sizeArcs(), 4u)
    }
  };

}   // namespace gum_tests
