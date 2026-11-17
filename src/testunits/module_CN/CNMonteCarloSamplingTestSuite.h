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


#include <format>
#include <iostream>
#include <string>

#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/threads/threads.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/CN/credalNet.h>
#include <agrum/CN/inference/CNMonteCarloSampling.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>


#define GET_CN_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/cn/" #x

/**
 * @file
 * @brief Mono-threaded version
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 * TestSuite operations are NOT thread safe (yet ?)
 */

namespace gum_tests {
  ////////////////////////////////////////////////////////////////////
  class CNMonteCarloSamplingListener: public gum::ApproximationSchemeListener {
    protected:
    int         nbr_;
    std::string msg_;

    public:
    CNMonteCarloSamplingListener(gum::ApproximationScheme& aS) :
        gum::ApproximationSchemeListener(aS), nbr_(0), msg_("") {}

    void whenProgress(const void* buffer, const gum::Size a, const double b, const double c) {
      nbr_++;
    }

    // the last period is notified by whenStop, not by whenProgress
    void whenStop(const void* buffer, std::string_view s) {
      nbr_++;
      msg_ = s;
    }

    int nbr() { return nbr_; }

    std::string& msg() { return msg_; }
  };   // end of : class mcSamplingListener

  ////////////////////////////////////////////////////////////////
  struct CNMonteCarloSamplingTestSuite {
    protected:

    protected:

    public:
    gum::credal::CredalNet< double >* cn;

    // not dynamic (2U network - fast)
    void initCNet() {
#ifdef _OPENMP
      GUM_DEBUG_ONLY(gum::setNumberOfThreads(1););
#endif

      gum::BayesNet< double >  monBNa;
      gum::BIFReader< double > readera(&monBNa, GET_CN_PATH_STR(2Umin.bif));
      REQUIRE_EQ(readera.proceed(), gum::Size(0));

      gum::BayesNet< double >  monBNb;
      gum::BIFReader< double > readerb(&monBNb, GET_CN_PATH_STR(2Umax.bif));
      REQUIRE_EQ(readerb.proceed(), gum::Size(0));

      cn = new gum::credal::CredalNet< double >(monBNa, monBNb);

      cn->intervalToCredal();
    }

    // dynamic (dynaCheese network - slow)
    void initDCNet() {
#ifdef _OPENMP
      GUM_DEBUG_ONLY(gum::setNumberOfThreads(1);)
#endif

      gum::BayesNet< double >  monBNa;
      gum::BIFReader< double > readera(&monBNa, GET_CN_PATH_STR(bn_c_3.bif));
      REQUIRE_EQ(readera.proceed(), gum::Size(0));

      gum::BayesNet< double >  monBNb;
      gum::BIFReader< double > readerb(&monBNb, GET_CN_PATH_STR(den_c_3.bif));
      REQUIRE_EQ(readerb.proceed(), gum::Size(0));

      cn = new gum::credal::CredalNet< double >(monBNa, monBNb);

      double beta = 0.8;
      cn->bnToCredal(beta, false);
    }

    void clearCNet() { delete cn; }

    // the lower and upper marginals are probabilities with min <= max, and so are the expectations
    void checkTheBounds(const gum::credal::CNMonteCarloSampling< double >& mcs) const {
      const double eps = gum::config().epsilonStandard();
      for (const auto node: cn->current_bn().nodes()) {
        const auto pmin = mcs.marginalMin(node);
        const auto pmax = mcs.marginalMax(node);
        for (gum::Instantiation inst(pmin); !inst.end(); inst.inc()) {
          CHECK(pmin.get(inst) >= -eps);
          CHECK(pmin.get(inst) <= pmax.get(inst) + eps);
          CHECK(pmax.get(inst) <= 1.0 + eps);
        }
        CHECK(mcs.expectationMin(node) <= mcs.expectationMax(node) + eps);
      }
    }

    // not dynamic (2U network) - with evidence
    // end of : testCNMonteCarloSamplingInference (2U network)

    // dynamic (dynaCheese) - strong indep
    // end of : testCNMonteCarloSamplingInferenceDStrong

