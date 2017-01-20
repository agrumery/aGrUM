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
#include <agrum/learning/scores_and_tests/recordCounter.h>

namespace gum_tests {

  class RecordCounterTestSuite : public CxxTest::TestSuite {
    public:
    void test1() {
      gum::learning::DatabaseFromCSV database( GET_RESSOURCES_PATH( "asia.csv" ) );

      gum::learning::DBRowTranslatorSet<gum::learning::CellTranslatorCompactIntId> translators;
      translators.insertTranslator ( 0, 8 );
      
      auto generators =
          gum::learning::make_generators( gum::learning::RowGeneratorIdentity() );

      auto filter =
          gum::learning::make_DB_row_filter( database, translators, generators );

      std::vector<gum::Size> modalities( 8, 2 );

      gum::learning::RecordCounter<> counter( filter, modalities );

      std::vector<gum::Idx> set1{0};
      std::vector<gum::Idx> set2{1};
      std::vector<gum::Idx> set3{1, 0};

      {
        gum::Idx id1 = counter.addNodeSet( set1 );
        gum::Idx id2 = counter.addNodeSet( set2 );
        gum::Idx id3 = counter.addNodeSet( set3 );

        counter.count();

        const std::vector<double>& vect1 = counter.getCounts( id1 );
        const std::vector<double>& vect2 = counter.getCounts( id2 );
        const std::vector<double>& vect3 = counter.getCounts( id3 );

        TS_ASSERT( vect1[0] == 5028 );
        TS_ASSERT( vect1[1] == 4972 );
        TS_ASSERT( vect2[0] == 538 );
        TS_ASSERT( vect2[1] == 9462 );

        TS_ASSERT( vect3[0] == 498 );
        TS_ASSERT( vect3[1] == 4530 );
        TS_ASSERT( vect3[2] == 40 );
        TS_ASSERT( vect3[3] == 4932 );
      }

      counter.clearNodeSets();
      std::vector<gum::Idx> set4{1, 2, 3};
      {
        counter.addNodeSet( set2 );
        gum::Idx id2 = counter.addNodeSet( set4 );
        counter.addNodeSet( set1 );
        gum::Idx id4 = counter.addNodeSet( set4 );

        counter.count();

        TS_ASSERT( counter.getCounts( id2 ) == counter.getCounts( id4 ) );
      }

      counter.clearNodeSets();
      gum::Idx id = counter.addNodeSet( set4 );
      counter.count();
      std::vector<double> vect = counter.getCounts( id );

      counter.clearNodeSets();
      {
        std::vector<gum::Idx> set5{2, 1, 3};
        std::vector<gum::Idx> set6{3, 1, 2};
        std::vector<gum::Idx> set7{3, 2, 1};
        std::vector<gum::Idx> set8{1, 3, 2};
        gum::Idx              id1 = counter.addNodeSet( set4 );
        gum::Idx              id2 = counter.addNodeSet( set5 );
        gum::Idx              id3 = counter.addNodeSet( set6 );
        gum::Idx              id4 = counter.addNodeSet( set7 );
        gum::Idx              id5 = counter.addNodeSet( set8 );

        counter.count();

        TS_ASSERT( compare_vect3( vect, counter.getCounts( id1 ), {1, 2, 3} ) );
        TS_ASSERT( compare_vect3( vect, counter.getCounts( id2 ), {2, 1, 3} ) );
        TS_ASSERT( compare_vect3( vect, counter.getCounts( id3 ), {3, 1, 2} ) );
        TS_ASSERT( compare_vect3( vect, counter.getCounts( id4 ), {3, 2, 1} ) );
        TS_ASSERT( compare_vect3( vect, counter.getCounts( id5 ), {1, 3, 2} ) );
      }

      counter.clearNodeSets();
      std::vector<gum::Idx> setx13{1, 3};
      std::vector<gum::Idx> setx23{2, 3};
      std::vector<gum::Idx> setx2{2};
      std::vector<gum::Idx> setx3{3};
      gum::Idx              Idx13 = counter.addNodeSet( setx13 );
      gum::Idx              Idx23 = counter.addNodeSet( setx23 );
      gum::Idx              Idx2 = counter.addNodeSet( setx2 );
      gum::Idx              Idx3 = counter.addNodeSet( setx3 );
      counter.count();
      std::vector<double> vectx13 = counter.getCounts( Idx13 );
      std::vector<double> vectx23 = counter.getCounts( Idx23 );
      std::vector<double> vectx2 = counter.getCounts( Idx2 );
      std::vector<double> vectx3 = counter.getCounts( Idx3 );

      counter.clearNodeSets();
      {
        std::vector<gum::Idx> set5{1, 3};
        std::vector<gum::Idx> set6{3, 1};
        std::vector<gum::Idx> set7{3, 2};
        std::vector<gum::Idx> set8{2, 3};
        std::vector<gum::Idx> set9{3};
        std::vector<gum::Idx> set10{2};
        gum::Idx              id1 = counter.addNodeSet( set4 );
        gum::Idx              id2 = counter.addNodeSet( set5 );
        gum::Idx              id3 = counter.addNodeSet( set6 );
        gum::Idx              id4 = counter.addNodeSet( set7 );
        gum::Idx              id5 = counter.addNodeSet( set8 );
        gum::Idx              id6 = counter.addNodeSet( set9 );
        gum::Idx              id7 = counter.addNodeSet( set10 );

        counter.count();

        TS_ASSERT( compare_vect3( vect, counter.getCounts( id1 ), {1, 2, 3} ) );
        TS_ASSERT( compare_vect2( vectx13, counter.getCounts( id2 ), {1, 2} ) );
        TS_ASSERT( compare_vect2( vectx13, counter.getCounts( id3 ), {2, 1} ) );
        TS_ASSERT( compare_vect2( vectx23, counter.getCounts( id4 ), {2, 1} ) );
        TS_ASSERT( compare_vect2( vectx23, counter.getCounts( id5 ), {1, 2} ) );
        TS_ASSERT( vectx3 == counter.getCounts( id6 ) );
        TS_ASSERT( vectx2 == counter.getCounts( id7 ) );
      }
    }

