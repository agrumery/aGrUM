/****************************************************************************
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/math/gammaLog2.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/BN/learning/priors/noPrior.h>

#include <agrum/BN/learning/scores_and_tests/scoreK2.h>

namespace gum_tests {

  class [[maybe_unused]] ScoreK2TestSuite: public CxxTest::TestSuite {
    private:
    gum::GammaLog2 _gammalog2_;
    bool           _gum_destructor_increased_{false};

    void setUp() {}

    void tearDown() {
      if (!_gum_destructor_increased_) {
        GUM_DESTRUCTOR(GammaLog2);
        _gum_destructor_increased_ = true;
      }
    }

    double _score_(const std::vector< double >& N_ijk, const std::vector< double >& N_ij) const {
      double score = 0.0;

      if (!N_ij.empty()) {
        const double ri = double(N_ijk.size() / N_ij.size());
        score           = N_ij.size() * _gammalog2_(ri);

        for (const auto n_ij: N_ij) {
          score -= _gammalog2_(n_ij + ri);
        }
        for (const auto n_ijk: N_ijk) {
          score += _gammalog2_(n_ijk + 1);
        }
      } else {
        const double ri = double(N_ijk.size());
        score           = _gammalog2_(ri);
        double N        = 0;
        for (const auto n_ijk: N_ijk) {
          score += _gammalog2_(n_ijk + 1);
          N += n_ijk;
        }
        score -= _gammalog2_(N + ri);
      }

      return score;
    }

    bool _equal_(const double x, const double y) const {
      double dev = x >= y ? (x - y) / x : (y - x) / y;
      if (dev < 0) dev = -dev;
      return dev <= TS_GUM_SMALL_ERROR;
    }


    public:
    void _test_no_range_no_nodeId2col() {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet trans_set;
      {
        const std::vector< std::string >              miss;
        gum::learning::DBTranslator4LabelizedVariable translator(var, miss);
        std::vector< std::string >                    names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable database(trans_set);
      std::vector< std::string >   row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >   row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >   row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >   row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >   row4{"0", "0", "0", "1", "1", "1"};
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
      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      gum::learning::NoPrior prior(database);
      gum::learning::ScoreK2 score(parser, prior);

      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::learning::ScoreK2::isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreK2::isPriorCompatible(prior))
      TS_GUM_ASSERT_THROWS_NOTHING(score.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isPriorCompatible(prior))

      gum::NodeId                node0 = 0;
      gum::NodeId                node1 = 1;
      gum::NodeId                node3 = 3;
      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node1};
      std::vector< gum::NodeId > cond3{node3};

      gum::learning::IdCondSet idset1(node0, cond_empty);    // #3,#0
      gum::learning::IdCondSet idset2(node0, cond2, true);   // #9,#3
      gum::learning::IdCondSet idset3(node0, cond3, true);   // #9,#3

      // idset1: node0 | emptyset
      std::vector< double > N_ijk_1{1200.0, 125.0, 75.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = _score_(N_ijk_1, N_ij_1);
      TS_ASSERT(_equal_(xscore_1, score.score(node0)))

      // idset2: node0 | node1
      std::vector< double > N_ijk_2{200, 75, 0, 1000, 0, 75, 0, 50, 0};
      std::vector< double > N_ij_2{275, 1075, 50};
      double                xscore_2 = _score_(N_ijk_2, N_ij_2);
      TS_ASSERT(_equal_(xscore_2, score.score(node0, cond2)))

      // idset3: node0 | node3
      std::vector< double > N_ijk_3{0, 75, 0, 200, 50, 75, 1000, 0, 0};
      std::vector< double > N_ij_3{75, 325, 1000};
      double                xscore_3 = _score_(N_ijk_3, N_ij_3);
      TS_ASSERT(_equal_(xscore_3, score.score(node0, cond3)))

      gum::learning::ScoreK2 score2(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score2.score(node0)))
      TS_ASSERT(_equal_(xscore_2, score2.score(node0, cond2)))
      TS_ASSERT(_equal_(xscore_3, score2.score(node0, cond3)))

      gum::learning::ScoreK2 score3(std::move(score2));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score3.score(node0)))
      TS_ASSERT(_equal_(xscore_2, score3.score(node0, cond2)))
      TS_ASSERT(_equal_(xscore_3, score3.score(node0, cond3)))

      gum::learning::ScoreK2* score4 = score3.clone();
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node0)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node0, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node0, cond3)))

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node0)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node0, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node0, cond3)))

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node0)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node0, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node0, cond3)))

      delete score4;
    }

    void _test_no_range_has_nodeId2col() {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet trans_set;
      {
        const std::vector< std::string >              miss;
        gum::learning::DBTranslator4LabelizedVariable translator(var, miss);
        std::vector< std::string >                    names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable database(trans_set);
      std::vector< std::string >   row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >   row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >   row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >   row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >   row4{"0", "0", "0", "1", "1", "1"};
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
      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

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

      gum::learning::NoPrior prior(database, nodeId2columns);
      gum::learning::ScoreK2 score(parser, prior, nodeId2columns);

      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::learning::ScoreK2::isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreK2::isPriorCompatible(prior))
      TS_GUM_ASSERT_THROWS_NOTHING(score.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isPriorCompatible(prior))

      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node5};
      std::vector< gum::NodeId > cond3{node1};

      gum::learning::IdCondSet idset1(node2, cond_empty);    // #3,#0
      gum::learning::IdCondSet idset2(node2, cond2, true);   // #9,#3
      gum::learning::IdCondSet idset3(node2, cond3, true);   // #9,#3

      // idset1: node2 | emptyset
      std::vector< double > N_ijk_1{1200.0, 125.0, 75.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = _score_(N_ijk_1, N_ij_1);
      TS_ASSERT(_equal_(xscore_1, score.score(node2)))


      // idset2: node2 | node5
      std::vector< double > N_ijk_2{200, 75, 0, 1000, 0, 75, 0, 50, 0};
      std::vector< double > N_ij_2{275, 1075, 50};
      double                xscore_2 = _score_(N_ijk_2, N_ij_2);
      TS_ASSERT(_equal_(xscore_2, score.score(node2, cond2)))

      // idset3: node2 | node1
      std::vector< double > N_ijk_3{0, 75, 0, 200, 50, 75, 1000, 0, 0};
      std::vector< double > N_ij_3{75, 325, 1000};
      double                xscore_3 = _score_(N_ijk_3, N_ij_3);
      TS_ASSERT(_equal_(xscore_3, score.score(node2, cond3)))


      gum::learning::ScoreK2 score2(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score2.score(node2)))
      TS_ASSERT(_equal_(xscore_2, score2.score(node2, cond2)))
      TS_ASSERT(_equal_(xscore_3, score2.score(node2, cond3)))

      gum::learning::ScoreK2 score3(std::move(score2));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score3.score(node2)))
      TS_ASSERT(_equal_(xscore_2, score3.score(node2, cond2)))
      TS_ASSERT(_equal_(xscore_3, score3.score(node2, cond3)))

      gum::learning::ScoreK2* score4 = score3.clone();
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node2)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node2, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node2, cond3)))

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node2)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node2, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node2, cond3)))

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node2)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node2, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node2, cond3)))

      delete score4;
    }

    void _test_has_range_no_nodeId2col() {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet trans_set;
      {
        const std::vector< std::string >              miss;
        gum::learning::DBTranslator4LabelizedVariable translator(var, miss);
        std::vector< std::string >                    names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable database(trans_set);
      std::vector< std::string >   row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >   row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >   row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >   row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >   row4{"0", "0", "0", "1", "1", "1"};
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
      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000}, {1050, 1400}};

      gum::learning::NoPrior prior(database);
      gum::learning::ScoreK2 score(parser, prior, ranges);

      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::learning::ScoreK2::isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreK2::isPriorCompatible(prior))
      TS_GUM_ASSERT_THROWS_NOTHING(score.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isPriorCompatible(prior))

      gum::NodeId                node0 = 0;
      gum::NodeId                node1 = 1;
      gum::NodeId                node3 = 3;
      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node1};
      std::vector< gum::NodeId > cond3{node3};

      gum::learning::IdCondSet idset1(node0, cond_empty);    // #3,#0
      gum::learning::IdCondSet idset2(node0, cond2, true);   // #9,#3
      gum::learning::IdCondSet idset3(node0, cond3, true);   // #9,#3

      // idset1: node0 | emptyset
      std::vector< double > N_ijk_1{400.0, 75.0, 75.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = _score_(N_ijk_1, N_ij_1);
      TS_ASSERT(_equal_(xscore_1, score.score(node0)))

      // idset2: node0 | node1
      std::vector< double > N_ijk_2{200, 75, 0, 200, 0, 75, 0, 0, 0};
      std::vector< double > N_ij_2{275, 275, 0};
      double                xscore_2 = _score_(N_ijk_2, N_ij_2);
      TS_ASSERT(_equal_(xscore_2, score.score(node0, cond2)))

      // idset3: node0 | node3
      std::vector< double > N_ijk_3{0, 75, 0, 200, 0, 75, 200, 0, 0};
      std::vector< double > N_ij_3{75, 275, 200};
      double                xscore_3 = _score_(N_ijk_3, N_ij_3);
      TS_ASSERT(_equal_(xscore_3, score.score(node0, cond3)))

      gum::learning::ScoreK2 score2(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score2.score(node0)))
      TS_ASSERT(_equal_(xscore_2, score2.score(node0, cond2)))
      TS_ASSERT(_equal_(xscore_3, score2.score(node0, cond3)))

      gum::learning::ScoreK2 score3(std::move(score2));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score3.score(node0)))
      TS_ASSERT(_equal_(xscore_2, score3.score(node0, cond2)))
      TS_ASSERT(_equal_(xscore_3, score3.score(node0, cond3)))

      gum::learning::ScoreK2* score4 = score3.clone();
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node0)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node0, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node0, cond3)))

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node0)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node0, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node0, cond3)))

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node0)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node0, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node0, cond3)))

      delete score4;
    }

    void _test_has_range_has_nodeId2col() {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet trans_set;
      {
        const std::vector< std::string >              miss;
        gum::learning::DBTranslator4LabelizedVariable translator(var, miss);
        std::vector< std::string >                    names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable database(trans_set);
      std::vector< std::string >   row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >   row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >   row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >   row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >   row4{"0", "0", "0", "1", "1", "1"};
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
      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000}, {1050, 1400}};

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

      gum::learning::NoPrior prior(database, nodeId2columns);
      gum::learning::ScoreK2 score(parser, prior, ranges, nodeId2columns);

      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::learning::ScoreK2::isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreK2::isPriorCompatible(prior))
      TS_GUM_ASSERT_THROWS_NOTHING(score.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isPriorCompatible(prior))

      std::vector< gum::NodeId > cond_empty;
      std::vector< gum::NodeId > cond2{node5};
      std::vector< gum::NodeId > cond3{node1};

      gum::learning::IdCondSet idset1(node2, cond_empty);    // #3,#0
      gum::learning::IdCondSet idset2(node2, cond2, true);   // #9,#3
      gum::learning::IdCondSet idset3(node2, cond3, true);   // #9,#3

      // idset1: node2 | emptyset
      std::vector< double > N_ijk_1{400.0, 75.0, 75.0};
      std::vector< double > N_ij_1;
      double                xscore_1 = _score_(N_ijk_1, N_ij_1);
      TS_ASSERT(_equal_(xscore_1, score.score(node2)))


      // idset2: node2 | node5
      std::vector< double > N_ijk_2{200, 75, 0, 200, 0, 75, 0, 0, 0};
      std::vector< double > N_ij_2{275, 275, 0};
      double                xscore_2 = _score_(N_ijk_2, N_ij_2);
      TS_ASSERT(_equal_(xscore_2, score.score(node2, cond2)))

      // idset3: node2 | node1
      std::vector< double > N_ijk_3{0, 75, 0, 200, 0, 75, 200, 0, 0};
      std::vector< double > N_ij_3{75, 275, 200};
      double                xscore_3 = _score_(N_ijk_3, N_ij_3);
      TS_ASSERT(_equal_(xscore_3, score.score(node2, cond3)))


      gum::learning::ScoreK2 score2(score);
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score2.isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score2.score(node2)))
      TS_ASSERT(_equal_(xscore_2, score2.score(node2, cond2)))
      TS_ASSERT(_equal_(xscore_3, score2.score(node2, cond3)))

      gum::learning::ScoreK2 score3(std::move(score2));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score3.isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score3.score(node2)))
      TS_ASSERT(_equal_(xscore_2, score3.score(node2, cond2)))
      TS_ASSERT(_equal_(xscore_3, score3.score(node2, cond3)))

      gum::learning::ScoreK2* score4 = score3.clone();
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node2)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node2, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node2, cond3)))

      score4->operator=(score);
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node2)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node2, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node2, cond3)))

      score4->operator=(std::move(score));
      TS_GUM_ASSERT_THROWS_NOTHING(
          score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
      TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

      TS_ASSERT(_equal_(xscore_1, score4->score(node2)))
      TS_ASSERT(_equal_(xscore_2, score4->score(node2, cond2)))
      TS_ASSERT(_equal_(xscore_3, score4->score(node2, cond3)))

      delete score4;
    }

    GUM_ACTIVE_TEST(_multicore) {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet trans_set;
      {
        const std::vector< std::string >              miss;
        gum::learning::DBTranslator4LabelizedVariable translator(var, miss);
        std::vector< std::string >                    names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable database(trans_set);
      std::vector< std::string >   row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >   row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >   row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >   row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >   row4{"0", "0", "0", "1", "1", "1"};
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
      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      std::vector< std::pair< std::size_t, std::size_t > > ranges{{800, 1000}, {1050, 1400}};

      gum::learning::NoPrior prior(database);
      for (std::size_t i = std::size_t(1); i < std::size_t(24); ++i) {
        gum::learning::ScoreK2 score(parser, prior, ranges);
        score.setNumberOfThreads(i);

        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::learning::ScoreK2::isPriorCompatible(gum::learning::PriorType::NoPriorType));
        TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreK2::isPriorCompatible(prior))
        TS_GUM_ASSERT_THROWS_NOTHING(
            score.isPriorCompatible(gum::learning::PriorType::NoPriorType));
        TS_GUM_ASSERT_THROWS_NOTHING(score.isPriorCompatible(prior))

        gum::NodeId                node0 = 0;
        gum::NodeId                node1 = 1;
        gum::NodeId                node3 = 3;
        std::vector< gum::NodeId > cond_empty;
        std::vector< gum::NodeId > cond2{node1};
        std::vector< gum::NodeId > cond3{node3};

        gum::learning::IdCondSet idset1(node0, cond_empty);    // #3,#0
        gum::learning::IdCondSet idset2(node0, cond2, true);   // #9,#3
        gum::learning::IdCondSet idset3(node0, cond3, true);   // #9,#3

        // idset1: node0 | emptyset
        std::vector< double > N_ijk_1{400.0, 75.0, 75.0};
        std::vector< double > N_ij_1;
        double                xscore_1 = _score_(N_ijk_1, N_ij_1);
        TS_ASSERT(_equal_(xscore_1, score.score(node0)))

        // idset2: node0 | node1
        std::vector< double > N_ijk_2{200, 75, 0, 200, 0, 75, 0, 0, 0};
        std::vector< double > N_ij_2{275, 275, 0};
        double                xscore_2 = _score_(N_ijk_2, N_ij_2);
        TS_ASSERT(_equal_(xscore_2, score.score(node0, cond2)))

        // idset3: node0 | node3
        std::vector< double > N_ijk_3{0, 75, 0, 200, 0, 75, 200, 0, 0};
        std::vector< double > N_ij_3{75, 275, 200};
        double                xscore_3 = _score_(N_ijk_3, N_ij_3);
        TS_ASSERT(_equal_(xscore_3, score.score(node0, cond3)))

        gum::learning::ScoreK2 score2(score);
        TS_GUM_ASSERT_THROWS_NOTHING(
            score2.isPriorCompatible(gum::learning::PriorType::NoPriorType));
        TS_GUM_ASSERT_THROWS_NOTHING(score2.isPriorCompatible(prior))

        TS_ASSERT(_equal_(xscore_1, score2.score(node0)))
        TS_ASSERT(_equal_(xscore_2, score2.score(node0, cond2)))
        TS_ASSERT(_equal_(xscore_3, score2.score(node0, cond3)))

        gum::learning::ScoreK2 score3(std::move(score2));
        TS_GUM_ASSERT_THROWS_NOTHING(
            score3.isPriorCompatible(gum::learning::PriorType::NoPriorType));
        TS_GUM_ASSERT_THROWS_NOTHING(score3.isPriorCompatible(prior))

        TS_ASSERT(_equal_(xscore_1, score3.score(node0)))
        TS_ASSERT(_equal_(xscore_2, score3.score(node0, cond2)))
        TS_ASSERT(_equal_(xscore_3, score3.score(node0, cond3)))

        gum::learning::ScoreK2* score4 = score3.clone();
        TS_GUM_ASSERT_THROWS_NOTHING(
            score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

        TS_ASSERT(_equal_(xscore_1, score4->score(node0)))
        TS_ASSERT(_equal_(xscore_2, score4->score(node0, cond2)))
        TS_ASSERT(_equal_(xscore_3, score4->score(node0, cond3)))

        score4->operator=(score);
        TS_GUM_ASSERT_THROWS_NOTHING(
            score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

        TS_ASSERT(_equal_(xscore_1, score4->score(node0)))
        TS_ASSERT(_equal_(xscore_2, score4->score(node0, cond2)))
        TS_ASSERT(_equal_(xscore_3, score4->score(node0, cond3)))

        score4->operator=(std::move(score));
        TS_GUM_ASSERT_THROWS_NOTHING(
            score4->isPriorCompatible(gum::learning::PriorType::NoPriorType));
        TS_GUM_ASSERT_THROWS_NOTHING(score4->isPriorCompatible(prior))

        TS_ASSERT(_equal_(xscore_1, score4->score(node0)))
        TS_ASSERT(_equal_(xscore_2, score4->score(node0, cond2)))
        TS_ASSERT(_equal_(xscore_3, score4->score(node0, cond3)))

        delete score4;
      }
    }
  };


} /* namespace gum_tests */
