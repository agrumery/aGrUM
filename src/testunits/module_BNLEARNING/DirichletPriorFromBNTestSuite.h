/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBTranslatorSet.h>
#include <agrum/tools/stattests/recordCounter.h>

#include <agrum/BN/learning/priors/DirichletPriorFromBN.h>

namespace gum_tests {

  class [[maybe_unused]] DirichletPriorFromBNTestSuite: public CxxTest::TestSuite {
    private:
    void _test_prior_(gum::learning::DirichletPriorFromBN< double >& prior) {
      prior.setWeight(100.0);
      TS_ASSERT_EQUALS(prior.weight(), 100.0)

      TS_ASSERT_EQUALS(prior.getType(), gum::learning::PriorType::DirichletPriorType)

      gum::NodeId                node0 = 0;
      std::vector< gum::NodeId > cond_empty;

      gum::learning::IdCondSet idset1(node0, cond_empty);   // #3,#0
      std::vector< double >    vect(3, 10.0);
      prior.addJointPseudoCount(idset1, vect);
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[0], 43.33333)   // 10 + 0.3333*100
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[1], 43.33333)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[2], 43.33333)

      gum::NodeId              node1 = 1;                                // B
      gum::learning::IdCondSet idset2(node0, node1, cond_empty, true);   // #9,#0

      vect.clear();
      vect.resize(9, 1.0);
      std::fill(vect.begin(), vect.end(), 1.0);
      prior.setWeight(100.0);
      prior.addJointPseudoCount(idset2, vect);

      TS_GUM_ASSERT_ALMOST_EQUALS(vect[0], 7.66666)   // 1 + 100 * 0.333 * 0.2
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[1], 7.66666)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[2], 7.66666)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[3], 11.0)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[4], 11.0)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[5], 11.0)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[6], 17.66666)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[7], 17.66666)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[8], 17.666666)

      vect.clear();
      vect.resize(1, 1.0);
      std::fill(vect.begin(), vect.end(), 1.0);
      prior.addConditioningPseudoCount(idset1, vect);
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[0], 1 + prior.weight())

      vect.clear();
      vect.resize(3, 1.0);
      std::fill(vect.begin(), vect.end(), 1.0);
      prior.addJointPseudoCount(idset1, vect);
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[0], 34.33333)   // 1 + 0.3333*100
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[1], 34.33333)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[2], 34.33333)

      gum::NodeId                node3 = 3;
      std::vector< gum::NodeId > cond1{node3};
      gum::learning::IdCondSet   idset3(node0, cond1, true);   // #9,#0

      vect.clear();
      vect.resize(9, 1.0);
      std::fill(vect.begin(), vect.end(), 1.0);
      prior.addJointPseudoCount(idset3, vect);
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[0], 12.11111)   // 1+100* (1/3) * (1/3)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[1], 12.11111)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[2], 12.11111)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[3], 12.11111)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[4], 12.11111)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[5], 12.11111)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[6], 12.11111)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[7], 12.11111)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[8], 12.11111)

      vect.clear();
      vect.resize(3, 100.0);
      std::fill(vect.begin(), vect.end(), 100.0);
      prior.addConditioningPseudoCount(idset3, vect);
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[0], 133.33333)   // 100 + 100*(1/3)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[1], 133.33333)
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[2], 133.33333)

      gum::NodeId              node4 = 2;                    // C
      gum::learning::IdCondSet idset4(node4, cond1, true);   // C|D
      prior.setWeight(100.0);
      vect.resize(6, 1);
      std::fill(vect.begin(), vect.end(), 1.0);
      prior.addJointPseudoCount(idset4, vect);
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[0], 4.33333)    // 1+ 100 * 1/3 * 0.1
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[1], 31.0)       // 1+ 100 * 1/3 * 0.9
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[2], 11.0)       // 1+ 100 * 1/3 * 0.3
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[3], 24.33333)   // 1+ 100 * 1/3 * 0.7
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[4], 17.66666)   // 1+ 100 * 1/3 * 0.5
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[5], 17.66666)   // 1+ 100 * 1/3 * 0.5

      vect.resize(3, 1);
      std::fill(vect.begin(), vect.end(), 1.0);
      prior.addConditioningPseudoCount(idset4, vect);
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[0], 34.33333)   // 1+ 100 * 1/3
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[1], 34.33333)   // 1+ 100 * 1/3
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[2], 34.33333)   // 1+ 100 * 1/3

      std::vector< gum::NodeId > cond2{node3, node1};
      gum::learning::IdCondSet   idset5(node4, cond2, true);   // C|D,B = C|D
      prior.setWeight(100.0);
      vect.resize(18, 1);
      std::fill(vect.begin(), vect.end(), 1.0);
      prior.addJointPseudoCount(idset5, vect);

      TS_GUM_ASSERT_ALMOST_EQUALS(vect[0], 1.0 + (100.0 * 1.0 / 3.0 * 0.1 * 0.2))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[1], 1.0 + (100.0 * 1.0 / 3.0 * 0.9 * 0.2))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[2], 1.0 + (100.0 * 1.0 / 3.0 * 0.3 * 0.2))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[3], 1.0 + (100.0 * 1.0 / 3.0 * 0.7 * 0.2))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[4], 1.0 + (100.0 * 1.0 / 3.0 * 0.5 * 0.2))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[5], 1.0 + (100.0 * 1.0 / 3.0 * 0.5 * 0.2))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[6], 1.0 + (100.0 * 1.0 / 3.0 * 0.1 * 0.3))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[7], 1.0 + (100.0 * 1.0 / 3.0 * 0.9 * 0.3))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[8], 1.0 + (100.0 * 1.0 / 3.0 * 0.3 * 0.3))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[9], 1.0 + (100.0 * 1.0 / 3.0 * 0.7 * 0.3))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[10], 1.0 + (100.0 * 1.0 / 3.0 * 0.5 * 0.3))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[11], 1.0 + (100.0 * 1.0 / 3.0 * 0.5 * 0.3))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[12], 1.0 + (100.0 * 1.0 / 3.0 * 0.1 * 0.5))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[13], 1.0 + (100.0 * 1.0 / 3.0 * 0.9 * 0.5))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[14], 1.0 + (100.0 * 1.0 / 3.0 * 0.3 * 0.5))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[15], 1.0 + (100.0 * 1.0 / 3.0 * 0.7 * 0.5))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[16], 1.0 + (100.0 * 1.0 / 3.0 * 0.5 * 0.5))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[17], 1.0 + (100.0 * 1.0 / 3.0 * 0.5 * 0.5))

      vect.resize(9, 1);
      std::fill(vect.begin(), vect.end(), 1.0);
      prior.setWeight(100.0);
      prior.addConditioningPseudoCount(idset5, vect);
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[0], 1.0 + (100.0 * 1.0 / 3.0 * 0.2))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[1], 1.0 + (100.0 * 1.0 / 3.0 * 0.2))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[2], 1.0 + (100.0 * 1.0 / 3.0 * 0.2))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[3], 1.0 + (100.0 * 1.0 / 3.0 * 0.3))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[4], 1.0 + (100.0 * 1.0 / 3.0 * 0.3))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[5], 1.0 + (100.0 * 1.0 / 3.0 * 0.3))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[6], 1.0 + (100.0 * 1.0 / 3.0 * 0.5))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[7], 1.0 + (100.0 * 1.0 / 3.0 * 0.5))
      TS_GUM_ASSERT_ALMOST_EQUALS(vect[8], 1.0 + (100.0 * 1.0 / 3.0 * 0.5))
    }

    public:
    GUM_ACTIVE_TEST(1) {
      std::vector< std::string > names{"A", "B", "C", "D", "E", "F"};
      gum::BayesNet< double >    bn
         = gum::BayesNet< double >::fastPrototype("A[3];B[3];C<-D[3];E[3];F[3]");
      for (const auto& nom: names)
        bn.cpt(nom).fillWith(1).normalizeAsCPT();
      bn.cpt("B").fillWith({0.2, 0.3, 0.5});
      bn.cpt("C").fillWith({0.1, 0.9, 0.3, 0.7, 0.5, 0.5});

      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      // create the translator set
      gum::learning::DBTranslatorSet trans_set;
      {
        const std::vector< std::string >              miss;
        gum::learning::DBTranslator4LabelizedVariable translator(var, miss);

        for (auto i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the (useless) database
      gum::learning::DatabaseTable database(trans_set);
      database.insertRow({"0", "1", "0", "2", "1", "1"});
      database.insertRow({"1", "2", "0", "1", "2", "2"});
      database.insertRow({"2", "1", "0", "1", "1", "0"});
      database.insertRow({"1", "0", "0", "0", "0", "0"});
      database.insertRow({"0", "0", "0", "1", "1", "1"});

      gum::learning::DirichletPriorFromBN< double > prior(database, &bn);
      TS_ASSERT_EQUALS(prior.weight(), 1.0)
      _test_prior_(prior);

      gum::learning::DirichletPriorFromBN prior2(prior);
      TS_ASSERT_EQUALS(prior2.weight(), 100.0)
      _test_prior_(prior2);

      gum::learning::DirichletPriorFromBN prior3(std::move(prior2));
      TS_ASSERT_EQUALS(prior3.weight(), 100.0)
      _test_prior_(prior3);

      gum::learning::DirichletPriorFromBN< double >* prior4 = prior3.clone();
      TS_ASSERT_EQUALS(prior4->weight(), 100.0)
      _test_prior_(*prior4);
      delete (prior4);
    }
  };


} /* namespace gum_tests */