    void test_copy_move() {
      gum::learning::DatabaseFromCSV database( GET_RESSOURCES_PATH( "asia.csv" ) );

      gum::learning::DBRowTranslatorSet<gum::learning::CellTranslatorCompactIntId> translators;
      translators.insertTranslator ( 0, 8 );

      auto generators =
          gum::learning::make_generators( gum::learning::RowGeneratorIdentity() );

      auto filter =
          gum::learning::make_DB_row_filter( database, translators, generators );

      std::vector<gum::Idx> modalities( 8, 2 );

      gum::learning::RecordCounter<> counter( filter, modalities );

      std::vector<gum::Idx> set1{0};
      std::vector<gum::Idx> set2{1};
      std::vector<gum::Idx> set3{1, 0};

      {
        gum::Idx id1 = counter.addNodeSet( set1 );
        gum::Idx id2 = counter.addNodeSet( set2 );
        gum::Idx id3 = counter.addNodeSet( set3 );

        counter.count();

        gum::learning::RecordCounter<> counter2( counter );

        const std::vector<double>& vect1 = counter2.getCounts( id1 );
        const std::vector<double>& vect2 = counter2.getCounts( id2 );
        const std::vector<double>& vect3 = counter2.getCounts( id3 );

        TS_ASSERT( vect1[0] == 5028 );
        TS_ASSERT( vect1[1] == 4972 );
        TS_ASSERT( vect2[0] == 538 );
        TS_ASSERT( vect2[1] == 9462 );

        TS_ASSERT( vect3[0] == 498 );
        TS_ASSERT( vect3[1] == 4530 );
        TS_ASSERT( vect3[2] == 40 );
        TS_ASSERT( vect3[3] == 4932 );

        gum::learning::RecordCounter<> counter3( std::move( counter2 ) );

        const std::vector<double>& vvect1 = counter3.getCounts( id1 );
        const std::vector<double>& vvect2 = counter3.getCounts( id2 );
        const std::vector<double>& vvect3 = counter3.getCounts( id3 );

        TS_ASSERT( vvect1[0] == 5028 );
        TS_ASSERT( vvect1[1] == 4972 );
        TS_ASSERT( vvect2[0] == 538 );
        TS_ASSERT( vvect2[1] == 9462 );

        TS_ASSERT( vvect3[0] == 498 );
        TS_ASSERT( vvect3[1] == 4530 );
        TS_ASSERT( vvect3[2] == 40 );
        TS_ASSERT( vvect3[3] == 4932 );
      }

      counter.clearNodeSets();
      std::vector<gum::Idx> set4{1, 2, 3};
      {
        counter.addNodeSet( set2 );
        gum::Idx id2 = counter.addNodeSet( set4 );
        counter.addNodeSet( set1 );
        gum::Idx id4 = counter.addNodeSet( set4 );

        counter.count();

        TS_ASSERT( counter.getCounts( id2 ) == counter.getCounts( id4 ) );
      }

      counter.clearNodeSets();
      gum::Idx id = counter.addNodeSet( set4 );
      counter.count();
      std::vector<double> vect = counter.getCounts( id );

      counter.clearNodeSets();
      {
        std::vector<gum::Idx> set5{2, 1, 3};
        std::vector<gum::Idx> set6{3, 1, 2};
        std::vector<gum::Idx> set7{3, 2, 1};
        std::vector<gum::Idx> set8{1, 3, 2};
        gum::Idx              id1 = counter.addNodeSet( set4 );
        gum::Idx              id2 = counter.addNodeSet( set5 );
        gum::Idx              id3 = counter.addNodeSet( set6 );
        gum::Idx              id4 = counter.addNodeSet( set7 );
        gum::Idx              id5 = counter.addNodeSet( set8 );

        gum::learning::RecordCounter<> counter2( counter );
        gum::learning::RecordCounter<> counter2bis( counter );
        gum::learning::RecordCounter<> counter3( std::move( counter2bis ) );

        counter2.count();
        counter3.count();

        TS_ASSERT( compare_vect3( vect, counter2.getCounts( id1 ), {1, 2, 3} ) );
        TS_ASSERT( compare_vect3( vect, counter2.getCounts( id2 ), {2, 1, 3} ) );
        TS_ASSERT( compare_vect3( vect, counter2.getCounts( id3 ), {3, 1, 2} ) );
        TS_ASSERT( compare_vect3( vect, counter2.getCounts( id4 ), {3, 2, 1} ) );
        TS_ASSERT( compare_vect3( vect, counter2.getCounts( id5 ), {1, 3, 2} ) );

        TS_ASSERT( compare_vect3( vect, counter3.getCounts( id1 ), {1, 2, 3} ) );
        TS_ASSERT( compare_vect3( vect, counter3.getCounts( id2 ), {2, 1, 3} ) );
        TS_ASSERT( compare_vect3( vect, counter3.getCounts( id3 ), {3, 1, 2} ) );
        TS_ASSERT( compare_vect3( vect, counter3.getCounts( id4 ), {3, 2, 1} ) );
        TS_ASSERT( compare_vect3( vect, counter3.getCounts( id5 ), {1, 3, 2} ) );
      }
    }

