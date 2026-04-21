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
#include <sstream>
#include <string>

#include <agrum/base/variables/integerVariable.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  IntegerVariable
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct IntegerVariableTestSuite {
    public:
    static void testAll() {
      gum::IntegerVariable var1("var1", "this is var1");
      gum::IntegerVariable var2("var2", "this is var2", {1, 9, 7});

      CHECK_EQ(var1.domainSize(), static_cast< gum::Size >(0));
      const auto& vect1 = var1.integerDomain();
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(0));
      CHECK_EQ(var2.domainSize(), static_cast< gum::Size >(3));
      const auto& vect2 = var2.integerDomain();
      CHECK_EQ(vect2[0], 1);
      CHECK_EQ(vect2[1], 7);
      CHECK_EQ(vect2[2], 9);

      gum::IntegerVariable var3(var2);
      CHECK_EQ(var3.domainSize(), static_cast< gum::Size >(3));
      const auto& vect3 = var3.integerDomain();
      CHECK_EQ(vect3[0], 1);
      CHECK_EQ(vect3[1], 7);
      CHECK_EQ(vect3[2], 9);

      gum::IntegerVariable var4(std::move(var3));
      CHECK_EQ(var4.domainSize(), static_cast< gum::Size >(3));
      const auto& vect4 = var4.integerDomain();
      CHECK_EQ(vect4[0], 1);
      CHECK_EQ(vect4[1], 7);
      CHECK_EQ(vect4[2], 9);
      CHECK(vect3.empty());
      CHECK_EQ(var3.domainSize(), static_cast< gum::Size >(0));

      gum::IntegerVariable* var5 = var4.clone();
      CHECK_EQ(var5->domainSize(), static_cast< gum::Size >(3));
      const auto& vect5 = var5->integerDomain();
      CHECK_EQ(vect5[0], 1);
      CHECK_EQ(vect5[1], 7);
      CHECK_EQ(vect5[2], 9);

      gum::IntegerVariable var6("var6", "", {-2, -1, 4, 8});
      gum::IntegerVariable var7 = var2;
      CHECK_EQ(var7.domainSize(), static_cast< gum::Size >(3));
      var7 = var6;
      CHECK_EQ(var7.domainSize(), static_cast< gum::Size >(4));
      const auto& vect7 = var7.integerDomain();
      CHECK_EQ(vect7[0], -2);
      CHECK_EQ(vect7[1], -1);
      CHECK_EQ(vect7[2], 4);
      CHECK_EQ(vect7[3], 8);

      var3 = std::move(var7);
      CHECK_EQ(vect3[0], -2);
      CHECK_EQ(vect3[1], -1);
      CHECK_EQ(vect3[2], 4);
      CHECK_EQ(vect3[3], 8);
      CHECK_EQ(var7.domainSize(), static_cast< gum::Size >(0));

      CHECK_NE(var7, var6);
      CHECK_EQ(var3, var6);

      CHECK_EQ(var3.varType(), gum::VarType::INTEGER);
      CHECK_THROWS_AS(var3.index("0"), const gum::NotFound&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(var3.index("-1"));
      CHECK_EQ(var3.index("-2"), gum::Idx(0));
      CHECK_EQ(var3.index("-1"), gum::Idx(1));
      CHECK_EQ(var3.index("4"), gum::Idx(2));
      CHECK_EQ(var3.index("8"), gum::Idx(3));
      CHECK_EQ(var3.index("  8  "), gum::Idx(3));
      CHECK_EQ(var3.index("  8 7"), gum::Idx(3));

      CHECK_EQ(var3.label(0), "-2");
      CHECK_EQ(var3.label(1), "-1");
      CHECK_EQ(var3.label(2), "4");
      CHECK_EQ(var3.label(3), "8");
      CHECK_THROWS_AS(var3.label(4), const gum::OutOfBounds&);

      CHECK_EQ(var3.numerical(0), -2.0);
      CHECK_EQ(var3.numerical(1), -1.0);
      CHECK_EQ(var3.numerical(2), 4.0);
      CHECK_EQ(var3.numerical(3), 8.0);
      CHECK_THROWS_AS(var3.numerical(4), const gum::OutOfBounds&);

      CHECK_EQ(var3.domain(), "{-2|-1|4|8}");
      CHECK_EQ(var1.domain(), "{}");

      var1.addValue(4);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(1));
      CHECK_EQ(vect1[0], 4);
      var1.addValue(8);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(2));
      CHECK_EQ(vect1[0], 4);
      CHECK_EQ(vect1[1], 8);
      var1.addValue(6);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(3));
      CHECK_EQ(vect1[0], 4);
      CHECK_EQ(vect1[1], 6);
      CHECK_EQ(vect1[2], 8);
      var1.addValue(2);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(4));
      CHECK_EQ(vect1[0], 2);
      CHECK_EQ(vect1[1], 4);
      CHECK_EQ(vect1[2], 6);
      CHECK_EQ(vect1[3], 8);
      CHECK_THROWS_AS(var1.addValue(2), const gum::DuplicateElement&);
      CHECK_THROWS_AS(var1.addValue(4), const gum::DuplicateElement&);
      CHECK_THROWS_AS(var1.addValue(6), const gum::DuplicateElement&);
      CHECK_THROWS_AS(var1.addValue(8), const gum::DuplicateElement&);

      var1.changeValue(6, 7);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(4));
      CHECK_EQ(vect1[0], 2);
      CHECK_EQ(vect1[1], 4);
      CHECK_EQ(vect1[2], 7);
      CHECK_EQ(vect1[3], 8);

      var1.changeValue(7, 3);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(4));
      CHECK_EQ(vect1[0], 2);
      CHECK_EQ(vect1[1], 3);
      CHECK_EQ(vect1[2], 4);
      CHECK_EQ(vect1[3], 8);

      var1.changeValue(3, 0);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(4));
      CHECK_EQ(vect1[0], 0);
      CHECK_EQ(vect1[1], 2);
      CHECK_EQ(vect1[2], 4);
      CHECK_EQ(vect1[3], 8);

      var1.changeValue(0, 10);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(4));
      CHECK_EQ(vect1[0], 2);
      CHECK_EQ(vect1[1], 4);
      CHECK_EQ(vect1[2], 8);
      CHECK_EQ(vect1[3], 10);

      var1.changeValue(4, 12);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(4));
      CHECK_EQ(vect1[0], 2);
      CHECK_EQ(vect1[1], 8);
      CHECK_EQ(vect1[2], 10);
      CHECK_EQ(vect1[3], 12);

      CHECK_THROWS_AS(var1.changeValue(12, 10), const gum::DuplicateElement&);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(4));
      CHECK_EQ(vect1[0], 2);
      CHECK_EQ(vect1[1], 8);
      CHECK_EQ(vect1[2], 10);
      CHECK_EQ(vect1[3], 12);

      CHECK_NOTHROW(var1.changeValue(22, 10));

      var1.eraseValue(8);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(3));
      CHECK_EQ(vect1[0], 2);
      CHECK_EQ(vect1[1], 10);
      CHECK_EQ(vect1[2], 12);

      CHECK_NOTHROW(var1.eraseValue(22));
      var1.eraseValue(2);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(2));
      CHECK_EQ(vect1[0], 10);
      CHECK_EQ(vect1[1], 12);
      var1.eraseValue(12);
      CHECK_EQ(vect1.size(), static_cast< gum::Size >(1));
      CHECK_EQ(vect1[0], 10);

      CHECK_EQ(var6.domainSize(), static_cast< gum::Size >(4));
      var6.eraseValues();
      CHECK_EQ(var6.domainSize(), static_cast< gum::Size >(0));

      delete var5;
    }   // namespace gum_tests

    static void testSecondConstructor() {
      {
        gum::IntegerVariable var("var", "this is var2", 0, 10, gum::Size(6));

        CHECK_EQ(var.domainSize(), static_cast< gum::Size >(6));
        const auto& vect = var.integerDomain();
        CHECK_EQ(vect.size(), static_cast< gum::Size >(6));
        CHECK_EQ(vect[0], 0);
        CHECK_EQ(vect[1], 2);
        CHECK_EQ(vect[2], 4);
        CHECK_EQ(vect[3], 6);
        CHECK_EQ(vect[4], 8);
        CHECK_EQ(vect[5], 10);
      }
      {
        gum::IntegerVariable var("var", "this is var2", 0, 9, gum::Size(6));

        CHECK_EQ(var.domainSize(), static_cast< gum::Size >(6));
        const auto& vect = var.integerDomain();
        CHECK_EQ(vect.size(), static_cast< gum::Size >(6));
        CHECK_EQ(vect[0], 0);
        CHECK_EQ(vect[1], 1);
        CHECK_EQ(vect[2], 3);
        CHECK_EQ(vect[3], 5);
        CHECK_EQ(vect[4], 7);
        CHECK_EQ(vect[5], 9);
      }
    }
  };

  GUM_TEST_ACTIF(All)
  GUM_TEST_ACTIF(SecondConstructor)
}   // namespace gum_tests
