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


#include <cmath>
#include <string>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/exceptions.h>
#include <agrum/BN/algorithms/divergence/exactBNdistance.h>
#include <agrum/BN/algorithms/divergence/GibbsBNdistance.h>
#include <agrum/BN/algorithms/divergence/MCBNDistance.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/GibbsSampling.h>
#include <agrum/BN/inference/importanceSampling.h>
#include <agrum/BN/inference/loopyBeliefPropagation.h>
#include <agrum/BN/inference/loopySamplingInference.h>
#include <agrum/BN/inference/MonteCarloSampling.h>
#include <agrum/BN/inference/weightedSampling.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The default parameters of the approximation schemes of the BN module come from the
  // configuration (sections sampling, gibbs, lbp, loopySampling, gibbsKL and mcKL). The values
  // checked here are the ones that were written in the code before.
  struct ApproximationConfigurationTestSuite {
    public:
    static gum::BayesNet< double > buildBN() {
      // 10 nodes. The number of variables drawn between two samples of a Gibbs sampling is
      // 1 + 10 * percent / 100 (50% gives 6, 10% gives 2), and it is bounded by the number of nodes
      // only when the sampling starts (so 100% gives 11 here).
      return gum::BayesNet< double >::fastPrototype("a->b->c->d->e->f->g->h->i->j");
    }

    static void checkScheme(const gum::IApproximationSchemeConfiguration& scheme,
                            double                                        epsilon,
                            double                                        rate,
                            gum::Size                                     maxIter,
                            gum::Size                                     period,
                            bool                                          verbosity) {
      // a tolerance, since some defaults are written with a decimal literal (exp(-1.6) for Gibbs)
      CHECK_LT(std::abs(scheme.epsilon() - epsilon), 1e-15);
      CHECK_LT(std::abs(scheme.minEpsilonRate() - rate), 1e-15);
      CHECK_EQ(scheme.maxIter(), maxIter);
      CHECK_EQ(scheme.periodSize(), period);
      CHECK_EQ(scheme.verbosity(), verbosity);
    }

    static void checkSampling(const gum::IApproximationSchemeConfiguration& scheme) {
      checkScheme(scheme, 1e-2, 1e-5, 10000000, 100, false);
      CHECK_EQ(scheme.maxTime(), 6000.);
      CHECK(scheme.isEnabledMaxTime());
    }

    static void checkKL(const gum::IApproximationSchemeConfiguration& scheme) {
      checkScheme(scheme, 1e-10, 1e-10, 10000000, 200, false);
      CHECK_EQ(scheme.maxTime(), 6000.);
      CHECK(scheme.isEnabledMaxTime());
    }
  };

  GUM_TEST(SamplingDefaults) {
    ConfigurationGuard guard;
    auto               bn = buildBN();

    checkSampling(gum::MonteCarloSampling< double >(&bn));
    checkSampling(gum::WeightedSampling< double >(&bn));
    checkSampling(gum::ImportanceSampling< double >(&bn));
  }

  GUM_TEST(GibbsDefaults) {
    ConfigurationGuard           guard;
    auto                         bn = buildBN();
    gum::GibbsSampling< double > gibbs(&bn);

    checkScheme(gibbs, std::exp(-1.6), std::exp(-5), 10000000, 100, false);
    CHECK_EQ(gibbs.maxTime(), 6000.);
    CHECK_EQ(gibbs.burnIn(), gum::Size(300));
    CHECK_EQ(gibbs.nbrDrawnVar(), gum::Size(6));
    CHECK(gibbs.isDrawnAtRandom());
  }

  GUM_TEST(LoopyBeliefPropagationDefaults) {
    ConfigurationGuard                    guard;
    auto                                  bn = buildBN();
    gum::LoopyBeliefPropagation< double > lbp(&bn);

    checkScheme(lbp, 1e-8, 1e-10, 100, 1, false);
    // no timeout for the loopy belief propagation
    CHECK_EQ(lbp.maxTime(), 1.);
    CHECK(!lbp.isEnabledMaxTime());
  }

  GUM_TEST(HybridSamplingDefaults) {
    ConfigurationGuard guard;
    auto               bn = buildBN();

    gum::HybridMonteCarloSampling< double > hybridMC(&bn);
    CHECK_EQ(hybridMC.virtualLBPSize(), 5000.);
    checkSampling(hybridMC);

    gum::HybridGibbsSampling< double > hybridGibbs(&bn);
    CHECK_EQ(hybridGibbs.virtualLBPSize(), 5000.);
    CHECK_EQ(hybridGibbs.burnIn(), gum::Size(300));
  }

  GUM_TEST(GibbsKLDefaults) {
    ConfigurationGuard guard;
    auto               bn1 = buildBN();
    auto               bn2 = buildBN();

    gum::GibbsBNdistance< double > fromBNs(bn1, bn2);
    checkKL(fromBNs);
    CHECK_EQ(fromBNs.burnIn(), gum::Size(2000));
    CHECK_EQ(fromBNs.nbrDrawnVar(), gum::Size(2));
    CHECK(fromBNs.isDrawnAtRandom());

    gum::ExactBNdistance< double > exact(bn1, bn2);
    gum::GibbsBNdistance< double > fromKL(exact);
    checkKL(fromKL);
    CHECK_EQ(fromKL.burnIn(), gum::Size(2000));
    CHECK_EQ(fromKL.nbrDrawnVar(), gum::Size(2));
    // the two constructors behave the same way
    CHECK(fromKL.isDrawnAtRandom());
  }

  GUM_TEST(MonteCarloKLDefaults) {
    ConfigurationGuard guard;
    auto               bn1 = buildBN();
    auto               bn2 = buildBN();

    gum::MCBNDistance< double > fromBNs(bn1, bn2);
    checkKL(fromBNs);

    gum::ExactBNdistance< double > exact(bn1, bn2);
    gum::MCBNDistance< double >    fromKL(exact);
    checkKL(fromKL);
  }

  GUM_TEST(SectionsAreIndependent) {
    ConfigurationGuard guard;
    auto               bn   = buildBN();
    auto&              conf = gum::Configuration::instance();

    conf.set("sampling", "epsilon", "0.5");
    conf.set("sampling", "maxTime", "10");
    conf.set("sampling", "periodSize", "7");
    gum::MonteCarloSampling< double > mc(&bn);
    checkScheme(mc, 0.5, 1e-5, 10000000, 7, false);
    CHECK_EQ(mc.maxTime(), 10.);

    // Gibbs sampling reads the sampling section and then overrides epsilon and rate with its own
    // section
    gum::GibbsSampling< double > gibbs(&bn);
    CHECK_EQ(gibbs.periodSize(), gum::Size(7));
    CHECK_LT(std::abs(gibbs.epsilon() - std::exp(-1.6)), 1e-15);

    // other schemes are not concerned
    checkScheme(gum::LoopyBeliefPropagation< double >(&bn), 1e-8, 1e-10, 100, 1, false);
    checkKL(gum::MCBNDistance< double >(bn, bn));
    conf.reset();   // the defaults again

    checkSampling(gum::MonteCarloSampling< double >(&bn));
  }

  GUM_TEST(GibbsParametersAreReadFromTheConfiguration) {
    auto  bn   = buildBN();
    auto& conf = gum::Configuration::instance();

    ConfigurationGuard guard;
    conf.set("gibbs", "epsilon", "0.3");
    conf.set("gibbs", "minEpsilonRate", "0.2");
    conf.set("gibbs", "burnIn", "11");
    conf.set("gibbs", "percentDrawnSample", "100");
    conf.set("gibbs", "drawnAtRandom", "False");

    gum::GibbsSampling< double > gibbs(&bn);
    CHECK_EQ(gibbs.epsilon(), 0.3);
    CHECK_EQ(gibbs.minEpsilonRate(), 0.2);
    CHECK_EQ(gibbs.burnIn(), gum::Size(11));
    CHECK_EQ(gibbs.nbrDrawnVar(), gum::Size(11));
    CHECK(!gibbs.isDrawnAtRandom());

    // a percentage out of [0, 100] is refused by the configuration, and the value is kept
    CHECK_THROWS_AS(conf.set("gibbs", "percentDrawnSample", "500"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("gibbs", "percentDrawnSample", "-5"), gum::InvalidArgument);
    CHECK_EQ(gum::GibbsSampling< double >(&bn).nbrDrawnVar(), gum::Size(11));
    conf.set("gibbs", "percentDrawnSample", "0");
    CHECK_EQ(gum::GibbsSampling< double >(&bn).nbrDrawnVar(), gum::Size(1));
  }

  GUM_TEST(LoopyParametersAreReadFromTheConfiguration) {
    auto  bn   = buildBN();
    auto& conf = gum::Configuration::instance();

    ConfigurationGuard guard;
    conf.set("lbp", "maxIter", "3");
    conf.set("lbp", "epsilon", "0.1");
    conf.set("lbp", "verbosity", "True");
    conf.set("loopySampling", "virtualLBPSize", "12");

    checkScheme(gum::LoopyBeliefPropagation< double >(&bn), 0.1, 1e-10, 3, 1, true);
    CHECK_EQ(gum::HybridWeightedSampling< double >(&bn).virtualLBPSize(), 12.);
  }

  GUM_TEST(KLParametersAreReadFromTheConfiguration) {
    auto  bn1  = buildBN();
    auto  bn2  = buildBN();
    auto& conf = gum::Configuration::instance();

    ConfigurationGuard guard;
    conf.set("gibbsKL", "burnIn", "13");
    conf.set("gibbsKL", "maxTime", "42");
    conf.set("gibbsKL", "percentDrawnSample", "30");
    conf.set("gibbsKL", "drawnAtRandom", "False");
    conf.set("mcKL", "maxIter", "99");
    conf.set("mcKL", "periodSize", "9");

    gum::ExactBNdistance< double > exact(bn1, bn2);
    gum::GibbsBNdistance< double > fromKL(exact);
    CHECK_EQ(fromKL.burnIn(), gum::Size(13));
    CHECK_EQ(fromKL.maxTime(), 42.);
    CHECK_EQ(fromKL.nbrDrawnVar(), gum::Size(4));
    CHECK(!fromKL.isDrawnAtRandom());
    gum::GibbsBNdistance< double > fromBNs(bn1, bn2);
    CHECK_EQ(fromBNs.burnIn(), gum::Size(13));
    CHECK(!fromBNs.isDrawnAtRandom());

    gum::MCBNDistance< double > mc(bn1, bn2);
    CHECK_EQ(mc.maxIter(), gum::Size(99));
    CHECK_EQ(mc.periodSize(), gum::Size(9));
  }

  GUM_TEST(InvalidParametersAreRefused) {
    ConfigurationGuard guard;
    auto               bn   = buildBN();
    auto&              conf = gum::Configuration::instance();

    // an invalid value is refused by the configuration itself (domain of the key)
    CHECK_THROWS_AS(conf.set("lbp", "periodSize", "0"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("sampling", "maxIter", "-3"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("gibbs", "burnIn", "-1"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("gibbs", "percentDrawnSample", "101"), gum::InvalidArgument);
    conf.reset();   // the defaults again

    checkSampling(gum::MonteCarloSampling< double >(&bn));
  }
}   // namespace gum_tests
