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


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/threads/threads.h>
#include <agrum/base/database/databaseTable.h>
#include <agrum/base/database/DBTranslator4ContinuousVariable.h>
#include <agrum/base/database/DBTranslator4DiscretizedVariable.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslator4RangeVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DatabaseTable
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DatabaseTableTestSuite {
    public:
    static void test_db1() {
      {
        gum::learning::DatabaseTable database1;
        CHECK((database1.content().size()) == (static_cast< gum::Size >(0)));
        CHECK((database1.variableNames().size()) == (static_cast< gum::Size >(0)));

        gum::learning::DBTranslatorSet                set;
        std::vector< std::string >                    missing{"?", "N/A", "???"};
        gum::learning::DBTranslator4LabelizedVariable translator(missing, 3);
        set.insertTranslator(translator, 1);
        set.insertTranslator(translator, 3);
        set.insertTranslator(translator, 4);

        gum::learning::DatabaseTable database(set);
        CHECK((database.content().size()) == (static_cast< gum::Size >(0)));
        CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));
        const auto dom1 = database.domainSizes();
        CHECK((dom1.size()) == (std::size_t(3)));
        CHECK((dom1[0]) == (std::size_t(0)));
        CHECK((dom1[1]) == (std::size_t(0)));
        CHECK((dom1[2]) == (std::size_t(0)));

        database.setVariableNames({"v1", "v2", "v3"}, false);
        CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));
        CHECK((database.variableNames()[0]) == ("v1"));
        CHECK((database.variableNames()[1]) == ("v2"));
        CHECK((database.variableNames()[2]) == ("v3"));
        database.setVariableNames({"x1", "x2", "x3", "x4", "x5"}, true);
        CHECK((database.variableNames()[0]) == ("x2"));
        CHECK((database.variableNames()[1]) == ("x4"));
        CHECK((database.variableNames()[2]) == ("x5"));
        CHECK_THROWS_AS(database.setVariableNames({"x1", "x2", "x3", "x4"}, false),
                        const gum::SizeError&);
        CHECK_THROWS_AS(database.setVariableNames({"x1", "x2"}, true), const gum::SizeError&);
        CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));

        std::vector< std::string > row(7, "toto");
        database.insertRow(row);
        CHECK((database.content().size()) == (static_cast< gum::Size >(1)));
        database.insertRow(row);
        CHECK((database.content().size()) == (static_cast< gum::Size >(2)));
        std::vector< std::string > row2(3, "toto");
        CHECK_THROWS_AS(database.insertRow(row2), const gum::SizeError&);

        gum::learning::DatabaseTable db(database);
        CHECK((db.variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((db.nbVariables()) == (static_cast< std::size_t >(3)));
        CHECK((db.content().size()) == (static_cast< gum::Size >(2)));

        gum::learning::DatabaseTable db2(db);
        CHECK((db2.variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((db2.nbVariables()) == (static_cast< std::size_t >(3)));
        CHECK((db2.content().size()) == (static_cast< gum::Size >(2)));

        gum::learning::DatabaseTable db3(std::move(db2));
        CHECK((db3.variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((db3.nbVariables()) == (static_cast< std::size_t >(3)));
        CHECK((db3.content().size()) == (static_cast< gum::Size >(2)));

        database.insertRow(row);
        gum::learning::DatabaseTable db4(std::move(database));
        CHECK((db4.variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((db4.nbVariables()) == (static_cast< std::size_t >(3)));
        CHECK((db4.content().size()) == (static_cast< gum::Size >(3)));

        CHECK((db.content().size()) == (static_cast< gum::Size >(2)));
        db = db4;
        CHECK((db.content().size()) == (static_cast< gum::Size >(3)));

        db2 = std::move(db4);
        CHECK((db2.variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((db2.nbVariables()) == (static_cast< std::size_t >(3)));
        CHECK((db2.content().size()) == (static_cast< gum::Size >(3)));

        gum::learning::DatabaseTable* db5 = db.clone();
        CHECK((db5->variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((db5->nbVariables()) == (static_cast< std::size_t >(3)));
        CHECK((db5->content().size()) == (static_cast< gum::Size >(3)));
        delete db5;

        gum::learning::DatabaseTable* db6 = db.clone();
        CHECK((db6->variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((db6->nbVariables()) == (static_cast< std::size_t >(3)));
        CHECK((db6->content().size()) == (static_cast< gum::Size >(3)));
        CHECK((db6->size()) == (static_cast< gum::Size >(3)));

        delete db6;

        for (const auto& db_row: db) {
          CHECK((db_row.weight()) == (1));
        }

        db.setAllRowsWeight(10.0);
        for (const auto& db_row: db) {
          CHECK((db_row.weight()) == (10.0));
        }

        CHECK((db.weight()) == (10.0 * db.size()));

        const std::size_t nbr = db.nbRows();
        for (auto i = static_cast< std::size_t >(0); i < nbr; ++i) {
          if (i % 2) db.setWeight(i, 2.0);
        }

        auto index = static_cast< std::size_t >(0);
        for (const auto& row: db) {
          if (index % 2) {
            CHECK((row.weight()) == (2.0));
            CHECK((db.weight(index)) == (2.0));
          } else {
            CHECK((row.weight()) == (10.0));
            CHECK((db.weight(index)) == (10.0));
          }
          ++index;
        }
      }   // namespace gum_tests
    }

    static void test_db2() {
      gum::learning::DBTranslatorSet set;
      std::vector< std::string >     missing{"?", "N/A", "???"};
      gum::LabelizedVariable         var("var", "", 0);
      var.addLabel("L0");
      var.addLabel("L1");
      var.addLabel("L2");
      gum::learning::DBTranslator4LabelizedVariable translator(var, missing, true, 3);
      set.insertTranslator(translator, 0);
      set.insertTranslator(translator, 1);
      set.insertTranslator(translator, 2);
      set[0].setVariableName("v1");
      set[1].setVariableName("v2");
      set[2].setVariableName("v3");

      gum::learning::DatabaseTable database(set);
      CHECK((database.content().size()) == (static_cast< gum::Size >(0)));
      CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));
      CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));

      const auto xmiss = gum::learning::DatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
          3,
          gum::learning::DBTranslatedValue{std::size_t(1)},
          1.0);
      database.insertRow(row, xmiss);
      CHECK((database.size()) == (static_cast< gum::Size >(1)));

      typename gum::learning::DatabaseTable::HandlerSafe handler(database);
      typename gum::learning::DatabaseTable::Handler     uhandler(database);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(std::move(row), xmiss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(6)));

      CHECK((handler.range().second) == (static_cast< std::size_t >(6)));
      CHECK((handler.size()) == (static_cast< gum::Size >(6)));
      CHECK((handler.DBSize()) == (static_cast< gum::Size >(6)));
      CHECK((uhandler.size()) == (static_cast< gum::Size >(1)));
      CHECK((uhandler.DBSize()) == (static_cast< gum::Size >(6)));

      handler.setRange(1, 4);
      CHECK((handler.size()) == (static_cast< gum::Size >(3)));
      CHECK((handler.DBSize()) == (static_cast< gum::Size >(6)));
      CHECK((handler.numRow()) == (static_cast< std::size_t >(0)));
      uhandler.setRange(1, 4);
      CHECK((uhandler.size()) == (static_cast< gum::Size >(3)));
      CHECK((uhandler.DBSize()) == (static_cast< gum::Size >(6)));
      CHECK((uhandler.numRow()) == (static_cast< std::size_t >(0)));
      handler.nextRow();
      CHECK((handler.numRow()) == (static_cast< std::size_t >(1)));

      auto handler2(handler);
      CHECK((handler2.size()) == (static_cast< gum::Size >(3)));
      CHECK((handler2.DBSize()) == (static_cast< gum::Size >(6)));
      CHECK((handler2.range().second) == (static_cast< std::size_t >(4)));
      CHECK((handler2.numRow()) == (static_cast< std::size_t >(1)));
      handler2.nextRow();
      CHECK((handler2.numRow()) == (static_cast< std::size_t >(2)));

      auto uhandler2(uhandler);
      CHECK((uhandler2.size()) == (static_cast< gum::Size >(3)));
      CHECK((uhandler2.DBSize()) == (static_cast< gum::Size >(6)));
      CHECK((uhandler2.range().second) == (static_cast< std::size_t >(4)));
      CHECK((uhandler2.numRow()) == (static_cast< std::size_t >(0)));
      uhandler2.nextRow();
      CHECK((uhandler2.numRow()) == (static_cast< std::size_t >(1)));

      auto handler3(std::move(handler2));
      CHECK((handler3.size()) == (static_cast< gum::Size >(3)));
      CHECK((handler3.DBSize()) == (static_cast< gum::Size >(6)));
      CHECK((handler3.range().first) == (static_cast< std::size_t >(1)));
      CHECK((handler3.range().second) == (static_cast< std::size_t >(4)));
      CHECK((handler3.numRow()) == (static_cast< std::size_t >(2)));

      auto uhandler3(std::move(uhandler2));
      CHECK((uhandler3.size()) == (static_cast< gum::Size >(3)));
      CHECK((uhandler3.DBSize()) == (static_cast< gum::Size >(6)));
      CHECK((uhandler3.range().first) == (static_cast< std::size_t >(1)));
      CHECK((uhandler3.range().second) == (static_cast< std::size_t >(4)));
      CHECK((uhandler3.numRow()) == (static_cast< std::size_t >(1)));

      handler2 = handler;
      CHECK((handler.range().second) == (static_cast< std::size_t >(4)));
      CHECK((handler.size()) == (static_cast< gum::Size >(3)));
      CHECK((handler.DBSize()) == (static_cast< gum::Size >(6)));
      CHECK((handler.numRow()) == (static_cast< std::size_t >(1)));

      uhandler2 = uhandler;
      CHECK((uhandler.range().second) == (static_cast< std::size_t >(4)));
      CHECK((uhandler.size()) == (static_cast< gum::Size >(3)));
      CHECK((uhandler.DBSize()) == (static_cast< gum::Size >(6)));
      CHECK((uhandler.numRow()) == (static_cast< std::size_t >(0)));

      handler2 = std::move(handler3);
      CHECK((handler2.size()) == (static_cast< gum::Size >(3)));
      CHECK((handler2.DBSize()) == (static_cast< gum::Size >(6)));
      CHECK((handler2.range().second) == (static_cast< std::size_t >(4)));
      CHECK((handler2.numRow()) == (static_cast< std::size_t >(2)));

      uhandler2 = std::move(uhandler3);
      CHECK((uhandler2.size()) == (static_cast< gum::Size >(3)));
      CHECK((uhandler2.DBSize()) == (static_cast< gum::Size >(6)));
      CHECK((uhandler2.range().second) == (static_cast< std::size_t >(4)));
      CHECK((uhandler2.numRow()) == (static_cast< std::size_t >(1)));

      auto&       row2 = handler2.rowSafe();
      const auto& xrow = row2.row();
      CHECK((xrow.size()) == (static_cast< gum::Size >(3)));
      CHECK((xrow[1].discr_val) == (std::size_t(1)));

      CHECK((handler2.hasRows()) == (true));
      handler2.nextRow();
      CHECK((handler2.hasRows()) == (false));
      handler2.reset();
      CHECK((handler2.hasRows()) == (true));
      CHECK((handler2.numRow()) == (static_cast< std::size_t >(0)));

      const auto& vars = handler2.variableNames();
      CHECK((vars[1]) == ("v2"));
      CHECK((handler2.nbVariables()) == (static_cast< std::size_t >(3)));

      uhandler2.nextRow();
      auto&       urow2 = uhandler2.rowSafe();
      const auto& uxrow = urow2.row();
      CHECK((uxrow.size()) == (static_cast< gum::Size >(3)));
      CHECK((uxrow[1].discr_val) == (std::size_t(1)));

      CHECK((uhandler2.hasRows()) == (true));
      uhandler2.nextRow();
      CHECK((uhandler2.hasRows()) == (false));
      uhandler2.reset();
      CHECK((uhandler2.hasRows()) == (true));
      CHECK((uhandler2.numRow()) == (static_cast< std::size_t >(0)));

      const auto& uvars = uhandler2.variableNames();
      CHECK((uvars[1]) == ("v2"));
      CHECK((uhandler2.nbVariables()) == (static_cast< std::size_t >(3)));

      // check that we cannot convert an unsafe handler into a safe one
      CHECK_THROWS_AS(handler2 = uhandler2, std::bad_cast&);
      gum::learning::DatabaseTable::Handler& uxhandler2 = handler2;
      CHECK_THROWS_AS(uxhandler2 = uhandler2, std::bad_cast&);
    }

    static void test_db3() {
      gum::learning::DBTranslatorSet set;
      std::vector< std::string >     missing{"?", "N/A", "???"};
      gum::LabelizedVariable         var("var", "", 0);
      var.addLabel("L0");
      var.addLabel("L1");
      var.addLabel("L2");
      gum::learning::DBTranslator4LabelizedVariable translator(var, missing, true, 3);
      set.insertTranslator(translator, 0);
      set.insertTranslator(translator, 1);
      set.insertTranslator(translator, 2);
      set[0].setVariableName("v1");
      set[1].setVariableName("v2");
      set[2].setVariableName("v3");

      gum::learning::DatabaseTable database(set);
      CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));
      CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));

      const auto xmiss = gum::learning::DatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
          3,
          gum::learning::DBTranslatedValue{std::size_t(2)},
          1.0f);
      database.insertRow(row, xmiss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(1)));

      const auto dom1 = database.domainSizes();
      CHECK((dom1.size()) == (std::size_t(3)));
      CHECK((dom1[0]) == (std::size_t(3)));
      CHECK((dom1[1]) == (std::size_t(3)));
      CHECK((dom1[2]) == (std::size_t(3)));
      CHECK((database.domainSize(0)) == (dom1[0]));
      CHECK((database.domainSize(1)) == (dom1[1]));
      CHECK((database.domainSize(2)) == (dom1[2]));

      gum::learning::DBRow< gum::learning::DBTranslatedValue > row2(
          3,
          gum::learning::DBTranslatedValue{std::size_t(1)},
          1.0f);
      database.insertRow(row2, xmiss);
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
          3,
          gum::learning::DBTranslatedValue{std::size_t(0)},
          1.0f);
      database.insertRow(std::move(row3), xmiss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(3)));

      std::vector< gum::learning::DatabaseTable::IsMissing > is_miss(
          4,
          gum::learning::DatabaseTable::IsMissing::False);
      database.insertRows(
          std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > >(4, row),
          is_miss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(7)));
      std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > > vectx(4, row2);
      database.insertRows(vectx, is_miss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(11)));
      std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > > vecty(2, row);
      is_miss.resize(2);
      database.insertRows(std::move(vecty), is_miss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(13)));

      database.eraseRow(12);
      database.eraseRow(5);
      CHECK((database.content().size()) == (static_cast< gum::Size >(11)));
      database.eraseFirstRow();
      database.eraseLastRow();
      database.eraseFirstRows(2);
      CHECK((database.content().size()) == (static_cast< gum::Size >(7)));
      database.eraseLastRows(1);
      CHECK((database.content().size()) == (static_cast< gum::Size >(6)));
      database.eraseRows(2, 4);
      CHECK((database.content().size()) == (static_cast< gum::Size >(4)));
      database.eraseAllRows();
      CHECK((database.content().size()) == (static_cast< gum::Size >(0)));
      CHECK((database.content().size()) == (static_cast< gum::Size >(0)));

      {
        std::vector< std::string > new_names{"x1", "x2", "x3"};
        CHECK((database.variableNames()[1]) == ("v2"));
        database.setVariableNames(new_names);
        CHECK((database.variableNames()[1]) == ("x2"));
        std::vector< std::string > names2{"y1", "y2", "y3"};
        database.setVariableNames(names2);
        CHECK((database.variableNames()[1]) == ("y2"));

        database.clear();
        CHECK((database.variableNames().size()) == (static_cast< gum::Size >(0)));
      }
    }

    static void test_db4() {
      gum::learning::DBTranslatorSet                 set;
      std::vector< std::string >                     missing{"?", "N/A", "???"};
      gum::ContinuousVariable< double >              var("var", "");
      gum::learning::DBTranslator4ContinuousVariable translator(var, missing, true);
      set.insertTranslator(translator, 0);
      set.insertTranslator(translator, 1);
      set.insertTranslator(translator, 2);
      set[0].setVariableName("v1");
      set[1].setVariableName("v2");
      set[2].setVariableName("v3");

      gum::learning::DatabaseTable database(set);
      CHECK((database.content().size()) == (static_cast< gum::Size >(0)));
      CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));
      CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));

      const auto xmiss = gum::learning::DatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
          3,
          gum::learning::DBTranslatedValue{2.0f},
          1.0f);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(4)));

      gum::learning::DatabaseTable database2 = database;
      CHECK((database2.content().size()) == (static_cast< gum::Size >(4)));
      CHECK((database2.variableNames().size()) == (static_cast< gum::Size >(3)));
      CHECK((database2.nbVariables()) == (static_cast< std::size_t >(3)));

      gum::learning::DBRow< gum::learning::DBTranslatedValue > row2(
          2,
          gum::learning::DBTranslatedValue{2.0f},
          1.0f);
      CHECK_THROWS_AS(database2.insertRow(row2, xmiss), const gum::SizeError&);

      gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
          3,
          gum::learning::DBTranslatedValue{4.0f},
          1.0f);
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row4(
          3,
          gum::learning::DBTranslatedValue{5.0f},
          1.0f);
      std::vector< gum::learning::DatabaseTable::IsMissing > is_miss(
          2,
          gum::learning::DatabaseTable::IsMissing::False);
      database2.insertRows({row3, row4}, is_miss);
      auto handler2 = database2.beginSafe();
      auto handler3 = database2.endSafe();
      handler2.setRange(4, 6);
      handler3.setRange(5, 6);
      CHECK((handler2.row()[0].cont_val) == (4.0f));
      CHECK((handler3.row()[0].cont_val) == (5.0f));
      database2.eraseLastRow();
      CHECK((handler2.row()[0].cont_val) == (4.0f));
      CHECK_THROWS_AS(handler3.rowSafe()[0], const gum::OutOfBounds&);
      database2.insertRow(row4, xmiss);

      auto uhandler2 = database2.begin();
      auto uhandler3 = database2.end();
      uhandler2.setRange(4, 6);
      uhandler3.setRange(5, 6);
      CHECK((uhandler2.row()[0].cont_val) == (4.0f));
      CHECK((uhandler3.row()[0].cont_val) == (5.0f));
      database2.eraseLastRow();
      CHECK((uhandler2.row()[0].cont_val) == (4.0f));
      CHECK((uhandler3.size()) == (static_cast< gum::Size >(1)));

      gum::learning::DatabaseTable database3(std::move(database2));
      database2 = std::move(database);
      CHECK((database2.content().size()) == (static_cast< gum::Size >(4)));
      CHECK((database.content().size()) == (static_cast< gum::Size >(0)));
      database = std::move(database3);
      CHECK((database.content().size()) == (static_cast< gum::Size >(5)));

      handler2 = database.handlerSafe();
      CHECK((handler2.range().second) == (static_cast< std::size_t >(5)));
      handler2.nextRow();
      handler2.nextRow();
      handler2.nextRow();
      handler3 = std::move(handler2);
      CHECK((handler3.range().second) == (static_cast< std::size_t >(5)));
      CHECK(handler3.hasRows());
      handler3.nextRow();
      CHECK(handler3.hasRows());
      handler3.nextRow();
      CHECK((handler3.hasRows()) == (false));
    }

    static void test_db5() {
      {
        gum::learning::DBTranslatorSet                 set;
        std::vector< std::string >                     missing{"?", "N/A", "???"};
        gum::ContinuousVariable< double >              var("var", "");
        gum::learning::DBTranslator4ContinuousVariable translator(var, missing, true);
        set.insertTranslator(translator, 0);
        set.insertTranslator(translator, 1);
        set.insertTranslator(translator, 2);
        set[0].setVariableName("v1");
        set[1].setVariableName("v2");
        set[2].setVariableName("v3");
        gum::learning::DatabaseTable database(set);

        std::vector< std::string > vect{"v1", "v2", "v3"};
        database.setVariableNames(vect, false);
        CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));

        const auto xmiss = gum::learning::DatabaseTable::IsMissing::False;
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
            3,
            gum::learning::DBTranslatedValue{2.0f},
            1.0f);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        CHECK((database.content().size()) == (static_cast< gum::Size >(4)));

        gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
            3,
            gum::learning::DBTranslatedValue{4.0f},
            1.0f);
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row4(
            3,
            gum::learning::DBTranslatedValue{5.0f},
            1.0f);
        std::vector< gum::learning::DatabaseTable::IsMissing > is_miss(
            2,
            gum::learning::DatabaseTable::IsMissing::False);
        database.insertRows({row3, row4}, is_miss);
        typename gum::learning::DatabaseTable::Handler handler(database);

        int x = 0;
        for (const auto& row: handler) {
          x++;
          CHECK((row.size()) == (static_cast< gum::Size >(3)));
        }
        CHECK((x) == (6));

        handler.setRange(1, 3);
        x = 0;
        for (const auto& row: handler) {
          x++;
          CHECK((row.size()) == (static_cast< gum::Size >(3)));
        }
        CHECK((x) == (2));
      }
    }

    static void test_db6() {
      std::vector< std::string >                     missing{"?", "N/A", "???"};
      gum::ContinuousVariable< double >              var("var", "");
      gum::learning::DBTranslator4ContinuousVariable translator(var, missing, true);

      gum::learning::DatabaseTable database;
      translator.setVariableName("v1");
      database.insertTranslator(translator, 0);
      translator.setVariableName("v2");
      database.insertTranslator(translator, 1);
      translator.setVariableName("v3");
      database.insertTranslator(translator, 2);

      CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));
      CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));
      CHECK((database.variableNames()[0]) == ("v1"));
      CHECK((database.variableNames()[1]) == ("v2"));
      CHECK((database.variableNames()[2]) == ("v3"));
      CHECK((database.variable(0).name()) == ("v1"));
      CHECK((database.variable(1).name()) == ("v2"));
      CHECK((database.variable(2).name()) == ("v3"));

      database.ignoreColumn(4);
      CHECK_THROWS_AS(database.insertTranslator(translator, 4), const gum::OperationNotAllowed&);

      const auto xmiss = gum::learning::DatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
          3,
          gum::learning::DBTranslatedValue{2.0f},
          1.0f);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(4)));
      CHECK((database.nbRows()) == (static_cast< std::size_t >(4)));
      CHECK(!database.hasMissingValues());

      GUM_CHECK_ASSERT_THROWS_NOTHING(database.insertTranslator(translator, 5));
      GUM_CHECK_ASSERT_THROWS_NOTHING(database.insertTranslator(translator, 5, false));
      CHECK(database.hasMissingValues());
      CHECK((database.nbVariables()) == (static_cast< std::size_t >(5)));
      database.eraseTranslators(5, true);
      CHECK(!database.hasMissingValues());
      CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));
      CHECK((database.content()[0].size()) == (static_cast< gum::Size >(3)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(database.eraseTranslators(10));
      database.eraseTranslators(1);
      CHECK((database.variableNames().size()) == (static_cast< gum::Size >(2)));
      CHECK((database.nbVariables()) == (static_cast< std::size_t >(2)));
      CHECK((database.variableNames()[0]) == ("v1"));
      CHECK((database.variableNames()[1]) == ("v3"));
      CHECK((database.variable(0).name()) == ("v1"));
      CHECK((database.variable(1).name()) == ("v3"));
      CHECK((database.content()[0].size()) == (static_cast< gum::Size >(2)));
      CHECK((database.content()[1].size()) == (static_cast< gum::Size >(2)));
      CHECK((database.content()[2].size()) == (static_cast< gum::Size >(2)));
      CHECK((database.content()[3].size()) == (static_cast< gum::Size >(2)));

      std::vector< std::string > row2(4, "3.0");
      database.insertRow(row2);
      database.insertRow(row2);

      gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
          2,
          gum::learning::DBTranslatedValue{4.0f},
          1.0f);
      database.insertRow(row3, xmiss);
      database.insertRow(row3, xmiss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(8)));
    }

    void test_db_parallel() {
      {
        gum::learning::DBTranslatorSet                 set;
        std::vector< std::string >                     missing{"?", "N/A", "???"};
        gum::ContinuousVariable< double >              var("var", "");
        gum::learning::DBTranslator4ContinuousVariable translator(var, missing, true);
        set.insertTranslator(translator, 0);
        set.insertTranslator(translator, 1);
        set.insertTranslator(translator, 2);
        set[0].setVariableName("v1");
        set[1].setVariableName("v2");
        set[2].setVariableName("v3");
        gum::learning::DatabaseTable database(set);
        CHECK((database.content().size()) == (static_cast< gum::Size >(0)));
        CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));

        std::vector< std::string > vect{"v1", "v2", "v3"};
        database.setVariableNames(vect);
        CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));

        const auto xmiss = gum::learning::DatabaseTable::IsMissing::False;
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
            3,
            gum::learning::DBTranslatedValue{2.0f},
            1.0f);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        CHECK((database.content().size()) == (static_cast< gum::Size >(4)));

        gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
            3,
            gum::learning::DBTranslatedValue{4.0f},
            1.0f);
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row4(
            3,
            gum::learning::DBTranslatedValue{5.0f},
            1.0f);
        std::vector< gum::learning::DatabaseTable::IsMissing > is_miss(
            2,
            gum::learning::DatabaseTable::IsMissing::False);
        database.insertRows({row3, row4}, is_miss);

        const unsigned int num_threads = gum::getNumberOfThreads();

        std::vector< gum::learning::DatabaseTable::Handler* > handlers(num_threads);
        std::vector< std::thread* >                           threads(num_threads);
        std::vector< int >                                    nb(num_threads);

        for (unsigned int i = 0; i < num_threads; ++i) {
          threads[i] = new std::thread(&DatabaseTableTestSuite::_create_handler_,
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
          CHECK((nb[i]) == (6));
        }
      }
    }

    void test_db_parallel2() {
      {
        gum::learning::DBTranslatorSet                 set;
        std::vector< std::string >                     missing{"?", "N/A", "???"};
        gum::ContinuousVariable< double >              var("var", "");
        gum::learning::DBTranslator4ContinuousVariable translator(var, missing, true);
        set.insertTranslator(translator, 0);
        set.insertTranslator(translator, 1);
        set.insertTranslator(translator, 2);
        set[0].setVariableName("v1");
        set[1].setVariableName("v2");
        set[2].setVariableName("v3");
        gum::learning::DatabaseTable database(set);
        CHECK((database.content().size()) == (static_cast< gum::Size >(0)));
        CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));

        std::vector< std::string > vect{"v1", "v2", "v3"};
        database.setVariableNames(vect);
        CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));

        const auto xmiss = gum::learning::DatabaseTable::IsMissing::False;
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
            3,
            gum::learning::DBTranslatedValue{2.0f},
            1.0f);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        CHECK((database.content().size()) == (static_cast< gum::Size >(4)));

        gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
            3,
            gum::learning::DBTranslatedValue{4.0f},
            1.0f);
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row4(
            3,
            gum::learning::DBTranslatedValue{5.0f},
            1.0f);
        std::vector< gum::learning::DatabaseTable::IsMissing > is_miss(
            2,
            gum::learning::DatabaseTable::IsMissing::False);
        database.insertRows({row3, row4}, is_miss);

        const unsigned int num_threads = gum::getNumberOfThreads();

        std::vector< gum::learning::DatabaseTable::HandlerSafe* > handlers(num_threads);
        std::vector< std::thread* >                               threads(num_threads);
        std::vector< int >                                        nb(num_threads);

        for (unsigned int i = 0; i < num_threads; ++i) {
          threads[i] = new std::thread(&DatabaseTableTestSuite::_create_handler_safe_,
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
          CHECK((nb[i]) == (6));
        }
      }
    }

    static void test_iterators() {
      {
        gum::learning::DBTranslatorSet                 set;
        std::vector< std::string >                     missing{"?", "N/A", "???"};
        gum::ContinuousVariable< double >              var("var", "");
        gum::learning::DBTranslator4ContinuousVariable translator(var, missing, true);
        set.insertTranslator(translator, 0);
        set.insertTranslator(translator, 1);
        set.insertTranslator(translator, 2);
        set[0].setVariableName("v1");
        set[1].setVariableName("v2");
        set[2].setVariableName("v3");
        gum::learning::DatabaseTable database(set);
        CHECK((database.content().size()) == (static_cast< gum::Size >(0)));
        CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));

        std::vector< std::string > vect{"v1", "v2", "v3"};
        database.setVariableNames(vect);
        CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));
        CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));

        const auto xmiss = gum::learning::DatabaseTable::IsMissing::False;
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
            3,
            gum::learning::DBTranslatedValue{2.0f},
            1.0f);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        CHECK((database.content().size()) == (static_cast< gum::Size >(4)));

        int nb_col1 = 0, nb_col2 = 0;
        for (const auto& row: database) {
          const auto& r = row.row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        CHECK((nb_col1) == (8));
        CHECK((nb_col2) == (8));

        nb_col1 = 0;
        nb_col2 = 0;
        for (auto iter = database.begin(); iter != database.end(); ++iter) {
          const auto& r = iter->row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        CHECK((nb_col1) == (8));
        CHECK((nb_col2) == (8));

        nb_col1 = 0;
        nb_col2 = 0;
        for (auto iter = database.beginSafe(); iter != database.endSafe(); ++iter) {
          const auto& r = (*iter).row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        CHECK((nb_col1) == (8));
        CHECK((nb_col2) == (8));

        nb_col1 = 0;
        nb_col2 = 0;
        for (auto iter = database.beginSafe(); iter != database.endSafe(); ++iter) {
          const auto& r = iter->row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        CHECK((nb_col1) == (8));
        CHECK((nb_col2) == (8));

        gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
            3,
            gum::learning::DBTranslatedValue{4.0f},
            1.0f);
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row4(
            3,
            gum::learning::DBTranslatedValue{5.0f},
            1.0f);
        std::vector< gum::learning::DatabaseTable::IsMissing > is_miss(
            2,
            gum::learning::DatabaseTable::IsMissing::False);
        database.insertRows({row3, row4}, is_miss);

        nb_col1 = 0;
        nb_col2 = 0;
        for (const auto& row: database) {
          const auto& r = row.row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        CHECK((nb_col1) == (17));
        CHECK((nb_col2) == (17));

        nb_col1 = 0;
        nb_col2 = 0;
        for (auto iter = database.beginSafe(); iter != database.endSafe(); ++iter) {
          const auto& r = (*iter).row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        CHECK((nb_col1) == (17));
        CHECK((nb_col2) == (17));
      }
    }

    static void test_ignored_colums() {
      gum::learning::DBTranslatorSet                 set;
      std::vector< std::string >                     missing{"?", "N/A", "???"};
      gum::ContinuousVariable< double >              var("var", "");
      gum::learning::DBTranslator4ContinuousVariable translator(var, missing, true);
      for (std::size_t i = std::size_t(0); i < std::size_t(6); ++i)
        set.insertTranslator(translator, i);
      gum::learning::DatabaseTable database(set);
      CHECK((database.content().size()) == (static_cast< gum::Size >(0)));
      CHECK((database.variableNames().size()) == (static_cast< gum::Size >(6)));
      std::vector< std::string > names{"v0", "v1", "v2", "v3", "v4", "v5"};
      database.setVariableNames(names, true);
      const auto& dbnames = database.variableNames();
      const auto& content = database.content();
      CHECK((dbnames[0]) == ("v0"));
      CHECK((dbnames[1]) == ("v1"));
      CHECK((dbnames[2]) == ("v2"));
      CHECK((dbnames[3]) == ("v3"));

      std::vector< std::string > row{"0", "1", "2", "3", "4", "5"};
      database.insertRow(row);
      database.insertRow(row);
      CHECK((database.content().size()) == (std::size_t(2)));
      CHECK((content[0][0].cont_val) == (0.0f));
      CHECK((content[0][1].cont_val) == (1.0f));
      CHECK((content[0][2].cont_val) == (2.0f));
      CHECK((content[0][3].cont_val) == (3.0f));
      CHECK((content[1][0].cont_val) == (0.0f));
      CHECK((content[1][1].cont_val) == (1.0f));
      CHECK((content[1][2].cont_val) == (2.0f));
      CHECK((content[1][3].cont_val) == (3.0f));

      database.ignoreColumn(0, true);
      CHECK((dbnames.size()) == (std::size_t(5)));
      CHECK((dbnames[0]) == ("v1"));
      CHECK((dbnames[1]) == ("v2"));
      CHECK((dbnames[2]) == ("v3"));
      CHECK((dbnames[3]) == ("v4"));
      CHECK((database.content().size()) == (std::size_t(2)));
      CHECK((database.content()[0].size()) == (std::size_t(5)));
      CHECK((content[0][0].cont_val) == (1.0f));
      CHECK((content[0][1].cont_val) == (2.0f));
      CHECK((content[0][2].cont_val) == (3.0f));
      CHECK((content[0][3].cont_val) == (4.0f));
      CHECK((content[1][0].cont_val) == (1.0f));
      CHECK((content[1][1].cont_val) == (2.0f));
      CHECK((content[1][2].cont_val) == (3.0f));
      CHECK((content[1][3].cont_val) == (4.0f));

      database.ignoreColumn(1, false);
      CHECK((dbnames.size()) == (std::size_t(4)));
      CHECK((dbnames[0]) == ("v1"));
      CHECK((dbnames[1]) == ("v3"));
      CHECK((dbnames[2]) == ("v4"));
      CHECK((dbnames[3]) == ("v5"));
      CHECK((database.content().size()) == (std::size_t(2)));
      CHECK((database.content()[0].size()) == (std::size_t(4)));
      CHECK((content[0][0].cont_val) == (1.0f));
      CHECK((content[0][1].cont_val) == (3.0f));
      CHECK((content[0][2].cont_val) == (4.0f));
      CHECK((content[0][3].cont_val) == (5.0f));
      CHECK((content[1][0].cont_val) == (1.0f));
      CHECK((content[1][1].cont_val) == (3.0f));
      CHECK((content[1][2].cont_val) == (4.0f));
      CHECK((content[1][3].cont_val) == (5.0f));

      const auto ignored_vect = database.ignoredColumns();
      CHECK((ignored_vect.size()) == (std::size_t(3)));
      CHECK((ignored_vect[0]) == (std::size_t(0)));
      CHECK((ignored_vect[1]) == (std::size_t(2)));
      CHECK((ignored_vect[2]) == (std::size_t(6)));

      database.ignoreColumn(3, true);
      CHECK((dbnames.size()) == (std::size_t(3)));
      CHECK((dbnames[0]) == ("v1"));
      CHECK((dbnames[1]) == ("v4"));
      CHECK((dbnames[2]) == ("v5"));
      CHECK((database.content().size()) == (std::size_t(2)));
      CHECK((database.content()[0].size()) == (std::size_t(3)));
      CHECK((content[0][0].cont_val) == (1.0f));
      CHECK((content[0][1].cont_val) == (4.0f));
      CHECK((content[0][2].cont_val) == (5.0f));
      CHECK((content[1][0].cont_val) == (1.0f));
      CHECK((content[1][1].cont_val) == (4.0f));
      CHECK((content[1][2].cont_val) == (5.0f));

      database.ignoreColumn(5, true);
      CHECK((dbnames.size()) == (std::size_t(2)));
      database.ignoreColumn(1, true);
      CHECK((dbnames.size()) == (std::size_t(1)));
      CHECK((content[0][0].cont_val) == (4.0f));
      CHECK((content[1][0].cont_val) == (4.0f));
      database.ignoreColumn(4, true);
      CHECK((dbnames.size()) == (std::size_t(0)));
      CHECK((database.content().size()) == (std::size_t(0)));


      gum::learning::DatabaseTable database2;
      database2.ignoreColumn(1, true);
      database2.ignoreColumn(3, true);
      {
        const auto ignored_vect = database2.ignoredColumns();
        CHECK((ignored_vect.size()) == (std::size_t(1)));
        CHECK((ignored_vect[0]) == (std::size_t(0)));
      }

      database2.insertTranslator(translator, 5);
      {
        const auto ignored_vect = database2.ignoredColumns();
        CHECK((ignored_vect.size()) == (std::size_t(6)));
        CHECK((ignored_vect[0]) == (std::size_t(0)));
        CHECK((ignored_vect[4]) == (std::size_t(4)));
        CHECK((ignored_vect[5]) == (std::size_t(6)));
      }

      CHECK_THROWS_AS(database2.insertTranslator(translator, 1), const gum::OperationNotAllowed&);
      database2.insertTranslator(translator, 2);
      {
        const auto cols = database2.inputColumns();
        CHECK((cols[0]) == (std::size_t(5)));
        CHECK((cols[1]) == (std::size_t(2)));

        const auto ignored_vect = database2.ignoredColumns();
        CHECK((ignored_vect.size()) == (std::size_t(5)));
        CHECK((ignored_vect[0]) == (std::size_t(0)));
        CHECK((ignored_vect[1]) == (std::size_t(1)));
        CHECK((ignored_vect[2]) == (std::size_t(3)));
        CHECK((ignored_vect[3]) == (std::size_t(4)));
        CHECK((ignored_vect[4]) == (std::size_t(6)));
      }
    }

    static void test_insert_translators() {
      std::vector< std::string >   missing{"?", "N/A", "???"};
      gum::learning::DatabaseTable database;

      gum::LabelizedVariable var("var0", "", 0);
      var.addLabel("L0");
      var.addLabel("L1");
      var.addLabel("L2");

      database.insertTranslator(var, 0, missing);

      var.setName("var1");
      database.insertTranslator(var, 1, missing);

      var.setName("var2");
      database.insertTranslator(var, 2, missing);

      const auto& vnames = database.variableNames();
      CHECK((vnames.size()) == (static_cast< gum::Size >(3)));
      CHECK((vnames[0]) == ("var0"));
      CHECK((vnames[1]) == ("var1"));
      CHECK((vnames[2]) == ("var2"));

      const auto xmiss = gum::learning::DatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
          3,
          gum::learning::DBTranslatedValue{std::size_t(2)},
          1.0f);
      database.insertRow(row, xmiss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(1)));

      const auto dom1 = database.domainSizes();
      CHECK((dom1.size()) == (std::size_t(3)));
      CHECK((dom1[0]) == (std::size_t(3)));
      CHECK((dom1[1]) == (std::size_t(3)));
      CHECK((dom1[2]) == (std::size_t(3)));
      CHECK((database.domainSize(0)) == (dom1[0]));
      CHECK((database.domainSize(1)) == (dom1[1]));
      CHECK((database.domainSize(2)) == (dom1[2]));

      gum::learning::DBRow< gum::learning::DBTranslatedValue > row2(
          3,
          gum::learning::DBTranslatedValue{std::size_t(1)},
          1.0f);
      database.insertRow(row2, xmiss);
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
          3,
          gum::learning::DBTranslatedValue{std::size_t(0)},
          1.0f);
      database.insertRow(std::move(row3), xmiss);
      CHECK((database.nbRows()) == (static_cast< std::size_t >(3)));

      std::vector< gum::learning::DatabaseTable::IsMissing > is_miss(
          4,
          gum::learning::DatabaseTable::IsMissing::False);
      database.insertRows(
          std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > >(4, row),
          is_miss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(7)));
      std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > > vectx(4, row2);
      database.insertRows(vectx, is_miss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(11)));
      is_miss.resize(2);
      std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > > vecty(2, row);
      database.insertRows(std::move(vecty), is_miss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(13)));
    }

    void test_reordering() {
      std::vector< std::string >   missing{"?", "N/A", "???"};
      gum::learning::DatabaseTable database;

      gum::LabelizedVariable var("var0", "", 0);
      var.addLabel("L1");
      var.addLabel("L2");
      var.addLabel("L0");

      database.insertTranslator(var, 0, missing);

      gum::LabelizedVariable var1("var1", "", 0);
      var1.addLabel("L0");
      var1.addLabel("L1");
      var1.addLabel("L2");
      database.insertTranslator(var1, 1, missing);

      var.setName("var2");
      database.insertTranslator(var, 2, missing);

      const auto& vnames = database.variableNames();
      CHECK((vnames.size()) == (static_cast< gum::Size >(3)));
      CHECK((vnames[0]) == ("var0"));
      CHECK((vnames[1]) == ("var1"));
      CHECK((vnames[2]) == ("var2"));

      const auto xmiss = gum::learning::DatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
          3,
          gum::learning::DBTranslatedValue{std::size_t(2)},
          1.0f);
      database.insertRow(row, xmiss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(1)));

      const auto dom1 = database.domainSizes();
      CHECK((dom1.size()) == (std::size_t(3)));
      CHECK((dom1[0]) == (std::size_t(3)));
      CHECK((dom1[1]) == (std::size_t(3)));
      CHECK((dom1[2]) == (std::size_t(3)));
      CHECK((database.domainSize(0)) == (dom1[0]));
      CHECK((database.domainSize(1)) == (dom1[1]));
      CHECK((database.domainSize(2)) == (dom1[2]));

      gum::learning::DBRow< gum::learning::DBTranslatedValue > row2(
          3,
          gum::learning::DBTranslatedValue{std::size_t(1)},
          1.0f);
      database.insertRow(row2, xmiss);
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
          3,
          gum::learning::DBTranslatedValue{std::size_t(0)},
          1.0f);
      database.insertRow(std::move(row3), xmiss);
      CHECK((database.content().size()) == (static_cast< gum::Size >(3)));

      CHECK(database.needsReordering(0));
      CHECK((database.needsReordering(1)) == (false));
      CHECK(database.needsReordering(2));

      const auto& content = database.content();
      database.reorder(0);
      CHECK((content[0][0].discr_val) == (static_cast< std::size_t >(0)));
      CHECK((content[0][1].discr_val) == (static_cast< std::size_t >(2)));
      CHECK((content[0][2].discr_val) == (static_cast< std::size_t >(2)));

      CHECK((content[1][0].discr_val) == (static_cast< std::size_t >(2)));
      CHECK((content[1][1].discr_val) == (static_cast< std::size_t >(1)));
      CHECK((content[1][2].discr_val) == (static_cast< std::size_t >(1)));

      CHECK((content[2][0].discr_val) == (static_cast< std::size_t >(1)));
      CHECK((content[2][1].discr_val) == (static_cast< std::size_t >(0)));
      CHECK((content[2][2].discr_val) == (static_cast< std::size_t >(0)));

      database.reorder();
      CHECK((content[0][0].discr_val) == (static_cast< std::size_t >(0)));
      CHECK((content[0][1].discr_val) == (static_cast< std::size_t >(2)));
      CHECK((content[0][2].discr_val) == (static_cast< std::size_t >(0)));

      CHECK((content[1][0].discr_val) == (static_cast< std::size_t >(2)));
      CHECK((content[1][1].discr_val) == (static_cast< std::size_t >(1)));
      CHECK((content[1][2].discr_val) == (static_cast< std::size_t >(2)));

      CHECK((content[2][0].discr_val) == (static_cast< std::size_t >(1)));
      CHECK((content[2][1].discr_val) == (static_cast< std::size_t >(0)));
      CHECK((content[2][2].discr_val) == (static_cast< std::size_t >(1)));
    }

    void test_missing_vals() {
      std::vector< std::string >   missing{"?", "N/A", "???"};
      gum::learning::DatabaseTable database;

      gum::LabelizedVariable var("var0", "", 0);
      var.addLabel("L1");
      var.addLabel("L2");
      var.addLabel("L0");

      database.insertTranslator(var, 0, missing);

      gum::LabelizedVariable var1("var1", "", 0);
      var1.addLabel("L0");
      var1.addLabel("L1");
      var1.addLabel("L2");
      database.insertTranslator(var1, 1, missing);

      var.setName("var2");
      database.insertTranslator(var, 2, missing);

      var.setName("var3");
      database.insertTranslator(var, 3, missing);

      const auto& vnames = database.variableNames();
      CHECK((vnames.size()) == (static_cast< gum::Size >(4)));
      CHECK((vnames[0]) == ("var0"));
      CHECK((vnames[1]) == ("var1"));
      CHECK((vnames[2]) == ("var2"));
      CHECK((vnames[3]) == ("var3"));

      // row 0
      std::vector< std::string > row{"L0", "L1", "L2", "L0"};
      database.insertRow(row);

      // row 1
      row[0] = "?";
      database.insertRow(row);

      // row 2
      row[0] = "L0";
      row[1] = "?";
      database.insertRow(row);

      // row 3
      row[2] = "N/A";
      database.insertRow(row);

      // row 4
      row[0] = "???";
      database.insertRow(row);

      // row 5
      row[0] = "L0";
      row[1] = "L0";
      row[2] = "L0";
      database.insertRow(row);

      CHECK(database.hasMissingValues());

      CHECK((database.hasMissingValues(0)) == (false));
      CHECK((database.hasMissingValues(1)) == (true));
      CHECK((database.hasMissingValues(2)) == (true));
      CHECK((database.hasMissingValues(3)) == (true));
      CHECK((database.hasMissingValues(4)) == (true));
      CHECK((database.hasMissingValues(5)) == (false));

      database.ignoreColumn(1);
      CHECK(database.hasMissingValues());
      CHECK((database.hasMissingValues(0)) == (false));
      CHECK((database.hasMissingValues(1)) == (true));
      CHECK((database.hasMissingValues(2)) == (false));
      CHECK((database.hasMissingValues(3)) == (true));
      CHECK((database.hasMissingValues(4)) == (true));
      CHECK((database.hasMissingValues(5)) == (false));

      database.ignoreColumn(2);
      CHECK(database.hasMissingValues());
      CHECK((database.hasMissingValues(0)) == (false));
      CHECK((database.hasMissingValues(1)) == (true));
      CHECK((database.hasMissingValues(2)) == (false));
      CHECK((database.hasMissingValues(3)) == (false));
      CHECK((database.hasMissingValues(4)) == (true));
      CHECK((database.hasMissingValues(5)) == (false));

      database.ignoreColumn(0);
      CHECK((database.hasMissingValues()) == (false));
      CHECK((database.hasMissingValues(0)) == (false));
      CHECK((database.hasMissingValues(1)) == (false));
      CHECK((database.hasMissingValues(2)) == (false));
      CHECK((database.hasMissingValues(3)) == (false));
      CHECK((database.hasMissingValues(4)) == (false));
      CHECK((database.hasMissingValues(5)) == (false));

      database.ignoreColumn(3);
      CHECK((database.hasMissingValues()) == (false));
    }

    static void test_threads() {
      std::vector< std::string >                     missing{"?", "N/A", "???"};
      gum::ContinuousVariable< double >              var("var", "");
      gum::learning::DBTranslator4ContinuousVariable translator(var, missing, true);

      gum::learning::DatabaseTable database;
      translator.setVariableName("v1");
      database.insertTranslator(translator, 0);
      translator.setVariableName("v2");
      database.insertTranslator(translator, 1);
      translator.setVariableName("v3");
      database.insertTranslator(translator, 2);

      CHECK((database.variableNames().size()) == (static_cast< gum::Size >(3)));
      CHECK((database.nbVariables()) == (static_cast< std::size_t >(3)));
      CHECK((database.variableNames()[0]) == ("v1"));
      CHECK((database.variableNames()[1]) == ("v2"));
      CHECK((database.variableNames()[2]) == ("v3"));
      CHECK((database.variable(0).name()) == ("v1"));
      CHECK((database.variable(1).name()) == ("v2"));
      CHECK((database.variable(2).name()) == ("v3"));

      database.ignoreColumn(4);
      CHECK_THROWS_AS(database.insertTranslator(translator, 4), const gum::OperationNotAllowed&);

      const auto xmiss = gum::learning::DatabaseTable::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
          3,
          gum::learning::DBTranslatedValue{2.0f},
          1.0f);
      std::size_t xsize = 1004;
      for (std::size_t i = 0; i < xsize; ++i)
        database.insertRow(row, xmiss);
      CHECK((database.content().size()) == (xsize));
      CHECK((database.nbRows()) == (xsize));
      CHECK(!database.hasMissingValues());

      translator.setVariableName("v4");

      for (int i = 0; i < 20; ++i)
        database.insertTranslator(translator, 2, false);
      CHECK(database.hasMissingValues());
      database.setMinNbRowsPerThread(10000);
      database.insertTranslator(translator, 2, false);
    }

    static void test_changeTranslator() {
      gum::learning::DBTranslatorSet                set;
      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator(missing);
      set.insertTranslator(translator, 0);
      set.insertTranslator(translator, 1);
      set.insertTranslator(translator, 2);

      gum::learning::DatabaseTable db1(set);
      CHECK((db1.content().size()) == (static_cast< gum::Size >(0)));
      CHECK((db1.variableNames().size()) == (static_cast< gum::Size >(3)));

      db1.setVariableNames({"x0", "x1", "x2"}, false);

      db1.insertRow({"1", "3", "1"});
      db1.insertRow({"3", "3", "2"});

      gum::IntegerVariable intvar("X1", "");
      intvar.addValue(1);
      intvar.addValue(2);
      intvar.addValue(3);
      gum::learning::DBTranslator4IntegerVariable trans2(intvar);
      db1.changeTranslator(trans2, 0);

      db1.insertRow({"3", "3", "?"});
      const auto& c1 = db1.content();
      const auto& r1 = c1[0];
      const auto& r2 = c1[1];
      CHECK((r1[0].discr_val) == (static_cast< std::size_t >(0)));
      CHECK((r2[0].discr_val) == (static_cast< std::size_t >(2)));

      CHECK_THROWS_AS(db1.changeTranslator(trans2, 2), const gum::OperationNotAllowed&);

      std::vector< std::string >                  missing2{"?", "N/A", "??", "???"};
      gum::learning::DBTranslator4IntegerVariable trans3(intvar, missing2);
      CHECK_NOTHROW(db1.changeTranslator(trans3, 2));
      CHECK((r1[2].discr_val) == (static_cast< std::size_t >(0)));
      CHECK((r2[2].discr_val) == (static_cast< std::size_t >(1)));

      CHECK_NOTHROW(db1.changeTranslator(intvar, 1));
      CHECK((r1[1].discr_val) == (static_cast< std::size_t >(2)));
      CHECK((r2[1].discr_val) == (static_cast< std::size_t >(2)));


      gum::IntegerVariable intvar2("X1", "");
      intvar2.addValue(1);
      intvar2.addValue(2);
      CHECK_THROWS_AS(db1.changeTranslator(intvar2, 1), const gum::OperationNotAllowed&);

      gum::IntegerVariable intvar3("X1", "");
      intvar3.addValue(1);
      intvar3.addValue(2);
      intvar3.addValue(3);
      std::vector< std::string > missing3{"?"};
      CHECK_NOTHROW(db1.changeTranslator(intvar3, 0, true, missing3));
      CHECK_THROWS_AS(db1.changeTranslator(intvar3, 2, true, missing3),
                      const gum::OperationNotAllowed&);

      const auto better = db1.betterTranslators();
      CHECK(!better.empty());
    }

    static void testBetter() {
      gum::learning::DBTranslatorSet                set;
      std::vector< std::string >                    missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable translator1(missing);
      set.insertTranslator(translator1, 0);   // best = integer
      set.insertTranslator(translator1, 1);   // best = continuous
      set.insertTranslator(translator1, 2);   // best = range
      set.insertTranslator(translator1, 3);   // best = labelized

      gum::IntegerVariable                        var4("x4", "", {1, 2, 3, 4});
      gum::learning::DBTranslator4IntegerVariable translator4(var4, missing);
      set.insertTranslator(translator4, 4);   // best = range

      gum::IntegerVariable                        var5("x5", "", {1, 2, 4});
      gum::learning::DBTranslator4IntegerVariable translator5(var5, missing);
      set.insertTranslator(translator5, 5);   // best = integer

      gum::RangeVariable                        var6("x6", "", 0, 4);
      gum::learning::DBTranslator4RangeVariable translator6(var6, missing);
      set.insertTranslator(translator6, 6);   // best = range

      gum::learning::DatabaseTable db1(set);
      CHECK((db1.content().size()) == (static_cast< gum::Size >(0)));
      CHECK((db1.variableNames().size()) == (static_cast< gum::Size >(7)));

      db1.setVariableNames({"x0", "x1", "x2", "x3", "x4", "x5", "x6"}, false);

      db1.insertRow({"1", "2.4", "4", "3", "3", "1", "0"});
      db1.insertRow({"3", "4", "2", "a", "3", "2", "1"});
      db1.insertRow({"2", "5.5", "?", "?", "1", "4", "0"});
      db1.insertRow({"5", "???", "1", "???", "1", "?", "1"});
      db1.insertRow({"7", "?", "3", "bb", "4", "???", "1"});
      db1.insertRow({"?", "0.0", "1", "0", "4", "4", "?"});

      const auto better = db1.betterTranslators();
      CHECK(!better.empty());
      CHECK((better.size()) == (static_cast< gum::Size >(4)));

      CHECK((better[0].first) == (static_cast< std::size_t >(0)));
      CHECK((better[0].second->variable()->varType()) == (gum::VarType::INTEGER));
      CHECK((better[1].first) == (static_cast< std::size_t >(1)));
      CHECK((better[1].second->variable()->varType()) == (gum::VarType::CONTINUOUS));
      CHECK((better[2].first) == (static_cast< std::size_t >(2)));
      CHECK((better[2].second->variable()->varType()) == (gum::VarType::RANGE));
      CHECK((better[3].first) == (static_cast< std::size_t >(4)));
      CHECK((better[3].second->variable()->varType()) == (gum::VarType::RANGE));

      for (const auto& tr: better) {
        CHECK_NOTHROW(db1.changeTranslator(*tr.second, tr.first));
      }

      const auto& content = db1.content();
      const auto& row0    = content[0].row();
      const auto& row1    = content[1].row();
      const auto& row2    = content[2].row();
      const auto& row3    = content[3].row();

      // col0: integer 1,2,3,5,7
      CHECK((row0[0].discr_val) == (static_cast< std::size_t >(0)));
      CHECK((row1[0].discr_val) == (static_cast< std::size_t >(2)));
      CHECK((row2[0].discr_val) == (static_cast< std::size_t >(1)));
      CHECK((row3[0].discr_val) == (static_cast< std::size_t >(3)));

      // col1: continuous
      CHECK((row0[1].cont_val) == doctest::Approx(2.4f).epsilon(0.001));
      CHECK((row1[1].cont_val) == doctest::Approx(4.0f).epsilon(0.001));
      CHECK((row2[1].cont_val) == doctest::Approx(5.5f).epsilon(0.001));
      CHECK((row3[1].cont_val) == (std::numeric_limits< float >::max()));

      // col2: range(1-4)
      CHECK((row0[2].discr_val) == (static_cast< std::size_t >(3)));
      CHECK((row1[2].discr_val) == (static_cast< std::size_t >(1)));
      CHECK((row2[2].discr_val) == (std::numeric_limits< std::size_t >::max()));
      CHECK((row3[2].discr_val) == (static_cast< std::size_t >(0)));

      // col4: range(1-4)
      CHECK((row0[4].discr_val) == (static_cast< std::size_t >(2)));
      CHECK((row1[4].discr_val) == (static_cast< std::size_t >(2)));
      CHECK((row2[4].discr_val) == (static_cast< std::size_t >(0)));
      CHECK((row3[4].discr_val) == (static_cast< std::size_t >(0)));
    }


    private:
    static void _create_handler_(gum::learning::DatabaseTable*           database,
                                 gum::learning::DatabaseTable::Handler** handler,
                                 int*                                    nb) {
      *handler = new gum::learning::DatabaseTable::Handler(*database);

      int x = 0;
      for (const auto& row: **handler) {
        CHECK((row.size()) == (static_cast< gum::Size >(3)));
        x++;
      }

      *nb = x;
    }

    static void _create_handler_safe_(gum::learning::DatabaseTable*               database,
                                      gum::learning::DatabaseTable::HandlerSafe** handler,
                                      int*                                        nb) {
      *handler = new gum::learning::DatabaseTable::HandlerSafe(*database);

      int x = 0;
      for (const auto& row: **handler) {
        CHECK((row.size()) == (static_cast< gum::Size >(3)));
        x++;
      }

      *nb = x;
    }
  };

  GUM_TEST_ACTIF(_db1)
  GUM_TEST_ACTIF(_db2)
  GUM_TEST_ACTIF(_db3)
  GUM_TEST_ACTIF(_db4)
  GUM_TEST_ACTIF(_db5)
  GUM_TEST_ACTIF(_db6)
  GUM_TEST_ACTIF(_db_parallel)
  GUM_TEST_ACTIF(_db_parallel2)
  GUM_TEST_ACTIF(_iterators)
  GUM_TEST_ACTIF(_ignored_colums)
  GUM_TEST_ACTIF(_insert_translators)
  GUM_TEST_ACTIF(_reordering)
  GUM_TEST_ACTIF(_missing_vals)
  GUM_TEST_ACTIF(_threads)
  GUM_TEST_ACTIF(_changeTranslator)
  GUM_TEST_ACTIF(Better)

} /* namespace gum_tests */
