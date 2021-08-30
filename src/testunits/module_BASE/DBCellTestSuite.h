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
#include <iostream>

#include <agrum/tools/database/DBCell.h>

namespace gum_tests {

  class DBCellTestSuite: public CxxTest::TestSuite {
    public:
    void test_cell1() {
      const std::vector< std::string > miss{"N/A", "???"};

      gum::learning::DBCell cell1;
      TS_ASSERT_EQUALS(cell1.type(), gum::learning::DBCell::EltType::MISSING)
      TS_ASSERT_THROWS(cell1.integer(), gum::TypeError)
      TS_ASSERT_THROWS(cell1.string(), gum::TypeError)
      TS_ASSERT_THROWS(cell1.real(), gum::TypeError)
      TS_ASSERT_EQUALS(cell1.toString(miss), "N/A")

      gum::learning::DBCell cell2(3.5f);
      TS_ASSERT_EQUALS(cell2.type(), gum::learning::DBCell::EltType::REAL)
      TS_ASSERT_THROWS(cell2.integer(), gum::TypeError)
      TS_ASSERT_THROWS(cell2.string(), gum::TypeError)
      TS_ASSERT_EQUALS(cell2.isMissing(), false)
      TS_ASSERT_EQUALS(cell2.real(), 3.5f)
      TS_ASSERT_EQUALS(std::stof(cell2.toString(miss)), 3.5f)

      gum::learning::DBCell cell3(int(3));
      TS_ASSERT_EQUALS(cell3.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_THROWS(cell3.real(), gum::TypeError)
      TS_ASSERT_THROWS(cell3.string(), gum::TypeError)
      TS_ASSERT_EQUALS(cell3.isMissing(), false)
      TS_ASSERT_EQUALS(cell3.integer(), 3)

      gum::learning::DBCell cell3b("toto");
      TS_ASSERT_EQUALS(cell3b.type(), gum::learning::DBCell::EltType::STRING)
      TS_ASSERT_THROWS(cell3b.real(), gum::TypeError)
      TS_ASSERT_THROWS(cell3b.integer(), gum::TypeError)
      TS_ASSERT_EQUALS(cell3b.isMissing(), false)
      TS_ASSERT_EQUALS(cell3b.string(), "toto")

      gum::learning::DBCell cell4(4);
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_EQUALS(cell4.integer(), 4)

      gum::learning::DBCell cell5(cell4);
      TS_ASSERT_EQUALS(cell5.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_EQUALS(cell5.integer(), 4)
      gum::learning::DBCell cell6(std::move(cell3));
      TS_ASSERT_EQUALS(cell6.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_EQUALS(cell6.integer(), 3)

      cell2 = cell3;
      TS_ASSERT_EQUALS(cell3.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_EQUALS(cell3.integer(), 3)
      cell3 = cell1;
      TS_ASSERT_EQUALS(cell3.type(), gum::learning::DBCell::EltType::MISSING)
      cell4 = 5.2f;
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::REAL)
      TS_ASSERT_EQUALS(cell4.real(), 5.2f)
      cell4 = 2;
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_EQUALS(cell4.integer(), 2)
      cell4 = std::move(cell6);
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_EQUALS(cell4.integer(), 3)

      cell4.convertType(gum::learning::DBCell::EltType::REAL);
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::REAL)
      TS_ASSERT_EQUALS(cell4.real(), 3.0f)
      cell4.convertType(gum::learning::DBCell::EltType::INTEGER);
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_EQUALS(cell4.integer(), 3)
      cell4.convertType(gum::learning::DBCell::EltType::STRING);
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::STRING)
      TS_ASSERT_EQUALS(cell4.string(), "3")
      cell4.convertType(gum::learning::DBCell::EltType::INTEGER);
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_EQUALS(cell4.integer(), 3)
      cell4.convertType(gum::learning::DBCell::EltType::INTEGER);
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_EQUALS(cell4.integer(), 3)
      cell4.convertType(gum::learning::DBCell::EltType::MISSING);
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::MISSING)
      TS_ASSERT_THROWS(cell4.real(), gum::TypeError)
      TS_ASSERT_THROWS(cell4.integer(), gum::TypeError)
      TS_ASSERT_THROWS(cell4.string(), gum::TypeError)

      cell4.setReal(4.5);
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::REAL)
      TS_ASSERT_EQUALS(cell4.real(), 4.5f)
      TS_ASSERT_EQUALS(cell4.convertType(gum::learning::DBCell::EltType::INTEGER), false)

      cell4.setReal(5);
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::REAL)
      TS_ASSERT_EQUALS(cell4.real(), 5.0f)
      cell4.setReal("55");
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::REAL)
      TS_ASSERT_EQUALS(cell4.real(), 55.0f)
      TS_ASSERT_THROWS(cell4.setReal("toto"), gum::TypeError)
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::REAL)
      TS_ASSERT_EQUALS(cell4.real(), 55.0f)
      TS_ASSERT_THROWS(cell4.setReal("3.4toto"), gum::TypeError)
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::REAL)
      TS_ASSERT_EQUALS(cell4.real(), 55.0f)

      cell4.setInteger(6);
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_EQUALS(cell4.integer(), 6)
      cell4.setInteger("55");
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_EQUALS(cell4.integer(), 55)

      TS_ASSERT_EQUALS(cell4.toString(miss), "55")

      cell4.setString("titi");
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::STRING)
      TS_ASSERT_EQUALS(cell4.string(), "titi")
      TS_ASSERT_EQUALS(cell4.toString(miss), "titi")

      TS_ASSERT_EQUALS(gum::learning::DBCell::string(0), "toto")
      TS_ASSERT_EQUALS(gum::learning::DBCell::string(2), "titi")
      TS_ASSERT_EQUALS(cell4.stringIndex(), 2)

      cell4.setMissingState();
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::MISSING)
      TS_ASSERT_THROWS(cell4.integer(), gum::TypeError)
      TS_ASSERT_THROWS(cell4.string(), gum::TypeError)
      TS_ASSERT_THROWS(cell4.real(), gum::TypeError)
    }
  };

}   // namespace gum_tests