    // dynamic (dynaCheese) - repetitive indep
    // end of : testCNMonteCarloSamplingInferenceDRep (dynamic - dynacheese)

    // with dynamic network
    // end of : testCNMonteCarloSamplingListener

  };   // end of : class CNMonteCarloSamplingTestSuite

  GUM_TEST(CNMonteCarloSamplingInference) {
    initCNet();

    // using gum::LazyPropagation<double> by default
    gum::credal::CNMonteCarloSampling< double > mcs(*cn);

    // evidence from file
    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.insertEvidenceFile(GET_CN_PATH_STR(L2U.evi)););

    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence(););


    // evidence from map
    std::map< std::string, std::vector< double > > eviMap;
    std::vector< double >                          evi0(2, 0);
    evi0[0] = 1;
    std::vector< double > evi1(2, 0);
    evi1[1]     = 1;
    eviMap["L"] = evi1;
    eviMap["G"] = evi0;


    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.insertEvidence(eviMap););

    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence(););


    mcs.setRepetitiveInd(false);
    mcs.setMaxTime(1);

    // mcs.storeBNOpt ( true );

    std::map< std::string, std::vector< double > > modals;
    std::vector< double >                          binaryModal(2, 0);
    binaryModal[1] = 1;


    // modalities from map
    // from file with dynamic network, not 2U
    try {
      for (const auto node: cn->current_bn().nodes())
        modals[cn->current_bn().variable(node).name()] = binaryModal;
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }


    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.insertModals(modals););
    try {
      mcs.makeInference();
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }


    try {
      checkTheBounds(mcs);
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }


    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence(););

    clearCNet();
  }

  GUM_TEST(CNMonteCarloSamplingInferenceDStrong) {
    initDCNet();

    using exp = std::vector< double >;

    gum::credal::CNMonteCarloSampling< double > mcs(*cn);

    //////////////////////////////////////////////////////
    // strong independence
    //////////////////////////////////////////////////////

    // evidence from file

    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.insertEvidenceFile(GET_CN_PATH_STR(f_3.evi)););

    mcs.setRepetitiveInd(false);
    mcs.setMaxTime(1);

    // modalities from file
    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.insertModalsFile(GET_CN_PATH_STR(modalities.modal)););

    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.makeInference(););

    try {
      checkTheBounds(mcs);
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.dynamicExpectations(););

    try {
      // exp ekm_inf ( mcs.dynamicExpMin ( "km" ) );
      // exp ekm_sup ( mcs.dynamicExpMax ( "km" ) );
      // exp elo_inf ( mcs.dynamicExpMin ( "lo" ) );
      // exp elo_sup ( mcs.dynamicExpMax ( "lo" ) );
      exp etemp_inf(mcs.dynamicExpMin("temp"));
      exp etemp_sup(mcs.dynamicExpMax("temp"));
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence(););

    clearCNet();
  }

  GUM_TEST(CNMonteCarloSamplingInferenceDRep) {
    initDCNet();
    using exp = std::vector< double >;

    gum::credal::CNMonteCarloSampling< double > mcs(*cn);

    //////////////////////////////////////////////////////
    // repetitive independence
    //////////////////////////////////////////////////////

    // evidence from file
    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.insertEvidenceFile(GET_CN_PATH_STR(f_3.evi)););

    mcs.setRepetitiveInd(true);
    mcs.setMaxTime(1);

    // modalities from file
    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.insertModalsFile(GET_CN_PATH_STR(modalities.modal)););

    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.makeInference(););

    try {
      checkTheBounds(mcs);
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.dynamicExpectations(););

    try {
      // exp ekm_inf ( mcs.dynamicExpMin ( "km" ) );
      // exp ekm_sup ( mcs.dynamicExpMax ( "km" ) );
      // exp elo_inf ( mcs.dynamicExpMin ( "lo" ) );
      // exp elo_sup ( mcs.dynamicExpMax ( "lo" ) );
      exp etemp_inf(mcs.dynamicExpMin("temp"));
      exp etemp_sup(mcs.dynamicExpMax("temp"));
    } catch (gum::Exception& e) {
      GUM_SHOWERROR(e);
      CHECK(false);
    }

    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence(););

    clearCNet();
  }

  // each sample has its own seed: the same samples, hence the same result, whatever the number of
  // threads (a seed gives the same result on every machine; the inference engine of each thread may
  // round the posteriors differently)
  GUM_TEST(CNMonteCarloSamplingDoesNotDependOnTheNumberOfThreads) {
    gum::RandomGeneratorGuard randomGuard;   // the seed of the run is given back at the end

    initCNet();
    const auto seed
        = static_cast< unsigned int >(gum::randomValue(1u << 30)) + 1;   // not 0 (clock)

    std::vector< gum::Tensor< double > > results[2];
    for (int run = 0; run < 2; ++run) {
      gum::initRandom(seed);
      gum::credal::CNMonteCarloSampling< double > mcs(*cn);
      mcs.setNumberOfThreads(run == 0 ? 1 : 4);
      mcs.setMaxTime(60);   // stop on epsilon, never on the time
      GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.makeInference());
      // otherwise the numbers of samples differ (a slow machine), hence the results
      REQUIRE(mcs.stateApproximationScheme()
              == gum::IApproximationSchemeConfiguration::ApproximationSchemeSTATE::Epsilon);
      for (const auto node: cn->current_bn().nodes()) {
        results[run].push_back(mcs.marginalMin(node));
        results[run].push_back(mcs.marginalMax(node));
      }
    }
    REQUIRE_EQ(results[0].size(), results[1].size());
    for (std::size_t k = 0; k < results[0].size(); ++k)
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(results[0][k],
                                                    results[1][k],
                                                    gum::config().epsilonStandard());

    clearCNet();
  }

  // the marginals of all the nodes are merged whatever the number of threads at construction (with
  // 1 or 2 threads, the ranges of the threads used to stop after the first nodes, which kept their
  // initial bounds min=1, max=0)
  GUM_TEST(CNMonteCarloSamplingWithFewThreads) {
    initDCNet();
    NumberOfThreadsGuard threads;   // the global number of threads is restored even on failure
    for (const unsigned int nt: {1u, 2u, 4u}) {
      gum::setNumberOfThreads(nt);
      gum::credal::CNMonteCarloSampling< double > mcs(*cn);
      mcs.setMaxTime(60);   // stop on epsilon, never on the time
      GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.insertModalsFile(GET_CN_PATH_STR(modalities.modal)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.makeInference());
      checkTheBounds(mcs);
    }
    clearCNet();
  }

  // the constructor sets the default criteria; the inference keeps the user's ones
  GUM_TEST(CNMonteCarloSamplingKeepsTheUserCriteria) {
    initCNet();
    gum::credal::CNMonteCarloSampling< double > mcs(*cn);

    const double floor = 1000 * std::numeric_limits< double >::epsilon();
    CHECK(mcs.isEnabledEpsilon());
    CHECK_EQ(mcs.epsilon(), floor);
    CHECK(!mcs.isEnabledMinEpsilonRate());
    CHECK(!mcs.isEnabledMaxIter());
    CHECK(mcs.isEnabledMaxTime());

    // below the floor, the result would depend on the number of threads
    CHECK_THROWS_AS(mcs.setEpsilon(floor / 2), gum::OutOfBounds);
    CHECK_THROWS_AS(mcs.setEpsilon(0), gum::OutOfBounds);
    CHECK_EQ(mcs.epsilon(), floor);

    // only maxIter stops the inference
    mcs.disableEpsilon();
    mcs.setMaxIter(3000);
    mcs.setMaxTime(60);
    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.makeInference());
    CHECK_EQ(mcs.stateApproximationScheme(),
             gum::IApproximationSchemeConfiguration::ApproximationSchemeSTATE::Limit);
    CHECK_EQ(mcs.nbrIterations(), static_cast< gum::Size >(3000));
    CHECK(!mcs.isEnabledEpsilon());
    CHECK(mcs.isEnabledMaxIter());

    clearCNet();
  }

  GUM_TEST(CNMonteCarloSamplingDefaultsComeFromTheConfiguration) {
    initCNet();
    {
      gum::credal::CNMonteCarloSampling< double > mcs(*cn);
      CHECK_EQ(mcs.maxTime(), 60.);
      CHECK(mcs.isEnabledMaxTime());
      CHECK_EQ(mcs.periodSize(), static_cast< gum::Size >(1000));
    }
    {
      auto&              conf = gum::Configuration::instance();
      ConfigurationGuard guard;
      conf.set("CNMontecarlo", "maxTime", "12.5");
      conf.set("CNMontecarlo", "periodSize", "20");
      gum::credal::CNMonteCarloSampling< double > mcs(*cn);
      CHECK_EQ(mcs.maxTime(), 12.5);
      CHECK(mcs.isEnabledMaxTime());
      CHECK_EQ(mcs.periodSize(), static_cast< gum::Size >(20));
    }
    clearCNet();
  }

  GUM_TEST(CNMonteCarloSamplingHistoryHasOneErrorPerPeriod) {
    // the history only contains the errors of the sampled periods
    initCNet();
    gum::credal::CNMonteCarloSampling< double > mcs(*cn);
    mcs.setVerbosity(true);
    mcs.disableEpsilon();
    mcs.setPeriodSize(100);
    mcs.setMaxIter(500);
    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.makeInference());
    CHECK_EQ(mcs.nbrIterations(), static_cast< gum::Size >(500));
    CHECK_EQ(mcs.history().size(), static_cast< gum::Size >(5));
    clearCNet();
  }

  GUM_TEST(CNMonteCarloSamplingListener) {
    initDCNet();
    gum::credal::CNMonteCarloSampling< double > mcs(*cn);

    // evidence from file
    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.insertEvidenceFile(GET_CN_PATH_STR(f_3.evi)););

    mcs.setRepetitiveInd(false);
    mcs.setMaxTime(1);

    /// mcs.setBurnIn( 100000 ); // so we can test time out criterion during
    /// burn
    /// in
    mcs.setPeriodSize(10);

    CNMonteCarloSamplingListener mcl(mcs);

    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.makeInference(););

    /// time out criterion during burn in test
    CHECK_EQ(mcl.nbr() * mcs.periodSize(), mcs.nbrIterations());
    CHECK_NE(mcl.msg(), std::string(""));

    GUM_CHECK_ASSERT_THROWS_NOTHING(mcs.eraseAllEvidence(););

    clearCNet();
  }

  GUM_TEST(ApproximatedBinarizationKeepsTheParentConfigurations) {
    // a precise credal network (min = max) binarized with parents whose domain size is not a power
    // of 2: sampling gives the exact posteriors of the Bayesian network
    auto                           bn = gum::BayesNet< double >::fastPrototype("X[3]->Y[2]<-Z[3]");
    gum::LazyPropagation< double > exact(&bn);
    exact.makeInference();

    gum::credal::CredalNet< double > cnet(bn, bn);
    cnet.intervalToCredal();
    cnet.approximatedBinarization();
    cnet.computeBinaryCPTMinMax();

    gum::credal::CNMonteCarloSampling< double > mc(cnet);
    mc.makeInference();

    // probability of the k-th value in a tensor over one variable
    const auto proba = [](const gum::Tensor< double >& t, gum::Idx k) {
      gum::Instantiation inst(t);
      inst.chgVal(0, k);
      return t.get(inst);
    };
    const auto& bin = cnet.current_bn();
    CHECK_EQ(proba(mc.marginalMin(bin.idFromName("Y")), 1),
             doctest::Approx(proba(exact.posterior("Y"), 1)).epsilon(1e-9));
    for (const std::string name: {"X", "Z"})
      for (gum::Idx k = 0; k < 3; ++k) {
        const auto indic = bin.idFromName(std::format("{}-v{}", name, k));
        CHECK_EQ(proba(mc.marginalMin(indic), 1),
                 doctest::Approx(proba(exact.posterior(name), k)).epsilon(1e-9));
      }
  }

}   // namespace gum_tests
