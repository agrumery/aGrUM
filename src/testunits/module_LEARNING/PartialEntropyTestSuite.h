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
#include <agrum/learning/scores_and_tests/partialEntropy.h>

namespace gum_tests {

  class PartialEntropyTestSuite : public CxxTest::TestSuite {
    public:
    void test_entropy() {
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

      gum::learning::PartialEntropy<> score(filter, modalities);
      /*
       * H de [3, 4] vaut 0.1941536767067103
           * H de [4, 0] vaut 1.0961635890107093
           * H de [1, 2, 5, 6] vaut 1.9712290949273594
           * H1 de [3] vaut 0.0988305218931441
           * H de [0, 1, 4, 6] vaut 2.2968805077576997
           * H de [0, 1, 4, 6, 7] vaut 2.575690824789043
       */
      gum::Idx id1 = score.addNodeSet(3);
      gum::Idx id2 = score.addNodeSet(std::vector< gum::Idx >{1, 2, 5, 6});
      gum::Idx id3 = score.addNodeSet(std::vector< gum::Idx >{3, 4});
      gum::Idx id4 = score.addNodeSet(3, 4);
      gum::Idx id5 = score.addNodeSet(std::vector< gum::Idx >{3});

      TS_ASSERT(fabs(score.score(id1) - 0.098) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) - 1.971) <= 0.01);
      TS_ASSERT(fabs(score.score(id3) - 0.194) <= 0.01);
      TS_ASSERT(fabs(score.score(id4) - 0.194) <= 0.01);
      TS_ASSERT(fabs(score.score(id5) - 0.098) <= 0.01);

      score.clear();
      id1 = score.addNodeSet(3, 4);
      id2 = score.addNodeSet(4, 3);
      TS_ASSERT(fabs(score.score(id1) - 0.194) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) - 0.194) <= 0.01);

      score.clear();
      id1 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 0, 6});
      id2 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 6, 0});
      id3 = score.addNodeSet(std::vector< gum::Idx >{0, 1, 4, 6});
      id4 = score.addNodeSet(std::vector< gum::Idx >{4, 6, 0, 1});
      id5 = score.addNodeSet(std::vector< gum::Idx >{6, 0, 4, 1});
      TS_ASSERT(fabs(score.score(id1) - 2.2968) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) - 2.2968) <= 0.01);
      TS_ASSERT(fabs(score.score(id3) - 2.2968) <= 0.01);
      TS_ASSERT(fabs(score.score(id4) - 2.2968) <= 0.01);
      TS_ASSERT(fabs(score.score(id5) - 2.2968) <= 0.01);

      score.clear();
      id1 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 0, 6, 5});
      id2 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 6, 0, 5});
      id3 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 0, 5, 6});
      id4 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 6, 5, 0});
      TS_ASSERT(fabs(score.score(id1) - 2.2968) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) - 2.2968) <= 0.01);
      TS_ASSERT(fabs(score.score(id3) - 2.2968) <= 0.01);
      TS_ASSERT(fabs(score.score(id4) - 2.2968) <= 0.01);

      score.clear();
      id1 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 0, 6, 7});
      id2 = score.addNodeSet(std::vector< gum::Idx >{1, 4, 6, 0, 7});
      id3 = score.addNodeSet(std::vector< gum::Idx >{7, 0, 1, 4, 6});
      id4 = score.addNodeSet(std::vector< gum::Idx >{4, 6, 7, 0, 1});
      id5 = score.addNodeSet(std::vector< gum::Idx >{6, 7, 0, 4, 1});
      TS_ASSERT(fabs(score.score(id1) - 2.575) <= 0.01);
      TS_ASSERT(fabs(score.score(id2) - 2.575) <= 0.01);
      TS_ASSERT(fabs(score.score(id3) - 2.575) <= 0.01);
      TS_ASSERT(fabs(score.score(id4) - 2.575) <= 0.01);
      TS_ASSERT(fabs(score.score(id5) - 2.575) <= 0.01);

      /*
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
