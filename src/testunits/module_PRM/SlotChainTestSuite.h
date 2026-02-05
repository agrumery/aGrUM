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
#include <agrum/PRM/elements/PRMSlotChain.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  SlotChain
#define GUM_CURRENT_MODULE PRM

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  struct SlotChainTestSuite {
    private:
    using PRMSlotChain = gum::prm::PRMSlotChain< double >;

    ClassElementTestSuiteAbstract*                         _classEltTestSuite_;
    gum::prm::PRMClass< double >*                          _A_;
    gum::prm::PRMClass< double >*                          _B_;
    gum::prm::PRMClass< double >*                          _C_;
    gum::prm::PRMType*                                     _boolean_;
    gum::prm::PRMType*                                     _state_;
    gum::Sequence< gum::prm::PRMClassElement< double >* >* _booleanChain_;
    gum::Sequence< gum::prm::PRMClassElement< double >* >* _stateChain_;

    // these will be dispodes by their repsective class
    gum::prm::PRMReferenceSlot< double >* _refI_;
    gum::prm::PRMReferenceSlot< double >* _refJ_;
    gum::prm::PRMReferenceSlot< double >* _refK_;
    gum::prm::PRMAttribute< double >*     _boolAttr_;
    gum::prm::PRMAttribute< double >*     _stateAttr_;

    public:
    /// PRMClassElement Tests
    /// @{
    SlotChainTestSuite() {
      _classEltTestSuite_ = new ClassElementTestSuiteAbstract();
      _A_                 = new gum::prm::PRMClass< double >("A");
      _B_                 = new gum::prm::PRMClass< double >("B");
      _C_                 = new gum::prm::PRMClass< double >("C");
      _refI_              = new gum::prm::PRMReferenceSlot< double >("refA", *_B_);
      _A_->add(_refI_);
      _refJ_ = new gum::prm::PRMReferenceSlot< double >("refB", *_C_);
      _B_->add(_refJ_);
      _refK_ = new gum::prm::PRMReferenceSlot< double >("refK", *_C_, true);
      _B_->add(_refK_);
      _boolean_  = gum::prm::PRMType::boolean();
      _boolAttr_ = new gum::prm::PRMScalarAttribute< double >("myBool", *_boolean_);
      _C_->add(_boolAttr_);
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      _state_     = new gum::prm::PRMType(*_boolean_, map, state);
      _stateAttr_ = new gum::prm::PRMScalarAttribute< double >("myState", *_state_);
      _C_->add(_stateAttr_);
      _booleanChain_ = new gum::Sequence< gum::prm::PRMClassElement< double >* >();
      _booleanChain_->insert(_refI_);
      _booleanChain_->insert(_refJ_);
      _booleanChain_->insert(_boolAttr_);
      _stateChain_ = new gum::Sequence< gum::prm::PRMClassElement< double >* >();
      _stateChain_->insert(_refI_);
      _stateChain_->insert(_refK_);
      _stateChain_->insert(_stateAttr_);
    }

    ~SlotChainTestSuite() {
      delete _classEltTestSuite_;
      delete _A_;
      delete _B_;
      delete _C_;
      delete _boolean_;
      delete _state_;
      delete _booleanChain_;
      delete _stateChain_;
    }

    static void testIsReferenceSlot() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      bool         expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsReferenceSlot(slot, expected);
    }

    static void testIsAttribute() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      bool         expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsAttribute(slot, expected);
    }

    static void testIsSlotChain() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      bool         expected = true;
      // Act & Assert
      _classEltTestSuite_->testIsSlotChain(slot, expected);
    }

    static void testSetNodeId() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      // Act & Assert
      _classEltTestSuite_->testSetNodeId(slot);
    }

    static void testObjType() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      // Act & Assert
      _classEltTestSuite_->test_obj_type(slot);
    }

    static void testSafeName() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      // Act & Assert
      _classEltTestSuite_->testSafeName(slot);
    }

    static void testCast_NotAllowed() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      // Act & Assert
      _classEltTestSuite_->testCast_NotAllowed(slot);
    }

    static void testCast() {
      // Arrange
      PRMSlotChain slot("slot", *_stateChain_);
      // Act & Assert
      _classEltTestSuite_->testCast(slot, *_boolean_);
    }

    /// @}

    /// Constructors & destructors
    /// @{
    static void testConstructorSimple() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      CHECK_NOTHROW(slot = new PRMSlotChain("slot", *_booleanChain_));
      CHECK_NOTHROW(delete slot);
    }

    static void testSecondConstructorSimple() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      CHECK_NOTHROW(slot = new PRMSlotChain(_booleanChain_, "slot"));
      CHECK_NOTHROW(delete slot);
    }

    static void testConstructorComplex() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      CHECK_NOTHROW(slot = new PRMSlotChain("slot", *_stateChain_));
      CHECK_NOTHROW(delete slot);
    }

    static void testSecondConstructorComplex() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      CHECK_NOTHROW(slot = new PRMSlotChain(_stateChain_, "slot"));
      CHECK_NOTHROW(delete slot);
    }

    /// @}

    /// Methods
    /// @{
    static void testIsMultiple() {
      // Arrange
      PRMSlotChain simple("simple", *_booleanChain_);
      PRMSlotChain multiple("multiple", *_stateChain_);
      // Act & Assert
      CHECK(!simple.isMultiple());
      CHECK(multiple.isMultiple());
    }

    static void testLastEltSimple() {
      // Arrange
      PRMSlotChain                         simple("simple", *_booleanChain_);
      gum::prm::PRMClassElement< double >* elt = nullptr;
      // Act
      CHECK_NOTHROW(elt = &(simple.lastElt()));
      // Assert
      CHECK((_boolAttr_->safeName()) == (elt->safeName()));
    }

    static void testLastEltSimpleConst() {
      // Arrange
      PRMSlotChain                               simple("simple", *_booleanChain_);
      const auto&                                const_simple = simple;
      gum::prm::PRMClassElement< double > const* elt          = nullptr;
      // Act
      CHECK_NOTHROW(elt = &(const_simple.lastElt()));
      // Assert
      CHECK((_boolAttr_->safeName()) == (elt->safeName()));
    }

    static void testChain() {
      // Arrange
      PRMSlotChain simple("simple", *_booleanChain_);
      gum::Sequence< gum::prm::PRMClassElement< double >* > const* chain = nullptr;
      // Act
      CHECK_NOTHROW(chain = &(simple.chain()));
      // ASSERT
      CHECK((chain->size()) == (_booleanChain_->size()));
      CHECK((chain->atPos(0)) == (_booleanChain_->atPos(0)));
      CHECK((chain->atPos(1)) == (_booleanChain_->atPos(1)));
      CHECK((chain->atPos(2)) != (_booleanChain_->atPos(2)));
    }

    static void testCpf() {
      // Arrange
      PRMSlotChain          slot("simple", *_booleanChain_);
      gum::Tensor< double > expected;
      expected << slot.type().variable();
      // Act
      auto& actual = slot.cpf();
      // Assert
      gum::Instantiation i(expected);
      gum::Instantiation j(actual);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        CHECK((expected[i]) == (actual[j]));
      }
      CHECK(i.end());
      CHECK(j.end());
    }

    static void testCpfConst() {
      // Arrange
      PRMSlotChain          slot("simple", *_booleanChain_);
      const auto&           slot_const = slot;
      gum::Tensor< double > expected;
      expected << slot.type().variable();
      // Act
      const auto& actual = slot_const.cpf();
      // Assert
      gum::Instantiation i(expected);
      gum::Instantiation j(actual);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        CHECK((expected[i]) == (actual[j]));
      }
      CHECK(i.end());
      CHECK(j.end());
    }

    /// @}

    /// Add parents and children
    /// @{
    static void testAddParentCheckChild() {
      // Arrange
      gum::prm::PRMScalarAttribute< double > parent("attr", *_boolean_);
      PRMSlotChain                           child("child", *_booleanChain_);
      auto                                   before = child.cpf().variablesSequence().size();
      // Act
      CHECK_NOTHROW(child.addParent(parent));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      CHECK((before) == (after));
      CHECK(!child.cpf().contains(parent.type().variable()));
    }

    static void testAddParentDupplicateError() {
      // Arrange
      gum::prm::PRMScalarAttribute< double > parent("attr", *_boolean_);
      PRMSlotChain                           child("child", *_booleanChain_);
      child.addParent(parent);
      auto before = child.cpf().variablesSequence().size();
      // Act
      CHECK_NOTHROW(child.addParent(parent));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      CHECK((before) == (after));
      CHECK(!child.cpf().contains(parent.type().variable()));
    }

    static void testAddChild() {
      // Arrange
      PRMSlotChain                           parent("simple", *_booleanChain_);
      gum::prm::PRMScalarAttribute< double > child("attr", *_boolean_);
      auto                                   before = parent.cpf().variablesSequence().size();
      // Act
      CHECK_NOTHROW(parent.addChild(child));
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      CHECK((before) == (after));
      CHECK(!parent.cpf().contains(child.type().variable()));
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
  GUM_TEST_ACTIF(Cast)
  GUM_TEST_ACTIF(ConstructorSimple)
  GUM_TEST_ACTIF(SecondConstructorSimple)
  GUM_TEST_ACTIF(ConstructorComplex)
  GUM_TEST_ACTIF(SecondConstructorComplex)
  GUM_TEST_ACTIF(IsMultiple)
  GUM_TEST_ACTIF(LastEltSimple)
  GUM_TEST_ACTIF(LastEltSimpleConst)
  GUM_TEST_ACTIF(Chain)
  GUM_TEST_ACTIF(Cpf)
  GUM_TEST_ACTIF(CpfConst)
  GUM_TEST_ACTIF(AddParentCheckChild)
  GUM_TEST_ACTIF(AddParentDupplicateError)
  GUM_TEST_ACTIF(AddChild)

}   // namespace gum_tests
