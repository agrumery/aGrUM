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

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#include <agrum/BN/algorithms/MarkovBlanket.h>
#include <agrum/BN/BayesNet.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MarkovBlanket
#define GUM_CURRENT_MODULE BN

namespace gum_tests {
  struct MarkovBlanketTestSuite {
    public:
    static void testChain() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c");
      {
        auto mb = gum::MarkovBlanket(bn, "a");

        CHECK_EQ(mb.size(), 2u);
        CHECK_EQ(mb.sizeArcs(), 1u);
      }
      {
        auto mb = gum::MarkovBlanket(bn, 0);

        CHECK_EQ(mb.size(), 2u);
        CHECK_EQ(mb.sizeArcs(), 1u);
      }

      {
        auto mb = gum::MarkovBlanket(bn, "b");

        CHECK_EQ(mb.size(), 3u);
        CHECK_EQ(mb.sizeArcs(), 2u);
      }
      {
        auto mb = gum::MarkovBlanket(bn, 1);

        CHECK_EQ(mb.size(), 3u);
        CHECK_EQ(mb.sizeArcs(), 2u);
      }

      {
        auto mb = gum::MarkovBlanket(bn, "c");

        CHECK_EQ(mb.size(), 2u);
        CHECK_EQ(mb.sizeArcs(), 1u);
      }
      {
        auto mb = gum::MarkovBlanket(bn, 2);

        CHECK_EQ(mb.size(), 2u);
        CHECK_EQ(mb.sizeArcs(), 1u);
      }
    }   // namespace gum_tests

    static void testMarkovBlanketStructure() {
      const auto bn = gum::BayesNet< float >::fastPrototype("a->b->c->d->e;f->d->g;h->i->g");
      CHECK(!gum::MarkovBlanket(bn, "a").hasSameStructure(
          gum::BayesNet< float >::fastPrototype("b->a")));

      CHECK(gum::MarkovBlanket(bn, "a").hasSameStructure(
          gum::BayesNet< float >::fastPrototype("a->b")));
      CHECK(gum::MarkovBlanket(bn, "b").hasSameStructure(
          gum::BayesNet< float >::fastPrototype("a->b->c")));
      CHECK(gum::MarkovBlanket(bn, "c").hasSameStructure(
          gum::BayesNet< float >::fastPrototype("b->c->d;f->d")));
      CHECK(gum::MarkovBlanket(bn, "d").hasSameStructure(
          gum::BayesNet< float >::fastPrototype("c->d->e;f->d->g;i->g")));
      CHECK(gum::MarkovBlanket(bn, "e").hasSameStructure(
          gum::BayesNet< float >::fastPrototype("d->e")));
      CHECK(gum::MarkovBlanket(bn, "f").hasSameStructure(
          gum::BayesNet< float >::fastPrototype("c->d;f->d;")));
      CHECK(gum::MarkovBlanket(bn, "g").hasSameStructure(
          gum::BayesNet< float >::fastPrototype("d->g;i->g;")));
      CHECK(gum::MarkovBlanket(bn, "h").hasSameStructure(
          gum::BayesNet< float >::fastPrototype("h->i;")));
      CHECK(gum::MarkovBlanket(bn, "i").hasSameStructure(
          gum::BayesNet< float >::fastPrototype("d->g;h->i->g;;")));
    }

    static void testMarkovBlanketSpecialArcs() {
      const auto bn = gum::BayesNet< float >::fastPrototype(
          "aa->bb->cc->dd->ee;ff->dd->gg;hh->ii->gg;ff->ii;ff->gg");
      const auto mb
          = gum::BayesNet< float >::fastPrototype("cc->dd->ee;ff->dd->gg;ff->gg;ff->ii->gg");
      CHECK(gum::MarkovBlanket(bn, "dd").hasSameStructure(mb));
    }

    static void testMarkovBlanketMultiLevel() {
      const auto bn
          = gum::BayesNet< float >::fastPrototype("Z<-A->B->C->D->E<-Y;X->G<-F<-C<-I<-H->W");
      CHECK_THROWS_AS(gum::MarkovBlanket(bn, "C", 0).size(), const gum::InvalidArgument&);
      CHECK_EQ(gum::MarkovBlanket(bn, "C", 1).size(), static_cast< gum::Size >(5));
      CHECK_EQ(gum::MarkovBlanket(bn, "C", 2).size(), static_cast< gum::Size >(11));
      CHECK_EQ(gum::MarkovBlanket(bn, "C", 3).size(), static_cast< gum::Size >(13));
    }

    static void testAncestorsDescendants() {
      // chain a(0)->b(1)->c(2); MB of b contains all 3 nodes
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c");
      auto mb = gum::MarkovBlanket(bn, "b");

      CHECK_EQ(mb.descendants(0), gum::NodeSet({1, 2}));
      CHECK_EQ(mb.descendants(1), gum::NodeSet({2}));
      CHECK_EQ(mb.descendants(2), gum::NodeSet());
      CHECK_EQ(mb.ancestors(0), gum::NodeSet());
      CHECK_EQ(mb.ancestors(1), gum::NodeSet({0}));
      CHECK_EQ(mb.ancestors(2), gum::NodeSet({0, 1}));
    }
  };

  GUM_TEST_ACTIF(Chain)
  GUM_TEST_ACTIF(MarkovBlanketStructure)
  GUM_TEST_ACTIF(MarkovBlanketSpecialArcs)
  GUM_TEST_ACTIF(MarkovBlanketMultiLevel)
  GUM_TEST_ACTIF(AncestorsDescendants)

}   // namespace gum_tests
