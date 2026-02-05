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
#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/BN/algorithms/divergence/exactBNdistance.h>
#include <agrum/BN/algorithms/divergence/GibbsBNdistance.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BNdistance
#define GUM_CURRENT_MODULE BN

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
    static void testConstructor() {
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
      CHECK_THROWS_AS(gum::ExactBNdistance< double > kl(net1, net2),
                      const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(gum::ExactBNdistance< double > kl(net2, net3),
                      const gum::OperationNotAllowed&);
    }   // namespace gum_tests

    static void testComparableBNsForAnyDiscreteVariables() {
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

    static void testDifficulty1() {
      gum::BayesNet< double > net2;
      {
        gum::BIFReader< double > reader(&net2, GET_RESSOURCES_PATH("bif/BIFReader_file3.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::ExactBNdistance< double > kl(net2, net2);
      CHECK((kl.difficulty()) == (gum::Complexity::Correct));

      gum::BayesNet< double > net;
      {
        gum::BIFReader< double > reader(&net, GET_RESSOURCES_PATH("bif/hailfinder.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BNdistance< double > kl2(net, net);
      CHECK((kl2.difficulty()) == (gum::Complexity::Heavy));
    }

    static void testKLComputation() {
      gum::BayesNet< double > net3;
      {
        gum::BIFReader< double > reader(&net3, GET_RESSOURCES_PATH("bif/BIFReader_file3.bif"));
        reader.trace(false);
        reader.proceed();
      }

      double                         vkl = 0.0;
      gum::ExactBNdistance< double > stupid_bfkl(net3, net3);
      CHECK((stupid_bfkl.difficulty()) == (gum::Complexity::Correct));
      GUM_CHECK_ASSERT_THROWS_NOTHING(vkl = stupid_bfkl.klPQ());
      CHECK((vkl) == (0.0));
      GUM_CHECK_ASSERT_THROWS_NOTHING(vkl = stupid_bfkl.klQP());
      CHECK((vkl) == (0.0));

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
      CHECK((kl.difficulty()) == (gum::Complexity::Correct));
      {
        gum::ExactBNdistance< double > gkl(kl);
        GUM_CHECK_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ());
        CHECK((vkl) != (static_cast< float >(0.0)));
      }
      {
        gum::GibbsBNdistance< double > gkl(kl);
        gkl.setMaxIter(40);
        GUM_CHECK_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ());
        CHECK((vkl) != (static_cast< float >(0.0)));
        CHECK_THROWS_AS(gkl.history(), const gum::OperationNotAllowed&);
      }

      {
        gum::GibbsBNdistance< double > gkl(kl);
        gkl.setMaxIter(40);
        GUM_CHECK_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ());
        CHECK((vkl) != (static_cast< float >(0.0)));
        CHECK_THROWS_AS(gkl.history(), const gum::OperationNotAllowed&);
      }
    }

    static void testExactValues() {
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
      CHECK((kl.errorPQ()) == (static_cast< gum::Size >(0)));
      CHECK((kl.errorQP()) == (static_cast< gum::Size >(0)));
      CHECK((kl.hellinger()) == doctest::Approx(0.321089688).epsilon(1e-7));
      CHECK((kl.jsd()) == doctest::Approx(0.0696153).epsilon(1e-7));
      CHECK((kl.bhattacharya()) == doctest::Approx(0.0529255).epsilon(1e-7));
    }

    static void testGibbsValues() {
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
  };   // namespace gum_tests

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(ComparableBNsForAnyDiscreteVariables)
  GUM_TEST_ACTIF(Difficulty1)
  GUM_TEST_ACTIF(KLComputation)
  GUM_TEST_ACTIF(ExactValues)
  GUM_TEST_ACTIF(GibbsValues)
}   // namespace gum_tests
