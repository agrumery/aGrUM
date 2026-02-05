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

#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/O3prmReader.h>
#include <agrum/PRM/PRM.h>

#include <agrum/base/core/math/math_utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  O3Class
#define GUM_CURRENT_MODULE PRM

namespace gum_tests {

  struct O3ClassTestSuite {
    public:
    static void testEmptyClass() {   // Arrange
      std::stringstream input;
      input << "class Bar { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }   // namespace gum_tests

    static void testEmptyClassError1() {
      // Arrange
      std::stringstream input;
      input << "clas Bar { }";
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
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isClass("Bar"));
    }

    static void testEmptyClassError2() {
      // Arrange
      std::stringstream input;
      input << "class { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 7| Error : label expected";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isClass("Bar"));
    }

    static void testEmptyClassError3() {
      // Arrange
      std::stringstream input;
      input << "class ++++/ze { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 7| Error : label expected";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(0)));
      CHECK(!prm.isClass("Bar"));
    }

    static void testSimpleClass1() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl << "boolean state {[0.2, 0.8]};" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      const auto& cpf = state.cpf();
      auto        i   = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testSimpleClass2() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl << "t_state state {[0.2, 0.8]};" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      const auto& cpf = state.cpf();
      auto        i   = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testSimpleClass3() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,  // false" << std::endl
            << "   0.8, 0.6]}; // true" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.4).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.6).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testSimpleClassError1() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,  // false" << std::endl
            << "    0.2, 0.6]}; // true" << std::endl
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
      msg << "|4 col 9| Error : PRMAttribute Bar.isWorking CPT does not sum to "
             "1, "
             "found 0.4";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testSimpleClassError2() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson stat " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,  // false" << std::endl
            << "    0.2, 0.6]}; // true" << std::endl
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
      msg << "|4 col 29| Error : Parent stat not found";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testSimpleClassError3() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,   // false" << std::endl
            << "    0.2, 0.4,   // true" << std::endl
            << "    0.6, 0.2]}; // oops" << std::endl
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
      msg << "|4 col 9| Error : Illegal CPT size, expected 4 found 6 for "
             "attribute Bar.isWorking";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testSimpleRules1() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: 0.1, 0.9;" << std::endl
            << "NOK: 0.8, 0.2;" << std::endl
            << "};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(0.1).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.9).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testSimpleRules2() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: 0.1, 0.9;" << std::endl
            << "*: 0.8, 0.2;" << std::endl
            << "};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testSimpleRules3() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: \"10/100\", \"90/100\";" << std::endl
            << "NOK: \"80/100\", \"20/100\";" << std::endl
            << "};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(0.1).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.9).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testSimpleRules4() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: '10/100', '90/100';" << std::endl
            << "NOK: '80/100', '20/100';" << std::endl
            << "};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(0.1).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.9).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testSimpleRulesError1() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: '10/100', '110/100';" << std::endl
            << "NOK: '80/100', '20/100';" << std::endl
            << "};" << std::endl
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
      msg << "|5 col 15| Error : Illegal CPT value \"110/100\" in attribute "
             "Bar.isWorking, formula resolve to 1.1";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testSimpleRulesError2() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK, FOO: '10/100', '110/100';" << std::endl
            << "NOK, BAR: '80/100', '20/100';" << std::endl
            << "};" << std::endl
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
      msg << "|5 col 1| Error : Expected 1 value(s), found 2";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testSimpleRulesError3() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: '10/100', '90/100';" << std::endl
            << "NOK: 'FOO', '20/100';" << std::endl
            << "};" << std::endl
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
      msg << "|6 col 6| Error : Illegal CPT value \"FOO\" in attribute "
             "Bar.isWorking, could not resolve the following formula: \"FOO\"";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testSimpleRulesError4() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "BAR: 0.1, 0.9;" << std::endl
            << "FOO: 0.8, 0.2;" << std::endl
            << "};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::string msg = "|5 col 1| Error : Label BAR is not part of state domain";
      CHECK((line) == (msg));
      std::getline(output, line);
      msg = "|6 col 1| Error : Label FOO is not part of state domain";
      CHECK((line) == (msg));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testSimpleParameter1() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "param real lambda default 0.003;" << std::endl
            << "param int t default 8760;" << std::endl
            << "boolean state { " << std::endl
            << "[ 'exp(-lambda*t)', '1-exp(-lambda*t)' ]" << std::endl
            << "};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      const auto& cpf = state.cpf();
      CHECK(cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(2)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(std::exp(-0.003 * 8760)).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(1 - std::exp(-0.003 * 8760)).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testSimpleParameter2() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "param real lambda default 0.003;" << std::endl
            << "param int t default 8760;" << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: 'exp(-lambda*t)', '1-exp(-lambda*t)';" << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(std::exp(-0.003 * 8760)).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(1 - std::exp(-0.003 * 8760)).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(1.0).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.0).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testClassWithReference1() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl << "Foo myFoo;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoo"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoo")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoo"));
      CHECK((&(myFoo.slotType())) == (&(foo)));
      CHECK(!myFoo.isArray());
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testClassWithReference2() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl << "Foo[] myFoo;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoo"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoo")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoo"));
      CHECK((&(myFoo.slotType())) == (&(foo)));
      CHECK(myFoo.isArray());
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testClassWithReference3() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "param real lambda default 0.003;" << std::endl
            << "param int t default 8760;" << std::endl
            << "Foo myFoo;" << std::endl
            << "boolean isWorking dependson myFoo.state {" << std::endl
            << "OK: 'exp(-lambda*t)', '1-exp(-lambda*t)';" << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoo"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoo")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoo"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(!myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(!cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK(bar.exists("myFoo.state"));
      const auto& chain = bar.get("myFoo.state");
      CHECK(cpf.variablesSequence().exists(&(chain.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(std::exp(-0.003 * 8760)).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(1 - std::exp(-0.003 * 8760)).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(1.0).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.0).epsilon(1e-6));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testClassWithReference4() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "class Foo { " << std::endl
            << "param int t default 8760;" << std::endl
            << "state state {[ 0.2, 0.8]};" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "param real lambda default 0.003;" << std::endl
            << "Foo myFoo;" << std::endl
            << "boolean isWorking dependson myFoo.state {" << std::endl
            << "OK: 'exp(-lambda*myFoo.t)', '1-exp(-lambda*myFoo.t)';" << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(2)));
      CHECK(prm.isClass("Foo"));
      const auto& foo = prm.getClass("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoo"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoo")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoo"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(!myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(!cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK(bar.exists("myFoo.state"));
      const auto& chain = bar.get("myFoo.state");
      CHECK(cpf.variablesSequence().exists(&(chain.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(std::exp(-0.003 * 8760)).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(1 - std::exp(-0.003 * 8760)).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(1.0).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.0).epsilon(1e-6));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testClassWithReferenceError1() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "param real lambda default 0.003;" << std::endl
            << "param int t default 8760;" << std::endl
            << "Foo myFoo;" << std::endl
            << "boolean isWorking dependson myfoo.state {" << std::endl
            << "OK: 'exp(-lambda*t)', '1-exp(-lambda*t)';" << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
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
      msg << "|9 col 29| Error : Link myfoo in chain myfoo.state "
             "not found";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testClassWithReferenceError2() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "param real lambda default 0.003;" << std::endl
            << "param int t default 8760;" << std::endl
            << "Foo myFoo;" << std::endl
            << "boolean isWorking dependson myFoo.stat {" << std::endl
            << "OK: 'exp(-lambda*t)', '1-exp(-lambda*t)';" << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
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
      msg << "|9 col 29| Error : Link stat in chain myFoo.stat not "
             "found";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testClassWithReferenceError3() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "param real lambda default 0.003;" << std::endl
            << "param int t default 8760;" << std::endl
            << "boolean isWorking dependson myFoo.state {" << std::endl
            << "OK: 'exp(-lambda*t)', '1-exp(-lambda*t)';" << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
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
      msg << "|8 col 29| Error : Link myFoo in chain myFoo.state "
             "not found";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testClassWithReferenceError4() {
      // Arrange
      std::stringstream input;
      input << "class Bar extends Foo { " << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};" << std::endl
            << "state state {[0.5, 0.5]};" << std::endl
            << "}" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "Bar myBar;" << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}" << std::endl;
      input << "type state extends boolean (OK: true, NOK:false);" << std::endl;
      input << "interface iFoo { " << std::endl
            << "Bar myBar;" << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|6 col 1| Error : Class Foo cannot reference "
             "subclass Bar";
      CHECK((line) == (msg.str()));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(2)));
    }

    static void testClassWithReferenceError6() {
      // Arrange
      std::stringstream input;
      input << "class Bar extends Foo { " << std::endl
            << "Bar myBar;" << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};" << std::endl
            << "state state {[0.5, 0.5]};" << std::endl
            << "}" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "Bar myBar;" << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}" << std::endl;
      input << "type state extends boolean (OK: true, NOK:false);" << std::endl;
      input << "interface iFoo { " << std::endl
            << "Bar myBar;" << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|7 col 1| Error : Class Foo cannot reference "
             "subclass Bar";
      CHECK((line) == (msg.str()));
    }

    static void testOrAggregate() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::OR));
    }

    static void testOrAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = or([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::OR));
    }

    static void testAndAggregate() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = and(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::AND));
    }

    static void testAndAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = and([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::AND));
    }

    static void testForAllAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = forall(myFoos.state, OK);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::FORALL));
    }

    static void testForAllAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = forall([myFoos.state, myBoos.state], OK);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::FORALL));
    }

    static void testExistsAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = exists(myFoos.state, OK);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::EXISTS));
    }

    static void testExistsAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = exists([myFoos.state, myBoos.state], OK);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::EXISTS));
    }

    static void testMinAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = min(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::MIN));
    }

    static void testMinAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = min([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::MIN));
    }

    static void testMaxAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = max(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::MAX));
    }

    static void testMaxAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = max([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::MAX));
    }

    static void testMedianAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = median(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::MEDIAN));
    }

    static void testMedianAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = median([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::MEDIAN));
    }

    static void testAmplitudeAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = amplitude(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::AMPLITUDE));
    }

    static void testAmplitudeAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = amplitude([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::AMPLITUDE));
    }

    static void testCountAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = count(myFoos.state, 5);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::COUNT));
    }

    static void testCountAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = count([myFoos.state, myBoos.state], 5);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::COUNT));
    }

    static void testSumAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = sum(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::SUM));
    }

    static void testSumAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = sum([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      const auto& state = foo.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.aggregates().size()) == (static_cast< gum::Size >(1)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("myFoos"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("myFoos"));
      CHECK((&(myFoo.slotType())) == (&foo));
      CHECK(myFoo.isArray());
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg = static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      CHECK((agg.agg_type()) == (gum::prm::PRMAggregate< double >::AggregateType::SUM));
    }

    static void testOrAggregateWithErrors1() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or(myBoos.state);" << std::endl
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
      msg << "|6 col 24| Error : Link myBoos in chain myBoos.state "
             "not found";
      CHECK((line) == (msg.str()));
    }

    static void testOrAggregateWithErrors2() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or(myFoos.st);" << std::endl
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
      msg << "|6 col 24| Error : Link st in chain myFoos.st not "
             "found";
      CHECK((line) == (msg.str()));
    }

    static void testOrAggregateWithErrors3() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or();" << std::endl
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
      msg << "|6 col 24| Error : invalid declaration";
      CHECK((line) == (msg.str()));
    }

    static void testOrAggregateWithErrors4() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Boo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Boo[] myBoos;" << std::endl
            << "boolean isWorking = or([myFoos.state, myBoos.state]);" << std::endl
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
      msg << "|11 col 39| Error : Expected type boolean for parent "
             "myBoos.state, found state";
      CHECK((line) == (msg.str()));
    }

    static void testCountAggregateWithErrors5() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = count(myFoos.state, foobar);" << std::endl
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
      msg << "|7 col 39| Error : Parameter foobar in aggregate "
             "isWorking does not match any expected values";
      CHECK((line) == (msg.str()));
    }

    static void testInheritance1() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "class Bar implements Foo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK(("") == (output.str()));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK(prm.isClass("Bar"));
      CHECK((foo.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      const auto& bar = prm.getClass("Bar");
      CHECK(foo.isSuperTypeOf(bar));
      CHECK(bar.implements().contains(const_cast< gum::prm::PRMInterface< double >* >(&foo)));
      CHECK((bar.parameters().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.slotChains().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testInheritance2() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl << "boolean state;" << std::endl << "}" << std::endl;
      input << "class Bar implements Foo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK(("") == (output.str()));
      CHECK(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      CHECK(prm.isClass("Bar"));
      CHECK((foo.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      const auto& bar = prm.getClass("Bar");
      CHECK(foo.isSuperTypeOf(bar));
      CHECK(bar.implements().contains(const_cast< gum::prm::PRMInterface< double >* >(&foo)));
      CHECK((bar.parameters().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.slotChains().size()) == (static_cast< gum::Size >(0)));
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(1)));
    }

    static void testInheritance3() {
      // Arrange
      std::stringstream input;
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      input << "type state extends boolean (OK: true, NOK:false);" << std::endl;
      input << "class Bar extends Foo { " << std::endl
            << "state state {[0.5, 0.5]};" << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK(("") == (output.str()));
      CHECK(prm.isInterface("iFoo"));
      const auto& ifoo = prm.getInterface("iFoo");
      CHECK((ifoo.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK((ifoo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Foo"));
      const auto& foo = prm.getClass("Foo");
      CHECK(ifoo.isSuperTypeOf(foo));
      CHECK(foo.implements().contains(const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
      const auto& bar = prm.getClass("Bar");
      CHECK(ifoo.isSuperTypeOf(bar));
      CHECK(foo.isSuperTypeOf(bar));
      CHECK((&(bar.super())) == (&(foo)));
      CHECK(bar.implements().contains(const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
    }

    static void testInheritanceError1() {
      // Arrange
      std::stringstream input;
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Foo implements Bar { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
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
      msg << "|4 col 22| Error : Interface Bar not found";
      CHECK((line) == (msg.str()));
    }

    static void testInheritanceError2() {
      // Arrange
      std::stringstream input;
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "boolean stat {[0.2, 0.8]};" << std::endl
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
      msg << "|4 col 7| Error : Class Foo does not implement all of "
             "interface iFoo attributes";
      CHECK((line) == (msg.str()));
    }

    static void testInheritanceError3() {
      // Arrange
      std::stringstream input;
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "state state {[0.2, 0.8]};" << std::endl
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
      msg << "|6 col 7| Error : Class Foo attribute state does not "
             "respect interface iFoo";
      CHECK((line) == (msg.str()));
    }

    static void testInheritanceError4() {
      // Arrange
      std::stringstream input;
      input << "class Foo { " << std::endl
            << "boolean state {[0.3, 0.7]};" << std::endl
            << "}" << std::endl;
      input << "class Bar extends oo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
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
      msg << "|4 col 19| Error : Unknown class oo";
      CHECK((line) == (msg.str()));
    }

    static void testInheritanceError5() {
      // Arrange
      std::stringstream input;
      input << "class Foo { " << std::endl
            << "boolean state {[0.3, 0.7]};" << std::endl
            << "}" << std::endl;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "class Bar extends Foo { " << std::endl
            << "state state {[0.2, 0.8]};" << std::endl
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
      msg << "|6 col 7| Error : Illegal overload of element state from "
             "class Foo";
      CHECK((line) == (msg.str()));
    }

    static void testOrderDoesNotMatter1() {
      // Arrange
      std::stringstream input;
      input << "class Bar extends Foo { " << std::endl
            << "state state {[0.5, 0.5]};" << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};" << std::endl
            << "}" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}" << std::endl;
      input << "type state extends boolean (OK: true, NOK:false);" << std::endl;
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK(("") == (output.str()));
      CHECK(prm.isInterface("iFoo"));
      const auto& ifoo = prm.getInterface("iFoo");
      CHECK((ifoo.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK((ifoo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Foo"));
      const auto& foo = prm.getClass("Foo");
      CHECK(ifoo.isSuperTypeOf(foo));
      CHECK(foo.implements().contains(const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
      const auto& bar = prm.getClass("Bar");
      CHECK(ifoo.isSuperTypeOf(bar));
      CHECK(foo.isSuperTypeOf(bar));
      CHECK((&(bar.super())) == (&(foo)));
      CHECK(bar.implements().contains(const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
    }

    static void testOrderDoesNotMatter2() {
      // Arrange
      std::stringstream input;
      input << "class Bar extends Foo { " << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};" << std::endl
            << "state state {[0.5, 0.5]};" << std::endl
            << "}" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}" << std::endl;
      input << "type state extends boolean (OK: true, NOK:false);" << std::endl;
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK(("") == (output.str()));
      CHECK(prm.isInterface("iFoo"));
      const auto& ifoo = prm.getInterface("iFoo");
      CHECK((ifoo.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK((ifoo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Foo"));
      const auto& foo = prm.getClass("Foo");
      CHECK(ifoo.isSuperTypeOf(foo));
      CHECK(foo.implements().contains(const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
      const auto& bar = prm.getClass("Bar");
      CHECK(ifoo.isSuperTypeOf(bar));
      CHECK(foo.isSuperTypeOf(bar));
      CHECK((&(bar.super())) == (&(foo)));
      CHECK(bar.implements().contains(const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
    }

    static void testPrinters() {
      // Arrange
      auto                    path = GET_RESSOURCES_PATH("o3prm/printers.o3prm");
      gum::prm::PRM< double > prm;
      std::ifstream           input(path);
      std::stringstream       output;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK(("") == (output.str()));
    }

    static void testClassWithOverloading() {
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
      input << "class Foo { "
               "Plop plop;"
               "boolean state {[ 0.2, 0.8]};"
               "}";
      input << std::endl;
      input << "class Bar extends Foo { "
               "SubPlop plop;"
               "state state {[ 0.1, 0.9 ]};"
               "}";
      input << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.interfaces().size()) == (static_cast< gum::Size >(2)));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(2)));
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

      CHECK(prm.isClass("Foo"));
      const auto& foo = prm.getClass("Foo");
      CHECK((foo.attributes().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("state"));
      CHECK(foo.exists("(boolean)state"));
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(foo.get("state")));
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(foo.get("(boolean)state")));
      CHECK((&(foo.get("state"))) == (&(foo.get("(boolean)state"))));
      CHECK((foo.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(foo.exists("plop"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(foo.get("plop")));
      const auto& ref_plop
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(foo.get("plop"));
      CHECK((&(ref_plop.slotType())) == (&plop));

      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("state"));
      CHECK(bar.exists("(boolean)state"));
      CHECK(bar.exists("(state)state"));
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(bar.get("state")));
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(bar.get("(boolean)state")));
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(bar.get("(state)state")));
      CHECK((&(bar.get("state"))) == (&(bar.get("(state)state"))));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      CHECK(bar.exists("plop"));
      CHECK(gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("plop")));
      const auto& ref_subplop
          = static_cast< const gum::prm::PRMReferenceSlot< double >& >(bar.get("plop"));
      CHECK((&(ref_subplop.slotType())) == (&sub_plop));
    }

    static void testClassWithOverloadingError1() {
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
      input << "class Foo { "
               "Plop plop;"
               "boolean state {[ 0.2, 0.8]};"
               "}";
      input << std::endl;
      input << "class Bar extends Foo { "
               "SubPlop plop;"
               "state state {[ 0.1, 0.9 ]};"
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
      msg << "|5 col 33| Error : Illegal overload of element plop from class "
             "Bar";
      CHECK((line) == (msg.str()));
    }

    static void testAggregateWithError() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl << "state state;" << std::endl << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = count([myFoos, myBoos.state], 5);" << std::endl
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
      msg << "|8 col 26| Error : Illegal parent myFoos";
      CHECK((line) == (msg.str()));
    }

    static void testAnonymousType1() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "labels(OK, NOK) state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,  // false" << std::endl
            << "   0.8, 0.6]}; // true" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.4).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.6).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testAnonymousType2() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "labels(OK, NOK) state {[0.2, 0.8]};" << std::endl
            << "labels(NOK, OK) isWorking dependson state " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,  // NOK" << std::endl
            << "   0.8, 0.6]}; // OK" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.4).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.6).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testAnonymousType3() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "int(0, 1) state {[0.2, 0.8]};" << std::endl
            << "int(2, 3) isWorking dependson state " << std::endl
            << "// 0 | 1 " << std::endl
            << "  {[0.2, 0.4,  // 2" << std::endl
            << "   0.8, 0.6]}; // 3" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.4).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.6).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testAnonymousType4() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "real(0.0, 0.5, 1.0) state {[0.2, 0.8]};" << std::endl
            << "real(2.0, 2.5, 3.0) isWorking dependson state " << std::endl
            << "// 0.0-0.5 | 0.5-1.0 " << std::endl
            << "{[ 0.2,      0.4,  // 2.0-2.5" << std::endl
            << "   0.8,      0.6]}; // 2.5-3.0" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == (""));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(1)));
      CHECK(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      CHECK((bar.attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK(bar.exists("state"));
      const auto& state = bar.get("state");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(state));
      CHECK(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      CHECK(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      CHECK(cpf.variablesSequence().exists(&(state.type().variable())));
      CHECK((cpf.domainSize()) == (static_cast< gum::Size >(4)));
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      CHECK((cpf[i]) == doctest::Approx(0.2).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.8).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.4).epsilon(1e-6));
      i.inc();
      CHECK((cpf[i]) == doctest::Approx(0.6).epsilon(1e-6));
      CHECK((bar.referenceSlots().size()) == (static_cast< gum::Size >(0)));
      CHECK_THROWS_AS(bar.super(), const gum::NotFound&);
    }

    static void testAnonymousTypeError1() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "label(OK, NOK) state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,  // false" << std::endl
            << "   0.8, 0.6]}; // true" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == ("|2 col 6| Error : invalid declaration\n"));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(0)));
    }

    static void testAnonymousTypeError2() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "labels(&é, NOK) state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,  // false" << std::endl
            << "   0.8, 0.6]}; // true" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == ("|2 col 8| Error : invalid declaration\n"));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(0)));
    }

    static void testAnonymousTypeError3() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "in(0, 1) state {[0.2, 0.8]};" << std::endl
            << "int(2, 3) isWorking dependson state " << std::endl
            << "// 0 | 1 " << std::endl
            << "  {[0.2, 0.4,  // 2" << std::endl
            << "   0.8, 0.6]}; // 3" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == ("|2 col 3| Error : invalid declaration\n"));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(0)));
    }

    static void testAnonymousTypeError4() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "int(azeaz, 1) state {[0.2, 0.8]};" << std::endl
            << "int(2, 3) isWorking dependson state " << std::endl
            << "// 0 | 1 " << std::endl
            << "  {[0.2, 0.4,  // 2" << std::endl
            << "   0.8, 0.6]}; // 3" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == ("|2 col 5| Error : integer expected\n"));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(0)));
    }

    static void testAnonymousTypeError5() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "rel(0.0, 0.5, 1.0) state {[0.2, 0.8]};" << std::endl
            << "real(2.0, 2.5, 3.0) isWorking dependson state " << std::endl
            << "// 0.0-0.5 | 0.5-1.0 " << std::endl
            << "{[ 0.2,      0.4,  // 2.0-2.5" << std::endl
            << "   0.8,      0.6]}; // 2.5-3.0" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == ("|2 col 4| Error : invalid declaration\n"));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(0)));
    }

    static void testAnonymousTypeError6() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "real(FOO, 0.5, 1.0) state {[0.2, 0.8]};" << std::endl
            << "real(2.0, 2.5, 3.0) isWorking dependson state " << std::endl
            << "// 0.0-0.5 | 0.5-1.0 " << std::endl
            << "{[ 0.2,      0.4,  // 2.0-2.5" << std::endl
            << "   0.8,      0.6]}; // 2.5-3.0" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto                    factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      GUM_CHECK_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      CHECK((output.str()) == ("|2 col 6| Error : invalid declaration\n"));
      CHECK((prm.classes().size()) == (static_cast< gum::Size >(0)));
    }
  };

  GUM_TEST_ACTIF(EmptyClass)
  GUM_TEST_ACTIF(EmptyClassError1)
  GUM_TEST_ACTIF(EmptyClassError2)
  GUM_TEST_ACTIF(EmptyClassError3)
  GUM_TEST_ACTIF(SimpleClass1)
  GUM_TEST_ACTIF(SimpleClass2)
  GUM_TEST_ACTIF(SimpleClass3)
  GUM_TEST_ACTIF(SimpleClassError1)
  GUM_TEST_ACTIF(SimpleClassError2)
  GUM_TEST_ACTIF(SimpleClassError3)
  GUM_TEST_ACTIF(SimpleRules1)
  GUM_TEST_ACTIF(SimpleRules2)
  GUM_TEST_ACTIF(SimpleRules3)
  GUM_TEST_ACTIF(SimpleRules4)
  GUM_TEST_ACTIF(SimpleRulesError1)
  GUM_TEST_ACTIF(SimpleRulesError2)
  GUM_TEST_ACTIF(SimpleRulesError3)
  GUM_TEST_ACTIF(SimpleRulesError4)
  GUM_TEST_ACTIF(SimpleParameter1)
  GUM_TEST_ACTIF(SimpleParameter2)
  GUM_TEST_ACTIF(ClassWithReference1)
  GUM_TEST_ACTIF(ClassWithReference2)
  GUM_TEST_ACTIF(ClassWithReference3)
  GUM_TEST_ACTIF(ClassWithReference4)
  GUM_TEST_ACTIF(ClassWithReferenceError1)
  GUM_TEST_ACTIF(ClassWithReferenceError2)
  GUM_TEST_ACTIF(ClassWithReferenceError3)
  GUM_TEST_ACTIF(ClassWithReferenceError4)
  GUM_TEST_ACTIF(ClassWithReferenceError6)
  GUM_TEST_ACTIF(OrAggregate)
  GUM_TEST_ACTIF(OrAggregateArray)
  GUM_TEST_ACTIF(AndAggregate)
  GUM_TEST_ACTIF(AndAggregateArray)
  GUM_TEST_ACTIF(ForAllAggregate)
  GUM_TEST_ACTIF(ForAllAggregateArray)
  GUM_TEST_ACTIF(ExistsAggregate)
  GUM_TEST_ACTIF(ExistsAggregateArray)
  GUM_TEST_ACTIF(MinAggregate)
  GUM_TEST_ACTIF(MinAggregateArray)
  GUM_TEST_ACTIF(MaxAggregate)
  GUM_TEST_ACTIF(MaxAggregateArray)
  GUM_TEST_ACTIF(MedianAggregate)
  GUM_TEST_ACTIF(MedianAggregateArray)
  GUM_TEST_ACTIF(AmplitudeAggregate)
  GUM_TEST_ACTIF(AmplitudeAggregateArray)
  GUM_TEST_ACTIF(CountAggregate)
  GUM_TEST_ACTIF(CountAggregateArray)
  GUM_TEST_ACTIF(SumAggregate)
  GUM_TEST_ACTIF(SumAggregateArray)
  GUM_TEST_ACTIF(OrAggregateWithErrors1)
  GUM_TEST_ACTIF(OrAggregateWithErrors2)
  GUM_TEST_ACTIF(OrAggregateWithErrors3)
  GUM_TEST_ACTIF(OrAggregateWithErrors4)
  GUM_TEST_ACTIF(CountAggregateWithErrors5)
  GUM_TEST_ACTIF(Inheritance1)
  GUM_TEST_ACTIF(Inheritance2)
  GUM_TEST_ACTIF(Inheritance3)
  GUM_TEST_ACTIF(InheritanceError1)
  GUM_TEST_ACTIF(InheritanceError2)
  GUM_TEST_ACTIF(InheritanceError3)
  GUM_TEST_ACTIF(InheritanceError4)
  GUM_TEST_ACTIF(InheritanceError5)
  GUM_TEST_ACTIF(OrderDoesNotMatter1)
  GUM_TEST_ACTIF(OrderDoesNotMatter2)
  GUM_TEST_ACTIF(Printers)
  GUM_TEST_ACTIF(ClassWithOverloading)
  GUM_TEST_ACTIF(ClassWithOverloadingError1)
  GUM_TEST_ACTIF(AggregateWithError)
  GUM_TEST_ACTIF(AnonymousType1)
  GUM_TEST_ACTIF(AnonymousType2)
  GUM_TEST_ACTIF(AnonymousType3)
  GUM_TEST_ACTIF(AnonymousType4)
  GUM_TEST_ACTIF(AnonymousTypeError1)
  GUM_TEST_ACTIF(AnonymousTypeError2)
  GUM_TEST_ACTIF(AnonymousTypeError3)
  GUM_TEST_ACTIF(AnonymousTypeError4)
  GUM_TEST_ACTIF(AnonymousTypeError5)
  GUM_TEST_ACTIF(AnonymousTypeError6)

}   // namespace gum_tests
