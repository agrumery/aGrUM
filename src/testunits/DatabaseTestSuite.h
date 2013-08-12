/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
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
#include "testsuite_utils.h"

#include <agrum/learning/database.h>

namespace gum_tests {

  class DatabaseTestSuite: public CxxTest::TestSuite {
    public:
      void testCreationCSV() {
        std::string db1_name( GET_PATH_STR( database1.csv ) );
        TS_GUM_ASSERT_THROWS_NOTHING( gum::Database d =
                                        gum::Database::createFromCSV( db1_name, ',' ) );

        gum::Database d = gum::Database::createFromCSV( db1_name, ',' );
        TS_ASSERT_EQUALS( d.nbrNodes(), ( gum::Idx ) 6 );
        TS_ASSERT_EQUALS( d.nbrLines(), ( gum::Idx ) 9 );

        TS_ASSERT_THROWS( d.nbrModalities( 6 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.nodeName( 6 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.modalitiesNames( 6 ) , gum::NotFound );

        TS_ASSERT_EQUALS( d.nbrModalities( 0 ), ( gum::Idx ) 3 );   // 1,0 and "toto"
        TS_ASSERT_THROWS( d.modalityName( 1, 6 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.modalityName( 4, 0 ) , gum::NotFound );
        TS_GUM_ASSERT_THROWS_NOTHING( d.modalityName( 1, 0 ) );

        TS_ASSERT_THROWS( d.value( 1, 6 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.value( 10, 3 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.value( 10, 6 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.line( 10 ) , gum::NotFound );
      }

      void testParseCSV() {
        std::string db1_name( GET_PATH_STR( database1.csv ) );
        gum::Database d = gum::Database::createFromCSV( db1_name, ',' );
        TS_ASSERT_EQUALS( d.nbrNodes(), ( gum::Idx ) 6 );

        TS_ASSERT_EQUALS( d.nbrModalities( 0 ), ( gum::Idx ) 3 );   // 1,0 and "toto"

        for ( unsigned int i = 1; i < d.nbrNodes(); i++ ) {
          TS_ASSERT_EQUALS( d.nbrModalities( i ), ( gum::Idx ) 2 );   // 1,0
        }

        TS_ASSERT( ! d.hasMissingValue() );

        for ( unsigned int i = 1; i < d.nbrNodes(); i++ ) {
          TS_ASSERT( ! d.hasMissingValue( i ) );
        }
      }

      void testParseMissingCSV() {
        std::string db1_name( GET_PATH_STR( database_missingvalue.csv ) );
        gum::Database d = gum::Database::createFromCSV( db1_name, ',' );
        TS_ASSERT_EQUALS( d.nbrNodes(), ( gum::Idx ) 6 );

        TS_ASSERT_EQUALS( d.nbrModalities( 0 ), ( gum::Idx ) 4 );   // 1,0,"toto" and "?"
        TS_ASSERT_EQUALS( d.nbrModalities( 1 ), ( gum::Idx ) 2 );   // 1,0,"toto" and "?"
        TS_ASSERT_EQUALS( d.nbrModalities( 2 ), ( gum::Idx ) 2 );   // 1,0,"toto" and "?"
        TS_ASSERT_EQUALS( d.nbrModalities( 3 ), ( gum::Idx ) 3 );   // 1,0,"toto" and "?"
        TS_ASSERT_EQUALS( d.nbrModalities( 4 ), ( gum::Idx ) 2 );   // 1,0,"toto" and "?"
        TS_ASSERT_EQUALS( d.nbrModalities( 5 ), ( gum::Idx ) 3 );   // 1,0,"toto" and "?"


        TS_ASSERT( d.hasMissingValue() );

        TS_ASSERT( d.hasMissingValue( 0 ) );
        TS_ASSERT( ! d.hasMissingValue( 1 ) );
        TS_ASSERT( ! d.hasMissingValue( 2 ) );
        TS_ASSERT( d.hasMissingValue( 3 ) );
        TS_ASSERT( ! d.hasMissingValue( 4 ) );
        TS_ASSERT( d.hasMissingValue( 5 ) );
      }

      void testParseXML() {
        std::string db1_name( GET_PATH_STR( database1.xml ) );
        gum::Database d = gum::Database::createFromXmlCSV( db1_name );
        TS_ASSERT_EQUALS( d.nbrNodes(), ( gum::Idx ) 3 );
        TS_ASSERT_EQUALS( d.nbrLines(), ( gum::Idx ) 2 );

        for ( unsigned int i = 0; i < d.nbrNodes(); i++ ) {
          TS_ASSERT_EQUALS( d.nbrModalities( i ), ( gum::Idx ) 2 );   // 1,0
        }
      }

      void testParseMissingXML() {
        std::string db1_name( GET_PATH_STR( database_missingvalue.xml ) );
        TS_ASSERT_THROWS( gum::Database d =
                            gum::Database::createFromXmlCSV( db1_name ),
                          gum::Exception );
        TS_ASSERT_THROWS( gum::Database d =
                            gum::Database::createFromXmlCSV( db1_name ),
                          gum::Exception );
      }

      void testDatabase_with_newline() {
        std::string db1_name( GET_PATH_STR( db_with_newline.csv ) );
        TS_GUM_ASSERT_THROWS_NOTHING( gum::Database d = gum::Database::createFromCSV( db1_name, ',' ) );

        gum::Database d = gum::Database::createFromCSV( db1_name, ',' );
        TS_ASSERT_EQUALS( d.nbrNodes(), ( gum::Idx ) 2 );
        TS_ASSERT_EQUALS( d.nbrLines(), ( gum::Idx ) 8 );

        TS_ASSERT_THROWS( d.nbrModalities( 2 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.nodeName( 2 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.modalitiesNames( 2 ) , gum::NotFound );

        TS_ASSERT_EQUALS( d.nbrModalities( 0 ), ( gum::Idx ) 3 );   // 1,0 and "toto"
        TS_ASSERT_THROWS( d.modalityName( 1, 6 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.modalityName( 4, 0 ) , gum::NotFound );
        TS_GUM_ASSERT_THROWS_NOTHING( d.modalityName( 1, 0 ) );

        TS_ASSERT_THROWS( d.value( 1, 6 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.value( 10, 3 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.value( 10, 6 ) , gum::NotFound );
        TS_ASSERT_THROWS( d.line( 10 ) , gum::NotFound );
      }
  };

}
