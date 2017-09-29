/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/GibbsSampling.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/core/approximations/approximationSchemeListener.h>
#include <cxxtest/AgrumApproximationUtils.h>  // must be last include


#define EPSILON_FOR_GIBBS_SIMPLE_TEST 5e-2
#define EPSILON_FOR_GIBBS 5e-2

#define EPSILON_FOR_GIBBS_HARD_TEST 8e-1 // it is a KL divergence
#define EPSILON_FOR_HARD_GIBBS 1e-2

namespace gum_tests {

  class aSimpleGibbsApproxListener : public gum::ApproximationSchemeListener {
    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleGibbsApproxListener(gum::ApproximationScheme& sch)
        : gum::ApproximationSchemeListener(sch)
        , __nbr(0)
        , __mess(""){};

    void whenProgress(const void*     buffer,
                      const gum::Size a,
                      const double    b,
                      const double    c) {
      __nbr++;
    }
    void whenStop(const void* buffer, const std::string s) { __mess = s; }

    int         getNbr() { return __nbr; }
    std::string getMess() { return __mess; }
  };
  class GibbsSamplingTestSuite : public CxxTest::TestSuite {


    public:
    void testGibbsSimpleBN() {
      auto bn =
        gum::BayesNet< float >::fastPrototype("a->b->c->f;d->b->e->f;e->g");
      unsharpen(bn);

      gum::LazyPropagation< float > lazy(&bn);
      lazy.makeInference();

      try {
        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }
    void testGibbsApproxBinaryTreeWithoutEvidence() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      gum::LazyPropagation< float > lazy(&bn);
      lazy.addEvidence("d", 0);
      lazy.addEvidence("g", 1);
      lazy.makeInference();

      try {
        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.addEvidence("d", 0);
        inf.addEvidence("g", 1);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testGibbsApproxBinaryTreeWithEvidenceOnRoot() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "b";

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

    }  //

    void testGibbsApproxBinaryTreeWithEvidenceOnLeaf() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "h";

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testGibbsApproxBinaryTreeWithEvidenceOnMid() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "e";

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testGibbsApproxBinaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("e"), 0);
        lazy.addEvidence(bn.idFromName("b"), 1);
        lazy.addEvidence(bn.idFromName("h"), 0);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testGibbsApproxNaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a[4]->d[8]->f[3];b->d->g[5];b->e[4]->h;c->e;i[10]->j[3]->h");
      unsharpen(bn);

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("e"), 0);
        lazy.addEvidence(bn.idFromName("b"), 1);
        lazy.addEvidence(bn.idFromName("h"), 0);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testGibbsApproxSimpleBN() {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c;a->d->c", 3);
      unsharpen(bn);

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testGibbsApproxCplxBN() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);
      unsharpen(bn);

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testGibbsApproxAsia() {
      gum::BayesNet< float >  bn;
      gum::BIFReader< float > reader(&bn, GET_RESSOURCES_PATH("asia.bif"));
      int                     nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);

      try {

        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HARD_GIBBS);
        inf.setMinEpsilonRate(0.001);
        inf.makeInference();
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_HARD_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testGibbsApproxAlarm() {
      gum::BayesNet< float >  bn;
      gum::BIFReader< float > reader(&bn, GET_RESSOURCES_PATH("alarm.bif"));
      int                     nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);

      try {
        gum::LazyPropagation< float > lazy(&bn);
        lazy.makeInference();

        APPROXINFERENCE_TEST_BEGIN_ITERATION
        gum::GibbsSampling< float > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HARD_GIBBS);
        inf.setMinEpsilonRate(0.001);
        inf.makeInference();
        // alarm is not good for GibbsApprox
        APPROXINFERENCE_TEST_END_ITERATION(EPSILON_FOR_GIBBS_HARD_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testGibbsApproxInfListener() {
      gum::BayesNet< float >  bn;
      gum::BIFReader< float > reader(&bn, GET_RESSOURCES_PATH("alarm.bif"));
      int                     nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);

      gum::GibbsSampling< float > inf(&bn);
      aSimpleGibbsApproxListener  agsl(inf);
      inf.setVerbosity(true);

      try {
        // Testing the inference
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
      } catch (gum::Exception e) {
        TS_ASSERT(false);
      }
      TS_ASSERT_EQUALS(agsl.getNbr() * inf.periodSize() + inf.burnIn(),
                       inf.nbrIterations());
      TS_ASSERT_DIFFERS(agsl.getMess(), std::string(""));
    }


    void testGibbsApproxDiabetes() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("Diabetes.bif"));
      int                      nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);

      try {
        gum::GibbsSampling< double > inf(&bn);
        inf.addEvidence("bg_24", 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setMaxTime(10);
        inf.makeInference();
      } catch (gum::Exception e) {
        GUM_SHOWERROR(e);
      }
    }


    void testEvidenceAsTargetOnCplxBN() {
      auto bn = gum::BayesNet< float >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);
      unsharpen(bn);

      try {
        gum::GibbsSampling< float > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();

        TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior("d"));
        TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(bn.idFromName("d")));

      } catch (gum::Exception& e) {

        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }
  };
}  // namespace gum_tests
