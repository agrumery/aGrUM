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

#include <agrum/learning/database/databaseFromCSV.h>

namespace gum_tests {

  class DatabaseFromCSVTestSuite : public CxxTest::TestSuite {
    public:
    void test_db1() {
      gum::learning::DatabaseFromCSV database(GET_PATH_STR("asia.csv"));

      TS_ASSERT(database.content().size() == 10000);
      TS_ASSERT(database.variableNames().size() == 8);
      TS_ASSERT(database.variableNames()[0] == "smoking?");
      TS_ASSERT(database.variableNames()[7] == "positive_XraY?");

      auto handler = database.handler();
      TS_ASSERT(handler.DBSize() == 10000);
      TS_ASSERT(handler.size() == 10000);

      const gum::learning::DBRow &row = handler.row();

      for (unsigned int i = 0; i < row.size(); ++i) {
        TS_ASSERT(row[i].type() == gum::learning::DBCell::EltType::FLOAT);
      }

      for (unsigned int i = 0; i < row.size(); ++i) {
        if ((i != 3) && (i != 4)) {
          TS_ASSERT(row[i].getFloat() == 0);
        } else {
          TS_ASSERT(row[i].getFloat() == 1);
        }
      }

      // 0,0,0,1,1,0,0,0

      unsigned int nb = 0;

      while (handler.hasRows()) {
        ++nb;
        handler.nextRow();
      }

      TS_ASSERT(nb == 10000);

      nb = 0;
      handler.reset();

      while (handler.hasRows()) {
        ++nb;
        handler.nextRow();
      }

      TS_ASSERT(nb == 10000);

      handler.reset();

      while (handler.hasRows()) {
        const gum::learning::DBRow &row = handler.row();

        for (unsigned int i = 0; i < row.size(); ++i) {
          TS_ASSERT(row[i].type() == gum::learning::DBCell::EltType::FLOAT);
          TS_ASSERT((row[i].getFloat() == 0) || (row[i].getFloat() == 1));
        }

        handler.nextRow();
      }

      nb = 0;
      handler.setRange(5000, 6000);

      while (handler.hasRows()) {
        ++nb;
        handler.nextRow();
      }

      TS_ASSERT(nb == 1000);

      nb = 0;
      handler.reset();

      while (handler.hasRows()) {
        ++nb;
        handler.nextRow();
      }

      TS_ASSERT(nb == 1000);
    }

    void testOnError() {
      try {
        gum::learning::DatabaseFromCSV database(
            GET_PATH_STR("notExistingDummyDatabse.csv"));
      } catch (gum::IOError &e) {
        TS_ASSERT(true);
        return;
      } catch (...) {
      }

      TS_ASSERT(false);
    }
  };

} /* namespace gum_tests */
