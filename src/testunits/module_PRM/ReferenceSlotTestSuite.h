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
 *   along with this program; if !, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMFormAttribute.h>
#include <agrum/PRM/elements/PRMReferenceSlot.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class ReferenceSlotTestSuite : public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMReferenceSlot< double > PRMReferenceSlot;
    ClassElementTestSuiteAbstract*               __classEltTestSuite;
    gum::prm::PRMClass< double >*                __A;
    gum::prm::PRMClass< double >*                __B;
    gum::prm::PRMType< double >*                 __boolean;

    public:
    void setUp() {
      __classEltTestSuite = new ClassElementTestSuiteAbstract();
      __A = new gum::prm::PRMClass< double >("A");
      __B = new gum::prm::PRMClass< double >("B");
      __boolean = gum::prm::PRMType< double >::boolean();
    }

    void tearDown() {
      delete __classEltTestSuite;
      delete __A;
      delete __B;
      delete __boolean;
    }

    /// PRMClassElement Tests
    /// @{
    void testIsReferenceSlot() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A);
      bool             expected = true;
      // Act & Assert
      __classEltTestSuite->testIsReferenceSlot(ref, expected);
    }

    void testIsAttribute() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A);
      bool             expected = false;
      // Act & Assert
      __classEltTestSuite->testIsAttribute(ref, expected);
    }

    void testIsSlotChain() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A);
      bool             expected = false;
      // Act & Assert
      __classEltTestSuite->testIsSlotChain(ref, expected);
    }

    void testSetNodeId() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A);
      // Act & Assert
      __classEltTestSuite->testSetNodeId(ref);
    }

    void testObjType() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A);
      // Act & Assert
      __classEltTestSuite->test_obj_type(ref);
    }

    void testSafeName() {
      try {
        // Arrange
        PRMReferenceSlot ref("ref", *__A);
        // Act & Assert
        __classEltTestSuite->testSafeName(ref);
      } catch (gum::Exception& e) {
        GUM_TRACE(e.errorContent());
        GUM_TRACE(e.errorCallStack());
        TS_ASSERT(false);
      }
    }

    void testCast_NotAllowed() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A);
      // Act & Assert
      __classEltTestSuite->testCast_NotAllowed(ref);
    }

    /// @}

    /// Constructors & destructors
    /// @{
    void testConstructor() {
      // Arrange
      PRMReferenceSlot* ref = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(ref = new PRMReferenceSlot("ref", *__A, false));
      delete ref;
    }

    void testConstructorArray() {
      // Arrange
      PRMReferenceSlot* ref = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(ref = new PRMReferenceSlot("ref", *__A, true));
      delete ref;
    }
    /// @}

    /// Methods
    /// @{
    void testSlotType() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A, false);
      // Act
      auto& type = ref.slotType();
      // Assert
      TS_ASSERT_EQUALS(__A, &type);
    }

    void testSlotTypeConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A, false);
      const auto&      const_ref = ref;
      // Act
      const auto& type = const_ref.slotType();
      // Assert
      TS_ASSERT_EQUALS(__A, &type);
    }

    void testIsArrayFalse() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A, false);
      // Act & Assert
      TS_ASSERT(!ref.isArray());
    }

    void testIsArrayTrue() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A, true);
      // Act & Assert
      TS_ASSERT(ref.isArray());
    }

    void testType() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A, false);
      // Act & assert
      TS_ASSERT_THROWS(ref.type(), gum::OperationNotAllowed);
    }

    void testTypeConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A, false);
      const auto&      const_ref = ref;
      // Act & assert
      TS_ASSERT_THROWS(const_ref.type(), gum::OperationNotAllowed);
    }

    void testCPF() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A, false);
      // Act & assert
      TS_ASSERT_THROWS(ref.cpf(), gum::OperationNotAllowed);
    }

    void testCPFConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *__A, false);
      const auto&      const_ref = ref;
      // Act & assert
      TS_ASSERT_THROWS(const_ref.cpf(), gum::OperationNotAllowed);
    }
    /// @}

    /// Add parents and children
    /// @{
    void testAddParentCheckChild() {
      // Arrange
      gum::prm::PRMScalarAttribute< double > parent("attr", *__boolean);
      PRMReferenceSlot                       child("child", *__A);
      auto before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent));
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
    }

    void testAddChild() {
      // Arrange
      PRMReferenceSlot                       parent("simple", *__A);
      gum::prm::PRMScalarAttribute< double > child("attr", *__boolean);
      auto before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(parent.addChild(child));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
    }
    /// @}
  };

}  // gum_tests
