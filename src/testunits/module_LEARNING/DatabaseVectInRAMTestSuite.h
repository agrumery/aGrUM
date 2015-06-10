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

#include <agrum/learning/database/databaseVectInRAM.h>

namespace gum_tests {

  class DatabaseVectInRAMTestSuite : public CxxTest::TestSuite {
    public:
    void test_db1() {
      gum::learning::DatabaseVectInRAM database;
      TS_ASSERT(database.content().size() == 0);
      TS_ASSERT(database.variableNames().size() == 0);

      database.setVariableNames({"v1", "v2", "v3"});
      TS_ASSERT(database.variableNames().size() == 3);
      TS_ASSERT(database.nbVariables() == 3);

      gum::learning::DBRow row(3, gum::learning::DBCell(2));
      database.insertDBRow(row);

      TS_ASSERT(database.content().size() == 1);
      gum::learning::DatabaseVectInRAM::Handler handler(database);
      database.insertDBRow(row);
      database.insertDBRow(row);
      database.insertDBRow(std::move(row));
      TS_ASSERT(database.content().size() == 4);
      auto range = handler.range();
      TS_ASSERT(range.second == 4);

      gum::learning::DatabaseVectInRAM database2 = database;
      TS_ASSERT(database2.content().size() == 4);
      TS_ASSERT(database2.variableNames().size() == 3);
      TS_ASSERT(database2.nbVariables() == 3);

      gum::learning::DBRow row2(4, gum::learning::DBCell(2));
      TS_ASSERT_THROWS(database2.insertDBRow(row2), gum::SizeError);

      gum::learning::DBRow row3(3, gum::learning::DBCell(4));
      gum::learning::DBRow row4(3, gum::learning::DBCell(5));
      database2.insertDBRows({row3, row4});
      gum::learning::DatabaseVectInRAM::Handler handler2(database2);
      gum::learning::DatabaseVectInRAM::Handler handler3(database2);
      handler2.setRange(4, 6);
      handler3.setRange(5, 6);
      TS_ASSERT(handler2.row()[0].getFloat() == 4);
      TS_ASSERT(handler3.row()[0].getFloat() == 5);
      database2.eraseLastDBRow();
      TS_ASSERT(handler2.row()[0].getFloat() == 4);
      TS_ASSERT_THROWS(handler3.rowSafe()[0].getFloat(), gum::OutOfBounds);

      gum::learning::DatabaseVectInRAM database3(std::move(database2));
      database2 = std::move(database);
      TS_ASSERT(database2.content().size() == 4);
      TS_ASSERT(database.content().size() == 0);
      database = std::move(database3);
      TS_ASSERT(database.content().size() == 5);

      handler2 = database.handler();
      TS_ASSERT(handler2.range().second == 5);
      handler2.nextRow();
      handler2.nextRow();
      handler2.nextRow();
      handler3 = std::move(handler2);
      TS_ASSERT(handler3.range().second == 5);
      TS_ASSERT(handler3.hasRows());
      handler3.nextRow();
      TS_ASSERT(handler3.hasRows());
      handler3.nextRow();
      TS_ASSERT(handler3.hasRows() == false);
    }
  };

} /* namespace gum_tests */
