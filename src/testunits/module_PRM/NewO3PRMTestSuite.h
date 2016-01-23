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

  class NewO3PRMTestSuite : public CxxTest::TestSuite {
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
      auto line = std::string();
      std::getline( output, line );
      auto msg = "|1 col 1| Syntax error : this symbol not expected in "
                 "type declaration";
      TS_ASSERT_EQUALS( line, msg);
      TS_ASSERT_EQUALS( prm.types().size(), 1 );
      TS_ASSERT( not prm.isType( "state" ) );
    }
  };

}  // namespace gum_tests
