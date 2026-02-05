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
#define GUM_CURRENT_SUITE  O3Interface
#define GUM_CURRENT_MODULE PRM

namespace gum_tests {

  struct O3InterfaceTestSuite {
    public:
    static void testEmptyInterface() {   // Arrange
      std::stringstream input;
      input << "interface IBar { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((i_bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(i_bar.super(), const gum::NotFound&);
    }   // namespace gum_tests

    static void testEmptyInterfaceError1() {
      // Arrange
      std::stringstream input;
      input << "interfaces IBar { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 1| Error : invalid declaration";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isInterface("IBar"));
    }

    static void testEmptyInterfaceError2() {
      // Arrange
      std::stringstream input;
      input << "interface { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 11| Error : label expected";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isInterface("IBar"));
    }

    static void testEmptyInterfaceError3() {
      // Arrange
      std::stringstream input;
      input << "interface ++++/ze { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 11| Error : label expected";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isInterface("IBar"));
    }

    static void testSimpleInterface() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "boolean state;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(i_bar.super(), const gum::NotFound&);
    }

    static void testSimpleInterfaceError1() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "t_state state;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 1| Error : Unknown type t_state";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((i_bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(i_bar.super(), const gum::NotFound&);
    }

    static void testSimpleInterfaceError2() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "=%+ state;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 1| Error : \"}\" expected";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isInterface("IBar"));
    }

    static void testSimpleInterfaceError3() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "state;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 6| Error : label expected";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isInterface("IBar"));
    }

    static void testSimpleInterfaceError4() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "boolean state" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|3 col 1| Error : semicolon expected";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isInterface("IBar"));
    }

    static void testSimpleInterfaceError5() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "boolean +/+/;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 9| Error : label expected";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isInterface("IBar"));
    }

    static void testWeAreOKWithSemicolons() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "boolean state;" << std::endl << "};";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(i_bar.super(), const gum::NotFound&);
    }

    static void testSimpleInterfaceError6() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "boolean state;" << std::endl
            << "boolean state;" << std::endl
            << "};";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|3 col 1| Error : Element state already exists";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(i_bar.super(), const gum::NotFound&);
    }

    static void testInterfaceWithReference() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl << "boolean state;" << std::endl << "}";
      input << "interface IBar { " << std::endl
            << "IFoo myFoo;" << std::endl
            << "boolean state;" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isInterface("IFoo"));
      const auto& i_foo = prm.getInterface("IFoo");
      CHECK((i_foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_foo.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(i_foo.super(), const gum::NotFound&);
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK_THROWS_AS(i_bar.super(), const gum::NotFound&);
    }

    static void testSuperInterface() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl << "boolean state;" << std::endl << "}";
      input << "interface IBar extends IFoo { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isInterface("IFoo"));
      const auto& i_foo = prm.getInterface("IFoo");
      CHECK((i_foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_foo.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(i_foo.super(), const gum::NotFound&);
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK((i_bar.super()) == (i_foo));
    }

    static void testSuperInterfaceError1() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extends IFooBar { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|4 col 24| Error : Interface IFooBar not found";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isInterface("IFoo"));
      CHECK(!prm.isInterface("IBar"));
    }

    static void testSuperInterfaceError2() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extends t_state { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|5 col 24| Error : Interface t_state not found";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isInterface("IFoo"));
      CHECK(!prm.isInterface("IBar"));
    }

    static void testSuperInterfaceError3() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extend IFoo { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|4 col 16| Error : \"{\" expected";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(0)));
    }

    static void testSuperInterfaceWithAttributeOverload() {
      // Arrange
      std::stringstream input;
      input << "type t_state extends boolean (" << std::endl;
      input << "OK:true, NOK:false);" << std::endl;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extends IFoo { " << std::endl
            << "t_state state;" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isInterface("IFoo"));
      const auto& i_foo = prm.getInterface("IFoo");
      CHECK((i_foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_foo.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(i_foo.super(), const gum::NotFound&);
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK((i_bar.super()) == (i_foo));
    }

    static void testSuperInterfaceWithReferenceOverload() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extends IFoo { " << std::endl << "}";
      input << "interface IPlop { " << std::endl << "IFoo myFoo;" << std::endl << "}";
      input << "interface IPloc extends IPlop { " << std::endl
            << "IBar myFoo; " << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(4)));
      CHECK(prm.isInterface("IPlop"));
      const auto& i_plop = prm.getInterface("IPlop");
      CHECK((i_plop.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((i_plop.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK_THROWS_AS(i_plop.super(), const gum::NotFound&);
      CHECK(prm.isInterface("IPloc"));
      const auto& i_ploc = prm.getInterface("IPloc");
      CHECK((i_ploc.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((i_ploc.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_ploc.super()) == (i_plop));
    }

    static void testOrderDoesNotMatter1() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "t_state state;" << std::endl
            << "}" << std::endl
            << "type t_state labels(OK, NOK);";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isType("t_state"));
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(i_bar.super(), const gum::NotFound&);
    }

    static void testOrderDoesNotMatter2() {
      // Arrange
      std::stringstream input;
      input << "interface IBar extends IFoo { " << std::endl
            << "t_state state;" << std::endl
            << "}";
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "type t_state extends boolean (" << std::endl << "OK:true, NOK:false);" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isInterface("IFoo"));
      const auto& i_foo = prm.getInterface("IFoo");
      CHECK((i_foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_foo.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(i_foo.super(), const gum::NotFound&);
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK((i_bar.super()) == (i_foo));
    }

    static void testOrderDoesNotMatter3() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "IFoo foo;" << std::endl
            << "boolean state;" << std::endl
            << "}";
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isInterface("IFoo"));
      const auto& i_foo = prm.getInterface("IFoo");
      CHECK((i_foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_foo.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(i_foo.super(), const gum::NotFound&);
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK_THROWS_AS(i_bar.super(), const gum::NotFound&);
    }

    static void testOrderDoesNotMatter4() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "IFoo foo;" << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IFoo { " << std::endl
            << "IBar bar; " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isInterface("IFoo"));
      const auto& i_foo = prm.getInterface("IFoo");
      CHECK((i_foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_foo.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK_THROWS_AS(i_foo.super(), const gum::NotFound&);
      CHECK(prm.isInterface("IBar"));
      const auto& i_bar = prm.getInterface("IBar");
      CHECK((i_bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((i_bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK_THROWS_AS(i_bar.super(), const gum::NotFound&);
    }

    static void testInterfaceWithReferenceError() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl
            << "IFoo foo;" << std::endl
            << "boolean state;" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 1| Error : Interface IFoo cannot reference "
             "itself";
      CHECK((line) == (msg.str()));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(1)));
    }

    static void testInterfaceWithOverloading() {
      // Arrange
      std::stringstream input;
      input << "type state extends boolean (OK: true, NOK: false);";
      input << std::endl;
      input << "interface Plop { "
               "boolean state;"
               "}";
      input << std::endl;
      input << "interface SubPlop extends Plop { "
               "state state;"
               "}";
      input << std::endl;
      input << "interface Foo { "
               "Plop plop;"
               "}";
      input << std::endl;
      input << "interface Bar extends Foo { "
               "SubPlop plop;"
               "}";
      input << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(4)));
      CHECK(prm.isInterface("Plop"));
      const auto& plop = prm.getInterface("Plop");
      CHECK((plop.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(plop.exists("state"));
      CHECK(plop.exists("(boolean)state"));

      CHECK(prm.isInterface("SubPlop"));
      const auto& sub_plop = prm.getInterface("SubPlop");
      CHECK((sub_plop.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(sub_plop.exists("state"));
      CHECK(sub_plop.exists("(boolean)state"));
      CHECK(sub_plop.exists("(state)state"));

      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("plop"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(foo.get("plop")));
      const auto& ref_plop
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(foo.get("plop"));
      CHECK((&(ref_plop.slotType())) == (&plop));

      CHECK(prm.isInterface("Bar"));
      const auto& bar = prm.getInterface("Bar");
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("plop"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("plop")));
      const auto& ref_subplop
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("plop"));
      CHECK((&(ref_subplop.slotType())) == (&sub_plop));
    }

    static void testInterfaceWithOverloadingError1() {
      // Arrange
      std::stringstream input;
      input << "type state extends boolean (OK: true, NOK: false);";
      input << std::endl;
      input << "interface Plop { "
               "boolean state;"
               "}";
      input << std::endl;
      input << "interface SubPlop { "
               "state state;"
               "}";
      input << std::endl;
      input << "interface Foo { "
               "Plop plop;"
               "}";
      input << std::endl;
      input << "interface Bar extends Foo { "
               "SubPlop plop;"
               "}";
      input << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|5 col 29| Error : Illegal overload of element plop";
      CHECK((line) == (msg.str()));
    }

    static void testInterfaceWithOverloadingError2() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);";
      input << std::endl;
      input << "interface Plop { "
               "boolean state;"
               "}";
      input << std::endl;
      input << "interface SubPlop extends Plop { "
               "state state;"
               "}";
      input << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|3 col 34| Error : Illegal overload of element state";
      CHECK((line) == (msg.str()));
    }
  };

  GUM_TEST_ACTIF(EmptyInterface)
  GUM_TEST_ACTIF(EmptyInterfaceError1)
  GUM_TEST_ACTIF(EmptyInterfaceError2)
  GUM_TEST_ACTIF(EmptyInterfaceError3)
  GUM_TEST_ACTIF(SimpleInterface)
  GUM_TEST_ACTIF(SimpleInterfaceError1)
  GUM_TEST_ACTIF(SimpleInterfaceError2)
  GUM_TEST_ACTIF(SimpleInterfaceError3)
  GUM_TEST_ACTIF(SimpleInterfaceError4)
  GUM_TEST_ACTIF(SimpleInterfaceError5)
  GUM_TEST_ACTIF(WeAreOKWithSemicolons)
  GUM_TEST_ACTIF(SimpleInterfaceError6)
  GUM_TEST_ACTIF(InterfaceWithReference)
  GUM_TEST_ACTIF(SuperInterface)
  GUM_TEST_ACTIF(SuperInterfaceError1)
  GUM_TEST_ACTIF(SuperInterfaceError2)
  GUM_TEST_ACTIF(SuperInterfaceError3)
  GUM_TEST_ACTIF(SuperInterfaceWithAttributeOverload)
  GUM_TEST_ACTIF(SuperInterfaceWithReferenceOverload)
  GUM_TEST_ACTIF(OrderDoesNotMatter1)
  GUM_TEST_ACTIF(OrderDoesNotMatter2)
  GUM_TEST_ACTIF(OrderDoesNotMatter3)
  GUM_TEST_ACTIF(OrderDoesNotMatter4)
  GUM_TEST_ACTIF(InterfaceWithReferenceError)
  GUM_TEST_ACTIF(InterfaceWithOverloading)
  GUM_TEST_ACTIF(InterfaceWithOverloadingError1)
  GUM_TEST_ACTIF(InterfaceWithOverloadingError2)

}   // namespace gum_tests
