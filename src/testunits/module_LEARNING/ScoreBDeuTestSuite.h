
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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

#include <agrum/core/math/gammaLog2.h>
#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/aprioris/aprioriNoApriori.h>
#include <agrum/learning/scores_and_tests/scoreBDeu.h>

namespace gum_tests {

  class ScoreBDeuTestSuite: public CxxTest::TestSuite {
    private:
    gum::GammaLog2 __gammalog2;
    bool           __gum_destructor_increased{false};

    void setUp() {}

    void tearDown() {
      if (!__gum_destructor_increased) {
        GUM_DESTRUCTOR(GammaLog2);
        __gum_destructor_increased = true;
      }
    }

    double __score(const std::vector< double >& N_ijk,
                   const std::vector< double >& N_ij) const {
      double score = 0.0;
      double ess = 2.0;

      if (!N_ij.empty()) {
        double ess_qi = ess / N_ij.size();
        double ess_riqi = ess / N_ijk.size();
        for (const auto n_ij: N_ij) {
          score += __gammalog2(ess_qi) - __gammalog2(n_ij + ess_qi);
        }
        for (const auto n_ijk: N_ijk) {
          score += __gammalog2(n_ijk + ess_riqi) - __gammalog2(ess_riqi);
        }
      } else {
        double N = 0.0;
        double ess_ri = ess / N_ijk.size();
        for (const auto n_ijk: N_ijk) {
          score += __gammalog2(n_ijk + ess_ri) - __gammalog2(ess_ri);
          N += n_ijk;
        }
        score += __gammalog2(ess) - __gammalog2(N + ess);
      }

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

      gum::learning::AprioriNoApriori<> apriori(database);
      gum::learning::ScoreBDeu<>        score(parser, apriori);
      score.setEffectiveSampleSize(2.0);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreBDeu<>::isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::learning::ScoreBDeu<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
         score.isAprioriCompatible(gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      gum::NodeId                node0 = 0;
      gum::NodeId                node1 = 1;
      gum::NodeId                node3 = 3;
      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node1};
      std::vector< gum::NodeId > cond3{node3};

      gum::learning::IdSet<> idset1(node0, cond_empty);    // #3,#0
      gum::learning::IdSet<> idset2(node0, cond2, true);   // #9,#3
      gum::learning::IdSet<> idset3(node0, cond3, true);   // #9,#3

      // idset1: node0 | emptyset
      std::vector< double > N_ijk_1{1200.0, 125.0, 75.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = __score(N_ijk_1, N_ij_1);
      TS_ASSERT(__equal(xscore_1, score.score(node0)));

      // idset2: node0 | node1
      std::vector< double > N_ijk_2{200, 75, 0, 1000, 0, 75, 0, 50, 0};
      std::vector< double > N_ij_2{275, 1075, 50};
      double                xscore_2 = __score(N_ijk_2, N_ij_2);
      TS_ASSERT(__equal(xscore_2, score.score(node0, cond2)));

      // idset3: node0 | node3
      std::vector< double > N_ijk_3{0, 75, 0, 200, 50, 75, 1000, 0, 0};
      std::vector< double > N_ij_3{75, 325, 1000};
      double                xscore_3 = __score(N_ijk_3, N_ij_3);
      TS_ASSERT(__equal(xscore_3, score.score(node0, cond3)));

      gum::learning::ScoreBDeu<> score2(score);
      score2.setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score2.score(node0)));
      TS_ASSERT(__equal(xscore_2, score2.score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score2.score(node0, cond3)));

