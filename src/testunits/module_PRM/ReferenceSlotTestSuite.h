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

#include <agrum/PRM/elements/formAttribute.h>
#include <agrum/PRM/elements/referenceSlot.h>
#include <agrum/PRM/elements/class.h>

#include <module_PRM/ClassElementTestSuite.h>

/**
 * This class is used to test gum::prm::ClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::ClassElement.
 */
namespace gum_tests {

  class ReferenceSlotTestSuite : public CxxTest::TestSuite {
    private:
    typedef gum::prm::ReferenceSlot<double> ReferenceSlot;
    ClassElementTestSuite* __classEltTestSuite;
    gum::prm::Class<double>* __A;
    gum::prm::Class<double>* __B;
    gum::prm::Type<double>* __boolean;

    public:
    void setUp() {
      __classEltTestSuite = new ClassElementTestSuite();
      __A = new gum::prm::Class<double>( "A" );
      __B = new gum::prm::Class<double>( "B" );
      __boolean = gum::prm::Type<double>::boolean();
    }

    void tearDown() {
      delete __classEltTestSuite;
      delete __A;
      delete __B;
      delete __boolean;
    }

    /// ClassElement Tests
    /// @{
    void testIsReferenceSlot() {
      // Arrange
      ReferenceSlot ref( "ref", *__A );
      bool expected = true;
      // Act & Assert
      __classEltTestSuite->testIsReferenceSlot( ref, expected );
    }

    void testIsAttribute() {
      // Arrange
      ReferenceSlot ref( "ref", *__A );
      bool expected = false;
      // Act & Assert
      __classEltTestSuite->testIsAttribute( ref, expected );
    }

    void testIsSlotChain() {
      // Arrange
      ReferenceSlot ref( "ref", *__A );
      bool expected = false;
      // Act & Assert
      __classEltTestSuite->testIsSlotChain( ref, expected );
    }

    void testSetNodeId() {
      // Arrange
      ReferenceSlot ref( "ref", *__A );
      // Act & Assert
      __classEltTestSuite->testSetNodeId( ref );
    }

    void testObjType() {
      // Arrange
      ReferenceSlot ref( "ref", *__A );
      // Act & Assert
      __classEltTestSuite->test_obj_type( ref );
    }

    void testSafeName() {
      try {
        // Arrange
        ReferenceSlot ref( "ref", *__A );
        // Act & Assert
        __classEltTestSuite->testSafeName( ref );
      } catch ( gum::Exception& e ) {
        GUM_TRACE( e.errorContent() );
        GUM_TRACE( e.errorCallStack() );
        TS_ASSERT( false );
      }
    }

    void testCast_NotAllowed() {
      // Arrange
      ReferenceSlot ref( "ref", *__A );
      // Act & Assert
      __classEltTestSuite->testCast_NotAllowed( ref );
    }

    /// @}

    /// Constructors & destructors
    /// @{
    void testConstructor() {
      // Arrange
      ReferenceSlot* ref = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING( ref = new ReferenceSlot( "ref", *__A, false ) );
      delete ref;
    }

    void testConstructorArray() {
      // Arrange
      ReferenceSlot* ref = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING( ref = new ReferenceSlot( "ref", *__A, true ) );
      delete ref;
    }
    /// @}

    /// Methods
    /// @{
    void testSlotType() {
      // Arrange
      ReferenceSlot ref( "ref", *__A, false );
      // Act
      auto& type = ref.slotType();
      // Assert
      TS_ASSERT_EQUALS( __A, &type );
    }

    void testSlotTypeConst() {
      // Arrange
      ReferenceSlot ref( "ref", *__A, false );
      const auto& const_ref = ref;
      // Act
      const auto& type = const_ref.slotType();
      // Assert
      TS_ASSERT_EQUALS( __A, &type );
    }

    void testIsArrayFalse() {
      // Arrange
      ReferenceSlot ref( "ref", *__A, false );
      // Act & Assert
      TS_ASSERT( not ref.isArray() );
    }

    void testIsArrayTrue() {
      // Arrange
      ReferenceSlot ref( "ref", *__A, true );
      // Act & Assert
      TS_ASSERT( ref.isArray() );
    }

    void testType() {
      // Arrange
      ReferenceSlot ref( "ref", *__A, false );
      // Act & assert
      TS_ASSERT_THROWS( ref.type(), gum::OperationNotAllowed );
    }

    void testTypeConst() {
      // Arrange
      ReferenceSlot ref( "ref", *__A, false );
      const auto& const_ref = ref;
      // Act & assert
      TS_ASSERT_THROWS( const_ref.type(), gum::OperationNotAllowed );
    }

    void testCPF() {
      // Arrange
      ReferenceSlot ref( "ref", *__A, false );
      // Act & assert
      TS_ASSERT_THROWS( ref.cpf(), gum::OperationNotAllowed );
    }

    void testCPFConst() {
      // Arrange
      ReferenceSlot ref( "ref", *__A, false );
      const auto& const_ref = ref;
      // Act & assert
      TS_ASSERT_THROWS( const_ref.cpf(), gum::OperationNotAllowed );
    }
    /// @}

    /// Add parents and children
    /// @{
    void testAddParentCheckChild() {
      // Arrange
      gum::prm::ScalarAttribute<double> parent( "attr", *__boolean );
      ReferenceSlot child( "child", *__A );
      auto before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING( child.addParent( parent ) );
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS( before, after );
    }

    void testAddChild() {
      // Arrange
      ReferenceSlot parent( "simple", *__A );
      gum::prm::ScalarAttribute<double> child( "attr", *__boolean );
      auto before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING( parent.addChild( child ) );
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS( before, after );
    }
    /// @}
  };

}  // gum_tests
