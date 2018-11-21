/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/aprioris/aprioriSmoothing.h>
#include <agrum/learning/scores_and_tests/scoreLog2Likelihood.h>

namespace gum_tests {

  class ScoreLog2Likelihood2TestSuite : public CxxTest::TestSuite {
    private:
    double __score(const std::vector< double >& N_ijk,
                   const std::vector< double >& N_ij) const {
      double score = 0.0;
      double N = 0;
      for (const auto n_ijk : N_ijk)
        if (n_ijk) {
          score += n_ijk * std::log2(n_ijk);
          N += n_ijk;
        }
      if (!N_ij.empty()) {
        for (const auto n_ij : N_ij)
          if (n_ij) score -= n_ij * std::log2(n_ij);
      } else
        score -= N * std::log2(N);
      return score;
    }


    bool __equal(const double x, const double y) const {
      double dev = x >= y ? (x - y) / x : (y - x) / y;
      if (dev < 0) dev = -dev;
      return dev <= 1e-5;
    }


    public:
    void test1() {
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

      gum::learning::AprioriSmoothing2<>    apriori(database);
      gum::learning::ScoreLog2Likelihood2<> score(parser, apriori);

      TS_GUM_ASSERT_THROWS_NOTHING(
        gum::learning::ScoreLog2Likelihood2<>::isAprioriCompatible(
          gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
        gum::learning::ScoreLog2Likelihood2<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
        score.isAprioriCompatible(gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      gum::NodeId                node0 = 0;
      gum::NodeId                node1 = 1;
      gum::NodeId                node3 = 3;
      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node1};
      std::vector< gum::NodeId > cond3{node3};

      gum::learning::IdSet2<> idset1(node0, cond_empty);    // #3,#0
      gum::learning::IdSet2<> idset2(node0, cond2, true);   // #9,#3
      gum::learning::IdSet2<> idset3(node0, cond3, true);   // #9,#3

      // idset1: node0 | emptyset
      std::vector< double > N_ijk_1{1201.0, 126.0, 76.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = __score(N_ijk_1, N_ij_1);
      TS_ASSERT(__equal(xscore_1, score.score(node0)));

      // idset2: node0 | node1
      std::vector< double > N_ijk_2{201, 76, 1, 1001, 1, 76, 1, 51, 1};
      std::vector< double > N_ij_2{278, 1078, 53};
      double                xscore_2 = __score(N_ijk_2, N_ij_2);
      TS_ASSERT(__equal(xscore_2, score.score(node0, cond2)));

      // idset3: node0 | node3
      std::vector< double > N_ijk_3{1, 76, 1, 201, 51, 76, 1001, 1, 1};
      std::vector< double > N_ij_3{78, 328, 1003};
      double                xscore_3 = __score(N_ijk_3, N_ij_3);
      TS_ASSERT(__equal(xscore_3, score.score(node0, cond3)));


      gum::learning::ScoreLog2Likelihood2<> score2(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
        gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score2.score(node0)));
      TS_ASSERT(__equal(xscore_2, score2.score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score2.score(node0, cond3)));

      gum::learning::ScoreLog2Likelihood2<> score3(std::move(score2));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
        gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score3.score(node0)));
      TS_ASSERT(__equal(xscore_2, score3.score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score3.score(node0, cond3)));

      gum::learning::ScoreLog2Likelihood2<>* score4 = score3.clone();
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
        gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
        gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
        gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      delete score4;
    }


    void test2() {
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

      gum::learning::AprioriSmoothing2<>    apriori(database, nodeId2columns);
      gum::learning::ScoreLog2Likelihood2<> score(parser, apriori, nodeId2columns);

      TS_GUM_ASSERT_THROWS_NOTHING(
        gum::learning::ScoreLog2Likelihood2<>::isAprioriCompatible(
          gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
        gum::learning::ScoreLog2Likelihood2<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
        score.isAprioriCompatible(gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node5};
      std::vector< gum::NodeId > cond3{node1};

      gum::learning::IdSet2<> idset1(node2, cond_empty);    // #3,#0
      gum::learning::IdSet2<> idset2(node2, cond2, true);   // #9,#3
      gum::learning::IdSet2<> idset3(node2, cond3, true);   // #9,#3

      // idset1: node2 | emptyset
      std::vector< double > N_ijk_1{1201.0, 126.0, 76.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = __score(N_ijk_1, N_ij_1);
      TS_ASSERT(__equal(xscore_1, score.score(node2)));


      // idset2: node2 | node5
      std::vector< double > N_ijk_2{201, 76, 1, 1001, 1, 76, 1, 51, 1};
      std::vector< double > N_ij_2{278, 1078, 53};
      double                xscore_2 = __score(N_ijk_2, N_ij_2);
      TS_ASSERT(__equal(xscore_2, score.score(node2, cond2)));

      // idset3: node2 | node1
      std::vector< double > N_ijk_3{1, 76, 1, 201, 51, 76, 1001, 1, 1};
      std::vector< double > N_ij_3{78, 328, 1003};
      double                xscore_3 = __score(N_ijk_3, N_ij_3);
      TS_ASSERT(__equal(xscore_3, score.score(node2, cond3)));


      gum::learning::ScoreLog2Likelihood2<> score2(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
        gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score2.score(node2)));
      TS_ASSERT(__equal(xscore_2, score2.score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score2.score(node2, cond3)));

      gum::learning::ScoreLog2Likelihood2<> score3(std::move(score2));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
        gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score3.score(node2)));
      TS_ASSERT(__equal(xscore_2, score3.score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score3.score(node2, cond3)));

      gum::learning::ScoreLog2Likelihood2<>* score4 = score3.clone();
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
        gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
        gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
        gum::learning::AprioriSmoothing2<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      delete score4;
    }
  };


} /* namespace gum_tests */
