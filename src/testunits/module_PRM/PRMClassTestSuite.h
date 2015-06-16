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

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <sstream>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/PRM/elements/class.h>

/**
 * This class is used to test gum::prm::ClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of 
 * gum::prm::ClassElement.
 */
namespace gum_tests {

  class PRMClassTestSuite : public CxxTest::TestSuite {
    private:
      typedef gum::prm::Class<double> Class;
      typedef gum::prm::Type<double> Type;
      typedef gum::prm::ScalarAttribute<double> Attribute;
      typedef gum::prm::ReferenceSlot<double> Reference;

      Type *__boolean;
      Type *__state;

    public:

      void setUp() {
        __boolean = gum::prm::Type<double>::boolean();
        gum::LabelizedVariable state { "state", "A state variable", 0 };
        state.addLabel( "OK" );
        state.addLabel( "NOK" );
        std::vector<gum::Idx> map;
        map.push_back( 1 );
        map.push_back( 0 );
        __state = new Type( *__boolean, map, state );
      }

      void tearDown() {
        delete __boolean;
        delete __state;
      }

      /// Constructor & Destructor
      /// @{
      void testConstructor() {
        // Arrange
        Class *c = nullptr;
        // Act & Assert
        TS_ASSERT_THROWS_NOTHING( c = new Class("class") );
        TS_ASSERT_THROWS_NOTHING( delete c );
      }
      /// @}
      /// Belongs and exists tests
      /// @{
      void testBelongsTo() {
        // Arrange
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        c.add(attr);
        bool actual = false;
        // Act
        TS_ASSERT_THROWS_NOTHING( actual = c.belongsTo( *attr ) );
        // Assert
        TS_ASSERT(actual);
      }

      void testBelongsToNot() {
        // Arrange
        Class c("class");
        Attribute attr("attr", *__boolean);
        bool actual = false;
        // Act
        TS_ASSERT_THROWS_NOTHING( actual = c.belongsTo( attr ) );
        // Assert
        TS_ASSERT(not actual);
      }

      void testExists() {
        // Arrange
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        c.add(attr);
        bool actual = false;
        // Act
        TS_ASSERT_THROWS_NOTHING( actual = c.exists( "attr" ) );
        // Assert
        TS_ASSERT(actual);
      }

      void testExistsNot() {
        // Arrange
        Class c("class");
        bool actual = false;
        // Act
        TS_ASSERT_THROWS_NOTHING( actual = c.exists( "attr" ) );
        // Assert
        TS_ASSERT(not actual);
      }

      void testGet() {
        // Arrange
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        c.add( attr );
        // Act
        auto &actual = c.get( attr->name() );
        // Assert
        TS_ASSERT_EQUALS( &actual, attr );
      }

      void testGetConst() {
        // Arrange
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        c.add( attr );
        const auto &const_c = c;
        // Act
        const auto &actual = const_c.get( attr->name() );
        // Assert
        TS_ASSERT_EQUALS( &actual, attr );
      }

      void testGetNotFound() {
        // Arrange
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        c.add( attr );
        // Act & Assert
        TS_ASSERT_THROWS( c.get( "foo" ), gum::NotFound );
      }

      void testGetConstNotFound() {
        // Arrange
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        c.add( attr );
        const auto &const_c = c;
        // Act & Assert
        TS_ASSERT_THROWS( const_c.get( "foo" ), gum::NotFound );
      }

      void testAdd() {
        // Arra,ge
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        gum::NodeId id = 100; // Id generation starts at 0
        // Act & assert
        TS_ASSERT_THROWS_NOTHING( id = c.add( attr ) );
        TS_ASSERT( c.exists( attr->name() ) );
        TS_ASSERT_EQUALS( &( c.get( attr->name() ) ), attr );
        TS_ASSERT_EQUALS( id, attr->id() );
        TS_ASSERT_DIFFERS( id, (gum::NodeId) 100 );
      }

      void testAddDuplicate() {
        // Arrange
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        // Act & assert
        TS_ASSERT_THROWS_NOTHING( c.add( attr ) );
        TS_ASSERT_THROWS( c.add( attr ), gum::DuplicateElement );
        TS_ASSERT_EQUALS( c.attributes().size(), (gum::Size) 1 );
      }

      void testOverloadOperationNotAllowed() {
        // Arrange
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        // Act & assert
        TS_ASSERT_THROWS( c.overload( attr ), gum::OperationNotAllowed );
        // Cleanup
        delete attr;
      }

      void testOverloadWrongClassElement() {
        // Arrange
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        c.add( attr );
        Class sub_c("sub c", c);
        Reference *ref = new Reference("attr", c);
        // Act & Assert
        TS_ASSERT_THROWS( sub_c.overload( ref ), gum::OperationNotAllowed );
        // Cleanup
        delete ref;
      }

      void testOverloadTypeError() {
        // Arrange
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        c.add( attr );
        Class sub_c("sub_c", c);
        gum::LabelizedVariable var("foo", "bar", 2);
        gum::prm::Type<double> type(var);
        Attribute *bttr = new Attribute("attr", type);
        // Act & Assert
        TS_ASSERT_THROWS( sub_c.overload( bttr ), gum::OperationNotAllowed );
        // Cleanup
        delete bttr;
      }

