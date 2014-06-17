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
#include <agrum/learning/scores+tests/scoreAIC.h>

namespace gum_tests {

  class ScoreAICTestSuite: public CxxTest::TestSuite {
  public:

    class CellTranslator : public gum::learning::DBCellTranslator<1,1> {
    public:
      CellTranslator () {}

      ~CellTranslator () {}
      
      CellTranslator ( const CellTranslator& from ) :
      gum::learning::DBCellTranslator<1,1> ( from ),
      __values ( from.__values ) {}

      virtual CellTranslator* copyFactory () final {
        return new CellTranslator ( *this );
      }
      
      CellTranslator& operator= ( const CellTranslator& from )  {
        if ( this != & from ) {
          gum::learning::DBCellTranslator<1,1>::operator= ( from );
          __values = from.__values;
        }
        return *this;
      }
      
      void translate () { out (0) = in (0).getFloat (); }
      void initialize () {
        unsigned int nb = in(0).getFloat ();
        if ( ! __values.exists ( nb ) ) __values.insert ( nb );
      }
      void modalities ( std::vector<unsigned int>& modal ) const noexcept {
        modal.push_back ( __values.size () );
      }
      bool requiresInitialization () const noexcept { return true; }
      std::string translateBack ( unsigned int col,
                                  unsigned int translated_val ) {
        std::stringstream str;
        str << translated_val;
        return  str.str ();
      }

    private:
      gum::Set<unsigned int> __values;
      
    };


    class SimpleGenerator : public gum::learning::FilteredRowGenerator {
    public:
      gum::learning::FilteredRow& generate () {
        decreaseRemainingRows ();
        return *_input_row;
      }
      unsigned int _computeRows () { return 1; }
    };


    void test_aic () {
      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<CellTranslator, gum::learning::Col<0>, 8 > () );

      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );
      
      std::vector<unsigned int> modalities ( 8, 2);

      gum::learning::ScoreAIC<decltype ( filter ) >
        score ( filter, modalities );

      unsigned int id1 = score.addNodeSet ( 3 );
      unsigned int id2 = score.addNodeSet ( 1 );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 989.314  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 3024.254 ) <= 0.01 );

      score.clear ();
      id1 = score.addNodeSet ( 0 );
      id2 = score.addNodeSet ( 2 );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 10000.767 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 9930.478  ) <= 0.01 );
     
      score.clear ();
      id1 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 } );
      id2 = score.addNodeSet ( 1, std::vector<unsigned int> { 4 } );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 980.407 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 3019.57 ) <= 0.01 );

      score.clear ();
      id1 = score.addNodeSet ( 3, std::vector<unsigned int> { 1, 2 } );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 989.801  ) <= 0.01 );

      unsigned int id3, id4, id5, id6, id7;
      score.clear ();
      id1 = score.addNodeSet ( 3 );
      id2 = score.addNodeSet ( 1 );
      id3 = score.addNodeSet ( 3, std::vector<unsigned int> { 1, 2 } );
      id4 = score.addNodeSet ( 2 );
      id5 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 } );
      id6 = score.addNodeSet ( 2 );
      id7 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 } );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 989.314  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 3024.254 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id3 ) + 989.801  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id4 ) + 9930.478 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id5 ) + 980.407  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id6 ) + 9930.478 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id7 ) + 980.407  ) <= 0.01 );
    }

    
    void test_cache () {
      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<CellTranslator, gum::learning::Col<0>, 8 > () );
      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );
      std::vector<unsigned int> modalities = filter.modalities ();
      gum::learning::ScoreAIC<decltype ( filter ) >
        score ( filter, modalities );
      //score.useCache ( false );
      
      unsigned int id1, id2, id3, id4, id5, id6, id7;
      for ( unsigned int i = 0; i < 10000; ++i ) {
        score.clear ();
        id1 = score.addNodeSet ( 3 );
        id2 = score.addNodeSet ( 1 );
        id3 = score.addNodeSet ( 3, std::vector<unsigned int> { 1, 2 } );
        id4 = score.addNodeSet ( 2 );
        id5 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 } );
        id6 = score.addNodeSet ( 2 );
        id7 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 } );
        TS_ASSERT ( fabs ( score.score ( id1 ) + 989.314  ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id2 ) + 3024.254 ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id3 ) + 989.801  ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id4 ) + 9930.478 ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id5 ) + 980.407  ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id6 ) + 9930.478 ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id7 ) + 980.407  ) <= 0.01 );
      }
    }


    void test_clearcache () {
      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<CellTranslator, gum::learning::Col<0>, 8 > () );
      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      auto filter = gum::learning::make_DB_row_filter ( database, translators,
                                                        generators );
      std::vector<unsigned int> modalities = filter.modalities ();
      gum::learning::ScoreAIC<decltype ( filter ) >
        score ( filter, modalities );
      
      unsigned int id1, id2, id3, id4, id5, id6, id7;
      for ( unsigned int i = 0; i < 4; ++i ) {
        score.clearCache ();
        id1 = score.addNodeSet ( 3 );
        id2 = score.addNodeSet ( 1 );
        id3 = score.addNodeSet ( 3, std::vector<unsigned int> { 1, 2 } );
        id4 = score.addNodeSet ( 2 );
        id5 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 } );
        id6 = score.addNodeSet ( 2 );
        id7 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 } );
        TS_ASSERT ( fabs ( score.score ( id1 ) + 989.314  ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id2 ) + 3024.254 ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id3 ) + 989.801  ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id4 ) + 9930.478 ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id5 ) + 980.407  ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id6 ) + 9930.478 ) <= 0.01 );
        TS_ASSERT ( fabs ( score.score ( id7 ) + 980.407  ) <= 0.01 );
      }
    }

  };


} /* namespace gum_tests */
