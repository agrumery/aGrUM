/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <iostream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/exceptions.h>
#include <agrum/multidim/formula/Parser.h>

namespace gum_tests {

  class MultiDimFormulaTestSuite: public CxxTest::TestSuite {

    public:
      void setUp() {

      }

      void tearDown() {

      }

      void testMultiplication() {
        try {
          // Arrange
          std::string eq = "10 * 2";
          double expected = 20.0;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testDivision() {
        try {
          // Arrange
          std::string eq = "10 / 2";
          double expected = 5.0;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testAddition() {
        try {
          // Arrange
          std::string eq = "10 + 2";
          double expected = 12.0;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testSubstraction() {
        try {
          // Arrange
          std::string eq = "10 - 2";
          double expected = 8.0;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testSimplePriority1() {
        try {
          // Arrange
          std::string eq = "10 - 2 * 5";
          double expected = 0.0;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testSimplePriority2() {
        try {
          // Arrange
          std::string eq = "10 * 2 - 5";
          double expected = 15.0;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testComplexPriority() {
        try {
          // Arrange
          std::string eq = "5 + ( ( 1 + 2 ) * 4 ) - 3";
          double expected = 14.0;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_GUM_ASSERT_THROWS_NOTHING( parser.formula().result() );
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testMinusSign() {
        try {
          // Arrange
          std::string eq = "-3";
          double expected = -3.0;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_GUM_ASSERT_THROWS_NOTHING( parser.formula().result() );
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testExp() {
        try {
          // Arrange
          std::string eq = "exp(0.001)";
          double expected = 1.0010005;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_GUM_ASSERT_THROWS_NOTHING( parser.formula().result() );
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testLog() {
        try {
          // Arrange
          std::string eq = "1 + log(1)";
          double expected = 1;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_GUM_ASSERT_THROWS_NOTHING( parser.formula().result() );
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testLn() {
        try {
          // Arrange
          std::string eq = "2 + ln(1)";
          double expected = 2;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_GUM_ASSERT_THROWS_NOTHING( parser.formula().result() );
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testSqrt() {
        try {
          // Arrange
          std::string eq = "1 + sqrt(4)";
          double expected = 3;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_GUM_ASSERT_THROWS_NOTHING( parser.formula().result() );
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testPow() {
        try {
          // Arrange
          std::string eq = "1 + pow(2,2)";
          double expected = 5;
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_GUM_ASSERT_THROWS_NOTHING( parser.formula().result() );
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testGoal() {
        try {
          // Arrange
          std::string eq = "1-exp(-0.001*2)";
          double expected = 1 - std::exp(-0.001 * 2);
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_GUM_ASSERT_THROWS_NOTHING( parser.formula().result() );
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }

      void testVariables() {
        try {
          // Arrange
          double lambda = 0.001;
          double t = 2;
          double unused = 10;
          std::string eq = "1-exp(-lambda*t)";
          double expected = 1 - std::exp(-lambda* t);
          gum::Scanner scanner((unsigned char*)eq.c_str(), (int) (eq.size()));
          gum::Parser parser(&scanner);
          parser.formula().variables().insert("lambda", lambda);
          parser.formula().variables().insert("t", t);
          parser.formula().variables().insert("unused", unused);
          // Act
          TS_GUM_ASSERT_THROWS_NOTHING(parser.Parse());
          // Assert
          TS_ASSERT( parser.formula().variables().exists("lambda") );
          TS_ASSERT( parser.formula().variables().exists("t") );
          TS_ASSERT( parser.formula().variables().exists("unused") );
          TS_GUM_ASSERT_THROWS_NOTHING( parser.formula().result() );
          TS_ASSERT_DELTA( parser.formula().result(), expected, 1e-6 );
        } catch (gum::Exception& e) {
          TS_ASSERT( false );
        }
      }


  };
}
