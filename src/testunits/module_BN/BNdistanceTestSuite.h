/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class [[maybe_unused]] BNdistanceTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Constructor) {
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

      TS_GUM_ASSERT_THROWS_NOTHING(gum::ExactBNdistance< double > kl(net1, net1))
      TS_ASSERT_THROWS(gum::ExactBNdistance< double > kl(net1, net2),
                       const gum::OperationNotAllowed&)
      TS_ASSERT_THROWS(gum::ExactBNdistance< double > kl(net2, net3),
                       const gum::OperationNotAllowed&)
    }

    GUM_ACTIVE_TEST(ComparableBNsForAnyDiscreteVariables) {
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

    GUM_ACTIVE_TEST(Difficulty1) {
      gum::BayesNet< double > net2;
      {
        gum::BIFReader< double > reader(&net2, GET_RESSOURCES_PATH("bif/BIFReader_file3.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::ExactBNdistance< double > kl(net2, net2);
      TS_ASSERT_EQUALS(kl.difficulty(), gum::Complexity::Correct)

      gum::BayesNet< double > net;
      {
        gum::BIFReader< double > reader(&net, GET_RESSOURCES_PATH("bif/hailfinder.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BNdistance< double > kl2(net, net);
      TS_ASSERT_EQUALS(kl2.difficulty(), gum::Complexity::Heavy)
    }

    GUM_ACTIVE_TEST(KLComputation) {
      gum::BayesNet< double > net3;
      {
        gum::BIFReader< double > reader(&net3, GET_RESSOURCES_PATH("bif/BIFReader_file3.bif"));
        reader.trace(false);
        reader.proceed();
      }

      double                         vkl = 0.0;
      gum::ExactBNdistance< double > stupid_bfkl(net3, net3);
      TS_ASSERT_EQUALS(stupid_bfkl.difficulty(), gum::Complexity::Correct)
      TS_GUM_ASSERT_THROWS_NOTHING(vkl = stupid_bfkl.klPQ())
      TS_ASSERT_EQUALS(vkl, 0.0)
      TS_GUM_ASSERT_THROWS_NOTHING(vkl = stupid_bfkl.klQP())
      TS_ASSERT_EQUALS(vkl, 0.0)

      gum::BayesNet< double > net4;
      {
        gum::BIFReader< double > reader(&net4, GET_RESSOURCES_PATH("bif/BIFReader_file4.bif"));
        reader.trace(false);
        reader.proceed();
      }

      TS_ASSERT_THROWS(gum::BNdistance< double > kl(net3, net4), const gum::OperationNotAllowed&);

      gum::BayesNet< double > net5;
      {
        gum::BIFReader< double > reader(&net5, GET_RESSOURCES_PATH("bif/BIFReader_file5.bif"));
        reader.trace(false);
        reader.proceed();
      }
      gum::BNdistance< double > kl(net3, net5);
      TS_ASSERT_EQUALS(kl.difficulty(), gum::Complexity::Correct)
      {
        gum::ExactBNdistance< double > gkl(kl);
        TS_GUM_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ())
        TS_ASSERT_DIFFERS(vkl, (float)0.0)
      }
      {
        gum::GibbsBNdistance< double > gkl(kl);
        gkl.setMaxIter(40);
        TS_GUM_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ())
        TS_ASSERT_DIFFERS(vkl, (float)0.0)
        TS_ASSERT_THROWS(gkl.history(), const gum::OperationNotAllowed&)
      }

      {
        gum::GibbsBNdistance< double > gkl(kl);
        gkl.setMaxIter(40);
        TS_GUM_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ())
        TS_ASSERT_DIFFERS(vkl, (float)0.0)
        TS_ASSERT_THROWS(gkl.history(), const gum::OperationNotAllowed&)
      }
    }

    GUM_ACTIVE_TEST(ExactValues) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(kl.klPQ())
      TS_ASSERT_DELTA(kl.klPQ(), 0.241864114, 1e-7)
      TS_ASSERT_DELTA(kl.klQP(), 0.399826689, 1e-7)
      TS_ASSERT_EQUALS(kl.errorPQ(), (gum::Size)0)
      TS_ASSERT_EQUALS(kl.errorQP(), (gum::Size)0)
      TS_ASSERT_DELTA(kl.hellinger(), 0.321089688, 1e-7)
      TS_ASSERT_DELTA(kl.jsd(), 0.0696153, 1e-7)
      TS_ASSERT_DELTA(kl.bhattacharya(), 0.0529255, 1e-7)
    }

    GUM_ACTIVE_TEST(GibbsValues) {
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
          TS_FAIL(" (GibbsBNdistance failed even with several tries.)");
          break;
        }
        gum::GibbsBNdistance< double > kl(netP, netQ);
        kl.setVerbosity(true);
        // very rough approximation in order to not penalize TestSuite
        kl.setEpsilon(TS_GUM_SMALL_ERROR);
        kl.setMinEpsilonRate(TS_GUM_SMALL_ERROR);
        if (fabs(kl.klPQ() - 0.241864114) >= 1e-1) continue;         // next try
        if (fabs(kl.klQP() - 0.399826689) >= 1e-1) continue;         // next try
        if (kl.errorPQ() != (gum::Size)0) continue;                  // next try
        if (kl.errorQP() != (gum::Size)0) continue;                  // next try
        if (fabs(kl.hellinger() - 0.321089688) >= 1e-1) continue;    // next try
        if (fabs(kl.jsd() - 0.0696153) >= 1e-1) continue;            // next try
        if (fabs(kl.bhattacharya() - 0.0529255) >= 1e-1) continue;   // next try
        if (kl.history().size() - (kl.nbrIterations() - kl.burnIn()) / kl.periodSize() >= 2)
          continue;                                                  // next try

        break;   // everything is ok : can stop the loop
      }
    }
  };   // namespace gum_tests
}   // namespace gum_tests
