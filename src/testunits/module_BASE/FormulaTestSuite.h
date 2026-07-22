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

#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/math/formula.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct FormulaTestSuite {
    public:
    // regression test for HIGH-7: ln used log2 instead of log
    // ln(exp(1)) == 1 with log, but ≈ 1.4427 with log2
  };

  GUM_TEST(ConstantInt) {
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

  GUM_TEST(ConstantFloat) {
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

  GUM_TEST(Multiplication) {
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

  GUM_TEST(Division) {
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

  GUM_TEST(Addition) {
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

  GUM_TEST(Substraction) {
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

  GUM_TEST(SimplePriority1) {
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

  GUM_TEST(SimplePriority2) {
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

  GUM_TEST(ComplexPriority) {
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

  GUM_TEST(MinusSign) {
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

  GUM_TEST(Exp) {
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

  GUM_TEST(Log) {
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

  GUM_TEST(Ln) {
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

  GUM_TEST(LnNatural) {
    try {
      gum::Formula formula("ln(exp(1))");
      GUM_CHECK_ASSERT_THROWS_NOTHING(formula.result());
      CHECK(formula.result() == doctest::Approx(1.0).epsilon(1e-9));
    } catch (gum::Exception&) { CHECK(false); }
  }

  GUM_TEST(Sqrt) {
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

  GUM_TEST(Pow) {
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

  GUM_TEST(Goal) {
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

  GUM_TEST(Variables) {
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

  GUM_TEST(ScientificNotation1) {
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

  GUM_TEST(ScientificNotation2) {
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

  GUM_TEST(ScientificNotation3) {
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

  GUM_TEST(OperatorUnary) {
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

  GUM_TEST(OperatorPlus) {
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

  GUM_TEST(OperatorMinus) {
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

  GUM_TEST(OperatorTimes) {
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

  GUM_TEST(OperatorDivides) {
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

  GUM_TEST(DoubleImplicitConversion) {
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

  GUM_TEST(DoubleExplicitConversion) {
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

  GUM_TEST(DoubleInitialisation) {
    // Arrange
    try {
      auto expected = 3.14;
      // Act
      gum::Formula f = 3.14;
      // Assert
      CHECK(((double)f) == doctest::Approx(expected).epsilon(1e-6));
    } catch (gum::Exception&) { CHECK(false); }
  }

  GUM_TEST(ToString) {
    // Arrange
    auto f        = gum::Formula("2*5+6");
    auto expected = std::to_string(2.0 * 5.0 + 6.0);
    auto result   = std::string();
    // Act
    GUM_CHECK_ASSERT_THROWS_NOTHING(result = gum::to_string(f));
    // Assert
    CHECK_EQ(result, expected);
  }

  GUM_TEST(ToStream) {
    // Arrange
    auto              f        = gum::Formula("2*5+6");
    auto              expected = std::to_string(2 * 5 + 6);
    std::stringstream result;
    // Act
    GUM_CHECK_ASSERT_THROWS_NOTHING(result << f);
    // Assert
    CHECK_EQ(result.str(), expected);
  }
}   // namespace gum_tests
