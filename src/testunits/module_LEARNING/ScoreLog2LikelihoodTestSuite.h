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

#include <iostream>
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>
#include <agrum/learning/scores_and_tests/scoreLog2Likelihood.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/aprioris/aprioriSmoothing.h>
#include <agrum/learning/aprioris/aprioriNoApriori.h>

namespace gum_tests {

  class ScoreLog2LikelihoodTestSuite : public CxxTest::TestSuite {
    public:
    void test_LL() {
      gum::learning::DatabaseFromCSV database(GET_PATH_STR("asia.csv"));

      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8>());

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());

      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities(8, 2);

      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreLog2Likelihood<> score(filter, modalities, apriori);

      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::learning::ScoreLog2Likelihood<>::isAprioriCompatible(
              gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::learning::ScoreLog2Likelihood<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
          score.isAprioriCompatible(gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      unsigned int id1 = score.addNodeSet(3);
      unsigned int id2 = score.addNodeSet(1);
      TS_ASSERT(fabs(score.score(id1) + 988.314) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) + 3023.254) <= 0.01);

      score.clear();
      id1 = score.addNodeSet(0);
      id2 = score.addNodeSet(2);
      TS_ASSERT(fabs(score.score(id1) + 9999.767) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) + 9929.478) <= 0.01);

      score.clear();
      id1 = score.addNodeSet(3, std::vector<unsigned int>{4});
      id2 = score.addNodeSet(1, std::vector<unsigned int>{4});
      TS_ASSERT(fabs(score.score(id1) + 978.407) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) + 3017.57) <= 0.01);

      score.clear();
      id1 = score.addNodeSet(3, std::vector<unsigned int>{1, 2});
      TS_ASSERT(fabs(score.score(id1) + 985.801) <= 0.01);
      return;
      unsigned int id3, id4, id5, id6, id7;
      score.clear();
      id1 = score.addNodeSet(3);
      id2 = score.addNodeSet(1);
      id3 = score.addNodeSet(3, std::vector<unsigned int>{1, 2});
      id4 = score.addNodeSet(2);
      id5 = score.addNodeSet(3, std::vector<unsigned int>{4});
      id6 = score.addNodeSet(2);
      id7 = score.addNodeSet(3, std::vector<unsigned int>{4});
      TS_ASSERT(fabs(score.score(id1) + 988.314) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) + 3023.254) <= 0.01);
      TS_ASSERT(fabs(score.score(id3) + 985.801) <= 0.01);
      TS_ASSERT(fabs(score.score(id4) + 9929.478) <= 0.01);
      TS_ASSERT(fabs(score.score(id5) + 978.407) <= 0.01);
      TS_ASSERT(fabs(score.score(id6) + 9929.478) <= 0.01);
      TS_ASSERT(fabs(score.score(id7) + 978.407) <= 0.01);
    }

    void test_cache() {
      gum::learning::DatabaseFromCSV database(GET_PATH_STR("asia.csv"));
      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8>());
      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());
      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);
      std::vector<unsigned int> modalities = filter.modalities();
      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreLog2Likelihood<> score(filter, modalities, apriori);
      // score.useCache ( false );

      unsigned int id1, id2, id3, id4, id5, id6, id7;
      for (unsigned int i = 0; i < 10000; ++i) {
        score.clear();
        id1 = score.addNodeSet(3);
        id2 = score.addNodeSet(1);
        id3 = score.addNodeSet(3, std::vector<unsigned int>{1, 2});
        id4 = score.addNodeSet(2);
        id5 = score.addNodeSet(3, std::vector<unsigned int>{4});
        id6 = score.addNodeSet(2);
        id7 = score.addNodeSet(3, std::vector<unsigned int>{4});
        TS_ASSERT(fabs(score.score(id1) + 988.314) <= 0.01);
        TS_ASSERT(fabs(score.score(id2) + 3023.254) <= 0.01);
        TS_ASSERT(fabs(score.score(id3) + 985.801) <= 0.01);
        TS_ASSERT(fabs(score.score(id4) + 9929.478) <= 0.01);
        TS_ASSERT(fabs(score.score(id5) + 978.407) <= 0.01);
        TS_ASSERT(fabs(score.score(id6) + 9929.478) <= 0.01);
        TS_ASSERT(fabs(score.score(id7) + 978.407) <= 0.01);
      }
    }

    void test_clearcache() {
      gum::learning::DatabaseFromCSV database(GET_PATH_STR("asia.csv"));
      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8>());
      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());
      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);
      std::vector<unsigned int> modalities = filter.modalities();
      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreLog2Likelihood<> score(filter, modalities, apriori);
      // score.useCache ( false );

      unsigned int id1, id2, id3, id4, id5, id6, id7;
      for (unsigned int i = 0; i < 4; ++i) {
        score.clearCache();
        id1 = score.addNodeSet(3);
        id2 = score.addNodeSet(1);
        id3 = score.addNodeSet(3, std::vector<unsigned int>{1, 2});
        id4 = score.addNodeSet(2);
        id5 = score.addNodeSet(3, std::vector<unsigned int>{4});
        id6 = score.addNodeSet(2);
        id7 = score.addNodeSet(3, std::vector<unsigned int>{4});
        TS_ASSERT(fabs(score.score(id1) + 988.314) <= 0.01);
        TS_ASSERT(fabs(score.score(id2) + 3023.254) <= 0.01);
        TS_ASSERT(fabs(score.score(id3) + 985.801) <= 0.01);
        TS_ASSERT(fabs(score.score(id4) + 9929.478) <= 0.01);
        TS_ASSERT(fabs(score.score(id5) + 978.407) <= 0.01);
        TS_ASSERT(fabs(score.score(id6) + 9929.478) <= 0.01);
        TS_ASSERT(fabs(score.score(id7) + 978.407) <= 0.01);
      }
    }
  };

} /* namespace gum_tests */
