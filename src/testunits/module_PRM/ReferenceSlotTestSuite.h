/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
    ClassElementTestSuiteAbstract*               _classEltTestSuite_;
    gum::prm::PRMClass< double >*                _A_;
    gum::prm::PRMClass< double >*                _B_;
    gum::prm::PRMType*                           _boolean_;

    public:
    void setUp() {
      _classEltTestSuite_ = new ClassElementTestSuiteAbstract();
      _A_                 = new gum::prm::PRMClass< double >("A");
      _B_                 = new gum::prm::PRMClass< double >("B");
      _boolean_           = gum::prm::PRMType::boolean();
    }

    void tearDown() {
      delete _classEltTestSuite_;
      delete _A_;
      delete _B_;
      delete _boolean_;
    }

    /// PRMClassElement Tests
    /// @{
    void testIsReferenceSlot() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      bool             expected = true;
      // Act & Assert
      _classEltTestSuite_->testIsReferenceSlot(ref, expected);
    }

    void testIsAttribute() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      bool             expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsAttribute(ref, expected);
    }

    void testIsSlotChain() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      bool             expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsSlotChain(ref, expected);
    }

    void testSetNodeId() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      // Act & Assert
      _classEltTestSuite_->testSetNodeId(ref);
    }

    void testObjType() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      // Act & Assert
      _classEltTestSuite_->test_obj_type(ref);
    }

    void testSafeName() {
      try {
        // Arrange
        PRMReferenceSlot ref("ref", *_A_);
        // Act & Assert
        _classEltTestSuite_->testSafeName(ref);
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testCast_NotAllowed() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      // Act & Assert
      _classEltTestSuite_->testCast_NotAllowed(ref);
    }

    /// @}

    /// Constructors & destructors
    /// @{
    void testConstructor() {
      // Arrange
      PRMReferenceSlot* ref = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(ref = new PRMReferenceSlot("ref", *_A_, false));
      delete ref;
    }

    void testConstructorArray() {
      // Arrange
      PRMReferenceSlot* ref = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(ref = new PRMReferenceSlot("ref", *_A_, true));
      delete ref;
    }
    /// @}

    /// Methods
    /// @{
    void testSlotType() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act
      auto& type = ref.slotType();
      // Assert
      TS_ASSERT_EQUALS(_A_, &type);
    }

    void testSlotTypeConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      const auto&      const_ref = ref;
      // Act
      const auto& type = const_ref.slotType();
      // Assert
      TS_ASSERT_EQUALS(_A_, &type);
    }

    void testIsArrayFalse() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act & Assert
      TS_ASSERT(!ref.isArray());
    }

    void testIsArrayTrue() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, true);
      // Act & Assert
      TS_ASSERT(ref.isArray());
    }

    void testType() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act & assert
      TS_ASSERT_THROWS(ref.type(), gum::OperationNotAllowed);
    }

    void testTypeConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      const auto&      const_ref = ref;
      // Act & assert
      TS_ASSERT_THROWS(const_ref.type(), gum::OperationNotAllowed);
    }

    void testCPF() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act & assert
      TS_ASSERT_THROWS(ref.cpf(), gum::OperationNotAllowed);
    }

    void testCPFConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      const auto&      const_ref = ref;
      // Act & assert
      TS_ASSERT_THROWS(const_ref.cpf(), gum::OperationNotAllowed);
    }
    /// @}

    /// Add parents and children
    /// @{
    void testAddParentCheckChild() {
      // Arrange
      gum::prm::PRMScalarAttribute< double > parent("attr", *_boolean_);
      PRMReferenceSlot                       child("child", *_A_);
      auto                                   before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent));
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
    }

    void testAddChild() {
      // Arrange
      PRMReferenceSlot                       parent("simple", *_A_);
      gum::prm::PRMScalarAttribute< double > child("attr", *_boolean_);
      auto                                   before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(parent.addChild(child));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
    }
    /// @}
  };

}   // namespace gum_tests
