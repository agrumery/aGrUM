/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/loopyBeliefPropagation.h>
#include <agrum/tools/variables/labelizedVariable.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/tools/core/approximations/approximationSchemeListener.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class aSimpleLBPListener: public gum::ApproximationSchemeListener {
    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleLBPListener(gum::ApproximationScheme& sch) :
        gum::ApproximationSchemeListener(sch), __nbr(0), __mess(""){};
    void whenProgress(const void*     buffer,
                      const gum::Size a,
                      const double    b,
                      const double    c) {
      __nbr++;
    }
    void whenStop(const void* buffer, const std::string s) {
      __nbr++;
      __mess = s;
    }

    int         getNbr() { return __nbr; }
    std::string getMess() { return __mess; }
  };

#define MAX_ITER 10

  class LoopyBeliefPropagationTestSuite: public CxxTest::TestSuite {
    public:
    void testLBPBinaryTreeWithoutEvidence() {
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e;i->j->h");

        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        gum::LoopyBeliefPropagation< double > inf(&bn);
        try {
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
    }

    void testLBPBinaryTreeWithEvidenceOnRoot() {
      const std::string ev = "b";
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e;i->j->h");

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName(ev), 0);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }

      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName(ev), std::vector< double >{0.2f, 0.8f});
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), std::vector< double >{0.2f, 0.8f});
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
    }

    void testLBPBinaryTreeWithEvidenceOnLeaf() {
      const std::string ev = "h";
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e;i->j->h");

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName(ev), 0);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName(ev), std::vector< double >{0.2f, 0.8f});
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), std::vector< double >{0.2f, 0.8f});
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
    }

    void testLBPBinaryTreeWithEvidenceOnMid() {
      const std::string ev = "e";
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e;i->j->h");

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName(ev), 0);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName(ev), std::vector< double >{0.2f, 0.8f});
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), std::vector< double >{0.2f, 0.8f});
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
    }

    void testLBPBinaryTreeWithMultipleEvidence() {
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e;i->j->h");

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("e"), 0);
          lazy.addEvidence(bn.idFromName("b"), 1);
          lazy.addEvidence(bn.idFromName("h"), 0);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("e"), 0);
          inf.addEvidence(bn.idFromName("b"), 1);
          inf.addEvidence(bn.idFromName("h"), 0);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("e"), std::vector< double >{0.2f, 0.8f});
          lazy.addEvidence(bn.idFromName("b"), 0);
          lazy.addEvidence(bn.idFromName("h"), std::vector< double >{0.7f, 0.3f});
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("e"), std::vector< double >{0.2f, 0.8f});
          inf.addEvidence(bn.idFromName("b"), 0);
          inf.addEvidence(bn.idFromName("h"), std::vector< double >{0.7f, 0.3f});
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
    }
    void testLBPNaryTreeWithMultipleEvidence() {
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a[4]->d[8]->f[3];b->d->g[5];b->e[4]->h;c->e;i[10]->j[3]->h");

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("e"), 0);
          lazy.addEvidence(bn.idFromName("b"), 1);
          lazy.addEvidence(bn.idFromName("h"), 0);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("e"), 0);
          inf.addEvidence(bn.idFromName("b"), 1);
          inf.addEvidence(bn.idFromName("h"), 0);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a[4]->d[8]->f[3];b->d->g[5];b->e[4]->h;c->e;i[10]->j[3]->h");
        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("e"),
                           std::vector< double >{0.1f, 0.3f, 0.4f, 0.7f});
          lazy.addEvidence(bn.idFromName("b"), 0);
          lazy.addEvidence(bn.idFromName("h"), std::vector< double >{0.7f, 0.3f});
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("e"),
                          std::vector< double >{0.1f, 0.3f, 0.4f, 0.7f});
          inf.addEvidence(bn.idFromName("b"), 0);
          inf.addEvidence(bn.idFromName("h"), std::vector< double >{0.7f, 0.3f});
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
    }

    void testLBPSimpleBN() {
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn =
           gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn =
           gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("a"), 0);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("a"), 0);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn =
           gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("d"), 0);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("d"), 0);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn =
           gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("a"),
                           std::vector< double >{0.7f, 0.3f, 1.0f});
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("a"),
                          std::vector< double >{0.7f, 0.3f, 1.0f});
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn =
           gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("d"),
                           std::vector< double >{0.7f, 0.3f, 1.0f});
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("d"),
                          std::vector< double >{0.7f, 0.3f, 1.0f});
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
    }

    void testLBPCplxBN() {
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("a"), 0);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("a"), 0);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("d"), 0);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("d"), 0);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("a"),
                           std::vector< double >{0.7f, 0.3f, 1.0f});
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("a"),
                          std::vector< double >{0.7f, 0.3f, 1.0f});
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
      for (int i = 0; i < MAX_ITER; i++) {
        const auto bn = gum::BayesNet< double >::fastPrototype(
           "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);

        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.addEvidence(bn.idFromName("d"),
                           std::vector< double >{0.7f, 0.3f, 1.0f});
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.addEvidence(bn.idFromName("d"),
                          std::vector< double >{0.7f, 0.3f, 1.0f});
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
    }

    void testLBPAsia() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/asia.bif"));
      gum::Size                nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));

      for (int i = 0; i < MAX_ITER; i++) {
        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.setVerbosity(false);
          inf.makeInference();
          if (__compareInference(bn, lazy, inf)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
    }

    void testLBPAlarm() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
      gum::Size                nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));

      for (int i = 0; i < MAX_ITER; i++) {
        try {
          gum::LazyPropagation< double > lazy(&bn);
          lazy.makeInference();

          gum::LoopyBeliefPropagation< double > inf(&bn);
          inf.setVerbosity(false);
          inf.makeInference();

          // alarm is not good for LBP
          if (__compareInference(bn, lazy, inf, 2.5e-1)) break;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          TS_ASSERT(false);
        }
      }
    }

    void testLBPInfListener() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
      gum::Size                nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));

      gum::LoopyBeliefPropagation< double > inf(&bn);
      aSimpleLBPListener                    agsl(inf);
      inf.setVerbosity(true);

      try {
        // Testing the inference
        inf.makeInference();
      } catch (gum::Exception e) { TS_ASSERT(false); }
      TS_ASSERT_EQUALS(agsl.getNbr() * inf.periodSize(), inf.nbrIterations());
      TS_ASSERT_DIFFERS(agsl.getMess(), std::string(""));
    }


    void testAggregatorsInLBP() {
      gum::BayesNet< double > bn;
      for (const auto& e: {"a", "b", "c", "d"})
        bn.add(e, 2);
      bn.addOR(gum::LabelizedVariable("O", "", 2));
      bn.addAND(gum::LabelizedVariable("A", "", 2));
      bn.addFORALL(gum::LabelizedVariable("F", "", 2));

      bn.addArc("a", "O");
      bn.addArc("a", "A");

      bn.addArc("b", "O");
      bn.addArc("b", "A");

      bn.addArc("c", "O");

      bn.addArc("O", "F");
      bn.addArc("A", "F");
      bn.addArc("a", "F");
      bn.addArc("d", "F");

      auto ie = gum::LoopyBeliefPropagation< double >(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(ie.makeInference();)
    }

    void testLogitInLBP() {
      gum::BayesNet< double > bn;
      for (const auto& item: {"Cold", "Flu", "Malaria", "X", "Y", "Z"})
        bn.add(item, 2);

      gum::LabelizedVariable fever("Fever", "", 2);
      TS_GUM_ASSERT_THROWS_NOTHING(bn.addLogit(fever, 0.3f));

      bn.addWeightedArc("Malaria", "Fever", 0.9f);
      bn.addWeightedArc("Flu", "Fever", 0.8f);
      bn.addWeightedArc("Cold", "Fever", 0.4f);

      TS_ASSERT_THROWS(bn.addWeightedArc("Malaria", "Cold", 0.8f),
                       gum::InvalidArc);

      bn.addArc("Y", "X");
      bn.addArc("Fever", "X");
      bn.addArc("Z", "X");

      gum::LoopyBeliefPropagation< double > ie(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(ie.makeInference();)
    }

    private:
    template < typename GUM_SCALAR >
    bool __compareInference(const gum::BayesNet< GUM_SCALAR >&         bn,
                            gum::LazyPropagation< GUM_SCALAR >&        lazy,
                            gum::LoopyBeliefPropagation< GUM_SCALAR >& inf,
                            double errmax = 1e-2) {
      GUM_SCALAR  err = static_cast< GUM_SCALAR >(0);
      std::string argstr = "";
      for (const auto& node: bn.nodes()) {
        GUM_SCALAR e = lazy.posterior(node).KL(inf.posterior(node));
        if (e > err) {
          err = e;
          argstr =
             bn.variable(node).name() + " (err=" + std::to_string(err) + ") : \n";
          argstr += "  lazy : " + lazy.posterior(node).toString() + "\n";
          argstr += "  inf : " + inf.posterior(node).toString() + " \n";
        }
      }
      if (err > errmax) { GUM_TRACE(argstr); }
      return err < errmax;
    }
  };
}   // namespace gum_tests