    bool compare_vect3( const std::vector<double>&   v1,
                        const std::vector<double>&   v2,
                        const std::vector<gum::Idx>& order ) {
      gum::Idx index1, index2;
      for ( gum::Idx k = 0; k < 2; ++k ) {
        for ( gum::Idx j = 0; j < 2; ++j ) {
          for ( gum::Idx i = 0; i < 2; ++i ) {
            index2 = i + j * 2 + k * 4;
            index1 = i * ( gum::Idx( 1 ) << ( order[0] - 1 ) ) +
                     j * ( gum::Idx( 1 ) << ( order[1] - 1 ) ) +
                     k * ( gum::Idx( 1 ) << ( order[2] - 1 ) );
            if ( v1[index1] != v2[index2] ) return false;
          }
        }
      }

      return true;
    }

    bool compare_vect2( const std::vector<double>&   v1,
                        const std::vector<double>&   v2,
                        const std::vector<gum::Idx>& order ) {
      gum::Idx index1, index2;
      for ( gum::Idx j = 0; j < 2; ++j ) {
        for ( gum::Idx i = 0; i < 2; ++i ) {
          index2 = i + j * 2;
          index1 = i * ( gum::Idx( 1 ) << ( order[0] - 1 ) ) +
                   j * ( gum::Idx( 1 ) << ( order[1] - 1 ) );
          if ( v1[index1] != v2[index2] ) return false;
        }
      }

      return true;
    }
  };

} /* namespace gum_tests */
