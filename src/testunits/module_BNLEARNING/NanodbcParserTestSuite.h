/***************************************************************************
 *   Copyright (C) 2005-2020 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)   *
 *   info_at_agrum_dot_org                                               *
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
#include <ressources/include/myalloc.h>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/database/nanodbcParser.h>


namespace gum_tests {

  class NanodbcParserTestSuite : public CxxTest::TestSuite {
    public:
    void testSimpleSQL() {

#ifdef _ODBC

      try {
        const std::string dataSource = "PostgreSQL";
        const std::string login = "gonzales";
        const std::string password = "agrum";
        const std::string query = "SELECT * FROM testunit.asia";

        // the database contains the following rows:
        // smoking lung_cancer bronchitis visit_to_Asia tuberculosis
        // tuberculos_or_cancer dyspnoea positive_XraY
        //
        // Their types are:
        // boolean  boolean  boolean  boolean  boolean  character varying
        // text numeric
        //
        // Their content:
        // FALSE FALSE FALSE TRUE TRUE "false" "toto titi" -1.2
        // TRUE  TRUE  FALSE TRUE TRUE "true"  "toto titi"  2.45
        // TRUE  TRUE  TRUE  TRUE TRUE "true"  "toto titi"  4

        const std::string current_locale = std::setlocale(LC_NUMERIC, NULL);
        nanodbc::connection connection(dataSource, login, password, 0);
        std::setlocale(LC_NUMERIC, current_locale.c_str());

        gum::learning::NanodbcParser<> parser(connection, query);
        TS_ASSERT_EQUALS(parser.nbColumns(), std::size_t(8));

        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(0));
        const auto& row1 = parser.current();
        TS_ASSERT_EQUALS(row1[0], "0");
        TS_ASSERT_EQUALS(row1[1], "0");
        TS_ASSERT_EQUALS(row1[2], "0");
        TS_ASSERT_EQUALS(row1[3], "1");
        TS_ASSERT_EQUALS(row1[4], "1");
        TS_ASSERT_EQUALS(row1[5], "false");
        TS_ASSERT_EQUALS(row1[6], "toto titi");
        TS_ASSERT_EQUALS(row1[7], "-1.2");

        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(1));
        const auto& row2 = parser.current();
        TS_ASSERT_EQUALS(row2[0], "1");
        TS_ASSERT_EQUALS(row2[1], "1");
        TS_ASSERT_EQUALS(row2[2], "0");
        TS_ASSERT_EQUALS(row2[3], "1");
        TS_ASSERT_EQUALS(row2[4], "1");
        TS_ASSERT_EQUALS(row2[5], "true");
        TS_ASSERT_EQUALS(row2[6], "toto titi");
        TS_ASSERT_EQUALS(row2[7], "2.45");

        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(2));
        const auto& row3 = parser.current();
        TS_ASSERT_EQUALS(row3[0], "1");
        TS_ASSERT_EQUALS(row3[1], "1");
        TS_ASSERT_EQUALS(row3[2], "1");
        TS_ASSERT_EQUALS(row3[3], "1");
        TS_ASSERT_EQUALS(row3[4], "1");
        TS_ASSERT_EQUALS(row3[5], "true");
        TS_ASSERT_EQUALS(row3[6], "toto titi");
        TS_ASSERT_EQUALS(row3[7], "4");

        TS_ASSERT_EQUALS(parser.columnName(std::size_t(0)), "smoking");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(1)), "lung_cancer");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(2)), "bronchitis");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(3)), "visit_to_Asia");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(4)), "tuberculosis");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(5)),
                         "tuberculos_or_cancer");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(6)), "dyspnoea");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(7)), "positive_XraY");

        TS_ASSERT_EQUALS(parser.next(), false);

        parser.useNewQuery(connection, query);
        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(0));
        const auto& xrow1 = parser.current();
        TS_ASSERT_EQUALS(xrow1[0], "0");
        TS_ASSERT_EQUALS(xrow1[1], "0");
        TS_ASSERT_EQUALS(xrow1[2], "0");
        TS_ASSERT_EQUALS(xrow1[3], "1");
        TS_ASSERT_EQUALS(xrow1[4], "1");
        TS_ASSERT_EQUALS(xrow1[5], "false");
        TS_ASSERT_EQUALS(xrow1[6], "toto titi");
        TS_ASSERT_EQUALS(xrow1[7], "-1.2");

        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(1));
        const auto& xrow2 = parser.current();
        TS_ASSERT_EQUALS(xrow2[0], "1");
        TS_ASSERT_EQUALS(xrow2[1], "1");
        TS_ASSERT_EQUALS(xrow2[2], "0");
        TS_ASSERT_EQUALS(xrow2[3], "1");
        TS_ASSERT_EQUALS(xrow2[4], "1");
        TS_ASSERT_EQUALS(xrow2[5], "true");
        TS_ASSERT_EQUALS(xrow2[6], "toto titi");
        TS_ASSERT_EQUALS(xrow2[7], "2.45");

        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(2));
        const auto& xrow3 = parser.current();
        TS_ASSERT_EQUALS(xrow3[0], "1");
        TS_ASSERT_EQUALS(xrow3[1], "1");
        TS_ASSERT_EQUALS(xrow3[2], "1");
        TS_ASSERT_EQUALS(xrow3[3], "1");
        TS_ASSERT_EQUALS(xrow3[4], "1");
        TS_ASSERT_EQUALS(xrow3[5], "true");
        TS_ASSERT_EQUALS(xrow3[6], "toto titi");
        TS_ASSERT_EQUALS(xrow3[7], "4");

        TS_ASSERT_EQUALS(parser.columnName(std::size_t(0)), "smoking");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(1)), "lung_cancer");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(2)), "bronchitis");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(3)), "visit_to_Asia");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(4)), "tuberculosis");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(5)),
                         "tuberculos_or_cancer");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(6)), "dyspnoea");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(7)), "positive_XraY");

        TS_ASSERT_EQUALS(parser.next(), false);

      } catch (nanodbc::database_error& e) {
        // no database. see e.what()
        std::cout << "[PostgreSQL error]";
      }
#endif  // _ODBC
    }

    
    void testAlarmSQL() {
#ifdef _ODBC
      const std::string driver_name = "SQLite3";
#ifdef _WIN32
      driver_name += " ODBC Driver";
#endif // _WIN32

      try {
        const std::string connection_string =
          "Driver=" + driver_name + ";Database=" +
          GET_RESSOURCES_PATH("sqlite/alarm.sqlite") + ";";

        const std::string current_locale = std::setlocale(LC_NUMERIC, NULL);
        nanodbc::connection connection(connection_string, 0);
        std::setlocale(LC_NUMERIC, current_locale.c_str());

      } catch (nanodbc::database_error& e) {
        // no database. see e.what()
        std::cout << "[SQLite error]" << e.what();
      }
      
#endif  // _ODBC
    }


    void testAsiaSQL() {
#ifdef _ODBC
      const std::string driver_name = "SQLite3";
#ifdef _WIN32
      driver_name += " ODBC Driver";
#endif // _WIN32

      try {
        const std::string connection_string =
          "Driver=" + driver_name + ";Database=" +
          GET_RESSOURCES_PATH("sqlite/asia.sqlite") + ";";

        // the database contains the following rows:
        // smoking lung_cancer bronchitis visit_to_Asia tuberculosis
        // tuberculos_or_cancer dyspnoea positive_XraY
        //
        // Their types are:
        // boolean  boolean  boolean  boolean  boolean  character varying
        // text numeric
        //
        // Their content:
        // FALSE FALSE FALSE TRUE TRUE "false" "toto titi" -1.2
        // TRUE  TRUE  FALSE TRUE TRUE "true"  "toto titi"  2.45
        // TRUE  TRUE  TRUE  TRUE TRUE "true"  "toto titi"  4

        const std::string current_locale = std::setlocale(LC_NUMERIC, NULL);
        nanodbc::connection connection(connection_string, 0);
        TS_ASSERT(connection.connected());
        std::setlocale(LC_NUMERIC, current_locale.c_str());

        const std::string query_count = "select count(*) from asia;";

        auto row_count = nanodbc::execute(connection, query_count);
        row_count.next();
        TS_ASSERT_EQUALS(row_count.get< int >(0), 3);

        const std::string query = "select * from asia;";
  
        auto row = nanodbc::execute(connection, query);
        row.next();
        TS_ASSERT_EQUALS(row.column_name(0), "smoking");
        TS_ASSERT_EQUALS(row.columns(), 8);
        
        TS_ASSERT_EQUALS(row.get< int >(0, 5), 1);
        TS_ASSERT_EQUALS(row.get< std::string >(6), "toto titi");
        TS_ASSERT_EQUALS(row.get< double >(7), -1.2);
        
        gum::learning::NanodbcParser<> parser(connection, query);
        TS_ASSERT_EQUALS(parser.nbColumns(), std::size_t(8));

        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(0));
        const auto& row1 = parser.current();
        TS_ASSERT_EQUALS(row1[0], "1");
        TS_ASSERT_EQUALS(row1[1], "0");
        TS_ASSERT_EQUALS(row1[2], "0");
        TS_ASSERT_EQUALS(row1[3], "1");
        TS_ASSERT_EQUALS(row1[4], "1");
        TS_ASSERT_EQUALS(row1[5], "0");
        TS_ASSERT_EQUALS(row1[6], "toto titi");
        TS_ASSERT_EQUALS(row1[7], "-1.2");

        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(1));
        const auto& row2 = parser.current();
        TS_ASSERT_EQUALS(row2[0], "1");
        TS_ASSERT_EQUALS(row2[1], "1");
        TS_ASSERT_EQUALS(row2[2], "0");
        TS_ASSERT_EQUALS(row2[3], "1");
        TS_ASSERT_EQUALS(row2[4], "1");
        TS_ASSERT_EQUALS(row2[5], "1");
        TS_ASSERT_EQUALS(row2[6], "toto titi");
        TS_ASSERT_EQUALS(row2[7], "2.45");

        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(2));
        const auto& row3 = parser.current();
        TS_ASSERT_EQUALS(row3[0], "1");
        TS_ASSERT_EQUALS(row3[1], "1");
        TS_ASSERT_EQUALS(row3[2], "1");
        TS_ASSERT_EQUALS(row3[3], "1");
        TS_ASSERT_EQUALS(row3[4], "1");
        TS_ASSERT_EQUALS(row3[5], "1");
        TS_ASSERT_EQUALS(row3[6], "toto titi");
        TS_ASSERT_EQUALS(row3[7], "4");

        TS_ASSERT_EQUALS(parser.columnName(std::size_t(0)), "smoking");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(1)), "lung_cancer");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(2)), "bronchitis");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(3)), "visit_to_Asia");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(4)), "tuberculosis");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(5)), "tuberculos_or_cancer");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(6)), "dyspnoea");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(7)), "positive_XraY");

        TS_ASSERT_EQUALS(parser.next(), false);

        parser.useNewQuery(connection, query);
        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(0));
        const auto& xrow1 = parser.current();
        TS_ASSERT_EQUALS(xrow1[0], "1");
        TS_ASSERT_EQUALS(xrow1[1], "0");
        TS_ASSERT_EQUALS(xrow1[2], "0");
        TS_ASSERT_EQUALS(xrow1[3], "1");
        TS_ASSERT_EQUALS(xrow1[4], "1");
        TS_ASSERT_EQUALS(xrow1[5], "0");
        TS_ASSERT_EQUALS(xrow1[6], "toto titi");
        TS_ASSERT_EQUALS(xrow1[7], "-1.2");

        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(1));
        const auto& xrow2 = parser.current();
        TS_ASSERT_EQUALS(xrow2[0], "1");
        TS_ASSERT_EQUALS(xrow2[1], "1");
        TS_ASSERT_EQUALS(xrow2[2], "0");
        TS_ASSERT_EQUALS(xrow2[3], "1");
        TS_ASSERT_EQUALS(xrow2[4], "1");
        TS_ASSERT_EQUALS(xrow2[5], "1");
        TS_ASSERT_EQUALS(xrow2[6], "toto titi");
        TS_ASSERT_EQUALS(xrow2[7], "2.45");

        parser.next();
        TS_ASSERT_EQUALS(parser.nbLine(), std::size_t(2));
        const auto& xrow3 = parser.current();
        TS_ASSERT_EQUALS(xrow3[0], "1");
        TS_ASSERT_EQUALS(xrow3[1], "1");
        TS_ASSERT_EQUALS(xrow3[2], "1");
        TS_ASSERT_EQUALS(xrow3[3], "1");
        TS_ASSERT_EQUALS(xrow3[4], "1");
        TS_ASSERT_EQUALS(xrow3[5], "1");
        TS_ASSERT_EQUALS(xrow3[6], "toto titi");
        TS_ASSERT_EQUALS(xrow3[7], "4");

        TS_ASSERT_EQUALS(parser.columnName(std::size_t(0)), "smoking");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(1)), "lung_cancer");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(2)), "bronchitis");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(3)), "visit_to_Asia");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(4)), "tuberculosis");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(5)), "tuberculos_or_cancer");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(6)), "dyspnoea");
        TS_ASSERT_EQUALS(parser.columnName(std::size_t(7)), "positive_XraY");

        TS_ASSERT_EQUALS(parser.next(), false);
        
      } catch (nanodbc::database_error& e) {
        // no database. see e.what()
        std::cout << "[SQLite error]";
      }
#endif  // _ODBC
    }
  };

} /* namespace gum_tests */
