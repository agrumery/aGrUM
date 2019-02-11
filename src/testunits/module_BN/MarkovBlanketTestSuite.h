
/***************************************************************************
 *   Copyright (C) 2017 by Pierre-Henri WUILLEMIN and Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/algorithms/MarkovBlanket.h>

namespace gum_tests {
  class MarkovBlanketTestSuite : public CxxTest::TestSuite {
    public:
    void testChain() {
      auto bn = gum::BayesNet< int >::fastPrototype("a->b->c");
      {
        auto mb = gum::MarkovBlanket(bn, "a");

        TS_ASSERT_EQUALS(mb.size(), 2u);
        TS_ASSERT_EQUALS(mb.sizeArcs(), 1u);
      }
      {
        auto mb = gum::MarkovBlanket(bn, 0);

        TS_ASSERT_EQUALS(mb.size(), 2u);
        TS_ASSERT_EQUALS(mb.sizeArcs(), 1u);
      }

      {
        auto mb = gum::MarkovBlanket(bn, "b");

        TS_ASSERT_EQUALS(mb.size(), 3u);
        TS_ASSERT_EQUALS(mb.sizeArcs(), 2u);
      }
      {
        auto mb = gum::MarkovBlanket(bn, 1);

        TS_ASSERT_EQUALS(mb.size(), 3u);
        TS_ASSERT_EQUALS(mb.sizeArcs(), 2u);
      }

      {
        auto mb = gum::MarkovBlanket(bn, "c");

        TS_ASSERT_EQUALS(mb.size(), 2u);
        TS_ASSERT_EQUALS(mb.sizeArcs(), 1u);
      }
      {
        auto mb = gum::MarkovBlanket(bn, 2);

        TS_ASSERT_EQUALS(mb.size(), 2u);
        TS_ASSERT_EQUALS(mb.sizeArcs(), 1u);
      }
    }

    void testMarkovBlanketStructure() {
      const auto bn =
         gum::BayesNet< int >::fastPrototype("a->b->c->d->e;f->d->g;h->i->g");
      TS_ASSERT(!gum::MarkovBlanket(bn, "a").hasSameStructure(
         gum::BayesNet< int >::fastPrototype("b->a")));

      TS_ASSERT(gum::MarkovBlanket(bn, "a").hasSameStructure(
         gum::BayesNet< int >::fastPrototype("a->b")));
      TS_ASSERT(gum::MarkovBlanket(bn, "b").hasSameStructure(
         gum::BayesNet< int >::fastPrototype("a->b->c")));
      TS_ASSERT(gum::MarkovBlanket(bn, "c").hasSameStructure(
         gum::BayesNet< int >::fastPrototype("b->c->d;f->d")));
      TS_ASSERT(gum::MarkovBlanket(bn, "d").hasSameStructure(
         gum::BayesNet< int >::fastPrototype("c->d->e;f->d->g;i->g")));
      TS_ASSERT(gum::MarkovBlanket(bn, "e").hasSameStructure(
         gum::BayesNet< int >::fastPrototype("d->e")));
      TS_ASSERT(gum::MarkovBlanket(bn, "f").hasSameStructure(
         gum::BayesNet< int >::fastPrototype("c->d;f->d;")));
      TS_ASSERT(gum::MarkovBlanket(bn, "g").hasSameStructure(
         gum::BayesNet< int >::fastPrototype("d->g;i->g;")));
      TS_ASSERT(gum::MarkovBlanket(bn, "h").hasSameStructure(
         gum::BayesNet< int >::fastPrototype("h->i;")));
      TS_ASSERT(gum::MarkovBlanket(bn, "i").hasSameStructure(
         gum::BayesNet< int >::fastPrototype("d->g;h->i->g;;")));
    }

    void testMarkovBlanketSpecialArcs() {
      const auto bn = gum::BayesNet< int >::fastPrototype(
         "aa->bb->cc->dd->ee;ff->dd->gg;hh->ii->gg;ff->ii;ff->gg");
      const auto mb = gum::BayesNet< int >::fastPrototype(
         "cc->dd->ee;ff->dd->gg;ff->gg;ff->ii->gg");
      TS_ASSERT(gum::MarkovBlanket(bn, "dd").hasSameStructure(mb));
    }
  };

}   // namespace gum_tests
