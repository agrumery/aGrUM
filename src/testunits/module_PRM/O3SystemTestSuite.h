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
#include <cmath>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/core/exceptions.h>
#include <agrum/prm/PRM.h>
#include <agrum/prm/newo3prm/o3prm.h>
#include <agrum/prm/newo3prm/O3PRMFactory.h>

namespace gum_tests {

  class O3SystemTestSuite : public CxxTest::TestSuite {
    public:
    gum::prm::PRM<double>* prm;

    void setUp() {
      try {
        auto path = GET_RESSOURCES_PATH( "o3prm/printers.o3prm" );
        prm = new gum::prm::PRM<double>();
        auto input = std::ifstream( path );
        auto output = std::stringstream();
        gum::prm::o3prm::parse_stream( *prm, input, output );
        if ( output.str() != "" ) {
          GUM_TRACE( output.str() );
          GUM_ERROR( gum::FatalError, "corrupted ressource file" );
        }
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
        throw e;
      }
    }

    void tearDown() { delete prm; }

    void testSimpleSystem() {
      // Arrange
      auto input = std::stringstream();
      input << "system Foo { }";
      auto output = std::stringstream();
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( *prm, input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm->systems().size(), 1 );
      TS_ASSERT( prm->isSystem( "Foo" ) );
      const auto& foo = prm->system( "Foo" );
      TS_ASSERT_EQUALS( foo.size(), 0 );
    }

    void testMicroSystem() {
      // Arrange
      auto input = std::stringstream();
      input << "system microSys {" << std::endl
            << "PowerSupply pow;" << std::endl
            << "Room r;" << std::endl
            << "Printer p;" << std::endl
            << "Computer c;" << std::endl
            << "r.power = pow;" << std::endl
            << "p.room = r;" << std::endl
            << "c.room = r;" << std::endl
            << "c.printers = p;" << std::endl
            << "Equipment e;" << std::endl
            << "e.room = r;" << std::endl
            << "}" << std::endl;
      auto output = std::stringstream();
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( *prm, input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm->systems().size(), 1 );
      TS_ASSERT( prm->isSystem( "microSys" ) );
      const auto& foo = prm->system( "microSys" );
      TS_ASSERT_EQUALS( foo.size(), 5 );
    }

    void testSmallSystem() {
      // Arrange
      auto input = std::stringstream();
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
      auto output = std::stringstream();
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( *prm, input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm->systems().size(), 1 );
      TS_ASSERT( prm->isSystem( "smallSys" ) );
      const auto& foo = prm->system( "smallSys" );
      TS_ASSERT_EQUALS( foo.size(), 10 );
    }

  };

}  // namespace gum_tests

