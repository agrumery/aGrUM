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
#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/importanceSampling.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <gumtest/AgrumApproximationUtils.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

// must be last include

#define EPSILON_FOR_IMPORTANCE_SIMPLE_TEST 15e-2
#define EPSILON_FOR_IMPORTANCE             7e-2

namespace gum_tests {

  class aSimpleImportanceListener: public gum::ApproximationSchemeListener {
    protected:
    int         __nbr;
    std::string __mess;

    public:
    aSimpleImportanceListener(gum::ApproximationScheme& sch) :
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

  struct ImportanceSamplingTestSuite {
    public:
    // namespace gum_tests
  };

  GUM_TEST(ImportanceBinaryTreeWithoutEvidence) {
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
  }

  GUM_TEST(ImportanceBinaryTreeWithEvidenceOnRoot) {
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

  GUM_TEST(ImportanceBinaryTreeWithEvidenceOnLeaf) {
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

  GUM_TEST(ImportanceBinaryTreeWithEvidenceOnMid) {
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

  GUM_TEST(ImportanceBinaryTreeWithMultipleEvidence) {
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

  GUM_TEST(ImportanceNaryTreeWithMultipleEvidence) {
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

  GUM_TEST(ImportanceSimpleBN) {
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

  GUM_TEST(ImportanceCplxBN) {
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

  GUM_TEST(ImportanceEvidenceAsTargetOnCplxBN) {
    auto bn = gum::BayesNet< double >::fastPrototype(
        "a->d->f;b->d->g;b->e->h;c->e->g;i->j->h;c->j;x->c;x->j;",
        3);
    unsharpen(bn);

    try {
      gum::ImportanceSampling< double > inf(&bn);
      inf.addEvidence(bn.idFromName("d"), 0);
      CHECK_THROWS_AS(inf.addEvidence("i", std::vector< double >{1, 0, 1}), const gum::FatalError&);

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

  GUM_TEST(ImportanceAsia) {
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/asia.bif"));
    gum::Size                nbrErr = static_cast< gum::Size >(0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
    CHECK_EQ(nbrErr, static_cast< gum::Size >(0));

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

  GUM_TEST(ImportanceAlarm) {
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
    gum::Size                nbrErr = static_cast< gum::Size >(0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
    CHECK_EQ(nbrErr, static_cast< gum::Size >(0));

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

  GUM_TEST(ImportanceDiabetes) {
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/Diabetes.bif"));
    gum::Size                nbrErr = static_cast< gum::Size >(0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
    CHECK_EQ(nbrErr, static_cast< gum::Size >(0));

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

  GUM_TEST(ImportanceInfListener) {
    gum::BayesNet< double >  bn;
    gum::BIFReader< double > reader(&bn, GET_RESSOURCES_PATH("bif/alarm.bif"));
    gum::Size                nbrErr = static_cast< gum::Size >(0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
    CHECK_EQ(nbrErr, static_cast< gum::Size >(0));

    gum::ImportanceSampling< double > inf(&bn);
    aSimpleImportanceListener         agsl(inf);
    inf.setVerbosity(false);

    inf.setEpsilon(EPSILON_FOR_IMPORTANCE);
    GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());

    CHECK_EQ(agsl.getNbr() * inf.periodSize(), inf.nbrIterations());
    CHECK_NE(agsl.getMess(), std::string(""));
  }

  GUM_TEST(ImportanceWeightWithEvidenceNotLastInTopologicalOrder) {
    // The importance weight of a sample is the product of P/Q over all the nodes. With evidence
    // on B (not the last node in topological order), keeping only the last ratio would drop the
    // likelihood of the evidence and the estimate of P(A|B=1) would collapse onto the prior P(A).
    auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");
    bn.cpt("A").fillWith({0.2, 0.8});
    bn.cpt("B").fillWith({0.9, 0.1, 0.1, 0.9});
    bn.cpt("C").fillWith({0.9, 0.1, 0.1, 0.9});

    gum::LazyPropagation< double > lazy(&bn);
    lazy.addEvidence("B", 1);
    lazy.makeInference();

    gum::ImportanceSampling< double > inf(&bn);
    inf.addEvidence("B", 1);
    inf.setEpsilon(1e-3);
    inf.setVerbosity(false);
    inf.makeInference();

    // exact P(A=1|B=1) = 0.72/0.74 ~ 0.973, prior P(A=1) = 0.8
    GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(lazy.posterior("A"), inf.posterior("A"), 0.05);
  }

  GUM_TEST(ApproxTestMacroDetectsFailure) {
    // the approximation loop must detect that none of the tries is close enough to the exact
    // inference: with a null tolerance, a sampled posterior never matches the exact one.
    auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");

    gum::LazyPropagation< double > lazy(&bn);
    lazy.makeInference();

    GUM_APPROX_TEST_BEGIN_ITERATION
      gum::ImportanceSampling< double > inf(&bn);
      inf.setMaxIter(10);
      inf.setVerbosity(false);
      inf.makeInference();
    GUM_APPROX_TEST_END_ITERATION_NO_FAIL(0.0)
    CHECK_FALSE(res);
  }

  GUM_TEST(ImportanceWithSharpAndNullParameters) {
    // issue #196: a tiny parameter (< epsilon/maxDomainSize) triggers the unsharpening of the
    // proposal; the estimate must stay consistent with the exact inference
    auto bn = gum::BayesNet< double >::fastPrototype("A->B[3]->C");
    bn.cpt("A").fillWith({0.5, 0.5});
    bn.cpt("B").fillWith({1.0, 0.0, 0.0, 0.4999995, 1e-6, 0.4999995});
    bn.cpt("C").fillWith({0.9, 0.1, 0.01, 0.99, 0.5, 0.5});

    gum::LazyPropagation< double > lazy(&bn);
    lazy.addEvidence("C", 1);
    lazy.makeInference();

    gum::ImportanceSampling< double > inf(&bn);
    inf.addEvidence("C", 1);
    inf.setEpsilon(1e-2);
    inf.setVerbosity(false);
    inf.makeInference();

    GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(lazy.posterior("A"), inf.posterior("A"), 0.05);
    GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(lazy.posterior("B"), inf.posterior("B"), 0.05);
  }

  GUM_TEST(ImportanceAfterEvidenceChange) {
    // the sampling BN was built once for the evidence of the first inference: after a change of
    // the value of the evidence, the proposal kept the former value and the sampling never ended
    auto bn = gum::BayesNet< double >::fastPrototype("A->B");
    bn.cpt("A").fillWith({0.3, 0.7});
    bn.cpt("B").fillWith({0.9, 0.1, 0.2, 0.8});

    gum::ImportanceSampling< double > inf(&bn);
    inf.setEpsilon(1e-3);
    inf.setVerbosity(false);
    inf.addEvidence("B", 0);
    inf.makeInference();
    inf.chgEvidence("B", 1);
    inf.makeInference();

    gum::LazyPropagation< double > lazy(&bn);
    lazy.addEvidence("B", 1);
    lazy.makeInference();
    GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(lazy.posterior("A"), inf.posterior("A"), 0.05);
  }

  GUM_TEST(ImportanceAfterTargetChange) {
    // the barren nodes were computed for the targets of the first inference: a node added as a
    // target afterwards was not installed in the sampling BN (NotFound)
    auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");
    bn.cpt("A").fillWith({0.3, 0.7});
    bn.cpt("B").fillWith({0.9, 0.1, 0.2, 0.8});
    bn.cpt("C").fillWith({0.6, 0.4, 0.1, 0.9});

    gum::ImportanceSampling< double > inf(&bn);
    inf.setEpsilon(1e-3);
    inf.setVerbosity(false);
    inf.addTarget("A");
    inf.addEvidence("B", 1);
    inf.makeInference();
    inf.addTarget("C");
    GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());

    gum::LazyPropagation< double > lazy(&bn);
    lazy.addEvidence("B", 1);
    lazy.makeInference();
    GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(lazy.posterior("C"), inf.posterior("C"), 0.05);
  }

  GUM_TEST(ImportanceEvidenceImpact) {
    auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");
    bn.cpt("A").fillWith({0.3, 0.7});
    bn.cpt("B").fillWith({0.9, 0.1, 0.2, 0.8});
    bn.cpt("C").fillWith({0.6, 0.4, 0.1, 0.9});

    gum::LazyPropagation< double > lazy(&bn);
    for (const auto& evs: {std::vector< std::string >{"B"}, std::vector< std::string >{"C"}}) {
      gum::ImportanceSampling< double > inf(&bn);
      inf.setEpsilon(1e-3);
      inf.setVerbosity(false);
      const auto impact = inf.evidenceImpact("A", evs);
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(lazy.evidenceImpact("A", evs), impact, 0.05);
    }
  }

  // the proposal of ImportanceSampling (copies of the CPTs) follows a CPT changed in place after
  // invalidateTensors()
  GUM_TEST(ImportanceInvalidateTensors) {
    auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");
    bn.cpt("A").fillWith({0.2, 0.8});
    bn.cpt("B").fillWith({0.9, 0.1, 0.3, 0.7});
    bn.cpt("C").fillWith({0.6, 0.4, 0.1, 0.9});

    gum::ImportanceSampling< double > ie(&bn);
    ie.setEpsilon(1e-3);
    ie.setVerbosity(false);
    ie.addEvidence("B", 1);
    ie.makeInference();

    const_cast< gum::Tensor< double >& >(bn.cpt("A")).fillWith({0.999, 0.001});
    ie.invalidateTensors();
    ie.makeInference();

    gum::LazyPropagation< double > lazy(&bn);
    lazy.addEvidence("B", 1);
    lazy.makeInference();
    GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(lazy.posterior("A"),
                                                  ie.posterior("A"),
                                                  gum::config().epsilonCoarse() * 5);
  }
}   // namespace gum_tests
