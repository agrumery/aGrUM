/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

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

  class ReferenceSlotTestSuite: public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMReferenceSlot< double > PRMReferenceSlot;
    ClassElementTestSuiteAbstract*               classEltTestSuite__;
    gum::prm::PRMClass< double >*                A__;
    gum::prm::PRMClass< double >*                B__;
    gum::prm::PRMType*                           boolean__;

    public:
    void setUp() {
      classEltTestSuite__ = new ClassElementTestSuiteAbstract();
      A__ = new gum::prm::PRMClass< double >("A");
      B__ = new gum::prm::PRMClass< double >("B");
      boolean__ = gum::prm::PRMType::boolean();
    }

    void tearDown() {
      delete classEltTestSuite__;
      delete A__;
      delete B__;
      delete boolean__;
    }

    /// PRMClassElement Tests
    /// @{
    void testIsReferenceSlot() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__);
      bool             expected = true;
      // Act & Assert
      classEltTestSuite__->testIsReferenceSlot(ref, expected);
    }

    void testIsAttribute() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__);
      bool             expected = false;
      // Act & Assert
      classEltTestSuite__->testIsAttribute(ref, expected);
    }

    void testIsSlotChain() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__);
      bool             expected = false;
      // Act & Assert
      classEltTestSuite__->testIsSlotChain(ref, expected);
    }

    void testSetNodeId() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__);
      // Act & Assert
      classEltTestSuite__->testSetNodeId(ref);
    }

    void testObjType() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__);
      // Act & Assert
      classEltTestSuite__->test_obj_type(ref);
    }

    void testSafeName() {
      try {
        // Arrange
        PRMReferenceSlot ref("ref", *A__);
        // Act & Assert
        classEltTestSuite__->testSafeName(ref);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testCast_NotAllowed() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__);
      // Act & Assert
      classEltTestSuite__->testCast_NotAllowed(ref);
    }

    /// @}

    /// Constructors & destructors
    /// @{
    void testConstructor() {
      // Arrange
      PRMReferenceSlot* ref = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(ref = new PRMReferenceSlot("ref", *A__, false));
      delete ref;
    }

    void testConstructorArray() {
      // Arrange
      PRMReferenceSlot* ref = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(ref = new PRMReferenceSlot("ref", *A__, true));
      delete ref;
    }
    /// @}

    /// Methods
    /// @{
    void testSlotType() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__, false);
      // Act
      auto& type = ref.slotType();
      // Assert
      TS_ASSERT_EQUALS(A__, &type);
    }

    void testSlotTypeConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__, false);
      const auto&      const_ref = ref;
      // Act
      const auto& type = const_ref.slotType();
      // Assert
      TS_ASSERT_EQUALS(A__, &type);
    }

    void testIsArrayFalse() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__, false);
      // Act & Assert
      TS_ASSERT(!ref.isArray());
    }

    void testIsArrayTrue() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__, true);
      // Act & Assert
      TS_ASSERT(ref.isArray());
    }

    void testType() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__, false);
      // Act & assert
      TS_ASSERT_THROWS(ref.type(), gum::OperationNotAllowed);
    }

    void testTypeConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__, false);
      const auto&      const_ref = ref;
      // Act & assert
      TS_ASSERT_THROWS(const_ref.type(), gum::OperationNotAllowed);
    }

    void testCPF() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__, false);
      // Act & assert
      TS_ASSERT_THROWS(ref.cpf(), gum::OperationNotAllowed);
    }

    void testCPFConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *A__, false);
      const auto&      const_ref = ref;
      // Act & assert
      TS_ASSERT_THROWS(const_ref.cpf(), gum::OperationNotAllowed);
    }
    /// @}

    /// Add parents and children
    /// @{
    void testAddParentCheckChild() {
      // Arrange
      gum::prm::PRMScalarAttribute< double > parent("attr", *boolean__);
      PRMReferenceSlot                       child("child", *A__);
      auto before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent));
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
    }

    void testAddChild() {
      // Arrange
      PRMReferenceSlot                       parent("simple", *A__);
      gum::prm::PRMScalarAttribute< double > child("attr", *boolean__);
      auto before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(parent.addChild(child));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
    }
    /// @}
  };

}   // namespace gum_tests
