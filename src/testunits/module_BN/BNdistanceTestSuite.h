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


#define TESTKL_MAX_ITER_GIBBS_KL 15
#define TESTKL_MAX_ITER_MC_KL    15
#include <iostream>
#include <string>
#include <vector>

#include <agrum/BN/algorithms/divergence/exactBNdistance.h>
#include <agrum/BN/algorithms/divergence/GibbsBNdistance.h>
#include <agrum/BN/algorithms/divergence/MCBNDistance.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  struct BNdistanceTestSuite {
    public:
    // namespace gum_tests


  };   // namespace gum_tests

  GUM_TEST(Constructor) {
    gum::BayesNet< double > net1;
    {
      gum::BIFReader< double > reader(&net1, GET_RESSOURCES_PATH("bif/BIFReader_file2.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::BayesNet< double > net2;
    {
      gum::BIFReader< double > reader(&net2, GET_RESSOURCES_PATH("bif/BIFReader_file3.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::BayesNet< double > net3;
    {
      gum::BIFReader< double > reader(&net3, GET_RESSOURCES_PATH("bif/BIFReader_file4.bif"));
      reader.trace(false);
      reader.proceed();
    }

    GUM_CHECK_ASSERT_THROWS_NOTHING(gum::ExactBNdistance< double > kl(net1, net1));
    CHECK_THROWS_AS(gum::ExactBNdistance< double > kl(net1, net2), const gum::OperationNotAllowed&);
    CHECK_THROWS_AS(gum::ExactBNdistance< double > kl(net2, net3), const gum::OperationNotAllowed&);
  }

  GUM_TEST(ComparableBNsForAnyDiscreteVariables) {
    {
      const std::string typevar = "";
      auto bn1 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      auto bn2 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      gum::ExactBNdistance< double > kl(bn1, bn2);
    }
    {
      const std::string typevar = "{yes|no}";
      auto bn1 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      auto bn2 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      gum::ExactBNdistance< double > kl(bn1, bn2);
    }
    {
      const std::string typevar = "{1|2|3|4|5}";
      auto bn1 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      auto bn2 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      gum::ExactBNdistance< double > kl(bn1, bn2);
    }
    {
      const std::string typevar = "{1.2|5.4|5.5|6.3}";
      auto bn1 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      auto bn2 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      gum::ExactBNdistance< double > kl(bn1, bn2);
    }
    {
      const std::string typevar = "[5]";
      auto bn1 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      auto bn2 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      gum::ExactBNdistance< double > kl(bn1, bn2);
    }
    {
      const std::string typevar = "[2,5]";
      auto bn1 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      auto bn2 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      gum::ExactBNdistance< double > kl(bn1, bn2);
    }
    {
      const std::string typevar = "[1.4,2.5,4.5,7.7]";
      auto bn1 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      auto bn2 = gum::BayesNet< double >::fastPrototype("A" + typevar + "->B" + typevar);
      gum::ExactBNdistance< double > kl(bn1, bn2);
    }
  }

  GUM_TEST(Difficulty1) {
    gum::BayesNet< double > net2;
    {
      gum::BIFReader< double > reader(&net2, GET_RESSOURCES_PATH("bif/BIFReader_file3.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::ExactBNdistance< double > kl(net2, net2);
    CHECK_EQ(kl.difficulty(), gum::Complexity::Correct);

    gum::BayesNet< double > net;
    {
      gum::BIFReader< double > reader(&net, GET_RESSOURCES_PATH("bif/hailfinder.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::BNdistance< double > kl2(net, net);
    CHECK_EQ(kl2.difficulty(), gum::Complexity::Heavy);
  }

  GUM_TEST(KLComputation) {
    gum::BayesNet< double > net3;
    {
      gum::BIFReader< double > reader(&net3, GET_RESSOURCES_PATH("bif/BIFReader_file3.bif"));
      reader.trace(false);
      reader.proceed();
    }

    double                         vkl = 0.0;
    gum::ExactBNdistance< double > stupid_bfkl(net3, net3);
    CHECK_EQ(stupid_bfkl.difficulty(), gum::Complexity::Correct);
    GUM_CHECK_ASSERT_THROWS_NOTHING(vkl = stupid_bfkl.klPQ());
    CHECK_EQ(vkl, 0.0);
    GUM_CHECK_ASSERT_THROWS_NOTHING(vkl = stupid_bfkl.klQP());
    CHECK_EQ(vkl, 0.0);

    gum::BayesNet< double > net4;
    {
      gum::BIFReader< double > reader(&net4, GET_RESSOURCES_PATH("bif/BIFReader_file4.bif"));
      reader.trace(false);
      reader.proceed();
    }

    CHECK_THROWS_AS(gum::BNdistance< double > kl(net3, net4), const gum::OperationNotAllowed&);

    gum::BayesNet< double > net5;
    {
      gum::BIFReader< double > reader(&net5, GET_RESSOURCES_PATH("bif/BIFReader_file5.bif"));
      reader.trace(false);
      reader.proceed();
    }
    gum::BNdistance< double > kl(net3, net5);
    CHECK_EQ(kl.difficulty(), gum::Complexity::Correct);
    {
      gum::ExactBNdistance< double > gkl(kl);
      GUM_CHECK_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ());
      CHECK_NE(vkl, static_cast< float >(0.0));
    }
    {
      gum::GibbsBNdistance< double > gkl(kl);
      gkl.setMaxIter(40);
      GUM_CHECK_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ());
      CHECK_NE(vkl, static_cast< float >(0.0));
      CHECK_THROWS_AS(gkl.history(), const gum::OperationNotAllowed&);
    }

    {
      gum::GibbsBNdistance< double > gkl(kl);
      gkl.setMaxIter(40);
      GUM_CHECK_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ());
      CHECK_NE(vkl, static_cast< float >(0.0));
      CHECK_THROWS_AS(gkl.history(), const gum::OperationNotAllowed&);
    }

    {
      gum::MCBNDistance< double > mckl(kl);
      mckl.setMaxIter(40);
      GUM_CHECK_ASSERT_THROWS_NOTHING(vkl = mckl.klPQ());
      CHECK_NE(vkl, static_cast< float >(0.0));
      CHECK_THROWS_AS(mckl.history(), const gum::OperationNotAllowed&);
    }
  }

  GUM_TEST(ExactValues) {
    gum::BayesNet< double > netP;
    {
      gum::BIFReader< double > reader(&netP, GET_RESSOURCES_PATH("bif/bnP.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::BayesNet< double > netQ;
    {
      gum::BIFReader< double > reader(&netQ, GET_RESSOURCES_PATH("bif/bnQ.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::ExactBNdistance< double > kl(netP, netQ);
    GUM_CHECK_ASSERT_THROWS_NOTHING(kl.klPQ());
    CHECK((kl.klPQ()) == doctest::Approx(0.241864114).epsilon(1e-7));
    CHECK((kl.klQP()) == doctest::Approx(0.399826689).epsilon(1e-7));
    CHECK_EQ(kl.errorPQ(), static_cast< gum::Size >(0));
    CHECK_EQ(kl.errorQP(), static_cast< gum::Size >(0));
    CHECK((kl.hellinger()) == doctest::Approx(0.321089688).epsilon(1e-7));
    CHECK((kl.jsd()) == doctest::Approx(0.0696153).epsilon(1e-7));
    CHECK((kl.bhattacharya()) == doctest::Approx(0.0529255).epsilon(1e-7));
  }

  GUM_TEST(GibbsValues) {
    gum::BayesNet< double > netP;
    {
      gum::BIFReader< double > reader(&netP, GET_RESSOURCES_PATH("bif/bnP.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::BayesNet< double > netQ;
    {
      gum::BIFReader< double > reader(&netQ, GET_RESSOURCES_PATH("bif/bnQ.bif"));
      reader.trace(false);
      reader.proceed();
    }

    // iterations for better robustness : BNdistance may fail from time to time
    for (int ii = 0; ii < TESTKL_MAX_ITER_GIBBS_KL; ii++) {
      if (ii == TESTKL_MAX_ITER_GIBBS_KL - 1) {
        FAIL(" (GibbsBNdistance failed even with several tries.)");
        break;
      }
      gum::GibbsBNdistance< double > kl(netP, netQ);
      kl.setVerbosity(true);
      // very rough approximation in order to not penalize TestSuite
      kl.setEpsilon(GUM_SMALL_ERROR);
      kl.setMinEpsilonRate(GUM_SMALL_ERROR);
      if (fabs(kl.klPQ() - 0.241864114) >= 1e-1) continue;         // next try
      if (fabs(kl.klQP() - 0.399826689) >= 1e-1) continue;         // next try
      if (kl.errorPQ() != static_cast< gum::Size >(0)) continue;   // next try
      if (kl.errorQP() != static_cast< gum::Size >(0)) continue;   // next try
      if (fabs(kl.hellinger() - 0.321089688) >= 1e-1) continue;    // next try
      if (fabs(kl.jsd() - 0.0696153) >= 1e-1) continue;            // next try
      if (fabs(kl.bhattacharya() - 0.0529255) >= 1e-1) continue;   // next try
      if (kl.history().size() - (kl.nbrIterations() - kl.burnIn()) / kl.periodSize() >= 2)
        continue;                                                  // next try

      break;   // everything is ok : can stop the loop
    }
  }

  GUM_TEST(MCValues) {
    gum::BayesNet< double > netP;
    {
      gum::BIFReader< double > reader(&netP, GET_RESSOURCES_PATH("bif/bnP.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::BayesNet< double > netQ;
    {
      gum::BIFReader< double > reader(&netQ, GET_RESSOURCES_PATH("bif/bnQ.bif"));
      reader.trace(false);
      reader.proceed();
    }

    // iterations for better robustness : BNdistance may fail from time to time
    for (int ii = 0; ii < TESTKL_MAX_ITER_MC_KL; ii++) {
      if (ii == TESTKL_MAX_ITER_MC_KL - 1) {
        FAIL(" (MCBNDistance failed even with several tries.)");
        break;
      }
      gum::MCBNDistance< double > kl(netP, netQ);
      kl.setVerbosity(true);
      // very rough approximation in order to not penalize TestSuite
      kl.setEpsilon(GUM_SMALL_ERROR);
      kl.setMinEpsilonRate(GUM_SMALL_ERROR);
      if (fabs(kl.klPQ() - 0.241864114) >= 1e-1) continue;                             // next try
      if (fabs(kl.klQP() - 0.399826689) >= 1e-1) continue;                             // next try
      if (kl.errorPQ() != static_cast< gum::Size >(0)) continue;                       // next try
      if (kl.errorQP() != static_cast< gum::Size >(0)) continue;                       // next try
      if (fabs(kl.hellinger() - 0.321089688) >= 1e-1) continue;                        // next try
      if (fabs(kl.jsd() - 0.0696153) >= 1e-1) continue;                                // next try
      if (fabs(kl.bhattacharya() - 0.0529255) >= 1e-1) continue;                       // next try
      if (kl.history().size() - kl.nbrIterations() / kl.periodSize() >= 2) continue;   // next try

      break;   // everything is ok : can stop the loop
    }
  }

  GUM_TEST(CopyKeepsAllTheResults) {
    gum::BayesNet< double > netP;
    {
      gum::BIFReader< double > reader(&netP, GET_RESSOURCES_PATH("bif/bnP.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::BayesNet< double > netQ;
    {
      gum::BIFReader< double > reader(&netQ, GET_RESSOURCES_PATH("bif/bnQ.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::ExactBNdistance< double > kl(netP, netQ);
    kl.compute();
    gum::ExactBNdistance< double > copy(kl);
    CHECK((copy.hellinger()) == doctest::Approx(0.321089688).epsilon(1e-7));
    CHECK((copy.jsd()) == doctest::Approx(0.0696153).epsilon(1e-7));
    CHECK((copy.bhattacharya()) == doctest::Approx(0.0529255).epsilon(1e-7));
  }

  GUM_TEST(ChangedParameterInvalidatesTheResults) {
    gum::BayesNet< double > netP;
    {
      gum::BIFReader< double > reader(&netP, GET_RESSOURCES_PATH("bif/bnP.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::BayesNet< double > netQ;
    {
      gum::BIFReader< double > reader(&netQ, GET_RESSOURCES_PATH("bif/bnQ.bif"));
      reader.trace(false);
      reader.proceed();
    }

    // only maxIter stops the schemes
    gum::MCBNDistance< double > mc(netP, netQ);
    mc.disableEpsilon();
    mc.disableMinEpsilonRate();
    mc.setMaxIter(400);
    const double klPQ = mc.klPQ();
    CHECK_EQ(mc.nbrIterations(), static_cast< gum::Size >(400));
    mc.compute();
    CHECK_EQ(mc.klPQ(), klPQ);   // nothing changed: not computed again
    mc.setMaxIter(1000);
    mc.klPQ();                   // the accessors compute again outdated results
    CHECK_EQ(mc.nbrIterations(), static_cast< gum::Size >(1000));
    CHECK((mc.bhattacharya()) == doctest::Approx(0.0529255).epsilon(1e-1));

    gum::GibbsBNdistance< double > gibbs(netP, netQ);
    gibbs.setBurnIn(0);
    gibbs.disableEpsilon();
    gibbs.disableMinEpsilonRate();
    gibbs.setMaxIter(400);
    gibbs.compute();
    CHECK_EQ(gibbs.nbrIterations(), static_cast< gum::Size >(400));
    gibbs.setMaxIter(1000);
    gibbs.compute();
    CHECK_EQ(gibbs.nbrIterations(), static_cast< gum::Size >(1000));
    // the burn-in is counted in the iterations, and maxIter is checked at the end of the periods
    gibbs.setBurnIn(100);
    gibbs.compute();
    CHECK_EQ(gibbs.nbrIterations(), static_cast< gum::Size >(1100));
  }

  GUM_TEST(SampledDistancesWithDifferentSupports) {
    // P(A=0,B=1)=0 and Q(A=1,B=1)=0: each BN gives a positive probability to a configuration that
    // the other one excludes, so that the Jensen-Shannon divergence depends on the two missing
    // masses
    auto p = gum::BayesNet< double >::fastPrototype("A->B");
    p.cpt("A").fillWith({0.4, 0.6});
    p.cpt("B").fillWith({1.0, 0.0, 0.3, 0.7});
    auto q = gum::BayesNet< double >::fastPrototype("A->B");
    q.cpt("A").fillWith({0.7, 0.3});
    q.cpt("B").fillWith({0.5, 0.5, 1.0, 0.0});

    // P without null parameter: only the samples where Q is null matter
    auto pFull = gum::BayesNet< double >::fastPrototype("A->B");
    pFull.cpt("A").fillWith({0.4, 0.6});
    pFull.cpt("B").fillWith({0.9, 0.1, 0.3, 0.7});

    const auto check = [](const gum::BayesNet< double >& bn1, const gum::BayesNet< double >& bn2) {
      gum::ExactBNdistance< double > exact(bn1, bn2);
      const double                   jsd = exact.jsd();

      gum::MCBNDistance< double >    mc(bn1, bn2);
      gum::GibbsBNdistance< double > gibbs(bn1, bn2);
      for (gum::ApproximationScheme* algo: {static_cast< gum::ApproximationScheme* >(&mc),
                                            static_cast< gum::ApproximationScheme* >(&gibbs)}) {
        algo->disableEpsilon();
        algo->disableMinEpsilonRate();
        algo->setMaxIter(200000);
      }
      for (const double estimate: {mc.jsd(), gibbs.jsd()}) {
        // the missing masses would lower the estimate by 0.385 (p, q) and 0.21 (pFull, q)
        CHECK(fabs(estimate - jsd) < 0.02);
        CHECK(estimate >= 0.0);
        CHECK(estimate <= 1.0);
      }
      // the Hellinger estimate takes the mass of Q where P is null into account (2 - 2 rho)
      for (const double estimate: {mc.hellinger(), gibbs.hellinger()})
        CHECK(fabs(estimate - exact.hellinger()) < 0.02);
      // a configuration where P is positive and Q is null makes KL(P||Q) infinite
      if (exact.errorPQ() > 0) {
        CHECK(std::isinf(exact.klPQ()));
        CHECK(std::isinf(mc.klPQ()));
        CHECK(std::isinf(gibbs.klPQ()));
      } else {
        CHECK(!std::isinf(exact.klPQ()));
      }
      CHECK_EQ(std::isinf(exact.klQP()), exact.errorQP() > 0);
    };

    check(p, q);
    check(pFull, q);
    check(pFull, pFull);   // same BN: the estimate stays in [0,1] (no negative noise)
  }

  GUM_TEST(GibbsDistanceFollowsDeterministicNodes) {
    // B is a copy of A in P: the Gibbs chain on P must move A and B together, else it keeps the
    // configuration of its first sample
    auto p = gum::BayesNet< double >::fastPrototype("A->B->C");
    p.cpt("A").fillWith({0.3, 0.7});
    p.cpt("B").fillWith({1.0, 0.0, 0.0, 1.0});
    p.cpt("C").fillWith({0.8, 0.2, 0.4, 0.6});
    auto q = gum::BayesNet< double >::fastPrototype("A->B->C");
    q.cpt("A").fillWith({0.5, 0.5});
    q.cpt("B").fillWith({0.9, 0.1, 0.2, 0.8});
    q.cpt("C").fillWith({0.7, 0.3, 0.3, 0.7});

    gum::ExactBNdistance< double > exact(p, q);
    gum::GibbsBNdistance< double > gibbs(p, q);
    gibbs.disableEpsilon();
    gibbs.disableMinEpsilonRate();
    gibbs.setMaxIter(200000);
    CHECK(fabs(gibbs.klPQ() - exact.klPQ()) < 0.02);
    CHECK(fabs(gibbs.hellinger() - exact.hellinger()) < 0.02);
    CHECK(fabs(gibbs.bhattacharya() - exact.bhattacharya()) < 0.02);
    CHECK(fabs(gibbs.jsd() - exact.jsd()) < 0.02);
  }

  GUM_TEST(ComputationKeepsTheUserChoiceForTheRate) {
    gum::BayesNet< double > netP;
    {
      gum::BIFReader< double > reader(&netP, GET_RESSOURCES_PATH("bif/bnP.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::BayesNet< double > netQ;
    {
      gum::BIFReader< double > reader(&netQ, GET_RESSOURCES_PATH("bif/bnQ.bif"));
      reader.trace(false);
      reader.proceed();
    }

    const auto check = [](gum::ApproximationScheme& algo, auto compute) {
      algo.disableEpsilon();
      algo.setMaxIter(1000);

      // a rate always reached: it stops the computation only if it is enabled
      algo.setMinEpsilonRate(1e9);
      compute();
      CHECK_EQ(algo.stateApproximationScheme(),
               gum::IApproximationSchemeConfiguration::ApproximationSchemeSTATE::Rate);
      CHECK_LT(algo.nbrIterations(), static_cast< gum::Size >(1000));
      CHECK(algo.isEnabledMinEpsilonRate());

      algo.disableMinEpsilonRate();
      compute();
      CHECK_EQ(algo.stateApproximationScheme(),
               gum::IApproximationSchemeConfiguration::ApproximationSchemeSTATE::Limit);
      CHECK_EQ(algo.nbrIterations(), static_cast< gum::Size >(1000));
      CHECK(!algo.isEnabledMinEpsilonRate());

      // without the rate, the epsilon criterion still receives the errors of the samples
      algo.setEpsilon(1e-15);
      compute();
      CHECK_EQ(algo.stateApproximationScheme(),
               gum::IApproximationSchemeConfiguration::ApproximationSchemeSTATE::Limit);
      CHECK_EQ(algo.nbrIterations(), static_cast< gum::Size >(1000));
      CHECK(!algo.isEnabledMinEpsilonRate());
    };

    gum::MCBNDistance< double > mc(netP, netQ);
    check(mc, [&mc] { mc.compute(); });

    gum::GibbsBNdistance< double > gibbs(netP, netQ);
    gibbs.setBurnIn(0);
    check(gibbs, [&gibbs] { gibbs.compute(); });
  }

  GUM_TEST(NotLazyComputeTakesChangedBNsIntoAccount) {
    gum::BayesNet< double > netP;
    {
      gum::BIFReader< double > reader(&netP, GET_RESSOURCES_PATH("bif/bnP.bif"));
      reader.trace(false);
      reader.proceed();
    }

    gum::BayesNet< double >        copy(netP);
    gum::ExactBNdistance< double > kl(netP, copy);
    CHECK_EQ(kl.hellinger(), doctest::Approx(0.0));

    // the BNs are only referenced: a change in place is not detected...
    copy.generateCPT("X");
    kl.compute();
    CHECK_EQ(kl.hellinger(), doctest::Approx(0.0));
    // ... unless the computation is forced
    kl.compute(false);
    CHECK_GT(kl.hellinger(), 0.0);
  }
}   // namespace gum_tests
