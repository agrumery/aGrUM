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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBCell.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DBCell
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct DBCellTestSuite {
    public:
    static void test_cell1() {
      const std::vector< std::string > miss{"N/A", "???"};

      gum::learning::DBCell cell1;
      CHECK((cell1.type()) == (gum::learning::DBCell::EltType::MISSING));
      CHECK_THROWS_AS(cell1.integer(), const gum::TypeError&);
      CHECK_THROWS_AS(cell1.string(), const gum::TypeError&);
      CHECK_THROWS_AS(cell1.real(), const gum::TypeError&);
      CHECK((cell1.toString(miss)) == ("N/A"));

      gum::learning::DBCell cell2(3.5f);
      CHECK((cell2.type()) == (gum::learning::DBCell::EltType::REAL));
      CHECK_THROWS_AS(cell2.integer(), const gum::TypeError&);
      CHECK_THROWS_AS(cell2.string(), const gum::TypeError&);
      CHECK((cell2.isMissing()) == (false));
      CHECK((cell2.real()) == (3.5f));
      CHECK((std::stof(cell2.toString(miss))) == (3.5f));

      gum::learning::DBCell cell3(int(3));
      CHECK((cell3.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK_THROWS_AS(cell3.real(), const gum::TypeError&);
      CHECK_THROWS_AS(cell3.string(), const gum::TypeError&);
      CHECK((cell3.isMissing()) == (false));
      CHECK((cell3.integer()) == (3));

      gum::learning::DBCell cell3b("toto");
      CHECK((cell3b.type()) == (gum::learning::DBCell::EltType::STRING));
      CHECK_THROWS_AS(cell3b.real(), const gum::TypeError&);
      CHECK_THROWS_AS(cell3b.integer(), const gum::TypeError&);
      CHECK((cell3b.isMissing()) == (false));
      CHECK((cell3b.string()) == ("toto"));

      gum::learning::DBCell cell4(4);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK((cell4.integer()) == (4));

      gum::learning::DBCell cell5(cell4);
      CHECK((cell5.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK((cell5.integer()) == (4));
      gum::learning::DBCell cell6(std::move(cell3));
      CHECK((cell6.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK((cell6.integer()) == (3));

      cell2 = cell3;
      CHECK((cell3.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK((cell3.integer()) == (3));
      cell3 = cell1;
      CHECK((cell3.type()) == (gum::learning::DBCell::EltType::MISSING));
      cell4 = 5.2f;
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::REAL));
      CHECK((cell4.real()) == (5.2f));
      cell4 = 2;
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK((cell4.integer()) == (2));
      cell4 = std::move(cell6);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK((cell4.integer()) == (3));

      cell4.convertType(gum::learning::DBCell::EltType::REAL);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::REAL));
      CHECK((cell4.real()) == (3.0f));
      cell4.convertType(gum::learning::DBCell::EltType::INTEGER);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK((cell4.integer()) == (3));
      cell4.convertType(gum::learning::DBCell::EltType::STRING);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::STRING));
      CHECK((cell4.string()) == ("3"));
      cell4.convertType(gum::learning::DBCell::EltType::INTEGER);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK((cell4.integer()) == (3));
      cell4.convertType(gum::learning::DBCell::EltType::INTEGER);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK((cell4.integer()) == (3));
      cell4.convertType(gum::learning::DBCell::EltType::MISSING);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::MISSING));
      CHECK_THROWS_AS(cell4.real(), const gum::TypeError&);
      CHECK_THROWS_AS(cell4.integer(), const gum::TypeError&);
      CHECK_THROWS_AS(cell4.string(), const gum::TypeError&);

      cell4.setReal(4.5);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::REAL));
      CHECK((cell4.real()) == (4.5f));
      CHECK((cell4.convertType(gum::learning::DBCell::EltType::INTEGER)) == (false));

      cell4.setReal(5);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::REAL));
      CHECK((cell4.real()) == (5.0f));
      cell4.setReal("55");
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::REAL));
      CHECK((cell4.real()) == (55.0f));
      CHECK_THROWS_AS(cell4.setReal("toto"), const gum::TypeError&);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::REAL));
      CHECK((cell4.real()) == (55.0f));
      CHECK_THROWS_AS(cell4.setReal("3.4toto"), const gum::TypeError&);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::REAL));
      CHECK((cell4.real()) == (55.0f));

      cell4.setInteger(6);
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK((cell4.integer()) == (6));
      cell4.setInteger("55");
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::INTEGER));
      CHECK((cell4.integer()) == (55));

      CHECK((cell4.toString(miss)) == ("55"));

      cell4.setString("titi");
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::STRING));
      CHECK((cell4.string()) == ("titi"));
      CHECK((cell4.toString(miss)) == ("titi"));

      CHECK((gum::learning::DBCell::string(0)) == ("toto"));
      CHECK((gum::learning::DBCell::string(2)) == ("titi"));
      CHECK((cell4.stringIndex()) == (2));

      cell4.setMissingState();
      CHECK((cell4.type()) == (gum::learning::DBCell::EltType::MISSING));
      CHECK_THROWS_AS(cell4.integer(), const gum::TypeError&);
      CHECK_THROWS_AS(cell4.string(), const gum::TypeError&);
      CHECK_THROWS_AS(cell4.real(), const gum::TypeError&);
    }   // namespace gum_tests
  };

  GUM_TEST_ACTIF(_cell1)

}   // namespace gum_tests