      void testOverloadAttribute() {
        // Arrange
        Class c("class");
        Attribute *attr = new Attribute("attr", *__boolean);
        c.add( attr );
        Class sub_c("sub_c", c);
        Attribute *state = new Attribute("attr", *__state);
        // Act
        try {
          sub_c.overload( state);
        } catch ( gum::Exception& e ) {
          GUM_TRACE( e.errorContent() );
          GUM_TRACE( e.errorCallStack() );
        }
        TS_ASSERT_THROWS_NOTHING( sub_c.overload( state) );
        // Assert
        TS_ASSERT( sub_c.exists( state->safeName() ) );
        TS_ASSERT( sub_c.exists( attr->safeName() ) );
        const auto &b = sub_c.get( attr->safeName() );
        const auto &s = sub_c.get( state->safeName() );
        TS_ASSERT_DIFFERS( b.type(), s.type() );
      }

      void testOverloadAttributeWithSeveralCastDescendants() {
        // Arrange
        int size = 10;
        std::vector< Type* > types;
        types.push_back( __boolean );
        std::vector<gum::Idx> map;
        map.push_back( 1 );
        map.push_back( 0 );
        for ( int i = 1; i < size; i++) {
          auto &super = *( types[i-1] );
          std::stringstream sbuff;
          sbuff << "type_" << i;
          auto name = sbuff.str();
          auto var = gum::LabelizedVariable(name, "", 2);
          auto t = new Type( super, map, var );
          types.push_back( t );
        }
        Class c("class");
        Attribute *attr = new Attribute("attr", *(types[0]));
        c.add( attr );
        Class sub_c("sub_c", c);
        Attribute *state = new Attribute("attr", *(types[size-1]));
        // Act
        try {
          sub_c.overload( state);
        } catch ( gum::Exception& e ) {
          GUM_TRACE( e.errorContent() );
          GUM_TRACE( e.errorCallStack() );
        }
        TS_ASSERT_THROWS_NOTHING( sub_c.overload( state) );
        // Assert
        for (int i = 0; i < size; i++) {
          std::string i_name = "(" + types[i]->name() + ")attr";
          TS_ASSERT( sub_c.exists( i_name ) );

          for (int j = i + 1; j < size; j++) {
            std::string j_name = "(" + types[j]->name() + ")attr";
            auto i_attr = sub_c.get( i_name ).type();
            auto j_attr = sub_c.get( j_name ).type();
            TS_ASSERT_DIFFERS( i_attr, j_attr );
          }
        }
        // Cleanup
        for (int i = 1; i < size; ++i) {
          delete types[i];
        }

      }
      /// @}
      /// Input, output and inner nodes methods.
      /// @{
      void testIsInputNode() {
        // Arrange
        Class c("class");
        Attribute *a = new Attribute("attr", *__boolean);
        c.add( a );
        bool actual = false;
        // Act
        TS_ASSERT_THROWS_NOTHING( actual = c.isInputNode( *a ) );
        // Assert
        TS_ASSERT( not actual );
      }

      void testSetInputNode() {
        // Arrange
        Class c("class");
        Attribute *a = new Attribute("attr", *__boolean);
        c.add( a );
        bool before = c.isInputNode( *a );
        bool after = false;
        // Act
        TS_ASSERT_THROWS_NOTHING( c.setInputNode(*a, true) );
        // Assert
        TS_ASSERT( after = c.isInputNode(*a) );
        TS_ASSERT_DIFFERS( before, after );
        TS_ASSERT( after );
      }

      void testIsOutputNode() {
        // Arrange
        Class c("class");
        Attribute *a = new Attribute("attr", *__boolean);
        c.add( a );
        bool actual = false;
        // Act
        TS_ASSERT_THROWS_NOTHING( actual = c.isOutputNode( *a ) );
        // Assert
        TS_ASSERT( not actual );
      }

      void testSetOutputNode() {
        // Arrange
        Class c("class");
        Attribute *a = new Attribute("attr", *__boolean);
        c.add( a );
        bool before = c.isOutputNode( *a );
        bool after = false;
        // Act
        TS_ASSERT_THROWS_NOTHING( c.setOutputNode(*a, true) );
        // Assert
        TS_ASSERT( after = c.isOutputNode(*a) );
        TS_ASSERT_DIFFERS( before, after );
        TS_ASSERT( after );
      }

      void testIsInnerNode() {
        // Arrange
        Class c("class");
        Attribute *a = new Attribute("attr", *__boolean);
        c.add( a );
        bool actual = false;
        // Act
        TS_ASSERT_THROWS_NOTHING( actual = c.isInnerNode( *a ) );
        // Assert
        TS_ASSERT( actual );
      }

      void testInnerNodeConsistency() {
        // Arrange
        Class c("class");
        Attribute *a = new Attribute("attr", *__boolean);
        c.add( a );
        // Act & Assert
        TS_ASSERT( c.isInnerNode( *a ) );
        TS_ASSERT( not c.isInputNode( *a ) );
        TS_ASSERT( not c.isOutputNode( *a ) );
        TS_ASSERT_THROWS_NOTHING( c.setInputNode( *a, true ) );
        TS_ASSERT( not c.isInnerNode( *a ) );
        TS_ASSERT( c.isInputNode( *a ) );
        TS_ASSERT( not c.isOutputNode( *a ) );
        TS_ASSERT_THROWS_NOTHING( c.setOutputNode( *a, true ) );
        TS_ASSERT( not c.isInnerNode( *a ) );
        TS_ASSERT( c.isInputNode( *a ) );
        TS_ASSERT( c.isOutputNode( *a ) );
        TS_ASSERT_THROWS_NOTHING( c.setInputNode( *a, false ) );
        TS_ASSERT_THROWS_NOTHING( c.setOutputNode( *a, false ) );
        TS_ASSERT( c.isInnerNode( *a ) );
        TS_ASSERT( not c.isInputNode( *a ) );
        TS_ASSERT( not c.isOutputNode( *a ) );
      }
      /// @}

  };

} // gum_tests
