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

#include <agrum/learning/databaseFromCSV.h>
#include <agrum/learning/scoreK2.h>

#define MAX_LOG_CACHE 20

namespace gum_tests {

  class ScoreK2TestSuite: public CxxTest::TestSuite {
  public:

    class CellTranslator : public gum::learning::DBCellTranslator<1,1> {
    public:
      void translate () { out (0) = in (0).getFloat (); }
      void initialize () {}
      std::string translateBack ( unsigned int col,
                                  unsigned int translated_val ) {
        std::stringstream str;
        str << translated_val;
        return  str.str ();
      }
     };

    class SimpleGenerator : public gum::learning::FilteredRowGenerator {
    public:
      gum::learning::FilteredRow& generate () {
        decreaseRemainingRows ();
        return *_input_row;
      }
      unsigned int _computeRows () { return 1; }
    };

    
    std::vector<float> __log_fact;
    float __log_sqrt_2pi { 0.5f * logf ( 2 * M_PI) };

      
    float logFactorial ( unsigned int n ) {
      // if the log (n!) exists in the cache, returns it
      if ( n < MAX_LOG_CACHE ) {
        return __log_fact[n];
      }

      // returns the approximation by the stirling formula
      return ( __log_sqrt_2pi + ( n + 0.5f ) * logf ( (float) n ) -
               n + logf ( 1.0f + 1.0f / ( 12 * n ) ) );
    }
 

    void test_k2 () {
      __log_fact.resize ( MAX_LOG_CACHE, 0 );
      float nb = 0;
       for ( unsigned int i = 1; i < MAX_LOG_CACHE; ++i ) {
        nb += logf ( i );
        __log_fact[i] = nb;
      }

      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      
      auto handler = database.handler ();
      
      auto translators = gum::learning::make_translators
        ( gum::learning::Create<CellTranslator, gum::learning::Col<0>, 8 > () );

      auto generators =  gum::learning::make_generators ( SimpleGenerator () );
      
      auto filter = gum::learning::make_row_filter ( handler, translators,
                                                     generators );
      
      std::vector<unsigned int> modalities ( 8, 2);

      gum::learning::ScoreK2<decltype ( filter ) >
        score ( filter, modalities );

      unsigned int id1 = score.addNodeSet ( 3 );
      unsigned int id2 = score.addNodeSet ( 1 );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 690.914  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 2100.73 ) <= 0.01 );

      score.clear ();
      id1 = score.addNodeSet ( 0 );
      id2 = score.addNodeSet ( 2 );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 6935.7  ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 6886.97 ) <= 0.01 );

      score.clear ();
      id1 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 } );
      id2 = score.addNodeSet ( 1, std::vector<unsigned int> { 4 } );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 686.953 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 2100.61 ) <= 0.01 );

      score.clear ();
      id1 = score.addNodeSet ( 3, std::vector<unsigned int> { 1, 2 } );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 703.137  ) <= 0.01 );

      unsigned int id3, id4, id5, id6, id7;
      score.clear ();
      id1 = score.addNodeSet ( 3 );
      id2 = score.addNodeSet ( 1 );
      id3 = score.addNodeSet ( 3, std::vector<unsigned int> { 1, 2 } );
      id4 = score.addNodeSet ( 2 );
      id5 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 } );
      id6 = score.addNodeSet ( 2 );
      id7 = score.addNodeSet ( 3, std::vector<unsigned int> { 4 } );
      TS_ASSERT ( fabs ( score.score ( id1 ) + 690.914 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id2 ) + 2100.73 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id3 ) + 703.137 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id4 ) + 6886.97 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id5 ) + 686.953 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id6 ) + 6886.97 ) <= 0.01 );
      TS_ASSERT ( fabs ( score.score ( id7 ) + 686.953 ) <= 0.01 );
    }

  };


} /* namespace gum_tests */
