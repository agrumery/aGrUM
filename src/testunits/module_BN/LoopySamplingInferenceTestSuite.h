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
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/loopyBeliefPropagation.h>
#include <agrum/BN/inference/loopySamplingInference.h>
#include <agrum/multidim/implementations/multiDimArray.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/core/approximations/approximationSchemeListener.h>
#include <cxxtest/AgrumApproximationUtils.h>   // must be last include

#define EPSILON_FOR_HYBRID_SIMPLE_TEST 2e-1
#define EPSILON_FOR_HYBRID 1e-1

#define EPSILON_FOR_HYBRID_HARD_TEST 2e-1
#define EPSILON_FOR_HARD_HYBRID 6e-2

namespace gum_tests {

  class aSimpleHybridListener : public gum::ApproximationSchemeListener {
    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleHybridListener(gum::ApproximationScheme& sch) :
        gum::ApproximationSchemeListener(sch), __nbr(0), __mess(""){};

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

  class loopySamplingInferenceTestSuite : public CxxTest::TestSuite {
    public:
    void /*test*/ HybridBinaryTreeWithoutEvidence() {
      auto bn = gum::BayesNet< double >::fastPrototype(
         "a->d->f;b->d->g;b->e->h;c->e;i->j->h");

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        gum::LoopyBeliefPropagation< double > lbp(&bn);
        lbp.makeInference();
        compareInference(__FILE__, __LINE__, bn, lazy, lbp);

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::LoopySamplingInference< double, gum::WeightedSampling > inf(&bn);
        inf.setEpsilon(EPSILON_FOR_HYBRID);
        inf.setVerbosity(false);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void /*test*/ HybridBinaryTreeWithEvidenceOnRoot() {
      auto bn = gum::BayesNet< double >::fastPrototype(
         "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      std::string ev = "b";

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::LoopySamplingInference< double, gum::ImportanceSampling > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_HYBRID);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void /*test*/ HybridBinaryTreeWithEvidenceOnLeaf() {
      auto bn = gum::BayesNet< double >::fastPrototype(
         "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      std::string ev = "h";

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::LoopySamplingInference< double, gum::MonteCarloSampling > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_HYBRID);
        inf.setVerbosity(false);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void /*test*/ HybridBinaryTreeWithEvidenceOnMid() {
      auto bn = gum::BayesNet< double >::fastPrototype(
         "a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      std::string ev = "e";

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::LoopySamplingInference< double, gum::WeightedSampling > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_HYBRID);
        inf.setVerbosity(false);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void /*test*/ HybridBinaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet< double >::fastPrototype(
         "a->d->f;b->d->g;b->e->h;c->e;i->j->h");

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("e"), 0);
        lazy.addEvidence(bn.idFromName("b"), 1);
        lazy.addEvidence(bn.idFromName("h"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::LoopySamplingInference< double, gum::ImportanceSampling > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setEpsilon(EPSILON_FOR_HYBRID);
        inf.setVerbosity(false);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void /*test*/ HybridNaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet< double >::fastPrototype(
         "a[4]->d[8]->f[3];b->d->g[5];b->e[4]->h;c->e;i[10]->j[3]->h");

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("e"), 0);
        lazy.addEvidence(bn.idFromName("b"), 1);
        lazy.addEvidence(bn.idFromName("h"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
        gum::LoopySamplingInference< double, gum::ImportanceSampling > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HYBRID);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void /*test*/ HybridSimpleBN() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION;
        gum::LoopySamplingInference< double, gum::GibbsSampling > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HYBRID);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION;
        gum::LoopySamplingInference< double, gum::ImportanceSampling > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HYBRID);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION;
        gum::LoopySamplingInference< double, gum::ImportanceSampling > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HYBRID);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void /*test*/ HybridCplxBN() {
      auto bn = gum::BayesNet< double >::fastPrototype(
         "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;", 3);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION;
        gum::LoopySamplingInference< double, gum::WeightedSampling > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HYBRID);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION;
        gum::LoopySamplingInference< double, gum::ImportanceSampling > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HYBRID);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION;
        gum::LoopySamplingInference< double, gum::GibbsSampling > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HARD_HYBRID);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_HARD_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void /*test*/ HybridAsia() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("asia.bif"));
      gum::Size                nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION;
        gum::LoopySamplingInference< double, gum::GibbsSampling > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HARD_HYBRID);
        inf.makeInference();
        inf.setMinEpsilonRate(1e-3);
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_HARD_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    void /*test*/ HybridAlarm() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("alarm.bif"));
      gum::Size                nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();


        GUM_APPROX_TEST_BEGIN_ITERATION;
        gum::LoopySamplingInference< double, gum::WeightedSampling > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HARD_HYBRID);
        inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_HYBRID_HARD_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    void testMultipleInferenceWithSameEngine() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);
      unsharpen(bn);

      try {
        gum::LoopySamplingInference< double, gum::WeightedSampling > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HARD_HYBRID);
        inf.makeInference();

        inf.eraseAllEvidence();
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HARD_HYBRID);
        inf.makeInference();

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }
  };

}   // namespace gum_tests
