/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/PRM/elements/type.h>

#include <module_PRM/ClassElementTestSuite.h>

/**
 * This class is used to test gum::prm::ClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of 
 * gum::prm::ClassElement.
 */
namespace gum_tests {

  class PRMTypeTestSuite : public CxxTest::TestSuite {
    typedef gum::prm::Type<double> Type;

    gum::LabelizedVariable *__boolean;
    gum::LabelizedVariable *__state;

    public:

    void setUp() {
      __boolean = new gum::LabelizedVariable { "boolean", "Boolean variable", 0 };
      __boolean->addLabel( "false" );
      __boolean->addLabel( "true" );

      __state = new gum::LabelizedVariable { "state", "State variable", 0 };
      __state->addLabel( "OK" );
      __state->addLabel( "NOK" );
    }

    void tearDown() {
      delete __boolean;
      delete __state;
    }

    void testStaticBoolean() {
      // Arrange
      std::string labels[] = { "false", "true" };
      std::string name = "boolean";
      std::string decription = "Boolean variable";
      // Act
      auto boolean = Type::boolean();
      // Assert
      TS_ASSERT_EQUALS(boolean->variable().domainSize(), (gum::Size) 2);
      TS_ASSERT_EQUALS(boolean->variable().label(0), labels[0]);
      TS_ASSERT_EQUALS(boolean->variable().label(1), labels[1]);
      delete boolean;
    }

    void testConstructor() {
      // Arrange
      Type *type = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( type = new Type { *__boolean } );
      // Assert
      TS_ASSERT_THROWS_NOTHING( delete type );
    }

    void testSuperConstructor() {
      // Arrange
      Type boolean { *__boolean };
      Type *state = nullptr;
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      // Act
      try {
        state = new Type { boolean, map, *__state };
      } catch (...) {
        // TS_ASSERT_THROWS_NOTHING does not work here
        TS_FAIL( "Exception thrown" );
      }
      // Assert
      TS_ASSERT_THROWS_NOTHING( delete state );
    }

    void testCopyConstructor() {
      // Arrange
      Type boolean { *__boolean };
      Type *copy = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( copy = new Type { boolean } );
      // Assert
      TS_ASSERT_THROWS_NOTHING( delete copy );
    }

    void testGetVariable() {
      // Arrange
      Type boolean { *__boolean };
      gum::DiscreteVariable *variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( variable = &( boolean.variable() ) );
      // Assert
      TS_ASSERT_EQUALS( variable->name(), __boolean->name() );
      TS_ASSERT_EQUALS( variable->description(), __boolean->description() );
      TS_ASSERT_EQUALS( variable->label(0), __boolean->label(0) );
      TS_ASSERT_EQUALS( variable->label(1), __boolean->label(1) );
      TS_ASSERT_EQUALS( variable->domainSize(), __boolean->domainSize() );
      TS_ASSERT_DIFFERS( variable, __boolean );
    }

    void testGetVariableConst() {
      // Arrange
      Type boolean { *__boolean };
      const Type &const_boolean = boolean;
      gum::DiscreteVariable const *variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( variable = &( const_boolean.variable() ) );
      // Assert
      TS_ASSERT_EQUALS( variable->name(), __boolean->name() );
      TS_ASSERT_EQUALS( variable->description(), __boolean->description() );
      TS_ASSERT_EQUALS( variable->label(0), __boolean->label(0) );
      TS_ASSERT_EQUALS( variable->label(1), __boolean->label(1) );
      TS_ASSERT_EQUALS( variable->domainSize(), __boolean->domainSize() );
      TS_ASSERT_DIFFERS( variable, __boolean );
    }

    void testIndirectionOperator() {
      // Arrange
      Type boolean { *__boolean };
      gum::DiscreteVariable *variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( variable = &( *boolean ) );
      // Assert
      TS_ASSERT_EQUALS( variable->name(), __boolean->name() );
      TS_ASSERT_EQUALS( variable->description(), __boolean->description() );
      TS_ASSERT_EQUALS( variable->label(0), __boolean->label(0) );
      TS_ASSERT_EQUALS( variable->label(1), __boolean->label(1) );
      TS_ASSERT_EQUALS( variable->domainSize(), __boolean->domainSize() );
      TS_ASSERT_DIFFERS( variable, __boolean );
    }

    void testIndirectionOperatorConst() {
      // Arrange
      Type boolean { *__boolean };
      const Type &const_boolean = boolean;
      gum::DiscreteVariable const *variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( variable = &( *const_boolean ) );
      // Assert
      TS_ASSERT_EQUALS( variable->name(), __boolean->name() );
      TS_ASSERT_EQUALS( variable->description(), __boolean->description() );
      TS_ASSERT_EQUALS( variable->label(0), __boolean->label(0) );
      TS_ASSERT_EQUALS( variable->label(1), __boolean->label(1) );
      TS_ASSERT_EQUALS( variable->domainSize(), __boolean->domainSize() );
      TS_ASSERT_DIFFERS( variable, __boolean );
    }

  };

} // gum_tests
