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

#include <agrum/graphs/DAG.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>
#include <agrum/learning/database/DBCellTranslator.h>
#include <agrum/learning/database/DBRowTranslatorSetStatic.h>
#include <agrum/learning/database/DBRowTranslatorSetDynamic.h>
#include <agrum/learning/database/CSVParser.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/scores_and_tests/scoreK2.h>

namespace gum_tests {


  class DBRowTestSuite: public CxxTest::TestSuite {
  public:  

    void test_asia1 () {
      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      
      auto translators1 = gum::learning::make_translators
        ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId,
                                gum::learning::Col<0>, 8 > () );

      auto generators1 =
        gum::learning::make_generators ( gum::learning::RowGeneratorIdentity () );

      auto filter1 = gum::learning::make_DB_row_filter ( database, translators1,
                                                         generators1 );
      
      gum::learning::DBRowTranslatorSetDynamic
        <gum::learning::CellTranslatorCompactIntId> translators2;
      translators2.insertTranslator ( gum::learning::Col<0> (), 8 );

      auto generators2 =
        gum::learning::make_generators ( gum::learning::RowGeneratorIdentity () );

      auto filter2 = gum::learning::make_DB_row_filter ( database, translators2,
                                                         generators2 );

      unsigned nb1 = 0;
      while ( filter1.hasRows () ) {
        ++nb1;
        filter1.row ();
      }

      
      unsigned nb2 = 0;
      while ( filter2.hasRows () ) {
        ++nb2;
        filter2.row ();
      }

      TS_ASSERT ( nb1 == nb2 );
      TS_ASSERT ( nb1 == 10000 );

      filter1.reset ();
      filter2.reset ();

      nb1 = 0;
      while ( filter1.hasRows () && filter2.hasRows () ) {
        gum::learning::FilteredRow& row1 = filter1.row ();
        gum::learning::FilteredRow& row2 = filter2.row ();
        TS_ASSERT ( row1 == row2 );
        ++nb1;
      }
      
    }

    
    void test_asia2 () {
      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "asia.csv" ) );
      
      auto translators1 = gum::learning::make_translators
        ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId,
                                gum::learning::Col<0>, 8 > () );

      auto generators1 =
        gum::learning::make_generators ( gum::learning::RowGeneratorIdentity () );

      auto filter1 = gum::learning::make_DB_row_filter ( database, translators1,
                                                         generators1 );
      
      gum::learning::DBRowTranslatorSetDynamic< gum::learning::DBCellTranslator<1,1> >
        translators2;
      translators2.insertTranslator ( gum::learning::CellTranslatorCompactIntId (),
                                      gum::learning::Col<0> (), 8 );

      auto generators2 =
        gum::learning::make_generators ( gum::learning::RowGeneratorIdentity () );

      auto filter2 = gum::learning::make_DB_row_filter ( database, translators2,
                                                         generators2 );

      

      unsigned nb1 = 0;
      while ( filter1.hasRows () ) {
        ++nb1;
        filter1.row ();
      }

      
      unsigned nb2 = 0;
      while ( filter2.hasRows () ) {
        ++nb2;
        filter2.row ();
      }

      TS_ASSERT ( nb1 == nb2 );
      TS_ASSERT ( nb1 == 10000 );

      filter1.reset ();
      filter2.reset ();

      std::ifstream in ( GET_PATH_STR( "asia.csv" ), std::ifstream::in);
      gum::learning::CSVParser parser ( in );
      parser.next (); // skip the names
      
      nb1 = 0;
      while ( filter1.hasRows () && filter2.hasRows () ) {
        gum::learning::FilteredRow& row1 = filter1.row ();
        gum::learning::FilteredRow& row2 = filter2.row ();
        TS_ASSERT ( row1 == row2 );
        ++nb1;

        std::vector<unsigned int>& row = row1.row ();
        parser.next ();
        const std::vector<std::string>& data = parser.current();

        TS_ASSERT ( row.size () == data.size () );
        for ( unsigned int i = 0; i < data.size (); ++i ) {
          TS_ASSERT ( (int) row[i] == std::stoi ( data[i] ) );
        }
      }
      
    }

    
    void test_alarm () {
      gum::learning::DatabaseFromCSV database ( GET_PATH_STR( "alarm.csv" ) );
      
      auto translators1 = gum::learning::make_translators
        ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId,
                                gum::learning::Col<0>, 37 > () );

      auto generators1 =
        gum::learning::make_generators ( gum::learning::RowGeneratorIdentity () );

      auto filter1 = gum::learning::make_DB_row_filter ( database, translators1,
                                                         generators1 );
      
      gum::learning::DBRowTranslatorSetDynamic< gum::learning::DBCellTranslator<1,1> >
        translators2;
      translators2.insertTranslator ( gum::learning::CellTranslatorCompactIntId (),
                                      gum::learning::Col<0> (), 37 );

      auto generators2 =
        gum::learning::make_generators ( gum::learning::RowGeneratorIdentity () );

      auto filter2 = gum::learning::make_DB_row_filter ( database, translators2,
                                                         generators2 );

      

      unsigned nb1 = 0;
      while ( filter1.hasRows () ) {
        ++nb1;
        filter1.row ();
      }

      
      unsigned nb2 = 0;
      while ( filter2.hasRows () ) {
        ++nb2;
        filter2.row ();
      }

      TS_ASSERT ( nb1 == nb2 );
      TS_ASSERT ( nb1 == 10000 );

      filter1.reset ();
      filter2.reset ();

      std::ifstream in ( GET_PATH_STR( "alarm.csv" ), std::ifstream::in);
      gum::learning::CSVParser parser ( in );
      parser.next (); // skip the names
      
      nb1 = 0;
      while ( filter1.hasRows () && filter2.hasRows () ) {
        gum::learning::FilteredRow& row1 = filter1.row ();
        gum::learning::FilteredRow& row2 = filter2.row ();
        TS_ASSERT ( row1 == row2 );
        ++nb1;

        std::vector<unsigned int>& row = row1.row ();
        parser.next ();
        const std::vector<std::string>& data = parser.current();

        TS_ASSERT ( row.size () == data.size () );
        for ( unsigned int i = 0; i < data.size (); ++i ) {
          TS_ASSERT ( (int) row[i] == std::stoi ( data[i] ) );
        }
      }
      
    }
  };


} /* namespace gum_tests */
