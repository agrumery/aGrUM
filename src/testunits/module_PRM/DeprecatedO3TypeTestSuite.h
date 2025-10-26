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
#pragma once


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/O3prmReader.h>
#include <agrum/PRM/PRM.h>

#include <agrum/base/core/math/math_utils.h>

namespace gum_tests {

  class GUM_TEST_SUITE(DeprecatedO3Type) {
    public:
    void setUp() {}

    void tearDown() {}

    GUM_ACTIVE_TEST(SimpleType) {   // Arrange
      std::stringstream input;
      input << "type t_state OK, NOK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_state is declared using a deprecated syntax.";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(2))
      TS_ASSERT(prm.isType("t_state"))
      auto state = prm.type("t_state");
      TS_ASSERT_EQUALS(state.variable().domainSize(), static_cast< gum::Size >(2))
      TS_ASSERT_EQUALS(state.variable().label(0), "OK")
      TS_ASSERT_EQUALS(state.variable().label(1), "NOK")
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(SimpleTypeError1) {
      // Arrange
      std::stringstream input;
      input << "types t_state OK, NOK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 1| Error : invalid declaration";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
    }

    GUM_ACTIVE_TEST(SimpleTypeError2) {
      // Arrange
      std::stringstream input;
      input << "type t_state OK, NOK";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 21| Error : semicolon expected" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
    }

    GUM_ACTIVE_TEST(SimpleTypeError3) {
      // Arrange
      std::stringstream input;
      input << "type t_state OK, NOK" << std::endl << "type t_ink empty, full;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|2 col 1| Error : semicolon expected" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
      TS_ASSERT(!prm.isType("t_ink"))
    }

    GUM_ACTIVE_TEST(SimpleTypeError4) {
      // Arrange
      std::stringstream input;
      input << "type t_state OK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 16| Error : comma expected" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
    }

    GUM_ACTIVE_TEST(SimpleTypeError5) {
      // Arrange
      std::stringstream input;
      input << "type t_state OK,;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 17| Error : invalid declaration" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
    }

    GUM_ACTIVE_TEST(SimpleTypeError6) {
      // Arrange
      std::stringstream input;
      input << "type +==+ OK, NOK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 6| Error : label expected" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
    }

    GUM_ACTIVE_TEST(SimpleTypeError7) {
      // Arrange
      std::stringstream input;
      input << "type t_state +=%./, NOK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 14| Error : invalid declaration" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
    }

    GUM_ACTIVE_TEST(SimpleTypeError8) {
      // Arrange
      std::stringstream input;
      input << "type t_state OK, +NOK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 18| Error : invalid declaration" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
    }

    GUM_ACTIVE_TEST(SimpleTypeError9) {
      // Arrange
      std::stringstream input;
      input << "type t_state OK, NOK;" << std::endl << "type t_state OK, YEA, GOO;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_state is declared using a deprecated syntax." << std::endl;
      msg << "|2 col 6| Warning : t_state is declared using a deprecated syntax." << std::endl;
      msg << "|2 col 6| Error : Type t_state exists already" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
    }

    GUM_ACTIVE_TEST(ExtendedType) {
      // Arrange
      std::stringstream input;
      input << "type t_state extends boolean "
            << "OK: true,"
            << "NOK: false;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_state is declared using a deprecated syntax." << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(2))
      TS_ASSERT(prm.isType("t_state"))
      const auto& boolean = prm.type("boolean");
      const auto& state   = prm.type("t_state");
      TS_ASSERT(state.isSubTypeOf(boolean))
      const auto& map = state.label_map();
      TS_ASSERT_EQUALS(map.size(), static_cast< gum::Size >(2))
      TS_ASSERT_EQUALS(map[0], static_cast< gum::Size >(1))
      TS_ASSERT_EQUALS(map[1], static_cast< gum::Size >(0))
    }

