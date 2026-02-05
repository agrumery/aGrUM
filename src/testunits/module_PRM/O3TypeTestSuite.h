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
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/O3prmReader.h>
#include <agrum/PRM/PRM.h>

#include <agrum/base/core/math/math_utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  O3Type
#define GUM_CURRENT_MODULE PRM

namespace gum_tests {

  struct O3TypeTestSuite {
    public:
    static void testSimpleType() {   // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isType("t_state"));
      auto state = prm.type("t_state");
      CHECK((state.variable().domainSize()) == (static_cast< gum::Size >(2)));
      CHECK((state.variable().label(0)) == ("OK"));
      CHECK((state.variable().label(1)) == ("NOK"));
    }   // namespace gum_tests

    static void testSimpleTypeError1() {
      // Arrange
      std::stringstream input;
      input << "types t_state labels(OK, NOK);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 1| Error : invalid declaration" << std::endl;
      CHECK((output.str()) == (msg.str()));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
    }

    static void testSimpleTypeError2() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK)";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 29| Error : semicolon expected" << std::endl;
      CHECK((output.str()) == (msg.str()));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
    }

    static void testSimpleTypeError3() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK)" << std::endl << "type t_ink labels(empty, full);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|2 col 1| Error : semicolon expected" << std::endl;
      CHECK((output.str()) == (msg.str()));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
      CHECK(!prm.isType("t_ink"));
    }

    static void testSimpleTypeError4() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 23| Error : comma expected" << std::endl;
      CHECK((output.str()) == (msg.str()));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
    }

    static void testSimpleTypeError5() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK,);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 24| Error : invalid declaration" << std::endl;
      CHECK((output.str()) == (msg.str()));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
    }

    static void testSimpleTypeError6() {
      // Arrange
      std::stringstream input;
      input << "type +==+ labels(OK, NOK);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 6| Error : label expected" << std::endl;
      CHECK((output.str()) == (msg.str()));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
    }

    static void testSimpleTypeError7() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(+=%./, NOK);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 21| Error : invalid declaration" << std::endl;
      CHECK((output.str()) == (msg.str()));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
    }

    static void testSimpleTypeError8() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, +NOK);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 25| Error : invalid declaration" << std::endl;
      CHECK((output.str()) == (msg.str()));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
    }

    static void testSimpleTypeError9() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl << "type t_state labels(OK, YEA, GOO);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|2 col 6| Error : Type t_state exists already" << std::endl;
      CHECK((output.str()) == (msg.str()));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
    }

    static void testExtendedType() {
      // Arrange
      std::stringstream input;
      input << "type t_state extends boolean ("
            << "OK: true,"
            << "NOK: false);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isType("t_state"));
      const auto& boolean = prm.type("boolean");
      const auto& state   = prm.type("t_state");
      CHECK(state.isSubTypeOf(boolean));
      const auto& map = state.label_map();
      CHECK((map.size()) == (static_cast< gum::Size >(2)));
      CHECK((map[0]) == (static_cast< gum::Size >(1)));
      CHECK((map[1]) == (static_cast< gum::Size >(0)));
    }

    static void testExtendedTypeError1() {
      // Arrange
      std::stringstream input;
      input << "type t_state extend boolean ("
            << "OK: true,"
            << "NOK: false);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 21| Error : comma expected";
      CHECK((line) == (msg.str()));
    }

    static void testExtendedTypeError2() {
      // Arrange
      std::stringstream input;
      input << "type t_state extends foobar ("   //
            << "OK: true,"                       //
            << "NOK: false);";                   //
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
      std::stringstream msg;
      msg << "|1 col 22| Error : Unknown type foobar" << std::endl;
      CHECK((output.str()) == (msg.str()));
    }

    static void testExtendedTypeError3() {
      // Arrange
      std::stringstream input;
      input << "type t_state extends boolean (" << std::endl
            << "OK: vrue," << std::endl
            << "NOK: false);" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_state"));
      std::stringstream msg;
      msg << "|2 col 5| Error : Unknown label vrue in boolean" << std::endl;
      CHECK((output.str()) == (msg.str()));
    }

    static void testOrderDoesNotMatter() {
      // Arrange
      std::stringstream input;
      input << "type t_degraded extends t_state (" << std::endl
            << "OK: OK," << std::endl
            << "Dysfunctional: NOK," << std::endl
            << "Degraded: NOK);" << std::endl;
      input << "type t_state extends boolean(" << std::endl
            << "OK: true," << std::endl
            << "NOK: false);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(3)));
      CHECK(prm.isType("t_state"));
      CHECK(prm.isType("t_degraded"));
      CHECK((output.str()) == (""));
    }

    static void testIntType() {
      // Arrange
      std::stringstream input;
      input << "type t_power int (0,9);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isType("t_power"));
      auto power = prm.type("t_power");
      CHECK((power.variable().domainSize()) == (static_cast< gum::Size >(10)));
      CHECK((power.variable().label(0)) == ("0"));
      CHECK((power.variable().label(1)) == ("1"));
      CHECK((power.variable().label(2)) == ("2"));
      CHECK((power.variable().label(3)) == ("3"));
      CHECK((power.variable().label(4)) == ("4"));
      CHECK((power.variable().label(5)) == ("5"));
      CHECK((power.variable().label(6)) == ("6"));
      CHECK((power.variable().label(7)) == ("7"));
      CHECK((power.variable().label(8)) == ("8"));
      CHECK((power.variable().label(9)) == ("9"));
    }

    static void testIntTypeWithNegatives() {
      try {
        // Arrange
        std::stringstream input;
        input << "type t_power int (-9,9);";
        std::stringstream       output;
        gum::prm::PRM< double > prm;
        auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
        // Act
        GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
        // Assert
        CHECK((output.str()) == (""));
        CHECK((prm.types().size()) == (static_cast< gum::Size >(2)));
        CHECK(prm.isType("t_power"));
        auto power = prm.type("t_power");
        CHECK((power.variable().domainSize()) == (static_cast< gum::Size >(19)));
        CHECK((power.variable().label(0)) == ("-9"));
        CHECK((power.variable().label(1)) == ("-8"));
        CHECK((power.variable().label(2)) == ("-7"));
        CHECK((power.variable().label(3)) == ("-6"));
        CHECK((power.variable().label(4)) == ("-5"));
        CHECK((power.variable().label(5)) == ("-4"));
        CHECK((power.variable().label(6)) == ("-3"));
        CHECK((power.variable().label(7)) == ("-2"));
        CHECK((power.variable().label(8)) == ("-1"));
        CHECK((power.variable().label(9)) == ("0"));
        CHECK((power.variable().label(10)) == ("1"));
        CHECK((power.variable().label(11)) == ("2"));
        CHECK((power.variable().label(12)) == ("3"));
        CHECK((power.variable().label(13)) == ("4"));
        CHECK((power.variable().label(14)) == ("5"));
        CHECK((power.variable().label(15)) == ("6"));
        CHECK((power.variable().label(16)) == ("7"));
        CHECK((power.variable().label(17)) == ("8"));
      } catch (gum::OutOfBounds& e) { GUM_SHOWERROR(e); }
    }

    static void testIntTypeError1() {
      // Arrange
      std::stringstream input;
      input << "type t_power ints (0,9);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 19| Error : comma expected";
      CHECK((line) == (msg.str()));
    }

    static void testIntTypeError2() {
      // Arrange
      std::stringstream input;
      input << "type t_power int 0,9;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 18| Error : \"(\" expected";
      CHECK((line) == (msg.str()));
    }

    static void testIntTypeError3() {
      // Arrange
      std::stringstream input;
      input << "type t_power int (0 9);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 21| Error : comma expected";
      CHECK((line) == (msg.str()));
    }

    static void testIntTypeError4() {
      // Arrange
      std::stringstream input;
      input << "type +==++ int (0, 9);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 6| Error : label expected";
      CHECK((line) == (msg.str()));
    }

    static void testIntTypeError5() {
      // Arrange
      std::stringstream input;
      input << "type t_power int (plop, 9);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 19| Error : integer expected";
      CHECK((line) == (msg.str()));
    }

    static void testIntTypeError6() {
      // Arrange
      std::stringstream input;
      input << "type t_power int (0, 9, 15);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 23| Error : \")\" expected";
      CHECK((line) == (msg.str()));
    }

    static void testIntTypeError7() {
      // Arrange
      std::stringstream input;
      input << "type t_power int (0.0, 9);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 19| Error : integer expected";
      CHECK((line) == (msg.str()));
    }

    static void testIntTypeError8() {
      // Arrange
      std::stringstream input;
      input << "type t_power int (9, 9)";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 24| Error : semicolon expected";
      CHECK((line) == (msg.str()));
    }

    static void testIntTypeError9() {
      // Arrange
      std::stringstream input;
      input << "type t_power int (10, 9)";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 25| Error : semicolon expected";
      CHECK((line) == (msg.str()));
    }

    static void testIntTypeError10() {
      // Arrange
      std::stringstream input;
      input << "type t_power int (9, 0)";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((prm.types().size()) == (static_cast< gum::Size >(1)));
      CHECK(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 24| Error : semicolon expected";
      CHECK((line) == (msg.str()));
    }

    static void testTypeInModule1() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output, "fr.agrum"));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(2)));
      CHECK(!prm.isType("t_state"));
      CHECK(prm.isType("fr.agrum.t_state"));
    }

    static void testTypeInModule2() {
      // Arrange
      std::stringstream input;
      input << "type t_degraded extends t_state (" << std::endl
            << "OK: OK," << std::endl
            << "Dysfunctional: NOK," << std::endl
            << "Degraded: NOK);" << std::endl;
      input << "type t_state extends boolean (" << std::endl
            << "OK: true," << std::endl
            << "NOK: false);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output, "fr.agrum"));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(3)));
      CHECK(!prm.isType("t_state"));
      CHECK(prm.isType("fr.agrum.t_state"));
      CHECK(!prm.isType("t_degraded"));
      CHECK(prm.isType("fr.agrum.t_degraded"));
    }

    static void testRangeType() {
      // Arrange
      std::stringstream input;
      input << "type range int(1, 10);" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isType("range"));
      const auto& range = prm.type("range");
      CHECK((range.variable().labels().size()) == (static_cast< gum::Size >(10)));
      CHECK((range.variable().varType()) == (gum::VarType::RANGE));
      CHECK((range.variable().labels().at(0)) == ("1"));
      CHECK((range.variable().labels().at(1)) == ("2"));
      CHECK((range.variable().labels().at(2)) == ("3"));
      CHECK((range.variable().labels().at(9)) == ("10"));
    }

    static void testRealType1() {
      // Arrange
      std::stringstream input;
      input << "type angle real(0, 90, 180);" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isType("angle"));
      const auto& angle = prm.type("angle");
      CHECK((angle.variable().varType()) == (gum::VarType::DISCRETIZED));
      CHECK((angle.variable().labels().size()) == (static_cast< gum::Size >(2)));
      CHECK((angle.variable().labels().at(0)) == ("[0;90["));
      CHECK((angle.variable().labels().at(1)) == ("[90;180]"));
    }

    static void testRealType2() {
      // Arrange
      std::stringstream input;
      input << "type angle real(0, 90, 180, 360);" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.types().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isType("angle"));
      const auto& angle = prm.type("angle");
      CHECK((angle.variable().labels().size()) == (static_cast< gum::Size >(3)));
      CHECK((angle.variable().labels().at(0)) == ("[0;90["));
      CHECK((angle.variable().labels().at(1)) == ("[90;180["));
      CHECK((angle.variable().labels().at(2)) == ("[180;360]"));
    }

    static void testRealTypeError1() {
      // Arrange
      std::stringstream input;
      input << "type angle real(0, 90);" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 1| Error : Found 2 values in range expected at least 3" << std::endl;
      CHECK((output.str()) == (msg.str()));
    }

    static void testRealTypeError2() {
      // Arrange
      std::stringstream input;
      input << "type angle real(0);" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 1| Error : Found 1 values in range expected at least 3" << std::endl;
      CHECK((output.str()) == (msg.str()));
    }

    static void testRealTypeError3() {
      // Arrange
      std::stringstream input;
      input << "type angle real(0, plop);" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 20| Error : invalid declaration";
      CHECK((line) == (msg.str()));
    }
  };

  GUM_TEST_ACTIF(SimpleType)
  GUM_TEST_ACTIF(SimpleTypeError1)
  GUM_TEST_ACTIF(SimpleTypeError2)
  GUM_TEST_ACTIF(SimpleTypeError3)
  GUM_TEST_ACTIF(SimpleTypeError4)
  GUM_TEST_ACTIF(SimpleTypeError5)
  GUM_TEST_ACTIF(SimpleTypeError6)
  GUM_TEST_ACTIF(SimpleTypeError7)
  GUM_TEST_ACTIF(SimpleTypeError8)
  GUM_TEST_ACTIF(SimpleTypeError9)
  GUM_TEST_ACTIF(ExtendedType)
  GUM_TEST_ACTIF(ExtendedTypeError1)
  GUM_TEST_ACTIF(ExtendedTypeError2)
  GUM_TEST_ACTIF(ExtendedTypeError3)
  GUM_TEST_ACTIF(OrderDoesNotMatter)
  GUM_TEST_ACTIF(IntType)
  GUM_TEST_ACTIF(IntTypeWithNegatives)
  GUM_TEST_ACTIF(IntTypeError1)
  GUM_TEST_ACTIF(IntTypeError2)
  GUM_TEST_ACTIF(IntTypeError3)
  GUM_TEST_ACTIF(IntTypeError4)
  GUM_TEST_ACTIF(IntTypeError5)
  GUM_TEST_ACTIF(IntTypeError6)
  GUM_TEST_ACTIF(IntTypeError7)
  GUM_TEST_ACTIF(IntTypeError8)
  GUM_TEST_ACTIF(IntTypeError9)
  GUM_TEST_ACTIF(IntTypeError10)
  GUM_TEST_ACTIF(TypeInModule1)
  GUM_TEST_ACTIF(TypeInModule2)
  GUM_TEST_ACTIF(RangeType)
  GUM_TEST_ACTIF(RealType1)
  GUM_TEST_ACTIF(RealType2)
  GUM_TEST_ACTIF(RealTypeError1)
  GUM_TEST_ACTIF(RealTypeError2)
  GUM_TEST_ACTIF(RealTypeError3)
}   // namespace gum_tests
