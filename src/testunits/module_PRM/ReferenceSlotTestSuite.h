/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

  class [[maybe_unused]] ReferenceSlotTestSuite: public CxxTest::TestSuite {
    private:
    using PRMReferenceSlot = gum::prm::PRMReferenceSlot< double >;

    ClassElementTestSuiteAbstract* _classEltTestSuite_;
    gum::prm::PRMClass< double >*  _A_;
    gum::prm::PRMClass< double >*  _B_;
    gum::prm::PRMType*             _boolean_;

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
    GUM_TEST(IsReferenceSlot) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      bool             expected = true;
      // Act & Assert
      _classEltTestSuite_->testIsReferenceSlot(ref, expected);
    }

    GUM_TEST(IsAttribute) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      bool             expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsAttribute(ref, expected);
    }

    GUM_TEST(IsSlotChain) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      bool             expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsSlotChain(ref, expected);
    }

    GUM_TEST(SetNodeId) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      // Act & Assert
      _classEltTestSuite_->testSetNodeId(ref);
    }

    GUM_TEST(ObjType) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      // Act & Assert
      _classEltTestSuite_->test_obj_type(ref);
    }

    GUM_TEST(SafeName) {
      try {
        // Arrange
        PRMReferenceSlot ref("ref", *_A_);
        // Act & Assert
        _classEltTestSuite_->testSafeName(ref);
      } catch (gum::Exception&) { TS_ASSERT(false) }
    }

    GUM_TEST(Cast_NotAllowed) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      // Act & Assert
      _classEltTestSuite_->testCast_NotAllowed(ref);
    }

    /// @}

    /// Constructors & destructors
    /// @{
    GUM_TEST(Constructor) {
      // Arrange
      PRMReferenceSlot* ref = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(ref = new PRMReferenceSlot("ref", *_A_, false))
      delete ref;
    }

    GUM_TEST(ConstructorArray) {
      // Arrange
      PRMReferenceSlot* ref = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(ref = new PRMReferenceSlot("ref", *_A_, true))
      delete ref;
    }
    /// @}

    /// Methods
    /// @{
    GUM_TEST(SlotType) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act
      auto& type = ref.slotType();
      // Assert
      TS_ASSERT_EQUALS(_A_, &type)
    }

    GUM_TEST(SlotTypeConst) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      const auto&      const_ref = ref;
      // Act
      const auto& type = const_ref.slotType();
      // Assert
      TS_ASSERT_EQUALS(_A_, &type)
    }

    GUM_TEST(IsArrayFalse) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act & Assert
      TS_ASSERT(!ref.isArray())
    }

    GUM_TEST(IsArrayTrue) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, true);
      // Act & Assert
      TS_ASSERT(ref.isArray())
    }

    GUM_TEST(Type) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act & assert
      TS_ASSERT_THROWS(ref.type(), const gum::OperationNotAllowed&)
    }

    GUM_TEST(TypeConst) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      const auto&      const_ref = ref;
      // Act & assert
      TS_ASSERT_THROWS(const_ref.type(), const gum::OperationNotAllowed&)
    }

    GUM_TEST(CPF) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act & assert
      TS_ASSERT_THROWS(ref.cpf(), const gum::OperationNotAllowed&)
    }

    GUM_TEST(CPFConst) {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      const auto&      const_ref = ref;
      // Act & assert
      TS_ASSERT_THROWS(const_ref.cpf(), const gum::OperationNotAllowed&)
    }
    /// @}

    /// Add parents and children
    /// @{
    GUM_TEST(AddParentCheckChild) {
      // Arrange
      gum::prm::PRMScalarAttribute< double > parent("attr", *_boolean_);
      PRMReferenceSlot                       child("child", *_A_);
      auto                                   before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent))
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
    }

    GUM_TEST(AddChild) {
      // Arrange
      PRMReferenceSlot                       parent("simple", *_A_);
      gum::prm::PRMScalarAttribute< double > child("attr", *_boolean_);
      auto                                   before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(parent.addChild(child))
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
    }
    /// @}
  };

}   // namespace gum_tests
