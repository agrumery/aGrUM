
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#define TESTKL_MAX_ITER_GIBBS_KL 15
#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <agrum/BN/algorithms/divergence/GibbsBNdistance.h>
#include <agrum/BN/algorithms/divergence/exactBNdistance.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class BNdistanceTestSuite: public CxxTest::TestSuite {
    public:
    void testConstructor() {
      gum::BayesNet< double > net1;
      {
        gum::BIFReader< double > reader(
           &net1, GET_RESSOURCES_PATH("bif/BIFReader_file2.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BayesNet< double > net2;
      {
        gum::BIFReader< double > reader(
           &net2, GET_RESSOURCES_PATH("bif/BIFReader_file3.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BayesNet< double > net3;
      {
        gum::BIFReader< double > reader(
           &net3, GET_RESSOURCES_PATH("bif/BIFReader_file4.bif"));
        reader.trace(false);
        reader.proceed();
      }

      TS_GUM_ASSERT_THROWS_NOTHING(gum::ExactBNdistance< double > kl(net1, net1));
      TS_ASSERT_THROWS(gum::ExactBNdistance< double > kl(net1, net2),
                       gum::OperationNotAllowed);
      TS_GUM_ASSERT_THROWS_NOTHING(gum::ExactBNdistance< double > kl(net2, net3));
    }

    void testDifficulty1() {
      gum::BayesNet< double > net2;
      {
        gum::BIFReader< double > reader(
           &net2, GET_RESSOURCES_PATH("bif/BIFReader_file3.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::ExactBNdistance< double > kl(net2, net2);
      TS_ASSERT_EQUALS(kl.difficulty(), gum::Complexity::Correct);

      gum::BayesNet< double > net;
      {
        gum::BIFReader< double > reader(&net,
                                        GET_RESSOURCES_PATH("bif/hailfinder.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BNdistance< double > kl2(net, net);
      TS_ASSERT_EQUALS(kl2.difficulty(), gum::Complexity::Heavy);
    }

    void testKLComputation() {
      gum::BayesNet< double > net3;
      {
        gum::BIFReader< double > reader(
           &net3, GET_RESSOURCES_PATH("bif/BIFReader_file3.bif"));
        reader.trace(false);
        reader.proceed();
      }

      double vkl = 0.0;

      gum::ExactBNdistance< double > stupid_bfkl(net3, net3);
      TS_GUM_ASSERT_THROWS_NOTHING(vkl = stupid_bfkl.klPQ());
      TS_ASSERT_EQUALS(vkl, 0.0);
      TS_GUM_ASSERT_THROWS_NOTHING(vkl = stupid_bfkl.klQP());
      TS_ASSERT_EQUALS(vkl, 0.0);

      gum::BayesNet< double > net4;
      {
        gum::BIFReader< double > reader(
           &net4, GET_RESSOURCES_PATH("bif/BIFReader_file4.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BNdistance< double > kl(net3, net4);
      TS_ASSERT_EQUALS(kl.difficulty(), gum::Complexity::Correct);

      {
        gum::ExactBNdistance< double > bfkl(kl);
        TS_GUM_ASSERT_THROWS_NOTHING(vkl = bfkl.klPQ());
        TS_ASSERT_DIFFERS(vkl, (float)0.0);
      }

      {
        gum::GibbsBNdistance< double > gkl(kl);
        gkl.setMaxIter(40);
        TS_GUM_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ());
        TS_ASSERT_DIFFERS(vkl, (float)0.0);
        TS_ASSERT_THROWS(gkl.history(), gum::OperationNotAllowed);
      }

      {
        gum::GibbsBNdistance< double > gkl(kl);
        gkl.setMaxIter(40);
        TS_GUM_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ());
        TS_ASSERT_DIFFERS(vkl, (float)0.0);
        TS_ASSERT_THROWS(gkl.history(), gum::OperationNotAllowed);
      }
    }

    void testExactValues() {
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
      TS_GUM_ASSERT_THROWS_NOTHING(kl.klPQ());
      TS_ASSERT_DELTA(kl.klPQ(), 0.241864114, 1e-7);
      TS_ASSERT_DELTA(kl.klQP(), 0.399826689, 1e-7);
      TS_ASSERT_EQUALS(kl.errorPQ(), (gum::Size)0);
      TS_ASSERT_EQUALS(kl.errorQP(), (gum::Size)0);
      TS_ASSERT_DELTA(kl.hellinger(), 0.321089688, 1e-7);
      TS_ASSERT_DELTA(kl.jsd(), 0.0696153, 1e-7);
      TS_ASSERT_DELTA(kl.bhattacharya(), 0.0529255, 1e-7);
    }

    void testGibbsValues() {
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
        kl.setEpsilon(1e-5);
        kl.setMinEpsilonRate(1e-5);
        if (fabs(kl.klPQ() - 0.241864114) >= 1e-1) continue;         // next try
        if (fabs(kl.klQP() - 0.399826689) >= 1e-1) continue;         // next try
        if (kl.errorPQ() != (gum::Size)0) continue;                  // next try
        if (kl.errorQP() != (gum::Size)0) continue;                  // next try
        if (fabs(kl.hellinger() - 0.321089688) >= 1e-1) continue;    // next try
        if (fabs(kl.jsd() - 0.0696153) >= 1e-1) continue;            // next try
        if (fabs(kl.bhattacharya() - 0.0529255) >= 1e-1) continue;   // next try
        if (kl.history().size()
               - (kl.nbrIterations() - kl.burnIn()) / kl.periodSize()
            >= 2)
          continue;   // next try

        break;   // everything is ok : can stop the loop
      }
    }
  };   // namespace gum_tests
}   // namespace gum_tests
