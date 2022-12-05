/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/core/math/formula.h>

namespace gum_tests {

  class [[maybe_unused]] MultiDimFormulaTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    GUM_TEST(ConstantInt) {
      try {
        // Arrange
        std::string  eq       = "1";
        double       expected = 1;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(ConstantFloat) {
      try {
        // Arrange
        std::string  eq       = "0.99";
        double       expected = 0.99;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(Multiplication) {
      try {
        // Arrange
        std::string  eq       = "10 * 2";
        double       expected = 20.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(Division) {
      try {
        // Arrange
        std::string  eq       = "10 / 2";
        double       expected = 5.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(Addition) {
      try {
        // Arrange
        std::string  eq       = "10 + 2";
        double       expected = 12.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(Substraction) {
      try {
        // Arrange
        std::string  eq       = "10 - 2";
        double       expected = 8.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(SimplePriority1) {
      try {
        // Arrange
        std::string  eq       = "10 - 2 * 5";
        double       expected = 0.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(SimplePriority2) {
      try {
        // Arrange
        std::string  eq       = "10 * 2 - 5";
        double       expected = 15.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(ComplexPriority) {
      try {
        // Arrange
        std::string  eq       = "5 + ( ( 1 + 2 ) * 4 ) - 3";
        double       expected = 14.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(MinusSign) {
      try {
        // Arrange
        std::string  eq       = "-3";
        double       expected = -3.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(Exp) {
      try {
        // Arrange
        std::string  eq       = "exp(0.001)";
        double       expected = 1.0010005;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(Log) {
      try {
        // Arrange
        std::string  eq       = "1 + log(1)";
        double       expected = 1;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(Ln) {
      try {
        // Arrange
        std::string  eq       = "2 + ln(1)";
        double       expected = 2;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(Sqrt) {
      try {
        // Arrange
        std::string  eq       = "1 + sqrt(4)";
        double       expected = 3;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(Pow) {
      try {
        // Arrange
        std::string  eq       = "1 + pow(2,2)";
        double       expected = 5;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(Goal) {
      try {
        // Arrange
        std::string  eq       = "1-exp(-0.001*2)";
        double       expected = 1 - std::exp(-0.001 * 2);
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
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
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT(formula.variables().exists("lambda"))
        TS_ASSERT(formula.variables().exists("t"))
        TS_ASSERT(formula.variables().exists("unused"))
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(ScientificNotation1) {
      try {
        // Arrange
        std::string  eq       = "3.72663E-06";
        double       expected = 3.72663E-06;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(ScientificNotation2) {
      try {
        // Arrange
        std::string  eq       = "1 - 1.0e-9";
        double       expected = 1 - 1e-9;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(ScientificNotation3) {
      try {
        // Arrange
        std::string  eq       = "1 - 1e-9";
        double       expected = 1 - 1e-9;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result())
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(OperatorUnary) {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto expected = -(2.0 * 5.0 - 6.0);
        auto result   = gum::Formula("0");
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = -a)
        // Assert
        TS_ASSERT_DELTA(result.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(OperatorPlus) {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto b        = gum::Formula("2/8");
        auto expected = 2.0 * 5.0 - 6.0 + 2.0 / 8.0;
        auto result   = gum::Formula("0");
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = a + b)
        // Assert
        TS_ASSERT_DELTA(result.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(OperatorMinus) {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto b        = gum::Formula("2/8");
        auto expected = 2.0 * 5.0 - 6.0 - 2.0 / 8.0;
        auto result   = gum::Formula("0");
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = a - b)
        // Assert
        TS_ASSERT_DELTA(result.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(OperatorTimes) {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto b        = gum::Formula("2/8");
        auto expected = (2.0 * 5.0 - 6.0) * (2.0 / 8.0);
        auto result   = gum::Formula("0");
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = a * b)
        // Assert
        TS_ASSERT_DELTA(result.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(OperatorDivides) {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto b        = gum::Formula("2/8");
        auto expected = (2.0 * 5.0 - 6.0) / (2.0 / 8.0);
        auto result   = gum::Formula("0");
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = a / b)
        // Assert
        TS_ASSERT_DELTA(result.result(), expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(DoubleImplicitConversion) {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto expected = 2.0 * 5.0 - 6.0;
        auto result   = 0.0;
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = (double)a)
        // Assert
        TS_ASSERT_DELTA(result, expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(DoubleExplicitConversion) {
      try {
        // Arrange
        auto a        = gum::Formula("2*5-6");
        auto expected = 2.0 * 5.0 - 6.0;
        auto result   = 0.0;
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = static_cast< double >(a))
        // Assert
        TS_ASSERT_DELTA(result, expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(DoubleInitialisation) {
      // Arrange
      try {
        auto expected = 3.14;
        // Act
        gum::Formula f = 3.14;
        // Assert
        TS_ASSERT_DELTA((double)f, expected, 1e-6)
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(ToString) {
      // Arrange
      auto f        = gum::Formula("2*5+6");
      auto expected = std::to_string(2.0 * 5.0 + 6.0);
      auto result   = std::string();
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(result = gum::to_string(f))
      // Assert
      TS_ASSERT_EQUALS(result, expected)
    }

    GUM_TEST(ToStream) {
      // Arrange
      auto              f        = gum::Formula("2*5+6");
      auto              expected = std::to_string(2 * 5 + 6);
      std::stringstream result;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(result << f)
      // Assert
      TS_ASSERT_EQUALS(result.str(), expected)
    }
  };
}   // namespace gum_tests
