/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/core/math/math_utils.h>
#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/O3prmReader.h>
#include <agrum/tools/variables/discretizedVariable.h>

namespace gum_tests {

  class O3ClassTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testEmptyClass() {
      // Arrange
      std::stringstream input;
      input << "class Bar { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testEmptyClassError1() {
      // Arrange
      std::stringstream input;
      input << "clas Bar { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 1| Error : invalid declaration";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)0);
      TS_ASSERT(!prm.isClass("Bar"));
    }

    void testEmptyClassError2() {
      // Arrange
      std::stringstream input;
      input << "class { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 7| Error : label expected";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)0);
      TS_ASSERT(!prm.isClass("Bar"));
    }

    void testEmptyClassError3() {
      // Arrange
      std::stringstream input;
      input << "class ++++/ze { }";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 7| Error : label expected";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)0);
      TS_ASSERT(!prm.isClass("Bar"));
    }

    void testSimpleClass1() {
      // Arrange
      std::stringstream input;
      input << "class Bar { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      const auto& cpf = state.cpf();
      auto        i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testSimpleClass2() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      const auto& cpf = state.cpf();
      auto        i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testSimpleClass3() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.4, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.6, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testSimpleClassError1() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|4 col 9| Error : PRMAttribute Bar.isWorking CPT does not sum to "
             "1, "
             "found 0.4";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
    }

    void testSimpleClassError2() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|4 col 29| Error : Parent stat not found";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
    }

    void testSimpleClassError3() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|4 col 9| Error : Illegal CPT size, expected 4 found 6 for "
             "attribute Bar.isWorking";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
    }

    void testSimpleRules1() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], 0.1, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.9, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testSimpleRules2() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testSimpleRules3() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], 0.1, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.9, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testSimpleRules4() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], 0.1, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.9, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testSimpleRulesError1() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|5 col 15| Error : Illegal CPT value \"110/100\" in attribute "
             "Bar.isWorking, formula resolve to 1.1";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
    }

    void testSimpleRulesError2() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|5 col 1| Error : Expected 1 value(s), found 2";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
    }

    void testSimpleRulesError3() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|6 col 6| Error : Illegal CPT value \"FOO\" in attribute "
             "Bar.isWorking, could not resolve the following formula: \"FOO\"";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
    }

    void testSimpleRulesError4() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::string msg = "|5 col 1| Error : Label BAR is not part of state domain";
      TS_ASSERT_EQUALS(line, msg);
      std::getline(output, line);
      msg = "|6 col 1| Error : Label FOO is not part of state domain";
      TS_ASSERT_EQUALS(line, msg);
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
    }

    void testSimpleParameter1() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      const auto& cpf = state.cpf();
      TS_ASSERT(cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)2);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], std::exp(-0.003 * 8760), 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 1 - std::exp(-0.003 * 8760), 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testSimpleParameter2() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], std::exp(-0.003 * 8760), 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 1 - std::exp(-0.003 * 8760), 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 1.0, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.0, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testClassWithReference1() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl << "Foo myFoo;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoo"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoo")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoo"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &(foo));
      TS_ASSERT(!myFoo.isArray());
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testClassWithReference2() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl << "Foo[] myFoo;" << std::endl << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoo"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoo")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoo"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &(foo));
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testClassWithReference3() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoo"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoo")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoo"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(!myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(!cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT(bar.exists("myFoo.state"));
      const auto& chain = bar.get("myFoo.state");
      TS_ASSERT(cpf.variablesSequence().exists(&(chain.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], std::exp(-0.003 * 8760), 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 1 - std::exp(-0.003 * 8760), 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 1.0, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.0, 1e-6);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testClassWithReference4() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)2);
      TS_ASSERT(prm.isClass("Foo"));
      const auto& foo = prm.getClass("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoo"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoo")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoo"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(!myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(!cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT(bar.exists("myFoo.state"));
      const auto& chain = bar.get("myFoo.state");
      TS_ASSERT(cpf.variablesSequence().exists(&(chain.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], std::exp(-0.003 * 8760), 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 1 - std::exp(-0.003 * 8760), 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 1.0, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.0, 1e-6);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testClassWithReferenceError1() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|9 col 29| Error : Link myfoo in chain myfoo.state "
             "not found";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
    }

    void testClassWithReferenceError2() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|9 col 29| Error : Link stat in chain myFoo.stat not "
             "found";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
    }

    void testClassWithReferenceError3() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|8 col 29| Error : Link myFoo in chain myFoo.state "
             "not found";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
    }

    void testClassWithReferenceError4() {
      // Arrange
      std::stringstream input;
      input << "class Bar extends Foo { " << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};"
            << std::endl
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|6 col 1| Error : Class Foo cannot reference "
             "subclass Bar";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)2);
    }

    void testClassWithReferenceError6() {
      // Arrange
      std::stringstream input;
      input << "class Bar extends Foo { " << std::endl
            << "Bar myBar;" << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};"
            << std::endl
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|7 col 1| Error : Class Foo cannot reference "
             "subclass Bar";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testOrAggregate() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::OR);
    }

    void testOrAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = or([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::OR);
    }

    void testAndAggregate() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = and(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::AND);
    }

    void testAndAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = and([myFoos.state, myBoos.state]);"
            << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::AND);
    }

    void testForAllAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = forall(myFoos.state, OK);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::FORALL);
    }

    void testForAllAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = forall([myFoos.state, myBoos.state], OK);"
            << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::FORALL);
    }

    void testExistsAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = exists(myFoos.state, OK);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::EXISTS);
    }

    void testExistsAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = exists([myFoos.state, myBoos.state], OK);"
            << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::EXISTS);
    }

    void testMinAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = min(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::MIN);
    }

    void testMinAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = min([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::MIN);
    }

    void testMaxAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = max(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::MAX);
    }

    void testMaxAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = max([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::MAX);
    }

    void testMedianAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = median(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::MEDIAN);
    }

    void testMedianAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = median([myFoos.state, myBoos.state]);"
            << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::MEDIAN);
    }

    void testAmplitudeAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = amplitude(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::AMPLITUDE);
    }

    void testAmplitudeAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = amplitude([myFoos.state, myBoos.state]);"
            << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::AMPLITUDE);
    }

    void testCountAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = count(myFoos.state, 5);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::COUNT);
    }

    void testCountAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = count([myFoos.state, myBoos.state], 5);"
            << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::COUNT);
    }

    void testSumAggregate() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = sum(myFoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::SUM);
    }

    void testSumAggregateArray() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = sum([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      const auto& state = foo.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.aggregates().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("myFoos"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("myFoos")));
      const auto& myFoo =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("myFoos"));
      TS_ASSERT_EQUALS(&(myFoo.slotType()), &foo);
      TS_ASSERT(myFoo.isArray());
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAggregate(isWorking));
      const auto& agg =
         static_cast< const gum::prm::PRMAggregate< double >& >(isWorking);
      TS_ASSERT_EQUALS(agg.agg_type(),
                       gum::prm::PRMAggregate< double >::AggregateType::SUM);
    }


    void testOrAggregateWithErrors1() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or(myBoos.state);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|6 col 24| Error : Link myBoos in chain myBoos.state "
             "not found";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testOrAggregateWithErrors2() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or(myFoos.st);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|6 col 24| Error : Link st in chain myFoos.st not "
             "found";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testOrAggregateWithErrors3() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or();" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|6 col 24| Error : invalid declaration";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testOrAggregateWithErrors4() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "type state labels(OK, NOK);" << std::endl;
      input << "interface Boo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Boo[] myBoos;" << std::endl
            << "boolean isWorking = or([myFoos.state, myBoos.state]);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|11 col 39| Error : Expected type boolean for parent "
             "myBoos.state, found state";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testCountAggregateWithErrors5() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = count(myFoos.state, foobar);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|7 col 39| Error : Parameter foobar in aggregate "
             "isWorking does not match any expected values";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testInheritance1() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar implements Foo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS("", output.str());
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT(prm.isClass("Bar"));
      TS_ASSERT_EQUALS(foo.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT(foo.isSuperTypeOf(bar));
      TS_ASSERT(bar.implements().contains(
         const_cast< gum::prm::PRMInterface< double >* >(&foo)));
      TS_ASSERT_EQUALS(bar.parameters().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.slotChains().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)1);
    }

    void testInheritance2() {
      // Arrange
      std::stringstream input;
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar implements Foo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS("", output.str());
      TS_ASSERT(prm.isInterface("Foo"));
      const auto& foo = prm.getInterface("Foo");
      TS_ASSERT(prm.isClass("Bar"));
      TS_ASSERT_EQUALS(foo.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT(foo.isSuperTypeOf(bar));
      TS_ASSERT(bar.implements().contains(
         const_cast< gum::prm::PRMInterface< double >* >(&foo)));
      TS_ASSERT_EQUALS(bar.parameters().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.slotChains().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)1);
    }

    void testInheritance3() {
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
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};"
            << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS("", output.str());
      TS_ASSERT(prm.isInterface("iFoo"));
      const auto& ifoo = prm.getInterface("iFoo");
      TS_ASSERT_EQUALS(ifoo.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(ifoo.attributes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Foo"));
      const auto& foo = prm.getClass("Foo");
      TS_ASSERT(ifoo.isSuperTypeOf(foo));
      TS_ASSERT(foo.implements().contains(
         const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT(ifoo.isSuperTypeOf(bar));
      TS_ASSERT(foo.isSuperTypeOf(bar));
      TS_ASSERT_EQUALS(&(bar.super()), &(foo));
      TS_ASSERT(bar.implements().contains(
         const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
    }

    void testInheritanceError1() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|4 col 22| Error : Interface Bar not found";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testInheritanceError2() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|4 col 7| Error : Class Foo does not implement all of "
             "interface iFoo attributes";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testInheritanceError3() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|6 col 7| Error : Class Foo attribute state does not "
             "respect interface iFoo";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testInheritanceError4() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|4 col 19| Error : Unknown class oo";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testInheritanceError5() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|6 col 7| Error : Illegal overload of element state from "
             "class Foo";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testOrderDoesNotMatter1() {
      // Arrange
      std::stringstream input;
      input << "class Bar extends Foo { " << std::endl
            << "state state {[0.5, 0.5]};" << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};"
            << std::endl
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS("", output.str());
      TS_ASSERT(prm.isInterface("iFoo"));
      const auto& ifoo = prm.getInterface("iFoo");
      TS_ASSERT_EQUALS(ifoo.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(ifoo.attributes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Foo"));
      const auto& foo = prm.getClass("Foo");
      TS_ASSERT(ifoo.isSuperTypeOf(foo));
      TS_ASSERT(foo.implements().contains(
         const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT(ifoo.isSuperTypeOf(bar));
      TS_ASSERT(foo.isSuperTypeOf(bar));
      TS_ASSERT_EQUALS(&(bar.super()), &(foo));
      TS_ASSERT(bar.implements().contains(
         const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
    }

    void testOrderDoesNotMatter2() {
      // Arrange
      std::stringstream input;
      input << "class Bar extends Foo { " << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};"
            << std::endl
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS("", output.str());
      TS_ASSERT(prm.isInterface("iFoo"));
      const auto& ifoo = prm.getInterface("iFoo");
      TS_ASSERT_EQUALS(ifoo.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(ifoo.attributes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Foo"));
      const auto& foo = prm.getClass("Foo");
      TS_ASSERT(ifoo.isSuperTypeOf(foo));
      TS_ASSERT(foo.implements().contains(
         const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT(ifoo.isSuperTypeOf(bar));
      TS_ASSERT(foo.isSuperTypeOf(bar));
      TS_ASSERT_EQUALS(&(bar.super()), &(foo));
      TS_ASSERT(bar.implements().contains(
         const_cast< gum::prm::PRMInterface< double >* >(&ifoo)));
    }

    void testPrinters() {
      // Arrange
      auto                    path = GET_RESSOURCES_PATH("o3prm/printers.o3prm");
      gum::prm::PRM< double > prm;
      std::ifstream           input(path);
      std::stringstream       output;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS("", output.str());
    }

    void testClassWithOverloading() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.interfaces().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)2);
      TS_ASSERT(prm.isInterface("Plop"));
      const auto& plop = prm.getInterface("Plop");
      TS_ASSERT_EQUALS(plop.attributes().size(), (gum::Size)1);
      TS_ASSERT(plop.exists("state"));
      TS_ASSERT(plop.exists("(boolean)state"));

      TS_ASSERT(prm.isInterface("SubPlop"));
      const auto& sub_plop = prm.getInterface("SubPlop");
      TS_ASSERT_EQUALS(sub_plop.attributes().size(), (gum::Size)2);
      TS_ASSERT(sub_plop.exists("state"));
      TS_ASSERT(sub_plop.exists("(boolean)state"));
      TS_ASSERT(sub_plop.exists("(state)state"));

      TS_ASSERT(prm.isClass("Foo"));
      const auto& foo = prm.getClass("Foo");
      TS_ASSERT_EQUALS(foo.attributes().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("state"));
      TS_ASSERT(foo.exists("(boolean)state"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isAttribute(foo.get("state")));
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(
         foo.get("(boolean)state")));
      TS_ASSERT_EQUALS(&(foo.get("state")), &(foo.get("(boolean)state")));
      TS_ASSERT_EQUALS(foo.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(foo.exists("plop"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(foo.get("plop")));
      const auto& ref_plop =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            foo.get("plop"));
      TS_ASSERT_EQUALS(&(ref_plop.slotType()), &plop);

      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("state"));
      TS_ASSERT(bar.exists("(boolean)state"));
      TS_ASSERT(bar.exists("(state)state"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isAttribute(bar.get("state")));
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(
         bar.get("(boolean)state")));
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(
         bar.get("(state)state")));
      TS_ASSERT_EQUALS(&(bar.get("state")), &(bar.get("(state)state")));
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)1);
      TS_ASSERT(bar.exists("plop"));
      TS_ASSERT(
         gum::prm::PRMClassElement< double >::isReferenceSlot(bar.get("plop")));
      const auto& ref_subplop =
         static_cast< const gum::prm::PRMReferenceSlot< double >& >(
            bar.get("plop"));
      TS_ASSERT_EQUALS(&(ref_subplop.slotType()), &sub_plop);
    }

    void testClassWithOverloadingError1() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|5 col 33| Error : Illegal overload of element plop from class "
             "Bar";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testAggregateWithError() {
      // Arrange
      std::stringstream input;
      input << "type state int(0,10);" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = count([myFoos, myBoos.state], 5);" << std::endl
            << "}";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|8 col 26| Error : Illegal parent myFoos";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testAnonymousType1() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.4, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.6, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testAnonymousType2() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.4, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.6, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testAnonymousType3() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.4, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.6, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testAnonymousType4() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)1);
      TS_ASSERT(prm.isClass("Bar"));
      const auto& bar = prm.getClass("Bar");
      TS_ASSERT_EQUALS(bar.attributes().size(), (gum::Size)2);
      TS_ASSERT(bar.exists("state"));
      const auto& state = bar.get("state");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(state));
      TS_ASSERT(bar.exists("isWorking"));
      const auto& isWorking = bar.get("isWorking");
      TS_ASSERT(gum::prm::PRMClassElement< double >::isAttribute(isWorking));
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(cpf.variablesSequence().exists(&(state.type().variable())));
      TS_ASSERT_EQUALS(cpf.domainSize(), (gum::Size)4);
      auto i = gum::Instantiation(cpf);
      i.setFirst();
      TS_ASSERT_DELTA(cpf[i], 0.2, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.8, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.4, 1e-6);
      i.inc();
      TS_ASSERT_DELTA(cpf[i], 0.6, 1e-6);
      TS_ASSERT_EQUALS(bar.referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_THROWS(bar.super(), gum::NotFound);
    }

    void testAnonymousTypeError1() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "|2 col 6| Error : invalid declaration\n");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)0);
    }

    void testAnonymousTypeError2() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "|2 col 8| Error : invalid declaration\n");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)0);
    }

    void testAnonymousTypeError3() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "|2 col 3| Error : invalid declaration\n");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)0);
    }

    void testAnonymousTypeError4() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "|2 col 5| Error : integer expected\n");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)0);
    }

    void testAnonymousTypeError5() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "|2 col 4| Error : invalid declaration\n");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)0);
    }

    void testAnonymousTypeError6() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "|2 col 6| Error : invalid declaration\n");
      TS_ASSERT_EQUALS(prm.classes().size(), (gum::Size)0);
    }
  };

}   // namespace gum_tests
