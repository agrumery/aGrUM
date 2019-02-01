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
#include <agrum/learning/scores_and_tests/indepTestG2.h>

namespace gum_tests {

  class IndepTestG2TestSuite : public CxxTest::TestSuite {
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

      TS_ASSERT(fabs(score.score(0, 1) - 61.181) <= 0.01);
      TS_ASSERT(fabs(score.score(3, 1) + 0.617) <= 0.01);

      TS_ASSERT(fabs(score.score(0, 2) - 114.873) <= 0.01);
      TS_ASSERT(fabs(score.score(2, 0) - 114.873) <= 0.01);

      TS_ASSERT(fabs(score.score(1, 3, std::vector< gum::NodeId >{4}) + 0.778)
                <= 0.01);
      TS_ASSERT(fabs(score.score(0, 2, std::vector< gum::NodeId >{4}) - 73.36)
                <= 0.01);

      TS_ASSERT(fabs(score.score(3, 6, std::vector< gum::NodeId >{1, 2}) + 0.808)
                <= 0.01);

      TS_ASSERT(fabs(score.score(0, 1) - 61.181) <= 0.01);
      TS_ASSERT(fabs(score.score(1, 3) + 0.617) <= 0.01);
      TS_ASSERT(fabs(score.score(3, 6, std::vector< gum::NodeId >{1, 2}) + 0.808)
                <= 0.01);
      TS_ASSERT(fabs(score.score(2, 0) - 114.873) <= 0.01);
      TS_ASSERT(fabs(score.score(3, 1, std::vector< gum::NodeId >{4}) + 0.778)
                <= 0.01);
      TS_ASSERT(fabs(score.score(2, 0) - 114.873) <= 0.01);
      TS_ASSERT(fabs(score.score(1, 3, std::vector< gum::NodeId >{4}) + 0.778)
                <= 0.01);
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
        TS_ASSERT(fabs(score.score(0, 1) - 61.181) <= 0.01);
        TS_ASSERT(fabs(score.score(1, 3) + 0.617) <= 0.01);
        TS_ASSERT(fabs(score.score(3, 6, std::vector< gum::NodeId >{1, 2}) + 0.808)
                  <= 0.01);
        TS_ASSERT(fabs(score.score(2, 0) - 114.873) <= 0.01);
        TS_ASSERT(fabs(score.score(3, 1, std::vector< gum::NodeId >{4}) + 0.778)
                  <= 0.01);
        TS_ASSERT(fabs(score.score(2, 0) - 114.873) <= 0.01);
        TS_ASSERT(fabs(score.score(1, 3, std::vector< gum::NodeId >{4}) + 0.778)
                  <= 0.01);
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
        TS_ASSERT(fabs(score.score(0, 1) - 61.181) <= 0.01);
        TS_ASSERT(fabs(score.score(1, 3) + 0.617) <= 0.01);
        TS_ASSERT(fabs(score.score(3, 6, std::vector< gum::NodeId >{1, 2}) + 0.808)
                  <= 0.01);
        TS_ASSERT(fabs(score.score(2, 0) - 114.873) <= 0.01);
        TS_ASSERT(fabs(score.score(3, 1, std::vector< gum::NodeId >{4}) + 0.778)
                  <= 0.01);
        TS_ASSERT(fabs(score.score(2, 0) - 114.873) <= 0.01);
        TS_ASSERT(fabs(score.score(1, 3, std::vector< gum::NodeId >{4}) + 0.778)
                  <= 0.01);
      }
    }
  };

} /* namespace gum_tests */
