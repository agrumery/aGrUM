/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMFormAttribute.h>
#include <agrum/PRM/elements/PRMReferenceSlot.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  struct ReferenceSlotTestSuite {
    protected:
    using PRMReferenceSlot = gum::prm::PRMReferenceSlot< double >;

    ClassElementTestSuiteAbstract* _classEltTestSuite_;
    gum::prm::PRMClass< double >*  _A_;
    gum::prm::PRMClass< double >*  _B_;
    gum::prm::PRMType*             _boolean_;

    public:
    /// PRMClassElement Tests
    /// @{
    ReferenceSlotTestSuite() {
      _classEltTestSuite_ = new ClassElementTestSuiteAbstract();
      _A_                 = new gum::prm::PRMClass< double >("A");
      _B_                 = new gum::prm::PRMClass< double >("B");
      _boolean_           = gum::prm::PRMType::boolean();
    }

    ~ReferenceSlotTestSuite() {
      delete _classEltTestSuite_;
      delete _A_;
      delete _B_;
      delete _boolean_;
    }

    /// @}

    /// Constructors & destructors
    /// @{


    /// @}

    /// Methods
    /// @{


    /// @}

    /// Add parents and children
    /// @{


    /// @}
  };

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
    } catch (gum::Exception&) { CHECK(false); }
  }

  GUM_TEST(Cast_NotAllowed) {
    // Arrange
    PRMReferenceSlot ref("ref", *_A_);
    // Act & Assert
    _classEltTestSuite_->testCast_NotAllowed(ref);
  }

  GUM_TEST(Constructor) {
    // Arrange
    PRMReferenceSlot* ref = nullptr;
    // Act & Assert
    CHECK_NOTHROW(ref = new PRMReferenceSlot("ref", *_A_, false));
    delete ref;
  }

  GUM_TEST(ConstructorArray) {
    // Arrange
    PRMReferenceSlot* ref = nullptr;
    // Act & Assert
    CHECK_NOTHROW(ref = new PRMReferenceSlot("ref", *_A_, true));
    delete ref;
  }

  GUM_TEST(SlotType) {
    // Arrange
    PRMReferenceSlot ref("ref", *_A_, false);
    // Act
    auto& type = ref.slotType();
    // Assert
    CHECK_EQ(_A_, &type);
  }

  GUM_TEST(SlotTypeConst) {
    // Arrange
    PRMReferenceSlot ref("ref", *_A_, false);
    const auto&      const_ref = ref;
    // Act
    const auto& type = const_ref.slotType();
    // Assert
    CHECK_EQ(_A_, &type);
  }

  GUM_TEST(IsArrayFalse) {
    // Arrange
    PRMReferenceSlot ref("ref", *_A_, false);
    // Act & Assert
    CHECK(!ref.isArray());
  }

  GUM_TEST(IsArrayTrue) {
    // Arrange
    PRMReferenceSlot ref("ref", *_A_, true);
    // Act & Assert
    CHECK(ref.isArray());
  }

  GUM_TEST(Type) {
    // Arrange
    PRMReferenceSlot ref("ref", *_A_, false);
    // Act & assert
    CHECK_THROWS_AS(ref.type(), const gum::OperationNotAllowed&);
  }

  GUM_TEST(TypeConst) {
    // Arrange
    PRMReferenceSlot ref("ref", *_A_, false);
    const auto&      const_ref = ref;
    // Act & assert
    CHECK_THROWS_AS(const_ref.type(), const gum::OperationNotAllowed&);
  }

  GUM_TEST(CPF) {
    // Arrange
    PRMReferenceSlot ref("ref", *_A_, false);
    // Act & assert
    CHECK_THROWS_AS(ref.cpf(), const gum::OperationNotAllowed&);
  }

  GUM_TEST(CPFConst) {
    // Arrange
    PRMReferenceSlot ref("ref", *_A_, false);
    const auto&      const_ref = ref;
    // Act & assert
    CHECK_THROWS_AS(const_ref.cpf(), const gum::OperationNotAllowed&);
  }

  GUM_TEST(AddParentCheckChild) {
    // Arrange
    gum::prm::PRMScalarAttribute< double > parent("attr", *_boolean_);
    PRMReferenceSlot                       child("child", *_A_);
    auto                                   before = parent.cpf().variablesSequence().size();
    // Act
    CHECK_NOTHROW(child.addParent(parent));
    // Assert
    auto after = parent.cpf().variablesSequence().size();
    CHECK_EQ(before, after);
  }

  GUM_TEST(AddChild) {
    // Arrange
    PRMReferenceSlot                       parent("simple", *_A_);
    gum::prm::PRMScalarAttribute< double > child("attr", *_boolean_);
    auto                                   before = child.cpf().variablesSequence().size();
    // Act
    CHECK_NOTHROW(parent.addChild(child));
    // Assert
    auto after = child.cpf().variablesSequence().size();
    CHECK_EQ(before, after);
  }

}   // namespace gum_tests
