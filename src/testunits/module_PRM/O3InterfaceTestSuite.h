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

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/newo3prm/o3prm.h>
#include <agrum/PRM/newo3prm/O3PRMFactory.h>

namespace gum_tests {

  class O3InterfaceTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testEmptyInterface() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { }";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>(prm);
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testEmptyInterfaceError1() {
      // Arrange
      auto input = std::stringstream();
      input << "interfaces IBar { }";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 1| Error : invalid declaration";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( not prm.isInterface( "IBar" ) );
    }

    void testEmptyInterfaceError2() {
      // Arrange
      auto input = std::stringstream();
      input << "interface { }";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 11| Error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( not prm.isInterface( "IBar" ) );
    }

    void testEmptyInterfaceError3() {
      // Arrange
      auto input = std::stringstream();
      input << "interface ++++/ze { }";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 11| Error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( not prm.isInterface( "IBar" ) );
    }

    void testSimpleInterface() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { " << std::endl
            << "boolean state;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testSimpleInterfaceError1() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { " << std::endl
            << "t_state state;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|2 col 1| Error : Unknown type t_state";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testSimpleInterfaceError2() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { " << std::endl
            << "=%+ state;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|2 col 1| Error : \"}\" expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( not prm.isInterface( "IBar" ) );
    }

    void testSimpleInterfaceError3() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { " << std::endl
            << "state;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|2 col 6| Error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( not prm.isInterface( "IBar" ) );
    }

    void testSimpleInterfaceError4() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { " << std::endl
            << "boolean state" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|3 col 1| Error : semicolon expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( not prm.isInterface( "IBar" ) );
    }

    void testSimpleInterfaceError5() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { " << std::endl
            << "boolean +/+/;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|2 col 9| Error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( not prm.isInterface( "IBar" ) );
    }

    void testWeAreOKWithSemicolons() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { " << std::endl
            << "boolean state;" << std::endl
            << "};";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testSimpleInterfaceError6() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { " << std::endl
            << "boolean state;" << std::endl
            << "boolean state;" << std::endl
            << "};";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|3 col 1| Error : Element state already exists";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testInterfaceWithReference() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}";
      input << "interface IBar { " << std::endl
            << "IFoo myFoo;" << std::endl
            << "boolean state;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.interface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testSuperInterface() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}";
      input << "interface IBar extends IFoo { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.interface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT_EQUALS( i_bar.super(), i_foo );
    }

    void testSuperInterfaceError1() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extends IFooBar { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|4 col 24| Error : Interface IFooBar not found";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( not prm.isInterface( "IFoo" ) );
      TS_ASSERT( not prm.isInterface( "IBar" ) );
    }

    void testSuperInterfaceError2() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extends t_state { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|5 col 24| Error : Interface t_state not found";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
      TS_ASSERT( not prm.isInterface( "IFoo" ) );
      TS_ASSERT( not prm.isInterface( "IBar" ) );
    }

    void testSuperInterfaceError3() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extend IFoo { " << std::endl
            << "boolean unstate;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|4 col 16| Error : \"{\" expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)0 );
    }

    void testSuperInterfaceWithAttributeOverload() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state extends boolean" << std::endl;
      input << "OK:true, NOK:false;" << std::endl;
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extends IFoo { " << std::endl
            << "t_state state;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.interface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT_EQUALS( i_bar.super(), i_foo );
    }

    void testSuperInterfaceWithReferenceOverload() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IBar extends IFoo { " << std::endl
            << "}";
      input << "interface IPlop { " << std::endl
            << "IFoo myFoo;" << std::endl
            << "}";
      input << "interface IPloc extends IPlop { " << std::endl
            << "IBar myFoo; " << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)4 );
      TS_ASSERT( prm.isInterface( "IPlop" ) );
      const auto& i_plop = prm.interface( "IPlop" );
      TS_ASSERT_EQUALS( i_plop.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( i_plop.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT_THROWS( i_plop.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IPloc" ) );
      const auto& i_ploc = prm.interface( "IPloc" );
      TS_ASSERT_EQUALS( i_ploc.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( i_ploc.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_ploc.super(), i_plop );
    }

    void testOrderDoesNotMatter1() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { " << std::endl
            << "t_state state;" << std::endl
            << "}" << std::endl
            << "type t_state OK, NOK;";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
      TS_ASSERT( prm.isType( "t_state" ) );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testOrderDoesNotMatter2() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar extends IFoo { " << std::endl
            << "t_state state;" << std::endl
            << "}";
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "type t_state extends boolean" << std::endl
            << "OK:true, NOK:false;" << std::endl;
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.interface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT_EQUALS( i_bar.super(), i_foo );
    }

    void testOrderDoesNotMatter3() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { " << std::endl
            << "IFoo foo;" << std::endl
            << "boolean state;" << std::endl
            << "}";
      input << "interface IFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.interface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testOrderDoesNotMatter4() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IBar { " << std::endl
            << "IFoo foo;" << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "interface IFoo { " << std::endl
            << "IBar bar; " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT( prm.isInterface( "IFoo" ) );
      const auto& i_foo = prm.interface( "IFoo" );
      TS_ASSERT_EQUALS( i_foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_foo.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT_THROWS( i_foo.super(), gum::NotFound );
      TS_ASSERT( prm.isInterface( "IBar" ) );
      const auto& i_bar = prm.interface( "IBar" );
      TS_ASSERT_EQUALS( i_bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( i_bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT_THROWS( i_bar.super(), gum::NotFound );
    }

    void testInterfaceWithReferenceError() {
      // Arrange
      auto input = std::stringstream();
      input << "interface IFoo { " << std::endl
            << "IFoo foo;" << std::endl
            << "boolean state;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3PRMFactory<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|2 col 1| Error : Interface IFoo cannot reference "
             "itself";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)1 );
    }
  };

}  // namespace gum_tests
