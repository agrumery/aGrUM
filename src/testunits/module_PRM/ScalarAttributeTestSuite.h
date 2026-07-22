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


#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMScalarAttribute.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  struct ScalarAttributeTestSuite {
    using PRMAttribute = gum::prm::PRMScalarAttribute< double >;

    ClassElementTestSuiteAbstract* _classEltTestSuite_;
    gum::prm::PRMType*             _boolean_;
    gum::prm::PRMType*             _state_;

    public:
    /// PRMClassElement Tests
    /// @{
    ScalarAttributeTestSuite() {
      _classEltTestSuite_ = new ClassElementTestSuiteAbstract;
      _boolean_           = gum::prm::PRMType::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      _state_ = new gum::prm::PRMType{*_boolean_, map, state};
    }

    ~ScalarAttributeTestSuite() {
      delete _classEltTestSuite_;
      delete _boolean_;
      delete _state_;
    }

    /// @}

    /// Constructor & destructor
    /// @{


    /// }

    /// Getters & setters
    /// @{


    /// @}

    /// Add parents and children
    /// @{


    /// @}

    /// Get and set Cast Descendants
    /// @{


    /// @}
  };

  GUM_TEST(IsReferenceSlot) {
    // Arrange
    gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
    var.addLabel("False");
    var.addLabel("True");
    gum::prm::PRMType type{var};
    PRMAttribute      attr("my_attr", type);
    bool              expected = false;
    // Act & Assert
    _classEltTestSuite_->testIsReferenceSlot(attr, expected);
  }

  GUM_TEST(IsAttribute) {
    // Arrange
    gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
    var.addLabel("False");
    var.addLabel("True");
    gum::prm::PRMType type{var};
    PRMAttribute      attr("my_attr", type);
    bool              expected = true;
    // Act & Assert
    _classEltTestSuite_->testIsAttribute(attr, expected);
  }

  GUM_TEST(IsSlotChain) {
    // Arrange
    gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
    var.addLabel("False");
    var.addLabel("True");
    gum::prm::PRMType type{var};
    PRMAttribute      attr("my_attr", type);
    bool              expected = false;
    // Act & Assert
    _classEltTestSuite_->testIsSlotChain(attr, expected);
  }

  GUM_TEST(SetNodeId) {
    // Arrange
    gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
    var.addLabel("False");
    var.addLabel("True");
    gum::prm::PRMType type{var};
    PRMAttribute      attr("my_attr", type);
    // Act & Assert
    _classEltTestSuite_->testSetNodeId(attr);
  }

  GUM_TEST(ObjType) {
    // Arrange
    gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
    var.addLabel("False");
    var.addLabel("True");
    gum::prm::PRMType type{var};
    PRMAttribute      attr("my_attr", type);
    // Act & Assert
    _classEltTestSuite_->test_obj_type(attr);
  }

  GUM_TEST(SafeName) {
    // Arrange
    gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
    var.addLabel("False");
    var.addLabel("True");
    gum::prm::PRMType type{var};
    PRMAttribute      attr("my_attr", type);

    // Act & Assert
    _classEltTestSuite_->testSafeName(attr);
  }

  GUM_TEST(Cast_NotAllowed) {
    // Arrange
    gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
    var.addLabel("False");
    var.addLabel("True");
    gum::prm::PRMType type{var};
    PRMAttribute      attr("my_attr", type);

    // Act & Assert
    _classEltTestSuite_->testCast_NotAllowed(attr);
  }

  GUM_TEST(Cast) {
    // Arrange
    gum::LabelizedVariable boolean_var{"boolean", "A boolean discrete variable", 0};
    boolean_var.addLabel("False");
    boolean_var.addLabel("True");

    gum::LabelizedVariable state_var{"state", "A binary discete variable to represent a state", 0};
    state_var.addLabel("OK");
    state_var.addLabel("NOK");

    std::vector< gum::Idx > map;
    map.push_back(1);
    map.push_back(0);

    gum::prm::PRMType boolean{boolean_var};
    gum::prm::PRMType state{boolean, map, state_var};
    PRMAttribute      attr("my_attr", state);

    // Act & Assert
    _classEltTestSuite_->testCast(attr, boolean);
  }

  GUM_TEST(ClassConstructor) {
    // Arrange
    std::string   name = "my_state";
    PRMAttribute* attr = nullptr;
    // Act & Assert
    try {
      attr = new PRMAttribute(name, *_boolean_);
    } catch (...) {   // CHECK_NOTHROW does not work
      FAIL("Exception raised.");
    }
    CHECK_NOTHROW(delete attr);
  }

  GUM_TEST(Type) {
    // Arrange
    PRMAttribute attr("attr", *_boolean_);
    auto&        expected = *_boolean_;
    // Act
    auto& actual = attr.type();
    // Assert
    CHECK_EQ(expected, actual);
    CHECK_NE(&expected, &actual);
  }

  GUM_TEST(TypeConst) {
    // Arrange
    PRMAttribute attr("attr", *_boolean_);
    const auto&  attr_const = attr;
    const auto&  expected   = *_boolean_;
    // Act
    const auto& actual = attr_const.type();
    // Assert
    CHECK_EQ(expected, actual);
    CHECK_NE(&expected, &actual);
  }

  GUM_TEST(Cpf) {
    // Arrange
    PRMAttribute          attr("attr", *_boolean_);
    gum::Tensor< double > expected;
    expected << attr.type().variable();
    // Act
    auto& actual = attr.cpf();
    // Assert
    gum::Instantiation i(expected);
    gum::Instantiation j(actual);
    for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
      CHECK_EQ(expected[i], actual[j]);
    }
    CHECK(i.end());
    CHECK(j.end());
  }

  GUM_TEST(CpfConst) {
    // Arrange
    PRMAttribute          attr("attr", *_boolean_);
    const auto&           attr_const = attr;
    gum::Tensor< double > expected;
    expected << attr.type().variable();
    // Act
    const auto& actual = attr_const.cpf();
    // Assert
    gum::Instantiation i(expected);
    gum::Instantiation j(actual);
    for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
      CHECK_EQ(expected[i], actual[j]);
    }
    CHECK(i.end());
    CHECK(j.end());
  }

  GUM_TEST(AddParentCheckChild) {
    // Arrange
    PRMAttribute parent("attr", *_boolean_);
    PRMAttribute child("attr", *_boolean_);
    auto         before = child.cpf().variablesSequence().size();
    // Act
    CHECK_NOTHROW(child.addParent(parent));
    // Assert
    auto after = child.cpf().variablesSequence().size();
    CHECK_EQ(before + 1, after);
    CHECK(child.cpf().contains(parent.type().variable()));
  }

  GUM_TEST(AddParentDupplicateError) {
    // Arrange
    PRMAttribute parent("attr", *_boolean_);
    PRMAttribute child("attr", *_boolean_);
    child.addParent(parent);
    auto before = child.cpf().variablesSequence().size();
    // Act
    CHECK_THROWS_AS(child.addParent(parent), const gum::DuplicateElement&);
    // Assert
    auto after = child.cpf().variablesSequence().size();
    CHECK_EQ(before, after);
    CHECK(child.cpf().contains(parent.type().variable()));
  }

  GUM_TEST(AddChild) {
    // Arrange
    PRMAttribute parent("attr", *_boolean_);
    PRMAttribute child("attr", *_boolean_);
    auto         before = parent.cpf().variablesSequence().size();
    // Act
    CHECK_NOTHROW(parent.addChild(child));
    // Assert
    auto after = parent.cpf().variablesSequence().size();
    CHECK_EQ(before, after);
    CHECK(!parent.cpf().contains(child.type().variable()));
  }

  GUM_TEST(GetCastDescendant) {
    // Arrange
    PRMAttribute                      state("state", *_state_);
    gum::prm::PRMAttribute< double >* cast = nullptr;
    // Act
    CHECK_NOTHROW(cast = state.getCastDescendant());
    // Assert
    CHECK_NE(cast, nullptr);
    CHECK_EQ(cast->type(), *_boolean_);
    CHECK_NE(&(cast->type().variable()), &(_boolean_->variable()));
    CHECK(cast->cpf().contains(cast->type().variable()));
    CHECK(cast->cpf().contains(state.type().variable()));
    CHECK_NOTHROW(delete cast);
  }

  GUM_TEST(SetAsCastDescendant) {
    // Arrange
    PRMAttribute boolean("boolean", *_boolean_);
    PRMAttribute state("state", *_state_);
    auto         before = boolean.cpf().variablesSequence().size();
    // Act
    CHECK_NOTHROW(state.setAsCastDescendant(&boolean));
    // Assert
    auto after = boolean.cpf().variablesSequence().size();
    CHECK_EQ(before + 1, after);
  }

  GUM_TEST(SetAsCastDescendantOperationNotAllowed) {
    // Arrange
    PRMAttribute boolean("boolean", *_boolean_);
    PRMAttribute boolean_bis("boolean", *_boolean_);
    auto         before = boolean.cpf().variablesSequence().size();
    // Act
    CHECK_THROWS_AS(boolean_bis.setAsCastDescendant(&boolean), const gum::OperationNotAllowed&);
    // Assert
    auto after = boolean.cpf().variablesSequence().size();
    CHECK_EQ(before, after);
  }

  GUM_TEST(SetAsCastDescendantTypeError) {
    // Arrange
    gum::LabelizedVariable foovar{"Foo", "Bar", 5};
    PRMAttribute           foo("foobar", gum::prm::PRMType(foovar));
    PRMAttribute           state("state", *_state_);
    auto                   before = foo.cpf().variablesSequence().size();
    // Act
    CHECK_THROWS_AS(state.setAsCastDescendant(&foo), const gum::TypeError&);
    // Assert
    auto after = foo.cpf().variablesSequence().size();
    CHECK_EQ(before, after);
  }

}   // namespace gum_tests
