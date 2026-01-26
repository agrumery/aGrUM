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
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/importanceSampling.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <gumtest/AgrumApproximationUtils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  ImportanceSampling
#define GUM_CURRENT_MODULE BN
// must be last include

#define EPSILON_FOR_IMPORTANCE_SIMPLE_TEST 15e-2
#define EPSILON_FOR_IMPORTANCE             7e-2

namespace gum_tests {

  class aSimpleImportanceListener: public gum::ApproximationSchemeListener {
    private:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleImportanceListener(gum::ApproximationScheme& sch) :
        gum::ApproximationSchemeListener(sch), __nbr(0), __mess("") {};

    void whenProgress(const void* buffer, const gum::Size a, const double b, const double c) {
      __nbr++;
    }

    void whenStop(const void* buffer, const std::string& s) {
      __nbr++;
      __mess = s;
    }

    int getNbr() { return __nbr; }

    std::string getMess() { return __mess; }
  };

  struct ImportanceSamplingTestSuite {
    public:
    static void testImportanceBinaryTreeWithoutEvidence() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      gum::LazyPropagation< double > lazy(&bn);
      lazy.makeInference();


      try {
        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);

          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }   // namespace gum_tests

    static void testImportanceBinaryTreeWithEvidenceOnRoot() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "b";

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testImportanceBinaryTreeWithEvidenceOnLeaf() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "h";

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.setVerbosity(false);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testImportanceBinaryTreeWithEvidenceOnMid() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);
      std::string ev = "e";

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName(ev), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName(ev), 0);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.setVerbosity(false);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testImportanceBinaryTreeWithMultipleEvidence() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->d->f;b->d->g;b->e->h;c->e;i->j->h");
      unsharpen(bn);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("e"), 0);
        lazy.addEvidence(bn.idFromName("b"), 1);
        lazy.addEvidence(bn.idFromName("h"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("e"), 0);
          inf.addEvidence(bn.idFromName("b"), 1);
          inf.addEvidence(bn.idFromName("h"), 0);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.setVerbosity(false);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testImportanceNaryTreeWithMultipleEvidence() {
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
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("e"), 0);
          inf.addEvidence(bn.idFromName("b"), 1);
          inf.addEvidence(bn.idFromName("h"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testImportanceSimpleBN() {
      auto bn = gum::BayesNet< double >::fastPrototype("a->b->c;a->d->c", 3);
      unsharpen(bn);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("a"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("d"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testImportanceCplxBN() {
      auto bn = gum::BayesNet< double >::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;",
          3);
      unsharpen(bn);

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("a"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("a"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }


      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.addEvidence(bn.idFromName("d"), 0);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.addEvidence(bn.idFromName("d"), 0);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testImportanceEvidenceAsTargetOnCplxBN() {
      auto bn = gum::BayesNet< double >::fastPrototype(
          "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;",
          3);
      unsharpen(bn);

      try {
        gum::ImportanceSampling< double > inf(&bn);
        inf.addEvidence(bn.idFromName("d"), 0);
        CHECK_THROWS_AS(inf.addEvidence("i", std::vector< double >{1, 0, 1}),
                        const gum::FatalError&);

        inf.setVerbosity(false);
        inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
        inf.makeInference();
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf.posterior("d"));
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf.posterior(bn.idFromName("d")));

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testImportanceAsia() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/asia.bif"));
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == (static_cast< gum::Size >(0)));

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testImportanceAlarm() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == (static_cast< gum::Size >(0)));

      try {
        gum::LazyPropagation< double > lazy(&bn);
        lazy.makeInference();

        GUM_APPROX_TEST_BEGIN_ITERATION
          gum::ImportanceSampling< double > inf(&bn);
          inf.setVerbosity(false);
          inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
          inf.makeInference();
        GUM_APPROX_TEST_END_ITERATION(EPSILON_FOR_IMPORTANCE_SIMPLE_TEST)

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testImportanceDiabetes() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/Diabetes.bif"));
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == (static_cast< gum::Size >(0)));

      try {
        gum::ImportanceSampling< double > inf(&bn);
        inf.setVerbosity(false);
        inf.setMaxTime(5);
        inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
        inf.makeInference();

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
      CHECK(true);
    }

    static void testImportanceInfListener() {
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == (static_cast< gum::Size >(0)));

      gum::ImportanceSampling< double > inf(&bn);
      aSimpleImportanceListener         agsl(inf);
      inf.setVerbosity(false);

      inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());

      CHECK((agsl.getNbr() * inf.periodSize()) == (inf.nbrIterations()));
      CHECK((agsl.getMess()) != (std::string("")));
    }
  };

  GUM_TEST_ACTIF(ImportanceBinaryTreeWithoutEvidence)
  GUM_TEST_ACTIF(ImportanceBinaryTreeWithEvidenceOnRoot)
  GUM_TEST_ACTIF(ImportanceBinaryTreeWithEvidenceOnLeaf)
  GUM_TEST_ACTIF(ImportanceBinaryTreeWithEvidenceOnMid)
  GUM_TEST_ACTIF(ImportanceBinaryTreeWithMultipleEvidence)
  GUM_TEST_ACTIF(ImportanceNaryTreeWithMultipleEvidence)
  GUM_TEST_ACTIF(ImportanceSimpleBN)
  GUM_TEST_ACTIF(ImportanceCplxBN)
  GUM_TEST_ACTIF(ImportanceEvidenceAsTargetOnCplxBN)
  GUM_TEST_ACTIF(ImportanceAsia)
  GUM_TEST_ACTIF(ImportanceAlarm)
  GUM_TEST_ACTIF(ImportanceDiabetes)
  GUM_TEST_ACTIF(ImportanceInfListener)
}   // namespace gum_tests
