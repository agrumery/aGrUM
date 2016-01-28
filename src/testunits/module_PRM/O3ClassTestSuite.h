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
#include <agrum/prm/newo3prm/o3prm.h>
#include <agrum/prm/newo3prm/O3PRMFactory.h>

namespace gum_tests {

  class O3ClassTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testEmptyClass() {
      // Arrange
      auto input = std::stringstream();
      input << "class Bar { }";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), 1 );
      TS_ASSERT( prm.isClass( "IBar" ) );
      const auto& bar = prm.getClass( "IBar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), 0 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), 0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    //void testEmptyInterfaceError1() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interfaces IBar { }";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|1 col 1| Syntax error : invalid declaration";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 0 );
    //  TS_ASSERT( not prm.isInterface( "IBar" ) );
    //}

    //void testEmptyInterfaceError2() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface { }";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|1 col 11| Syntax error : label expected";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 0 );
    //  TS_ASSERT( not prm.isInterface( "IBar" ) );
    //}

    //void testEmptyInterfaceError3() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface ++++/ze { }";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|1 col 11| Syntax error : label expected";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 0 );
    //  TS_ASSERT( not prm.isInterface( "IBar" ) );
    //}

    //void testSimpleInterface() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IBar { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  TS_ASSERT_EQUALS( output.str(), "" );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 1 );
    //  TS_ASSERT( prm.isInterface( "IBar" ) );
    //  const auto& i_bar = prm.interface( "IBar" );
    //  TS_ASSERT_EQUALS( i_bar.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), 0 );
    //  TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    //}

    //void testSimpleInterfaceError1() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IBar { " << std::endl
    //        << "t_state state;" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|2 col 1| Interface error : Unknown identifier t_state";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 1 );
    //  TS_ASSERT( prm.isInterface( "IBar" ) );
    //  const auto& i_bar = prm.interface( "IBar" );
    //  TS_ASSERT_EQUALS( i_bar.attributes().size(), 0 );
    //  TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), 0 );
    //  TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    //}

    //void testSimpleInterfaceError2() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IBar { " << std::endl
    //        << "=%+ state;" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|2 col 1| Syntax error : \"}\" expected";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 0 );
    //  TS_ASSERT( not prm.isInterface( "IBar" ) );
    //}

    //void testSimpleInterfaceError3() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IBar { " << std::endl
    //        << "state;" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|2 col 6| Syntax error : label expected";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 0 );
    //  TS_ASSERT( not prm.isInterface( "IBar" ) );
    //}

    //void testSimpleInterfaceError4() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IBar { " << std::endl
    //        << "boolean state" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|3 col 1| Syntax error : semicolon expected";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 0 );
    //  TS_ASSERT( not prm.isInterface( "IBar" ) );
    //}

    //void testSimpleInterfaceError5() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IBar { " << std::endl
    //        << "boolean +/+/;" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|2 col 9| Syntax error : label expected";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 0 );
    //  TS_ASSERT( not prm.isInterface( "IBar" ) );
    //}

    //void testWeAreOKWithSemicolons() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IBar { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "};";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  TS_ASSERT_EQUALS( output.str(), "" );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 1 );
    //  TS_ASSERT( prm.isInterface( "IBar" ) );
    //  const auto& i_bar = prm.interface( "IBar" );
    //  TS_ASSERT_EQUALS( i_bar.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), 0 );
    //  TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    //}

    //void testSimpleInterfaceError6() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IBar { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "boolean state;" << std::endl
    //        << "};";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|3 col 1| Interface error : Element state already exists";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 1 );
    //  TS_ASSERT( prm.isInterface( "IBar" ) );
    //  const auto& i_bar = prm.interface( "IBar" );
    //  TS_ASSERT_EQUALS( i_bar.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), 0 );
    //  TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    //}

    //void testInterfaceWithReference() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IFoo { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "}";
    //  input << "interface IBar { " << std::endl
    //        << "IFoo myFoo;" << std::endl
    //        << "boolean state;" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  TS_ASSERT_EQUALS( output.str(), "" );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 2 );
    //  TS_ASSERT( prm.isInterface( "IFoo" ) );
    //  const auto& i_foo = prm.interface( "IFoo" );
    //  TS_ASSERT_EQUALS( i_foo.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), 0 );
    //  TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
    //  TS_ASSERT( prm.isInterface( "IBar" ) );
    //  const auto& i_bar = prm.interface( "IBar" );
    //  TS_ASSERT_EQUALS( i_bar.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), 1 );
    //  TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    //}

    //void testSuperInterface() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IFoo { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "}";
    //  input << "interface IBar extends IFoo { " << std::endl
    //        << "boolean unstate;" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  TS_ASSERT_EQUALS( output.str(), "" );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 2 );
    //  TS_ASSERT( prm.isInterface( "IFoo" ) );
    //  const auto& i_foo = prm.interface( "IFoo" );
    //  TS_ASSERT_EQUALS( i_foo.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), 0 );
    //  TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
    //  TS_ASSERT( prm.isInterface( "IBar" ) );
    //  const auto& i_bar = prm.interface( "IBar" );
    //  TS_ASSERT_EQUALS( i_bar.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_bar.super(), i_foo );
    //}

    //void testSuperInterfaceError1() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IFoo { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "}" << std::endl;
    //  input << "interface IBar extends IFooBar { " << std::endl
    //        << "boolean unstate;" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|4 col 24| Interface error : Unknown interface IFooBar";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 0 );
    //  TS_ASSERT( not prm.isInterface( "IFoo" ) );
    //  TS_ASSERT( not prm.isInterface( "IBar" ) );
    //}

    //void testSuperInterfaceError2() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "type t_state OK, NOK;" << std::endl;
    //  input << "interface IFoo { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "}" << std::endl;
    //  input << "interface IBar extends t_state { " << std::endl
    //        << "boolean unstate;" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|5 col 24| Interface error : Unknown interface t_state";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 0 );
    //  TS_ASSERT( not prm.isInterface( "IFoo" ) );
    //  TS_ASSERT( not prm.isInterface( "IBar" ) );
    //}

    //void testSuperInterfaceError3() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IFoo { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "}" << std::endl;
    //  input << "interface IBar extend IFoo { " << std::endl
    //        << "boolean unstate;" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  std::string line;
    //  std::getline( output, line );
    //  auto msg = std::stringstream();
    //  msg << "|4 col 16| Syntax error : \"{\" expected";
    //  TS_ASSERT_EQUALS( line, msg.str() );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 1 );
    //  TS_ASSERT( prm.isInterface( "IFoo" ) );
    //  const auto& i_foo = prm.interface( "IFoo" );
    //  TS_ASSERT_EQUALS( i_foo.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), 0 );
    //  TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
    //  TS_ASSERT( not prm.isInterface( "IBar" ) );
    //}

    //void testSuperInterfaceWithOverload() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IFoo { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "}" << std::endl;
    //  input << "interface IBar extends IFoo { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "}";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  TS_ASSERT_EQUALS( output.str(), "" );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 2 );
    //  TS_ASSERT( prm.isInterface( "IFoo" ) );
    //  const auto& i_foo = prm.interface( "IFoo" );
    //  TS_ASSERT_EQUALS( i_foo.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), 0 );
    //  TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
    //  TS_ASSERT( prm.isInterface( "IBar" ) );
    //  const auto& i_bar = prm.interface( "IBar" );
    //  TS_ASSERT_EQUALS( i_bar.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_bar.super(), i_foo );
    //}

    //void testOrderDoesNotMatter1() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IBar { " << std::endl
    //        << "t_state state;" << std::endl
    //        << "}" << std::endl
    //        << "type t_state OK, NOK;";
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  TS_ASSERT_EQUALS( output.str(), "" );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 1 );
    //  TS_ASSERT( prm.isType( "t_state" ) );
    //  TS_ASSERT( prm.isInterface( "IBar" ) );
    //  const auto& i_bar = prm.interface( "IBar" );
    //  TS_ASSERT_EQUALS( i_bar.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), 0 );
    //  TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    //}

    //void testOrderDoesNotMatter2() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IBar extends IFoo { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "}";
    //  input << "interface IFoo { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "}" << std::endl;
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  TS_ASSERT_EQUALS( output.str(), "" );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 2 );
    //  TS_ASSERT( prm.isInterface( "IFoo" ) );
    //  const auto& i_foo = prm.interface( "IFoo" );
    //  TS_ASSERT_EQUALS( i_foo.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), 0 );
    //  TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
    //  TS_ASSERT( prm.isInterface( "IBar" ) );
    //  const auto& i_bar = prm.interface( "IBar" );
    //  TS_ASSERT_EQUALS( i_bar.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_bar.super(), i_foo );
    //}

    //void testOrderDoesNotMatter3() {
    //  // Arrange
    //  auto input = std::stringstream();
    //  input << "interface IBar { " << std::endl
    //        << "IFoo foo;" << std::endl
    //        << "boolean state;" << std::endl
    //        << "}";
    //  input << "interface IFoo { " << std::endl
    //        << "boolean state;" << std::endl
    //        << "}" << std::endl;
    //  auto output = std::stringstream();
    //  gum::prm::PRM<double> prm;
    //  // Act
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      gum::prm::o3prm::parse_stream( prm, input, output ) );
    //  // Assert
    //  TS_ASSERT_EQUALS( output.str(), "" );
    //  TS_ASSERT_EQUALS( prm.interfaces().size(), 2 );
    //  TS_ASSERT( prm.isInterface( "IFoo" ) );
    //  const auto& i_foo = prm.interface( "IFoo" );
    //  TS_ASSERT_EQUALS( i_foo.attributes().size(), 1 );
    //  TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), 0 );
    //  TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
    //  TS_ASSERT( prm.isInterface( "IBar" ) );
    //  const auto& i_bar = prm.interface( "IBar" );
    //  TS_ASSERT_EQUALS( i_bar.attributes().size(), 1 );
    //  TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    //}
  };

}  // namespace gum_tests
