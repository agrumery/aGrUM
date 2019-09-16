
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
#include <agrum/BN/algorithms/MarkovBlanket.h>

namespace gum_tests {
  class MarkovBlanketTestSuite : public CxxTest::TestSuite {
    public:
    void testChain() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c");
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
         gum::BayesNet< float >::fastPrototype("a->b->c->d->e;f->d->g;h->i->g");
      TS_ASSERT(!gum::MarkovBlanket(bn, "a").hasSameStructure(
         gum::BayesNet< float >::fastPrototype("b->a")));

      TS_ASSERT(gum::MarkovBlanket(bn, "a").hasSameStructure(
         gum::BayesNet< float >::fastPrototype("a->b")));
      TS_ASSERT(gum::MarkovBlanket(bn, "b").hasSameStructure(
         gum::BayesNet< float >::fastPrototype("a->b->c")));
      TS_ASSERT(gum::MarkovBlanket(bn, "c").hasSameStructure(
         gum::BayesNet< float >::fastPrototype("b->c->d;f->d")));
      TS_ASSERT(gum::MarkovBlanket(bn, "d").hasSameStructure(
         gum::BayesNet< float >::fastPrototype("c->d->e;f->d->g;i->g")));
      TS_ASSERT(gum::MarkovBlanket(bn, "e").hasSameStructure(
         gum::BayesNet< float >::fastPrototype("d->e")));
      TS_ASSERT(gum::MarkovBlanket(bn, "f").hasSameStructure(
         gum::BayesNet< float >::fastPrototype("c->d;f->d;")));
      TS_ASSERT(gum::MarkovBlanket(bn, "g").hasSameStructure(
         gum::BayesNet< float >::fastPrototype("d->g;i->g;")));
      TS_ASSERT(gum::MarkovBlanket(bn, "h").hasSameStructure(
         gum::BayesNet< float >::fastPrototype("h->i;")));
      TS_ASSERT(gum::MarkovBlanket(bn, "i").hasSameStructure(
         gum::BayesNet< float >::fastPrototype("d->g;h->i->g;;")));
    }

    void testMarkovBlanketSpecialArcs() {
      const auto bn = gum::BayesNet< float >::fastPrototype(
         "aa->bb->cc->dd->ee;ff->dd->gg;hh->ii->gg;ff->ii;ff->gg");
      const auto mb = gum::BayesNet< float >::fastPrototype(
         "cc->dd->ee;ff->dd->gg;ff->gg;ff->ii->gg");
      TS_ASSERT(gum::MarkovBlanket(bn, "dd").hasSameStructure(mb));
    }

    void testMarkovBlanketMultiLevel() {
      const auto bn = gum::BayesNet< float >::fastPrototype(
         "Z<-A->B->C->D->E<-Y;X->G<-F<-C<-I<-H->W");
      TS_ASSERT_THROWS(gum::MarkovBlanket(bn, "C", 0).size(), gum::InvalidArgument);
      TS_ASSERT_EQUALS(gum::MarkovBlanket(bn, "C", 1).size(), gum::Size(5));
      TS_ASSERT_EQUALS(gum::MarkovBlanket(bn, "C", 2).size(), gum::Size(11));
      TS_ASSERT_EQUALS(gum::MarkovBlanket(bn, "C", 3).size(), gum::Size(13));
    }
  };

}   // namespace gum_tests