    GUM_ACTIVE_TEST(ExtendedTypeError1) {
      // Arrange
      std::stringstream input;
      input << "type t_state extend boolean "
            << "OK: true,"
            << "NOK: false;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 21| Error : comma expected";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(ExtendedTypeError2) {
      // Arrange
      std::stringstream input;
      input << "type t_state extends foobar "
            << "OK: true,"
            << "NOK: false;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_state is declared using a deprecated syntax." << std::endl;
      msg << "|1 col 22| Error : Unknown type foobar" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
    }

    GUM_ACTIVE_TEST(ExtendedTypeError3) {
      // Arrange
      std::stringstream input;
      input << "type t_state extends boolean " << std::endl
            << "OK: vrue," << std::endl
            << "NOK: false;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_state"))
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_state is declared using a deprecated syntax." << std::endl;
      msg << "|2 col 5| Error : Unknown label vrue in boolean" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
    }

    GUM_ACTIVE_TEST(OrderDoesNotMatter) {
      // Arrange
      std::stringstream input;
      input << "type t_degraded extends t_state " << std::endl
            << "OK: OK," << std::endl
            << "Dysfunctional: NOK," << std::endl
            << "Degraded: NOK;" << std::endl;
      input << "type t_state extends boolean" << std::endl
            << "OK: true," << std::endl
            << "NOK: false;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(3))
      TS_ASSERT(prm.isType("t_state"))
      TS_ASSERT(prm.isType("t_degraded"))
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_degraded is declared using a deprecated "
             "syntax."
          << std::endl;
      msg << "|5 col 6| Warning : t_state is declared using a deprecated syntax." << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
    }

    GUM_ACTIVE_TEST(IntType) {
      // Arrange
      std::stringstream input;
      input << "int (0,9) t_power;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 11| Warning : t_power is declared using a deprecated syntax." << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(2))
      TS_ASSERT(prm.isType("t_power"))
      auto power = prm.type("t_power");
      TS_ASSERT_EQUALS(power.variable().domainSize(), static_cast< gum::Size >(10))
      TS_ASSERT_EQUALS(power.variable().label(0), "0")
      TS_ASSERT_EQUALS(power.variable().label(1), "1")
      TS_ASSERT_EQUALS(power.variable().label(2), "2")
      TS_ASSERT_EQUALS(power.variable().label(3), "3")
      TS_ASSERT_EQUALS(power.variable().label(4), "4")
      TS_ASSERT_EQUALS(power.variable().label(5), "5")
      TS_ASSERT_EQUALS(power.variable().label(6), "6")
      TS_ASSERT_EQUALS(power.variable().label(7), "7")
      TS_ASSERT_EQUALS(power.variable().label(8), "8")
      TS_ASSERT_EQUALS(power.variable().label(9), "9")
    }

    GUM_ACTIVE_TEST(IntTypeWithNegatives) {
      try {
        // Arrange
        std::stringstream input;
        input << "int (-9,9) t_power;";
        std::stringstream       output;
        gum::prm::PRM< double > prm;
        auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
        // Assert
        std::stringstream msg;
        msg << "|1 col 12| Warning : t_power is declared using a deprecated "
               "syntax."
            << std::endl;
        TS_ASSERT_EQUALS(output.str(), msg.str())
        TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(2))
        TS_ASSERT(prm.isType("t_power"))
        auto power = prm.type("t_power");
        TS_ASSERT_EQUALS(power.variable().domainSize(), static_cast< gum::Size >(19))
        TS_ASSERT_EQUALS(power.variable().label(0), "-9")
        TS_ASSERT_EQUALS(power.variable().label(1), "-8")
        TS_ASSERT_EQUALS(power.variable().label(2), "-7")
        TS_ASSERT_EQUALS(power.variable().label(3), "-6")
        TS_ASSERT_EQUALS(power.variable().label(4), "-5")
        TS_ASSERT_EQUALS(power.variable().label(5), "-4")
        TS_ASSERT_EQUALS(power.variable().label(6), "-3")
        TS_ASSERT_EQUALS(power.variable().label(7), "-2")
        TS_ASSERT_EQUALS(power.variable().label(8), "-1")
        TS_ASSERT_EQUALS(power.variable().label(9), "0")
        TS_ASSERT_EQUALS(power.variable().label(10), "1")
        TS_ASSERT_EQUALS(power.variable().label(11), "2")
        TS_ASSERT_EQUALS(power.variable().label(12), "3")
        TS_ASSERT_EQUALS(power.variable().label(13), "4")
        TS_ASSERT_EQUALS(power.variable().label(14), "5")
        TS_ASSERT_EQUALS(power.variable().label(15), "6")
        TS_ASSERT_EQUALS(power.variable().label(16), "7")
        TS_ASSERT_EQUALS(power.variable().label(17), "8")
      } catch (gum::OutOfBounds& e) { GUM_SHOWERROR(e); }
    }

    GUM_ACTIVE_TEST(IntTypeError1) {
      // Arrange
      std::stringstream input;
      input << "ints (0,9) t_power;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_power"))
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 1| Error : invalid declaration";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(IntTypeError2) {
      // Arrange
      std::stringstream input;
      input << "int 0,9 t_power;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_power"))
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 5| Error : \"(\" expected";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(IntTypeError3) {
      // Arrange
      std::stringstream input;
      input << "int (0 9) t_power;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_power"))
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 8| Error : comma expected";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(IntTypeError4) {
      // Arrange
      std::stringstream input;
      input << "int (0, 9) +==++;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_power"))
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 12| Error : label expected";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(IntTypeError5) {
      // Arrange
      std::stringstream input;
      input << "int (plop, 9) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_power"))
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 6| Error : integer expected";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(IntTypeError6) {
      // Arrange
      std::stringstream input;
      input << "int (0, 9, 15) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_power"))
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 10| Error : \")\" expected";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(IntTypeError7) {
      // Arrange
      std::stringstream input;
      input << "int (0.0, 9) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_power"))
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 6| Error : integer expected";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(IntTypeError8) {
      // Arrange
      std::stringstream input;
      input << "int (9, 9) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_power"))
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 19| Error : semicolon expected";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(IntTypeError9) {
      // Arrange
      std::stringstream input;
      input << "int (10, 9) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_power"))
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 20| Error : semicolon expected";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(IntTypeError10) {
      // Arrange
      std::stringstream input;
      input << "int (9, 0) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(1))
      TS_ASSERT(!prm.isType("t_power"))
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 19| Error : semicolon expected";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(TypeInModule1) {
      // Arrange
      std::stringstream input;
      input << "type t_state OK, NOK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output, "fr.agrum"))
      // Assert
      std::stringstream msg;
      msg << "|1 col 6| Warning : fr.agrum.t_state is declared using a deprecated "
             "syntax."
          << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(2))
      TS_ASSERT(!prm.isType("t_state"))
      TS_ASSERT(prm.isType("fr.agrum.t_state"))
    }

    GUM_ACTIVE_TEST(TypeInModule2) {
      // Arrange
      std::stringstream input;
      input << "type t_degraded extends t_state " << std::endl
            << "OK: OK," << std::endl
            << "Dysfunctional: NOK," << std::endl
            << "Degraded: NOK;" << std::endl;
      input << "type t_state extends boolean" << std::endl
            << "OK: true," << std::endl
            << "NOK: false;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output, "fr.agrum"))
      // Assert
      std::stringstream msg;
      msg << "|1 col 6| Warning : fr.agrum.t_degraded is declared using a "
             "deprecated syntax."
          << std::endl;
      msg << "|5 col 6| Warning : fr.agrum.t_state is declared using a deprecated "
             "syntax."
          << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(3))
      TS_ASSERT(!prm.isType("t_state"))
      TS_ASSERT(prm.isType("fr.agrum.t_state"))
      TS_ASSERT(!prm.isType("t_degraded"))
      TS_ASSERT(prm.isType("fr.agrum.t_degraded"))
    }

    GUM_ACTIVE_TEST(RealType1) {
      // Arrange
      std::stringstream input;
      input << "real(0, 90, 180) angle;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 18| Warning : angle is declared using a deprecated syntax." << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(2))
      TS_ASSERT(prm.isType("angle"))
      const auto& angle = prm.type("angle");
      TS_ASSERT_EQUALS(angle.variable().labels().size(), static_cast< gum::Size >(2))
      TS_ASSERT_EQUALS(angle.variable().labels().at(0), "[0;90[")
      TS_ASSERT_EQUALS(angle.variable().labels().at(1), "[90;180]")
    }

    GUM_ACTIVE_TEST(RealType2) {
      // Arrange
      std::stringstream input;
      input << "real(0, 90, 180, 360) angle;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 23| Warning : angle is declared using a deprecated syntax." << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
      TS_ASSERT_EQUALS(prm.types().size(), static_cast< gum::Size >(2))
      TS_ASSERT(prm.isType("angle"))
      const auto& angle = prm.type("angle");
      TS_ASSERT_EQUALS(angle.variable().labels().size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(angle.variable().labels().at(0), "[0;90[")
      TS_ASSERT_EQUALS(angle.variable().labels().at(1), "[90;180[")
      TS_ASSERT_EQUALS(angle.variable().labels().at(2), "[180;360]")
    }

    GUM_ACTIVE_TEST(RealTypeError1) {
      // Arrange
      std::stringstream input;
      input << "real(0, 90) angle;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 13| Warning : angle is declared using a deprecated syntax." << std::endl;
      msg << "|1 col 1| Error : Found 2 values in range expected at least 3" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
    }

    GUM_ACTIVE_TEST(RealTypeError2) {
      // Arrange
      std::stringstream input;
      input << "real(0) angle;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::stringstream msg;
      msg << "|1 col 9| Warning : angle is declared using a deprecated syntax." << std::endl;
      msg << "|1 col 1| Error : Found 1 values in range expected at least 3" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str())
    }

    GUM_ACTIVE_TEST(RealTypeError3) {
      // Arrange
      std::stringstream input;
      input << "real(0, plop) angle;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 9| Error : invalid declaration";
      TS_ASSERT_EQUALS(line, msg.str())
    }
  };
}   // namespace gum_tests
