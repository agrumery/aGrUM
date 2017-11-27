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
    void test() {
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
      /* 3-4 K 2.6142422616165197
       * 2-6 K 4.1414644088786756
       * 4-7|5 K 3.6019012803288695
       */
      gum::Idx id1 = score.addNodeSet(3, 4);
      gum::Idx id2 = score.addNodeSet(2, 6);
      gum::Idx id3 = score.addNodeSet(4, 7, std::vector< gum::Idx >{5});
      TS_ASSERT_DELTA(score.score(id1), 2.6142, 1e-2);
      TS_ASSERT_DELTA(score.score(id2), 4.1414, 1e-2);
      TS_ASSERT_DELTA(score.score(id3), 3.6019, 1e-2);

      score.clear();
      id1 = score.addNodeSet(6, 7, std::vector< gum::Idx >{5, 1, 4});
      id2 = score.addNodeSet(6, 7, std::vector< gum::Idx >{1, 4});
      TS_ASSERT_DELTA(score.score(id1) - score.score(id2), 0.0, 1e-2);
      /*
            score.clear();
            id1 = score.addNodeSet( 0, 2 );
            id2 = score.addNodeSet( 2, 0 );
            TS_ASSERT( fabs( score.score( id1 ) - 227.09 ) <= 0.01 );
            TS_ASSERT( fabs( score.score( id2 ) - 227.09 ) <= 0.01 );

            score.clear();
            id1 = score.addNodeSet( 1, 3, std::vector<gum::Idx>{4} );
            id2 = score.addNodeSet( 0, 2, std::vector<gum::Idx>{4} );
            TS_ASSERT( fabs( score.score( id1 ) + 0.648 ) <= 0.01 );
            TS_ASSERT( fabs( score.score( id2 ) - 145.376 ) <= 0.01 );

            score.clear();
            id1 = score.addNodeSet( 3, 6, std::vector<gum::Idx>{1, 2} );
            TS_ASSERT( fabs( score.score( id1 ) + 0.692 ) <= 0.01 );

            gum::Idx id3, id4, id5, id6, id7;
            score.clear();
            id1 = score.addNodeSet( 0, 1 );
            id2 = score.addNodeSet( 1, 3 );
            id3 = score.addNodeSet( 3, 6, std::vector<gum::Idx>{1, 2} );
            id4 = score.addNodeSet( 2, 0 );
            id5 = score.addNodeSet( 3, 1, std::vector<gum::Idx>{4} );
            id6 = score.addNodeSet( 2, 0 );
            id7 = score.addNodeSet( 1, 3, std::vector<gum::Idx>{4} );
            TS_ASSERT( fabs( score.score( id1 ) - 104.865 ) <= 0.01 );
            TS_ASSERT( fabs( score.score( id2 ) + 0.389 ) <= 0.01 );
            TS_ASSERT( fabs( score.score( id3 ) + 0.692 ) <= 0.01 );
            TS_ASSERT( fabs( score.score( id4 ) - 227.09 ) <= 0.01 );
            TS_ASSERT( fabs( score.score( id5 ) + 0.648 ) <= 0.01 );
            TS_ASSERT( fabs( score.score( id6 ) - 227.09 ) <= 0.01 );
            TS_ASSERT( fabs( score.score( id7 ) + 0.648 ) <= 0.01 );
            */
    }
  };

} /* namespace gum_tests */
