/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
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
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/core/math/math.h>
#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {

  class DeprecatedO3TypeTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testSimpleType() {
      // Arrange
      std::stringstream input;
      input << "type t_state OK, NOK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_state is declared using a deprecated syntax.";
      TS_ASSERT_EQUALS(line, msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)2);
      TS_ASSERT(prm.isType("t_state"));
      auto state = prm.type("t_state");
      TS_ASSERT_EQUALS(state.variable().domainSize(), (gum::Size)2);
      TS_ASSERT_EQUALS(state.variable().label(0), "OK");
      TS_ASSERT_EQUALS(state.variable().label(1), "NOK");
    }

    void testSimpleTypeError1() {
      // Arrange
      std::stringstream input;
      input << "types t_state OK, NOK;";
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
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
    }

    void testSimpleTypeError2() {
      // Arrange
      std::stringstream input;
      input << "type t_state OK, NOK";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 21| Error : semicolon expected" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
    }

    void testSimpleTypeError3() {
      // Arrange
      std::stringstream input;
      input << "type t_state OK, NOK" << std::endl << "type t_ink empty, full;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|2 col 1| Error : semicolon expected" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
      TS_ASSERT(!prm.isType("t_ink"));
    }

    void testSimpleTypeError4() {
      // Arrange
      std::stringstream input;
      input << "type t_state OK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 16| Error : comma expected" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
    }

    void testSimpleTypeError5() {
      // Arrange
      std::stringstream input;
      input << "type t_state OK,;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 17| Error : invalid declaration" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
    }

    void testSimpleTypeError6() {
      // Arrange
      std::stringstream input;
      input << "type +==+ OK, NOK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 6| Error : label expected" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
    }

    void testSimpleTypeError7() {
      // Arrange
      std::stringstream input;
      input << "type t_state +=%./, NOK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 14| Error : invalid declaration" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
    }

    void testSimpleTypeError8() {
      // Arrange
      std::stringstream input;
      input << "type t_state OK, +NOK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 18| Error : invalid declaration" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
    }

    void testSimpleTypeError9() {
      // Arrange
      std::stringstream input;
      input << "type t_state OK, NOK;" << std::endl
            << "type t_state OK, YEA, GOO;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_state is declared using a deprecated syntax."
          << std::endl;
      msg << "|2 col 6| Warning : t_state is declared using a deprecated syntax."
          << std::endl;
      msg << "|2 col 6| Error : Type t_state exists already" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
    }

    void testExtendedType() {
      // Arrange
      std::stringstream input;
      input << "type t_state extends boolean "
            << "OK: true,"
            << "NOK: false;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_state is declared using a deprecated syntax."
          << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)2);
      TS_ASSERT(prm.isType("t_state"));
      const auto& boolean = prm.type("boolean");
      const auto& state = prm.type("t_state");
      TS_ASSERT(state.isSubTypeOf(boolean));
      const auto& map = state.label_map();
      TS_ASSERT_EQUALS(map.size(), (gum::Size)2);
      TS_ASSERT_EQUALS(map[0], (gum::Size)1);
      TS_ASSERT_EQUALS(map[1], (gum::Size)0);
    }

    void testExtendedTypeError1() {
      // Arrange
      std::stringstream input;
      input << "type t_state extend boolean "
            << "OK: true,"
            << "NOK: false;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 21| Error : comma expected";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testExtendedTypeError2() {
      // Arrange
      std::stringstream input;
      input << "type t_state extends foobar "
            << "OK: true,"
            << "NOK: false;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_state is declared using a deprecated syntax."
          << std::endl;
      msg << "|1 col 22| Error : Unknown type foobar" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
    }

    void testExtendedTypeError3() {
      // Arrange
      std::stringstream input;
      input << "type t_state extends boolean " << std::endl
            << "OK: vrue," << std::endl
            << "NOK: false;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_state"));
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_state is declared using a deprecated syntax."
          << std::endl;
      msg << "|2 col 5| Error : Unknown label vrue in boolean" << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
    }

    void testOrderDoesNotMatter() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)3);
      TS_ASSERT(prm.isType("t_state"));
      TS_ASSERT(prm.isType("t_degraded"));
      std::stringstream msg;
      msg << "|1 col 6| Warning : t_degraded is declared using a deprecated "
             "syntax."
          << std::endl;
      msg << "|5 col 6| Warning : t_state is declared using a deprecated syntax."
          << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
    }

    void testIntType() {
      // Arrange
      std::stringstream input;
      input << "int (0,9) t_power;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 11| Warning : t_power is declared using a deprecated syntax."
          << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)2);
      TS_ASSERT(prm.isType("t_power"));
      auto power = prm.type("t_power");
      TS_ASSERT_EQUALS(power.variable().domainSize(), (gum::Size)10);
      TS_ASSERT_EQUALS(power.variable().label(0), "0");
      TS_ASSERT_EQUALS(power.variable().label(1), "1");
      TS_ASSERT_EQUALS(power.variable().label(2), "2");
      TS_ASSERT_EQUALS(power.variable().label(3), "3");
      TS_ASSERT_EQUALS(power.variable().label(4), "4");
      TS_ASSERT_EQUALS(power.variable().label(5), "5");
      TS_ASSERT_EQUALS(power.variable().label(6), "6");
      TS_ASSERT_EQUALS(power.variable().label(7), "7");
      TS_ASSERT_EQUALS(power.variable().label(8), "8");
      TS_ASSERT_EQUALS(power.variable().label(9), "9");
    }

    void testIntTypeWithNegatives() {
      try {
        // Arrange
        std::stringstream input;
        input << "int (-9,9) t_power;";
        std::stringstream       output;
        gum::prm::PRM< double > prm;
        auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
        // Assert
        std::stringstream msg;
        msg << "|1 col 12| Warning : t_power is declared using a deprecated "
               "syntax."
            << std::endl;
        TS_ASSERT_EQUALS(output.str(), msg.str());
        TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)2);
        TS_ASSERT(prm.isType("t_power"));
        auto power = prm.type("t_power");
        TS_ASSERT_EQUALS(power.variable().domainSize(), (gum::Size)19);
        TS_ASSERT_EQUALS(power.variable().label(0), "-9");
        TS_ASSERT_EQUALS(power.variable().label(1), "-8");
        TS_ASSERT_EQUALS(power.variable().label(2), "-7");
        TS_ASSERT_EQUALS(power.variable().label(3), "-6");
        TS_ASSERT_EQUALS(power.variable().label(4), "-5");
        TS_ASSERT_EQUALS(power.variable().label(5), "-4");
        TS_ASSERT_EQUALS(power.variable().label(6), "-3");
        TS_ASSERT_EQUALS(power.variable().label(7), "-2");
        TS_ASSERT_EQUALS(power.variable().label(8), "-1");
        TS_ASSERT_EQUALS(power.variable().label(9), "0");
        TS_ASSERT_EQUALS(power.variable().label(10), "1");
        TS_ASSERT_EQUALS(power.variable().label(11), "2");
        TS_ASSERT_EQUALS(power.variable().label(12), "3");
        TS_ASSERT_EQUALS(power.variable().label(13), "4");
        TS_ASSERT_EQUALS(power.variable().label(14), "5");
        TS_ASSERT_EQUALS(power.variable().label(15), "6");
        TS_ASSERT_EQUALS(power.variable().label(16), "7");
        TS_ASSERT_EQUALS(power.variable().label(17), "8");
      } catch (gum::OutOfBounds& e) { GUM_SHOWERROR(e); }
    }

    void testIntTypeError1() {
      // Arrange
      std::stringstream input;
      input << "ints (0,9) t_power;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 1| Error : invalid declaration";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testIntTypeError2() {
      // Arrange
      std::stringstream input;
      input << "int 0,9 t_power;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 5| Error : \"(\" expected";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testIntTypeError3() {
      // Arrange
      std::stringstream input;
      input << "int (0 9) t_power;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 8| Error : comma expected";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testIntTypeError4() {
      // Arrange
      std::stringstream input;
      input << "int (0, 9) +==++;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 12| Error : label expected";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testIntTypeError5() {
      // Arrange
      std::stringstream input;
      input << "int (plop, 9) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 6| Error : integer expected";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testIntTypeError6() {
      // Arrange
      std::stringstream input;
      input << "int (0, 9, 15) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 10| Error : \")\" expected";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testIntTypeError7() {
      // Arrange
      std::stringstream input;
      input << "int (0.0, 9) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 6| Error : integer expected";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testIntTypeError8() {
      // Arrange
      std::stringstream input;
      input << "int (9, 9) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 19| Error : semicolon expected";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testIntTypeError9() {
      // Arrange
      std::stringstream input;
      input << "int (10, 9) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 20| Error : semicolon expected";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testIntTypeError10() {
      // Arrange
      std::stringstream input;
      input << "int (9, 0) t_power";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)1);
      TS_ASSERT(!prm.isType("t_power"));
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 19| Error : semicolon expected";
      TS_ASSERT_EQUALS(line, msg.str());
    }

    void testTypeInModule1() {
      // Arrange
      std::stringstream input;
      input << "type t_state OK, NOK;";
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output, "fr.agrum"));
      // Assert
      std::stringstream msg;
      msg << "|1 col 6| Warning : fr.agrum.t_state is declared using a deprecated "
             "syntax."
          << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)2);
      TS_ASSERT(!prm.isType("t_state"));
      TS_ASSERT(prm.isType("fr.agrum.t_state"));
    }

    void testTypeInModule2() {
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
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output, "fr.agrum"));
      // Assert
      std::stringstream msg;
      msg << "|1 col 6| Warning : fr.agrum.t_degraded is declared using a "
             "deprecated syntax."
          << std::endl;
      msg << "|5 col 6| Warning : fr.agrum.t_state is declared using a deprecated "
             "syntax."
          << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)3);
      TS_ASSERT(!prm.isType("t_state"));
      TS_ASSERT(prm.isType("fr.agrum.t_state"));
      TS_ASSERT(!prm.isType("t_degraded"));
      TS_ASSERT(prm.isType("fr.agrum.t_degraded"));
    }

    void testRealType1() {
      // Arrange
      std::stringstream input;
      input << "real(0, 90, 180) angle;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 18| Warning : angle is declared using a deprecated syntax."
          << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)2);
      TS_ASSERT(prm.isType("angle"));
      const auto& angle = prm.type("angle");
      TS_ASSERT_EQUALS(angle.variable().labels().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(angle.variable().labels().at(0), "[0;90[");
      TS_ASSERT_EQUALS(angle.variable().labels().at(1), "[90;180]");
    }

    void testRealType2() {
      // Arrange
      std::stringstream input;
      input << "real(0, 90, 180, 360) angle;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 23| Warning : angle is declared using a deprecated syntax."
          << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
      TS_ASSERT_EQUALS(prm.types().size(), (gum::Size)2);
      TS_ASSERT(prm.isType("angle"));
      const auto& angle = prm.type("angle");
      TS_ASSERT_EQUALS(angle.variable().labels().size(), (gum::Size)3);
      TS_ASSERT_EQUALS(angle.variable().labels().at(0), "[0;90[");
      TS_ASSERT_EQUALS(angle.variable().labels().at(1), "[90;180[");
      TS_ASSERT_EQUALS(angle.variable().labels().at(2), "[180;360]");
    }

    void testRealTypeError1() {
      // Arrange
      std::stringstream input;
      input << "real(0, 90) angle;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 13| Warning : angle is declared using a deprecated syntax."
          << std::endl;
      msg << "|1 col 1| Error : Found 2 values in range expected at least 3"
          << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
    }

    void testRealTypeError2() {
      // Arrange
      std::stringstream input;
      input << "real(0) angle;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::stringstream msg;
      msg << "|1 col 9| Warning : angle is declared using a deprecated syntax."
          << std::endl;
      msg << "|1 col 1| Error : Found 1 values in range expected at least 3"
          << std::endl;
      TS_ASSERT_EQUALS(output.str(), msg.str());
    }

    void testRealTypeError3() {
      // Arrange
      std::stringstream input;
      input << "real(0, plop) angle;" << std::endl;
      std::stringstream       output;
      gum::prm::PRM< double > prm;
      auto factory = gum::prm::o3prm::O3prmReader< double >(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      auto line = std::string();
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 9| Error : invalid declaration";
      TS_ASSERT_EQUALS(line, msg.str());
    }
  };
}   // namespace gum_tests
