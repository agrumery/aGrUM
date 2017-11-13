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
#include <sstream>

#include <agrum/core/set.h>
#include <agrum/learning/aprioris/aprioriNoApriori.h>
#include <agrum/learning/aprioris/aprioriSmoothing.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>
#include <agrum/learning/scores_and_tests/scoreBIC.h>

namespace gum_tests {

  class ScoreBICTestSuite : public CxxTest::TestSuite {
    public:
    void test_bic() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("asia.csv"));

      gum::learning::DBRowTranslatorSet<
        gum::learning::CellTranslatorCompactIntId >
        translators;
      translators.insertTranslator(0, 8);

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();
      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector< gum::Size > modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreBIC<> score(filter, modalities, apriori);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::learning::ScoreBIC<>::isAprioriCompatible(
        gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(
        gum::learning::ScoreBIC<>::isAprioriCompatible(apriori));
      TS_GUM_ASSERT_THROWS_NOTHING(
        score.isAprioriCompatible(gum::learning::AprioriSmoothing<>::type::type));
      TS_GUM_ASSERT_THROWS_NOTHING(score.isAprioriCompatible(apriori));

      gum::Idx id1 = score.addNodeSet(3);
      gum::Idx id2 = score.addNodeSet(1);
      TS_ASSERT(fabs(score.score(id1) + 994.953) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) + 3029.892) <= 0.01);

      score.clear();
      id1 = score.addNodeSet(0);
      id2 = score.addNodeSet(2);
      TS_ASSERT(fabs(score.score(id1) + 10006.417) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) + 9936.115) <= 0.01);

      score.clear();
      id1 = score.addNodeSet(3, std::vector< gum::Idx >{4});
      id2 = score.addNodeSet(1, std::vector< gum::Idx >{4});
      TS_ASSERT(fabs(score.score(id1) + 991.688) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) + 3030.86) <= 0.01);

      score.clear();
      id1 = score.addNodeSet(3, std::vector< gum::Idx >{1, 2});
      TS_ASSERT(fabs(score.score(id1) + 1012.376) <= 0.01);

      gum::Idx id3, id4, id5, id6, id7;
      score.clear();
      id1 = score.addNodeSet(3);
      id2 = score.addNodeSet(1);
      id3 = score.addNodeSet(3, std::vector< gum::Idx >{1, 2});
      id4 = score.addNodeSet(2);
      id5 = score.addNodeSet(3, std::vector< gum::Idx >{4});
      id6 = score.addNodeSet(2);
      id7 = score.addNodeSet(3, std::vector< gum::Idx >{4});
      TS_ASSERT(fabs(score.score(id1) + 994.953) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) + 3029.892) <= 0.01);
      TS_ASSERT(fabs(score.score(id3) + 1012.376) <= 0.01);
      TS_ASSERT(fabs(score.score(id4) + 9936.115) <= 0.01);
      TS_ASSERT(fabs(score.score(id5) + 991.688) <= 0.01);
      TS_ASSERT(fabs(score.score(id6) + 9936.115) <= 0.01);
      TS_ASSERT(fabs(score.score(id7) + 991.688) <= 0.01);
    }

    void test_cache() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("asia.csv"));

      gum::learning::DBRowTranslatorSet<
        gum::learning::CellTranslatorCompactIntId >
        translators;
      translators.insertTranslator(0, 8);

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();
      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);
      std::vector< gum::Idx >           modalities = filter.modalities();
      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreBIC<> score(filter, modalities, apriori);
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
        TS_ASSERT(fabs(score.score(id1) + 994.953) <= 0.01);
        TS_ASSERT(fabs(score.score(id2) + 3029.892) <= 0.01);
        TS_ASSERT(fabs(score.score(id3) + 1012.376) <= 0.01);
        TS_ASSERT(fabs(score.score(id4) + 9936.115) <= 0.01);
        TS_ASSERT(fabs(score.score(id5) + 991.688) <= 0.01);
        TS_ASSERT(fabs(score.score(id6) + 9936.115) <= 0.01);
        TS_ASSERT(fabs(score.score(id7) + 991.688) <= 0.01);
      }
    }

    void test_clearcache() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("asia.csv"));

      gum::learning::DBRowTranslatorSet<
        gum::learning::CellTranslatorCompactIntId >
        translators;
      translators.insertTranslator(0, 8);

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();
      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);
      std::vector< gum::Idx >           modalities = filter.modalities();
      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreBIC<> score(filter, modalities, apriori);

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
        TS_ASSERT(fabs(score.score(id1) + 994.953) <= 0.01);
        TS_ASSERT(fabs(score.score(id2) + 3029.892) <= 0.01);
        TS_ASSERT(fabs(score.score(id3) + 1012.376) <= 0.01);
        TS_ASSERT(fabs(score.score(id4) + 9936.115) <= 0.01);
        TS_ASSERT(fabs(score.score(id5) + 991.688) <= 0.01);
        TS_ASSERT(fabs(score.score(id6) + 9936.115) <= 0.01);
        TS_ASSERT(fabs(score.score(id7) + 991.688) <= 0.01);
      }
    }
  };

} /* namespace gum_tests */
