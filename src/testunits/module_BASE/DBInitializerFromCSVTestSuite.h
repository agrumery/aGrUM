/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/databaseTable.h>
#include <agrum/base/database/DBInitializerFromCSV.h>
#include <agrum/base/database/DBTranslator4ContinuousVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/base/database/rawDatabaseTable.h>

namespace gum_tests {

  class [[maybe_unused]] DBInitializerFromCSVTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_init1) {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/asia.csv"));

      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars   = var_names.size();
      TS_ASSERT_EQUALS(nb_vars, std::size_t(8))

      gum::learning::DBTranslatorSet                 translator_set;
      gum::learning::DBTranslator4ContinuousVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);

      database.setVariableNames(initializer.variableNames());
      TS_ASSERT_EQUALS(database.size(), std::size_t(0))
      initializer.fillDatabase(database);
      TS_ASSERT_EQUALS(database.size(), std::size_t(10000))

      gum::learning::DBInitializerFromCSV initializer2(initializer);
      gum::learning::DatabaseTable        database2;
      database2.insertTranslator(translator, 1);
      database2.insertTranslator(translator, 3);
      database2.insertTranslator(translator, 4);
      database2.setVariableNames(initializer2.variableNames(), true);
      TS_ASSERT_EQUALS(database2.variableNames().size(), (gum::Size)3)
      {
        const auto ignored_vect = database2.ignoredColumns();
        TS_ASSERT_EQUALS(ignored_vect.size(), (std::size_t)3)
        TS_ASSERT_EQUALS(ignored_vect[0], (std::size_t)0)
        TS_ASSERT_EQUALS(ignored_vect[1], (std::size_t)2)
        TS_ASSERT_EQUALS(ignored_vect[2], (std::size_t)5)
      }

      initializer2.fillDatabase(database2);
      TS_ASSERT_EQUALS(database2.size(), std::size_t(10000))
      const auto& content = database2.content();

      const auto& row0 = content[0];
      TS_ASSERT_EQUALS(row0[0].cont_val, 0.0f)
      TS_ASSERT_EQUALS(row0[1].cont_val, 1.0f)
      TS_ASSERT_EQUALS(row0[2].cont_val, 1.0f)
      const auto& row1 = content[1];
      TS_ASSERT_EQUALS(row1[0].cont_val, 1.0f)
      TS_ASSERT_EQUALS(row1[1].cont_val, 1.0f)
      TS_ASSERT_EQUALS(row1[2].cont_val, 1.0f)

      gum::learning::DBInitializerFromCSV initializer3(initializer);
      gum::learning::DatabaseTable        database3;
      database3.ignoreColumn(0);
      database3.ignoreColumn(2);
      database3.ignoreColumn(5);

      for (std::size_t i = 0; i < nb_vars; ++i) {
        try {
          database3.insertTranslator(translator, i);
        } catch (gum::OperationNotAllowed&) {}
      }
      const std::vector< std::string > names{"x1", "x3", "x4", "x6", "x7"};
      database3.setVariableNames(names, false);
      TS_ASSERT_EQUALS(database3.variableNames().size(), std::size_t(5))

      {
        const auto ignored_vect = database3.ignoredColumns();
        TS_ASSERT_EQUALS(ignored_vect.size(), (std::size_t)4)
        TS_ASSERT_EQUALS(ignored_vect[0], (std::size_t)0)
        TS_ASSERT_EQUALS(ignored_vect[1], (std::size_t)2)
        TS_ASSERT_EQUALS(ignored_vect[2], (std::size_t)5)
        TS_ASSERT_EQUALS(ignored_vect[3], (std::size_t)8)
      }

      initializer3.fillDatabase(database3);
      TS_ASSERT_EQUALS(database3.size(), std::size_t(10000))
      const auto& content3 = database3.content();

      const auto& row30 = content3[0];
      TS_ASSERT_EQUALS(row30.size(), std::size_t(5))
      TS_ASSERT_EQUALS(row30[0].cont_val, 0.0f)
      TS_ASSERT_EQUALS(row30[1].cont_val, 1.0f)
      TS_ASSERT_EQUALS(row30[2].cont_val, 1.0f)
      TS_ASSERT_EQUALS(row30[3].cont_val, 0.0f)
      TS_ASSERT_EQUALS(row30[4].cont_val, 0.0f)
      const auto& row31 = content3[1];
      TS_ASSERT_EQUALS(row31.size(), std::size_t(5))
      TS_ASSERT_EQUALS(row31[0].cont_val, 1.0f)
      TS_ASSERT_EQUALS(row31[1].cont_val, 1.0f)
      TS_ASSERT_EQUALS(row31[2].cont_val, 1.0f)
      TS_ASSERT_EQUALS(row31[3].cont_val, 0.0f)
      TS_ASSERT_EQUALS(row31[4].cont_val, 1.0f)

      gum::learning::DBInitializerFromCSV initializer4(std::move(initializer));
      gum::learning::DatabaseTable        database4;
      database4.insertTranslator(translator, 1);
      database4.insertTranslator(translator, 3);
      database4.insertTranslator(translator, 4);
      database4.setVariableNames(initializer4.variableNames(), true);
      TS_ASSERT_EQUALS(database4.variableNames().size(), (gum::Size)3)
      initializer4.fillDatabase(database4);
      TS_ASSERT_EQUALS(database4.size(), std::size_t(10000))

      gum::learning::DBInitializerFromCSV initializer5(std::move(initializer));
      gum::learning::DatabaseTable        database5;
      database5.insertTranslator(translator, 1);
      database5.insertTranslator(translator, 3);
      database5.insertTranslator(translator, 4);
      database5.setVariableNames(initializer5.variableNames(), true);
      TS_ASSERT_EQUALS(database5.variableNames().size(), (gum::Size)3)
      initializer5.fillDatabase(database5);
      TS_ASSERT_EQUALS(database5.size(), std::size_t(10000))

      gum::learning::DBInitializerFromCSV* initializer6 = initializer4.clone();
      gum::learning::DatabaseTable         database6;
      database6.insertTranslator(translator, 1);
      database6.insertTranslator(translator, 3);
      database6.insertTranslator(translator, 4);
      database6.setVariableNames(initializer6->variableNames(), true);
      TS_ASSERT_EQUALS(database6.variableNames().size(), (gum::Size)3)
      initializer6->fillDatabase(database6);
      TS_ASSERT_EQUALS(database6.size(), std::size_t(10000))

      delete initializer6;

      gum::learning::DBInitializerFromCSV* initializer7 = initializer4.clone();
      gum::learning::DatabaseTable         database7;
      database7.insertTranslator(translator, 1);
      database7.insertTranslator(translator, 3);
      database7.insertTranslator(translator, 4);
      database7.setVariableNames(initializer7->variableNames(), true);
      TS_ASSERT_EQUALS(database7.variableNames().size(), (gum::Size)3)
      initializer7->fillDatabase(database7);
      TS_ASSERT_EQUALS(database7.size(), std::size_t(10000))

      delete initializer7;

      initializer = initializer5;
      gum::learning::DatabaseTable database8;
      database8.insertTranslator(translator, 1);
      database8.insertTranslator(translator, 3);
      database8.insertTranslator(translator, 4);
      database8.setVariableNames(initializer.variableNames(), true);
      TS_ASSERT_EQUALS(database8.variableNames().size(), (gum::Size)3)
      initializer.fillDatabase(database8);
      TS_ASSERT_EQUALS(database8.size(), std::size_t(10000))

      initializer = std::move(initializer5);
      gum::learning::DatabaseTable database9;
      database9.insertTranslator(translator, 1);
      database9.insertTranslator(translator, 3);
      database9.insertTranslator(translator, 4);
      database9.setVariableNames(initializer.variableNames(), true);
      TS_ASSERT_EQUALS(database9.variableNames().size(), (gum::Size)3)
      initializer.fillDatabase(database9);
      TS_ASSERT_EQUALS(database9.size(), std::size_t(10000))
    }
  };

} /* namespace gum_tests */
