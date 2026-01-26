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

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMFormAttribute.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  FormAttribute
#define GUM_CURRENT_MODULE PRM

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  struct FormAttributeTestSuite {
    private:
    using PRMAttribute = gum::prm::PRMFormAttribute< double >;

    ClassElementTestSuiteAbstract* _classEltTestSuite_;
    gum::prm::PRMType*             _boolean_;
    gum::prm::PRMType*             _state_;
    gum::prm::PRMClass< double >*  _class_;

    public:
    /// PRMClassElement Tests
    /// @{
    FormAttributeTestSuite() {
      _classEltTestSuite_ = new ClassElementTestSuiteAbstract;
      _boolean_           = gum::prm::PRMType::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      _state_ = new gum::prm::PRMType{*_boolean_, map, state};
      _class_ = new gum::prm::PRMClass< double >("dummy");
    }

    ~FormAttributeTestSuite() {
      delete _classEltTestSuite_;
      delete _boolean_;
      delete _state_;
      delete _class_;
    }

    static void testIsReferenceSlot() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);
      bool              expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsReferenceSlot(attr, expected);
    }

    static void testIsAttribute() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);
      bool              expected = true;
      // Act & Assert
      _classEltTestSuite_->testIsAttribute(attr, expected);
    }

    static void testIsSlotChain() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);
      bool              expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsSlotChain(attr, expected);
    }

    static void testSetNodeId() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);
      // Act & Assert
      _classEltTestSuite_->testSetNodeId(attr);
    }

    static void testObjType() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);
      // Act & Assert
      _classEltTestSuite_->test_obj_type(attr);
    }

    static void testSafeName() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);

      // Act & Assert
      _classEltTestSuite_->testSafeName(attr);
    }

    static void testCast_NotAllowed() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);

      // Act & Assert
      _classEltTestSuite_->testCast_NotAllowed(attr);
    }

    static void testCast() {
      // Arrange
      gum::LabelizedVariable boolean_var{"boolean", "A boolean discrete variable", 0};
      boolean_var.addLabel("False");
      boolean_var.addLabel("True");

      gum::LabelizedVariable state_var{"state",
                                       "A binary discete variable to represent a state",
                                       0};
      state_var.addLabel("OK");
      state_var.addLabel("NOK");

      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);

      gum::prm::PRMType boolean{boolean_var};
      gum::prm::PRMType state{boolean, map, state_var};
      PRMAttribute      attr(*_class_, "my_attr", state);

      // Act & Assert
      _classEltTestSuite_->testCast(attr, boolean);
    }

    /// @}

    /// Constructor & destructor
    /// @{
    static void testClassConstructor() {
      // Arrange
      std::string   name = "my_state";
      PRMAttribute* attr = nullptr;
      // Act & Assert
      try {
        attr = new PRMAttribute(*_class_, name, *_boolean_);
      } catch (...) {   // CHECK_NOTHROW does not work
        FAIL("Exception raised.");
      }
      CHECK_NOTHROW(delete attr);
    }

    /// }

    /// Getters & setters
    /// @{
    static void testType() {
      // Arrange
      PRMAttribute attr(*_class_, "attr", *_boolean_);
      auto&        expected = *_boolean_;
      // Act
      auto& actual = attr.type();
      // Assert
      CHECK((expected) == (actual));
      CHECK((&expected) != (&actual));
    }

    static void testTypeConst() {
      // Arrange
      PRMAttribute attr(*_class_, "attr", *_boolean_);
      const auto&  attr_const = attr;
      const auto&  expected   = *_boolean_;
      // Act
      const auto& actual = attr_const.type();
      // Assert
      CHECK((expected) == (actual));
      CHECK((&expected) != (&actual));
    }

    static void testCpf() {
      // Arrange
      PRMAttribute          attr(*_class_, "attr", *_boolean_);
      gum::Tensor< double > expected;
      expected << attr.type().variable();
      // Act
      auto& actual = attr.cpf();
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
      PRMAttribute          attr(*_class_, "attr", *_boolean_);
      const auto&           attr_const = attr;
      gum::Tensor< double > expected;
      expected << attr.type().variable();
      // Act
      const auto& actual = attr_const.cpf();
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
      PRMAttribute parent(*_class_, "attr", *_boolean_);
      PRMAttribute child(*_class_, "attr", *_boolean_);
      auto         before = child.cpf().variablesSequence().size();
      // Act
      CHECK_NOTHROW(child.addParent(parent));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      CHECK((before + 1) == (after));
      CHECK(child.cpf().contains(parent.type().variable()));
    }

    static void testAddParentDupplicateError() {
      // Arrange
      PRMAttribute parent(*_class_, "attr", *_boolean_);
      PRMAttribute child(*_class_, "attr", *_boolean_);
      child.addParent(parent);
      auto before = child.cpf().variablesSequence().size();
      // Act
      CHECK_THROWS_AS(child.addParent(parent), const gum::DuplicateElement&);
      // Assert
      auto after = child.cpf().variablesSequence().size();
      CHECK((before) == (after));
      CHECK(child.cpf().contains(parent.type().variable()));
    }

    static void testAddChild() {
      // Arrange
      PRMAttribute parent(*_class_, "attr", *_boolean_);
      PRMAttribute child(*_class_, "attr", *_boolean_);
      auto         before = parent.cpf().variablesSequence().size();
      // Act
      CHECK_NOTHROW(parent.addChild(child));
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      CHECK((before) == (after));
      CHECK(!parent.cpf().contains(child.type().variable()));
    }

    /// @}

    /// Get and set Cast Descendants
    /// @{
    static void testGetCastDescendant() {
      try {
        // Arrange
        PRMAttribute                      state(*_class_, "state", *_state_);
        gum::prm::PRMAttribute< double >* cast = nullptr;
        // Act
        CHECK_NOTHROW(cast = state.getCastDescendant());
        // Assert
        CHECK((cast) != (nullptr));
        if (cast != nullptr) {
          CHECK((cast->type()) == (*_boolean_));
          CHECK((&(cast->type().variable())) != (&(_boolean_->variable())));
          CHECK(cast->cpf().contains(cast->type().variable()));
          CHECK(cast->cpf().contains(state.type().variable()));
          CHECK_NOTHROW(delete cast);
        }
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK_NOTHROW(throw e);
      }
    }

    static void testSetAsCastDescendant() {
      // Arrange
      PRMAttribute boolean(*_class_, "boolean", *_boolean_);
      PRMAttribute state(*_class_, "state", *_state_);
      auto         before = boolean.cpf().variablesSequence().size();
      // Act
      CHECK_NOTHROW(state.setAsCastDescendant(&boolean));
      // Assert
      auto after = boolean.cpf().variablesSequence().size();
      CHECK((before + 1) == (after));
    }

    static void testSetAsCastDescendantOperationNotAllowed() {
      // Arrange
      PRMAttribute boolean(*_class_, "boolean", *_boolean_);
      PRMAttribute boolean_bis(*_class_, "boolean", *_boolean_);
      auto         before = boolean.cpf().variablesSequence().size();
      // Act
      CHECK_THROWS_AS(boolean_bis.setAsCastDescendant(&boolean), const gum::OperationNotAllowed&);
      // Assert
      auto after = boolean.cpf().variablesSequence().size();
      CHECK((before) == (after));
    }

    static void testSetAsCastDescendantTypeError() {
      // Arrange
      gum::LabelizedVariable foovar{"Foo", "Bar", 5};
      PRMAttribute           foo(*_class_, "foobar", gum::prm::PRMType(foovar));
      PRMAttribute           state(*_class_, "state", *_state_);
      auto                   before = foo.cpf().variablesSequence().size();
      // Act
      CHECK_THROWS_AS(state.setAsCastDescendant(&foo), const gum::TypeError&);
      // Assert
      auto after = foo.cpf().variablesSequence().size();
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
  GUM_TEST_ACTIF(Cast)
  GUM_TEST_ACTIF(ClassConstructor)
  GUM_TEST_ACTIF(Type)
  GUM_TEST_ACTIF(TypeConst)
  GUM_TEST_ACTIF(Cpf)
  GUM_TEST_ACTIF(CpfConst)
  GUM_TEST_ACTIF(AddParentCheckChild)
  GUM_TEST_ACTIF(AddParentDupplicateError)
  GUM_TEST_ACTIF(AddChild)
  GUM_TEST_ACTIF(GetCastDescendant)
  GUM_TEST_ACTIF(SetAsCastDescendant)
  GUM_TEST_ACTIF(SetAsCastDescendantOperationNotAllowed)
  GUM_TEST_ACTIF(SetAsCastDescendantTypeError)

}   // namespace gum_tests
