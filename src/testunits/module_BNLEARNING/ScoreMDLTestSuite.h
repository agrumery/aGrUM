
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


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBTranslatorSet.h>
#include <agrum/BN/learning/aprioris/aprioriSmoothing.h>
#include <agrum/BN/learning/scores_and_tests/scoreMDL.h>

namespace gum_tests {

  class ScoreMDLTestSuite: public CxxTest::TestSuite {
    private:
    double __score(const std::vector< double >& N_ijk,
                   const std::vector< double >& N_ij,
                   const double                 penalty) const {
      double score = 0;
      double N = 0;
      for (const auto n_ijk: N_ijk)
        if (n_ijk) {
          score += n_ijk * std::log2(n_ijk);
          N += n_ijk;
        }
      if (!N_ij.empty()) {
        for (const auto n_ij: N_ij)
          if (n_ij) score -= n_ij * std::log2(n_ij);
      } else
        score -= N * std::log2(N);

      score -= penalty * 0.5 * std::log2(N);
      return score;
    }


    bool __equal(const double x, const double y) const {
      double dev = x >= y ? (x - y) / x : (y - x) / y;
      if (dev < 0) dev = -dev;
      return dev <= 1e-5;
    }


    public:
    void test_no_range__no_nodeId2col() {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet<> trans_set;
      {
        const std::vector< std::string >                miss;
        gum::learning::DBTranslator4LabelizedVariable<> translator(var, miss);
        std::vector< std::string > names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable<> database(trans_set);
      std::vector< std::string >     row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >     row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >     row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >     row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >     row4{"0", "0", "0", "1", "1", "1"};
      for (int i = 0; i < 1000; ++i)
        database.insertRow(row0);
      for (int i = 0; i < 50; ++i)
        database.insertRow(row1);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row2);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row3);
      for (int i = 0; i < 200; ++i)
        database.insertRow(row4);

      // create the parser
      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);

      gum::learning::AprioriSmoothing<> apriori(database);
      gum::learning::ScoreMDL<>         score(parser, apriori);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreMDL<>::isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::learning::ScoreMDL<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
         score.isAprioriCompatible(gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      gum::NodeId                node0 = 0;
      gum::NodeId                node1 = 1;
      gum::NodeId                node3 = 3;
      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node1};
      std::vector< gum::NodeId > cond3{node3};

      gum::learning::IdCondSet<> idset1(node0, cond_empty);    // #3,#0
      gum::learning::IdCondSet<> idset2(node0, cond2, true);   // #9,#3
      gum::learning::IdCondSet<> idset3(node0, cond3, true);   // #9,#3

      // idset1: node0 | emptyset
      double                penalty_1 = 2;
      std::vector< double > N_ijk_1{1201.0, 126.0, 76.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = __score(N_ijk_1, N_ij_1, penalty_1);
      TS_ASSERT(__equal(xscore_1, score.score(node0)));

      // idset2: node0 | node1
      double                penalty_2 = 6;
      std::vector< double > N_ijk_2{201, 76, 1, 1001, 1, 76, 1, 51, 1};
      std::vector< double > N_ij_2{278, 1078, 53};
      double                xscore_2 = __score(N_ijk_2, N_ij_2, penalty_2);
      TS_ASSERT(__equal(xscore_2, score.score(node0, cond2)));

      // idset3: node0 | node3
      double                penalty_3 = 6;
      std::vector< double > N_ijk_3{1, 76, 1, 201, 51, 76, 1001, 1, 1};
      std::vector< double > N_ij_3{78, 328, 1003};
      double                xscore_3 = __score(N_ijk_3, N_ij_3, penalty_3);
      TS_ASSERT(__equal(xscore_3, score.score(node0, cond3)));


      gum::learning::ScoreMDL<> score2(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score2.score(node0)));
      TS_ASSERT(__equal(xscore_2, score2.score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score2.score(node0, cond3)));

