
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
#include <agrum/tools/database/DBRowGeneratorParser.h>
#include <agrum/tools/database/DBInitializerFromCSV.h>
#include <agrum/tools/database/databaseTable.h>
#include <agrum/tools/database/DBTranslatorSet.h>
#include <agrum/BN/learning/aprioris/aprioriNoApriori.h>
#include <agrum/tools/stattests/indepTestG2.h>

namespace gum_tests {

  class IndepTestG2TestSuite: public CxxTest::TestSuite {
    public:
    void test_G2() {
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
      gum::learning::IndepTestG2<>          score(parser, apriori);

      TS_ASSERT_DELTA(score.score(0, 1), 123.3614, 1e-3);
      TS_ASSERT_DELTA(score.score(3, 1), -0.2345, 1e-3);

      TS_ASSERT_DELTA(score.score(0, 2), 230.7461, 1e-3);
      TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3);

      TS_ASSERT_DELTA(
         score.score(1, 3, std::vector< gum::NodeId >{4}), -0.5569, 1e-3)
      TS_ASSERT_DELTA(
         score.score(0, 2, std::vector< gum::NodeId >{4}), 147.7192, 1e-3);

      TS_ASSERT_DELTA(
         score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.6153, 1e-3);

      TS_ASSERT_DELTA(score.score(0, 1), 123.3614, 1e-3);
      TS_ASSERT_DELTA(score.score(1, 3), -0.2345, 1e-3);
      TS_ASSERT_DELTA(
         score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.6153, 1e-3);
      TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3);
      TS_ASSERT_DELTA(
         score.score(3, 1, std::vector< gum::NodeId >{4}), -0.5569, 1e-3);
      TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3);
      TS_ASSERT_DELTA(
         score.score(1, 3, std::vector< gum::NodeId >{4}), -0.5569, 1e-3);
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
      gum::learning::IndepTestG2<>          score(parser, apriori);
      // score.useCache ( false );

      for (gum::Idx i = 0; i < 1000; ++i) {
        TS_ASSERT_DELTA(score.score(0, 1), 123.3614, 1e-3);
        TS_ASSERT_DELTA(score.score(1, 3), -0.2345, 1e-3);
        TS_ASSERT_DELTA(
           score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.6153, 1e-3);
        TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3);
        TS_ASSERT_DELTA(
           score.score(3, 1, std::vector< gum::NodeId >{4}), -0.5569, 1e-3);
        TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3);
        TS_ASSERT_DELTA(
           score.score(1, 3, std::vector< gum::NodeId >{4}), -0.5569, 1e-3);
      }
    }

    void xtest_clearcache() {
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
      gum::learning::IndepTestG2<>          score(parser, apriori);
      // score.useCache ( false );

      for (gum::Idx i = 0; i < 4; ++i) {
        score.clearCache();
        TS_ASSERT_DELTA(score.score(0, 1), 61.181, 1e-3);
        TS_ASSERT_DELTA(score.score(1, 3), 0.617, 1e-3);
        TS_ASSERT_DELTA(
           score.score(3, 6, std::vector< gum::NodeId >{1, 2}), -0.6153, 1e-3);
        TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3);
        TS_ASSERT_DELTA(
           score.score(3, 1, std::vector< gum::NodeId >{4}), 0.778, 1e-3);
        TS_ASSERT_DELTA(score.score(2, 0), 230.7461, 1e-3);
        TS_ASSERT_DELTA(
           score.score(1, 3, std::vector< gum::NodeId >{4}), 0.778, 1e-3);
      }
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
      gum::learning::IndepTestG2<>          score(parser, apriori);

      auto stats = score.statistics(0, 1);
      TS_ASSERT_DELTA(stats.first, 16.6066, 1e-3);
      TS_ASSERT_DELTA(stats.second, 0.0108433, 1e-3);
    }
  };

} /* namespace gum_tests */
