/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
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
#include <string>
#include <ressources/myalloc.h>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/learning/database/nanodbcParser.h>


namespace gum_tests {

  class NanodbcParserTestSuite : public CxxTest::TestSuite {
  public:
    void testSimpleSQL() {

#ifdef _ODBC
      
      const std::string dataSource = "PostgreSQL";
      const std::string login      = "gonzales";
      const std::string password   = "agrum";
      const std::string query      = "SELECT * FROM testunit.asia";

      // the database contains the following rows:
      // smoking? lung_cancer? bronchitis? visit_to_Asia? tuberculosis?
      // tuberculos_or_cancer? dyspnoea? positive_XraY?
      //
      // Their types are:
      // boolean  boolean  boolean  boolean  boolean  character varying
      // text numeric
      //
      // Their content:
      // FALSE FALSE FALSE TRUE TRUE "false" "toto titi" -1.2
      // TRUE  TRUE  FALSE TRUE TRUE "true"  "toto titi"  2.45
      // TRUE  TRUE  TRUE  TRUE TRUE "true"  "toto titi"  4

      const std::string current_locale = std::setlocale(LC_NUMERIC, NULL );
      nanodbc::connection connection ( dataSource, login, password, 0 );
      std::setlocale(LC_NUMERIC, current_locale.c_str () );
      
      gum::learning::NanodbcParser<> parser ( connection, query );
      TS_ASSERT ( parser.nbColumns () == std::size_t(8) );
      
      parser.next();
      TS_ASSERT ( parser.nbLine() == std::size_t(1) );
      const auto& row1 = parser.current ();
      TS_ASSERT ( row1[0] == "0" );
      TS_ASSERT ( row1[1] == "0" );
      TS_ASSERT ( row1[2] == "0" );
      TS_ASSERT ( row1[3] == "1" );
      TS_ASSERT ( row1[4] == "1" );
      TS_ASSERT ( row1[5] == "false" );
      TS_ASSERT ( row1[6] == "toto titi" );
      TS_ASSERT ( row1[7] == "-1.2" );

      parser.next();
      TS_ASSERT ( parser.nbLine() == std::size_t(2) );
      const auto& row2 = parser.current ();
      TS_ASSERT ( row2[0] == "1" );
      TS_ASSERT ( row2[1] == "1" );
      TS_ASSERT ( row2[2] == "0" );
      TS_ASSERT ( row2[3] == "1" );
      TS_ASSERT ( row2[4] == "1" );
      TS_ASSERT ( row2[5] == "true" );
      TS_ASSERT ( row2[6] == "toto titi" );
      TS_ASSERT ( row2[7] == "2.45" );

      parser.next();
      TS_ASSERT ( parser.nbLine() == std::size_t(3) );
      const auto& row3 = parser.current ();
      TS_ASSERT ( row3[0] == "1" );
      TS_ASSERT ( row3[1] == "1" );
      TS_ASSERT ( row3[2] == "1" );
      TS_ASSERT ( row3[3] == "1" );
      TS_ASSERT ( row3[4] == "1" );
      TS_ASSERT ( row3[5] == "true" );
      TS_ASSERT ( row3[6] == "toto titi" );
      TS_ASSERT ( row3[7] == "4" );

      TS_ASSERT ( parser.columnName ( std::size_t(0) ) == "smoking?" );
      TS_ASSERT ( parser.columnName ( std::size_t(1) ) == "lung_cancer?" );
      TS_ASSERT ( parser.columnName ( std::size_t(2) ) == "bronchitis?" );
      TS_ASSERT ( parser.columnName ( std::size_t(3) ) == "visit_to_Asia?" );
      TS_ASSERT ( parser.columnName ( std::size_t(4) ) == "tuberculosis?" );
      TS_ASSERT ( parser.columnName ( std::size_t(5) ) == "tuberculos_or_cancer?");
      TS_ASSERT ( parser.columnName ( std::size_t(6) ) == "dyspnoea?" );
      TS_ASSERT ( parser.columnName ( std::size_t(7) ) == "positive_XraY?" );

      TS_ASSERT ( parser.next() == false );

      parser.useNewQuery ( connection, query );
      parser.next();
      TS_ASSERT ( parser.nbLine() == std::size_t(1) );
      const auto& xrow1 = parser.current ();
      TS_ASSERT ( xrow1[0] == "0" );
      TS_ASSERT ( xrow1[1] == "0" );
      TS_ASSERT ( xrow1[2] == "0" );
      TS_ASSERT ( xrow1[3] == "1" );
      TS_ASSERT ( xrow1[4] == "1" );
      TS_ASSERT ( xrow1[5] == "false" );
      TS_ASSERT ( xrow1[6] == "toto titi" );
      TS_ASSERT ( xrow1[7] == "-1.2" );

      parser.next();
      TS_ASSERT ( parser.nbLine() == std::size_t(2) );
      const auto& xrow2 = parser.current ();
      TS_ASSERT ( xrow2[0] == "1" );
      TS_ASSERT ( xrow2[1] == "1" );
      TS_ASSERT ( xrow2[2] == "0" );
      TS_ASSERT ( xrow2[3] == "1" );
      TS_ASSERT ( xrow2[4] == "1" );
      TS_ASSERT ( xrow2[5] == "true" );
      TS_ASSERT ( xrow2[6] == "toto titi" );
      TS_ASSERT ( xrow2[7] == "2.45" );

      parser.next();
      TS_ASSERT ( parser.nbLine() == std::size_t(3) );
      const auto& xrow3 = parser.current ();
      TS_ASSERT ( xrow3[0] == "1" );
      TS_ASSERT ( xrow3[1] == "1" );
      TS_ASSERT ( xrow3[2] == "1" );
      TS_ASSERT ( xrow3[3] == "1" );
      TS_ASSERT ( xrow3[4] == "1" );
      TS_ASSERT ( xrow3[5] == "true" );
      TS_ASSERT ( xrow3[6] == "toto titi" );
      TS_ASSERT ( xrow3[7] == "4" );

      TS_ASSERT ( parser.columnName ( std::size_t(0) ) == "smoking?" );
      TS_ASSERT ( parser.columnName ( std::size_t(1) ) == "lung_cancer?" );
      TS_ASSERT ( parser.columnName ( std::size_t(2) ) == "bronchitis?" );
      TS_ASSERT ( parser.columnName ( std::size_t(3) ) == "visit_to_Asia?" );
      TS_ASSERT ( parser.columnName ( std::size_t(4) ) == "tuberculosis?" );
      TS_ASSERT ( parser.columnName ( std::size_t(5) ) == "tuberculos_or_cancer?");
      TS_ASSERT ( parser.columnName ( std::size_t(6) ) == "dyspnoea?" );
      TS_ASSERT ( parser.columnName ( std::size_t(7) ) == "positive_XraY?" );

      TS_ASSERT ( parser.next() == false );

#endif // _ODBC
      
    }
    

  };

} /* namespace gum_tests */

