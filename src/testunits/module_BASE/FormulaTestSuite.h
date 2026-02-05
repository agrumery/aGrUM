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

#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/math/formula.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MultiDimFormula
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct MultiDimFormulaTestSuite {
    public:
    static void testConstantInt() {
      try {   // Arrange
        std::string  eq       = "1";
        double       expected = 1;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      }   // namespace gum_tests

      catch (gum::Exception&) {
        CHECK(false);
      }
    }

    static void testConstantFloat() {
      try {
        // Arrange
        std::string  eq       = "0.99";
        double       expected = 0.99;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testMultiplication() {
      try {
        // Arrange
        std::string  eq       = "10 * 2";
        double       expected = 20.0;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testDivision() {
      try {
        // Arrange
        std::string  eq       = "10 / 2";
        double       expected = 5.0;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testAddition() {
      try {
        // Arrange
        std::string  eq       = "10 + 2";
        double       expected = 12.0;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testSubstraction() {
      try {
        // Arrange
        std::string  eq       = "10 - 2";
        double       expected = 8.0;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testSimplePriority1() {
      try {
        // Arrange
        std::string  eq       = "10 - 2 * 5";
        double       expected = 0.0;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testSimplePriority2() {
      try {
        // Arrange
        std::string  eq       = "10 * 2 - 5";
        double       expected = 15.0;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testComplexPriority() {
      try {
        // Arrange
        std::string  eq       = "5 + ( ( 1 + 2 ) * 4 ) - 3";
        double       expected = 14.0;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testMinusSign() {
      try {
        // Arrange
        std::string  eq       = "-3";
        double       expected = -3.0;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testExp() {
      try {
        // Arrange
        std::string  eq       = "exp(0.001)";
        double       expected = 1.0010005;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testLog() {
      try {
        // Arrange
        std::string  eq       = "1 + log(1)";
        double       expected = 1;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testLn() {
      try {
        // Arrange
        std::string  eq       = "2 + ln(1)";
        double       expected = 2;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testSqrt() {
      try {
        // Arrange
        std::string  eq       = "1 + sqrt(4)";
        double       expected = 3;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testPow() {
      try {
        // Arrange
        std::string  eq       = "1 + pow(2,2)";
        double       expected = 5;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testGoal() {
      try {
        // Arrange
        std::string  eq       = "1-exp(-0.001*2)";
        double       expected = 1 - std::exp(-0.001 * 2);
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testVariables() {
      try {
        // Arrange
        double       lambda   = 0.001;
        double       t        = 2;
        double       unused   = 10;
        std::string  eq       = "1-exp(-lambda*t)";
        double       expected = 1 - std::exp(-lambda * t);
        gum::Formula formula(eq);
        formula.variables().insert("lambda", lambda);
        formula.variables().insert("t", t);
        formula.variables().insert("unused", unused);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK(formula.variables().exists("lambda"));
        CHECK(formula.variables().exists("t"));
        CHECK(formula.variables().exists("unused"));
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testScientificNotation1() {
      try {
        // Arrange
        std::string  eq       = "3.72663E-06";
        double       expected = 3.72663E-06;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testScientificNotation2() {
      try {
        // Arrange
        std::string  eq       = "1 - 1.0e-9";
        double       expected = 1 - 1e-9;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testScientificNotation3() {
      try {
        // Arrange
        std::string  eq       = "1 - 1e-9";
        double       expected = 1 - 1e-9;
        gum::Formula formula(eq);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        CHECK((formula.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testOperatorUnary() {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto expected = -(2.0 * 5.0 - 6.0);
        auto result   = gum::Formula("0");
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(result = -a);
        // Assert
        CHECK((result.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testOperatorPlus() {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto b        = gum::Formula("2/8");
        auto expected = 2.0 * 5.0 - 6.0 + 2.0 / 8.0;
        auto result   = gum::Formula("0");
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(result = a + b);
        // Assert
        CHECK((result.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testOperatorMinus() {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto b        = gum::Formula("2/8");
        auto expected = 2.0 * 5.0 - 6.0 - 2.0 / 8.0;
        auto result   = gum::Formula("0");
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(result = a - b);
        // Assert
        CHECK((result.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testOperatorTimes() {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto b        = gum::Formula("2/8");
        auto expected = (2.0 * 5.0 - 6.0) * (2.0 / 8.0);
        auto result   = gum::Formula("0");
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(result = a * b);
        // Assert
        CHECK((result.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testOperatorDivides() {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto b        = gum::Formula("2/8");
        auto expected = (2.0 * 5.0 - 6.0) / (2.0 / 8.0);
        auto result   = gum::Formula("0");
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(result = a / b);
        // Assert
        CHECK((result.result()) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testDoubleImplicitConversion() {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto expected = 2.0 * 5.0 - 6.0;
        auto result   = 0.0;
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(result = (double)a);
        // Assert
        CHECK((result) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testDoubleExplicitConversion() {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto expected = 2.0 * 5.0 - 6.0;
        auto result   = 0.0;
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(result = static_cast< double >(a));
        // Assert
        CHECK((result) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testDoubleInitialisation() {
      // Arrange
      try {
        auto expected = 3.14;
        // Act
        gum::Formula f = 3.14;
        // Assert
        CHECK(((double)f) == doctest::Approx(expected).epsilon(1e-6));
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testToString() {
      // Arrange
      auto f        = gum::Formula("2*5+6");
      auto expected = std::to_string(2.0 * 5.0 + 6.0);
      auto result   = std::string();
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(result = gum::to_string(f));
      // Assert
      CHECK((result) == (expected));
    }

    static void testToStream() {
      // Arrange
      auto              f        = gum::Formula("2*5+6");
      auto              expected = std::to_string(2 * 5 + 6);
      std::stringstream result;
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(result << f);
      // Assert
      CHECK((result.str()) == (expected));
    }
  };

  GUM_TEST_ACTIF(ConstantInt)
  GUM_TEST_ACTIF(ConstantFloat)
  GUM_TEST_ACTIF(Multiplication)
  GUM_TEST_ACTIF(Division)
  GUM_TEST_ACTIF(Addition)
  GUM_TEST_ACTIF(Substraction)
  GUM_TEST_ACTIF(SimplePriority1)
  GUM_TEST_ACTIF(SimplePriority2)
  GUM_TEST_ACTIF(ComplexPriority)
  GUM_TEST_ACTIF(MinusSign)
  GUM_TEST_ACTIF(Exp)
  GUM_TEST_ACTIF(Log)
  GUM_TEST_ACTIF(Ln)
  GUM_TEST_ACTIF(Sqrt)
  GUM_TEST_ACTIF(Pow)
  GUM_TEST_ACTIF(Goal)
  GUM_TEST_ACTIF(Variables)
  GUM_TEST_ACTIF(ScientificNotation1)
  GUM_TEST_ACTIF(ScientificNotation2)
  GUM_TEST_ACTIF(ScientificNotation3)
  GUM_TEST_ACTIF(OperatorUnary)
  GUM_TEST_ACTIF(OperatorPlus)
  GUM_TEST_ACTIF(OperatorMinus)
  GUM_TEST_ACTIF(OperatorTimes)
  GUM_TEST_ACTIF(OperatorDivides)
  GUM_TEST_ACTIF(DoubleImplicitConversion)
  GUM_TEST_ACTIF(DoubleExplicitConversion)
  GUM_TEST_ACTIF(DoubleInitialisation)
  GUM_TEST_ACTIF(ToString)
  GUM_TEST_ACTIF(ToStream)
}   // namespace gum_tests