      gum::learning::ScoreMDL<> score3(std::move(score2));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score3.score(node0)));
      TS_ASSERT(__equal(xscore_2, score3.score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score3.score(node0, cond3)));

      gum::learning::ScoreMDL<>* score4 = score3.clone();
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      // delete score4;

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      delete score4;
    }


    void test_no_range__has_nodeId2col() {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet<> trans_set;
      {
        const std::vector< std::string >                miss;
        gum::learning::DBTranslator4LabelizedVariable<> translator(var, miss);
        std::vector< std::string > names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable<> database(trans_set);
      std::vector< std::string >     row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >     row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >     row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >     row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >     row4{"0", "0", "0", "1", "1", "1"};
      for (int i = 0; i < 1000; ++i)
        database.insertRow(row0);
      for (int i = 0; i < 50; ++i)
        database.insertRow(row1);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row2);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row3);
      for (int i = 0; i < 200; ++i)
        database.insertRow(row4);

      // create the parser
      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);

      gum::Bijection< gum::NodeId, std::size_t > nodeId2columns;
      gum::NodeId                                node0 = 0;
      gum::NodeId                                node1 = 1;
      gum::NodeId                                node2 = 2;
      gum::NodeId                                node3 = 3;
      gum::NodeId                                node4 = 4;
      gum::NodeId                                node5 = 5;
      nodeId2columns.insert(node0, std::size_t(4));
      nodeId2columns.insert(node1, std::size_t(3));
      nodeId2columns.insert(node2, std::size_t(0));
      nodeId2columns.insert(node3, std::size_t(2));
      nodeId2columns.insert(node4, std::size_t(5));
      nodeId2columns.insert(node5, std::size_t(1));

      gum::learning::AprioriSmoothing<> apriori(database, nodeId2columns);
      gum::learning::ScoreMDL<>         score(parser, apriori, nodeId2columns);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreMDL<>::isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::learning::ScoreMDL<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
         score.isAprioriCompatible(gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node5};
      std::vector< gum::NodeId > cond3{node1};

      gum::learning::IdCondSet<> idset1(node2, cond_empty);    // #3,#0
      gum::learning::IdCondSet<> idset2(node2, cond2, true);   // #9,#3
      gum::learning::IdCondSet<> idset3(node2, cond3, true);   // #9,#3

      // idset1: node2 | emptyset
      double                penalty_1 = 2;
      std::vector< double > N_ijk_1{1201.0, 126.0, 76.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = __score(N_ijk_1, N_ij_1, penalty_1);
      TS_ASSERT(__equal(xscore_1, score.score(node2)));


      // idset2: node2 | node5
      double                penalty_2 = 6;
      std::vector< double > N_ijk_2{201, 76, 1, 1001, 1, 76, 1, 51, 1};
      std::vector< double > N_ij_2{278, 1078, 53};
      double                xscore_2 = __score(N_ijk_2, N_ij_2, penalty_2);
      TS_ASSERT(__equal(xscore_2, score.score(node2, cond2)));

      // idset3: node2 | node1
      double                penalty_3 = 6;
      std::vector< double > N_ijk_3{1, 76, 1, 201, 51, 76, 1001, 1, 1};
      std::vector< double > N_ij_3{78, 328, 1003};
      double                xscore_3 = __score(N_ijk_3, N_ij_3, penalty_3);
      TS_ASSERT(__equal(xscore_3, score.score(node2, cond3)));


      gum::learning::ScoreMDL<> score2(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score2.score(node2)));
      TS_ASSERT(__equal(xscore_2, score2.score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score2.score(node2, cond3)));

      gum::learning::ScoreMDL<> score3(std::move(score2));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score3.score(node2)));
      TS_ASSERT(__equal(xscore_2, score3.score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score3.score(node2, cond3)));

      gum::learning::ScoreMDL<>* score4 = score3.clone();
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      delete score4;
    }


    void test_has_range__no_nodeId2col() {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet<> trans_set;
      {
        const std::vector< std::string >                miss;
        gum::learning::DBTranslator4LabelizedVariable<> translator(var, miss);
        std::vector< std::string > names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable<> database(trans_set);
      std::vector< std::string >     row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >     row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >     row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >     row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >     row4{"0", "0", "0", "1", "1", "1"};
      for (int i = 0; i < 1000; ++i)
        database.insertRow(row0);
      for (int i = 0; i < 50; ++i)
        database.insertRow(row1);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row2);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row3);
      for (int i = 0; i < 200; ++i)
        database.insertRow(row4);

      // create the parser
      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000},
                                                                  {1050, 1400}};

      gum::learning::AprioriSmoothing<> apriori(database);
      gum::learning::ScoreMDL<>         score(parser, apriori, ranges);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreMDL<>::isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::learning::ScoreMDL<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
         score.isAprioriCompatible(gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      gum::NodeId                node0 = 0;
      gum::NodeId                node1 = 1;
      gum::NodeId                node3 = 3;
      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node1};
      std::vector< gum::NodeId > cond3{node3};

      gum::learning::IdCondSet<> idset1(node0, cond_empty);    // #3,#0
      gum::learning::IdCondSet<> idset2(node0, cond2, true);   // #9,#3
      gum::learning::IdCondSet<> idset3(node0, cond3, true);   // #9,#3

      // idset1: node0 | emptyset
      double                penalty_1 = 2;
      std::vector< double > N_ijk_1{401.0, 76.0, 76.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = __score(N_ijk_1, N_ij_1, penalty_1);
      TS_ASSERT(__equal(xscore_1, score.score(node0)));

      // idset2: node0 | node1
      double                penalty_2 = 6;
      std::vector< double > N_ijk_2{201, 76, 1, 201, 1, 76, 1, 1, 1};
      std::vector< double > N_ij_2{278, 278, 3};
      double                xscore_2 = __score(N_ijk_2, N_ij_2, penalty_2);
      TS_ASSERT(__equal(xscore_2, score.score(node0, cond2)));

      // idset3: node0 | node3
      double                penalty_3 = 6;
      std::vector< double > N_ijk_3{1, 76, 1, 201, 1, 76, 201, 1, 1};
      std::vector< double > N_ij_3{78, 278, 203};
      double                xscore_3 = __score(N_ijk_3, N_ij_3, penalty_3);
      TS_ASSERT(__equal(xscore_3, score.score(node0, cond3)));


      gum::learning::ScoreMDL<> score2(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score2.score(node0)));
      TS_ASSERT(__equal(xscore_2, score2.score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score2.score(node0, cond3)));

      gum::learning::ScoreMDL<> score3(std::move(score2));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score3.score(node0)));
      TS_ASSERT(__equal(xscore_2, score3.score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score3.score(node0, cond3)));

      gum::learning::ScoreMDL<>* score4 = score3.clone();
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      // delete score4;

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      delete score4;
    }


    void test_has_range__has_nodeId2col() {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet<> trans_set;
      {
        const std::vector< std::string >                miss;
        gum::learning::DBTranslator4LabelizedVariable<> translator(var, miss);
        std::vector< std::string > names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable<> database(trans_set);
      std::vector< std::string >     row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >     row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >     row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >     row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >     row4{"0", "0", "0", "1", "1", "1"};
      for (int i = 0; i < 1000; ++i)
        database.insertRow(row0);
      for (int i = 0; i < 50; ++i)
        database.insertRow(row1);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row2);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row3);
      for (int i = 0; i < 200; ++i)
        database.insertRow(row4);

      // create the parser
      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000},
                                                                  {1050, 1400}};

      gum::Bijection< gum::NodeId, std::size_t > nodeId2columns;
      gum::NodeId                                node0 = 0;
      gum::NodeId                                node1 = 1;
      gum::NodeId                                node2 = 2;
      gum::NodeId                                node3 = 3;
      gum::NodeId                                node4 = 4;
      gum::NodeId                                node5 = 5;
      nodeId2columns.insert(node0, std::size_t(4));
      nodeId2columns.insert(node1, std::size_t(3));
      nodeId2columns.insert(node2, std::size_t(0));
      nodeId2columns.insert(node3, std::size_t(2));
      nodeId2columns.insert(node4, std::size_t(5));
      nodeId2columns.insert(node5, std::size_t(1));

      gum::learning::AprioriSmoothing<> apriori(database, nodeId2columns);
      gum::learning::ScoreMDL<> score(parser, apriori, ranges, nodeId2columns);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreMDL<>::isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::learning::ScoreMDL<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
         score.isAprioriCompatible(gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node5};
      std::vector< gum::NodeId > cond3{node1};

      gum::learning::IdCondSet<> idset1(node2, cond_empty);    // #3,#0
      gum::learning::IdCondSet<> idset2(node2, cond2, true);   // #9,#3
      gum::learning::IdCondSet<> idset3(node2, cond3, true);   // #9,#3

      // idset1: node2 | emptyset
      double                penalty_1 = 2;
      std::vector< double > N_ijk_1{401.0, 76.0, 76.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = __score(N_ijk_1, N_ij_1, penalty_1);
      TS_ASSERT(__equal(xscore_1, score.score(node2)));


      // idset2: node2 | node5
      double                penalty_2 = 6;
      std::vector< double > N_ijk_2{201, 76, 1, 201, 1, 76, 1, 1, 1};
      std::vector< double > N_ij_2{278, 278, 3};
      double                xscore_2 = __score(N_ijk_2, N_ij_2, penalty_2);
      TS_ASSERT(__equal(xscore_2, score.score(node2, cond2)));

      // idset3: node2 | node1
      double                penalty_3 = 6;
      std::vector< double > N_ijk_3{1, 76, 1, 201, 1, 76, 201, 1, 1};
      std::vector< double > N_ij_3{78, 278, 203};
      double                xscore_3 = __score(N_ijk_3, N_ij_3, penalty_3);
      TS_ASSERT(__equal(xscore_3, score.score(node2, cond3)));


      gum::learning::ScoreMDL<> score2(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score2.score(node2)));
      TS_ASSERT(__equal(xscore_2, score2.score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score2.score(node2, cond3)));

      gum::learning::ScoreMDL<> score3(std::move(score2));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score3.score(node2)));
      TS_ASSERT(__equal(xscore_2, score3.score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score3.score(node2, cond3)));

      gum::learning::ScoreMDL<>* score4 = score3.clone();
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      delete score4;
    }


    void test_multicore() {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet<> trans_set;
      {
        const std::vector< std::string >                miss;
        gum::learning::DBTranslator4LabelizedVariable<> translator(var, miss);
        std::vector< std::string > names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable<> database(trans_set);
      std::vector< std::string >     row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >     row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >     row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >     row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >     row4{"0", "0", "0", "1", "1", "1"};
      for (int i = 0; i < 1000; ++i)
        database.insertRow(row0);
      for (int i = 0; i < 50; ++i)
        database.insertRow(row1);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row2);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row3);
      for (int i = 0; i < 200; ++i)
        database.insertRow(row4);

      // create the parser
      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000},
                                                                  {1050, 1400}};

      gum::learning::AprioriSmoothing<> apriori(database);

      for (std::size_t i = std::size_t(1); i < std::size_t(24); ++i) {
        gum::learning::ScoreMDL<> score(parser, apriori, ranges);
        score.setMaxNbThreads(i);

        TS_GUM_ASSERT_THROWS_NOTHING(
           gum::learning::ScoreMDL<>::isAprioriCompatible(
              gum::learning::AprioriSmoothing<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(
           gum::learning::ScoreMDL<>::isAprioriCompatible(apriori));
        TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(
           gum::learning::AprioriSmoothing<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

        gum::NodeId                node0 = 0;
        gum::NodeId                node1 = 1;
        gum::NodeId                node3 = 3;
        std::vector< gum::NodeId > cond_empty;
        std::vector< gum::NodeId > cond2{node1};
        std::vector< gum::NodeId > cond3{node3};

        gum::learning::IdCondSet<> idset1(node0, cond_empty);    // #3,#0
        gum::learning::IdCondSet<> idset2(node0, cond2, true);   // #9,#3
        gum::learning::IdCondSet<> idset3(node0, cond3, true);   // #9,#3

        // idset1: node0 | emptyset
        double                penalty_1 = 2;
        std::vector< double > N_ijk_1{401.0, 76.0, 76.0};
        std::vector< double > N_ij_1;
        double                xscore_1 = __score(N_ijk_1, N_ij_1, penalty_1);
        TS_ASSERT(__equal(xscore_1, score.score(node0)));

        // idset2: node0 | node1
        double                penalty_2 = 6;
        std::vector< double > N_ijk_2{201, 76, 1, 201, 1, 76, 1, 1, 1};
        std::vector< double > N_ij_2{278, 278, 3};
        double                xscore_2 = __score(N_ijk_2, N_ij_2, penalty_2);
        TS_ASSERT(__equal(xscore_2, score.score(node0, cond2)));

        // idset3: node0 | node3
        double                penalty_3 = 6;
        std::vector< double > N_ijk_3{1, 76, 1, 201, 1, 76, 201, 1, 1};
        std::vector< double > N_ij_3{78, 278, 203};
        double                xscore_3 = __score(N_ijk_3, N_ij_3, penalty_3);
        TS_ASSERT(__equal(xscore_3, score.score(node0, cond3)));


        gum::learning::ScoreMDL<> score2(score);
        TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
           gum::learning::AprioriSmoothing<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

        TS_ASSERT(__equal(xscore_1, score2.score(node0)));
        TS_ASSERT(__equal(xscore_2, score2.score(node0, cond2)));
        TS_ASSERT(__equal(xscore_3, score2.score(node0, cond3)));

        gum::learning::ScoreMDL<> score3(std::move(score2));
        TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
           gum::learning::AprioriSmoothing<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

        TS_ASSERT(__equal(xscore_1, score3.score(node0)));
        TS_ASSERT(__equal(xscore_2, score3.score(node0, cond2)));
        TS_ASSERT(__equal(xscore_3, score3.score(node0, cond3)));

        gum::learning::ScoreMDL<>* score4 = score3.clone();
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
           gum::learning::AprioriSmoothing<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

        TS_ASSERT(__equal(xscore_1, score4->score(node0)));
        TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
        TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

        score4->operator=(score);
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
           gum::learning::AprioriSmoothing<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

        TS_ASSERT(__equal(xscore_1, score4->score(node0)));
        TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
        TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

        // delete score4;

        score4->operator=(std::move(score));
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
           gum::learning::AprioriSmoothing<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

        TS_ASSERT(__equal(xscore_1, score4->score(node0)));
        TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
        TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

        delete score4;
      }
    }
  };


} /* namespace gum_tests */