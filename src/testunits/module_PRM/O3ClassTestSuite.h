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
      TS_ASSERT( prm.isClass( "Bar" ) );
      const auto& bar = prm.getClass( "Bar" );
      TS_ASSERT_EQUALS( bar.attributes().size(), 0 );
      TS_ASSERT_EQUALS( bar.referenceSlots().size(), 0 );
      TS_ASSERT_THROWS( bar.super(), gum::NotFound );
    }

    void testEmptyClassError1() {
      // Arrange
      auto input = std::stringstream();
      input << "clas Bar { }";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 1| Syntax error : invalid declaration";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), 0 );
      TS_ASSERT( not prm.isClass( "Bar" ) );
    }

    void testEmptyClassError2() {
      // Arrange
      auto input = std::stringstream();
      input << "class { }";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 7| Syntax error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), 0 );
      TS_ASSERT( not prm.isClass( "Bar" ) );
    }

    void testEmptyClassError3() {
      // Arrange
      auto input = std::stringstream();
      input << "class ++++/ze { }";
      auto output = std::stringstream();
      gum::prm::PRM<double> prm;
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::prm::o3prm::parse_stream( prm, input, output ) );
      // Assert
      std::string line;
      std::getline( output, line );
      auto msg = std::stringstream();
      msg << "|1 col 7| Syntax error : label expected";
      TS_ASSERT_EQUALS( line, msg.str() );
      TS_ASSERT_EQUALS( prm.classes().size(), 0 );
      TS_ASSERT( not prm.isClass( "Bar" ) );
    }

    void testSimpleClass1() {
      try {
        // Arrange
        auto input = std::stringstream();
        input << "class Bar { " << std::endl
              << "boolean state {[0.2, 0.8]};" << std::endl
              << "}";
        auto output = std::stringstream();
        gum::prm::PRM<double> prm;
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        TS_ASSERT( prm.isClass( "Bar" ) );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 1 );
        TS_ASSERT( bar.exists( "state" ) );
        const auto& state = bar.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        const auto& cpf = state.cpf();
        auto i = gum::Instantiation( cpf );
        i.setFirst();
        TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 0 );
        TS_ASSERT_THROWS( bar.super(), gum::NotFound );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleClass2() {
      try {
        // Arrange
        auto input = std::stringstream();
        input << "type t_state OK, NOK;" << std::endl;
        input << "class Bar { " << std::endl
              << "t_state state {[0.2, 0.8]};" << std::endl
              << "}";
        auto output = std::stringstream();
        gum::prm::PRM<double> prm;
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        TS_ASSERT( prm.isClass( "Bar" ) );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 1 );
        TS_ASSERT( bar.exists( "state" ) );
        const auto& state = bar.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        const auto& cpf = state.cpf();
        auto i = gum::Instantiation( cpf );
        i.setFirst();
        TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 0 );
        TS_ASSERT_THROWS( bar.super(), gum::NotFound );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleClass3() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        TS_ASSERT( prm.isClass( "Bar" ) );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 2 );
        TS_ASSERT( bar.exists( "state" ) );
        const auto& state = bar.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        TS_ASSERT( bar.exists( "isWorking" ) );
        const auto& isWorking = bar.get( "isWorking" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( isWorking ) );
        const auto& cpf = isWorking.cpf();
        TS_ASSERT(
            cpf.variablesSequence().exists( &( state.type().variable() ) ) );
        TS_ASSERT_EQUALS( cpf.domainSize(), 4 );
        auto i = gum::Instantiation( cpf );
        i.setFirst();
        TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.4, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.6, 1e-6 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 0 );
        TS_ASSERT_THROWS( bar.super(), gum::NotFound );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleClassError1() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        std::string line;
        std::getline( output, line );
        auto msg = std::stringstream();
        msg << "|4 col 9| Attribute error : CPT does not sum to 1";
        TS_ASSERT_EQUALS( line, msg.str() );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleClassError2() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        std::string line;
        std::getline( output, line );
        auto msg = std::stringstream();
        msg << "|4 col 29| Attribute error : Parent stat not found";
        TS_ASSERT_EQUALS( line, msg.str() );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleClassError3() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        std::string line;
        std::getline( output, line );
        auto msg = std::stringstream();
        msg << "|4 col 9| Attribute error : Illegal CPT size, expected 4 found "
               "6";
        TS_ASSERT_EQUALS( line, msg.str() );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleRules1() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        TS_ASSERT( prm.isClass( "Bar" ) );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 2 );
        TS_ASSERT( bar.exists( "state" ) );
        const auto& state = bar.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        TS_ASSERT( bar.exists( "isWorking" ) );
        const auto& isWorking = bar.get( "isWorking" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( isWorking ) );
        const auto& cpf = isWorking.cpf();
        TS_ASSERT(
            cpf.variablesSequence().exists( &( state.type().variable() ) ) );
        TS_ASSERT_EQUALS( cpf.domainSize(), 4 );
        auto i = gum::Instantiation( cpf );
        i.setFirst();
        TS_ASSERT_DELTA( cpf[i], 0.1, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.9, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 0 );
        TS_ASSERT_THROWS( bar.super(), gum::NotFound );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleRules2() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        TS_ASSERT( prm.isClass( "Bar" ) );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 2 );
        TS_ASSERT( bar.exists( "state" ) );
        const auto& state = bar.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        TS_ASSERT( bar.exists( "isWorking" ) );
        const auto& isWorking = bar.get( "isWorking" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( isWorking ) );
        const auto& cpf = isWorking.cpf();
        TS_ASSERT(
            cpf.variablesSequence().exists( &( state.type().variable() ) ) );
        TS_ASSERT_EQUALS( cpf.domainSize(), 4 );
        auto i = gum::Instantiation( cpf );
        i.setFirst();
        TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 0 );
        TS_ASSERT_THROWS( bar.super(), gum::NotFound );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleRules3() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        TS_ASSERT( prm.isClass( "Bar" ) );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 2 );
        TS_ASSERT( bar.exists( "state" ) );
        const auto& state = bar.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        TS_ASSERT( bar.exists( "isWorking" ) );
        const auto& isWorking = bar.get( "isWorking" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( isWorking ) );
        const auto& cpf = isWorking.cpf();
        TS_ASSERT(
            cpf.variablesSequence().exists( &( state.type().variable() ) ) );
        TS_ASSERT_EQUALS( cpf.domainSize(), 4 );
        auto i = gum::Instantiation( cpf );
        i.setFirst();
        TS_ASSERT_DELTA( cpf[i], 0.1, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.9, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 0 );
        TS_ASSERT_THROWS( bar.super(), gum::NotFound );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleRules4() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        TS_ASSERT( prm.isClass( "Bar" ) );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 2 );
        TS_ASSERT( bar.exists( "state" ) );
        const auto& state = bar.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        TS_ASSERT( bar.exists( "isWorking" ) );
        const auto& isWorking = bar.get( "isWorking" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( isWorking ) );
        const auto& cpf = isWorking.cpf();
        TS_ASSERT(
            cpf.variablesSequence().exists( &( state.type().variable() ) ) );
        TS_ASSERT_EQUALS( cpf.domainSize(), 4 );
        auto i = gum::Instantiation( cpf );
        i.setFirst();
        TS_ASSERT_DELTA( cpf[i], 0.1, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.9, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.8, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.2, 1e-6 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 0 );
        TS_ASSERT_THROWS( bar.super(), gum::NotFound );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleRulesError1() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        std::string line;
        std::getline( output, line );
        auto msg = std::stringstream();
        msg << "|5| Attribute error : CPT does not sum to 1";
        TS_ASSERT_EQUALS( line, msg.str() );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleRulesError2() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        std::string line;
        std::getline( output, line );
        auto msg = std::stringstream();
        msg << "|5| Attribute error : Expected 1 value(s), found 2";
        TS_ASSERT_EQUALS( line, msg.str() );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleRulesError3() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        std::string line;
        std::getline( output, line );
        auto msg = std::stringstream();
        msg << "|6| Attribute error : Unknown value in CPT: \"FOO\"";
        TS_ASSERT_EQUALS( line, msg.str() );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleParameter1() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 1 );
        TS_ASSERT( bar.exists( "state" ) );
        const auto& state = bar.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        const auto& cpf = state.cpf();
        TS_ASSERT(
            cpf.variablesSequence().exists( &( state.type().variable() ) ) );
        TS_ASSERT_EQUALS( cpf.domainSize(), 2 );
        auto i = gum::Instantiation( cpf );
        i.setFirst();
        TS_ASSERT_DELTA( cpf[i], std::exp( -0.003 * 8760 ), 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 1 - std::exp( -0.003 * 8760 ), 1e-6 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 0 );
        TS_ASSERT_THROWS( bar.super(), gum::NotFound );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testSimpleParameter2() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 2 );
        TS_ASSERT( bar.exists( "state" ) );
        const auto& state = bar.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        TS_ASSERT( bar.exists( "isWorking" ) );
        const auto& isWorking = bar.get( "isWorking" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( isWorking ) );
        const auto& cpf = isWorking.cpf();
        TS_ASSERT(
            cpf.variablesSequence().exists( &( state.type().variable() ) ) );
        TS_ASSERT_EQUALS( cpf.domainSize(), 4 );
        auto i = gum::Instantiation( cpf );
        i.setFirst();
        TS_ASSERT_DELTA( cpf[i], std::exp( -0.003 * 8760 ), 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 1 - std::exp( -0.003 * 8760 ), 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 1.0, 1e-6 );
        i.inc();
        TS_ASSERT_DELTA( cpf[i], 0.0, 1e-6 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 0 );
        TS_ASSERT_THROWS( bar.super(), gum::NotFound );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testClassWithReference1() {
      try {
        // Arrange
        auto input = std::stringstream();
        input << "interface Foo { " << std::endl
              << "boolean state;" << std::endl
              << "}" << std::endl;
        input << "class Bar { " << std::endl
              << "Foo myFoo;" << std::endl
              << "}";
        auto output = std::stringstream();
        gum::prm::PRM<double> prm;
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        TS_ASSERT( prm.isInterface( "Foo" ) );
        const auto& foo = prm.interface( "Foo" );
        TS_ASSERT_EQUALS( foo.attributes().size(), 1 );
        TS_ASSERT( foo.exists( "state" ) );
        const auto& state = foo.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        TS_ASSERT( prm.isClass( "Bar" ) );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 0 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 1 );
        TS_ASSERT( bar.exists( "myFoo" ) );
        TS_ASSERT( gum::prm::ClassElement<double>::isReferenceSlot(
            bar.get( "myFoo" ) ) );
        const auto& myFoo = static_cast<const gum::prm::ReferenceSlot<double>&>(
            bar.get( "myFoo" ) );
        TS_ASSERT_EQUALS( &( myFoo.slotType() ), &( foo ) );
        TS_ASSERT( not myFoo.isArray() );
        TS_ASSERT_THROWS( bar.super(), gum::NotFound );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testClassWithReference2() {
      try {
        // Arrange
        auto input = std::stringstream();
        input << "interface Foo { " << std::endl
              << "boolean state;" << std::endl
              << "}" << std::endl;
        input << "class Bar { " << std::endl
              << "Foo myFoo[];" << std::endl
              << "}";
        auto output = std::stringstream();
        gum::prm::PRM<double> prm;
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        TS_ASSERT( prm.isInterface( "Foo" ) );
        const auto& foo = prm.interface( "Foo" );
        TS_ASSERT_EQUALS( foo.attributes().size(), 1 );
        TS_ASSERT( foo.exists( "state" ) );
        const auto& state = foo.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        TS_ASSERT( prm.isClass( "Bar" ) );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 0 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 1 );
        TS_ASSERT( bar.exists( "myFoo" ) );
        TS_ASSERT( gum::prm::ClassElement<double>::isReferenceSlot(
            bar.get( "myFoo" ) ) );
        const auto& myFoo = static_cast<const gum::prm::ReferenceSlot<double>&>(
            bar.get( "myFoo" ) );
        TS_ASSERT_EQUALS( &( myFoo.slotType() ), &( foo ) );
        TS_ASSERT( myFoo.isArray() );
        TS_ASSERT_THROWS( bar.super(), gum::NotFound );
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }

    void testClassWithReference3() {
      try {
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
        // Act
        TS_GUM_ASSERT_THROWS_NOTHING(
            gum::prm::o3prm::parse_stream( prm, input, output ) );
        // Assert
        TS_ASSERT_EQUALS( output.str(), "" );
        TS_ASSERT_EQUALS( prm.classes().size(), 1 );
        TS_ASSERT( prm.isInterface( "Foo" ) );
        const auto& foo = prm.interface( "Foo" );
        TS_ASSERT_EQUALS( foo.attributes().size(), 1 );
        TS_ASSERT( foo.exists( "state" ) );
        const auto& state = foo.get( "state" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( state ) );
        TS_ASSERT( prm.isClass( "Bar" ) );
        const auto& bar = prm.getClass( "Bar" );
        TS_ASSERT_EQUALS( bar.attributes().size(), 1 );
        TS_ASSERT_EQUALS( bar.referenceSlots().size(), 1 );
        TS_ASSERT( bar.exists( "myFoo" ) );
        TS_ASSERT( gum::prm::ClassElement<double>::isReferenceSlot(
            bar.get( "myFoo" ) ) );
        const auto& myFoo = static_cast<const gum::prm::ReferenceSlot<double>&>(
            bar.get( "myFoo" ) );
        TS_ASSERT_EQUALS( &( myFoo.slotType() ), &foo );
        TS_ASSERT( not myFoo.isArray() );
        TS_ASSERT( bar.exists( "isWorking" ) );
        const auto& isWorking = bar.get( "isWorking" );
        TS_ASSERT( gum::prm::ClassElement<double>::isAttribute( isWorking ) );
        const auto& cpf = isWorking.cpf();
        TS_ASSERT( not cpf.variablesSequence().exists(
            &( state.type().variable() ) ) );
        TS_ASSERT( bar.exists( "myFoo.state" ) );
        const auto& chain = bar.get( "myFoo.state" );
        TS_ASSERT(
            cpf.variablesSequence().exists( &( chain.type().variable() ) ) );
        TS_ASSERT_EQUALS( cpf.domainSize(), 4 );
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
      } catch ( gum::Exception& e ) {
        GUM_TRACE_NEWLINE;
        GUM_SHOWERROR( e );
      }
    }
  };

}  // namespace gum_tests
