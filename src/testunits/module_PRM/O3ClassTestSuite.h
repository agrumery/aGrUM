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
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

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
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testEmptyClassError1() {
      // Arrange
      auto input = std::stringstream();
      input << "clas Bar { }";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 1| Error : invalid declaration";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)0 );
      TS_ASSERT( ! prm.isClass( "Bar" ) );
    }

    void testEmptyClassError2() {
      // Arrange
      auto input = std::stringstream();
      input << "class { }";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 7| Error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)0 );
      TS_ASSERT(!prm.isClass( "Bar" ) );
    }

    void testEmptyClassError3() {
      // Arrange
      auto input = std::stringstream();
      input << "class ++++/ze { }";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 7| Error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)0 );
      TS_ASSERT(!prm.isClass( "Bar" ) );
    }

    void testSimpleClass1() {
      // Arrange
      auto input = std::stringstream();
      input << "class Bar { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "state" ) );
      const auto& state = bar.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      const auto& cpf = state.cpf();
      auto i = gum::Instantiation( cpf );
      i.setFirst();
      TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testSimpleClass2() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "state" ) );
      const auto& state = bar.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      const auto& cpf = state.cpf();
      auto i = gum::Instantiation( cpf );
      i.setFirst();
      TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testSimpleClass3() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,  // false" << std::endl
            << "   0.8, 0.6]}; // true" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "state" ) );
      const auto& state = bar.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( isWorking ) );
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(
          cpf.variablesSequence().exists( &( state.type().variable() ) ) );
      TS_ASSERT_EQUALS( cpf.domainSize(), (gum::Size)4 );
      auto i = gum::Instantiation( cpf );
      i.setFirst();
      TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.4, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.6, 1e-6 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testSimpleClassError1() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,  // false" << std::endl
            << "    0.2, 0.6]}; // true" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|4 col 9| Error : PRMAttribute Bar.isWorking CPT does not sum to 1, "
             "found 0.4";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
    }

    void testSimpleClassError2() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson stat " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,  // false" << std::endl
            << "    0.2, 0.6]}; // true" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|4 col 29| Error : Parent stat not found";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
    }

    void testSimpleClassError3() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state " << std::endl
            << "// OK | NOK " << std::endl
            << "  {[0.2, 0.4,   // false" << std::endl
            << "    0.2, 0.4,   // true" << std::endl
            << "    0.6, 0.2]}; // oops" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|4 col 9| Error : Illegal CPT size, expected 4 found 6 for "
             "attribute Bar.isWorking";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
    }

    void testSimpleRules1() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: 0.1, 0.9;" << std::endl
            << "NOK: 0.8, 0.2;" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "state" ) );
      const auto& state = bar.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( isWorking ) );
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(
          cpf.variablesSequence().exists( &( state.type().variable() ) ) );
      TS_ASSERT_EQUALS( cpf.domainSize(), (gum::Size)4 );
      auto i = gum::Instantiation( cpf );
      i.setFirst();
      TS_ASSERT_DELTA( cpf[i], 0.1, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.9, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testSimpleRules2() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: 0.1, 0.9;" << std::endl
            << "*: 0.8, 0.2;" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "state" ) );
      const auto& state = bar.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( isWorking ) );
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(
          cpf.variablesSequence().exists( &( state.type().variable() ) ) );
      TS_ASSERT_EQUALS( cpf.domainSize(), (gum::Size)4 );
      auto i = gum::Instantiation( cpf );
      i.setFirst();
      TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testSimpleRules3() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: \"10/100\", \"90/100\";" << std::endl
            << "NOK: \"80/100\", \"20/100\";" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "state" ) );
      const auto& state = bar.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( isWorking ) );
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(
          cpf.variablesSequence().exists( &( state.type().variable() ) ) );
      TS_ASSERT_EQUALS( cpf.domainSize(), (gum::Size)4 );
      auto i = gum::Instantiation( cpf );
      i.setFirst();
      TS_ASSERT_DELTA( cpf[i], 0.1, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.9, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testSimpleRules4() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: '10/100', '90/100';" << std::endl
            << "NOK: '80/100', '20/100';" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "state" ) );
      const auto& state = bar.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( isWorking ) );
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(
          cpf.variablesSequence().exists( &( state.type().variable() ) ) );
      TS_ASSERT_EQUALS( cpf.domainSize(), (gum::Size)4 );
      auto i = gum::Instantiation( cpf );
      i.setFirst();
      TS_ASSERT_DELTA( cpf[i], 0.1, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.9, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testSimpleRulesError1() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: '10/100', '110/100';" << std::endl
            << "NOK: '80/100', '20/100';" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|5 col 15| Error : Illegal CPT value \"110/100\" in attribute "
             "Bar.isWorking1.1";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
    }

    void testSimpleRulesError2() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK, FOO: '10/100', '110/100';" << std::endl
            << "NOK, BAR: '80/100', '20/100';" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|5 col 1| Error : Expected 1 value(s), found 2";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
    }

    void testSimpleRulesError3() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: '10/100', '90/100';" << std::endl
            << "NOK: 'FOO', '20/100';" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|6 col 6| Error : Illegal CPT value \"FOO\" in attribute "
             "Bar.isWorking\"FOO\"";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
    }

    void testSimpleParameter1() {
      // Arrange
      auto input = std::stringstream();
      input << "class Bar { " << std::endl
            << "real lambda default 0.003;" << std::endl
            << "int t default 8760;" << std::endl
            << "boolean state { " << std::endl
            << "[ 'exp(-lambda*t)', '1-exp(-lambda*t)' ]" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "state" ) );
      const auto& state = bar.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      const auto& cpf = state.cpf();
      TS_ASSERT(
          cpf.variablesSequence().exists( &( state.type().variable() ) ) );
      TS_ASSERT_EQUALS( cpf.domainSize(), (gum::Size)2 );
      auto i = gum::Instantiation( cpf );
      i.setFirst();
      TS_ASSERT_DELTA( cpf[i], std::exp( -0.003 * 8760 ), 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 1 - std::exp( -0.003 * 8760 ), 1e-6 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testSimpleParameter2() {
      // Arrange
      auto input = std::stringstream();
      input << "type t_state OK, NOK;" << std::endl;
      input << "class Bar { " << std::endl
            << "real lambda default 0.003;" << std::endl
            << "int t default 8760;" << std::endl
            << "t_state state {[0.2, 0.8]};" << std::endl
            << "boolean isWorking dependson state {" << std::endl
            << "OK: 'exp(-lambda*t)', '1-exp(-lambda*t)';" << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "state" ) );
      const auto& state = bar.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( isWorking ) );
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(
          cpf.variablesSequence().exists( &( state.type().variable() ) ) );
      TS_ASSERT_EQUALS( cpf.domainSize(), (gum::Size)4 );
      auto i = gum::Instantiation( cpf );
      i.setFirst();
      TS_ASSERT_DELTA( cpf[i], std::exp( -0.003 * 8760 ), 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 1 - std::exp( -0.003 * 8760 ), 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 1.0, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.0, 1e-6 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testClassWithReference1() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl << "Foo myFoo;" << std::endl << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoo" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoo" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoo" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &( foo ) );
      TS_ASSERT(!myFoo.isArray() );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testClassWithReference2() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoo;" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoo" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoo" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoo" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &( foo ) );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testClassWithReference3() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "real lambda default 0.003;" << std::endl
            << "int t default 8760;" << std::endl
            << "Foo myFoo;" << std::endl
            << "boolean isWorking dependson myFoo.state {" << std::endl
            << "OK: 'exp(-lambda*t)', '1-exp(-lambda*t)';" << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoo" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoo" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoo" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT(!myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( isWorking ) );
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(
         !cpf.variablesSequence().exists( &( state.type().variable() ) ) );
      TS_ASSERT( bar.exists( "myFoo.state" ) );
      const auto& chain = bar.get( "myFoo.state" );
      TS_ASSERT(
          cpf.variablesSequence().exists( &( chain.type().variable() ) ) );
      TS_ASSERT_EQUALS( cpf.domainSize(), (gum::Size)4 );
      auto i = gum::Instantiation( cpf );
      i.setFirst();
      TS_ASSERT_DELTA( cpf[i], std::exp( -0.003 * 8760 ), 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 1 - std::exp( -0.003 * 8760 ), 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 1.0, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.0, 1e-6 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testClassWithReference4() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK;" << std::endl;
      input << "class Foo { " << std::endl
            << "int t default 8760;" << std::endl
            << "state state {[ 0.2, 0.8]};" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "real lambda default 0.003;" << std::endl
            << "Foo myFoo;" << std::endl
            << "boolean isWorking dependson myFoo.state {" << std::endl
            << "OK: 'exp(-lambda*myFoo.t)', '1-exp(-lambda*myFoo.t)';"
            << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)2 );
      TS_ASSERT( prm.isClass( "Foo" ) );
      const auto& foo = prm.getClass( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoo" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoo" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoo" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT(!myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( isWorking ) );
      const auto& cpf = isWorking.cpf();
      TS_ASSERT(
         !cpf.variablesSequence().exists( &( state.type().variable() ) ) );
      TS_ASSERT( bar.exists( "myFoo.state" ) );
      const auto& chain = bar.get( "myFoo.state" );
      TS_ASSERT(
          cpf.variablesSequence().exists( &( chain.type().variable() ) ) );
      TS_ASSERT_EQUALS( cpf.domainSize(), (gum::Size)4 );
      auto i = gum::Instantiation( cpf );
      i.setFirst();
      TS_ASSERT_DELTA( cpf[i], std::exp( -0.003 * 8760 ), 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 1 - std::exp( -0.003 * 8760 ), 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 1.0, 1e-6 );
      i.inc();
      TS_ASSERT_DELTA( cpf[i], 0.0, 1e-6 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testClassWithReferenceError1() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "real lambda default 0.003;" << std::endl
            << "int t default 8760;" << std::endl
            << "Foo myFoo;" << std::endl
            << "boolean isWorking dependson myfoo.state {" << std::endl
            << "OK: 'exp(-lambda*t)', '1-exp(-lambda*t)';" << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|9 col 29| Error : Link myfoo in chain myfoo.state "
             "not found";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
    }

    void testClassWithReferenceError2() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "real lambda default 0.003;" << std::endl
            << "int t default 8760;" << std::endl
            << "Foo myFoo;" << std::endl
            << "boolean isWorking dependson myFoo.stat {" << std::endl
            << "OK: 'exp(-lambda*t)', '1-exp(-lambda*t)';" << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|9 col 29| Error : Link stat in chain myFoo.stat not "
             "found";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
    }

    void testClassWithReferenceError3() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "real lambda default 0.003;" << std::endl
            << "int t default 8760;" << std::endl
            << "boolean isWorking dependson myFoo.state {" << std::endl
            << "OK: 'exp(-lambda*t)', '1-exp(-lambda*t)';" << std::endl
            << "NOK: '1.0', '0.0';" << std::endl
            << "};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|8 col 29| Error : Link myFoo in chain myFoo.state "
             "not found";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
    }

    void testClassWithReferenceError4() {
      // Arrange
      auto input = std::stringstream();
      input << "class Bar extends Foo { " << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};"
            << std::endl
            << "state state {[0.5, 0.5]};" << std::endl
            << "}" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "Bar myBar;" << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}" << std::endl;
      input << "type state extends boolean OK: true, NOK:false;" << std::endl;
      input << "interface iFoo { " << std::endl
            << "Bar myBar;" << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|6 col 1| Error : Class Foo cannot reference "
             "subclass Bar";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)2 );
    }

    void testClassWithReferenceError6() {
      // Arrange
      auto input = std::stringstream();
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
      input << "type state extends boolean OK: true, NOK:false;" << std::endl;
      input << "interface iFoo { " << std::endl
            << "Bar myBar;" << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|7 col 1| Error : Class Foo cannot reference "
             "subclass Bar";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testOrAggregate() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or(myFoos.state);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::OR );
    }

    void testOrAggregateArray() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = or([myFoos.state, myBoos.state]);"
            << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::OR );
    }

    void testAndAggregate() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = and(myFoos.state);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::AND );
    }

    void testAndAggregateArray() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = and([myFoos.state, myBoos.state]);"
            << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::AND );
    }

    void testForAllAggregate() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = forall(myFoos.state, OK);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::FORALL );
    }

    void testForAllAggregateArray() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = forall([myFoos.state, myBoos.state], OK);"
            << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::FORALL );
    }

    void testExistsAggregate() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = exists(myFoos.state, OK);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::EXISTS );
    }

    void testExistsAggregateArray() {
      // Arrange
      auto input = std::stringstream();
      input << "type state OK, NOK;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "boolean isWorking = exists([myFoos.state, myBoos.state], OK);"
            << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::EXISTS );
    }

    void testMinAggregate() {
      // Arrange
      auto input = std::stringstream();
      input << "int(0,10) state;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = min(myFoos.state);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::MIN );
    }

    void testMinAggregateArray() {
      // Arrange
      auto input = std::stringstream();
      input << "int(0,10) state;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = min([myFoos.state, myBoos.state]);"
            << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::MIN );
    }

    void testMaxAggregate() {
      // Arrange
      auto input = std::stringstream();
      input << "int(0,10) state;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = max(myFoos.state);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::MAX );
    }

    void testMaxAggregateArray() {
      // Arrange
      auto input = std::stringstream();
      input << "int(0,10) state;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = max([myFoos.state, myBoos.state]);"
            << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::MAX );
    }

    void testMedianAggregate() {
      // Arrange
      auto input = std::stringstream();
      input << "int(0,10) state;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = median(myFoos.state);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::MEDIAN );
    }

    void testMedianAggregateArray() {
      // Arrange
      auto input = std::stringstream();
      input << "int(0,10) state;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = median([myFoos.state, myBoos.state]);"
            << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::MEDIAN );
    }

    void testAmplitudeAggregate() {
      // Arrange
      auto input = std::stringstream();
      input << "int(0,10) state;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = amplitude(myFoos.state);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::AMPLITUDE );
    }

    void testAmplitudeAggregateArray() {
      // Arrange
      auto input = std::stringstream();
      input << "int(0,10) state;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = amplitude([myFoos.state, myBoos.state]);"
            << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::AMPLITUDE );
    }

    void testCountAggregate() {
      // Arrange
      auto input = std::stringstream();
      input << "int(0,10) state;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = count(myFoos.state, 5);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::COUNT );
    }

    void testCountAggregateArray() {
      // Arrange
      auto input = std::stringstream();
      input << "int(0,10) state;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Foo[] myBoos;" << std::endl
            << "state isWorking = count([myFoos.state, myBoos.state], 5);"
            << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      const auto& state = foo.get( "state" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAttribute( state ) );
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.aggregates().size(), (gum::Size)1 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "myFoos" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "myFoos" ) ) );
      const auto& myFoo = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "myFoos" ) );
      TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
      TS_ASSERT( myFoo.isArray() );
      TS_ASSERT( bar.exists( "isWorking" ) );
      const auto& isWorking = bar.get( "isWorking" );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isAggregate( isWorking ) );
      const auto& agg =
          static_cast<const gum::prm::PRMAggregate<double>&>( isWorking );
      TS_ASSERT_EQUALS( agg.agg_type(),
                        gum::prm::PRMAggregate<double>::AggregateType::COUNT );
    }

    void testOrAggregateWithErrors1() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or(myBoos.state);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|6 col 24| Error : Link myBoos in chain myBoos.state "
             "not found";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testOrAggregateWithErrors2() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or(myFoos.st);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|6 col 24| Error : Link st in chain myFoos.st not "
             "found";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testOrAggregateWithErrors3() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "boolean isWorking = or();" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|6 col 24| Error : invalid declaration";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testOrAggregateWithErrors4() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "type state OK, NOK;" << std::endl;
      input << "interface Boo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "Boo[] myBoos;" << std::endl
            << "boolean isWorking = or([myFoos.state, myBoos.state]);"
            << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|11 col 39| Error : Expected type boolean for parent "
             "myBoos.state, found state";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testCountAggregateWithErrors5() {
      // Arrange
      auto input = std::stringstream();
      input << "int(0,10) state;" << std::endl;
      input << "interface Foo { " << std::endl
            << "state state;" << std::endl
            << "}" << std::endl;
      input << "class Bar { " << std::endl
            << "Foo[] myFoos;" << std::endl
            << "state isWorking = count(myFoos.state, foobar);" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|7 col 39| Error : Parameter foobar in aggregate "
             "isWorking does not match any expected values";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testInheritance1() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar implements Foo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( "", output.str() );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT( prm.isClass( "Bar" ) );
      TS_ASSERT_EQUALS( foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT( foo.isSuperTypeOf( bar ) );
      TS_ASSERT( bar.implements().contains(
          const_cast<gum::prm::PRMInterface<double>*>( &foo ) ) );
      TS_ASSERT_EQUALS( bar.parameters().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.slotChains().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)1 );
    }

    void testInheritance2() {
      // Arrange
      auto input = std::stringstream();
      input << "interface Foo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Bar implements Foo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( "", output.str() );
      TS_ASSERT( prm.isInterface( "Foo" ) );
      const auto& foo = prm.getInterface( "Foo" );
      TS_ASSERT( prm.isClass( "Bar" ) );
      TS_ASSERT_EQUALS( foo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT( foo.isSuperTypeOf( bar ) );
      TS_ASSERT( bar.implements().contains(
          const_cast<gum::prm::PRMInterface<double>*>( &foo ) ) );
      TS_ASSERT_EQUALS( bar.parameters().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.slotChains().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)1 );
    }

    void testInheritance3() {
      // Arrange
      auto input = std::stringstream();
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      input << "type state extends boolean OK: true, NOK:false;" << std::endl;
      input << "class Bar extends Foo { " << std::endl
            << "state state {[0.5, 0.5]};" << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};"
            << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( "", output.str() );
      TS_ASSERT( prm.isInterface( "iFoo" ) );
      const auto& ifoo = prm.getInterface( "iFoo" );
      TS_ASSERT_EQUALS( ifoo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( ifoo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isClass( "Foo" ) );
      const auto& foo = prm.getClass( "Foo" );
      TS_ASSERT( ifoo.isSuperTypeOf( foo ) );
      TS_ASSERT( foo.implements().contains(
          const_cast<gum::prm::PRMInterface<double>*>( &ifoo ) ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT( ifoo.isSuperTypeOf( bar ) );
      TS_ASSERT( foo.isSuperTypeOf( bar ) );
      TS_ASSERT_EQUALS( &( bar.super() ), &( foo ) );
      TS_ASSERT( bar.implements().contains(
          const_cast<gum::prm::PRMInterface<double>*>( &ifoo ) ) );
    }

    void testInheritanceError1() {
      // Arrange
      auto input = std::stringstream();
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Foo implements Bar { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|4 col 22| Error : Interface Bar not found";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testInheritanceError2() {
      // Arrange
      auto input = std::stringstream();
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "boolean stat {[0.2, 0.8]};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|4 col 7| Error : Class Foo does not implement all of "
             "interface iFoo attributes";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testInheritanceError3() {
      // Arrange
      auto input = std::stringstream();
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      input << "type state OK, NOK;" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "state state {[0.2, 0.8]};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|6 col 7| Error : Class Foo attribute state does not "
             "respect interface iFoo";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testInheritanceError4() {
      // Arrange
      auto input = std::stringstream();
      input << "class Foo { " << std::endl
            << "boolean state {[0.3, 0.7]};" << std::endl
            << "}" << std::endl;
      input << "class Bar extends oo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|4 col 19| Error : Unknown class oo";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testInheritanceError5() {
      // Arrange
      auto input = std::stringstream();
      input << "class Foo { " << std::endl
            << "boolean state {[0.3, 0.7]};" << std::endl
            << "}" << std::endl;
      input << "type state OK, NOK;" << std::endl;
      input << "class Bar extends Foo { " << std::endl
            << "state state {[0.2, 0.8]};" << std::endl
            << "}";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|6 col 7| Error : Illegal overload of element state from "
             "class Foo";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

    void testOrderDoesNotMatter1() {
      // Arrange
      auto input = std::stringstream();
      input << "class Bar extends Foo { " << std::endl
            << "state state {[0.5, 0.5]};" << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};"
            << std::endl
            << "}" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}" << std::endl;
      input << "type state extends boolean OK: true, NOK:false;" << std::endl;
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( "", output.str() );
      TS_ASSERT( prm.isInterface( "iFoo" ) );
      const auto& ifoo = prm.getInterface( "iFoo" );
      TS_ASSERT_EQUALS( ifoo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( ifoo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isClass( "Foo" ) );
      const auto& foo = prm.getClass( "Foo" );
      TS_ASSERT( ifoo.isSuperTypeOf( foo ) );
      TS_ASSERT( foo.implements().contains(
          const_cast<gum::prm::PRMInterface<double>*>( &ifoo ) ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT( ifoo.isSuperTypeOf( bar ) );
      TS_ASSERT( foo.isSuperTypeOf( bar ) );
      TS_ASSERT_EQUALS( &( bar.super() ), &( foo ) );
      TS_ASSERT( bar.implements().contains(
          const_cast<gum::prm::PRMInterface<double>*>( &ifoo ) ) );
    }

    void testOrderDoesNotMatter2() {
      // Arrange
      auto input = std::stringstream();
      input << "class Bar extends Foo { " << std::endl
            << "boolean isWorking dependson state {[0.5, 0.5, 0.5, 0.5]};"
            << std::endl
            << "state state {[0.5, 0.5]};" << std::endl
            << "}" << std::endl;
      input << "class Foo implements iFoo { " << std::endl
            << "boolean state {[0.2, 0.8]};" << std::endl
            << "}" << std::endl;
      input << "type state extends boolean OK: true, NOK:false;" << std::endl;
      input << "interface iFoo { " << std::endl
            << "boolean state;" << std::endl
            << "}" << std::endl;
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( "", output.str() );
      TS_ASSERT( prm.isInterface( "iFoo" ) );
      const auto& ifoo = prm.getInterface( "iFoo" );
      TS_ASSERT_EQUALS( ifoo.referenceSlots().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( ifoo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( prm.isClass( "Foo" ) );
      const auto& foo = prm.getClass( "Foo" );
      TS_ASSERT( ifoo.isSuperTypeOf( foo ) );
      TS_ASSERT( foo.implements().contains(
          const_cast<gum::prm::PRMInterface<double>*>( &ifoo ) ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT( ifoo.isSuperTypeOf( bar ) );
      TS_ASSERT( foo.isSuperTypeOf( bar ) );
      TS_ASSERT_EQUALS( &( bar.super() ), &( foo ) );
      TS_ASSERT( bar.implements().contains(
          const_cast<gum::prm::PRMInterface<double>*>( &ifoo ) ) );
    }

    void testPrinters() {
      // Arrange
      auto path = GET_RESSOURCES_PATH( "o3prm/printers.o3prm" );
      gum::prm::PRM<double> prm;
      auto input = std::ifstream( path );
      auto output = std::stringstream();
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( "", output.str() );
    }

    void testClassWithOverloading() {
      // Arrange
      auto input = std::stringstream();
      input << "type state extends boolean OK: true, NOK: false;";
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
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      TS_ASSERT_EQUALS( output.str(), "" );
      TS_ASSERT_EQUALS( prm.interfaces().size(), (gum::Size)2 );
      TS_ASSERT_EQUALS( prm.classes().size(), (gum::Size)2 );
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

      TS_ASSERT( prm.isClass( "Foo" ) );
      const auto& foo = prm.getClass( "Foo" );
      TS_ASSERT_EQUALS( foo.attributes().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "state" ) );
      TS_ASSERT( foo.exists( "(boolean)state" ) );
      TS_ASSERT(
          gum::prm::PRMClassElement<double>::isAttribute( foo.get( "state" ) ) );
      TS_ASSERT(
          gum::prm::PRMClassElement<double>::isAttribute( foo.get( "(boolean)state" ) ) );
      TS_ASSERT_EQUALS( &( foo.get( "state" ) ),
                        &( foo.get( "(boolean)state" ) ) );
      TS_ASSERT_EQUALS( foo.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( foo.exists( "plop" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          foo.get( "plop" ) ) );
      const auto& ref_plop = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          foo.get( "plop" ) );
      TS_ASSERT_EQUALS( &( ref_plop.slotType() ), &plop );

      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), (gum::Size)2 );
      TS_ASSERT( bar.exists( "state" ) );
      TS_ASSERT( bar.exists( "(boolean)state" ) );
      TS_ASSERT( bar.exists( "(state)state" ) );
      TS_ASSERT(
          gum::prm::PRMClassElement<double>::isAttribute( bar.get( "state" ) ) );
      TS_ASSERT(
          gum::prm::PRMClassElement<double>::isAttribute( bar.get( "(boolean)state" ) ) );
      TS_ASSERT(
          gum::prm::PRMClassElement<double>::isAttribute( bar.get( "(state)state" ) ) );
      TS_ASSERT_EQUALS( &( bar.get( "state" ) ),
                        &( bar.get( "(state)state" ) ) );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), (gum::Size)1 );
      TS_ASSERT( bar.exists( "plop" ) );
      TS_ASSERT( gum::prm::PRMClassElement<double>::isReferenceSlot(
          bar.get( "plop" ) ) );
      const auto& ref_subplop = static_cast<const gum::prm::PRMReferenceSlot<double>&>(
          bar.get( "plop" ) );
      TS_ASSERT_EQUALS( &( ref_subplop.slotType() ), &sub_plop );
    }

    void testClassWithOverloadingError1() {
      // Arrange
      auto input = std::stringstream();
      input << "type state extends boolean OK: true, NOK: false;";
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
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      auto factory = gum::prm::o3prm::O3prmReader<double>( prm );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( factory.parseStream( input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|5 col 33| Error : Illegal overload of element plop from class Bar";
      TS_ASSERT_EQUALS( line, msg.str() );
    }

  };

}  // namespace gum_tests
