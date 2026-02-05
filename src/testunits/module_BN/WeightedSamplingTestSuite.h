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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/weightedSampling.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <gumtest/AgrumApproximationUtils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  WeightedSampling
#define GUM_CURRENT_MODULE BN

// must be last include

namespace gum_tests {
  constexpr auto EPSILON_FOR_WEIGHTED_SIMPLE_TEST = 15e-2;
  constexpr auto EPSILON_FOR_WEIGHTED             = 2e-1;

  class aSimpleWeightedListener: public gum::ApproximationSchemeListener {
    private:
    int         _nbr_ = 0;
    std::string _mess_;

    public:
    explicit aSimpleWeightedListener(gum::ApproximationScheme& sch) :
        gum::ApproximationSchemeListener(sch) {};

    void whenProgress(const void* buffer, const gum::Size a, const double b, const double c) final {
      _nbr_++;
    }

    void whenStop(const void* buffer, const std::string& s) final {
      _nbr_++;
      _mess_ = s;
    }

    int getNbr() { return _nbr_; }

    std::string getMess() { return _mess_; }
  };

  struct WeightedSamplingTestSuite {
    public:
    static void testWeightedBinaryTreeWithoutEvidence() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      gum::LazyPropagation< double > lazy(&bn);
      lazy.makeInference();

      try {
        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }   // namespace gum_tests

    static void testWeightedBinaryTreeWithEvidenceOnRoot() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "b";

      try {   // ok //

        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testWeightedBinaryTreeWithEvidenceOnLeaf() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "h";

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.setVerbosity(false);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testWeightedBinaryTreeWithEvidenceOnMid() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "e";

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.setVerbosity(false);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testWeightedBinaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("e"), 0);
        lazy.addEvidence(bn.idFromName("b"), 1);
        lazy.addEvidence(bn.idFromName("h"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("e"), 0);
          inf.addEvidence(bn.idFromName("b"), 1);
          inf.addEvidence(bn.idFromName("h"), 0);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.setVerbosity(false);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testWeightedNaryTreeWithMultipleEvidence() {
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
          gum::WeightedSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("e"), 0);
          inf.addEvidence(bn.idFromName("b"), 1);
          inf.addEvidence(bn.idFromName("h"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testWeightedSimpleBN() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);
      unsharpen(bn);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();
        ;
        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("a"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);


      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("d"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testWeightedCplxBN() {
      auto bn = gum::BayesNet< double >::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;",
          3);
      unsharpen(bn);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("a"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("d"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testWeightedAsia() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/asia.bif"));
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == (static_cast< gum::Size >(0)));

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testWeightedAlarm() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == (static_cast< gum::Size >(0)));

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::WeightedSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_WEIGHTED);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_WEIGHTED_SIMPLE_TEST);

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testWeightedInfListener() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == (static_cast< gum::Size >(0)));

      gum::WeightedSampling< double > inf(&bn);
      aSimpleWeightedListener         agsl(inf);
      inf.setVerbosity(true);

      inf.setVerbosity(false);
      inf.setEpsilon(EPSILON_FOR_WEIGHTED);
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());

      CHECK((agsl.getNbr() * inf.periodSize()) == (inf.nbrIterations()));
      CHECK((agsl.getMess()) != (std::string("")));
    }

    static void testEvidenceAsTargetOnCplxBN() {
      auto bn = gum::BayesNet< double >::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;",
          3);
      unsharpen(bn);

      try {
        gum::WeightedSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_WEIGHTED);
        inf.makeInference();
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf.posterior("d"));
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf.posterior(bn.idFromName("d")));

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }
  };

  GUM_TEST_ACTIF(WeightedBinaryTreeWithoutEvidence)
  GUM_TEST_ACTIF(WeightedBinaryTreeWithEvidenceOnRoot)
  GUM_TEST_ACTIF(WeightedBinaryTreeWithEvidenceOnLeaf)
  GUM_TEST_ACTIF(WeightedBinaryTreeWithEvidenceOnMid)
  GUM_TEST_ACTIF(WeightedBinaryTreeWithMultipleEvidence)
  GUM_TEST_ACTIF(WeightedNaryTreeWithMultipleEvidence)
  GUM_TEST_ACTIF(WeightedSimpleBN)
  GUM_TEST_ACTIF(WeightedCplxBN)
  GUM_TEST_ACTIF(WeightedAsia)
  GUM_TEST_ACTIF(WeightedAlarm)
  GUM_TEST_ACTIF(WeightedInfListener)
  GUM_TEST_ACTIF(EvidenceAsTargetOnCplxBN)
}   // namespace gum_tests
