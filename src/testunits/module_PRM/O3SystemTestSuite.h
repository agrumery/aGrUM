/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <agrum/tools/core/exceptions.h>

namespace gum_tests {

  class O3SystemTestSuite: public CxxTest::TestSuite {
    public:
    gum::prm::PRM< double >* simple_printers;
    gum::prm::PRM< double >* complex_printers;

    void setUp() {
      try {
        {
          // printers.o3prm
          auto path       = GET_RESSOURCES_PATH("o3prm/printers.o3prm");
          simple_printers = new gum::prm::PRM< double >();
          auto factory    = gum::prm::o3prm::O3prmReader< double >(*simple_printers);
          factory.readFile(path);
          if (factory.errors() > 0) {
            factory.showElegantErrorsAndWarnings();
            GUM_ERROR(gum::FatalError, "corrupted ressource file")
          }
        }
        {
          // complex_printers.o3prm
          auto path        = GET_RESSOURCES_PATH("o3prm/complexprinters.o3prm");
          complex_printers = new gum::prm::PRM< double >();
          auto factory     = gum::prm::o3prm::O3prmReader< double >(*complex_printers);
          factory.readFile(path, "fr.lip6.printers");
          if (factory.errors() > 0) {
            factory.showElegantErrorsAndWarnings();
            GUM_ERROR(gum::FatalError, "corrupted ressource file")
          }
        }
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void tearDown() {
      delete simple_printers;
      delete complex_printers;
    }

    void testSimpleSystem() {
      // Arrange
      std::stringstream input;
      input << "system Foo { }";
      std::stringstream output;
      auto              factory = gum::prm::o3prm::O3prmReader< double >(*simple_printers);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(simple_printers->systems().size(), (gum::Size)1)
      TS_ASSERT(simple_printers->isSystem("Foo"))
      const auto& foo = simple_printers->getSystem("Foo");
      TS_ASSERT_EQUALS(foo.size(), (gum::Size)0)
    }

    void testMicroSystem() {
      // Arrange
      std::stringstream input;
      input << "system microSys {" << std::endl
            << "PowerSupply pow;" << std::endl
            << "Room r;" << std::endl
            << "Printer p;" << std::endl
            << "Computer c;" << std::endl
            << "r.power = pow;" << std::endl
            << "p.room = r;" << std::endl
            << "c.room = r;" << std::endl
            << "c.printers += p;" << std::endl
            << "Equipment e;" << std::endl
            << "e.room = r;" << std::endl
            << "}" << std::endl;
      std::stringstream output;
      auto              factory = gum::prm::o3prm::O3prmReader< double >(*simple_printers);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(simple_printers->systems().size(), (gum::Size)1)
      TS_ASSERT(simple_printers->isSystem("microSys"))
      const auto& foo = simple_printers->getSystem("microSys");
      TS_ASSERT_EQUALS(foo.size(), (gum::Size)5)
    }

    void testMicroSystemWithError1() {
      // Arrange
      std::stringstream input;
      input << "system microSys {" << std::endl << "PowerSupply;" << std::endl << "}" << std::endl;
      std::stringstream output;
      auto              factory = gum::prm::o3prm::O3prmReader< double >(*simple_printers);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 12| Error : invalid declaration";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(simple_printers->systems().size(), (gum::Size)0)
    }

    void testMicroSystemWithError2() {
      // Arrange
      std::stringstream input;
      input << "system microSys {" << std::endl << "FOO bar;" << std::endl << "}" << std::endl;
      std::stringstream output;
      auto              factory = gum::prm::o3prm::O3prmReader< double >(*simple_printers);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|2 col 1| Error : Unknown class FOO";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(simple_printers->systems().size(), (gum::Size)0)
    }

    void testMicroSystemWithError3() {
      // Arrange
      std::stringstream input;
      input << "system {" << std::endl << "PowerSupply pow;" << std::endl << "}" << std::endl;
      std::stringstream output;
      auto              factory = gum::prm::o3prm::O3prmReader< double >(*simple_printers);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 8| Error : label expected";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(simple_printers->systems().size(), (gum::Size)0)
    }

    void testMicroSystemWithError4() {
      // Arrange
      std::stringstream input;
      input << "system microSys {" << std::endl
            << "PowerSupply pow" << std::endl
            << "Room r;" << std::endl
            << "}" << std::endl;
      std::stringstream output;
      auto              factory = gum::prm::o3prm::O3prmReader< double >(*simple_printers);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|3 col 1| Error : semicolon expected";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(simple_printers->systems().size(), (gum::Size)0)
    }

    void testMicroSystemWithError5() {
      // Arrange
      std::stringstream input;
      input << "system microSys {" << std::endl
            << "PowerSupply pow;" << std::endl
            << "Room r;" << std::endl
            << "Printer p;" << std::endl
            << "Computer c;" << std::endl
            << "r.power   pow;" << std::endl
            << "}" << std::endl;
      std::stringstream output;
      auto              factory = gum::prm::o3prm::O3prmReader< double >(*simple_printers);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|6 col 1| Error : Unknown class r.power";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(simple_printers->systems().size(), (gum::Size)0)
    }

    void testMicroSystemWithError6() {
      // Arrange
      std::stringstream input;
      input << "system microSys {" << std::endl
            << "PowerSupply pow;" << std::endl
            << "Room r;" << std::endl
            << "Printer p;" << std::endl
            << "Computer c;" << std::endl
            << "r.power = pow;" << std::endl
            << "p.room = r;" << std::endl
            << "//c.room = r;" << std::endl
            << "c.printers += p;" << std::endl
            << "}" << std::endl;
      std::stringstream output;
      auto              factory = gum::prm::o3prm::O3prmReader< double >(*simple_printers);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|1 col 8| Error : Could not instantiate the system, some "
             "reference slots must be unassigned";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(simple_printers->systems().size(), (gum::Size)1)
    }

    void testMicroSystemWithError7() {
      // Arrange
      std::stringstream input;
      input << "system microSys {" << std::endl
            << "PowerSupply pow;" << std::endl
            << "Room r;" << std::endl
            << "Printer p;" << std::endl
            << "Printer p;" << std::endl
            << "Computer c;" << std::endl
            << "r.power = pow;" << std::endl
            << "p.room = r;" << std::endl
            << "//c.room = r;" << std::endl
            << "c.printers = p;" << std::endl
            << "}" << std::endl;
      std::stringstream output;
      auto              factory = gum::prm::o3prm::O3prmReader< double >(*simple_printers);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      std::string line;
      std::getline(output, line);
      std::stringstream msg;
      msg << "|5 col 1| Error : Instance p already exists";
      TS_ASSERT_EQUALS(line, msg.str())
      TS_ASSERT_EQUALS(simple_printers->systems().size(), (gum::Size)0)
    }

    void testSmallSystem() {
      // Arrange
      std::stringstream input;
      input << "system smallSys {" << std::endl
            << "  PowerSupply pow;" << std::endl
            << "  Room r;" << std::endl
            << "  Printer[2] printers;" << std::endl
            << "  Printer another_printer;" << std::endl
            << "  Computer[4] computers;" << std::endl
            << "  Computer another_computer;" << std::endl
            << "  r.power = pow;" << std::endl
            << "  printers[0].room = r;" << std::endl
            << "  printers[1].room = r;" << std::endl
            << "  another_printer.room = r;" << std::endl
            << "  computers[0].room = r;" << std::endl
            << "  computers[1].room = r;" << std::endl
            << "  computers[2].room = r;" << std::endl
            << "  computers[3].room = r;" << std::endl
            << "  another_computer.room = r;" << std::endl
            << "  computers[0].printers = printers;" << std::endl
            << "  computers[0].printers += another_printer;" << std::endl
            << "  computers[1].printers = printers;" << std::endl
            << "  computers[1].printers += another_printer;" << std::endl
            << "  computers[2].printers = printers;" << std::endl
            << "  computers[2].printers += another_printer;" << std::endl
            << "  computers[3].printers = printers;" << std::endl
            << "  computers[3].printers += another_printer;" << std::endl
            << "  another_computer.printers = printers;" << std::endl
            << "  another_computer.printers += another_printer;" << std::endl
            << "}";
      std::stringstream output;
      auto              factory = gum::prm::o3prm::O3prmReader< double >(*simple_printers);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(simple_printers->systems().size(), (gum::Size)1)
      TS_ASSERT(simple_printers->isSystem("smallSys"))
      const auto& foo = simple_printers->getSystem("smallSys");
      TS_ASSERT_EQUALS(foo.size(), (gum::Size)10)
    }

    void testComplexSystem() {
      // Arrange
      std::stringstream input;
      input << "system aSys {" << std::endl
            << "  PowerSupply pow;" << std::endl
            << "  Room r;" << std::endl
            << "  BWPrinter[10] bw_printers;" << std::endl
            << "  ColorPrinter[2] color_printers;" << std::endl
            << "  bw_printers.room = r;" << std::endl
            << "  color_printers.room = r;" << std::endl
            << "  r.power = pow;" << std::endl
            << "  Computer c1;" << std::endl
            << "  Computer c2;" << std::endl
            << "  c1.room = r;" << std::endl
            << "  c2.room = r;" << std::endl
            << "  c1.printers = bw_printers;" << std::endl
            << "  c2.printers = bw_printers;" << std::endl
            << "  c1.printers += color_printers;" << std::endl
            << "  c2.printers += color_printers;" << std::endl
            << "  ParamClass params;" << std::endl
            << "  params.room = r;" << std::endl
            << "  ParamClass paramBis(lambda=0.001);" << std::endl
            << "  paramBis.room = r;" << std::endl
            << "}";
      std::stringstream output;
      auto              factory = gum::prm::o3prm::O3prmReader< double >(*complex_printers);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(factory.parseStream(input, output));
      // Assert
      TS_ASSERT_EQUALS(output.str(), "")
      TS_ASSERT_EQUALS(complex_printers->systems().size(), (gum::Size)1)
      TS_ASSERT(complex_printers->isSystem("aSys"))
      const auto& foo = complex_printers->getSystem("aSys");
      TS_ASSERT_EQUALS(foo.size(), (gum::Size)18)
    }
  };

}   // namespace gum_tests
