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
#include <agrum/learning/database/DBRowGeneratorParser.h>
#include <agrum/learning/database/DBInitializerFromCSV.h>
#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/aprioris/aprioriNoApriori.h>
#include <agrum/learning/scores_and_tests/indepTestChi2.h>

namespace gum_tests {

  class IndepTestChi2TestSuite : public CxxTest::TestSuite {
    public:
    void test_chi2() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("asia.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<>                translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriNoApriori<>     apriori(database);
      gum::learning::IndepTestChi2<>        score(parser, apriori);

      TS_ASSERT_DELTA(score.score(0, 3), -0.999, 0.01);
      TS_ASSERT_DELTA(score.score(3, 1), -0.389, 0.01);
      TS_ASSERT_DELTA(score.score(0, 2), 227.09, 0.01);
      TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01);
      TS_ASSERT_DELTA(
         score.score(1, 3, std::vector< gum::NodeId >{4}), -0.648, 0.01);
      TS_ASSERT_DELTA(
         score.score(0, 2, std::vector< gum::NodeId >{4}), 145.376, 0.01);
      TS_ASSERT_DELTA(
         score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.692, 0.01);

      score.clear();
      TS_ASSERT_DELTA(score.score(1, 3), -0.389, 0.01);
      TS_ASSERT_DELTA(score.score(0, 1), 104.865, 0.01);
      TS_ASSERT_DELTA(
         score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.692, 0.01);
      TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01);
      TS_ASSERT_DELTA(
         score.score(3, 1, std::vector< gum::NodeId >{4}), -0.648, 0.01);
      TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01);
      TS_ASSERT_DELTA(
         score.score(1, 3, std::vector< gum::NodeId >{4}), -0.648, 0.01);
    }

    void test_cache() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("asia.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<>                translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriNoApriori<>     apriori(database);
      gum::learning::IndepTestChi2<>        score(parser, apriori);
      // score.useCache ( false );

      for (gum::Idx i = 0; i < 100; ++i) {
        score.clear();

        TS_ASSERT_DELTA(score.score(0, 1), 104.865, 0.01);
        TS_ASSERT_DELTA(score.score(1, 3), -0.389, 0.01);
        TS_ASSERT_DELTA(
           score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.692, 0.01);
        TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01);
        TS_ASSERT_DELTA(
           score.score(3, 1, std::vector< gum::NodeId >{4}), -0.648, 0.01);
        TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01);
        TS_ASSERT_DELTA(
           score.score(1, 3, std::vector< gum::NodeId >{4}), -0.648, 0.01);
      }
    }

    void test_clearcache() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("asia.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<>                translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriNoApriori<>     apriori(database);
      gum::learning::IndepTestChi2<>        score(parser, apriori);
      // score.useCache ( false );

      for (gum::Idx i = 0; i < 4; ++i) {
        score.clear();
        TS_ASSERT_DELTA(score.score(0, 1), 104.865, 0.01);
        TS_ASSERT_DELTA(score.score(1, 3), -0.389, 0.01);
        TS_ASSERT_DELTA(
           score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.692, 0.01);
        TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01);
        TS_ASSERT_DELTA(
           score.score(3, 1, std::vector< gum::NodeId >{4}), -0.648, 0.01);
        TS_ASSERT_DELTA(score.score(2, 0), 227.09, 0.01);
        TS_ASSERT_DELTA(
           score.score(1, 3, std::vector< gum::NodeId >{4}), -0.648, 0.01);
      }
    }

    void test_statistics() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("chi2.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<>                translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriNoApriori<>     apriori(database);
      gum::learning::IndepTestChi2<>        score(parser, apriori);

      // A:0, B:1, C:2, D:3
      auto stat = score.statistics(0, 2);
      TS_ASSERT_DELTA(stat.first, 0.0007, 1e-3);
      TS_ASSERT_DELTA(stat.second, 0.978, 1e-3);

      stat = score.statistics(1, 0);
      TS_ASSERT_DELTA(stat.first, 21.4348, 1e-3);
      TS_ASSERT_DELTA(stat.second, 3.6e-6, 1e-5);

      stat = score.statistics(0, 1);
      TS_ASSERT_DELTA(stat.first, 21.4348, 1e-3);
      TS_ASSERT_DELTA(stat.second, 3.6e-6, 1e-5);

      stat = score.statistics(1, 3);
      TS_ASSERT_DELTA(stat.first, 0.903, 1e-3);
      TS_ASSERT_DELTA(stat.second, 0.341, 1e-3);

      stat = score.statistics(0, 2, std::vector< gum::NodeId >{1});
      TS_ASSERT_DELTA(stat.first, 15.2205, 1e-3);
      TS_ASSERT_DELTA(stat.second, 0.0005, 1e-4);
    }

    void test_statistics_2() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("testXYbase.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<>                translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriNoApriori<>     apriori(database);
      gum::learning::IndepTestChi2<>        score(parser, apriori);

      auto stats = score.statistics(0, 1);
      TS_ASSERT_DELTA(stats.first, 15.3389, 1e-3);
      TS_ASSERT_DELTA(stats.second, 0.0177784, 1e-3);
    }
  };

} /* namespace gum_tests */
