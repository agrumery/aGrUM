/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>
#include <ressources/include/countedAlloc.h>
#include <iostream>
#include <vector>
#include <string>
#include <ressources/include/countedAlloc.h>
#include <ressources/include/poolAlloc.h>

#include <agrum/tools/core/thread.h>
#include <agrum/tools/database/databaseTable.h>
#include <agrum/tools/database/DBTranslatorSet.h>
#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBTranslator4DiscretizedVariable.h>
#include <agrum/tools/database/DBTranslator4RangeVariable.h>
#include <agrum/tools/database/DBTranslator4ContinuousVariable.h>

namespace gum_tests {

  class DatabaseTableTestSuite: public CxxTest::TestSuite {
    public:
    void test_db1() {
      {
        gum::learning::DatabaseTable<> database1;
        TS_ASSERT_EQUALS(database1.content().size(), (gum::Size)0);
        TS_ASSERT_EQUALS(database1.variableNames().size(), (gum::Size)0);

        gum::learning::DBTranslatorSet<>                set;
        std::vector< std::string >                      missing{"?", "N/A", "???"};
        gum::learning::DBTranslator4LabelizedVariable<> translator(missing, 3);
        set.insertTranslator(translator, 1);
        set.insertTranslator(translator, 3);
        set.insertTranslator(translator, 4);

        gum::learning::DatabaseTable<> database(set);
        TS_ASSERT_EQUALS(database.content().size(), (gum::Size)0);
        TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);
        const auto dom1 = database.domainSizes();
        TS_ASSERT_EQUALS(dom1.size(), std::size_t(3));
        TS_ASSERT_EQUALS(dom1[0], std::size_t(0));
        TS_ASSERT_EQUALS(dom1[1], std::size_t(0));
        TS_ASSERT_EQUALS(dom1[2], std::size_t(0));

        database.setVariableNames({"v1", "v2", "v3"}, false);
        TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)3);
        TS_ASSERT_EQUALS(database.variableNames()[0], "v1");
        TS_ASSERT_EQUALS(database.variableNames()[1], "v2");
        TS_ASSERT_EQUALS(database.variableNames()[2], "v3");
        database.setVariableNames({"x1", "x2", "x3", "x4", "x5"}, true);
        TS_ASSERT_EQUALS(database.variableNames()[0], "x2");
        TS_ASSERT_EQUALS(database.variableNames()[1], "x4");
        TS_ASSERT_EQUALS(database.variableNames()[2], "x5");
        TS_ASSERT_THROWS(database.setVariableNames({"x1", "x2", "x3", "x4"}, false),
                         gum::SizeError);
        TS_ASSERT_THROWS(database.setVariableNames({"x1", "x2"}, true), gum::SizeError);
        TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)3);

        std::vector< std::string > row(7, "toto");
        database.insertRow(row);
        TS_ASSERT_EQUALS(database.content().size(), (gum::Size)1);
        database.insertRow(row);
        TS_ASSERT_EQUALS(database.content().size(), (gum::Size)2);
        std::vector< std::string, DebugCountedAlloc< std::string > > row2(3, "toto");
        TS_ASSERT_THROWS(database.insertRow(row2), gum::SizeError);

        gum::learning::DatabaseTable<> db(database);
        TS_ASSERT_EQUALS(db.variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(db.nbVariables(), (std::size_t)3);
        TS_ASSERT_EQUALS(db.content().size(), (gum::Size)2);

        gum::learning::DatabaseTable<> db2(db,
                                           std::allocator< gum::learning::DBTranslatedValue >());
        TS_ASSERT_EQUALS(db2.variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(db2.nbVariables(), (std::size_t)3);
        TS_ASSERT_EQUALS(db2.content().size(), (gum::Size)2);

        gum::learning::DatabaseTable<> db3(std::move(db2));
        TS_ASSERT_EQUALS(db3.variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(db3.nbVariables(), (std::size_t)3);
        TS_ASSERT_EQUALS(db3.content().size(), (gum::Size)2);

        database.insertRow(row);
        gum::learning::DatabaseTable<> db4(std::move(database),
                                           std::allocator< gum::learning::DBTranslatedValue >());
        TS_ASSERT_EQUALS(db4.variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(db4.nbVariables(), (std::size_t)3);
        TS_ASSERT_EQUALS(db4.content().size(), (gum::Size)3);

        TS_ASSERT_EQUALS(db.content().size(), (gum::Size)2);
        db = db4;
        TS_ASSERT_EQUALS(db.content().size(), (gum::Size)3);

        db2 = std::move(db4);
        TS_ASSERT_EQUALS(db2.variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(db2.nbVariables(), (std::size_t)3);
        TS_ASSERT_EQUALS(db2.content().size(), (gum::Size)3);

        gum::learning::DatabaseTable<>* db5 = db.clone();
        TS_ASSERT_EQUALS(db5->variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(db5->nbVariables(), (std::size_t)3);
        TS_ASSERT_EQUALS(db5->content().size(), (gum::Size)3);
        delete db5;

        gum::learning::DatabaseTable<>* db6
           = db.clone(std::allocator< gum::learning::DBTranslatedValue >());
        TS_ASSERT_EQUALS(db6->variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(db6->nbVariables(), (std::size_t)3);
        TS_ASSERT_EQUALS(db6->content().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(db6->size(), (gum::Size)3);

        delete db6;

        for (const auto& row: db) {
          TS_ASSERT_EQUALS(row.weight(), 1);
        }

        db.setAllRowsWeight(10.0);
        for (const auto& row: db) {
          TS_ASSERT_EQUALS(row.weight(), 10.0);
        }

        TS_ASSERT_EQUALS(db.weight(), 10.0 * db.size());

        const std::size_t nbr = db.nbRows();
        for (std::size_t i = std::size_t(0); i < nbr; ++i) {
          if (i % 2) db.setWeight(i, 2.0);
        }

        std::size_t index = std::size_t(0);
        for (const auto& row: db) {
          if (index % 2) {
            TS_ASSERT_EQUALS(row.weight(), 2.0);
            TS_ASSERT_EQUALS(db.weight(index), 2.0);
          } else {
            TS_ASSERT_EQUALS(row.weight(), 10.0);
            TS_ASSERT_EQUALS(db.weight(index), 10.0);
          }
          ++index;
        }
      }

      TS_ASSERT_EQUALS(CountedAlloc::hasMemoryLeak(), false);
    }

    void test_db2() {
      gum::learning::DBTranslatorSet<> set;
      std::vector< std::string >       missing{"?", "N/A", "???"};
      gum::LabelizedVariable           var("var", "", 0);
      var.addLabel("L0");
      var.addLabel("L1");
      var.addLabel("L2");
      gum::learning::DBTranslator4LabelizedVariable<> translator(var, missing, true, 3);
      set.insertTranslator(translator, 0);
      set.insertTranslator(translator, 1);
      set.insertTranslator(translator, 2);
      set[0].setVariableName("v1");
      set[1].setVariableName("v2");
      set[2].setVariableName("v3");

      gum::learning::DatabaseTable<> database(set);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);
      TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)3);

      const auto xmiss = gum::learning::DatabaseTable<>::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
         3,
         gum::learning::DBTranslatedValue{std::size_t(1)},
         1.0);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.size(), (gum::Size)1);

      typename gum::learning::DatabaseTable<>::HandlerSafe handler(database);
      typename gum::learning::DatabaseTable<>::Handler     uhandler(database);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(std::move(row), xmiss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)6);

      TS_ASSERT_EQUALS(handler.range().second, (std::size_t)6);
      TS_ASSERT_EQUALS(handler.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(handler.DBSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(uhandler.size(), (gum::Size)1);
      TS_ASSERT_EQUALS(uhandler.DBSize(), (gum::Size)6);

      handler.setRange(1, 4);
      TS_ASSERT_EQUALS(handler.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(handler.DBSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(handler.numRow(), (std::size_t)0);
      uhandler.setRange(1, 4);
      TS_ASSERT_EQUALS(uhandler.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(uhandler.DBSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(uhandler.numRow(), (std::size_t)0);
      handler.nextRow();
      TS_ASSERT_EQUALS(handler.numRow(), (std::size_t)1);

      auto handler2(handler);
      TS_ASSERT_EQUALS(handler2.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(handler2.DBSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(handler2.range().second, (std::size_t)4);
      TS_ASSERT_EQUALS(handler2.numRow(), (std::size_t)1);
      handler2.nextRow();
      TS_ASSERT_EQUALS(handler2.numRow(), (std::size_t)2);

      auto uhandler2(uhandler);
      TS_ASSERT_EQUALS(uhandler2.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(uhandler2.DBSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(uhandler2.range().second, (std::size_t)4);
      TS_ASSERT_EQUALS(uhandler2.numRow(), (std::size_t)0);
      uhandler2.nextRow();
      TS_ASSERT_EQUALS(uhandler2.numRow(), (std::size_t)1);

      auto handler3(std::move(handler2));
      TS_ASSERT_EQUALS(handler3.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(handler3.DBSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(handler3.range().first, (std::size_t)1);
      TS_ASSERT_EQUALS(handler3.range().second, (std::size_t)4);
      TS_ASSERT_EQUALS(handler3.numRow(), (std::size_t)2);

      auto uhandler3(std::move(uhandler2));
      TS_ASSERT_EQUALS(uhandler3.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(uhandler3.DBSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(uhandler3.range().first, (std::size_t)1);
      TS_ASSERT_EQUALS(uhandler3.range().second, (std::size_t)4);
      TS_ASSERT_EQUALS(uhandler3.numRow(), (std::size_t)1);

      handler2 = handler;
      TS_ASSERT_EQUALS(handler.range().second, (std::size_t)4);
      TS_ASSERT_EQUALS(handler.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(handler.DBSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(handler.numRow(), (std::size_t)1);

      uhandler2 = uhandler;
      TS_ASSERT_EQUALS(uhandler.range().second, (std::size_t)4);
      TS_ASSERT_EQUALS(uhandler.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(uhandler.DBSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(uhandler.numRow(), (std::size_t)0);

      handler2 = std::move(handler3);
      TS_ASSERT_EQUALS(handler2.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(handler2.DBSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(handler2.range().second, (std::size_t)4);
      TS_ASSERT_EQUALS(handler2.numRow(), (std::size_t)2);

      uhandler2 = std::move(uhandler3);
      TS_ASSERT_EQUALS(uhandler2.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(uhandler2.DBSize(), (gum::Size)6);
      TS_ASSERT_EQUALS(uhandler2.range().second, (std::size_t)4);
      TS_ASSERT_EQUALS(uhandler2.numRow(), (std::size_t)1);

      auto&       row2 = handler2.rowSafe();
      const auto& xrow = row2.row();
      TS_ASSERT_EQUALS(xrow.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(xrow[1].discr_val, std::size_t(1));

      TS_ASSERT_EQUALS(handler2.hasRows(), true);
      handler2.nextRow();
      TS_ASSERT_EQUALS(handler2.hasRows(), false);
      handler2.reset();
      TS_ASSERT_EQUALS(handler2.hasRows(), true);
      TS_ASSERT_EQUALS(handler2.numRow(), (std::size_t)0);

      const auto& vars = handler2.variableNames();
      TS_ASSERT_EQUALS(vars[1], "v2");
      TS_ASSERT_EQUALS(handler2.nbVariables(), (std::size_t)3);

      uhandler2.nextRow();
      auto&       urow2 = uhandler2.rowSafe();
      const auto& uxrow = urow2.row();
      TS_ASSERT_EQUALS(uxrow.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(uxrow[1].discr_val, std::size_t(1));

      TS_ASSERT_EQUALS(uhandler2.hasRows(), true);
      uhandler2.nextRow();
      TS_ASSERT_EQUALS(uhandler2.hasRows(), false);
      uhandler2.reset();
      TS_ASSERT_EQUALS(uhandler2.hasRows(), true);
      TS_ASSERT_EQUALS(uhandler2.numRow(), (std::size_t)0);

      const auto& uvars = uhandler2.variableNames();
      TS_ASSERT_EQUALS(uvars[1], "v2");
      TS_ASSERT_EQUALS(uhandler2.nbVariables(), (std::size_t)3);

      // check that we cannot convert an unsafe handler into a safe one
      TS_ASSERT_THROWS(handler2 = uhandler2, std::bad_cast&);
      gum::learning::DatabaseTable<>::Handler& uxhandler2 = handler2;
      TS_ASSERT_THROWS(uxhandler2 = uhandler2, std::bad_cast&);
    }

    void test_db3() {
      gum::learning::DBTranslatorSet<> set;
      std::vector< std::string >       missing{"?", "N/A", "???"};
      gum::LabelizedVariable           var("var", "", 0);
      var.addLabel("L0");
      var.addLabel("L1");
      var.addLabel("L2");
      gum::learning::DBTranslator4LabelizedVariable<> translator(var, missing, true, 3);
      set.insertTranslator(translator, 0);
      set.insertTranslator(translator, 1);
      set.insertTranslator(translator, 2);
      set[0].setVariableName("v1");
      set[1].setVariableName("v2");
      set[2].setVariableName("v3");

      gum::learning::DatabaseTable<> database(set);
      TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);
      TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)3);

      const auto xmiss = gum::learning::DatabaseTable<>::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
         3,
         gum::learning::DBTranslatedValue{std::size_t(2)},
         1.0f);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)1);

      const auto dom1 = database.domainSizes();
      TS_ASSERT_EQUALS(dom1.size(), std::size_t(3));
      TS_ASSERT_EQUALS(dom1[0], std::size_t(3));
      TS_ASSERT_EQUALS(dom1[1], std::size_t(3));
      TS_ASSERT_EQUALS(dom1[2], std::size_t(3));
      TS_ASSERT_EQUALS(database.domainSize(0), dom1[0]);
      TS_ASSERT_EQUALS(database.domainSize(1), dom1[1]);
      TS_ASSERT_EQUALS(database.domainSize(2), dom1[2]);

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
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)3);

      std::vector< gum::learning::DatabaseTable<>::IsMissing > is_miss(
         4,
         gum::learning::DatabaseTable<>::IsMissing::False);
      database.insertRows(
         std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > >(4, row),
         is_miss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)7);
      std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > > vectx(4, row2);
      database.insertRows(vectx, is_miss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)11);
      std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > > vecty(2, row);
      is_miss.resize(2);
      database.insertRows(std::move(vecty), is_miss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)13);

      database.eraseRow(12);
      database.eraseRow(5);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)11);
      database.eraseFirstRow();
      database.eraseLastRow();
      database.eraseFirstRows(2);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)7);
      database.eraseLastRows(1);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)6);
      database.eraseRows(2, 4);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)4);
      database.eraseAllRows();
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)0);

      {
        std::vector< std::string > new_names{"x1", "x2", "x3"};
        TS_ASSERT_EQUALS(database.variableNames()[1], "v2");
        database.setVariableNames(new_names);
        TS_ASSERT_EQUALS(database.variableNames()[1], "x2");
        std::vector< std::string, DebugCountedAlloc< std::string > > names2{"y1", "y2", "y3"};
        database.setVariableNames(names2);
        TS_ASSERT_EQUALS(database.variableNames()[1], "y2");

        database.clear();
        TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)0);
      }

      TS_ASSERT_EQUALS(CountedAlloc::hasMemoryLeak(), false);
    }


    void test_db4() {
      gum::learning::DBTranslatorSet<>                 set;
      std::vector< std::string >                       missing{"?", "N/A", "???"};
      gum::ContinuousVariable< double >                var("var", "");
      gum::learning::DBTranslator4ContinuousVariable<> translator(var, missing, true);
      set.insertTranslator(translator, 0);
      set.insertTranslator(translator, 1);
      set.insertTranslator(translator, 2);
      set[0].setVariableName("v1");
      set[1].setVariableName("v2");
      set[2].setVariableName("v3");

      gum::learning::DatabaseTable<> database(set);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);
      TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)3);

      const auto xmiss = gum::learning::DatabaseTable<>::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
         3,
         gum::learning::DBTranslatedValue{2.0f},
         1.0f);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)4);

      gum::learning::DatabaseTable<> database2 = database;
      TS_ASSERT_EQUALS(database2.content().size(), (gum::Size)4);
      TS_ASSERT_EQUALS(database2.variableNames().size(), (gum::Size)3);
      TS_ASSERT_EQUALS(database2.nbVariables(), (std::size_t)3);

      gum::learning::DBRow< gum::learning::DBTranslatedValue > row2(
         2,
         gum::learning::DBTranslatedValue{2.0f},
         1.0f);
      TS_ASSERT_THROWS(database2.insertRow(row2, xmiss), gum::SizeError);

      gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
         3,
         gum::learning::DBTranslatedValue{4.0f},
         1.0f);
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row4(
         3,
         gum::learning::DBTranslatedValue{5.0f},
         1.0f);
      std::vector< gum::learning::DatabaseTable<>::IsMissing > is_miss(
         2,
         gum::learning::DatabaseTable<>::IsMissing::False);
      database2.insertRows({row3, row4}, is_miss);
      auto handler2 = database2.beginSafe();
      auto handler3 = database2.endSafe();
      handler2.setRange(4, 6);
      handler3.setRange(5, 6);
      TS_ASSERT_EQUALS(handler2.row()[0].cont_val, 4.0f);
      TS_ASSERT_EQUALS(handler3.row()[0].cont_val, 5.0f);
      database2.eraseLastRow();
      TS_ASSERT_EQUALS(handler2.row()[0].cont_val, 4.0f);
      TS_ASSERT_THROWS(handler3.rowSafe()[0], gum::OutOfBounds);
      database2.insertRow(row4, xmiss);

      auto uhandler2 = database2.begin();
      auto uhandler3 = database2.end();
      uhandler2.setRange(4, 6);
      uhandler3.setRange(5, 6);
      TS_ASSERT_EQUALS(uhandler2.row()[0].cont_val, 4.0f);
      TS_ASSERT_EQUALS(uhandler3.row()[0].cont_val, 5.0f);
      database2.eraseLastRow();
      TS_ASSERT_EQUALS(uhandler2.row()[0].cont_val, 4.0f);
      TS_ASSERT_EQUALS(uhandler3.size(), (gum::Size)1);

      gum::learning::DatabaseTable<> database3(std::move(database2));
      database2 = std::move(database);
      TS_ASSERT_EQUALS(database2.content().size(), (gum::Size)4);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)0);
      database = std::move(database3);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)5);

      handler2 = database.handlerSafe();
      TS_ASSERT_EQUALS(handler2.range().second, (std::size_t)5);
      handler2.nextRow();
      handler2.nextRow();
      handler2.nextRow();
      handler3 = std::move(handler2);
      TS_ASSERT_EQUALS(handler3.range().second, (std::size_t)5);
      TS_ASSERT(handler3.hasRows());
      handler3.nextRow();
      TS_ASSERT(handler3.hasRows());
      handler3.nextRow();
      TS_ASSERT_EQUALS(handler3.hasRows(), false);
    }


    void test_db5() {
      {
        gum::learning::DBTranslatorSet<>                 set;
        std::vector< std::string >                       missing{"?", "N/A", "???"};
        gum::ContinuousVariable< double >                var("var", "");
        gum::learning::DBTranslator4ContinuousVariable<> translator(var, missing, true);
        set.insertTranslator(translator, 0);
        set.insertTranslator(translator, 1);
        set.insertTranslator(translator, 2);
        set[0].setVariableName("v1");
        set[1].setVariableName("v2");
        set[2].setVariableName("v3");
        gum::learning::DatabaseTable<> database(set);

        std::vector< std::string, DebugCountedAlloc< std::string > > vect{"v1", "v2", "v3"};
        database.setVariableNames(vect, false);
        TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)3);

        const auto xmiss = gum::learning::DatabaseTable<>::IsMissing::False;
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
           3,
           gum::learning::DBTranslatedValue{2.0f},
           1.0f);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        TS_ASSERT_EQUALS(database.content().size(), (gum::Size)4);

        gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
           3,
           gum::learning::DBTranslatedValue{4.0f},
           1.0f);
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row4(
           3,
           gum::learning::DBTranslatedValue{5.0f},
           1.0f);
        std::vector< gum::learning::DatabaseTable<>::IsMissing > is_miss(
           2,
           gum::learning::DatabaseTable<>::IsMissing::False);
        database.insertRows({row3, row4}, is_miss);
        typename gum::learning::DatabaseTable<>::Handler handler(database);

        int x = 0;
        for (const auto& row: handler) {
          x++;
          TS_ASSERT_EQUALS(row.size(), (gum::Size)3);
        }
        TS_ASSERT_EQUALS(x, 6);

        handler.setRange(1, 3);
        x = 0;
        for (const auto& row: handler) {
          x++;
          TS_ASSERT_EQUALS(row.size(), (gum::Size)3);
        }
        TS_ASSERT_EQUALS(x, 2);
      }

      TS_ASSERT_EQUALS(CountedAlloc::hasMemoryLeak(), false);
    }

    void test_db6() {
      std::vector< std::string >                       missing{"?", "N/A", "???"};
      gum::ContinuousVariable< double >                var("var", "");
      gum::learning::DBTranslator4ContinuousVariable<> translator(var, missing, true);

      gum::learning::DatabaseTable<> database;
      translator.setVariableName("v1");
      database.insertTranslator(translator, 0);
      translator.setVariableName("v2");
      database.insertTranslator(translator, 1);
      translator.setVariableName("v3");
      database.insertTranslator(translator, 2);

      TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);
      TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)3);
      TS_ASSERT_EQUALS(database.variableNames()[0], "v1");
      TS_ASSERT_EQUALS(database.variableNames()[1], "v2");
      TS_ASSERT_EQUALS(database.variableNames()[2], "v3");
      TS_ASSERT_EQUALS(database.variable(0).name(), "v1");
      TS_ASSERT_EQUALS(database.variable(1).name(), "v2");
      TS_ASSERT_EQUALS(database.variable(2).name(), "v3");

      database.ignoreColumn(4);
      TS_ASSERT_THROWS(database.insertTranslator(translator, 4), gum::OperationNotAllowed);

      const auto xmiss = gum::learning::DatabaseTable<>::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
         3,
         gum::learning::DBTranslatedValue{2.0f},
         1.0f);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)4);
      TS_ASSERT_EQUALS(database.nbRows(), (std::size_t)4);
      TS_ASSERT(!database.hasMissingValues());

      TS_GUM_ASSERT_THROWS_NOTHING(database.insertTranslator(translator, 5));
      TS_GUM_ASSERT_THROWS_NOTHING(database.insertTranslator(translator, 5, false));
      TS_ASSERT(database.hasMissingValues());
      TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)(5));
      database.eraseTranslators(5, true);
      TS_ASSERT(!database.hasMissingValues());
      TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)(3));
      TS_ASSERT_EQUALS(database.content()[0].size(), (gum::Size)3);

      TS_GUM_ASSERT_THROWS_NOTHING(database.eraseTranslators(10));
      database.eraseTranslators(1);
      TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)2);
      TS_ASSERT_EQUALS(database.variableNames()[0], "v1");
      TS_ASSERT_EQUALS(database.variableNames()[1], "v3");
      TS_ASSERT_EQUALS(database.variable(0).name(), "v1");
      TS_ASSERT_EQUALS(database.variable(1).name(), "v3");
      TS_ASSERT_EQUALS(database.content()[0].size(), (gum::Size)2);
      TS_ASSERT_EQUALS(database.content()[1].size(), (gum::Size)2);
      TS_ASSERT_EQUALS(database.content()[2].size(), (gum::Size)2);
      TS_ASSERT_EQUALS(database.content()[3].size(), (gum::Size)2);

      std::vector< std::string > row2(4, "3.0");
      database.insertRow(row2);
      database.insertRow(row2);

      gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
         2,
         gum::learning::DBTranslatedValue{4.0f},
         1.0f);
      database.insertRow(row3, xmiss);
      database.insertRow(row3, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)8);
    }

    void test_db_parallel() {
      {
        gum::learning::DBTranslatorSet<>                 set;
        std::vector< std::string >                       missing{"?", "N/A", "???"};
        gum::ContinuousVariable< double >                var("var", "");
        gum::learning::DBTranslator4ContinuousVariable<> translator(var, missing, true);
        set.insertTranslator(translator, 0);
        set.insertTranslator(translator, 1);
        set.insertTranslator(translator, 2);
        set[0].setVariableName("v1");
        set[1].setVariableName("v2");
        set[2].setVariableName("v3");
        gum::learning::DatabaseTable<> database(set);
        TS_ASSERT_EQUALS(database.content().size(), (gum::Size)0);
        TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);

        std::vector< std::string, DebugCountedAlloc< std::string > > vect{"v1", "v2", "v3"};
        database.setVariableNames(vect);
        TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)3);

        const auto xmiss = gum::learning::DatabaseTable<>::IsMissing::False;
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
           3,
           gum::learning::DBTranslatedValue{2.0f},
           1.0f);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        TS_ASSERT_EQUALS(database.content().size(), (gum::Size)4);

        gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
           3,
           gum::learning::DBTranslatedValue{4.0f},
           1.0f);
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row4(
           3,
           gum::learning::DBTranslatedValue{5.0f},
           1.0f);
        std::vector< gum::learning::DatabaseTable<>::IsMissing > is_miss(
           2,
           gum::learning::DatabaseTable<>::IsMissing::False);
        database.insertRows({row3, row4}, is_miss);

        const unsigned int num_threads = gum::thread::getMaxNumberOfThreads();

        std::vector< gum::learning::DatabaseTable<>::Handler* > handlers(num_threads);
        std::vector< std::thread* >                             threads(num_threads);
        std::vector< int >                                      nb(num_threads);

        for (unsigned int i = 0; i < num_threads; ++i) {
          threads[i] = new std::thread(&DatabaseTableTestSuite::_create_handler_,
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
          TS_ASSERT_EQUALS(nb[i], 6);
        }
      }

      TS_ASSERT_EQUALS(CountedAlloc::hasMemoryLeak(), false);
    }

    void test_db_parallel2() {
      {
        gum::learning::DBTranslatorSet<>                 set;
        std::vector< std::string >                       missing{"?", "N/A", "???"};
        gum::ContinuousVariable< double >                var("var", "");
        gum::learning::DBTranslator4ContinuousVariable<> translator(var, missing, true);
        set.insertTranslator(translator, 0);
        set.insertTranslator(translator, 1);
        set.insertTranslator(translator, 2);
        set[0].setVariableName("v1");
        set[1].setVariableName("v2");
        set[2].setVariableName("v3");
        gum::learning::DatabaseTable<> database(set);
        TS_ASSERT_EQUALS(database.content().size(), (gum::Size)0);
        TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);

        std::vector< std::string, DebugCountedAlloc< std::string > > vect{"v1", "v2", "v3"};
        database.setVariableNames(vect);
        TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)3);

        const auto xmiss = gum::learning::DatabaseTable<>::IsMissing::False;
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
           3,
           gum::learning::DBTranslatedValue{2.0f},
           1.0f);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        TS_ASSERT_EQUALS(database.content().size(), (gum::Size)4);

        gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
           3,
           gum::learning::DBTranslatedValue{4.0f},
           1.0f);
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row4(
           3,
           gum::learning::DBTranslatedValue{5.0f},
           1.0f);
        std::vector< gum::learning::DatabaseTable<>::IsMissing > is_miss(
           2,
           gum::learning::DatabaseTable<>::IsMissing::False);
        database.insertRows({row3, row4}, is_miss);

        const unsigned int num_threads = gum::thread::getMaxNumberOfThreads();

        std::vector< gum::learning::DatabaseTable<>::HandlerSafe* > handlers(num_threads);
        std::vector< std::thread* >                                 threads(num_threads);
        std::vector< int >                                          nb(num_threads);

        for (unsigned int i = 0; i < num_threads; ++i) {
          threads[i] = new std::thread(&DatabaseTableTestSuite::_create_handler_safe_,
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
          TS_ASSERT_EQUALS(nb[i], 6);
        }
      }

      TS_ASSERT_EQUALS(CountedAlloc::hasMemoryLeak(), false);
    }

    void test_iterators() {
      {
        gum::learning::DBTranslatorSet<>                 set;
        std::vector< std::string >                       missing{"?", "N/A", "???"};
        gum::ContinuousVariable< double >                var("var", "");
        gum::learning::DBTranslator4ContinuousVariable<> translator(var, missing, true);
        set.insertTranslator(translator, 0);
        set.insertTranslator(translator, 1);
        set.insertTranslator(translator, 2);
        set[0].setVariableName("v1");
        set[1].setVariableName("v2");
        set[2].setVariableName("v3");
        gum::learning::DatabaseTable<> database(set);
        TS_ASSERT_EQUALS(database.content().size(), (gum::Size)0);
        TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);

        std::vector< std::string, DebugCountedAlloc< std::string > > vect{"v1", "v2", "v3"};
        database.setVariableNames(vect);
        TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);
        TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)3);

        const auto xmiss = gum::learning::DatabaseTable<>::IsMissing::False;
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
           3,
           gum::learning::DBTranslatedValue{2.0f},
           1.0f);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        database.insertRow(row, xmiss);
        TS_ASSERT_EQUALS(database.content().size(), (gum::Size)4);

        int nb_col1 = 0, nb_col2 = 0;
        for (const auto& row: database) {
          const auto& r = row.row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        TS_ASSERT_EQUALS(nb_col1, 8);
        TS_ASSERT_EQUALS(nb_col2, 8);

        nb_col1 = 0;
        nb_col2 = 0;
        for (auto iter = database.begin(); iter != database.end(); ++iter) {
          const auto& r = iter->row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        TS_ASSERT_EQUALS(nb_col1, 8);
        TS_ASSERT_EQUALS(nb_col2, 8);

        nb_col1 = 0;
        nb_col2 = 0;
        for (auto iter = database.beginSafe(); iter != database.endSafe(); ++iter) {
          const auto& r = (*iter).row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        TS_ASSERT_EQUALS(nb_col1, 8);
        TS_ASSERT_EQUALS(nb_col2, 8);

        nb_col1 = 0;
        nb_col2 = 0;
        for (auto iter = database.beginSafe(); iter != database.endSafe(); ++iter) {
          const auto& r = iter->row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        TS_ASSERT_EQUALS(nb_col1, 8);
        TS_ASSERT_EQUALS(nb_col2, 8);

        gum::learning::DBRow< gum::learning::DBTranslatedValue > row3(
           3,
           gum::learning::DBTranslatedValue{4.0f},
           1.0f);
        gum::learning::DBRow< gum::learning::DBTranslatedValue > row4(
           3,
           gum::learning::DBTranslatedValue{5.0f},
           1.0f);
        std::vector< gum::learning::DatabaseTable<>::IsMissing > is_miss(
           2,
           gum::learning::DatabaseTable<>::IsMissing::False);
        database.insertRows({row3, row4}, is_miss);

        nb_col1 = 0;
        nb_col2 = 0;
        for (const auto& row: database) {
          const auto& r = row.row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        TS_ASSERT_EQUALS(nb_col1, 17);
        TS_ASSERT_EQUALS(nb_col2, 17);

        nb_col1 = 0;
        nb_col2 = 0;
        for (auto iter = database.beginSafe(); iter != database.endSafe(); ++iter) {
          const auto& r = (*iter).row();
          nb_col1 += (int)r[0].cont_val;
          nb_col2 += (int)r[1].cont_val;
        }
        TS_ASSERT_EQUALS(nb_col1, 17);
        TS_ASSERT_EQUALS(nb_col2, 17);
      }

      TS_ASSERT_EQUALS(CountedAlloc::hasMemoryLeak(), false);
    }

    void test_ignored_colums() {
      gum::learning::DBTranslatorSet<>                 set;
      std::vector< std::string >                       missing{"?", "N/A", "???"};
      gum::ContinuousVariable< double >                var("var", "");
      gum::learning::DBTranslator4ContinuousVariable<> translator(var, missing, true);
      for (std::size_t i = std::size_t(0); i < std::size_t(6); ++i)
        set.insertTranslator(translator, i);
      gum::learning::DatabaseTable<> database(set);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)6);
      std::vector< std::string > names{"v0", "v1", "v2", "v3", "v4", "v5"};
      database.setVariableNames(names, true);
      const auto& dbnames = database.variableNames();
      const auto& content = database.content();
      TS_ASSERT_EQUALS(dbnames[0], "v0");
      TS_ASSERT_EQUALS(dbnames[1], "v1");
      TS_ASSERT_EQUALS(dbnames[2], "v2");
      TS_ASSERT_EQUALS(dbnames[3], "v3");

      std::vector< std::string > row{"0", "1", "2", "3", "4", "5"};
      database.insertRow(row);
      database.insertRow(row);
      TS_ASSERT_EQUALS(database.content().size(), std::size_t(2));
      TS_ASSERT_EQUALS(content[0][0].cont_val, 0.0f);
      TS_ASSERT_EQUALS(content[0][1].cont_val, 1.0f);
      TS_ASSERT_EQUALS(content[0][2].cont_val, 2.0f);
      TS_ASSERT_EQUALS(content[0][3].cont_val, 3.0f);
      TS_ASSERT_EQUALS(content[1][0].cont_val, 0.0f);
      TS_ASSERT_EQUALS(content[1][1].cont_val, 1.0f);
      TS_ASSERT_EQUALS(content[1][2].cont_val, 2.0f);
      TS_ASSERT_EQUALS(content[1][3].cont_val, 3.0f);

      database.ignoreColumn(0, true);
      TS_ASSERT_EQUALS(dbnames.size(), std::size_t(5));
      TS_ASSERT_EQUALS(dbnames[0], "v1");
      TS_ASSERT_EQUALS(dbnames[1], "v2");
      TS_ASSERT_EQUALS(dbnames[2], "v3");
      TS_ASSERT_EQUALS(dbnames[3], "v4");
      TS_ASSERT_EQUALS(database.content().size(), std::size_t(2));
      TS_ASSERT_EQUALS(database.content()[0].size(), std::size_t(5));
      TS_ASSERT_EQUALS(content[0][0].cont_val, 1.0f);
      TS_ASSERT_EQUALS(content[0][1].cont_val, 2.0f);
      TS_ASSERT_EQUALS(content[0][2].cont_val, 3.0f);
      TS_ASSERT_EQUALS(content[0][3].cont_val, 4.0f);
      TS_ASSERT_EQUALS(content[1][0].cont_val, 1.0f);
      TS_ASSERT_EQUALS(content[1][1].cont_val, 2.0f);
      TS_ASSERT_EQUALS(content[1][2].cont_val, 3.0f);
      TS_ASSERT_EQUALS(content[1][3].cont_val, 4.0f);

      database.ignoreColumn(1, false);
      TS_ASSERT_EQUALS(dbnames.size(), std::size_t(4));
      TS_ASSERT_EQUALS(dbnames[0], "v1");
      TS_ASSERT_EQUALS(dbnames[1], "v3");
      TS_ASSERT_EQUALS(dbnames[2], "v4");
      TS_ASSERT_EQUALS(dbnames[3], "v5");
      TS_ASSERT_EQUALS(database.content().size(), std::size_t(2));
      TS_ASSERT_EQUALS(database.content()[0].size(), std::size_t(4));
      TS_ASSERT_EQUALS(content[0][0].cont_val, 1.0f);
      TS_ASSERT_EQUALS(content[0][1].cont_val, 3.0f);
      TS_ASSERT_EQUALS(content[0][2].cont_val, 4.0f);
      TS_ASSERT_EQUALS(content[0][3].cont_val, 5.0f);
      TS_ASSERT_EQUALS(content[1][0].cont_val, 1.0f);
      TS_ASSERT_EQUALS(content[1][1].cont_val, 3.0f);
      TS_ASSERT_EQUALS(content[1][2].cont_val, 4.0f);
      TS_ASSERT_EQUALS(content[1][3].cont_val, 5.0f);

      const auto ignored_vect = database.ignoredColumns();
      TS_ASSERT_EQUALS(ignored_vect.size(), std::size_t(3));
      TS_ASSERT_EQUALS(ignored_vect[0], std::size_t(0));
      TS_ASSERT_EQUALS(ignored_vect[1], std::size_t(2));
      TS_ASSERT_EQUALS(ignored_vect[2], std::size_t(6));

      database.ignoreColumn(3, true);
      TS_ASSERT_EQUALS(dbnames.size(), std::size_t(3));
      TS_ASSERT_EQUALS(dbnames[0], "v1");
      TS_ASSERT_EQUALS(dbnames[1], "v4");
      TS_ASSERT_EQUALS(dbnames[2], "v5");
      TS_ASSERT_EQUALS(database.content().size(), std::size_t(2));
      TS_ASSERT_EQUALS(database.content()[0].size(), std::size_t(3));
      TS_ASSERT_EQUALS(content[0][0].cont_val, 1.0f);
      TS_ASSERT_EQUALS(content[0][1].cont_val, 4.0f);
      TS_ASSERT_EQUALS(content[0][2].cont_val, 5.0f);
      TS_ASSERT_EQUALS(content[1][0].cont_val, 1.0f);
      TS_ASSERT_EQUALS(content[1][1].cont_val, 4.0f);
      TS_ASSERT_EQUALS(content[1][2].cont_val, 5.0f);

      database.ignoreColumn(5, true);
      TS_ASSERT_EQUALS(dbnames.size(), std::size_t(2));
      database.ignoreColumn(1, true);
      TS_ASSERT_EQUALS(dbnames.size(), std::size_t(1));
      TS_ASSERT_EQUALS(content[0][0].cont_val, 4.0f);
      TS_ASSERT_EQUALS(content[1][0].cont_val, 4.0f);
      database.ignoreColumn(4, true);
      TS_ASSERT_EQUALS(dbnames.size(), std::size_t(0));
      TS_ASSERT_EQUALS(database.content().size(), std::size_t(0));


      gum::learning::DatabaseTable<> database2;
      database2.ignoreColumn(1, true);
      database2.ignoreColumn(3, true);
      {
        const auto ignored_vect = database2.ignoredColumns();
        TS_ASSERT_EQUALS(ignored_vect.size(), std::size_t(1));
        TS_ASSERT_EQUALS(ignored_vect[0], std::size_t(0));
      }

      database2.insertTranslator(translator, 5);
      {
        const auto ignored_vect = database2.ignoredColumns();
        TS_ASSERT_EQUALS(ignored_vect.size(), std::size_t(6));
        TS_ASSERT_EQUALS(ignored_vect[0], std::size_t(0));
        TS_ASSERT_EQUALS(ignored_vect[4], std::size_t(4));
        TS_ASSERT_EQUALS(ignored_vect[5], std::size_t(6));
      }

      TS_ASSERT_THROWS(database2.insertTranslator(translator, 1), gum::OperationNotAllowed);
      database2.insertTranslator(translator, 2);
      {
        const auto cols = database2.inputColumns();
        TS_ASSERT_EQUALS(cols[0], std::size_t(5));
        TS_ASSERT_EQUALS(cols[1], std::size_t(2));

        const auto ignored_vect = database2.ignoredColumns();
        TS_ASSERT_EQUALS(ignored_vect.size(), std::size_t(5));
        TS_ASSERT_EQUALS(ignored_vect[0], std::size_t(0));
        TS_ASSERT_EQUALS(ignored_vect[1], std::size_t(1));
        TS_ASSERT_EQUALS(ignored_vect[2], std::size_t(3));
        TS_ASSERT_EQUALS(ignored_vect[3], std::size_t(4));
        TS_ASSERT_EQUALS(ignored_vect[4], std::size_t(6));
      }
    }


    void test_insert_translators() {
      std::vector< std::string >     missing{"?", "N/A", "???"};
      gum::learning::DatabaseTable<> database;

      gum::LabelizedVariable var("var0", "", 0);
      var.addLabel("L0");
      var.addLabel("L1");
      var.addLabel("L2");

      database.insertTranslator<>(var, 0, missing);

      var.setName("var1");
      database.insertTranslator<>(var, 1, missing);

      var.setName("var2");
      database.insertTranslator<>(var, 2, missing);

      const auto& vnames = database.variableNames();
      TS_ASSERT_EQUALS(vnames.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(vnames[0], "var0");
      TS_ASSERT_EQUALS(vnames[1], "var1");
      TS_ASSERT_EQUALS(vnames[2], "var2");

      const auto xmiss = gum::learning::DatabaseTable<>::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
         3,
         gum::learning::DBTranslatedValue{std::size_t(2)},
         1.0f);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)1);

      const auto dom1 = database.domainSizes();
      TS_ASSERT_EQUALS(dom1.size(), std::size_t(3));
      TS_ASSERT_EQUALS(dom1[0], std::size_t(3));
      TS_ASSERT_EQUALS(dom1[1], std::size_t(3));
      TS_ASSERT_EQUALS(dom1[2], std::size_t(3));
      TS_ASSERT_EQUALS(database.domainSize(0), dom1[0]);
      TS_ASSERT_EQUALS(database.domainSize(1), dom1[1]);
      TS_ASSERT_EQUALS(database.domainSize(2), dom1[2]);

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
      TS_ASSERT_EQUALS(database.nbRows(), (std::size_t)3);

      std::vector< gum::learning::DatabaseTable<>::IsMissing > is_miss(
         4,
         gum::learning::DatabaseTable<>::IsMissing::False);
      database.insertRows(
         std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > >(4, row),
         is_miss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)7);
      std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > > vectx(4, row2);
      database.insertRows(vectx, is_miss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)11);
      is_miss.resize(2);
      std::vector< gum::learning::DBRow< gum::learning::DBTranslatedValue > > vecty(2, row);
      database.insertRows(std::move(vecty), is_miss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)13);
    }


    void test_reordering() {
      std::vector< std::string >     missing{"?", "N/A", "???"};
      gum::learning::DatabaseTable<> database;

      gum::LabelizedVariable var("var0", "", 0);
      var.addLabel("L1");
      var.addLabel("L2");
      var.addLabel("L0");

      database.insertTranslator<>(var, 0, missing);

      gum::LabelizedVariable var1("var1", "", 0);
      var1.addLabel("L0");
      var1.addLabel("L1");
      var1.addLabel("L2");
      database.insertTranslator<>(var1, 1, missing);

      var.setName("var2");
      database.insertTranslator<>(var, 2, missing);

      const auto& vnames = database.variableNames();
      TS_ASSERT_EQUALS(vnames.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(vnames[0], "var0");
      TS_ASSERT_EQUALS(vnames[1], "var1");
      TS_ASSERT_EQUALS(vnames[2], "var2");

      const auto xmiss = gum::learning::DatabaseTable<>::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
         3,
         gum::learning::DBTranslatedValue{std::size_t(2)},
         1.0f);
      database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)1);

      const auto dom1 = database.domainSizes();
      TS_ASSERT_EQUALS(dom1.size(), std::size_t(3));
      TS_ASSERT_EQUALS(dom1[0], std::size_t(3));
      TS_ASSERT_EQUALS(dom1[1], std::size_t(3));
      TS_ASSERT_EQUALS(dom1[2], std::size_t(3));
      TS_ASSERT_EQUALS(database.domainSize(0), dom1[0]);
      TS_ASSERT_EQUALS(database.domainSize(1), dom1[1]);
      TS_ASSERT_EQUALS(database.domainSize(2), dom1[2]);

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
      TS_ASSERT_EQUALS(database.content().size(), (gum::Size)3);

      TS_ASSERT(database.needsReordering(0));
      TS_ASSERT_EQUALS(database.needsReordering(1), false);
      TS_ASSERT(database.needsReordering(2));

      const auto& content = database.content();
      database.reorder(0);
      TS_ASSERT_EQUALS(content[0][0].discr_val, (std::size_t)0);
      TS_ASSERT_EQUALS(content[0][1].discr_val, (std::size_t)2);
      TS_ASSERT_EQUALS(content[0][2].discr_val, (std::size_t)2);

      TS_ASSERT_EQUALS(content[1][0].discr_val, (std::size_t)2);
      TS_ASSERT_EQUALS(content[1][1].discr_val, (std::size_t)1);
      TS_ASSERT_EQUALS(content[1][2].discr_val, (std::size_t)1);

      TS_ASSERT_EQUALS(content[2][0].discr_val, (std::size_t)1);
      TS_ASSERT_EQUALS(content[2][1].discr_val, (std::size_t)0);
      TS_ASSERT_EQUALS(content[2][2].discr_val, (std::size_t)0);

      database.reorder();
      TS_ASSERT_EQUALS(content[0][0].discr_val, (std::size_t)0);
      TS_ASSERT_EQUALS(content[0][1].discr_val, (std::size_t)2);
      TS_ASSERT_EQUALS(content[0][2].discr_val, (std::size_t)0);

      TS_ASSERT_EQUALS(content[1][0].discr_val, (std::size_t)2);
      TS_ASSERT_EQUALS(content[1][1].discr_val, (std::size_t)1);
      TS_ASSERT_EQUALS(content[1][2].discr_val, (std::size_t)2);

      TS_ASSERT_EQUALS(content[2][0].discr_val, (std::size_t)1);
      TS_ASSERT_EQUALS(content[2][1].discr_val, (std::size_t)0);
      TS_ASSERT_EQUALS(content[2][2].discr_val, (std::size_t)1);
    }


    void test_missing_vals() {
      std::vector< std::string >     missing{"?", "N/A", "???"};
      gum::learning::DatabaseTable<> database;

      gum::LabelizedVariable var("var0", "", 0);
      var.addLabel("L1");
      var.addLabel("L2");
      var.addLabel("L0");

      database.insertTranslator<>(var, 0, missing);

      gum::LabelizedVariable var1("var1", "", 0);
      var1.addLabel("L0");
      var1.addLabel("L1");
      var1.addLabel("L2");
      database.insertTranslator<>(var1, 1, missing);

      var.setName("var2");
      database.insertTranslator<>(var, 2, missing);

      var.setName("var3");
      database.insertTranslator<>(var, 3, missing);

      const auto& vnames = database.variableNames();
      TS_ASSERT_EQUALS(vnames.size(), (gum::Size)4);
      TS_ASSERT_EQUALS(vnames[0], "var0");
      TS_ASSERT_EQUALS(vnames[1], "var1");
      TS_ASSERT_EQUALS(vnames[2], "var2");
      TS_ASSERT_EQUALS(vnames[3], "var3");

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

      TS_ASSERT(database.hasMissingValues());

      TS_ASSERT_EQUALS(database.hasMissingValues(0), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(1), true);
      TS_ASSERT_EQUALS(database.hasMissingValues(2), true);
      TS_ASSERT_EQUALS(database.hasMissingValues(3), true);
      TS_ASSERT_EQUALS(database.hasMissingValues(4), true);
      TS_ASSERT_EQUALS(database.hasMissingValues(5), false);

      database.ignoreColumn(1);
      TS_ASSERT(database.hasMissingValues());
      TS_ASSERT_EQUALS(database.hasMissingValues(0), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(1), true);
      TS_ASSERT_EQUALS(database.hasMissingValues(2), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(3), true);
      TS_ASSERT_EQUALS(database.hasMissingValues(4), true);
      TS_ASSERT_EQUALS(database.hasMissingValues(5), false);

      database.ignoreColumn(2);
      TS_ASSERT(database.hasMissingValues());
      TS_ASSERT_EQUALS(database.hasMissingValues(0), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(1), true);
      TS_ASSERT_EQUALS(database.hasMissingValues(2), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(3), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(4), true);
      TS_ASSERT_EQUALS(database.hasMissingValues(5), false);

      database.ignoreColumn(0);
      TS_ASSERT_EQUALS(database.hasMissingValues(), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(0), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(1), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(2), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(3), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(4), false);
      TS_ASSERT_EQUALS(database.hasMissingValues(5), false);

      database.ignoreColumn(3);
      TS_ASSERT_EQUALS(database.hasMissingValues(), false);
    }


    void test_threads() {
      std::vector< std::string >                       missing{"?", "N/A", "???"};
      gum::ContinuousVariable< double >                var("var", "");
      gum::learning::DBTranslator4ContinuousVariable<> translator(var, missing, true);

      gum::learning::DatabaseTable<> database;
      translator.setVariableName("v1");
      database.insertTranslator(translator, 0);
      translator.setVariableName("v2");
      database.insertTranslator(translator, 1);
      translator.setVariableName("v3");
      database.insertTranslator(translator, 2);

      TS_ASSERT_EQUALS(database.variableNames().size(), (gum::Size)3);
      TS_ASSERT_EQUALS(database.nbVariables(), (std::size_t)3);
      TS_ASSERT_EQUALS(database.variableNames()[0], "v1");
      TS_ASSERT_EQUALS(database.variableNames()[1], "v2");
      TS_ASSERT_EQUALS(database.variableNames()[2], "v3");
      TS_ASSERT_EQUALS(database.variable(0).name(), "v1");
      TS_ASSERT_EQUALS(database.variable(1).name(), "v2");
      TS_ASSERT_EQUALS(database.variable(2).name(), "v3");

      database.ignoreColumn(4);
      TS_ASSERT_THROWS(database.insertTranslator(translator, 4), gum::OperationNotAllowed);

      const auto xmiss = gum::learning::DatabaseTable<>::IsMissing::False;
      gum::learning::DBRow< gum::learning::DBTranslatedValue > row(
         3,
         gum::learning::DBTranslatedValue{2.0f},
         1.0f);
      std::size_t xsize = 1004;
      for (std::size_t i = 0; i < xsize; ++i)
        database.insertRow(row, xmiss);
      TS_ASSERT_EQUALS(database.content().size(), xsize);
      TS_ASSERT_EQUALS(database.nbRows(), xsize);
      TS_ASSERT(!database.hasMissingValues());

      translator.setVariableName("v4");

      for (int i = 0; i < 20; ++i)
        database.insertTranslator(translator, 2, false);
      TS_ASSERT(database.hasMissingValues());
      database.setMinNbRowsPerThread(10000);
      database.insertTranslator(translator, 2, false);
    }


    void test_changeTranslator() {
      gum::learning::DBTranslatorSet<>                set;
      std::vector< std::string >                      missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable<> translator(missing);
      set.insertTranslator(translator, 0);
      set.insertTranslator(translator, 1);
      set.insertTranslator(translator, 2);

      gum::learning::DatabaseTable<> db1(set);
      TS_ASSERT_EQUALS(db1.content().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(db1.variableNames().size(), (gum::Size)3);

      db1.setVariableNames({"x0", "x1", "x2"}, false);

      db1.insertRow({"1", "3", "1"});
      db1.insertRow({"3", "3", "2"});

      gum::IntegerVariable intvar("X1", "");
      intvar.addValue(1);
      intvar.addValue(2);
      intvar.addValue(3);
      gum::learning::DBTranslator4IntegerVariable<> trans2(intvar);
      db1.changeTranslator(trans2, 0);

      db1.insertRow({"3", "3", "?"});
      const auto& c1 = db1.content();
      const auto& r1 = c1[0];
      const auto& r2 = c1[1];
      TS_ASSERT_EQUALS(r1[0].discr_val, (std::size_t)0);
      TS_ASSERT_EQUALS(r2[0].discr_val, (std::size_t)2);

      TS_ASSERT_THROWS(db1.changeTranslator(trans2, 2), gum::OperationNotAllowed);

      std::vector< std::string >                    missing2{"?", "N/A", "??", "???"};
      gum::learning::DBTranslator4IntegerVariable<> trans3(intvar, missing2);
      TS_ASSERT_THROWS_NOTHING(db1.changeTranslator(trans3, 2));
      TS_ASSERT_EQUALS(r1[2].discr_val, (std::size_t)0);
      TS_ASSERT_EQUALS(r2[2].discr_val, (std::size_t)1);

      TS_ASSERT_THROWS_NOTHING(db1.changeTranslator(intvar, 1));
      TS_ASSERT_EQUALS(r1[1].discr_val, (std::size_t)2);
      TS_ASSERT_EQUALS(r2[1].discr_val, (std::size_t)2);


      gum::IntegerVariable intvar2("X1", "");
      intvar2.addValue(1);
      intvar2.addValue(2);
      TS_ASSERT_THROWS(db1.changeTranslator(intvar2, 1), gum::OperationNotAllowed);

      gum::IntegerVariable intvar3("X1", "");
      intvar3.addValue(1);
      intvar3.addValue(2);
      intvar3.addValue(3);
      std::vector< std::string > missing3{"?"};
      TS_ASSERT_THROWS_NOTHING(db1.changeTranslator(intvar3, 0, true, missing3));
      TS_ASSERT_THROWS(db1.changeTranslator(intvar3, 2, true, missing3), gum::OperationNotAllowed);

      const auto better = db1.betterTranslators();
      TS_ASSERT(!better.empty());
    }

    void testBetter() {
      gum::learning::DBTranslatorSet<>                set;
      std::vector< std::string >                      missing{"?", "N/A", "???"};
      gum::learning::DBTranslator4LabelizedVariable<> translator1(missing);
      set.insertTranslator(translator1, 0);   // best = integer
      set.insertTranslator(translator1, 1);   // best = continuous
      set.insertTranslator(translator1, 2);   // best = range
      set.insertTranslator(translator1, 3);   // best = labelized

      gum::IntegerVariable                          var4("x4", "", {1, 2, 3, 4});
      gum::learning::DBTranslator4IntegerVariable<> translator4(var4, missing);
      set.insertTranslator(translator4, 4);   // best = range

      gum::IntegerVariable                          var5("x5", "", {1, 2, 4});
      gum::learning::DBTranslator4IntegerVariable<> translator5(var5, missing);
      set.insertTranslator(translator5, 5);   // best = integer

      gum::RangeVariable                          var6("x6", "", 0, 4);
      gum::learning::DBTranslator4RangeVariable<> translator6(var6, missing);
      set.insertTranslator(translator6, 6);   // best = range

      gum::learning::DatabaseTable<> db1(set);
      TS_ASSERT_EQUALS(db1.content().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(db1.variableNames().size(), (gum::Size)7);

      db1.setVariableNames({"x0", "x1", "x2", "x3", "x4", "x5", "x6"}, false);

      db1.insertRow({"1", "2.4", "4", "3", "3", "1", "0"});
      db1.insertRow({"3", "4", "2", "a", "3", "2", "1"});
      db1.insertRow({"2", "5.5", "?", "?", "1", "4", "0"});
      db1.insertRow({"5", "???", "1", "???", "1", "?", "1"});
      db1.insertRow({"7", "?", "3", "bb", "4", "???", "1"});
      db1.insertRow({"?", "0.0", "1", "0", "4", "4", "?"});

      const auto better = db1.betterTranslators();
      TS_ASSERT(!better.empty());
      TS_ASSERT_EQUALS(better.size(), (gum::Size)4);

      TS_ASSERT_EQUALS(better[0].first, (std::size_t)0);
      TS_ASSERT_EQUALS(better[0].second->variable()->varType(), gum::VarType::Integer);
      TS_ASSERT_EQUALS(better[1].first, (std::size_t)1);
      TS_ASSERT_EQUALS(better[1].second->variable()->varType(), gum::VarType::Continuous);
      TS_ASSERT_EQUALS(better[2].first, (std::size_t)2);
      TS_ASSERT_EQUALS(better[2].second->variable()->varType(), gum::VarType::Range);
      TS_ASSERT_EQUALS(better[3].first, (std::size_t)4);
      TS_ASSERT_EQUALS(better[3].second->variable()->varType(), gum::VarType::Range);

      for (const auto& tr: better) {
        TS_ASSERT_THROWS_NOTHING(db1.changeTranslator(*tr.second, tr.first));
      }

      const auto& content = db1.content();
      const auto& row0    = content[0].row();
      const auto& row1    = content[1].row();
      const auto& row2    = content[2].row();
      const auto& row3    = content[3].row();

      // col0: integer 1,2,3,5,7
      TS_ASSERT_EQUALS(row0[0].discr_val, (std::size_t)0);
      TS_ASSERT_EQUALS(row1[0].discr_val, (std::size_t)2);
      TS_ASSERT_EQUALS(row2[0].discr_val, (std::size_t)1);
      TS_ASSERT_EQUALS(row3[0].discr_val, (std::size_t)3);

      // col1: continuous
      TS_ASSERT_DELTA(row0[1].cont_val, 2.4f, 0.001);
      TS_ASSERT_DELTA(row1[1].cont_val, 4.0f, 0.001);
      TS_ASSERT_DELTA(row2[1].cont_val, 5.5f, 0.001);
      TS_ASSERT_EQUALS(row3[1].cont_val, std::numeric_limits< float >::max());

      // col2: range(1-4)
      TS_ASSERT_EQUALS(row0[2].discr_val, (std::size_t)3);
      TS_ASSERT_EQUALS(row1[2].discr_val, (std::size_t)1);
      TS_ASSERT_EQUALS(row2[2].discr_val, std::numeric_limits< std::size_t >::max());
      TS_ASSERT_EQUALS(row3[2].discr_val, (std::size_t)0);

      // col4: range(1-4)
      TS_ASSERT_EQUALS(row0[4].discr_val, (std::size_t)2);
      TS_ASSERT_EQUALS(row1[4].discr_val, (std::size_t)2);
      TS_ASSERT_EQUALS(row2[4].discr_val, (std::size_t)0);
      TS_ASSERT_EQUALS(row3[4].discr_val, (std::size_t)0);
    }


    private:
    void _create_handler_(gum::learning::DatabaseTable<>*           database,
                          gum::learning::DatabaseTable<>::Handler** handler,
                          int*                                      nb) {
      *handler = new gum::learning::DatabaseTable<>::Handler(*database);

      int x = 0;
      for (const auto& row: **handler) {
        TS_ASSERT_EQUALS(row.size(), (gum::Size)3);
        x++;
      }

      *nb = x;
    }

    void _create_handler_safe_(gum::learning::DatabaseTable<>*               database,
                               gum::learning::DatabaseTable<>::HandlerSafe** handler,
                               int*                                          nb) {
      *handler = new gum::learning::DatabaseTable<>::HandlerSafe(*database);

      int x = 0;
      for (const auto& row: **handler) {
        TS_ASSERT_EQUALS(row.size(), (gum::Size)3);
        x++;
      }

      *nb = x;
    }
  };

} /* namespace gum_tests */
