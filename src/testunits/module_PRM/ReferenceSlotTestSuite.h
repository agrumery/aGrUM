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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMFormAttribute.h>
#include <agrum/PRM/elements/PRMReferenceSlot.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  ReferenceSlot
#define GUM_CURRENT_MODULE PRM

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  struct ReferenceSlotTestSuite {
    private:
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

    static void testIsReferenceSlot() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      bool             expected = true;
      // Act & Assert
      _classEltTestSuite_->testIsReferenceSlot(ref, expected);
    }

    static void testIsAttribute() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      bool             expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsAttribute(ref, expected);
    }

    static void testIsSlotChain() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      bool             expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsSlotChain(ref, expected);
    }

    static void testSetNodeId() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      // Act & Assert
      _classEltTestSuite_->testSetNodeId(ref);
    }

    static void testObjType() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      // Act & Assert
      _classEltTestSuite_->test_obj_type(ref);
    }

    static void testSafeName() {
      try {
        // Arrange
        PRMReferenceSlot ref("ref", *_A_);
        // Act & Assert
        _classEltTestSuite_->testSafeName(ref);
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testCast_NotAllowed() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_);
      // Act & Assert
      _classEltTestSuite_->testCast_NotAllowed(ref);
    }

    /// @}

    /// Constructors & destructors
    /// @{
    static void testConstructor() {
      // Arrange
      PRMReferenceSlot* ref = nullptr;
      // Act & Assert
      CHECK_NOTHROW(ref = new PRMReferenceSlot("ref", *_A_, false));
      delete ref;
    }

    static void testConstructorArray() {
      // Arrange
      PRMReferenceSlot* ref = nullptr;
      // Act & Assert
      CHECK_NOTHROW(ref = new PRMReferenceSlot("ref", *_A_, true));
      delete ref;
    }

    /// @}

    /// Methods
    /// @{
    static void testSlotType() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act
      auto& type = ref.slotType();
      // Assert
      CHECK((_A_) == (&type));
    }

    static void testSlotTypeConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      const auto&      const_ref = ref;
      // Act
      const auto& type = const_ref.slotType();
      // Assert
      CHECK((_A_) == (&type));
    }

    static void testIsArrayFalse() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act & Assert
      CHECK(!ref.isArray());
    }

    static void testIsArrayTrue() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, true);
      // Act & Assert
      CHECK(ref.isArray());
    }

    static void testType() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act & assert
      CHECK_THROWS_AS(ref.type(), const gum::OperationNotAllowed&);
    }

    static void testTypeConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      const auto&      const_ref = ref;
      // Act & assert
      CHECK_THROWS_AS(const_ref.type(), const gum::OperationNotAllowed&);
    }

    static void testCPF() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      // Act & assert
      CHECK_THROWS_AS(ref.cpf(), const gum::OperationNotAllowed&);
    }

    static void testCPFConst() {
      // Arrange
      PRMReferenceSlot ref("ref", *_A_, false);
      const auto&      const_ref = ref;
      // Act & assert
      CHECK_THROWS_AS(const_ref.cpf(), const gum::OperationNotAllowed&);
    }

    /// @}

    /// Add parents and children
    /// @{
    static void testAddParentCheckChild() {
      // Arrange
      gum::prm::PRMScalarAttribute< double > parent("attr", *_boolean_);
      PRMReferenceSlot                       child("child", *_A_);
      auto                                   before = parent.cpf().variablesSequence().size();
      // Act
      CHECK_NOTHROW(child.addParent(parent));
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      CHECK((before) == (after));
    }

    static void testAddChild() {
      // Arrange
      PRMReferenceSlot                       parent("simple", *_A_);
      gum::prm::PRMScalarAttribute< double > child("attr", *_boolean_);
      auto                                   before = child.cpf().variablesSequence().size();
      // Act
      CHECK_NOTHROW(parent.addChild(child));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      CHECK((before) == (after));
    }

    /// @}
  };

  GUM_TEST_ACTIF(IsReferenceSlot)
  GUM_TEST_ACTIF(IsAttribute)
  GUM_TEST_ACTIF(IsSlotChain)
  GUM_TEST_ACTIF(SetNodeId)
  GUM_TEST_ACTIF(ObjType)
  GUM_TEST_ACTIF(SafeName)
  GUM_TEST_ACTIF(Cast_NotAllowed)
  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(ConstructorArray)
  GUM_TEST_ACTIF(SlotType)
  GUM_TEST_ACTIF(SlotTypeConst)
  GUM_TEST_ACTIF(IsArrayFalse)
  GUM_TEST_ACTIF(IsArrayTrue)
  GUM_TEST_ACTIF(Type)
  GUM_TEST_ACTIF(TypeConst)
  GUM_TEST_ACTIF(CPF)
  GUM_TEST_ACTIF(CPFConst)
  GUM_TEST_ACTIF(AddParentCheckChild)
  GUM_TEST_ACTIF(AddChild)

}   // namespace gum_tests
