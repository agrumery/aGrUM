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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/numericalDiscreteVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  NumericalDiscreteVariable
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct NumericalDiscreteVariableTestSuite {
    public:
    static void testAll() {
      gum::NumericalDiscreteVariable var1("var1", "this is var1");
      gum::NumericalDiscreteVariable var2("var2", "this is var2", {1.0, 9.45, 7.0});

      CHECK((var1.domainSize()) == (static_cast< gum::Size >(0)));
      const auto& vect1 = var1.numericalDomain();
      CHECK((vect1.size()) == ((gum::Size) static_cast< gum::Size >(0)));
      CHECK((var2.domainSize()) == (static_cast< gum::Size >(3)));
      const auto& vect2 = var2.numericalDomain();
      CHECK((vect2[0]) == (1.0));
      CHECK((vect2[1]) == (7.0));
      CHECK((vect2[2]) == (9.45));

      gum::NumericalDiscreteVariable var3(var2);
      CHECK((var3.domainSize()) == (static_cast< gum::Size >(3)));
      const auto& vect3 = var3.numericalDomain();
      CHECK((vect3[0]) == (1.0));
      CHECK((vect3[1]) == (7.0));
      CHECK((vect3[2]) == (9.45));

      gum::NumericalDiscreteVariable var4(std::move(var3));
      CHECK((var4.domainSize()) == (static_cast< gum::Size >(3)));
      const auto& vect4 = var4.numericalDomain();
      CHECK((vect4[0]) == (1.0));
      CHECK((vect4[1]) == (7.0));
      CHECK((vect4[2]) == (9.45));
      CHECK(vect3.empty());
      CHECK((var3.domainSize()) == (static_cast< gum::Size >(0)));

      gum::NumericalDiscreteVariable* var5 = var4.clone();
      CHECK((var5->domainSize()) == (static_cast< gum::Size >(3)));
      const auto& vect5 = var5->numericalDomain();
      CHECK((vect5[0]) == (1.0));
      CHECK((vect5[1]) == (7.0));
      CHECK((vect5[2]) == (9.45));

      gum::NumericalDiscreteVariable var6("var6", "", {-2, -1, 4.25, 8});
      gum::NumericalDiscreteVariable var7 = var2;
      CHECK((var7.domainSize()) == (static_cast< gum::Size >(3)));
      var7 = var6;
      CHECK((var7.domainSize()) == (static_cast< gum::Size >(4)));
      const auto& vect7 = var7.numericalDomain();
      CHECK((vect7[0]) == (-2));
      CHECK((vect7[1]) == (-1));
      CHECK((vect7[2]) == (4.25));
      CHECK((vect7[3]) == (8));

      var3 = std::move(var7);
      CHECK((vect3[0]) == (-2));
      CHECK((vect3[1]) == (-1));
      CHECK((vect3[2]) == (4.25));
      CHECK((vect3[3]) == (8));
      CHECK((var7.domainSize()) == (static_cast< gum::Size >(0)));

      CHECK((var7) != (var6));
      CHECK((var3) == (var6));

      CHECK((var3.varType()) == (gum::VarType::NUMERICAL));
      CHECK_THROWS_AS(var3.index("0"), const gum::NotFound&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(var3.index("-1"));
      CHECK((var3.index("-2")) == (gum::Idx(0)));
      CHECK((var3.index("-1")) == (gum::Idx(1)));
      CHECK((var3.index("4.25")) == (gum::Idx(2)));
      CHECK((var3.index("8")) == (gum::Idx(3)));
      CHECK((var3.index("  8.0  ")) == (gum::Idx(3)));
      CHECK((var3.index("  8")) == (gum::Idx(3)));

      CHECK((var3.label(0)) == ("-2"));
      CHECK((var3.label(1)) == ("-1"));
      CHECK((var3.label(2)) == ("4.25"));
      CHECK((var3.label(3)) == ("8"));
      CHECK_THROWS_AS(var3.label(4), const gum::OutOfBounds&);

      CHECK((var3.numerical(0)) == (-2.0));
      CHECK((var3.numerical(1)) == (-1.0));
      CHECK((var3.numerical(2)) == (4.25));
      CHECK((var3.numerical(3)) == (8.0));
      CHECK_THROWS_AS(var3.numerical(4), const gum::OutOfBounds&);

      CHECK((var3.domain()) == ("{-2|-1|4.25|8}"));
      CHECK((var1.domain()) == ("{}"));

      var1.addValue(4.25);
      CHECK((vect1.size()) == (static_cast< gum::Size >(1)));
      CHECK((vect1[0]) == (4.25));
      var1.addValue(8);
      CHECK((vect1.size()) == (static_cast< gum::Size >(2)));
      CHECK((vect1[0]) == (4.25));
      CHECK((vect1[1]) == (8));
      var1.addValue(6);
      CHECK((vect1.size()) == (static_cast< gum::Size >(3)));
      CHECK((vect1[0]) == (4.25));
      CHECK((vect1[1]) == (6));
      CHECK((vect1[2]) == (8));
      var1.addValue(2);
      CHECK((vect1.size()) == (static_cast< gum::Size >(4)));
      CHECK((vect1[0]) == (2));
      CHECK((vect1[1]) == (4.25));
      CHECK((vect1[2]) == (6));
      CHECK((vect1[3]) == (8));
      CHECK_THROWS_AS(var1.addValue(2), const gum::DuplicateElement&);
      CHECK_THROWS_AS(var1.addValue(4.25), const gum::DuplicateElement&);
      CHECK_THROWS_AS(var1.addValue(6), const gum::DuplicateElement&);
      CHECK_THROWS_AS(var1.addValue(8), const gum::DuplicateElement&);

      var1.changeValue(6, 7);
      CHECK((vect1.size()) == (static_cast< gum::Size >(4)));
      CHECK((vect1[0]) == (2));
      CHECK((vect1[1]) == (4.25));
      CHECK((vect1[2]) == (7));
      CHECK((vect1[3]) == (8));

      var1.changeValue(7, 3);
      CHECK((vect1.size()) == (static_cast< gum::Size >(4)));
      CHECK((vect1[0]) == (2));
      CHECK((vect1[1]) == (3));
      CHECK((vect1[2]) == (4.25));
      CHECK((vect1[3]) == (8));

      var1.changeValue(3, 0);
      CHECK((vect1.size()) == (static_cast< gum::Size >(4)));
      CHECK((vect1[0]) == (0));
      CHECK((vect1[1]) == (2));
      CHECK((vect1[2]) == (4.25));
      CHECK((vect1[3]) == (8));

      var1.changeValue(0, 10);
      CHECK((vect1.size()) == (static_cast< gum::Size >(4)));
      CHECK((vect1[0]) == (2));
      CHECK((vect1[1]) == (4.25));
      CHECK((vect1[2]) == (8));
      CHECK((vect1[3]) == (10));

      var1.changeValue(4.25, 12.25);
      CHECK((vect1.size()) == (static_cast< gum::Size >(4)));
      CHECK((vect1[0]) == (2));
      CHECK((vect1[1]) == (8));
      CHECK((vect1[2]) == (10));
      CHECK((vect1[3]) == (12.25));

      CHECK_THROWS_AS(var1.changeValue(12.25, 10), const gum::DuplicateElement&);
      CHECK((vect1.size()) == (static_cast< gum::Size >(4)));
      CHECK((vect1[0]) == (2));
      CHECK((vect1[1]) == (8));
      CHECK((vect1[2]) == (10));
      CHECK((vect1[3]) == (12.25));

      CHECK_NOTHROW(var1.changeValue(22, 10));

      var1.eraseValue(8);
      CHECK((vect1.size()) == (static_cast< gum::Size >(3)));
      CHECK((vect1[0]) == (2));
      CHECK((vect1[1]) == (10));
      CHECK((vect1[2]) == (12.25));

      GUM_CHECK_ASSERT_THROWS_NOTHING(var1.eraseValue(22));
      var1.eraseValue(2);
      CHECK((vect1.size()) == (static_cast< gum::Size >(2)));
      CHECK((vect1[0]) == (10));
      CHECK((vect1[1]) == (12.25));
      var1.eraseValue(12.25);
      CHECK((vect1.size()) == (static_cast< gum::Size >(1)));
      CHECK((vect1[0]) == (10));

      CHECK((var6.domainSize()) == (static_cast< gum::Size >(4)));
      var6.eraseValues();
      CHECK((var6.domainSize()) == (static_cast< gum::Size >(0)));

      delete var5;
    }   // namespace gum_tests

    static void testSecondConstructor() {
      {
        gum::NumericalDiscreteVariable var("var", "this is var2", 0.0, 2.5, gum::Size(6));

        CHECK((var.domainSize()) == (static_cast< gum::Size >(6)));
        const auto& vect = var.numericalDomain();
        CHECK((vect.size()) == ((gum::Size) static_cast< gum::Size >(6)));
        CHECK((vect[0]) == (0.0));
        CHECK((vect[1]) == (0.5));
        CHECK((vect[2]) == (1.0));
        CHECK((vect[3]) == (1.5));
        CHECK((vect[4]) == (2.0));
        CHECK((vect[5]) == (2.5));
      }
      {
        gum::NumericalDiscreteVariable var("var", "this is var2", 0.0, 2.5, gum::Size(7));

        CHECK((var.domainSize()) == (static_cast< gum::Size >(7)));
        const auto& vect = var.numericalDomain();
        CHECK((vect.size()) == ((gum::Size) static_cast< gum::Size >(7)));
        CHECK((vect[0]) == (0.0));
        CHECK((vect[1]) == (0.4167));
        CHECK((vect[2]) == (0.8333));
        CHECK((vect[3]) == (1.25));
        CHECK((vect[4]) == (1.6667));
        CHECK((vect[5]) == (2.0833));
        CHECK((vect[6]) == (2.5));
      }
    }

    static void testInfiniteValue() {
      CHECK_THROWS_AS(gum::NumericalDiscreteVariable(
                          "var",
                          "this is var2",
                          {-std::numeric_limits< double >::infinity(), 0.0, 2.5, 3.7, 4.9}),
                      const gum::DefaultInLabel&);
    }

    static void testPositions() {
      gum::NumericalDiscreteVariable var("var", "this is var2", {-2.1, 0.0, 2.5, 3.7, 4.9});
      CHECK((var.closestIndex(-3.9)) == (gum::Idx(0)));
      CHECK((var.closestIndex(532.2)) == (gum::Idx(4)));
      CHECK((var.closestIndex(0)) == (gum::Idx(1)));
      CHECK((var.closestIndex(-2.05)) == (gum::Idx(0)));
      CHECK((var.closestIndex(-0.05)) == (gum::Idx(1)));
      CHECK((var.closestIndex(-1.05)) == (gum::Idx(0)));   // arbitrary
      CHECK((var.closestIndex(4.8)) == (gum::Idx(4)));
      CHECK((var.closestIndex(3.8)) == (gum::Idx(3)));
      CHECK((var.closestIndex(4.3)) == (gum::Idx(3)));     // arbitrary

      gum::NumericalDiscreteVariable v("v", "v", 4, 14, 300);
      CHECK((v.closestLabel(9.0)) == ("8.9833"));
      CHECK((v.closestLabel(8.983)) == ("8.9833"));
      CHECK((v.closestLabel(8.984)) == ("8.9833"));
      CHECK((v.closestLabel(8.9)) == ("8.9164"));
      CHECK((v.closestLabel(9.1)) == ("9.0836"));
      CHECK((v.closestLabel(1.0)) == ("4"));
      CHECK((v.closestLabel(15.0)) == ("14"));
    }
  };

  GUM_TEST_ACTIF(All)
  GUM_TEST_ACTIF(SecondConstructor)
  GUM_TEST_ACTIF(InfiniteValue)
  GUM_TEST_ACTIF(Positions)
}   // namespace gum_tests
