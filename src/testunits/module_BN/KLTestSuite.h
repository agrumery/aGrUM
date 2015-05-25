/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   (gumSize) 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.   *
 ***************************************************************************/

#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <agrum/BN/algorithms/divergence/bruteForceKL.h>
#include <agrum/BN/algorithms/divergence/GibbsKL.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class KLTestSuite : public CxxTest::TestSuite {

    public:
    void testConstructor() {
      gum::BayesNet<float> net1;
      {
        gum::BIFReader<float> reader(&net1, GET_PATH_STR("BIFReader_file2.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BayesNet<float> net2;
      {
        gum::BIFReader<float> reader(&net2, GET_PATH_STR("BIFReader_file3.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BayesNet<float> net3;
      {
        gum::BIFReader<float> reader(&net3, GET_PATH_STR("BIFReader_file4.bif"));
        reader.trace(false);
        reader.proceed();
      }

      TS_GUM_ASSERT_THROWS_NOTHING(gum::BruteForceKL<float> kl(net1, net1));
      TS_ASSERT_THROWS(gum::BruteForceKL<float> kl(net1, net2),
                       gum::OperationNotAllowed);
      TS_GUM_ASSERT_THROWS_NOTHING(gum::BruteForceKL<float> kl(net2, net3));
    }

    void testDifficulty1() {
      gum::BayesNet<float> net2;
      {
        gum::BIFReader<float> reader(&net2, GET_PATH_STR("BIFReader_file3.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BruteForceKL<float> kl(net2, net2);
      TS_ASSERT_EQUALS(kl.difficulty(), gum::Complexity::Correct);

      gum::BayesNet<float> net;
      {
        gum::BIFReader<float> reader(&net, GET_PATH_STR("hailfinder.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::KL<float> kl2(net, net);
      TS_ASSERT_EQUALS(kl2.difficulty(), gum::Complexity::Heavy);
    }

    void testKLComputation() {
      gum::BayesNet<float> net3;
      {
        gum::BIFReader<float> reader(&net3, GET_PATH_STR("BIFReader_file3.bif"));
        reader.trace(false);
        reader.proceed();
      }

      double vkl = 0.0;

      gum::BruteForceKL<float> stupid_bfkl(net3, net3);
      TS_GUM_ASSERT_THROWS_NOTHING(vkl = stupid_bfkl.klPQ());
      TS_ASSERT_EQUALS(vkl, 0.0);
      TS_GUM_ASSERT_THROWS_NOTHING(vkl = stupid_bfkl.klQP());
      TS_ASSERT_EQUALS(vkl, 0.0);

      gum::BayesNet<float> net4;
      {
        gum::BIFReader<float> reader(&net4, GET_PATH_STR("BIFReader_file4.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::KL<float> kl(net3, net4);
      TS_ASSERT_EQUALS(kl.difficulty(), gum::Complexity::Correct);

      {
        gum::BruteForceKL<float> bfkl(kl);
        TS_GUM_ASSERT_THROWS_NOTHING(vkl = bfkl.klPQ());
        TS_ASSERT_DIFFERS(vkl, (float)0.0);
      }

      {
        gum::GibbsKL<float> gkl(kl);
        gkl.setMaxIter(40);
        TS_GUM_ASSERT_THROWS_NOTHING(vkl = gkl.klPQ());
        TS_ASSERT_DIFFERS(vkl, (float)0.0);
        TS_ASSERT_THROWS(gkl.history(), gum::OperationNotAllowed);
      }
    }

    void testBruteForceValues() {
      gum::BayesNet<float> netP;
      {
        gum::BIFReader<float> reader(&netP, GET_PATH_STR("bnP.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BayesNet<float> netQ;
      {
        gum::BIFReader<float> reader(&netQ, GET_PATH_STR("bnQ.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BruteForceKL<float> kl(netP, netQ);
      TS_GUM_ASSERT_THROWS_NOTHING(kl.klPQ());
      TS_ASSERT_DELTA(kl.klPQ(), 0.241864114, 1e-7);
      TS_ASSERT_DELTA(kl.klQP(), 0.399826689, 1e-7);
      TS_ASSERT_EQUALS(kl.errorPQ(), (gum::Size)0);
      TS_ASSERT_EQUALS(kl.errorQP(), (gum::Size)0);
      TS_ASSERT_DELTA(kl.hellinger(), 0.321089688, 1e-7);
    }

    void testGibbsValues() {
      gum::BayesNet<float> netP;
      {
        gum::BIFReader<float> reader(&netP, GET_PATH_STR("bnP.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::BayesNet<float> netQ;
      {
        gum::BIFReader<float> reader(&netQ, GET_PATH_STR("bnQ.bif"));
        reader.trace(false);
        reader.proceed();
      }

      gum::GibbsKL<float> kl(netP, netQ);
      kl.setVerbosity(true);
      // very rough approximation in order to not penalize TestSuite
      kl.setEpsilon(1e-5);
      kl.setMinEpsilonRate(1e-5);
      TS_ASSERT_DELTA(kl.klPQ(), 0.241864114, 1e-1);
      TS_ASSERT_DELTA(kl.klQP(), 0.399826689, 1e-1);
      TS_ASSERT_EQUALS(kl.errorPQ(), (gum::Size)0);
      TS_ASSERT_EQUALS(kl.errorQP(), (gum::Size)0);
      TS_ASSERT_DELTA(kl.hellinger(), 0.321089688, 1e-1);
      TS_ASSERT(abs(kl.history().size() -
                    (kl.nbrIterations() - kl.burnIn()) / kl.periodSize()) < 2);
      // GUM_TRACE_VAR( kl.history() );
    }
  };
} // gum_tests
