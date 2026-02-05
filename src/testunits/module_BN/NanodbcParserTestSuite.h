/***************************************************************************
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)   *
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
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/nanodbcParser.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  NanodbcParser
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct NanodbcParserTestSuite {
    public:
    static void testSimpleSQL() {
#ifdef ODBC_

      try {
        const std::string dataSource = "PostgreSQL";
        const std::string login      = "gonzales";
        const std::string password   = "agrum";
        const std::string query      = "SELECT * FROM testunit.asia";

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

        const std::string   current_locale = std::setlocale(LC_NUMERIC, NULL);
        nanodbc::connection connection(dataSource, login, password, 0);
        std::setlocale(LC_NUMERIC, current_locale.c_str());

        gum::learning::NanodbcParser parser(connection, query);
        CHECK((parser.nbColumns()) == (std::size_t(8)));

        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(0)));
        const auto& row1 = parser.current();
        CHECK((row1[0]) == ("0"));
        CHECK((row1[1]) == ("0"));
        CHECK((row1[2]) == ("0"));
        CHECK((row1[3]) == ("1"));
        CHECK((row1[4]) == ("1"));
        CHECK((row1[5]) == ("false"));
        CHECK((row1[6]) == ("toto titi"));
        CHECK((row1[7]) == ("-1.2"));

        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(1)));
        const auto& row2 = parser.current();
        CHECK((row2[0]) == ("1"));
        CHECK((row2[1]) == ("1"));
        CHECK((row2[2]) == ("0"));
        CHECK((row2[3]) == ("1"));
        CHECK((row2[4]) == ("1"));
        CHECK((row2[5]) == ("true"));
        CHECK((row2[6]) == ("toto titi"));
        CHECK((row2[7]) == ("2.45"));

        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(2)));
        const auto& row3 = parser.current();
        CHECK((row3[0]) == ("1"));
        CHECK((row3[1]) == ("1"));
        CHECK((row3[2]) == ("1"));
        CHECK((row3[3]) == ("1"));
        CHECK((row3[4]) == ("1"));
        CHECK((row3[5]) == ("true"));
        CHECK((row3[6]) == ("toto titi"));
        CHECK((row3[7]) == ("4"));

        CHECK((parser.columnName(std::size_t(0))) == ("smoking"));
        CHECK((parser.columnName(std::size_t(1))) == ("lung_cancer"));
        CHECK((parser.columnName(std::size_t(2))) == ("bronchitis"));
        CHECK((parser.columnName(std::size_t(3))) == ("visit_to_Asia"));
        CHECK((parser.columnName(std::size_t(4))) == ("tuberculosis"));
        CHECK((parser.columnName(std::size_t(5))) == ("tuberculos_or_cancer"));
        CHECK((parser.columnName(std::size_t(6))) == ("dyspnoea"));
        CHECK((parser.columnName(std::size_t(7))) == ("positive_XraY"));

        CHECK((parser.next()) == (false));

        parser.useNewQuery(connection, query);
        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(0)));
        const auto& xrow1 = parser.current();
        CHECK((xrow1[0]) == ("0"));
        CHECK((xrow1[1]) == ("0"));
        CHECK((xrow1[2]) == ("0"));
        CHECK((xrow1[3]) == ("1"));
        CHECK((xrow1[4]) == ("1"));
        CHECK((xrow1[5]) == ("false"));
        CHECK((xrow1[6]) == ("toto titi"));
        CHECK((xrow1[7]) == ("-1.2"));

        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(1)));
        const auto& xrow2 = parser.current();
        CHECK((xrow2[0]) == ("1"));
        CHECK((xrow2[1]) == ("1"));
        CHECK((xrow2[2]) == ("0"));
        CHECK((xrow2[3]) == ("1"));
        CHECK((xrow2[4]) == ("1"));
        CHECK((xrow2[5]) == ("true"));
        CHECK((xrow2[6]) == ("toto titi"));
        CHECK((xrow2[7]) == ("2.45"));

        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(2)));
        const auto& xrow3 = parser.current();
        CHECK((xrow3[0]) == ("1"));
        CHECK((xrow3[1]) == ("1"));
        CHECK((xrow3[2]) == ("1"));
        CHECK((xrow3[3]) == ("1"));
        CHECK((xrow3[4]) == ("1"));
        CHECK((xrow3[5]) == ("true"));
        CHECK((xrow3[6]) == ("toto titi"));
        CHECK((xrow3[7]) == ("4"));

        CHECK((parser.columnName(std::size_t(0))) == ("smoking"));
        CHECK((parser.columnName(std::size_t(1))) == ("lung_cancer"));
        CHECK((parser.columnName(std::size_t(2))) == ("bronchitis"));
        CHECK((parser.columnName(std::size_t(3))) == ("visit_to_Asia"));
        CHECK((parser.columnName(std::size_t(4))) == ("tuberculosis"));
        CHECK((parser.columnName(std::size_t(5))) == ("tuberculos_or_cancer"));
        CHECK((parser.columnName(std::size_t(6))) == ("dyspnoea"));
        CHECK((parser.columnName(std::size_t(7))) == ("positive_XraY"));

        CHECK((parser.next()) == (false));

      } catch (nanodbc::database_error& e) {
        // no database. see e.what()
        std::cout << "[PostgreSQL error]";
      }
#endif   // ODBC_
    }

    static void testAlarmSQL() {
#ifdef ODBC_
      const std::string driver_name = "SQLite3";
#  ifdef _WIN32
      driver_name += " ODBC Driver";
#  endif   // _WIN32

      try {
        const std::string connection_string = "Driver=" + driver_name + ";Database="
                                            + GET_RESSOURCES_PATH("sqlite/alarm.sqlite") + ";";

        const std::string   current_locale = std::setlocale(LC_NUMERIC, NULL);
        nanodbc::connection connection(connection_string, 0);
        std::setlocale(LC_NUMERIC, current_locale.c_str());

      } catch (nanodbc::database_error& e) {
        // no database. see e.what()
        std::cout << "[SQLite error]" << e.what();
      }

#endif   // ODBC_
    }

    static void testAsiaSQL() {
#ifdef _ODBC
      const std::string driver_name = "SQLite3";
#  ifdef _WIN32
      driver_name += " ODBC Driver";
#  endif   // _WIN32

      try {
        const std::string connection_string = "Driver=" + driver_name + ";Database="
                                            + GET_RESSOURCES_PATH("sqlite/asia.sqlite") + ";";

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

        const std::string   current_locale = std::setlocale(LC_NUMERIC, NULL);
        nanodbc::connection connection(connection_string, 0);
        CHECK(connection.connected());
        std::setlocale(LC_NUMERIC, current_locale.c_str());

        const std::string query_count = "select count(*) from asia;";

        auto row_count = nanodbc::execute(connection, query_count);
        row_count.next();
        CHECK((row_count.get< int >(0)) == (3));

        const std::string query = "select * from asia;";

        auto row = nanodbc::execute(connection, query);
        row.next();
        CHECK((row.column_name(0)) == ("smoking"));
        CHECK((row.columns()) == (8));

        CHECK((row.get< int >(0, 5)) == (1));
        CHECK((row.get< std::string >(6)) == ("toto titi"));
        CHECK((row.get< double >(7)) == (-1.2));

        gum::learning::NanodbcParser parser(connection, query);
        CHECK((parser.nbColumns()) == (std::size_t(8)));

        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(0)));
        const auto& row1 = parser.current();
        CHECK((row1[0]) == ("1"));
        CHECK((row1[1]) == ("0"));
        CHECK((row1[2]) == ("0"));
        CHECK((row1[3]) == ("1"));
        CHECK((row1[4]) == ("1"));
        CHECK((row1[5]) == ("0"));
        CHECK((row1[6]) == ("toto titi"));
        CHECK((row1[7]) == ("-1.2"));

        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(1)));
        const auto& row2 = parser.current();
        CHECK((row2[0]) == ("1"));
        CHECK((row2[1]) == ("1"));
        CHECK((row2[2]) == ("0"));
        CHECK((row2[3]) == ("1"));
        CHECK((row2[4]) == ("1"));
        CHECK((row2[5]) == ("1"));
        CHECK((row2[6]) == ("toto titi"));
        CHECK((row2[7]) == ("2.45"));

        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(2)));
        const auto& row3 = parser.current();
        CHECK((row3[0]) == ("1"));
        CHECK((row3[1]) == ("1"));
        CHECK((row3[2]) == ("1"));
        CHECK((row3[3]) == ("1"));
        CHECK((row3[4]) == ("1"));
        CHECK((row3[5]) == ("1"));
        CHECK((row3[6]) == ("toto titi"));
        CHECK((row3[7]) == ("4"));

        CHECK((parser.columnName(std::size_t(0))) == ("smoking"));
        CHECK((parser.columnName(std::size_t(1))) == ("lung_cancer"));
        CHECK((parser.columnName(std::size_t(2))) == ("bronchitis"));
        CHECK((parser.columnName(std::size_t(3))) == ("visit_to_Asia"));
        CHECK((parser.columnName(std::size_t(4))) == ("tuberculosis"));
        CHECK((parser.columnName(std::size_t(5))) == ("tuberculos_or_cancer"));
        CHECK((parser.columnName(std::size_t(6))) == ("dyspnoea"));
        CHECK((parser.columnName(std::size_t(7))) == ("positive_XraY"));

        CHECK((parser.next()) == (false));

        parser.useNewQuery(connection, query);
        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(0)));
        const auto& xrow1 = parser.current();
        CHECK((xrow1[0]) == ("1"));
        CHECK((xrow1[1]) == ("0"));
        CHECK((xrow1[2]) == ("0"));
        CHECK((xrow1[3]) == ("1"));
        CHECK((xrow1[4]) == ("1"));
        CHECK((xrow1[5]) == ("0"));
        CHECK((xrow1[6]) == ("toto titi"));
        CHECK((xrow1[7]) == ("-1.2"));

        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(1)));
        const auto& xrow2 = parser.current();
        CHECK((xrow2[0]) == ("1"));
        CHECK((xrow2[1]) == ("1"));
        CHECK((xrow2[2]) == ("0"));
        CHECK((xrow2[3]) == ("1"));
        CHECK((xrow2[4]) == ("1"));
        CHECK((xrow2[5]) == ("1"));
        CHECK((xrow2[6]) == ("toto titi"));
        CHECK((xrow2[7]) == ("2.45"));

        parser.next();
        CHECK((parser.nbLine()) == (std::size_t(2)));
        const auto& xrow3 = parser.current();
        CHECK((xrow3[0]) == ("1"));
        CHECK((xrow3[1]) == ("1"));
        CHECK((xrow3[2]) == ("1"));
        CHECK((xrow3[3]) == ("1"));
        CHECK((xrow3[4]) == ("1"));
        CHECK((xrow3[5]) == ("1"));
        CHECK((xrow3[6]) == ("toto titi"));
        CHECK((xrow3[7]) == ("4"));

        CHECK((parser.columnName(std::size_t(0))) == ("smoking"));
        CHECK((parser.columnName(std::size_t(1))) == ("lung_cancer"));
        CHECK((parser.columnName(std::size_t(2))) == ("bronchitis"));
        CHECK((parser.columnName(std::size_t(3))) == ("visit_to_Asia"));
        CHECK((parser.columnName(std::size_t(4))) == ("tuberculosis"));
        CHECK((parser.columnName(std::size_t(5))) == ("tuberculos_or_cancer"));
        CHECK((parser.columnName(std::size_t(6))) == ("dyspnoea"));
        CHECK((parser.columnName(std::size_t(7))) == ("positive_XraY"));

        CHECK((parser.next()) == (false));

      } catch (nanodbc::database_error& e) {
        // no database. see e.what()
        std::cout << "[SQLite error]";
      }
#endif   // _ODBC
    }
  };

  GUM_TEST_ACTIF(SimpleSQL)
  GUM_TEST_ACTIF(AlarmSQL)
  GUM_TEST_ACTIF(AsiaSQL)

} /* namespace gum_tests */
