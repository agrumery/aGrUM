/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/algorithms/BayesBall.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/tools/graphicalModels/inference/scheduler/scheduleMultiDim.h>

namespace gum_tests {

  class [[maybe_unused]] BayesBallTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    GUM_TEST(RequisiteNodes) {
      gum::SimpleBayesNetGenerator< double > gen(50, 200, 2);
      gum::BayesNet< double >                bn;
      gen.generateBN(bn);
      gum::Set< gum::NodeId > requisite;

      gum::Set< gum::NodeId >      query, hardEvidence, softEvidence;
      gum::Sequence< gum::NodeId > nodes_seq;
      for (const auto node: bn.nodes())
        nodes_seq.insert(node);

      for (gum::Idx i = 0; i < 5; ++i)
        hardEvidence.insert(nodes_seq.atPos(i));

      for (gum::Idx j = 24; j > 19; --j)
        query.insert(nodes_seq.atPos(j));

      TS_ASSERT_THROWS_NOTHING(
         gum::BayesBall::requisiteNodes(bn.dag(), query, hardEvidence, softEvidence, requisite));

      TS_ASSERT(requisite.size() >= 5)
    }

    GUM_TEST(RelevantPotentials) {
      std::string file = GET_RESSOURCES_PATH("bif/asia.bif");

      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = reader.proceed();
      TS_ASSERT(nbrErr == gum::Size(0))

      gum::Sequence< gum::NodeId >                nodes_seq;
      gum::Set< const gum::Potential< double >* > potentials;

      for (const auto node: bn.nodes()) {
        nodes_seq.insert(node);
        potentials.insert(&bn.cpt(node));
        // std::cout << node << " : " << bn.variable(node).name() << std::endl;
      }

      gum::Set< gum::NodeId > requisite;
      gum::Set< gum::NodeId > query, hardEvidence, softEvidence;

      query.insert(gum::NodeId(1));          // tuberculosis
      hardEvidence.insert(gum::NodeId(5));   // smoking

      TS_ASSERT_THROWS_NOTHING(
         gum::BayesBall::requisiteNodes(bn.dag(), query, hardEvidence, softEvidence, requisite));
      TS_ASSERT_THROWS_NOTHING(
         gum::BayesBall::relevantPotentials(bn, query, hardEvidence, softEvidence, potentials));

      TS_ASSERT(potentials.size() == 5)
    }

    GUM_TEST(RelevantScheduleMultiDims) {
      std::string file = GET_RESSOURCES_PATH("bif/asia.bif");

      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = reader.proceed();
      TS_ASSERT(nbrErr == gum::Size(0))

      gum::Sequence< gum::NodeId >                                         nodes_seq;
      gum::Set< const gum::ScheduleMultiDim< gum::Potential< double > >* > potentials;

      for (const auto node: bn.nodes()) {
        nodes_seq.insert(node);
        auto sched
           = new gum::ScheduleMultiDim< gum::Potential< double > >(bn.cpt(node), false, node);
        potentials.insert(sched);
        // std::cout << node << " : " << bn.variable(node).name() << std::endl;
      }
      gum::Set< const gum::ScheduleMultiDim< gum::Potential< double > >* > pots = potentials;

      gum::Set< gum::NodeId > requisite;
      gum::Set< gum::NodeId > query, hardEvidence, softEvidence;

      query.insert(gum::NodeId(1));          // tuberculosis
      hardEvidence.insert(gum::NodeId(5));   // smoking

      TS_ASSERT_THROWS_NOTHING(
         gum::BayesBall::requisiteNodes(bn.dag(), query, hardEvidence, softEvidence, requisite));
      TS_ASSERT_THROWS_NOTHING(
         gum::BayesBall::relevantPotentials(bn, query, hardEvidence, softEvidence, potentials));

      TS_ASSERT(potentials.size() == 5)

      for (const auto pot: pots)
        delete pot;
    }
  };
}   // namespace gum_tests
