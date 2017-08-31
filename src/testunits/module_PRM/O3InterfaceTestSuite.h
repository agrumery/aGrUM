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
#include <cmath>
#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {

  class O3InterfaceTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testEmptyInterface() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { }";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testEmptyInterfaceError1() {
      // Arrange
      std::stringstream input;
      input << "interfaces IBar { }";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|1 col 1| Error : invalid declaration";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( !prm.isInterface( "IBar" ) );
    }

    void testEmptyInterfaceError2() {
      // Arrange
      std::stringstream input;
      input << "interface { }";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|1 col 11| Error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( !prm.isInterface( "IBar" ) );
    }

    void testEmptyInterfaceError3() {
      // Arrange
      std::stringstream input;
      input << "interface ++++/ze { }";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|1 col 11| Error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( !prm.isInterface( "IBar" ) );
    }

    void testSimpleInterface() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "boolean state;" << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testSimpleInterfaceError1() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "t_state state;" << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|2 col 1| Error : Unknown type t_state";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testSimpleInterfaceError2() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "=%+ state;" << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|2 col 1| Error : \"}\" expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( !prm.isInterface( "IBar" ) );
    }

    void testSimpleInterfaceError3() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl << "state;" << std::endl << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|2 col 6| Error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( !prm.isInterface( "IBar" ) );
    }

    void testSimpleInterfaceError4() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "boolean state" << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|3 col 1| Error : semicolon expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( !prm.isInterface( "IBar" ) );
    }

    void testSimpleInterfaceError5() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "boolean +/+/;" << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|2 col 9| Error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( !prm.isInterface( "IBar" ) );
    }

    void testWeAreOKWithSemicolons() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "boolean state;" << std::endl
            << "};";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testSimpleInterfaceError6() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "boolean state;" << std::endl
            << "boolean state;" << std::endl
            << "};";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|3 col 1| Error : Element state already exists";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testInterfaceWithReference() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}";
      input << "interface IBar { " << std::endl
            << "IFoo myFoo;" << std::endl
            << "boolean state;" << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.getInterface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testSuperInterface() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}";
      input << "interface IBar extends IFoo { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.getInterface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT_EQUALS( i_bar.super(), i_foo );
    }

    void testSuperInterfaceError1() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extends IFooBar { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|4 col 24| Error : Interface IFooBar not found";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( !prm.isInterface( "IFoo" ) );
      TS_ASSERT( !prm.isInterface( "IBar" ) );
    }

    void testSuperInterfaceError2() {
      // Arrange
      std::stringstream input;
      input << "type t_state labels(OK, NOK);" << std::endl;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extends t_state { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|5 col 24| Error : Interface t_state not found";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( !prm.isInterface( "IFoo" ) );
      TS_ASSERT( !prm.isInterface( "IBar" ) );
    }

    void testSuperInterfaceError3() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extend IFoo { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|4 col 16| Error : \"{\" expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
    }

    void testSuperInterfaceWithAttributeOverload() {
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
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.getInterface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT_EQUALS( i_bar.super(), i_foo );
    }

    void testSuperInterfaceWithReferenceOverload() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extends IFoo { " << std::endl << "}";
      input << "interface IPlop { " << std::endl
            << "IFoo myFoo;" << std::endl
            << "}";
      input << "interface IPloc extends IPlop { " << std::endl
            << "IBar myFoo; " << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)4 );
      TS_ASSERT( prm.isInterface( "IPlop" ) );
      const auto& i_plop = prm.getInterface( "IPlop" );
      TS_ASSERT_EQUALS( i_plop.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( i_plop.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT_THROWS( i_plop.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IPloc" ) );
      const auto& i_ploc = prm.getInterface( "IPloc" );
      TS_ASSERT_EQUALS( i_ploc.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( i_ploc.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_ploc.super(), i_plop );
    }

    void testOrderDoesNotMatter1() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "t_state state;" << std::endl
            << "}" << std::endl
            << "type t_state labels(OK, NOK);";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isType( "t_state" ) );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testOrderDoesNotMatter2() {
      // Arrange
      std::stringstream input;
      input << "interface IBar extends IFoo { " << std::endl
            << "t_state state;" << std::endl
            << "}";
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "type t_state extends boolean (" << std::endl
            << "OK:true, NOK:false);" << std::endl;
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.getInterface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT_EQUALS( i_bar.super(), i_foo );
    }

    void testOrderDoesNotMatter3() {
      // Arrange
      std::stringstream input;
      input << "interface IBar { " << std::endl
            << "IFoo foo;" << std::endl
            << "boolean state;" << std::endl
            << "}";
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.getInterface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testOrderDoesNotMatter4() {
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
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.getInterface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.getInterface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testInterfaceWithReferenceError() {
      // Arrange
      std::stringstream input;
      input << "interface IFoo { " << std::endl
            << "IFoo foo;" << std::endl
            << "boolean state;" << std::endl
            << "}";
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|2 col 1| Error : Interface IFoo cannot reference "
             "itself";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
    }

    void testInterfaceWithOverloading() {
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
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)4 );
      TS_ASSERT( prm.isInterface( "Plop" ) );
      const auto& plop = prm.getInterface( "Plop" );
      TS_ASSERT_EQUALS( plop.attributes().size(), (gum::Size)1 );
      TS_ASSERT( plop.exists( "state" ) );
      TS_ASSERT( plop.exists( "(boolean)state" ) );

      TS_ASSERT( prm.isInterface( "SubPlop" ) );
      const auto& sub_plop = prm.getInterface( "SubPlop" );
      TS_ASSERT_EQUALS( sub_plop.attributes().size(), (gum::Size)2 );
      TS_ASSERT( sub_plop.exists( "state" ) );
      TS_ASSERT( sub_plop.exists( "(boolean)state" ) );
      TS_ASSERT( sub_plop.exists( "(state)state" ) );

      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "plop" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          foo.get( "plop" ) ) );
      const auto& ref_plop =
          static_cast<const gum::prm::PRMReferenceSlot<double>&>(
              foo.get( "plop" ) );
      TS_ASSERT_EQUALS( &( ref_plop.slotType() ), &plop );

      TS_ASSERT( prm.isInterface( "Bar" ) );
      const auto& bar = prm.getInterface( "Bar" );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "plop" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "plop" ) ) );
      const auto& ref_subplop =
          static_cast<const gum::prm::PRMReferenceSlot<double>&>(
              bar.get( "plop" ) );
      TS_ASSERT_EQUALS( &( ref_subplop.slotType() ), &sub_plop );
    }

    void testInterfaceWithOverloadingError1() {
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
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|5 col 29| Error : Illegal overload of element plop";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testInterfaceWithOverloadingError2() {
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
      std::stringstream     output;
      gum::prm::PRM<double> prm;
      auto                  factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      std::stringstream msg;
      msg << "|3 col 34| Error : Illegal overload of element state";
      TS_ASSERT_EQUALS( line, msg.str() );
    }
  };

}  // namespace gum_tests
