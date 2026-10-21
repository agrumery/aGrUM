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
#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/GibbsSampling.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <gumtest/AgrumApproximationUtils.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

// must be last include


#define EPSILON_FOR_GIBBS_SIMPLE_TEST 2e-1
#define EPSILON_FOR_GIBBS             1e-1

#define EPSILON_FOR_GIBBS_HARD_TEST 2e-1
#define EPSILON_FOR_HARD_GIBBS      1e-1

namespace gum_tests {

  class aSimpleGibbsApproxListener: public gum::ApproximationSchemeListener {
    protected:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleGibbsApproxListener(gum::ApproximationScheme& sch) :
        gum::ApproximationSchemeListener(sch), __nbr(0), __mess("") {}

    void whenProgress(const void* buffer, const gum::Size a, const double b, const double c) {
      __nbr++;
    }

    void whenStop(const void* buffer, std::string_view s) { __mess = s; }

    int getNbr() { return __nbr; }

    std::string getMess() { return __mess; }
  };

  struct GibbsSamplingTestSuite {
    public:
    // namespace gum_tests


    //
  };

  GUM_TEST(GibbsSimpleBN) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->b->c->f;d->b->e->f;e->g");
    unsharpen(bn);

    gum::LazyPropagation< double > lazy(&bn);
    lazy.makeInference();

