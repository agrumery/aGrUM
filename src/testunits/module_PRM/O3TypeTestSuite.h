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

#include <agrum/prm/PRM.h>
#include <agrum/prm/newo3prm/utils.h>
#include <agrum/prm/newo3prm/o3prm.h>

namespace gum_tests {

  class O3TypeTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testSimpleType() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.types().size(), 2 );
      TS_ASSERT( prm.isType( "state" ) );
      auto state = prm.type( "state" );
      TS_ASSERT_EQUALS( state.variable().domainSize(), 2 );
      TS_ASSERT_EQUALS( state.variable().label( 0 ), "OK" );
      TS_ASSERT_EQUALS( state.variable().label( 1 ), "NOK" );
    }

    void testSimpleTypeError1() {
      // Arrange
      auto input = std::stringstream();
      input << "types state OK, NOK;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 1| Syntax error : type expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.types().size(), 1 );
      TS_ASSERT( not prm.isType( "state" ) );
    }

    void testSimpleTypeError2() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      auto msg = std::stringstream();
      msg << "|1 col 19| Syntax error : semicolon expected" << std::endl;
      TS_ASSERT_EQUALS( output.str(), msg.str() );
      TS_ASSERT_EQUALS( prm.types().size(), 1 );
      TS_ASSERT( not prm.isType( "state" ) );
    }

    void testSimpleTypeError3() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK" << std::endl << "type t_ink empty, full;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      auto msg = std::stringstream();
      msg << "|2 col 1| Syntax error : semicolon expected" << std::endl;
      TS_ASSERT_EQUALS( output.str(), msg.str() );
      TS_ASSERT_EQUALS( prm.types().size(), 2 );
      TS_ASSERT( not prm.isType( "state" ) );
      TS_ASSERT( prm.isType( "t_ink" ) );
    }

    void testSimpleTypeError4() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      auto msg = std::stringstream();
      msg << "|1 col 14| Syntax error : comma expected" << std::endl;
      TS_ASSERT_EQUALS( output.str(), msg.str() );
      TS_ASSERT_EQUALS( prm.types().size(), 1 );
      TS_ASSERT( not prm.isType( "state" ) );
    }

    void testSimpleTypeError5() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK,;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      auto msg = std::stringstream();
      msg << "|1 col 15| Syntax error : label expected" << std::endl;
      TS_ASSERT_EQUALS( output.str(), msg.str() );
      TS_ASSERT_EQUALS( prm.types().size(), 1 );
      TS_ASSERT( not prm.isType( "state" ) );
    }

    void testSimpleTypeError6() {
      // Arrange
      auto input = std::stringstream();
      input << "type +==+ OK, NOK;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      auto msg = std::stringstream();
      msg << "|1 col 6| Syntax error : label expected" << std::endl;
      TS_ASSERT_EQUALS( output.str(), msg.str() );
      TS_ASSERT_EQUALS( prm.types().size(), 1 );
      TS_ASSERT( not prm.isType( "state" ) );
    }

    void testSimpleTypeError7() {
      // Arrange
      auto input = std::stringstream();
      input << "type state +=%./, NOK;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      auto msg = std::stringstream();
      msg << "|1 col 12| Syntax error : invalid type declaration" << std::endl;
      TS_ASSERT_EQUALS( output.str(), msg.str() );
      TS_ASSERT_EQUALS( prm.types().size(), 1 );
      TS_ASSERT( not prm.isType( "state" ) );
    }

    void testSimpleTypeError8() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, +NOK;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      auto msg = std::stringstream();
      msg << "|1 col 16| Syntax error : label expected" << std::endl;
      TS_ASSERT_EQUALS( output.str(), msg.str() );
      TS_ASSERT_EQUALS( prm.types().size(), 1 );
      TS_ASSERT( not prm.isType( "state" ) );
    }

    void testExtendedType() {
      // Arrange
      auto input = std::stringstream();
      input << "type state extends boolean "
            << "OK: true,"
            << "NOK: false;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.types().size(), 2 );
      TS_ASSERT( prm.isType( "state" ) );
      const auto& boolean = prm.type( "boolean" );
      const auto& state = prm.type( "state" );
      TS_ASSERT( state.isSubTypeOf( boolean ) );
      const auto& map = state.label_map();
      TS_ASSERT_EQUALS( map.size(), 2 );
      TS_ASSERT_EQUALS( map[0], 1 );
      TS_ASSERT_EQUALS( map[1], 0 );
    }

    void testExtendedTypeError1() {
      // Arrange
      auto input = std::stringstream();
      input << "type state extend boolean "
            << "OK: true,"
            << "NOK: false;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      TS_ASSERT_EQUALS( prm.types().size(), 1 );
      TS_ASSERT( not prm.isType( "state" ) );
      auto line = std::string();
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 19| Syntax error : comma expected";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testExtendedTypeError2() {
      // Arrange
      auto input = std::stringstream();
      input << "type state extends foobar "
            << "OK: true,"
            << "NOK: false;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      TS_ASSERT_EQUALS( prm.types().size(), 1 );
      TS_ASSERT( not prm.isType( "state" ) );
      auto line = std::string();
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 20| Type error : Unknown type foobar";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testExtendedTypeError3() {
      // Arrange
      auto input = std::stringstream();
      input << "type state extends boolean " << std::endl
            << "OK: vrue," << std::endl
            << "NOK: false;" << std::endl;
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      TS_ASSERT_EQUALS( prm.types().size(), 1 );
      TS_ASSERT( not prm.isType( "state" ) );
      auto line = std::string();
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|2 col 5| Type error : Unknown label vrue in boolean";
      TS_ASSERT_EQUALS( line, msg.str() );
    }
  };

}  // namespace gum_tests
