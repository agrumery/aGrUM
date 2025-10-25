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
#pragma once


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/threads/threads.h>
#include <agrum/base/database/rawDatabaseTable.h>

namespace gum_tests {

  class GUM_TEST_SUITE(RawDatabaseTable) {
    public:
    GUM_ACTIVE_TEST(_db1) {
      gum::learning::RawDatabaseTable database;
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(0))

      database.setVariableNames({"v1", "v2", "v3"});
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database.variableNames()[0], "v1")
      TS_GUM_ASSERT_THROWS_NOTHING(database.setVariableNames({"x1", "x2"}, false))
      TS_GUM_ASSERT_THROWS_NOTHING(database.setVariableNames({"x1", "x2"}, true))
      database.setVariableNames({"x1", "x2", "x3"}, false);
      TS_ASSERT_EQUALS(database.variableNames()[0], "x1")

      const auto xmiss = gum::learning::RawDatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBCell > row(3, gum::learning::DBCell(2), 1.0f);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(1))
      TS_ASSERT_THROWS(database.setVariableNames({"x1", "x2"}, false), const gum::SizeError&)

      gum::learning::RawDatabaseTable db(database);
      TS_ASSERT_EQUALS(db.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db.nbVariables(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db.content().size(), static_cast< gum::Size >(1))

      gum::learning::RawDatabaseTable db2(db);
      TS_ASSERT_EQUALS(db2.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db2.nbVariables(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db2.content().size(), static_cast< gum::Size >(1))

      gum::learning::RawDatabaseTable db3(std::move(db2));
      TS_ASSERT_EQUALS(db3.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db3.nbVariables(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db3.content().size(), static_cast< gum::Size >(1))

      database.insertRow(row, xmiss);
      gum::learning::RawDatabaseTable db4(std::move(database));
      TS_ASSERT_EQUALS(db4.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db4.nbVariables(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db4.content().size(), static_cast< gum::Size >(2))

      TS_ASSERT_EQUALS(db.content().size(), static_cast< gum::Size >(1))
      db = db4;
      TS_ASSERT_EQUALS(db.content().size(), static_cast< gum::Size >(2))

      db2 = db4;
      TS_ASSERT_EQUALS(db2.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db2.nbVariables(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db2.content().size(), static_cast< gum::Size >(2))
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(_db1_bis) {
      const std::vector< std::string > empty_miss;
      std::vector< std::string >       vnames{"v1", "v2", "v3"};
      gum::learning::RawDatabaseTable  database(empty_miss, vnames);
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(3))

      const auto xmiss = gum::learning::RawDatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBCell > row(3, gum::learning::DBCell(2), 1.0f);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(1))

      gum::learning::RawDatabaseTable db(database);
      TS_ASSERT_EQUALS(db.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db.nbVariables(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db.content().size(), static_cast< gum::Size >(1))

      gum::learning::RawDatabaseTable db2(db);
      TS_ASSERT_EQUALS(db2.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db2.nbVariables(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db2.content().size(), static_cast< gum::Size >(1))

      gum::learning::RawDatabaseTable db3(std::move(db2));
      TS_ASSERT_EQUALS(db3.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db3.nbVariables(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db3.content().size(), static_cast< gum::Size >(1))

      database.insertRow(row, xmiss);
      gum::learning::RawDatabaseTable db4(std::move(database));
      TS_ASSERT_EQUALS(db4.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db4.nbVariables(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db4.content().size(), static_cast< gum::Size >(2))

      TS_ASSERT_EQUALS(db.content().size(), static_cast< gum::Size >(1))
      db = db4;
      TS_ASSERT_EQUALS(db.content().size(), static_cast< gum::Size >(2))

      db2 = db4;
      TS_ASSERT_EQUALS(db2.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db2.nbVariables(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(db2.content().size(), static_cast< gum::Size >(2))
    }

    GUM_ACTIVE_TEST(_db2) {
      gum::learning::RawDatabaseTable database;
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(0))

      std::vector< std::string > vect1{"v1", "v2"};
      database.setVariableNames(vect1);
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(2))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(2))
      std::vector< std::string > vect{"v1", "v2", "v3"};
      database.setVariableNames(vect);
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(3))

      const auto xmiss = gum::learning::RawDatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBCell > row(3, gum::learning::DBCell(2), 1.0f);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(1))
      std::vector< std::string > badvect{"v1", "v2", "v3", "v4"};
      TS_ASSERT_THROWS(database.setVariableNames(badvect), const gum::SizeError&)

      typename gum::learning::RawDatabaseTable::HandlerSafe handler(database);
      typename gum::learning::RawDatabaseTable::Handler     uhandler(database);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(std::move(row), xmiss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(6))

      TS_ASSERT_EQUALS(handler.range().second, (std::size_t)6)
      TS_ASSERT_EQUALS(handler.size(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(handler.DBSize(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(uhandler.size(), static_cast< gum::Size >(1))
      TS_ASSERT_EQUALS(uhandler.DBSize(), static_cast< gum::Size >(6))
      handler.setRange(1, 4);
      TS_ASSERT_EQUALS(handler.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(handler.DBSize(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(handler.numRow(), (std::size_t)0)
      uhandler.setRange(1, 4);
      TS_ASSERT_EQUALS(uhandler.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(uhandler.DBSize(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(uhandler.numRow(), (std::size_t)0)
      handler.nextRow();
      TS_ASSERT_EQUALS(handler.numRow(), (std::size_t)1)

      auto handler2(handler);
      TS_ASSERT_EQUALS(handler2.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(handler2.DBSize(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(handler2.range().second, (std::size_t)4)
      TS_ASSERT_EQUALS(handler2.numRow(), (std::size_t)1)
      handler2.nextRow();
      TS_ASSERT_EQUALS(handler2.numRow(), (std::size_t)2)

      auto uhandler2(uhandler);
      TS_ASSERT_EQUALS(uhandler2.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(uhandler2.DBSize(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(uhandler2.range().second, (std::size_t)4)
      TS_ASSERT_EQUALS(uhandler2.numRow(), (std::size_t)0)
      uhandler2.nextRow();
      TS_ASSERT_EQUALS(uhandler2.numRow(), (std::size_t)1)

      auto handler3(std::move(handler2));
      TS_ASSERT_EQUALS(handler3.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(handler3.DBSize(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(handler3.range().first, (std::size_t)1)
      TS_ASSERT_EQUALS(handler3.range().second, (std::size_t)4)
      TS_ASSERT_EQUALS(handler3.numRow(), (std::size_t)2)

      auto uhandler3(std::move(uhandler2));
      TS_ASSERT_EQUALS(uhandler3.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(uhandler3.DBSize(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(uhandler3.range().first, (std::size_t)1)
      TS_ASSERT_EQUALS(uhandler3.range().second, (std::size_t)4)
      TS_ASSERT_EQUALS(uhandler3.numRow(), (std::size_t)1)

      handler2 = handler;
      TS_ASSERT_EQUALS(handler.range().second, (std::size_t)4)
      TS_ASSERT_EQUALS(handler.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(handler.DBSize(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(handler.numRow(), (std::size_t)1)

      uhandler2 = uhandler;
      TS_ASSERT_EQUALS(uhandler.range().second, (std::size_t)4)
      TS_ASSERT_EQUALS(uhandler.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(uhandler.DBSize(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(uhandler.numRow(), (std::size_t)0)

      handler2 = std::move(handler3);
      TS_ASSERT_EQUALS(handler2.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(handler2.DBSize(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(handler2.range().second, (std::size_t)4)
      TS_ASSERT_EQUALS(handler2.numRow(), (std::size_t)2)

      uhandler2 = std::move(uhandler3);
      TS_ASSERT_EQUALS(uhandler2.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(uhandler2.DBSize(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(uhandler2.range().second, (std::size_t)4)
      TS_ASSERT_EQUALS(uhandler2.numRow(), (std::size_t)1)

      auto&                                       row2 = handler2.rowSafe();
      const std::vector< gum::learning::DBCell >& xrow = row2.row();
      TS_ASSERT_EQUALS(xrow.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(xrow[1], 2)

      TS_ASSERT_EQUALS(handler2.hasRows(), true)
      handler2.nextRow();
      TS_ASSERT_EQUALS(handler2.hasRows(), false)
      handler2.reset();
      TS_ASSERT_EQUALS(handler2.hasRows(), true)
      TS_ASSERT_EQUALS(handler2.numRow(), (std::size_t)0)

      const auto& vars = handler2.variableNames();
      TS_ASSERT_EQUALS(vars[1], "v2")
      TS_ASSERT_EQUALS(handler2.nbVariables(), static_cast< gum::Size >(3))

      uhandler2.nextRow();
      auto&       urow2 = uhandler2.rowSafe();
      const auto& uxrow = urow2.row();
      TS_ASSERT_EQUALS(uxrow.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(uxrow[1], 2)

      TS_ASSERT_EQUALS(uhandler2.hasRows(), true)
      uhandler2.nextRow();
      TS_ASSERT_EQUALS(uhandler2.hasRows(), false)
      uhandler2.reset();
      TS_ASSERT_EQUALS(uhandler2.hasRows(), true)
      TS_ASSERT_EQUALS(uhandler2.numRow(), (std::size_t)0)

      const auto& uvars = uhandler2.variableNames();
      TS_ASSERT_EQUALS(uvars[1], "v2")
      TS_ASSERT_EQUALS(uhandler2.nbVariables(), static_cast< gum::Size >(3))

      // check that we cannot convert an unsafe handler into a safe one
      TS_ASSERT_THROWS(handler2 = uhandler2, std::bad_cast&)
      gum::learning::RawDatabaseTable::Handler& uxhandler2 = handler2;
      TS_ASSERT_THROWS(uxhandler2 = uhandler2, std::bad_cast&)
    }

    GUM_ACTIVE_TEST(_db3) {
      gum::learning::RawDatabaseTable database;
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(0))

      std::vector< std::string > vect{"v1", "v2", "v3"};
      database.setVariableNames(vect);
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(3))

      const auto xmiss = gum::learning::RawDatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBCell > row(3, gum::learning::DBCell(2), 1.0f);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(1))

      gum::learning::DBRow< gum::learning::DBCell > row2(3, gum::learning::DBCell(5), 1.0f);
      database.insertRow(row2, xmiss);
      gum::learning::DBRow< gum::learning::DBCell > row3(3, gum::learning::DBCell(3), 1.0f);
      database.insertRow(std::move(row3), xmiss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(3))

      std::vector< gum::learning::RawDatabaseTable::IsMissing > is_miss(
          4,
          gum::learning::RawDatabaseTable::IsMissing::False);
      database.insertRows(std::vector< gum::learning::DBRow< gum::learning::DBCell > >(4, row),
                          is_miss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(7))
      std::vector< gum::learning::DBRow< gum::learning::DBCell > > vectx(4, row2);
      database.insertRows(vectx, is_miss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(11))
      is_miss.resize(2);
      std::vector< gum::learning::DBRow< gum::learning::DBCell > > vecty(2, row);
      database.insertRows(vecty, is_miss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(13))

      database.eraseRow(12);
      database.eraseRow(5);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(11))
      database.eraseFirstRow();
      database.eraseLastRow();
      database.eraseFirstRows(2);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(7))
      database.eraseLastRows(1);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(6))
      database.eraseRows(2, 4);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(4))
      database.eraseAllRows();
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      database.clear();
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
    }

    GUM_ACTIVE_TEST(_db4) {
      gum::learning::RawDatabaseTable database;
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(0))

      std::vector< std::string > vect{"v1", "v2", "v3"};
      database.setVariableNames(vect);
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(3))

      const auto xmiss = gum::learning::RawDatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBCell > row(3, gum::learning::DBCell(2), 1.0f);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(4))

      gum::learning::RawDatabaseTable database2 = database;
      TS_ASSERT_EQUALS(database2.content().size(), static_cast< gum::Size >(4))
      TS_ASSERT_EQUALS(database2.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database2.nbVariables(), static_cast< gum::Size >(3))

      gum::learning::DBRow< gum::learning::DBCell > row2(4, gum::learning::DBCell(2), 1.0f);
      TS_ASSERT_THROWS(database2.insertRow(row2, xmiss), const gum::SizeError&)

      std::vector< gum::learning::RawDatabaseTable::IsMissing > is_miss(
          2,
          gum::learning::RawDatabaseTable::IsMissing::False);
      gum::learning::DBRow< gum::learning::DBCell > row3(3, gum::learning::DBCell(4), 1.0f);
      gum::learning::DBRow< gum::learning::DBCell > row4(3, gum::learning::DBCell(5), 1.0f);
      database2.insertRows({row3, row4}, is_miss);
      typename gum::learning::RawDatabaseTable::HandlerSafe handler2(database2);
      typename gum::learning::RawDatabaseTable::HandlerSafe handler3(database2);
      handler2.setRange(4, 6);
      handler3.setRange(5, 6);
      TS_ASSERT_EQUALS(handler2.row()[0], 4)
      TS_ASSERT_EQUALS(handler3.row()[0], 5)
      database2.eraseLastRow();
      TS_ASSERT_EQUALS(handler2.row()[0], 4)
      TS_ASSERT_THROWS(handler3.rowSafe()[0], const gum::OutOfBounds&)
      database2.insertRow(row4, xmiss);

      typename gum::learning::RawDatabaseTable::Handler uhandler2(database2);
      typename gum::learning::RawDatabaseTable::Handler uhandler3(database2);
      uhandler2.setRange(4, 6);
      uhandler3.setRange(5, 6);
      TS_ASSERT_EQUALS(uhandler2.row()[0], 4)
      TS_ASSERT_EQUALS(uhandler3.row()[0], 5)
      database2.eraseLastRow();
      TS_ASSERT_EQUALS(uhandler2.row()[0], 4)
      TS_ASSERT_EQUALS(uhandler3.size(), static_cast< gum::Size >(1))

      gum::learning::RawDatabaseTable database3(std::move(database2));
      database2 = std::move(database);
      TS_ASSERT_EQUALS(database2.content().size(), static_cast< gum::Size >(4))
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      database = std::move(database3);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(5))

      handler2 = database.handlerSafe();
      TS_ASSERT_EQUALS(handler2.range().second, (std::size_t)5)
      handler2.nextRow();
      handler2.nextRow();
      handler2.nextRow();
      handler3 = std::move(handler2);
      TS_ASSERT_EQUALS(handler3.range().second, (std::size_t)5)
      TS_ASSERT(handler3.hasRows())
      handler3.nextRow();
      TS_ASSERT(handler3.hasRows())
      handler3.nextRow();
      TS_ASSERT_EQUALS(handler3.hasRows(), false)
    }

    GUM_ACTIVE_TEST(_db5) {
      gum::learning::RawDatabaseTable database;
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(0))

      std::vector< std::string > vect{"v1", "v2", "v3"};
      database.setVariableNames(vect);
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(3))

      const auto xmiss = gum::learning::RawDatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBCell > row(3, gum::learning::DBCell(2), 1.0f);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(4))

      std::vector< gum::learning::RawDatabaseTable::IsMissing > is_miss(
          2,
          gum::learning::RawDatabaseTable::IsMissing::False);
      gum::learning::DBRow< gum::learning::DBCell > row3(3, gum::learning::DBCell(4), 1.0f);
      gum::learning::DBRow< gum::learning::DBCell > row4(3, gum::learning::DBCell(5), 1.0f);
      database.insertRows({row3, row4}, is_miss);
      typename gum::learning::RawDatabaseTable::Handler handler(database);

      int x = 0;
      for (const auto& row: handler) {
        x++;
        TS_ASSERT_EQUALS(row.size(), static_cast< gum::Size >(3))
      }
      TS_ASSERT_EQUALS(x, 6)

      handler.setRange(1, 3);
      x = 0;
      for (const auto& row: handler) {
        x++;
        TS_ASSERT_EQUALS(row.size(), static_cast< gum::Size >(3))
      }
      TS_ASSERT_EQUALS(x, 2)
    }

    GUM_ACTIVE_TEST(_db6) {
      gum::learning::RawDatabaseTable database;
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(0))

      database.setVariableNames({"v1", "v2", "v3"});
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(3))

      std::vector< std::string > row(3, "2");
      database.insertRow(row);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(1))
      for (const auto& xrow: database) {
        for (const auto& xxx: xrow.row()) {
          TS_ASSERT_EQUALS(xxx, gum::learning::DBCell(2))
        }
      }

      std::vector< std::string > row2(3, "2");
      database.insertRow(row2);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(2))
      for (const auto& xrow: database) {
        for (const auto& xxx: xrow.row()) {
          TS_ASSERT_EQUALS(xxx, gum::learning::DBCell(2))
        }
      }

      std::vector< std::string > row3(5, "3");
      for (int i = 0; i < 5; i++)
        row3[i] = std::to_string(i);
      database.ignoreColumn(1);
      database.ignoreColumn(3);
      database.ignoreColumn(4);
      TS_GUM_ASSERT_THROWS_NOTHING(database.insertRow(row3))

      database.ignoreColumn(5);
      TS_GUM_ASSERT_THROWS_NOTHING(database.insertRow(row3))

      database.ignoreColumn(6);
      database.ignoreColumn(7);
      TS_GUM_ASSERT_THROWS_NOTHING(database.insertRow(row3))
    }

    GUM_ACTIVE_TEST(_db_parallel) {
      gum::learning::RawDatabaseTable database;
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(0))

      std::vector< std::string > vect{"v1", "v2", "v3"};
      database.setVariableNames(vect);
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(3))

      const auto xmiss = gum::learning::RawDatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBCell > row(3, gum::learning::DBCell(2), 1.0f);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(4))

      std::vector< gum::learning::RawDatabaseTable::IsMissing > is_miss(
          2,
          gum::learning::RawDatabaseTable::IsMissing::False);
      gum::learning::DBRow< gum::learning::DBCell > row3(3, gum::learning::DBCell(4), 1.0f);
      gum::learning::DBRow< gum::learning::DBCell > row4(3, gum::learning::DBCell(5), 1.0f);
      database.insertRows({row3, row4}, is_miss);

      const unsigned int num_threads = gum::getNumberOfThreads();

      std::vector< gum::learning::RawDatabaseTable::Handler* > handlers(num_threads);
      std::vector< std::thread* >                              threads(num_threads);
      std::vector< int >                                       nb(num_threads);

      for (unsigned int i = 0; i < num_threads; ++i) {
        threads[i] = new std::thread(&RawDatabaseTableTestSuite::_create_handler_,
                                     this,
                                     &database,
                                     &(handlers[i]),
                                     &(nb[i]));
      }

      for (unsigned int i = 0; i < num_threads; ++i) {
        threads[i]->join();
      }

      for (unsigned int i = 0; i < num_threads; ++i) {
        delete threads[i];
        delete handlers[i];
      }

      for (unsigned int i = 0; i < num_threads; ++i) {
        TS_ASSERT_EQUALS(nb[i], 6)
      }
    }

    GUM_ACTIVE_TEST(_db_parallel2) {
      gum::learning::RawDatabaseTable database;
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(0))

      std::vector< std::string > vect{"v1", "v2", "v3"};
      database.setVariableNames(vect);
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(3))

      const auto xmiss = gum::learning::RawDatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBCell > row(3, gum::learning::DBCell(2), 1.0f);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(4))

      std::vector< gum::learning::RawDatabaseTable::IsMissing > is_miss(
          2,
          gum::learning::RawDatabaseTable::IsMissing::False);
      gum::learning::DBRow< gum::learning::DBCell > row3(3, gum::learning::DBCell(4), 1.0f);
      gum::learning::DBRow< gum::learning::DBCell > row4(3, gum::learning::DBCell(5), 1.0f);
      database.insertRows({row3, row4}, is_miss);

      const unsigned int num_threads = gum::getNumberOfThreads();

      std::vector< gum::learning::RawDatabaseTable::HandlerSafe* > handlers(num_threads);
      std::vector< std::thread* >                                  threads(num_threads);
      std::vector< int >                                           nb(num_threads);

      for (unsigned int i = 0; i < num_threads; ++i) {
        threads[i] = new std::thread(&RawDatabaseTableTestSuite::_create_handler_safe_,
                                     this,
                                     &database,
                                     &(handlers[i]),
                                     &(nb[i]));
      }

      for (unsigned int i = 0; i < num_threads; ++i) {
        threads[i]->join();
      }

      for (unsigned int i = 0; i < num_threads; ++i) {
        delete threads[i];
        delete handlers[i];
      }

      for (unsigned int i = 0; i < num_threads; ++i) {
        TS_ASSERT_EQUALS(nb[i], 6)
      }
    }

    GUM_ACTIVE_TEST(_iterators) {
      gum::learning::RawDatabaseTable database;
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(0))

      std::vector< std::string > vect{"v1", "v2", "v3"};
      database.setVariableNames(vect);
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(3))

      const auto xmiss = gum::learning::RawDatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBCell > row(3, gum::learning::DBCell(2), 1.0f);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(4))

      int nb_col1 = 0, nb_col2 = 0;
      for (const auto& row: database) {
        const auto& r = row.row();
        nb_col1 += r[0].integer();
        nb_col2 += r[1].integer();
      }
      TS_ASSERT_EQUALS(nb_col1, 8)
      TS_ASSERT_EQUALS(nb_col2, 8)

      nb_col1 = 0;
      nb_col2 = 0;
      for (auto iter = database.begin(); iter != database.end(); ++iter) {
        const auto& r = iter->row();
        nb_col1 += r[0].integer();
        nb_col2 += r[1].integer();
      }
      TS_ASSERT_EQUALS(nb_col1, 8)
      TS_ASSERT_EQUALS(nb_col2, 8)

      nb_col1 = 0;
      nb_col2 = 0;
      for (auto iter = database.beginSafe(); iter != database.endSafe(); ++iter) {
        const auto& r = (*iter).row();
        nb_col1 += r[0].integer();
        nb_col2 += r[1].integer();
      }
      TS_ASSERT_EQUALS(nb_col1, 8)
      TS_ASSERT_EQUALS(nb_col2, 8)

      nb_col1 = 0;
      nb_col2 = 0;
      for (auto iter = database.beginSafe(); iter != database.endSafe(); ++iter) {
        const auto& r = iter->row();
        nb_col1 += r[0].integer();
        nb_col2 += r[1].integer();
      }
      TS_ASSERT_EQUALS(nb_col1, 8)
      TS_ASSERT_EQUALS(nb_col2, 8)

      std::vector< gum::learning::RawDatabaseTable::IsMissing > is_miss(
          2,
          gum::learning::RawDatabaseTable::IsMissing::False);
      gum::learning::DBRow< gum::learning::DBCell > row3(3, gum::learning::DBCell(4), 1.0f);
      gum::learning::DBRow< gum::learning::DBCell > row4(3, gum::learning::DBCell(5), 1.0f);
      database.insertRows({row3, row4}, is_miss);

      nb_col1 = 0;
      nb_col2 = 0;
      for (const auto& row: database) {
        const auto& r = row.row();
        nb_col1 += r[0].integer();
        nb_col2 += r[1].integer();
      }
      TS_ASSERT_EQUALS(nb_col1, 17)
      TS_ASSERT_EQUALS(nb_col2, 17)

      nb_col1 = 0;
      nb_col2 = 0;
      for (auto iter = database.beginSafe(); iter != database.endSafe(); ++iter) {
        const auto& r = (*iter).row();
        nb_col1 += r[0].integer();
        nb_col2 += r[1].integer();
      }
      TS_ASSERT_EQUALS(nb_col1, 17)
      TS_ASSERT_EQUALS(nb_col2, 17)
    }

    GUM_ACTIVE_TEST(_ignored_colums) {
      const std::vector< std::string > empty_miss;
      std::vector< std::string >       names{"v0", "v1", "v2", "v3", "v4", "v5"};
      gum::learning::RawDatabaseTable  database(empty_miss, names);
      const auto&                      dbnames = database.variableNames();
      const auto&                      content = database.content();
      TS_ASSERT_EQUALS(dbnames[0], "v0")
      TS_ASSERT_EQUALS(dbnames[1], "v1")
      TS_ASSERT_EQUALS(dbnames[2], "v2")
      TS_ASSERT_EQUALS(dbnames[3], "v3")

      gum::learning::DBRow< gum::learning::DBCell > row(6);
      for (std::size_t i = std::size_t(0); i < 6; ++i) {
        row[i] = gum::learning::DBCell(int(i));
      }

      const auto xmiss = gum::learning::RawDatabaseTable::IsMissing::False;
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)std::size_t(2))
      TS_ASSERT_EQUALS(content[0][0], gum::learning::DBCell(int(0)))
      TS_ASSERT_EQUALS(content[0][1], gum::learning::DBCell(int(1)))
      TS_ASSERT_EQUALS(content[0][2], gum::learning::DBCell(int(2)))
      TS_ASSERT_EQUALS(content[0][3], gum::learning::DBCell(int(3)))
      TS_ASSERT_EQUALS(content[1][0], gum::learning::DBCell(int(0)))
      TS_ASSERT_EQUALS(content[1][1], gum::learning::DBCell(int(1)))
      TS_ASSERT_EQUALS(content[1][2], gum::learning::DBCell(int(2)))
      TS_ASSERT_EQUALS(content[1][3], gum::learning::DBCell(int(3)))

      database.ignoreColumn(0, true);
      TS_ASSERT_EQUALS(dbnames.size(), (gum::Size)std::size_t(5))
      TS_ASSERT_EQUALS(dbnames[0], "v1")
      TS_ASSERT_EQUALS(dbnames[1], "v2")
      TS_ASSERT_EQUALS(dbnames[2], "v3")
      TS_ASSERT_EQUALS(dbnames[3], "v4")
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)std::size_t(2))
      TS_ASSERT_EQUALS(database.content()[0].size(), (gum::Size)std::size_t(5))
      TS_ASSERT_EQUALS(content[0][0], gum::learning::DBCell(int(1)))
      TS_ASSERT_EQUALS(content[0][1], gum::learning::DBCell(int(2)))
      TS_ASSERT_EQUALS(content[0][2], gum::learning::DBCell(int(3)))
      TS_ASSERT_EQUALS(content[0][3], gum::learning::DBCell(int(4)))
      TS_ASSERT_EQUALS(content[1][0], gum::learning::DBCell(int(1)))
      TS_ASSERT_EQUALS(content[1][1], gum::learning::DBCell(int(2)))
      TS_ASSERT_EQUALS(content[1][2], gum::learning::DBCell(int(3)))
      TS_ASSERT_EQUALS(content[1][3], gum::learning::DBCell(int(4)))

      database.ignoreColumn(1, false);
      TS_ASSERT_EQUALS(dbnames.size(), (gum::Size)std::size_t(4))
      TS_ASSERT_EQUALS(dbnames[0], "v1")
      TS_ASSERT_EQUALS(dbnames[1], "v3")
      TS_ASSERT_EQUALS(dbnames[2], "v4")
      TS_ASSERT_EQUALS(dbnames[3], "v5")
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)std::size_t(2))
      TS_ASSERT_EQUALS(database.content()[0].size(), (gum::Size)std::size_t(4))
      TS_ASSERT_EQUALS(content[0][0], gum::learning::DBCell(int(1)))
      TS_ASSERT_EQUALS(content[0][1], gum::learning::DBCell(int(3)))
      TS_ASSERT_EQUALS(content[0][2], gum::learning::DBCell(int(4)))
      TS_ASSERT_EQUALS(content[0][3], gum::learning::DBCell(int(5)))
      TS_ASSERT_EQUALS(content[1][0], gum::learning::DBCell(int(1)))
      TS_ASSERT_EQUALS(content[1][1], gum::learning::DBCell(int(3)))
      TS_ASSERT_EQUALS(content[1][2], gum::learning::DBCell(int(4)))
      TS_ASSERT_EQUALS(content[1][3], gum::learning::DBCell(int(5)))

      {
        const auto ignored_vect = database.ignoredColumns();
        TS_ASSERT_EQUALS(ignored_vect[0], std::size_t(0))
        TS_ASSERT_EQUALS(ignored_vect[1], std::size_t(2))

        const auto cols = database.inputColumns();
        TS_ASSERT_EQUALS(cols[0], std::size_t(1))
        TS_ASSERT_EQUALS(cols[1], std::size_t(3))
        TS_ASSERT_EQUALS(cols[2], std::size_t(4))
        TS_ASSERT_EQUALS(cols[3], std::size_t(5))
      }

      database.ignoreColumn(12, true);
      database.ignoreColumn(10, true);
      {
        const auto ignored_vect = database.ignoredColumns();
        TS_ASSERT_EQUALS(ignored_vect[0], std::size_t(0))
        TS_ASSERT_EQUALS(ignored_vect[1], std::size_t(2))
        TS_ASSERT_EQUALS(ignored_vect[2], std::size_t(10))
        TS_ASSERT_EQUALS(ignored_vect[3], std::size_t(12))

        const auto cols = database.inputColumns();
        TS_ASSERT_EQUALS(cols[0], std::size_t(1))
        TS_ASSERT_EQUALS(cols[1], std::size_t(3))
        TS_ASSERT_EQUALS(cols[2], std::size_t(4))
        TS_ASSERT_EQUALS(cols[3], std::size_t(5))
      }


      database.ignoreColumn(3, true);
      TS_ASSERT_EQUALS(dbnames.size(), (gum::Size)std::size_t(3))
      TS_ASSERT_EQUALS(dbnames[0], "v1")
      TS_ASSERT_EQUALS(dbnames[1], "v4")
      TS_ASSERT_EQUALS(dbnames[2], "v5")
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)std::size_t(2))
      TS_ASSERT_EQUALS(database.content()[0].size(), (gum::Size)std::size_t(3))
      TS_ASSERT_EQUALS(content[0][0], gum::learning::DBCell(int(1)))
      TS_ASSERT_EQUALS(content[0][1], gum::learning::DBCell(int(4)))
      TS_ASSERT_EQUALS(content[0][2], gum::learning::DBCell(int(5)))
      TS_ASSERT_EQUALS(content[1][0], gum::learning::DBCell(int(1)))
      TS_ASSERT_EQUALS(content[1][1], gum::learning::DBCell(int(4)))
      TS_ASSERT_EQUALS(content[1][2], gum::learning::DBCell(int(5)))

      database.ignoreColumn(5, true);
      TS_ASSERT_EQUALS(dbnames.size(), (gum::Size)std::size_t(2))
      database.ignoreColumn(1, true);
      TS_ASSERT_EQUALS(dbnames.size(), (gum::Size)std::size_t(1))
      TS_ASSERT_EQUALS(content[0][0], gum::learning::DBCell(int(4)))
      TS_ASSERT_EQUALS(content[1][0], gum::learning::DBCell(int(4)))
      database.ignoreColumn(4, true);
      TS_ASSERT_EQUALS(dbnames.size(), (gum::Size)std::size_t(0))
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)std::size_t(0))
    }

    GUM_ACTIVE_TEST(_missing_vals) {
      std::vector< std::string >      missing{"?", "N/A", "???"};
      gum::learning::RawDatabaseTable database(missing);
      TS_ASSERT_EQUALS(database.content().size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(0))

      std::vector< std::string > vect{"v0", "v1", "v2", "v3"};
      database.setVariableNames(vect);
      TS_ASSERT_EQUALS(database.variableNames().size(), static_cast< gum::Size >(4))
      TS_ASSERT_EQUALS(database.nbVariables(), static_cast< gum::Size >(4))

      std::vector< std::string > row{"L0", "L1", "L2", "L0"};
      database.insertRow(row);

      row[0] = "?";
      database.insertRow(row);

      row[0] = "L0";
      row[1] = "?";
      database.insertRow(row);

      row[2] = "N/A";
      database.insertRow(row);

      row[0] = "???";
      database.insertRow(row);

      row[0] = "L0";
      row[1] = "L0";
      row[2] = "L0";
      database.insertRow(row);

      TS_ASSERT(database.hasMissingValues())

      TS_ASSERT_EQUALS(database.hasMissingValues(0), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(1), true)
      TS_ASSERT_EQUALS(database.hasMissingValues(2), true)
      TS_ASSERT_EQUALS(database.hasMissingValues(3), true)
      TS_ASSERT_EQUALS(database.hasMissingValues(4), true)
      TS_ASSERT_EQUALS(database.hasMissingValues(5), false)

      database.ignoreColumn(1);
      TS_ASSERT(database.hasMissingValues())
      TS_ASSERT_EQUALS(database.hasMissingValues(0), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(1), true)
      TS_ASSERT_EQUALS(database.hasMissingValues(2), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(3), true)
      TS_ASSERT_EQUALS(database.hasMissingValues(4), true)
      TS_ASSERT_EQUALS(database.hasMissingValues(5), false)

      database.ignoreColumn(2);
      TS_ASSERT(database.hasMissingValues())
      TS_ASSERT_EQUALS(database.hasMissingValues(0), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(1), true)
      TS_ASSERT_EQUALS(database.hasMissingValues(2), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(3), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(4), true)
      TS_ASSERT_EQUALS(database.hasMissingValues(5), false)

      database.ignoreColumn(0);
      TS_ASSERT_EQUALS(database.hasMissingValues(), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(0), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(1), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(2), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(3), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(4), false)
      TS_ASSERT_EQUALS(database.hasMissingValues(5), false)

      database.ignoreColumn(3);
      TS_ASSERT_EQUALS(database.hasMissingValues(), false)
    }


    private:
    void _create_handler_(gum::learning::RawDatabaseTable*           database,
                          gum::learning::RawDatabaseTable::Handler** handler,
                          int*                                       nb) {
      *handler = new gum::learning::RawDatabaseTable::Handler(*database);

      int x = 0;
      for (const auto& row: **handler) {
        TS_ASSERT_EQUALS(row.size(), static_cast< gum::Size >(3))
        x++;
      }

      *nb = x;
    }

    void _create_handler_safe_(gum::learning::RawDatabaseTable*               database,
                               gum::learning::RawDatabaseTable::HandlerSafe** handler,
                               int*                                           nb) {
      *handler = new gum::learning::RawDatabaseTable::HandlerSafe(*database);

      int x = 0;
      for (const auto& row: **handler) {
        TS_ASSERT_EQUALS(row.size(), static_cast< gum::Size >(3))
        x++;
      }

      *nb = x;
    }
  };

} /* namespace gum_tests */