    try {
      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(GibbsApproxBinaryTreeWithoutEvidence) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
    unsharpen(bn);

    gum::LazyPropagation< double > lazy(&bn);
    lazy.addEvidence("d", 0);
    lazy.addEvidence("g", 1);
    lazy.makeInference();

    try {
      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.addEvidence("d", 0);
        inf.addEvidence("g", 1);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(GibbsApproxBinaryTreeWithEvidenceOnRoot) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
    unsharpen(bn);
    std::string ev = "b";

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName(ev), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(GibbsApproxBinaryTreeWithEvidenceOnLeaf) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
    unsharpen(bn);
    std::string ev = "h";

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName(ev), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(GibbsApproxBinaryTreeWithEvidenceOnMid) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
    unsharpen(bn);
    std::string ev = "e";

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName(ev), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName(ev), 0);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(GibbsApproxBinaryTreeWithMultipleEvidence) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
    unsharpen(bn);

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName("e"), 0);
      lazy.addEvidence(bn.idFromName("b"), 1);
      lazy.addEvidence(bn.idFromName("h"), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.setVerbosity(false);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(GibbsApproxNaryTreeWithMultipleEvidence) {
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
        gum::GibbsSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("e"), 0);
        inf.addEvidence(bn.idFromName("b"), 1);
        inf.addEvidence(bn.idFromName("h"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(GibbsApproxSimpleBN) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);
    unsharpen(bn);

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName("a"), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName("d"), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(GibbsApproxCplxBN) {
    auto bn = gum::BayesNet< double >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;",
        3);
    unsharpen(bn);

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName("a"), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("a"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.addEvidence(bn.idFromName("d"), 0);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_GIBBS);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_SIMPLE_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(GibbsApproxAsia) {
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/asia.bif"));
    gum::Size                nbrErr = static_cast< gum::Size >(0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
    CHECK_EQ(nbrErr, static_cast< gum::Size >(0));

    try {
      gum::LazyPropagation< double > lazy(&bn);
      lazy.makeInference();

      GUM_APPROX_TEST_BEGIN_ITERATION
        gum::GibbsSampling< double > inf(&bn);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_HARD_GIBBS);
        inf.setMinEpsilonRate(0.001);
        inf.makeInference();
      GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_GIBBS_HARD_TEST)
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(GibbsApproxInfListener) {
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
    gum::Size                nbrErr = static_cast< gum::Size >(0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
    CHECK_EQ(nbrErr, static_cast< gum::Size >(0));

    gum::GibbsSampling< double > inf(&bn);
    aSimpleGibbsApproxListener   agsl(inf);
    inf.setVerbosity(true);


    // Testing the inference
    inf.setVerbosity(false);
    inf.setEpsilon(EPSILON_FOR_GIBBS);
    GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());
    CHECK_EQ(agsl.getNbr() * inf.periodSize() + inf.burnIn(), inf.nbrIterations());
    CHECK_NE(agsl.getMess(), std::string(""));
  }

  GUM_TEST(EvidenceAsTargetOnCplxBN) {
    auto bn = gum::BayesNet< double >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;",
        3);
    unsharpen(bn);

    try {
      gum::GibbsSampling< double > inf(&bn);
      inf.addEvidence(bn.idFromName("d"), 0);
      inf.setVerbosity(false);
      inf.setEpsilon(EPSILON_FOR_GIBBS);
      inf.makeInference();

      GUM_CHECK_ASSERT_THROWS_NOTHING(inf.posterior("d"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf.posterior(bn.idFromName("d")));

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(MultipleInferenceWithSameEngine) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);
    unsharpen(bn);

    try {
      gum::GibbsSampling< double > inf(&bn);
      inf.addEvidence(bn.idFromName("d"), 0);
      inf.setVerbosity(false);
      inf.setEpsilon(EPSILON_FOR_GIBBS);
      inf.makeInference();

      inf.eraseAllEvidence();
      inf.addEvidence(bn.idFromName("d"), 0);
      inf.setVerbosity(false);
      inf.setEpsilon(EPSILON_FOR_GIBBS);
      inf.makeInference();

    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }
  }

  GUM_TEST(BugGibbs) {
    auto         bn           = gum::BayesNet< double >::fastPrototype("d->g->l;s<-i->g");
    const double RELEVANT_ERR = 0.02;
    const double EPSILON      = 1e-2;
    const double RATE         = 1e-6;

    {
      auto exact = gum::LazyPropagation(&bn);
      exact.makeInference();

      auto gibbs = gum::GibbsSampling(&bn);
      gibbs.setEpsilon(EPSILON);
      gibbs.setMinEpsilonRate(RATE);
      gibbs.makeInference();

      double err = 0.0;
      for (const auto n: bn.nodes()) {
        const double err_n = (exact.posterior(n) - gibbs.posterior(n)).abs().max();
        if (err < err_n) err = err_n;
      }
      CHECK_LT(err, RELEVANT_ERR);
    }
    {
      auto exact = gum::LazyPropagation(&bn);
      exact.addEvidence("d", 1);
      exact.makeInference();

      auto gibbs = gum::GibbsSampling(&bn);
      gibbs.addEvidence("d", 1);
      gibbs.setEpsilon(EPSILON);
      gibbs.setMinEpsilonRate(RATE);
      gibbs.makeInference();

      double err = 0.0;
      for (const auto n: bn.nodes()) {
        const double err_n = (exact.posterior(n) - gibbs.posterior(n)).abs().max();
        if (err < err_n) err = err_n;
      }
      CHECK_LT(err, RELEVANT_ERR);
    }
    {
      auto exact = gum::LazyPropagation(&bn);
      exact.addEvidence("g", 1);
      exact.makeInference();

      auto gibbs = gum::GibbsSampling(&bn);
      gibbs.addEvidence("g", 1);
      gibbs.setEpsilon(EPSILON);
      gibbs.setMinEpsilonRate(RATE);
      gibbs.makeInference();

      double err = 0.0;
      for (const auto n: bn.nodes()) {
        const double err_n = (exact.posterior(n) - gibbs.posterior(n)).abs().max();
        if (err < err_n) err = err_n;
      }
      CHECK_LT(err, RELEVANT_ERR);
    }
  }

  GUM_TEST(GibbsStartsFromACompatibleState) {
    // C = A AND B (deterministic) with evidence C = 1: the only compatible state is A = B = 1.
    // A chain started in a state incompatible with the evidence (e.g. A = B = 0, probability
    // 0.25 for a forward sample) has only zero-mass moves and would stay there forever.
    auto bn = gum::BayesNet< double >::fastPrototype("A->C<-B");
    bn.cpt("A").fillWith({0.5, 0.5});
    bn.cpt("B").fillWith({0.5, 0.5});
    gum::Instantiation I(bn.cpt("C"));
    for (I.setFirst(); !I.end(); I.inc())
      bn.cpt("C").set(I,
                      (I.val(bn.variable("C"))
                       == ((I.val(bn.variable("A")) == 1 && I.val(bn.variable("B")) == 1) ? 1 : 0))
                          ? 1.0
                          : 0.0);

    // with a buggy initialisation, 20 chains all avoid the absorbing state with prob. 0.75^20
    for (int i = 0; i < 20; ++i) {
      gum::GibbsSampling< double > inf(&bn);
      inf.addEvidence("C", 1);
      inf.setEpsilon(1e-2);
      inf.setVerbosity(false);
      inf.makeInference();
      for (const auto name: {"A", "B"}) {
        const auto&        post = inf.posterior(name);
        gum::Instantiation J(post);
        J.chgVal(bn.variable(name), 1);
        CHECK_EQ(post[J], doctest::Approx(1.0));
      }
    }
  }

  GUM_TEST(GibbsFindsACompatibleStateForUnlikelyEvidence) {
    // C = AND(X0..X9) with evidence C = 1: a forward sample is compatible with probability 2^-10,
    // the pruning (C = 1 forces every Xi = 1) finds the compatible start.
    std::string proto = "C";
    for (int i = 0; i < 10; ++i)
      proto += "<-X" + std::to_string(i) + ";C";
    auto bn = gum::BayesNet< double >::fastPrototype(proto);
    for (int i = 0; i < 10; ++i)
      bn.cpt("X" + std::to_string(i)).fillWith({0.5, 0.5});
    const auto&        c = bn.variable("C");
    gum::Instantiation I(bn.cpt("C"));
    for (I.setFirst(); !I.end(); I.inc()) {
      bool all = true;
      for (int i = 0; i < 10; ++i)
        all = all && (I.val(bn.variable("X" + std::to_string(i))) == 1);
      bn.cpt("C").set(I, (I.val(c) == (all ? 1u : 0u)) ? 1.0 : 0.0);
    }

    for (int k = 0; k < 5; ++k) {
      gum::GibbsSampling< double > inf(&bn);
      inf.addEvidence("C", 1);
      inf.setEpsilon(1e-2);
      inf.setVerbosity(false);
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());
      for (int i = 0; i < 10; ++i) {
        const auto         name = "X" + std::to_string(i);
        const auto&        post = inf.posterior(name);
        gum::Instantiation J(post);
        J.chgVal(bn.variable(name), 1);
        CHECK_EQ(post[J], doctest::Approx(1.0));
      }
    }
  }

  GUM_TEST(GibbsWithIncompatibleEvidence) {
    // C = A AND B with evidence C = 1 and A = 0: no sample has a positive probability
    auto               bn = gum::BayesNet< double >::fastPrototype("A->C<-B");
    gum::Instantiation I(bn.cpt("C"));
    for (I.setFirst(); !I.end(); I.inc())
      bn.cpt("C").set(I,
                      (I.val(bn.variable("C"))
                       == ((I.val(bn.variable("A")) == 1 && I.val(bn.variable("B")) == 1) ? 1 : 0))
                          ? 1.0
                          : 0.0);
    gum::GibbsSampling< double > inf(&bn);
    inf.addEvidence("C", 1);
    inf.addEvidence("A", 0);
    inf.setVerbosity(false);
    CHECK_THROWS_AS(inf.makeInference(), const gum::IncompatibleEvidence&);
  }

  GUM_TEST(GibbsFindsACompatibleStateBySmoothing) {
    // Y_j = X_j XOR X_{j+1} with evidence Y_j = 1 for all j: no value can be pruned and a forward
    // sample is compatible with probability 2^-20, so the compatible start is found by the smoothed
    // Gibbs moves. The two solutions alternate the values of the X's. Single-site Gibbs moves
    // cannot go from one solution to the other: each chain stays in one of them.
    const int   n = 20;
    std::string proto;
    for (int j = 0; j < n; ++j)
      proto += "X" + std::to_string(j) + "->Y" + std::to_string(j) + "<-X" + std::to_string(j + 1)
             + ";";
    auto bn = gum::BayesNet< double >::fastPrototype(proto);
    for (int j = 0; j <= n; ++j)
      bn.cpt("X" + std::to_string(j)).fillWith({0.5, 0.5});
    for (int j = 0; j < n; ++j) {
      const auto&        y   = bn.variable("Y" + std::to_string(j));
      const auto&        x1  = bn.variable("X" + std::to_string(j));
      const auto&        x2  = bn.variable("X" + std::to_string(j + 1));
      const auto&        cpt = bn.cpt("Y" + std::to_string(j));
      gum::Instantiation I(cpt);
      for (I.setFirst(); !I.end(); I.inc())
        cpt.set(I, (I.val(y) == (I.val(x1) ^ I.val(x2))) ? 1.0 : 0.0);
    }

    gum::GibbsSampling< double > inf(&bn);
    for (int j = 0; j < n; ++j)
      inf.addEvidence("Y" + std::to_string(j), 1);
    inf.setEpsilon(1e-2);
    inf.setVerbosity(false);
    GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());

    auto p1 = [&](int j) {
      const auto&        post = inf.posterior("X" + std::to_string(j));
      gum::Instantiation J(post);
      J.chgVal(post.variable(0), 1);
      return post[J];
    };
    for (int j = 0; j < n; ++j)
      CHECK_EQ(p1(j + 1), doctest::Approx(1.0 - p1(j)));
  }

  GUM_TEST(GibbsStartsOnDiabetesWithEvidence) {
    // a large model with many null and tiny probabilities: the search of a compatible first sample
    // must not prefer null probabilities to tiny positive ones
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/Diabetes.bif"));
    gum::Size                nbrErr = static_cast< gum::Size >(0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
    CHECK_EQ(nbrErr, static_cast< gum::Size >(0));

    // about 2.6% of the forward samples are compatible with this evidence
    for (int k = 0; k < 8; ++k) {
      gum::GibbsSampling< double > inf(&bn);
      inf.addEvidence("bg_24", 0);
      inf.addEvidence("ins_indep_util_23", 1);
      inf.addEvidence("renal_cl_14", 1);
      inf.setMaxIter(10);
      inf.setVerbosity(false);
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());
    }
  }

  GUM_TEST(GibbsWithoutBurnIn) {
    // burnIn(0) is a legal value: the chain starts from the first sample
    auto bn = gum::BayesNet< double >::fastPrototype("A->B");
    bn.cpt("A").fillWith({0.5, 0.5});
    bn.cpt("B").fillWith({0.9, 0.1, 0.1, 0.9});
    gum::LazyPropagation< double > lazy(&bn);
    lazy.makeInference();

    gum::GibbsSampling< double > inf(&bn);
    inf.setBurnIn(0);
    inf.setEpsilon(1e-2);
    inf.setVerbosity(false);
    GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());
    GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(inf.posterior("B"), lazy.posterior("B"), 0.1);
  }
}   // namespace gum_tests
