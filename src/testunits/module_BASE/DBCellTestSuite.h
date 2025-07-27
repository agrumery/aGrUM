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


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBCell.h>

namespace gum_tests {

  class [[maybe_unused]] DBCellTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_cell1) {
      const std::vector< std::string > miss{"N/A", "???"};

      gum::learning::DBCell cell1;
      TS_ASSERT_EQUALS(cell1.type(), gum::learning::DBCell::EltType::MISSING)
      TS_ASSERT_THROWS(cell1.integer(), const gum::TypeError&)
      TS_ASSERT_THROWS(cell1.string(), const gum::TypeError&)
      TS_ASSERT_THROWS(cell1.real(), const gum::TypeError&)
      TS_ASSERT_EQUALS(cell1.toString(miss), "N/A")

      gum::learning::DBCell cell2(3.5f);
      TS_ASSERT_EQUALS(cell2.type(), gum::learning::DBCell::EltType::REAL)
      TS_ASSERT_THROWS(cell2.integer(), const gum::TypeError&)
      TS_ASSERT_THROWS(cell2.string(), const gum::TypeError&)
      TS_ASSERT_EQUALS(cell2.isMissing(), false)
      TS_ASSERT_EQUALS(cell2.real(), 3.5f)
      TS_ASSERT_EQUALS(std::stof(cell2.toString(miss)), 3.5f)

      gum::learning::DBCell cell3(int(3));
      TS_ASSERT_EQUALS(cell3.type(), gum::learning::DBCell::EltType::INTEGER)
      TS_ASSERT_THROWS(cell3.real(), const gum::TypeError&)
      TS_ASSERT_THROWS(cell3.string(), const gum::TypeError&)
      TS_ASSERT_EQUALS(cell3.isMissing(), false)
      TS_ASSERT_EQUALS(cell3.integer(), 3)

      gum::learning::DBCell cell3b("toto");
      TS_ASSERT_EQUALS(cell3b.type(), gum::learning::DBCell::EltType::STRING)
      TS_ASSERT_THROWS(cell3b.real(), const gum::TypeError&)
      TS_ASSERT_THROWS(cell3b.integer(), const gum::TypeError&)
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
      TS_ASSERT_THROWS(cell4.real(), const gum::TypeError&)
      TS_ASSERT_THROWS(cell4.integer(), const gum::TypeError&)
      TS_ASSERT_THROWS(cell4.string(), const gum::TypeError&)

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
      TS_ASSERT_THROWS(cell4.setReal("toto"), const gum::TypeError&)
      TS_ASSERT_EQUALS(cell4.type(), gum::learning::DBCell::EltType::REAL)
      TS_ASSERT_EQUALS(cell4.real(), 55.0f)
      TS_ASSERT_THROWS(cell4.setReal("3.4toto"), const gum::TypeError&)
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
      TS_ASSERT_THROWS(cell4.integer(), const gum::TypeError&)
      TS_ASSERT_THROWS(cell4.string(), const gum::TypeError&)
      TS_ASSERT_THROWS(cell4.real(), const gum::TypeError&)
    }
  };

}   // namespace gum_tests
