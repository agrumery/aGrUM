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

#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>
#include <agrum/learning/scores_and_tests/kNML.h>

namespace gum_tests {

  class KNMLTestSuite : public CxxTest::TestSuite {
    public:
    void test1() {
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

      gum::learning::KNML<> score(filter, modalities);
      /* 3-4 K 2.6844818514806183
       * 2-6 K 4.1414644088786756
       * 4-7|5 K 3.763846399915938
       */
      gum::Idx id1 = score.addNodeSet(3, 4);
      gum::Idx id2 = score.addNodeSet(2, 6);
      gum::Idx id3 = score.addNodeSet(4, 7, std::vector< gum::Idx >{5});
      TS_ASSERT_DELTA(score.score(id1), 2.68448, 1e-2);
      TS_ASSERT_DELTA(score.score(id2), 4.1414, 1e-2);
      TS_ASSERT_DELTA(score.score(id3), 3.76384, 1e-2);

      score.clear();
      id1 = score.addNodeSet(6, 7, std::vector< gum::Idx >{5, 1, 4});
      id2 = score.addNodeSet(6, 7, std::vector< gum::Idx >{1, 4});
      TS_ASSERT_DELTA(score.score(id1) - score.score(id2), 0.0, 1e-2);
    }
  };

} /* namespace gum_tests */
