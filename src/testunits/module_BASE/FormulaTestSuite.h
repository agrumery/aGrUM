
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/core/math/formula.h>

namespace gum_tests {

  class MultiDimFormulaTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testConstantInt() {
      try {
        // Arrange
        std::string  eq = "1";
        double       expected = 1;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testConstantFloat() {
      try {
        // Arrange
        std::string  eq = "0.99";
        double       expected = 0.99;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testMultiplication() {
      try {
        // Arrange
        std::string  eq = "10 * 2";
        double       expected = 20.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testDivision() {
      try {
        // Arrange
        std::string  eq = "10 / 2";
        double       expected = 5.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testAddition() {
      try {
        // Arrange
        std::string  eq = "10 + 2";
        double       expected = 12.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testSubstraction() {
      try {
        // Arrange
        std::string  eq = "10 - 2";
        double       expected = 8.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testSimplePriority1() {
      try {
        // Arrange
        std::string  eq = "10 - 2 * 5";
        double       expected = 0.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testSimplePriority2() {
      try {
        // Arrange
        std::string  eq = "10 * 2 - 5";
        double       expected = 15.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testComplexPriority() {
      try {
        // Arrange
        std::string  eq = "5 + ( ( 1 + 2 ) * 4 ) - 3";
        double       expected = 14.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testMinusSign() {
      try {
        // Arrange
        std::string  eq = "-3";
        double       expected = -3.0;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testExp() {
      try {
        // Arrange
        std::string  eq = "exp(0.001)";
        double       expected = 1.0010005;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testLog() {
      try {
        // Arrange
        std::string  eq = "1 + log(1)";
        double       expected = 1;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testLn() {
      try {
        // Arrange
        std::string  eq = "2 + ln(1)";
        double       expected = 2;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testSqrt() {
      try {
        // Arrange
        std::string  eq = "1 + sqrt(4)";
        double       expected = 3;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testPow() {
      try {
        // Arrange
        std::string  eq = "1 + pow(2,2)";
        double       expected = 5;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testGoal() {
      try {
        // Arrange
        std::string  eq = "1-exp(-0.001*2)";
        double       expected = 1 - std::exp(-0.001 * 2);
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testVariables() {
      try {
        // Arrange
        double       lambda = 0.001;
        double       t = 2;
        double       unused = 10;
        std::string  eq = "1-exp(-lambda*t)";
        double       expected = 1 - std::exp(-lambda * t);
        gum::Formula formula(eq);
        formula.variables().insert("lambda", lambda);
        formula.variables().insert("t", t);
        formula.variables().insert("unused", unused);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT(formula.variables().exists("lambda"));
        TS_ASSERT(formula.variables().exists("t"));
        TS_ASSERT(formula.variables().exists("unused"));
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testScientificNotation1() {
      try {
        // Arrange
        std::string  eq = "3.72663E-06";
        double       expected = 3.72663E-06;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testScientificNotation2() {
      try {
        // Arrange
        std::string  eq = "1 - 1.0e-9";
        double       expected = 1 - 1e-9;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testScientificNotation3() {
      try {
        // Arrange
        std::string  eq = "1 - 1e-9";
        double       expected = 1 - 1e-9;
        gum::Formula formula(eq);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(formula.result());
        // Assert
        TS_ASSERT_DELTA(formula.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testOperatorUnary() {
      try {
        // Arrange
        auto a = gum::Formula("2*5-6");
        auto expected = -(2.0 * 5.0 - 6.0);
        auto result = gum::Formula("0");
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = -a);
        // Assert
        TS_ASSERT_DELTA(result.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testOperatorPlus() {
      try {
        // Arrange
        auto a = gum::Formula("2*5-6");
        auto b = gum::Formula("2/8");
        auto expected = 2.0 * 5.0 - 6.0 + 2.0 / 8.0;
        auto result = gum::Formula("0");
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = a + b);
        // Assert
        TS_ASSERT_DELTA(result.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testOperatorMinus() {
      try {
        // Arrange
        auto a = gum::Formula("2*5-6");
        auto b = gum::Formula("2/8");
        auto expected = 2.0 * 5.0 - 6.0 - 2.0 / 8.0;
        auto result = gum::Formula("0");
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = a - b);
        // Assert
        TS_ASSERT_DELTA(result.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testOperatorTimes() {
      try {
        // Arrange
        auto a = gum::Formula("2*5-6");
        auto b = gum::Formula("2/8");
        auto expected = (2.0 * 5.0 - 6.0) * (2.0 / 8.0);
        auto result = gum::Formula("0");
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = a * b);
        // Assert
        TS_ASSERT_DELTA(result.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testOperatorDivides() {
      try {
        // Arrange
        auto a = gum::Formula("2*5-6");
        auto b = gum::Formula("2/8");
        auto expected = (2.0 * 5.0 - 6.0) / (2.0 / 8.0);
        auto result = gum::Formula("0");
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = a / b);
        // Assert
        TS_ASSERT_DELTA(result.result(), expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testDoubleImplicitConversion() {
      try {
        // Arrange
        auto a = gum::Formula("2*5-6");
        auto expected = 2.0 * 5.0 - 6.0;
        auto result = 0.0;
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = (double)a);
        // Assert
        TS_ASSERT_DELTA(result, expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testDoubleExplicitConversion() {
      try {
        // Arrange
        auto a = gum::Formula("2*5-6");
        auto expected = 2.0 * 5.0 - 6.0;
        auto result = 0.0;
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(result = static_cast< double >(a));
        // Assert
        TS_ASSERT_DELTA(result, expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testDoubleInitialisation() {
      // Arrange
      try {
        auto expected = 3.14;
        // Act
        gum::Formula f = 3.14;
        // Assert
        TS_ASSERT_DELTA((double)f, expected, 1e-6);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testToString() {
      // Arrange
      auto f = gum::Formula("2*5+6");
      auto expected = std::to_string(2.0 * 5.0 + 6.0);
      auto result = std::string();
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(result = gum::to_string(f));
      // Assert
      TS_ASSERT_EQUALS(result, expected);
    }

    void testToStream() {
      // Arrange
      auto              f = gum::Formula("2*5+6");
      auto              expected = std::to_string(2 * 5 + 6);
      std::stringstream result;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(result << f);
      // Assert
      TS_ASSERT_EQUALS(result.str(), expected);
    }
  };
}   // namespace gum_tests
