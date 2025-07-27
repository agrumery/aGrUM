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


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/O3prmReader.h>
#include <agrum/PRM/PRM.h>

#include <agrum/base/core/math/math_utils.h>

namespace gum_tests {

  class [[maybe_unused]] O3InterfaceTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    GUM_ACTIVE_TEST(EmptyInterface) {
      // Arrange
      std::stringstream input;
      input << "interface IBar { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)1)
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)0)
      TS_ASSERT_EQUALS(i_bar.referenceSlots().size(), (gum::Size)0)
      TS_ASSERT_THROWS(i_bar.super(), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(EmptyInterfaceError1) {
      // Arrange
      std::stringstream input;
      input << "interfaces IBar { }";
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
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)0)
      TS_ASSERT(!prm.isInterface("IBar"))
    }

    GUM_ACTIVE_TEST(EmptyInterfaceError2) {
      // Arrange
      std::stringstream input;
      input << "interface { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 11| Error : label expected";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)0)
      TS_ASSERT(!prm.isInterface("IBar"))
    }

    GUM_ACTIVE_TEST(EmptyInterfaceError3) {
      // Arrange
      std::stringstream input;
      input << "interface ++++/ze { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 11| Error : label expected";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)0)
      TS_ASSERT(!prm.isInterface("IBar"))
    }

    GUM_ACTIVE_TEST(SimpleInterface) {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "boolean state;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)1)
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_bar.referenceSlots().size(), (gum::Size)0)
      TS_ASSERT_THROWS(i_bar.super(), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(SimpleInterfaceError1) {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "t_state state;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 1| Error : Unknown type t_state";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)1)
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)0)
      TS_ASSERT_EQUALS(i_bar.referenceSlots().size(), (gum::Size)0)
      TS_ASSERT_THROWS(i_bar.super(), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(SimpleInterfaceError2) {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "=%+ state;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 1| Error : \"}\" expected";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)0)
      TS_ASSERT(!prm.isInterface("IBar"))
    }

    GUM_ACTIVE_TEST(SimpleInterfaceError3) {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "state;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 6| Error : label expected";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)0)
      TS_ASSERT(!prm.isInterface("IBar"))
    }

    GUM_ACTIVE_TEST(SimpleInterfaceError4) {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "boolean state" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|3 col 1| Error : semicolon expected";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)0)
      TS_ASSERT(!prm.isInterface("IBar"))
    }

    GUM_ACTIVE_TEST(SimpleInterfaceError5) {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "boolean +/+/;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 9| Error : label expected";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)0)
      TS_ASSERT(!prm.isInterface("IBar"))
    }

    GUM_ACTIVE_TEST(WeAreOKWithSemicolons) {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "boolean state;" << std::endl << "};";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)1)
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_bar.referenceSlots().size(), (gum::Size)0)
      TS_ASSERT_THROWS(i_bar.super(), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(SimpleInterfaceError6) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|3 col 1| Error : Element state already exists";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)1)
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_bar.referenceSlots().size(), (gum::Size)0)
      TS_ASSERT_THROWS(i_bar.super(), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(InterfaceWithReference) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)2)
      TS_ASSERT(prm.isInterface("IFoo"))
      const auto& i_foo = prm.getInterface("IFoo");
      TS_ASSERT_EQUALS(i_foo.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_foo.referenceSlots().size(), (gum::Size)0)
      TS_ASSERT_THROWS(i_foo.super(), const gum::NotFound&)
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_bar.referenceSlots().size(), (gum::Size)1)
      TS_ASSERT_THROWS(i_bar.super(), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(SuperInterface) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)2)
      TS_ASSERT(prm.isInterface("IFoo"))
      const auto& i_foo = prm.getInterface("IFoo");
      TS_ASSERT_EQUALS(i_foo.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_foo.referenceSlots().size(), (gum::Size)0)
      TS_ASSERT_THROWS(i_foo.super(), const gum::NotFound&)
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)2)
      TS_ASSERT_EQUALS(i_bar.super(), i_foo)
    }

    GUM_ACTIVE_TEST(SuperInterfaceError1) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|4 col 24| Error : Interface IFooBar not found";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)0)
      TS_ASSERT(!prm.isInterface("IFoo"))
      TS_ASSERT(!prm.isInterface("IBar"))
    }

    GUM_ACTIVE_TEST(SuperInterfaceError2) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|5 col 24| Error : Interface t_state not found";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)0)
      TS_ASSERT(!prm.isInterface("IFoo"))
      TS_ASSERT(!prm.isInterface("IBar"))
    }

    GUM_ACTIVE_TEST(SuperInterfaceError3) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|4 col 16| Error : \"{\" expected";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(SuperInterfaceWithAttributeOverload) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)2)
      TS_ASSERT(prm.isInterface("IFoo"))
      const auto& i_foo = prm.getInterface("IFoo");
      TS_ASSERT_EQUALS(i_foo.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_foo.referenceSlots().size(), (gum::Size)0)
      TS_ASSERT_THROWS(i_foo.super(), const gum::NotFound&)
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)2)
      TS_ASSERT_EQUALS(i_bar.super(), i_foo)
    }

    GUM_ACTIVE_TEST(SuperInterfaceWithReferenceOverload) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)4)
      TS_ASSERT(prm.isInterface("IPlop"))
      const auto& i_plop = prm.getInterface("IPlop");
      TS_ASSERT_EQUALS(i_plop.attributes().size(), (gum::Size)0)
      TS_ASSERT_EQUALS(i_plop.referenceSlots().size(), (gum::Size)1)
      TS_ASSERT_THROWS(i_plop.super(), const gum::NotFound&)
      TS_ASSERT(prm.isInterface("IPloc"))
      const auto& i_ploc = prm.getInterface("IPloc");
      TS_ASSERT_EQUALS(i_ploc.attributes().size(), (gum::Size)0)
      TS_ASSERT_EQUALS(i_ploc.referenceSlots().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_ploc.super(), i_plop)
    }

    GUM_ACTIVE_TEST(OrderDoesNotMatter1) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)1)
      TS_ASSERT(prm.isType("t_state"))
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_bar.referenceSlots().size(), (gum::Size)0)
      TS_ASSERT_THROWS(i_bar.super(), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(OrderDoesNotMatter2) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)2)
      TS_ASSERT(prm.isInterface("IFoo"))
      const auto& i_foo = prm.getInterface("IFoo");
      TS_ASSERT_EQUALS(i_foo.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_foo.referenceSlots().size(), (gum::Size)0)
      TS_ASSERT_THROWS(i_foo.super(), const gum::NotFound&)
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)2)
      TS_ASSERT_EQUALS(i_bar.super(), i_foo)
    }

    GUM_ACTIVE_TEST(OrderDoesNotMatter3) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)2)
      TS_ASSERT(prm.isInterface("IFoo"))
      const auto& i_foo = prm.getInterface("IFoo");
      TS_ASSERT_EQUALS(i_foo.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_foo.referenceSlots().size(), (gum::Size)0)
      TS_ASSERT_THROWS(i_foo.super(), const gum::NotFound&)
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)1)
      TS_ASSERT_THROWS(i_bar.super(), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(OrderDoesNotMatter4) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)2)
      TS_ASSERT(prm.isInterface("IFoo"))
      const auto& i_foo = prm.getInterface("IFoo");
      TS_ASSERT_EQUALS(i_foo.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_foo.referenceSlots().size(), (gum::Size)1)
      TS_ASSERT_THROWS(i_foo.super(), const gum::NotFound&)
      TS_ASSERT(prm.isInterface("IBar"))
      const auto& i_bar = prm.getInterface("IBar");
      TS_ASSERT_EQUALS(i_bar.attributes().size(), (gum::Size)1)
      TS_ASSERT_EQUALS(i_bar.referenceSlots().size(), (gum::Size)1)
      TS_ASSERT_THROWS(i_bar.super(), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(InterfaceWithReferenceError) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 1| Error : Interface IFoo cannot reference "
             "itself";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)1)
    }

    GUM_ACTIVE_TEST(InterfaceWithOverloading) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)4)
      TS_ASSERT(prm.isInterface("Plop"))
      const auto& plop = prm.getInterface("Plop");
      TS_ASSERT_EQUALS(plop.attributes().size(), (gum::Size)1)
      TS_ASSERT(plop.exists("state"))
      TS_ASSERT(plop.exists("(boolean)state"))

      TS_ASSERT(prm.isInterface("SubPlop"))
      const auto& sub_plop = prm.getInterface("SubPlop");
      TS_ASSERT_EQUALS(sub_plop.attributes().size(), (gum::Size)2)
      TS_ASSERT(sub_plop.exists("state"))
      TS_ASSERT(sub_plop.exists("(boolean)state"))
      TS_ASSERT(sub_plop.exists("(state)state"))

      TS_ASSERT(prm.isInterface("Foo"))
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.referenceSlots().size(), (gum::Size)1)
      TS_ASSERT(foo.exists("plop"))
      TS_ASSERT(gum::prm::PRMClassElement< double >::isReferenceSlot(foo.get("plop")))
      const auto& ref_plop
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(foo.get("plop"));
      TS_ASSERT_EQUALS(&(ref_plop.slotType()), &plop)

      TS_ASSERT(prm.isInterface("Bar"))
      const auto& bar = prm.getInterface("Bar");
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1)
      TS_ASSERT(bar.exists("plop"))
      TS_ASSERT(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("plop")))
      const auto& ref_subplop
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("plop"));
      TS_ASSERT_EQUALS(&(ref_subplop.slotType()), &sub_plop)
    }

    GUM_ACTIVE_TEST(InterfaceWithOverloadingError1) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|5 col 29| Error : Illegal overload of element plop";
      TS_ASSERT_EQUALS(line, msg.str())
    }

    GUM_ACTIVE_TEST(InterfaceWithOverloadingError2) {
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
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output))
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|3 col 34| Error : Illegal overload of element state";
      TS_ASSERT_EQUALS(line, msg.str())
    }
  };

}   // namespace gum_tests
