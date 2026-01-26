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

#include <agrum/BN/algorithms/essentialGraph.h>
#include <agrum/BN/BayesNet.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  EssentialGraph
#define GUM_CURRENT_MODULE BN

namespace gum_tests {
  struct EssentialGraphTestSuite {
    public:
    static void testChain() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c");
      auto eg = gum::EssentialGraph(bn);

      CHECK((eg.size()) == (3u));
      CHECK((eg.sizeArcs()) == (0u));
      CHECK((eg.sizeEdges()) == (2u));
    }   // namespace gum_tests

    static void testVstructure() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b;c->b");
      auto eg = gum::EssentialGraph(bn);

      CHECK((eg.size()) == (3u));
      CHECK((eg.sizeArcs()) == (2u));
      CHECK((eg.sizeEdges()) == (0u));
    }

    static void testCaseD() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b;c1->b;c2->b;a->c1;a->c2");
      auto eg = gum::EssentialGraph(bn);

      CHECK((eg.size()) == (4u));
      CHECK((eg.sizeArcs()) == (3u));
      CHECK((eg.sizeEdges()) == (2u));
    }

    static void testNotebook1() {
      auto bn = gum::BayesNet< float >::fastPrototype(
          "A->B->C->D;E->B;F->G->D;F->H->I;E->J->K->I->M;K->L");
      auto eg = gum::EssentialGraph(bn);

      CHECK((eg.size()) == (13u));
      CHECK((eg.sizeArcs()) == (8u));
      CHECK((eg.sizeEdges()) == (5u));
    }

    static void testNotebook2() {
      auto bn = gum::BayesNet< float >::fastPrototype("A->B;C->B;C->D;B->D;A->C");
      auto eg = gum::EssentialGraph(bn);

      CHECK((eg.size()) == (4u));
      CHECK((eg.sizeArcs()) == (0u));
      CHECK((eg.sizeEdges()) == (5u));
    }

    static void testNotebook3() {
      auto bn = gum::BayesNet< float >::fastPrototype("Z->X->U;Y->X;Y->W");
      auto eg = gum::EssentialGraph(bn);

      CHECK((eg.size()) == (5u));
      CHECK((eg.sizeArcs()) == (3u));
      CHECK((eg.sizeEdges()) == (1u));
    }

    static void testSkeleton() {
      auto bn   = gum::BayesNet< float >::fastPrototype("Z->X->U;Y->X;Y->W");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();

      CHECK((skel.size()) == (5u));
      CHECK((eg.sizeEdges()) == (1u));
      CHECK((eg.sizeArcs()) == (3u));

      CHECK((eg.idFromName("Z")) == (0u));
      CHECK((eg.idFromName("X")) == (1u));
      CHECK((eg.idFromName("U")) == (2u));
      CHECK((eg.idFromName("Y")) == (3u));
      CHECK((eg.idFromName("W")) == (4u));

      CHECK((eg.nameFromId(0u)) == ("Z"));
      CHECK((eg.nameFromId(1u)) == ("X"));
      CHECK((eg.nameFromId(2u)) == ("U"));
      CHECK((eg.nameFromId(3u)) == ("Y"));
      CHECK((eg.nameFromId(4u)) == ("W"));
    }

    static void testNonRegression1() {
      auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2<-0");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();

      CHECK((skel.size()) == (3u));
      CHECK((eg.sizeEdges()) == (3u));
      CHECK((eg.sizeArcs()) == (0u));
    }

    static void testNonRegression2() {
      auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2<-0;3->1");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();

      CHECK((skel.size()) == (4u));
      CHECK((eg.sizeEdges()) == (0u));
      CHECK((eg.sizeArcs()) == (4u));
    }

    static void testNonRegression3() {
      auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2->3<-4<-2<-5");
      auto eg   = gum::EssentialGraph(bn);
      auto skel = eg.skeleton();
      CHECK((skel.size()) == (6u));
      CHECK((eg.sizeEdges()) == (2u));
      CHECK((eg.sizeArcs()) == (4u));
    }
  };

  GUM_TEST_ACTIF(Chain)
  GUM_TEST_ACTIF(Vstructure)
  GUM_TEST_ACTIF(CaseD)
  GUM_TEST_ACTIF(Notebook1)
  GUM_TEST_ACTIF(Notebook2)
  GUM_TEST_ACTIF(Notebook3)
  GUM_TEST_ACTIF(Skeleton)
  GUM_TEST_ACTIF(NonRegression1)
  GUM_TEST_ACTIF(NonRegression2)
  GUM_TEST_ACTIF(NonRegression3)

}   // namespace gum_tests
