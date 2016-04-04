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
#include <cxxtest/testsuite_utils.h>

#include <agrum/core/exceptions.h>
#include <agrum/PRM/elements/PRMType.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class PRMTypeTestSuite : public CxxTest::TestSuite {
    typedef gum::prm::PRMType<double> PRMType;

    gum::LabelizedVariable* __boolean;
    gum::LabelizedVariable* __state;

    public:
    void setUp() {
      __boolean = new gum::LabelizedVariable{"boolean", "Boolean variable", 0};
      __boolean->addLabel( "false" );
      __boolean->addLabel( "true" );

      __state = new gum::LabelizedVariable{"state", "State variable", 0};
      __state->addLabel( "OK" );
      __state->addLabel( "NOK" );
    }

    void tearDown() {
      delete __boolean;
      delete __state;
    }

    void testStaticBoolean() {
      // Arrange
      std::string labels[] = {"false", "true"};
      std::string name = "boolean";
      std::string decription = "Boolean variable";
      // Act
      auto boolean = PRMType::boolean();
      // Assert
      TS_ASSERT_EQUALS( boolean->variable().domainSize(), (gum::Size)2 );
      TS_ASSERT_EQUALS( boolean->variable().label( 0 ), labels[0] );
      TS_ASSERT_EQUALS( boolean->variable().label( 1 ), labels[1] );
      delete boolean;
    }

    void testConstructor() {
      // Arrange
      PRMType* type = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( type = new PRMType{*__boolean} );
      // Assert
      TS_ASSERT_THROWS_NOTHING( delete type );
    }

    void testSuperConstructor() {
      // Arrange
      PRMType boolean{*__boolean};
      PRMType* state = nullptr;
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      // Act
      try {
        state = new PRMType{boolean, map, *__state};
      } catch ( ... ) {
        // TS_ASSERT_THROWS_NOTHING does not work here
        TS_FAIL( "Exception thrown" );
      }
      // Assert
      TS_ASSERT_THROWS_NOTHING( delete state );
    }

    void testCopyConstructor() {
      // Arrange
      PRMType boolean{*__boolean};
      PRMType* copy = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( copy = new PRMType{boolean} );
      // Assert
      TS_ASSERT_THROWS_NOTHING( delete copy );
    }

    void testGetVariable() {
      // Arrange
      PRMType boolean{*__boolean};
      gum::DiscreteVariable* variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( variable = &( boolean.variable() ) );
      // Assert
      TS_ASSERT_EQUALS( variable->name(), __boolean->name() );
      TS_ASSERT_EQUALS( variable->description(), __boolean->description() );
      TS_ASSERT_EQUALS( variable->label( 0 ), __boolean->label( 0 ) );
      TS_ASSERT_EQUALS( variable->label( 1 ), __boolean->label( 1 ) );
      TS_ASSERT_EQUALS( variable->domainSize(), __boolean->domainSize() );
      TS_ASSERT_DIFFERS( variable, __boolean );
    }

    void testGetVariableConst() {
      // Arrange
      PRMType boolean{*__boolean};
      const PRMType& const_boolean = boolean;
      gum::DiscreteVariable const* variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( variable = &( const_boolean.variable() ) );
      // Assert
      TS_ASSERT_EQUALS( variable->name(), __boolean->name() );
      TS_ASSERT_EQUALS( variable->description(), __boolean->description() );
      TS_ASSERT_EQUALS( variable->label( 0 ), __boolean->label( 0 ) );
      TS_ASSERT_EQUALS( variable->label( 1 ), __boolean->label( 1 ) );
      TS_ASSERT_EQUALS( variable->domainSize(), __boolean->domainSize() );
      TS_ASSERT_DIFFERS( variable, __boolean );
    }

    void testIndirectionOperator() {
      // Arrange
      PRMType boolean{*__boolean};
      gum::DiscreteVariable* variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( variable = &( *boolean ) );
      // Assert
      TS_ASSERT_EQUALS( variable->name(), __boolean->name() );
      TS_ASSERT_EQUALS( variable->description(), __boolean->description() );
      TS_ASSERT_EQUALS( variable->label( 0 ), __boolean->label( 0 ) );
      TS_ASSERT_EQUALS( variable->label( 1 ), __boolean->label( 1 ) );
      TS_ASSERT_EQUALS( variable->domainSize(), __boolean->domainSize() );
      TS_ASSERT_DIFFERS( variable, __boolean );
    }

    void testIndirectionOperatorConst() {
      // Arrange
      PRMType boolean{*__boolean};
      const PRMType& const_boolean = boolean;
      gum::DiscreteVariable const* variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( variable = &( *const_boolean ) );
      // Assert
      TS_ASSERT_EQUALS( variable->name(), __boolean->name() );
      TS_ASSERT_EQUALS( variable->description(), __boolean->description() );
      TS_ASSERT_EQUALS( variable->label( 0 ), __boolean->label( 0 ) );
      TS_ASSERT_EQUALS( variable->label( 1 ), __boolean->label( 1 ) );
      TS_ASSERT_EQUALS( variable->domainSize(), __boolean->domainSize() );
      TS_ASSERT_DIFFERS( variable, __boolean );
    }

    void testEqualityOperator() {
      // Arrange
      PRMType a{*__boolean};
      PRMType b{*__boolean};
      PRMType c{*__state};
      // Act & Assert
      TS_ASSERT( a == a );
      TS_ASSERT( a == b );
      TS_ASSERT( b == a );

      TS_ASSERT( !( c == a ) );
      TS_ASSERT( !( a == c ) );
      TS_ASSERT( !( c == b ) );
      TS_ASSERT( !( b == c ) );
    }

    void testInequalityOperator() {
      // Arrange
      PRMType a{*__boolean};
      PRMType b{*__boolean};
      PRMType c{*__state};

      // Act & Assert
      TS_ASSERT( !( a != a ) );
      TS_ASSERT( !( a != b ) );
      TS_ASSERT( !( b != a ) );

      TS_ASSERT( c != a );
      TS_ASSERT( a != c );
      TS_ASSERT( c != b );
      TS_ASSERT( b != c );
    }

    void testObjType() {
      // Arrange
      PRMType boolean{*__boolean};
      auto expected = gum::prm::PRMObject::prm_type::TYPE;
      // Act
      auto actual = boolean.obj_type();
      // Assert
      TS_ASSERT_EQUALS( expected, actual );
    }

    void testName() {
      // Arrange
      PRMType boolean{*__boolean};
      auto expected = __boolean->name();
      // Act
      auto actual = boolean.name();
      // Assert
      TS_ASSERT_EQUALS( expected, actual );
    }

    void testIsSubType() {
      // Arrange
      PRMType boolean{*__boolean};
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      PRMType state{boolean, map, *__state};
      // Act & Assert
      TS_ASSERT( state.isSubType() );
      TS_ASSERT( not boolean.isSubType() );
    }

    void testIsSubTypeOf() {
      // Arrange
      PRMType boolean{*__boolean};
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      PRMType state{boolean, map, *__state};
      PRMType dummy{*__state};
      // Act & Assert
      TS_ASSERT( state.isSubTypeOf( boolean ) );
      TS_ASSERT( state.isSubTypeOf( state ) );
      TS_ASSERT( not boolean.isSubTypeOf( state ) );
      TS_ASSERT( not dummy.isSubTypeOf( boolean ) );
      TS_ASSERT( not boolean.isSubTypeOf( dummy ) );
      TS_ASSERT( dummy.isSubTypeOf( state ) );
      TS_ASSERT( state.isSubTypeOf( dummy ) );
    }

    void testIsSuperTypeOf() {
      // Arrange
      PRMType boolean{*__boolean};
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      PRMType state{boolean, map, *__state};
      PRMType dummy{*__state};
      // Act & Assert
      TS_ASSERT( boolean.isSuperTypeOf( state ) );
      TS_ASSERT( state.isSuperTypeOf( state ) );
      TS_ASSERT( not state.isSuperTypeOf( boolean ) );
      TS_ASSERT( not dummy.isSuperTypeOf( boolean ) );
      TS_ASSERT( not boolean.isSuperTypeOf( dummy ) );
      TS_ASSERT( dummy.isSuperTypeOf( state ) );
      TS_ASSERT( state.isSuperTypeOf( dummy ) );
    }

    void testSuper() {
      // Arrange
      PRMType boolean{*__boolean};
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      PRMType state{boolean, map, *__state};
      PRMType* super = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( super = &( state.superType() ) );
      // Act & Assert
      TS_ASSERT_EQUALS( *super, boolean );
    }

    void testSuperConst() {
      // Arrange
      PRMType boolean{*__boolean};
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      PRMType state{boolean, map, *__state};
      const auto& dummy = state;
      PRMType const* super = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( super = &( dummy.superType() ) );
      // Act & Assert
      TS_ASSERT_EQUALS( *super, boolean );
    }

    void testSuperNotFound() {
      // Arrange
      PRMType boolean{*__boolean};
      // Act & Assert
      TS_ASSERT_THROWS( boolean.superType(), gum::NotFound );
    }

    void testSetSuper() {
      // Arrange
      PRMType boolean{*__boolean};
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      PRMType state{boolean, map, *__state};
      PRMType boolean_bis{*__boolean};
      // Act
      TS_ASSERT_THROWS_NOTHING( state.setSuper( boolean_bis ) );
      // Assert
      TS_ASSERT_EQUALS( state.superType(), boolean );
      TS_ASSERT_EQUALS( state.superType(), boolean_bis );
      TS_ASSERT_DIFFERS( &( state.superType() ), &boolean );
      TS_ASSERT_EQUALS( &( state.superType() ), &boolean_bis );
    }

    void testSetSuperWrongtype() {
      // Arrange
      PRMType boolean{*__boolean};
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      PRMType state{boolean, map, *__state};
      PRMType state_bis{*__state};
      // Act & Assert
      TS_ASSERT_THROWS( state.setSuper( state_bis ), gum::WrongType );
    }

    void testSetSuperOperationNotAllowed() {
      // Arrange
      PRMType boolean{*__boolean};
      PRMType state{*__state};
      // Act & Assert
      TS_ASSERT_THROWS( state.setSuper( boolean ), gum::OperationNotAllowed );
    }

    void testLabelMap() {
      // Arrange
      PRMType boolean{*__boolean};
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      PRMType state{boolean, map, *__state};
      // Act
      auto actual = state.label_map();
      // Assert
      TS_ASSERT_EQUALS( map, actual );
    }

    void testLabelMapNotFound() {
      // Arrange
      PRMType boolean{*__boolean};
      // Act & Assert
      TS_ASSERT_THROWS( boolean.label_map(), gum::NotFound );
    }
  };

}  // gum_tests