      gum::learning::ScoreBDeu<> score3(std::move(score2));
      score3.setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score3.score(node0)));
      TS_ASSERT(__equal(xscore_2, score3.score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score3.score(node0, cond3)));

      gum::learning::ScoreBDeu<>* score4 = score3.clone();
      score4->setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
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

      gum::learning::AprioriNoApriori<> apriori(database, nodeId2columns);
      gum::learning::ScoreBDeu<>        score(parser, apriori, nodeId2columns);
      score.setEffectiveSampleSize(2.0);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreBDeu<>::isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::learning::ScoreBDeu<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
         score.isAprioriCompatible(gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node5};
      std::vector< gum::NodeId > cond3{node1};

      gum::learning::IdSet<> idset1(node2, cond_empty);    // #3,#0
      gum::learning::IdSet<> idset2(node2, cond2, true);   // #9,#3
      gum::learning::IdSet<> idset3(node2, cond3, true);   // #9,#3

      // idset1: node2 | emptyset
      std::vector< double > N_ijk_1{1200.0, 125.0, 75.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = __score(N_ijk_1, N_ij_1);
      TS_ASSERT(__equal(xscore_1, score.score(node2)));


      // idset2: node2 | node5
      std::vector< double > N_ijk_2{200, 75, 0, 1000, 0, 75, 0, 50, 0};
      std::vector< double > N_ij_2{275, 1075, 50};
      double                xscore_2 = __score(N_ijk_2, N_ij_2);
      TS_ASSERT(__equal(xscore_2, score.score(node2, cond2)));

      // idset3: node2 | node1
      std::vector< double > N_ijk_3{0, 75, 0, 200, 50, 75, 1000, 0, 0};
      std::vector< double > N_ij_3{75, 325, 1000};
      double                xscore_3 = __score(N_ijk_3, N_ij_3);
      TS_ASSERT(__equal(xscore_3, score.score(node2, cond3)));


      gum::learning::ScoreBDeu<> score2(score);
      score2.setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score2.score(node2)));
      TS_ASSERT(__equal(xscore_2, score2.score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score2.score(node2, cond3)));

      gum::learning::ScoreBDeu<> score3(std::move(score2));
      score3.setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score3.score(node2)));
      TS_ASSERT(__equal(xscore_2, score3.score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score3.score(node2, cond3)));

      gum::learning::ScoreBDeu<>* score4 = score3.clone();
      score4->setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
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

      gum::learning::AprioriNoApriori<> apriori(database);
      gum::learning::ScoreBDeu<>        score(parser, apriori, ranges);
      score.setEffectiveSampleSize(2.0);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreBDeu<>::isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::learning::ScoreBDeu<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
         score.isAprioriCompatible(gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      gum::NodeId                node0 = 0;
      gum::NodeId                node1 = 1;
      gum::NodeId                node3 = 3;
      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node1};
      std::vector< gum::NodeId > cond3{node3};

      gum::learning::IdSet<> idset1(node0, cond_empty);    // #3,#0
      gum::learning::IdSet<> idset2(node0, cond2, true);   // #9,#3
      gum::learning::IdSet<> idset3(node0, cond3, true);   // #9,#3

      // idset1: node0 | emptyset
      std::vector< double > N_ijk_1{400.0, 75.0, 75.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = __score(N_ijk_1, N_ij_1);
      TS_ASSERT(__equal(xscore_1, score.score(node0)));

      // idset2: node0 | node1
      std::vector< double > N_ijk_2{200, 75, 0, 200, 0, 75, 0, 0, 0};
      std::vector< double > N_ij_2{275, 275, 0};
      double                xscore_2 = __score(N_ijk_2, N_ij_2);
      TS_ASSERT(__equal(xscore_2, score.score(node0, cond2)));

      // idset3: node0 | node3
      std::vector< double > N_ijk_3{0, 75, 0, 200, 0, 75, 200, 0, 0};
      std::vector< double > N_ij_3{75, 275, 200};
      double                xscore_3 = __score(N_ijk_3, N_ij_3);
      TS_ASSERT(__equal(xscore_3, score.score(node0, cond3)));

      gum::learning::ScoreBDeu<> score2(score);
      score2.setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score2.score(node0)));
      TS_ASSERT(__equal(xscore_2, score2.score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score2.score(node0, cond3)));

      gum::learning::ScoreBDeu<> score3(std::move(score2));
      score3.setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score3.score(node0)));
      TS_ASSERT(__equal(xscore_2, score3.score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score3.score(node0, cond3)));

      gum::learning::ScoreBDeu<>* score4 = score3.clone();
      score4->setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node0)));
      TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
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

      gum::learning::AprioriNoApriori<> apriori(database, nodeId2columns);
      gum::learning::ScoreBDeu<> score(parser, apriori, ranges, nodeId2columns);
      score.setEffectiveSampleSize(2.0);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreBDeu<>::isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::learning::ScoreBDeu<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
         score.isAprioriCompatible(gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node5};
      std::vector< gum::NodeId > cond3{node1};

      gum::learning::IdSet<> idset1(node2, cond_empty);    // #3,#0
      gum::learning::IdSet<> idset2(node2, cond2, true);   // #9,#3
      gum::learning::IdSet<> idset3(node2, cond3, true);   // #9,#3

      // idset1: node2 | emptyset
      std::vector< double > N_ijk_1{400.0, 75.0, 75.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = __score(N_ijk_1, N_ij_1);
      TS_ASSERT(__equal(xscore_1, score.score(node2)));


      // idset2: node2 | node5
      std::vector< double > N_ijk_2{200, 75, 0, 200, 0, 75, 0, 0, 0};
      std::vector< double > N_ij_2{275, 275, 0};
      double                xscore_2 = __score(N_ijk_2, N_ij_2);
      TS_ASSERT(__equal(xscore_2, score.score(node2, cond2)));

      // idset3: node2 | node1
      std::vector< double > N_ijk_3{0, 75, 0, 200, 0, 75, 200, 0, 0};
      std::vector< double > N_ij_3{75, 275, 200};
      double                xscore_3 = __score(N_ijk_3, N_ij_3);
      TS_ASSERT(__equal(xscore_3, score.score(node2, cond3)));


      gum::learning::ScoreBDeu<> score2(score);
      score2.setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score2.score(node2)));
      TS_ASSERT(__equal(xscore_2, score2.score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score2.score(node2, cond3)));

      gum::learning::ScoreBDeu<> score3(std::move(score2));
      score3.setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score3.score(node2)));
      TS_ASSERT(__equal(xscore_2, score3.score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score3.score(node2, cond3)));

      gum::learning::ScoreBDeu<>* score4 = score3.clone();
      score4->setEffectiveSampleSize(2.0);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

      TS_ASSERT(__equal(xscore_1, score4->score(node2)));
      TS_ASSERT(__equal(xscore_2, score4->score(node2, cond2)));
      TS_ASSERT(__equal(xscore_3, score4->score(node2, cond3)));

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
         gum::learning::AprioriNoApriori<>::type::type));
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

      gum::learning::AprioriNoApriori<> apriori(database);
      for (std::size_t i = std::size_t(1); i < std::size_t(24); ++i) {
        gum::learning::ScoreBDeu<> score(parser, apriori, ranges);
        score.setMaxNbThreads(i);
        score.setEffectiveSampleSize(2.0);

        TS_GUM_ASSERT_THROWS_NOTHING(
           gum::learning::ScoreBDeu<>::isAprioriCompatible(
              gum::learning::AprioriNoApriori<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(
           gum::learning::ScoreBDeu<>::isAprioriCompatible(apriori));
        TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(
           gum::learning::AprioriNoApriori<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

        gum::NodeId                node0 = 0;
        gum::NodeId                node1 = 1;
        gum::NodeId                node3 = 3;
        std::vector< gum::NodeId > cond_empty;
        std::vector< gum::NodeId > cond2{node1};
        std::vector< gum::NodeId > cond3{node3};

        gum::learning::IdSet<> idset1(node0, cond_empty);    // #3,#0
        gum::learning::IdSet<> idset2(node0, cond2, true);   // #9,#3
        gum::learning::IdSet<> idset3(node0, cond3, true);   // #9,#3

        // idset1: node0 | emptyset
        std::vector< double > N_ijk_1{400.0, 75.0, 75.0};
        std::vector< double > N_ij_1;
        double                xscore_1 = __score(N_ijk_1, N_ij_1);
        TS_ASSERT(__equal(xscore_1, score.score(node0)));

        // idset2: node0 | node1
        std::vector< double > N_ijk_2{200, 75, 0, 200, 0, 75, 0, 0, 0};
        std::vector< double > N_ij_2{275, 275, 0};
        double                xscore_2 = __score(N_ijk_2, N_ij_2);
        TS_ASSERT(__equal(xscore_2, score.score(node0, cond2)));

        // idset3: node0 | node3
        std::vector< double > N_ijk_3{0, 75, 0, 200, 0, 75, 200, 0, 0};
        std::vector< double > N_ij_3{75, 275, 200};
        double                xscore_3 = __score(N_ijk_3, N_ij_3);
        TS_ASSERT(__equal(xscore_3, score.score(node0, cond3)));

        gum::learning::ScoreBDeu<> score2(score);
        score2.setEffectiveSampleSize(2.0);
        TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(
           gum::learning::AprioriNoApriori<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score2.isAprioriCompatible(apriori));

        TS_ASSERT(__equal(xscore_1, score2.score(node0)));
        TS_ASSERT(__equal(xscore_2, score2.score(node0, cond2)));
        TS_ASSERT(__equal(xscore_3, score2.score(node0, cond3)));

        gum::learning::ScoreBDeu<> score3(std::move(score2));
        score3.setEffectiveSampleSize(2.0);
        TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(
           gum::learning::AprioriNoApriori<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score3.isAprioriCompatible(apriori));

        TS_ASSERT(__equal(xscore_1, score3.score(node0)));
        TS_ASSERT(__equal(xscore_2, score3.score(node0, cond2)));
        TS_ASSERT(__equal(xscore_3, score3.score(node0, cond3)));

        gum::learning::ScoreBDeu<>* score4 = score3.clone();
        score4->setEffectiveSampleSize(2.0);
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
           gum::learning::AprioriNoApriori<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

        TS_ASSERT(__equal(xscore_1, score4->score(node0)));
        TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
        TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

        score4->operator=(score);
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
           gum::learning::AprioriNoApriori<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

        TS_ASSERT(__equal(xscore_1, score4->score(node0)));
        TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
        TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

        score4->operator=(std::move(score));
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(
           gum::learning::AprioriNoApriori<>::type::type));
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isAprioriCompatible(apriori));

        TS_ASSERT(__equal(xscore_1, score4->score(node0)));
        TS_ASSERT(__equal(xscore_2, score4->score(node0, cond2)));
        TS_ASSERT(__equal(xscore_3, score4->score(node0, cond3)));

        delete score4;
      }
    }
  };


} /* namespace gum_tests */
