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

#include <agrum/base/variables/allDiscreteVariables.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  AllVariables
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {
  struct AllVariablesTestSuite {
    public:
    static void testCreationRange() {
      try {
        {
          auto a = FASTVARDBL("A1", 2);
          CHECK((a->toString()) == ("A1:Range([0,1])"));
        }   // namespace gum_tests

        {
          // a way to create a variable with only one value
          auto a = FASTVARDBL("A2", 1);
          CHECK((a->toString()) == ("A2:Range([0,0])"));
        }
        {
          auto a = FASTVARDBL("A3", 4);
          CHECK((a->toString()) == ("A3:Range([0,3])"));
        }
        {
          auto a = FASTVARDBL("A4[3]", 4);
          CHECK((a->toString()) == ("A4:Range([0,2])"));
        }
        {
          auto a = FASTVARDBL("A5[3,7]", 4);
          CHECK((a->toString()) == ("A5:Range([3,7])"));
        }
        {
          auto a = FASTVARDBL("A6[-7,-3]", 4);
          CHECK((a->toString()) == ("A6:Range([-7,-3])"));
        }
        {
          auto a = FASTVARDBL("A7{0|1|2|3}", 4);
          CHECK((a->toString()) == ("A7:Range([0,3])"));
          CHECK((a->toFast()) == ("A7[4]"));
        }
        {
          auto a = FASTVARDBL("A8{1|2|3}", 4);
          CHECK((a->toString()) == ("A8:Range([1,3])"));
        }
        {
          auto a = FASTVARDBL("A9{0|1}", 4);
          CHECK((a->toString()) == ("A9:Range([0,1])"));
          CHECK((a->toFast()) == ("A9[2]"));
        }

        CHECK_THROWS_AS(auto a = FASTVARDBL("A7[7,3]", 4), const gum::InvalidArgument&);

        CHECK_THROWS_AS(auto a = FASTVARDBL("A8[-5]", 4), const gum::InvalidArgument&);

        CHECK_THROWS_AS(auto a = FASTVARDBL("A9[1,1]", 4), const gum::InvalidArgument&);

        {
          auto a = FASTVARDBL("AA[1,1]", 1);
          CHECK((a->toString()) == ("AA:Range([1,1])"));
        }
      } catch (gum::Exception const& e) GUM_SHOWERROR(e);
    }

    static void testCreationLabelized() {
      try {
        {
          auto a = FASTVARDBL("A{a|b|c}", 4);
          CHECK((a->toString()) == ("A:Labelized({a|b|c})"));
        }
        CHECK_THROWS_AS(auto a = FASTVARDBL("A{a}", 4), const gum::InvalidArgument&);

        {
          auto a = FASTVARDBL("A{a}", 1);
          CHECK((a->toString()) == ("A:Labelized({a})"));
        }
      } catch (gum::Exception const& e) { GUM_SHOWERROR(e); }
    }

    static void testCreationInteger() {
      {
        auto a = FASTVARDBL("A{0|3|5}", 4);
        CHECK((a->toString()) == ("A:Integer({0|3|5})"));
      }
      {
        auto a = FASTVARDBL("A{-3|0|3}", 4);
        CHECK((a->toString()) == ("A:Integer({-3|0|3})"));
      }
      {
        auto a = FASTVARDBL("A{15|-3|0|3}", 4);
        CHECK((a->toString()) == ("A:Integer({-3|0|3|15})"));
      }
      CHECK_THROWS_AS(auto a = FASTVARDBL("A{15}", 4), gum::InvalidArgument&);

      {
        auto a = FASTVARDBL("A{15}", 1);
        CHECK((a->toString()) == ("A:Integer({15})"));
      }
      {
        auto a = FASTVARDBL("A{15|14|12|13|11}");
        CHECK((a->toString()) == ("A:Range([11,15])"));
      }
    }

    static void testCreationDiscretized() {
      {
        auto a = FASTVARDBL("A[1,2,3,4,5,6]", 4);
        CHECK((a->toString()) == ("A:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)"));
        CHECK(!a->isEmpirical());
      }
      {
        auto a = FASTVARDBL("A+[1,2,3,4,5,6]", 4);
        CHECK((a->toString()) == ("A:Discretized(<(1;2[,[2;3[,[3;4[,[4;5[,[5;6)>)"));
        CHECK(a->isEmpirical());
      }

      CHECK_THROWS_AS(auto a = FASTVARDBL("A[0.3]", 4), gum::InvalidArgument&);
      CHECK_THROWS_AS(auto a = FASTVARDBL("A[0.3]", 1), gum::InvalidArgument&);
    }

    static void testCreationDiscretized2() {
      {
        auto a = FASTVARDBL("A[1:6:5]", 4);
        CHECK((a->toString()) == ("A:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)"));
        CHECK(!a->isEmpirical());
      }
      {
        auto a = FASTVARDBL("A[1:6:2]", 4);
        CHECK((a->toString()) == ("A:Discretized(<[1;3.5[,[3.5;6]>)"));
        CHECK(!a->isEmpirical());
      }
      {
        auto a = FASTVARDBL("A+[1:6:5]", 4);
        CHECK((a->toString()) == ("A:Discretized(<(1;2[,[2;3[,[3;4[,[4;5[,[5;6)>)"));
        CHECK(a->isEmpirical());
      }
      {
        auto a = FASTVARDBL("A+[1:6:2]", 4);
        CHECK((a->toString()) == ("A:Discretized(<(1;3.5[,[3.5;6)>)"));
        CHECK(a->isEmpirical());
      }

      CHECK_THROWS_AS(auto a = FASTVARDBL("A[1:6:1]", 4), gum::InvalidArgument&);
    }

    static void testCreationNumerical() {
      try {
        {
          auto a = FASTVARDBL("A{1|1.5|6|2.9|3.14}", 4);
          CHECK((a->toString()) == ("A:NumericalDiscrete({1|1.5|2.9|3.14|6})"));
        }
        CHECK_THROWS_AS(auto a = FASTVARDBL("A{3.14}", 4), const gum::InvalidArgument&);
        {
          auto a = FASTVARDBL("A{3.14}", 1);
          CHECK((a->toString()) == ("A:NumericalDiscrete({3.14})"));
        }
        {
          auto a = FASTVARDBL("A{1.2:5.2:5}", 5);
          CHECK((a->toString()) == ("A:NumericalDiscrete({1.2|2.2|3.2|4.2|5.2})"));
        }
        {
          auto a = FASTVARDBL("A{1.2:5.2:2}", 2);
          CHECK((a->toString()) == ("A:NumericalDiscrete({1.2|5.2})"));
          auto b = FASTVARDBL("A{1.2|5.2}", 2);
          CHECK((b->toString()) == ("A:NumericalDiscrete({1.2|5.2})"));
        }
        {
          auto a = FASTVARDBL("A{1.2:5.2:2.8}", 2);
          CHECK((a->toString()) == ("A:NumericalDiscrete({1.2|5.2})"));
        }
      } catch (gum::Exception const& e) {
        GUM_SHOWERROR(e);
        GUM_TRACE(e.errorCallStack());
      }
    }

    static void testToFastMethod() {
      std::string s;
      s = "A{On|Off|Defun}";
      CHECK((s) == ((FASTVARDBL(s)->toFast())));              // labelized
      s = "A{0|1.15|2.3}";
      CHECK((s) == ((FASTVARDBL(s)->toFast())));              // NumericalDiscrete
      CHECK((s) == ((FASTVARDBL("A{0:2.3:3}")->toFast())));   // NumericalDiscrete
      s = "A{1|3|5}";
      CHECK((s) == ((FASTVARDBL(s)->toFast())));              // Integer
      CHECK((s) == ((FASTVARDBL("A{1:5:3}")->toFast())));     // Integer
      s = "A[3,5]";
      CHECK((s) == ((FASTVARDBL(s)->toFast())));              // Range
      s = "A[5]";
      CHECK((s) == ((FASTVARDBL(s)->toFast())));              // Range
      s = "A[1,2,3,4,5,6]";
      CHECK((s) == ((FASTVARDBL(s)->toFast())));              // Discretized
      CHECK((s) == ((FASTVARDBL("A[1:6:5]")->toFast())));     // Discretized
      s = "A+[1,2,3,4,5,6]";
      CHECK((s) == ((FASTVARDBL(s)->toFast())));              // Discretized
      CHECK((s) == ((FASTVARDBL("A+[1:6:5]")->toFast())));    // Discretized
    }

    static void testFastNumericalWithoutInfinity() {
      std::string s;
      s = "A{0|1.15|inf}";   // NumericalDiscrete
      CHECK_THROWS_AS(FASTVARDBL(s), const gum::DefaultInLabel&);
      s = "A{1|3|inf}";      // Integer
      CHECK_THROWS_AS(FASTVARDBL(s), const gum::DefaultInLabel&);
      s = "A[3,2.5,inf]";    // Discretized
      // CHECK_THROWS_AS(FASTVARDBL(s), const gum::DefaultInLabel&);
      s = "A[inf]";             // Range
      CHECK_THROWS_AS(FASTVARDBL(s), const std::invalid_argument&);
      s = "A[2,inf]";           // Range
      CHECK_THROWS_AS(FASTVARDBL(s), const std::invalid_argument&);
      s = "A[inf,2,3,4,5,6]";   // Integer
      // CHECK_THROWS_AS(FASTVARDBL(s), const gum::DefaultInLabel&);
    }

    static void testLabelizedComparison() {
      CHECK(*FASTVARDBL("A{On|Off|Defun}") == *FASTVARDBL("A{On|Off|Defun}"));
      CHECK(*FASTVARDBL("A{On|Off|Defun}") != *FASTVARDBL("A{Off|On|Defun}"));
      CHECK(*FASTVARDBL("A{On|Off|Defun}") != *FASTVARDBL("B{On|Off|Defun}"));
      CHECK(*FASTVARDBL("A{On|Off|Defun}") != *FASTVARDBL("A{On|Off}"));
      CHECK(*FASTVARDBL("A{On|Off|Defun}") != *FASTVARDBL("A{On|Off|Defun|Beurk}"));
    }

    static void testRangeComparison() {
      CHECK(*FASTVARDBL("A[3,5]") == *FASTVARDBL("A[3,5]"));
      CHECK(*FASTVARDBL("A[5]  ") == *FASTVARDBL("A[0,4]"));
      CHECK(*FASTVARDBL("A[3,5]") != *FASTVARDBL("B[3,5]"));
      CHECK(*FASTVARDBL("A[3,5]") != *FASTVARDBL("A[2,5]"));
      CHECK(*FASTVARDBL("A[3,5]") != *FASTVARDBL("A[3,6]"));
    }

    static void testIntegerComparison() {
      CHECK(*FASTVARDBL("A{3|5|9}") == *FASTVARDBL("A{3|5|9}"));
      CHECK(*FASTVARDBL("A{3|5|9}") == *FASTVARDBL("A{3|9|5}"));
      CHECK(*FASTVARDBL("A{3|5|9}") != *FASTVARDBL("B{3|9|5}"));
      CHECK(*FASTVARDBL("A{3|5|9}") != *FASTVARDBL("A{3|9}"));
      CHECK(*FASTVARDBL("A{3|5|9}") != *FASTVARDBL("A{3|9|5|12}"));
      CHECK(*FASTVARDBL("A{1:7:4}") == *FASTVARDBL("A{1|3|5|7}"));
    }

    static void testNumericalDiscreteComparison() {
      CHECK(*FASTVARDBL("A{3.4|5.4|9.4}") == *FASTVARDBL("A{3.4|5.4|9.4}"));
      CHECK(*FASTVARDBL("A{3.4|5.4|9.4}") == *FASTVARDBL("A{3.4|9.4|5.4}"));
      CHECK(*FASTVARDBL("A{3.4|5.4|9.4}") != *FASTVARDBL("B{3.4|9.4|5.4}"));
      CHECK(*FASTVARDBL("A{3.4|5.4|9.4}") != *FASTVARDBL("A{3.4|9.4}"));
      CHECK(*FASTVARDBL("A{3.4|5.4|9.4}") != *FASTVARDBL("A{3.4|9.4|5|12.4}"));
      CHECK(*FASTVARDBL("A{1.4:7.4:4}") == *FASTVARDBL("A{1.4|3.4|5.4|7.4}"));
    }

    static void testEquallySpacedIntervall() {
      auto v = gum::fastVariable("alpha { 0 : 1 : 101 }");
      for (const auto& la: v->labels()) {
        CHECK((la.length()) <= (5u));   // 0.94 for instance
      }
    }

    static void testClosestIndex() {
      CHECK_THROWS_AS(FASTVARDBL("A{On|Off|Defun}")->closestIndex(1.5),
                      const gum::NotImplementedYet&);

      CHECK((FASTVARDBL("A{0|1.15|2.3}")->closestIndex(-1.0)) == (0u));
      CHECK((FASTVARDBL("A{0|1.15|2.3}")->closestIndex(0.0)) == (0u));
      CHECK((FASTVARDBL("A{0|1.15|2.3}")->closestIndex(0.575)) == (0u));
      CHECK((FASTVARDBL("A{0|1.15|2.3}")->closestIndex(1.2)) == (1u));
      CHECK((FASTVARDBL("A{0|1.15|2.3}")->closestIndex(1.15)) == (1u));
      CHECK((FASTVARDBL("A{0|1.15|2.3}")->closestIndex(3.0)) == (2u));
      CHECK((FASTVARDBL("A{0|1.15|2.3}")->closestIndex(2.0)) == (2u));
      CHECK((FASTVARDBL("A{0|1.15|2.3}")->closestIndex(2.3)) == (2u));

      CHECK((FASTVARDBL("A{1|3|5}")->closestIndex(0.1)) == (0u));
      CHECK((FASTVARDBL("A{1|3|5}")->closestIndex(1.0)) == (0u));
      CHECK((FASTVARDBL("A{1|3|5}")->closestIndex(1.5)) == (0u));
      CHECK((FASTVARDBL("A{1|3|5}")->closestIndex(2.0)) == (0u));
      CHECK((FASTVARDBL("A{1|3|5}")->closestIndex(2.5)) == (1u));
      CHECK((FASTVARDBL("A{1|3|5}")->closestIndex(3.1)) == (1u));
      CHECK((FASTVARDBL("A{1|3|5}")->closestIndex(3.0)) == (1u));
      CHECK((FASTVARDBL("A{1|3|5}")->closestIndex(3.9)) == (1u));
      CHECK((FASTVARDBL("A{1|3|5}")->closestIndex(4.9)) == (2u));
      CHECK((FASTVARDBL("A{1|3|5}")->closestIndex(5.0)) == (2u));
      CHECK((FASTVARDBL("A{1|3|5}")->closestIndex(9.9)) == (2u));

      CHECK((FASTVARDBL("A[3,5]")->closestIndex(1.5)) == (0u));
      CHECK((FASTVARDBL("A[3,5]")->closestIndex(3.0)) == (0u));
      CHECK((FASTVARDBL("A[3,5]")->closestIndex(3.2)) == (0u));
      CHECK((FASTVARDBL("A[3,5]")->closestIndex(3.5)) == (0u));
      CHECK((FASTVARDBL("A[3,5]")->closestIndex(3.8)) == (1u));
      CHECK((FASTVARDBL("A[3,5]")->closestIndex(4.0)) == (1u));
      CHECK((FASTVARDBL("A[3,5]")->closestIndex(4.2)) == (1u));
      CHECK((FASTVARDBL("A[3,5]")->closestIndex(4.5)) == (1u));
      CHECK((FASTVARDBL("A[3,5]")->closestIndex(4.7)) == (2u));
      CHECK((FASTVARDBL("A[3,5]")->closestIndex(5.0)) == (2u));
      CHECK((FASTVARDBL("A[3,5]")->closestIndex(9.0)) == (2u));

      CHECK((FASTVARDBL("A[-5,-3]")->closestIndex(-1.5)) == (2u));
      CHECK((FASTVARDBL("A[-5,-3]")->closestIndex(-3.2)) == (2u));
      CHECK((FASTVARDBL("A[-5,-3]")->closestIndex(-3.5)) == (1u));
      CHECK((FASTVARDBL("A[-5,-3]")->closestIndex(-3.8)) == (1u));
      CHECK((FASTVARDBL("A[-5,-3]")->closestIndex(-4.0)) == (1u));
      CHECK((FASTVARDBL("A[-5,-3]")->closestIndex(-4.2)) == (1u));
      CHECK((FASTVARDBL("A[-5,-3]")->closestIndex(-4.5)) == (0u));
      CHECK((FASTVARDBL("A[-5,-3]")->closestIndex(-4.7)) == (0u));
      CHECK((FASTVARDBL("A[-5,-3]")->closestIndex(-5.0)) == (0u));
      CHECK((FASTVARDBL("A[-5,-3]")->closestIndex(-9.0)) == (0u));

      CHECK((FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(0.5)) == (0u));
      CHECK((FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(1.0)) == (0u));
      CHECK((FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(1.3)) == (0u));
      CHECK((FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(1.5)) == (0u));
      CHECK((FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(1.7)) == (0u));
      CHECK((FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(2.0)) == (1u));
      CHECK((FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(2.5)) == (1u));
      CHECK((FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(3.0)) == (2u));
      CHECK((FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(5.5)) == (4u));
      CHECK((FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(6.0)) == (4u));
      CHECK((FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(9.9)) == (4u));
    }

    static void testRegressionTestFromChristopheEveland() {
      auto var = FASTVARDBL("A[0.0, 100.000001, 100.000002, 200.0]");
      CHECK((var->toString())
            == ("A:Discretized(<[0;100.000001[,[100.000001;100.000002[,[100.000002;200]>)"));
      CHECK((var->toFast()) == ("A[0,100.000001,100.000002,200]"));
    }
  };

  GUM_TEST_ACTIF(CreationRange)
  GUM_TEST_ACTIF(CreationLabelized)
  GUM_TEST_ACTIF(CreationInteger)
  GUM_TEST_ACTIF(CreationDiscretized)
  GUM_TEST_ACTIF(CreationDiscretized2)
  GUM_TEST_ACTIF(CreationNumerical)
  GUM_TEST_ACTIF(ToFastMethod)
  GUM_TEST_ACTIF(FastNumericalWithoutInfinity)
  GUM_TEST_ACTIF(LabelizedComparison)
  GUM_TEST_ACTIF(RangeComparison)
  GUM_TEST_ACTIF(IntegerComparison)
  GUM_TEST_ACTIF(NumericalDiscreteComparison)
  GUM_TEST_ACTIF(EquallySpacedIntervall)
  GUM_TEST_ACTIF(ClosestIndex)
  GUM_TEST_ACTIF(RegressionTestFromChristopheEveland)
}   // namespace gum_tests
