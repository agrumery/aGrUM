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

#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/MonteCarloSampling.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <gumtest/AgrumApproximationUtils.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

// must be last include

#define EPSILON_FOR_MONTECARLO_SIMPLE_TEST 15e-2
#define EPSILON_FOR_MONTECARLO             2e-1

namespace gum_tests {

  class aSimpleMCListener: public gum::ApproximationSchemeListener {
    protected:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleMCListener(gum::ApproximationScheme& sch) :
        gum::ApproximationSchemeListener(sch), __nbr(0), __mess("") {}

    void whenProgress(const void* buffer, const gum::Size a, const double b, const double c) {
      __nbr++;
    }

    void whenStop(const void* buffer, std::string_view s) {
      __nbr++;
      __mess = s;
    }

    int getNbr() { return __nbr; }

    std::string getMess() { return __mess; }
  };

  struct MonteCarloSamplingTestSuite {
    public:
    // namespace gum_tests
  };

  GUM_TEST(MCbasic) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->h->c");
    bn.cpt("a").fillWith({0.2f, 0.8f});
    bn.cpt("h").fillWith({0.4f, 0.6f, 0.7f, 0.3f});
    bn.cpt("c").fillWith({0.2f, 0.8f, 0.9f, 0.1f});

    gum::LazyPropagation< double > lazy(&bn);
    lazy.makeInference();

    try {
      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MCBinaryTreeWithoutEvidence) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
    unsharpen(bn);

    gum::LazyPropagation< double > lazy(&bn);
    lazy.makeInference();

    try {
      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MCBinaryTreeWithEvidenceOnRoot) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
    unsharpen(bn);

    std::string ev = "b";

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName(ev), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MCBinaryTreeWithEvidenceOnLeaf) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
    unsharpen(bn);
    std::string ev = "h";

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName(ev), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.setVerbosity(false);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MCBinaryTreeWithEvidenceOnMid) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
    unsharpen(bn);
    std::string ev = "e";

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName(ev), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.setVerbosity(false);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MCBinaryTreeWithMultipleEvidence) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
    unsharpen(bn);

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName("e"), 0);
      lazy.addEvidence(bn.idFromName("b"), 1);
      lazy.addEvidence(bn.idFromName("h"), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.setVerbosity(false);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MCNaryTreeWithMultipleEvidence) {
    auto bn = gum::BayesNet< double >::fastPrototype(
        "a[4]->d[8]->f[3];b->d->g[5];b->e[4]->h;c->e;i[10]->j[3]->h");
    unsharpen(bn);

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName("e"), 0);
      lazy.addEvidence(bn.idFromName("b"), 1);
      lazy.addEvidence(bn.idFromName("h"), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MCSimpleBN) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);
    unsharpen(bn);

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName("a"), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName("d"), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MCCplxBN) {
    auto bn = gum::BayesNet< double >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;",
        3);

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName("a"), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName("d"), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MCAsia) {
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/asia.bif"));
    gum::Size                nbrErr = static_cast< gum::Size >(0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
    CHECK_EQ(nbrErr, static_cast< gum::Size >(0));

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MCAlarm) {
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
    gum::Size                nbrErr = static_cast< gum::Size >(0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
    CHECK_EQ(nbrErr, static_cast< gum::Size >(0));

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::MonteCarloSampling< double > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_MONTECARLO);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_MONTECARLO_SIMPLE_TEST)

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MCInfListener) {
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
    gum::Size                nbrErr = static_cast< gum::Size >(0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
    CHECK_EQ(nbrErr, static_cast< gum::Size >(0));

    gum::MonteCarloSampling< double > inf(&bn);
    aSimpleMCListener                 agsl(inf);
    inf.setVerbosity(true);

    try {
      // Testing the inference
      inf.setVerbosity(false);
      inf.setEpsilon(EPSILON_FOR_MONTECARLO);
      inf.makeInference();

    } catch (const gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    CHECK_EQ(agsl.getNbr() * inf.periodSize(), inf.nbrIterations());
    CHECK_NE(agsl.getMess(), std::string(""));
  }

  GUM_TEST(Constructor) {
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
    gum::Size                nbrErr = static_cast< gum::Size >(0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
    CHECK_EQ(nbrErr, static_cast< gum::Size >(0));
    try {
      gum::MonteCarloSampling< double > inf(&bn);
      inf.setEpsilon(EPSILON_FOR_MONTECARLO);
    } catch (const gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(EvidenceAsTargetOnCplxBN) {
    auto bn = gum::BayesNet< double >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;",
        3);
    unsharpen(bn);

    try {
      gum::MonteCarloSampling< double > inf(&bn);
      inf.addEvidence(bn.idFromName("d"), 0);
      inf.setVerbosity(false);
      inf.setEpsilon(EPSILON_FOR_MONTECARLO);
      inf.makeInference();
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf.posterior("d"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf.posterior(bn.idFromName("d")));

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MonteCarloWithExplicitTargetAndEvidence) {
    // the sampling BN used to drop the nodes barren for the targets, while every node is drawn
    // (NotFound)
    auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");
    bn.cpt("A").fillWith({0.3, 0.7});
    bn.cpt("B").fillWith({0.9, 0.1, 0.2, 0.8});
    bn.cpt("C").fillWith({0.6, 0.4, 0.1, 0.9});

    gum::LazyPropagation< double > lazy(&bn);
    lazy.addEvidence("B", 1);
    lazy.makeInference();

    gum::MonteCarloSampling< double > inf(&bn);
    inf.setEpsilon(1e-3);
    inf.setVerbosity(false);
    inf.addTarget("A");
    inf.addEvidence("B", 1);
    GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());
    GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(lazy.posterior("A"), inf.posterior("A"), 0.05);
  }

  // the inference follows the CPTs changed in place after invalidateTensors()
  GUM_TEST(MonteCarloInvalidateTensors) {
    CHECK_LE(errorAfterInvalidateTensors< gum::MonteCarloSampling< double > >(), 0.05);
  }

  // non-regression: on a deterministic BN, all the samples agree; the error (length of a Wald
  // interval) was 0 and the inference stopped on epsilon at the first check. The length of the
  // Wilson score interval, z²/(n+z²) when all the samples agree, reaches epsilon=0.01 only after
  // n >= z²/0.01 - z² (about 380) samples
  GUM_TEST(DeterministicBNDoesNotStopAfterAFewSamples) {
    auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");
    bn.cpt("A").fillWith({1.0, 0.0});
    bn.cpt("B").fillWith({0.0, 1.0, 1.0, 0.0});
    bn.cpt("C").fillWith({1.0, 0.0, 0.0, 1.0});

    gum::MonteCarloSampling< double > inf(&bn);
    inf.setEpsilon(0.01);
    inf.disableMinEpsilonRate();
    inf.disableMaxTime();
    inf.setMaxIter(100000);
    inf.setPeriodSize(10);
    inf.makeInference();

    CHECK_EQ(inf.stateApproximationScheme(),
             gum::IApproximationSchemeConfiguration::ApproximationSchemeSTATE::Epsilon);
    constexpr double z2 = 1.96 * 1.96;
    CHECK_GE(double(inf.nbrIterations()), z2 / 0.01 - z2);
    CHECK_LT(inf.nbrIterations(), gum::Size(400));
    // A=0, B=1, C=1 in every sample
    const auto&        post = inf.posterior("C");
    gum::Instantiation i(post);
    i.chgVal(0, 1);
    CHECK_GT(post[i], 0.99);
  }
}   // namespace gum_tests
