/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <algorithm>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/databaseTable.h>
#include <agrum/base/database/DBInitializerFromSQL.h>
#include <agrum/base/database/DBTranslator4ContinuousVariable.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/base/database/rawDatabaseTable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DBInitializerFromSQL
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DBInitializerFromSQLTestSuite {
    public:
    static void test_init1() {
#ifdef ODBC_
      try {
        const std::string dataSource = "PostgreSQL";
        const std::string login      = "gonzales";
        const std::string password   = "agrum";
        const std::string query      = "SELECT * FROM testunit.asia";

        // the database contains the following rows:
        // smoking? lung_cancer? bronchitis? visit_to_Asia? tuberculosis
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

        gum::learning::DBInitializerFromSQL initializer(dataSource, login, password, query);

        const auto&       var_names = initializer.variableNames();
        const std::size_t nb_vars   = var_names.size();
        GUM_CHECK_EQ(nb_vars, std::size_t(8));

        gum::learning::DBTranslatorSet                 translator_set;
        gum::learning::DBTranslator4ContinuousVariable translator_cont;
        gum::learning::DBTranslator4LabelizedVariable  translator_lab;
        for (int i = 0; i <= 6; ++i)
          translator_set.insertTranslator(translator_lab, i);
        translator_set.insertTranslator(translator_cont, 7);

        gum::learning::DatabaseTable database(translator_set);

        database.setVariableNames(initializer.variableNames());
        GUM_CHECK_EQ(database.size(), std::size_t(0));

        initializer.fillDatabase(database);
        GUM_CHECK_EQ(database.size(), std::size_t(3));

        const auto& content1 = database.content();
        const auto& xrow10   = content1[0];
        GUM_CHECK_EQ(xrow10[5].discr_val, std::size_t(0));
        GUM_CHECK_EQ(xrow10[6].discr_val, std::size_t(0));
        GUM_CHECK_EQ(xrow10[7].cont_val, -1.2f);
        const auto& xrow11 = content1[1];
        GUM_CHECK_EQ(xrow11[5].discr_val, std::size_t(1));
        GUM_CHECK_EQ(xrow11[6].discr_val, std::size_t(0));
        GUM_CHECK_EQ(xrow11[7].cont_val, 2.45f);
        const auto& xrow12 = content1[2];
        GUM_CHECK_EQ(xrow12[5].discr_val, std::size_t(1));
        GUM_CHECK_EQ(xrow12[6].discr_val, std::size_t(0));
        GUM_CHECK_EQ(xrow12[7].cont_val, 4.0f);


        gum::learning::DBInitializerFromSQL initializer2(initializer);
        gum::learning::DatabaseTable        database2;
        database2.insertTranslator(translator_lab, std::size_t(1));
        database2.insertTranslator(translator_lab, std::size_t(3));
        database2.insertTranslator(translator_lab, std::size_t(4));
        database2.setVariableNames(initializer2.variableNames(), true);

        GUM_CHECK_EQ(database2.variableNames().size(), 3);
        {
          const auto ignored_vect = database2.ignoredColumns();
          GUM_CHECK_EQ(ignored_vect.size(), std::size_t(3));
          GUM_CHECK_EQ(ignored_vect[0], std::size_t(0));
          GUM_CHECK_EQ(ignored_vect[1], std::size_t(2));
          GUM_CHECK_EQ(ignored_vect[2], std::size_t(5));
        }

        initializer2.fillDatabase(database2);
        GUM_CHECK_EQ(database2.size(), std::size_t(3));
        const auto& content = database2.content();

        const auto& row0 = content[0];
        GUM_CHECK_EQ(row0[0].discr_val, std::size_t(0));
        GUM_CHECK_EQ(row0[1].discr_val, std::size_t(0));
        GUM_CHECK_EQ(row0[2].discr_val, std::size_t(0));
        const auto& row1 = content[1];
        GUM_CHECK_EQ(row1[0].discr_val, std::size_t(1));
        GUM_CHECK_EQ(row1[1].discr_val, std::size_t(0));
        GUM_CHECK_EQ(row1[2].discr_val, std::size_t(0));
        const auto& row2 = content[1];
        GUM_CHECK_EQ(row2[0].discr_val, std::size_t(1));
        GUM_CHECK_EQ(row2[1].discr_val, std::size_t(0));
        GUM_CHECK_EQ(row2[2].discr_val, std::size_t(0));


        gum::learning::DBInitializerFromSQL initializer3(initializer);
        gum::learning::DatabaseTable        database3;
        database3.ignoreColumn(0);
        database3.ignoreColumn(2);
        database3.ignoreColumn(5);

        for (std::size_t i = 0; i <= 6; ++i) {
          try {
            database3.insertTranslator(translator_lab, i);
          } catch (gum::OperationNotAllowed&) {}
        }
        database3.insertTranslator(translator_cont, 7);

        const std::vector< std::string > names{"x1", "x3", "x4", "x6", "x7"};
        database3.setVariableNames(names, false);

        GUM_CHECK_EQ(database3.variableNames().size(), std::size_t(5));

        {
          const auto ignored_vect = database3.ignoredColumns();
          GUM_CHECK_EQ(ignored_vect.size(), std::size_t(4));
          GUM_CHECK_EQ(ignored_vect[0], 0);
          GUM_CHECK_EQ(ignored_vect[1], 2);
          GUM_CHECK_EQ(ignored_vect[2], 5);
          GUM_CHECK_EQ(ignored_vect[3], 8);
        }

        initializer3.fillDatabase(database3);
        GUM_CHECK_EQ(database3.size(), std::size_t(3));
        const auto& content3 = database3.content();

        const auto& row30 = content3[0];
        GUM_CHECK_EQ(row30.size(), std::size_t(5));
        GUM_CHECK_EQ(row30[0].discr_val, std::size_t(0));
        GUM_CHECK_EQ(row30[1].discr_val, std::size_t(0));
        GUM_CHECK_EQ(row30[2].discr_val, std::size_t(0));
        GUM_CHECK_EQ(row30[3].discr_val, std::size_t(0));
        GUM_CHECK_EQ(row30[4].cont_val, -1.2f);
        const auto& row31 = content3[1];
        GUM_CHECK_EQ(row31.size(), std::size_t(5));
        GUM_CHECK_EQ(row31[0].discr_val, std::size_t(1));
        GUM_CHECK_EQ(row31[1].discr_val, std::size_t(0));
        GUM_CHECK_EQ(row31[2].discr_val, std::size_t(0));
        GUM_CHECK_EQ(row31[3].discr_val, std::size_t(0));
        GUM_CHECK_EQ(row31[4].cont_val, 2.45f);

        gum::learning::DBInitializerFromSQL initializer4(std::move(initializer));
        gum::learning::DatabaseTable        database4;
        database4.insertTranslator(translator_lab, std::size_t(1));
        database4.insertTranslator(translator_lab, std::size_t(3));
        database4.insertTranslator(translator_lab, std::size_t(4));
        database4.setVariableNames(initializer4.variableNames(), true);
        GUM_CHECK_EQ(database4.variableNames().size(), 3);
        initializer4.fillDatabase(database4);
        GUM_CHECK_EQ(database4.size(), std::size_t(3));

        gum::learning::DBInitializerFromSQL initializer5(std::move(initializer));
        gum::learning::DatabaseTable        database5;
        database5.insertTranslator(translator_lab, std::size_t(1));
        database5.insertTranslator(translator_lab, std::size_t(3));
        database5.insertTranslator(translator_lab, std::size_t(4));
        database5.setVariableNames(initializer5.variableNames(), true);
        GUM_CHECK_EQ(database5.variableNames().size(), 3);
        initializer5.fillDatabase(database5);
        GUM_CHECK_EQ(database5.size(), std::size_t(3));

        gum::learning::DBInitializerFromSQL* initializer6 = initializer4.clone();
        gum::learning::DatabaseTable         database6;
        database6.insertTranslator(translator_lab, std::size_t(1));
        database6.insertTranslator(translator_lab, std::size_t(3));
        database6.insertTranslator(translator_lab, std::size_t(4));
        database6.setVariableNames(initializer6->variableNames(), true);
        GUM_CHECK_EQ(database6.variableNames().size(), 3);
        initializer6->fillDatabase(database6);
        GUM_CHECK_EQ(database6.size(), std::size_t(3));

        delete initializer6;

        gum::learning::DBInitializerFromSQL* initializer7 = initializer4.clone();
        gum::learning::DatabaseTable         database7;
        database7.insertTranslator(translator_lab, std::size_t(1));
        database7.insertTranslator(translator_lab, std::size_t(3));
        database7.insertTranslator(translator_lab, std::size_t(4));
        database7.setVariableNames(initializer7->variableNames(), true);
        GUM_CHECK_EQ(database7.variableNames().size(), 3);
        initializer7->fillDatabase(database7);
        GUM_CHECK_EQ(database7.size(), std::size_t(3));

        delete initializer7;

        initializer = initializer5;
        gum::learning::DatabaseTable database8;
        database8.insertTranslator(translator_lab, std::size_t(1));
        database8.insertTranslator(translator_lab, std::size_t(3));
        database8.insertTranslator(translator_lab, std::size_t(4));
        database8.setVariableNames(initializer.variableNames(), true);
        GUM_CHECK_EQ(database8.variableNames().size(), 3);
        initializer.fillDatabase(database8);
        GUM_CHECK_EQ(database8.size(), std::size_t(3));

        initializer = std::move(initializer5);
        gum::learning::DatabaseTable database9;
        database9.insertTranslator(translator_lab, std::size_t(1));
        database9.insertTranslator(translator_lab, std::size_t(3));
        database9.insertTranslator(translator_lab, std::size_t(4));
        database9.setVariableNames(initializer.variableNames(), true);
        GUM_CHECK_EQ(database9.variableNames().size(), 3);
        initializer.fillDatabase(database9);
        GUM_CHECK_EQ(database9.size(), std::size_t(3));
      }

      catch (nanodbc::database_error& e) {
        // no database. see e.what()
        std::cout << "[PostgreSQL error]";
      }
#endif   // ODBC_
    }

    static void test_init_sqlite() {
#ifdef ODBC_
      const std::string driver_name = "SQLite3";
#  ifdef WIN32_
      driver_name += " ODBC Driver";
#  endif   // WIN32_

      try {
        const std::string connection_string = "Driver=" + driver_name + ";Database="
                                            + GET_RESSOURCES_PATH("sqlite/asia.sqlite") + ";";
        const std::string query             = "select * from asia";

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

        gum::learning::DBInitializerFromSQL initializer(connection_string, query);

        const auto&       var_names = initializer.variableNames();
        const std::size_t nb_vars   = var_names.size();
        GUM_CHECK_EQ(nb_vars, std::size_t(8));

        gum::learning::DBTranslatorSet                 translator_set;
        gum::learning::DBTranslator4ContinuousVariable translator_cont;
        gum::learning::DBTranslator4LabelizedVariable  translator_lab;
        for (int i = 0; i <= 6; ++i)
          translator_set.insertTranslator(translator_lab, i);
        translator_set.insertTranslator(translator_cont, 7);

        gum::learning::DatabaseTable database(translator_set);

        database.setVariableNames(initializer.variableNames());
        GUM_CHECK_EQ(database.size(), std::size_t(0));

        initializer.fillDatabase(database);
        GUM_CHECK_EQ(database.size(), std::size_t(3));

        const auto& content1 = database.content();
        const auto& xrow10   = content1[0];
        GUM_CHECK_EQ(xrow10[5].discr_val, std::size_t(0));
        GUM_CHECK_EQ(xrow10[6].discr_val, std::size_t(0));
        GUM_CHECK_EQ(xrow10[7].cont_val, -1.2f);
        const auto& xrow11 = content1[1];
        GUM_CHECK_EQ(xrow11[5].discr_val, std::size_t(1));
        GUM_CHECK_EQ(xrow11[6].discr_val, std::size_t(0));
        GUM_CHECK_EQ(xrow11[7].cont_val, 2.45f);
        const auto& xrow12 = content1[2];
        GUM_CHECK_EQ(xrow12[5].discr_val, std::size_t(1));
        GUM_CHECK_EQ(xrow12[6].discr_val, std::size_t(0));
        GUM_CHECK_EQ(xrow12[7].cont_val, 4.0f);
      } catch (nanodbc::database_error& e) {
        // no database. see e.what()
        std::cout << "[SQLite error]";
      }
#endif   // ODBC_
    }
  };

  GUM_TEST_ACTIF(_init1)
  GUM_TEST_ACTIF(_init_sqlite)

} /* namespace gum_tests */
