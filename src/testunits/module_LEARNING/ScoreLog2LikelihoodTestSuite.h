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

#include <agrum/learning/aprioris/aprioriNoApriori.h>
#include <agrum/learning/aprioris/aprioriSmoothing.h>
#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBRowGeneratorParser.h>
#include <agrum/learning/database/DBInitializerFromCSV.h>
#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/scores_and_tests/scoreLog2Likelihood.h>

namespace gum_tests {

  class ScoreLog2LikelihoodTestSuite : public CxxTest::TestSuite {
    public:
    void test_LL() {
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

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreLog2Likelihood<> score(parser, modalities, apriori);

      TS_GUM_ASSERT_THROWS_NOTHING(
        gum::learning::ScoreLog2Likelihood<>::isAprioriCompatible(
          gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
        gum::learning::ScoreLog2Likelihood<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
        score.isAprioriCompatible(gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      gum::Idx id1 = score.addNodeSet(3);
      gum::Idx id2 = score.addNodeSet(1);
      TS_ASSERT_DELTA(score.score(id1), -988.314, 1e-2);
      TS_ASSERT_DELTA(score.score(id2), -3023.254, 1e-2);

      score.clear();
      id1 = score.addNodeSet(0);
      id2 = score.addNodeSet(2);
      TS_ASSERT_DELTA(score.score(id1), -9999.767, 1e-2);
      TS_ASSERT_DELTA(score.score(id2), -9929.478, 1e-2);

      score.clear();
      id1 = score.addNodeSet(3, std::vector< gum::Idx >{4});
      id2 = score.addNodeSet(1, std::vector< gum::Idx >{4});
      TS_ASSERT_DELTA(score.score(id1), -978.407, 1e-2);
      TS_ASSERT_DELTA(score.score(id2), -3017.57, 1e-2);

      score.clear();
      id1 = score.addNodeSet(3, std::vector< gum::Idx >{1, 2});
      TS_ASSERT_DELTA(score.score(id1), -985.801, 1e-2);
      return;
      gum::Idx id3, id4, id5, id6, id7;
      score.clear();
      id1 = score.addNodeSet(3);
      id2 = score.addNodeSet(1);
      id3 = score.addNodeSet(3, std::vector< gum::Idx >{1, 2});
      id4 = score.addNodeSet(2);
      id5 = score.addNodeSet(3, std::vector< gum::Idx >{4});
      id6 = score.addNodeSet(2);
      id7 = score.addNodeSet(3, std::vector< gum::Idx >{4});
      TS_ASSERT_DELTA(score.score(id1), -988.314, 1e-2);
      TS_ASSERT_DELTA(score.score(id2), -3023.254, 1e-2);
      TS_ASSERT_DELTA(score.score(id3), -985.801, 1e-2);
      TS_ASSERT_DELTA(score.score(id4), -9929.478, 1e-2);
      TS_ASSERT_DELTA(score.score(id5), -978.407, 1e-2);
      TS_ASSERT_DELTA(score.score(id6), -9929.478, 1e-2);
      TS_ASSERT_DELTA(score.score(id7), -978.407, 1e-2);
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

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreLog2Likelihood<> score(parser, modalities, apriori);
      // score.useCache ( false );

      gum::Idx id1, id2, id3, id4, id5, id6, id7;
      for (gum::Idx i = 0; i < 10000; ++i) {
        score.clear();
        id1 = score.addNodeSet(3);
        id2 = score.addNodeSet(1);
        id3 = score.addNodeSet(3, std::vector< gum::Idx >{1, 2});
        id4 = score.addNodeSet(2);
        id5 = score.addNodeSet(3, std::vector< gum::Idx >{4});
        id6 = score.addNodeSet(2);
        id7 = score.addNodeSet(3, std::vector< gum::Idx >{4});
        TS_ASSERT_DELTA(score.score(id1), -988.314, 1e-2);
        TS_ASSERT_DELTA(score.score(id2), -3023.254, 1e-2);
        TS_ASSERT_DELTA(score.score(id3), -985.801, 1e-2);
        TS_ASSERT_DELTA(score.score(id4), -9929.478, 1e-2);
        TS_ASSERT_DELTA(score.score(id5), -978.407, 1e-2);
        TS_ASSERT_DELTA(score.score(id6), -9929.478, 1e-2);
        TS_ASSERT_DELTA(score.score(id7), -978.407, 1e-2);
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

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreLog2Likelihood<> score(parser, modalities, apriori);
      // score.useCache ( false );

      gum::Idx id1, id2, id3, id4, id5, id6, id7;
      for (gum::Idx i = 0; i < 4; ++i) {
        score.clearCache();
        id1 = score.addNodeSet(3);
        id2 = score.addNodeSet(1);
        id3 = score.addNodeSet(3, std::vector< gum::Idx >{1, 2});
        id4 = score.addNodeSet(2);
        id5 = score.addNodeSet(3, std::vector< gum::Idx >{4});
        id6 = score.addNodeSet(2);
        id7 = score.addNodeSet(3, std::vector< gum::Idx >{4});
        TS_ASSERT_DELTA(score.score(id1), -988.314, 1e-2);
        TS_ASSERT_DELTA(score.score(id2), -3023.254, 1e-2);
        TS_ASSERT_DELTA(score.score(id3), -985.801, 1e-2);
        TS_ASSERT_DELTA(score.score(id4), -9929.478, 1e-2);
        TS_ASSERT_DELTA(score.score(id5), -978.407, 1e-2);
        TS_ASSERT_DELTA(score.score(id6), -9929.478, 1e-2);
        TS_ASSERT_DELTA(score.score(id7), -978.407, 1e-2);
      }
    }
  };

} /* namespace gum_tests */
