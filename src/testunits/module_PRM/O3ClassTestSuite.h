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

    void testSimpleClass() {
      try {
        // Arrange
        auto input = std::stringstream();
        input << "class Bar { " << std::endl
              << "boolean state [0.2, 0.8];" << std::endl
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
        GUM_SHOWERROR( e );
      }
    }

  };

}  // namespace gum_tests
