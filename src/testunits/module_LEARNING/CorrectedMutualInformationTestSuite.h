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
#include <agrum/learning/scores_and_tests/correctedMutualInformation.h>

namespace gum_tests {

  class CorrectedMutualInformationTestSuite : public CxxTest::TestSuite {
    public:
    void test_Ixy_Kmdl() {
      gum::learning::DatabaseFromCSV database( GET_RESSOURCES_PATH( "asia.csv" ) );

      gum::learning::DBRowTranslatorSet<gum::learning::CellTranslatorCompactIntId> translators;
      translators.insertTranslator ( 0, 8 );
      
      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();

 
      auto filter =
          gum::learning::make_DB_row_filter( database, translators, generators );

      std::vector<gum::Size> modalities = filter.modalities();

      gum::learning::CorrectedMutualInformation<> score( filter, modalities );
      score.useMDL();
      /*
       * Edge 2-6 Ixy 0.36284131805213327 K 4.605170185988092
       * Edge 3-4 Ixy 0.0009904872464745473 K 4.605170185988092
       * Edge 4-7 Ixy 0.037064832434445805 K 4.605170185988092
       * Edge 4-7 [5] Ixy 1.1096815944910965e-05 K 9.210340371976184
       * Edge 1-6 [0] Ixy 0.016136807112147045 K 9.210340371976184
       */
      TS_ASSERT_DELTA( score.score( 2, 6 ), 0.36284131 - 4.605/10000, 1e-4 );
      TS_ASSERT_DELTA( score.score( std::pair<gum::Idx, gum::Idx>{4, 7} ), 0.0370648 - 4.605/10000, 1e-4 );
      TS_ASSERT_DELTA( score.score( std::pair<gum::Idx, gum::Idx>{4, 7}, std::vector<gum::Idx>{5} ), 1.10968e-5 - 9.2103/10000, 1e-5 );
      TS_ASSERT_DELTA( score.score( 1, 6, std::vector<gum::Idx>{0} ), 0.0161368 - 9.210/10000, 1e-4 );
      TS_ASSERT_DELTA( score.score( 2, 6, std::vector<gum::Idx>{} ), 0.36284131 - 4.605/10000, 1e-4 );

      score.clear();
      TS_ASSERT_DELTA( score.score( 6, 2 ), 0.36284131 - 4.605/10000, 1e-4 );
      TS_ASSERT_DELTA( score.score( 2, 6 ), 0.36284131 - 4.605/10000, 1e-4 );

      /* Ixyz de 263 vaut -7.199852186379196e-05
       * Triple 0 - 1 - 5 | [] with I(x,y,z)=0.024482957033569175 k=4.605170185988092
       * Triple 4 - 5 - 7 | [] with I(x,y,z)=0.037053735618500894 k=4.605170185988092
       * Triple 2 - 6 - 5 | [1] with I(x,y,z)=-0.00550537256993211 k=9.210340371976184
       * Triple 2 - 7 - 5 | [1, 4] with I(x,y,z)=1.1102230246251565e-16
       * Triple 0 - 6 - 5 | [1, 4] with I(x,y,z)=0.0
       */
      score.clear();
      TS_ASSERT_DELTA( score.score( 0, 1, 5 ), 0.0244829 + 4.605/10000, 1e-4 );
      TS_ASSERT_DELTA( score.score( 2, 6, 5, std::vector<gum::Idx>{1} ), - 0.005505 + 9.210/10000, 1e-4 );
      score.clear();
      TS_ASSERT_DELTA( score.score( 4, 5, 7 ), 0.0370537 + 4.605/10000, 1e-4 );
      TS_ASSERT_DELTA( score.score( 4, 7, 5 ), 0.0370537 + 4.605/10000, 1e-4 );
      TS_ASSERT_DELTA( score.score( 5, 4, 7 ), 0.0370537 + 4.605/10000, 1e-4 );
      TS_ASSERT_DELTA( score.score( 5, 7, 4 ), 0.0370537 + 4.605/10000, 1e-4 );
      TS_ASSERT_DELTA( score.score( 7, 5, 4 ), 0.0370537 + 4.605/10000, 1e-4 );
      TS_ASSERT_DELTA( score.score( 7, 4, 5 ), 0.0370537 + 4.605/10000, 1e-4 );

/*
      score.clear();
      TS_ASSERT_DELTA ( score.score( 0, 6, 5, std::vector<gum::Idx>{1, 4} ) == 0.0 );
      TS_ASSERT_DELTA ( score.score( 6, 0, 5, std::vector<gum::Idx>{1, 4} ) == 0.0 );
*/
    }

