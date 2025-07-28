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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/GibbsSampling.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <gumtest/AgrumApproximationUtils.h>   // must be last include


#define EPSILON_FOR_GIBBS_SIMPLE_TEST 2e-1
#define EPSILON_FOR_GIBBS             1e-1

#define EPSILON_FOR_GIBBS_HARD_TEST 2e-1
#define EPSILON_FOR_HARD_GIBBS      1e-1

namespace gum_tests {

  class aSimpleGibbsApproxListener: public gum::ApproximationSchemeListener {
    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleGibbsApproxListener(gum::ApproximationScheme& sch) :
        gum::ApproximationSchemeListener(sch), __nbr(0), __mess("") {};

    void whenProgress(const void* buffer, const gum::Size a, const double b, const double c) {
      __nbr++;
    }

    void whenStop(const void* buffer, const std::string& s) { __mess = s; }

    int getNbr() { return __nbr; }

    std::string getMess() { return __mess; }
  };

  class [[maybe_unused]] GibbsSamplingTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(GibbsSimpleBN) {
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
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(GibbsApproxBinaryTreeWithoutEvidence) {
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
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(GibbsApproxBinaryTreeWithEvidenceOnRoot) {
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
        TS_ASSERT(false)
      }

    }   //

    GUM_ACTIVE_TEST(GibbsApproxBinaryTreeWithEvidenceOnLeaf) {
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
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(GibbsApproxBinaryTreeWithEvidenceOnMid) {
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
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(GibbsApproxBinaryTreeWithMultipleEvidence) {
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
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(GibbsApproxNaryTreeWithMultipleEvidence) {
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
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(GibbsApproxSimpleBN) {
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
        TS_ASSERT(false)
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
        TS_ASSERT(false)
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
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(GibbsApproxCplxBN) {
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
        TS_ASSERT(false)
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
        TS_ASSERT(false)
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
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(GibbsApproxAsia) {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/asia.bif"));
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)

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
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(GibbsApproxInfListener) {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)

      gum::GibbsSampling< double > inf(&bn);
      aSimpleGibbsApproxListener   agsl(inf);
      inf.setVerbosity(true);


      // Testing the inference
      inf.setVerbosity(false);
      inf.setEpsilon(EPSILON_FOR_GIBBS);
      TS_GUM_ASSERT_THROWS_NOTHING(inf.makeInference())
      TS_ASSERT_EQUALS(agsl.getNbr() * inf.periodSize() + inf.burnIn(), inf.nbrIterations())
      TS_ASSERT_DIFFERS(agsl.getMess(), std::string(""))
    }

    GUM_ACTIVE_TEST(EvidenceAsTargetOnCplxBN) {
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

        TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior("d"))
        TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(bn.idFromName("d")))

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(MultipleInferenceWithSameEngine) {
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
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(BugGibbs) {
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
        TS_ASSERT_LESS_THAN(err, RELEVANT_ERR);
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
        TS_ASSERT_LESS_THAN(err, RELEVANT_ERR);
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
        TS_ASSERT_LESS_THAN(err, RELEVANT_ERR);
      }
    }
  };
}   // namespace gum_tests