    void test_Ixy_NoCorr() {
      gum::learning::DatabaseFromCSV database( GET_RESSOURCES_PATH( "asia.csv" ) );

      gum::learning::DBRowTranslatorSet<gum::learning::CellTranslatorCompactIntId> translators;
      translators.insertTranslator ( 0, 8 );

      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();


      auto filter =
          gum::learning::make_DB_row_filter( database, translators, generators );

      std::vector<gum::Size> modalities = filter.modalities();

      gum::learning::CorrectedMutualInformation<> score( filter, modalities );
      score.useNoCorr();
      /*
       * Edge 2-6 Ixy 0.36284131805213327
       * Edge 3-4 Ixy 0.0009904872464745473
       * Edge 4-7 Ixy 0.037064832434445805
       * Edge 1-6 [0] Ixy 0.016136807112147045
       */
      TS_ASSERT_DELTA( score.score( 2, 6 ), 0.362, 0.001 );
      TS_ASSERT_DELTA( score.score( std::pair<gum::Idx, gum::Idx>{4, 7} ), 0.0370, 0.001 );
      TS_ASSERT_DELTA( score.score( 1, 6, std::vector<gum::Idx>{0} ), 0.0161, 0.001 );
      TS_ASSERT_DELTA( score.score( 2, 6, std::vector<gum::Idx>{} ), 0.362, 0.001 );

      score.clear();
      TS_ASSERT_DELTA( score.score( 6, 2 ), 0.362, 0.001 );
      TS_ASSERT_DELTA( score.score( 2, 6 ), 0.362, 0.001 );

      /* Ixyz de 263 vaut -7.199852186379196e-05
       * Triple 0 - 1 - 5 | [] with I(x,y,z)=0.024482957033569175
       * Triple 4 - 5 - 7 | [] with I(x,y,z)=0.037053735618500894
       * Triple 2 - 6 - 5 | [1] with I(x,y,z)=-0.00550537256993211
       * Triple 2 - 7 - 5 | [1, 4] with I(x,y,z)=1.1102230246251565e-16
       * Triple 0 - 6 - 5 | [1, 4] with I(x,y,z)=0.0
       */
      score.clear();
      TS_ASSERT_DELTA( score.score( 0, 1, 5 ), 0.024, 0.01 );
      TS_ASSERT_DELTA( score.score( 2, 6, 5, std::vector<gum::Idx>{1} ), - 0.005, 0.001 );
      score.clear();
      TS_ASSERT_DELTA( score.score( 4, 5, 7 ), 0.0371, 0.001 );
      TS_ASSERT_DELTA( score.score( 4, 7, 5 ), 0.0371, 0.001 );
      TS_ASSERT_DELTA( score.score( 5, 4, 7 ), 0.0371, 0.001 );
      TS_ASSERT_DELTA( score.score( 5, 7, 4 ), 0.0371, 0.001 );
      TS_ASSERT_DELTA( score.score( 7, 5, 4 ), 0.0371, 0.001 );
      TS_ASSERT_DELTA( score.score( 7, 4, 5 ), 0.0371, 0.001 );

      score.clear();
      std::cout << "Ixyz|ui(6;5;0|1;4)" << std::endl;
      TS_ASSERT_DELTA( score.score( 0, 6, 5, std::vector<gum::Idx>{1, 4} ), 0.0, 1e-4 );
      TS_ASSERT_DELTA( score.score( 6, 0, 5, std::vector<gum::Idx>{1, 4} ), 0.0, 1e-4 );
    }
  };

} /* namespace gum_tests */
