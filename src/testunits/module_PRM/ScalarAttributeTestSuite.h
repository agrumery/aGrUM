/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMScalarAttribute.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class [[maybe_unused]] ScalarAttributeTestSuite: public CxxTest::TestSuite {
    private:
    using PRMAttribute = gum::prm::PRMScalarAttribute< double >;

    ClassElementTestSuiteAbstract* _classEltTestSuite_;
    gum::prm::PRMType*             _boolean_;
    gum::prm::PRMType*             _state_;

    public:
    void setUp() {
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

    void tearDown() {
      delete _classEltTestSuite_;
      delete _boolean_;
      delete _state_;
    }

    /// PRMClassElement Tests
    /// @{
    GUM_ACTIVE_TEST(IsReferenceSlot) {
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

    GUM_ACTIVE_TEST(IsAttribute) {
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

    GUM_ACTIVE_TEST(IsSlotChain) {
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

    GUM_ACTIVE_TEST(SetNodeId) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr("my_attr", type);
      // Act & Assert
      _classEltTestSuite_->testSetNodeId(attr);
    }

    GUM_ACTIVE_TEST(ObjType) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr("my_attr", type);
      // Act & Assert
      _classEltTestSuite_->test_obj_type(attr);
    }

    GUM_ACTIVE_TEST(SafeName) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr("my_attr", type);

      // Act & Assert
      _classEltTestSuite_->testSafeName(attr);
    }

    GUM_ACTIVE_TEST(Cast_NotAllowed) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr("my_attr", type);

      // Act & Assert
      _classEltTestSuite_->testCast_NotAllowed(attr);
    }

    GUM_ACTIVE_TEST(Cast) {
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
      PRMAttribute      attr("my_attr", state);

      // Act & Assert
      _classEltTestSuite_->testCast(attr, boolean);
    }

    /// @}

    /// Constructor & destructor
    /// @{
    GUM_ACTIVE_TEST(ClassConstructor) {
      // Arrange
      std::string   name = "my_state";
      PRMAttribute* attr = nullptr;
      // Act & Assert
      try {
        attr = new PRMAttribute(name, *_boolean_);
      } catch (...) {   // TS_ASSERT_THROWS_NOTHING does not work
        TS_FAIL("Exception raised.");
      }
      TS_ASSERT_THROWS_NOTHING(delete attr)
    }

    /// }

    /// Getters & setters
    /// @{
    GUM_ACTIVE_TEST(Type) {
      // Arrange
      PRMAttribute attr("attr", *_boolean_);
      auto&        expected = *_boolean_;
      // Act
      auto& actual = attr.type();
      // Assert
      TS_ASSERT_EQUALS(expected, actual)
      TS_ASSERT_DIFFERS(&expected, &actual)
    }

    GUM_ACTIVE_TEST(TypeConst) {
      // Arrange
      PRMAttribute attr("attr", *_boolean_);
      const auto&  attr_const = attr;
      const auto&  expected   = *_boolean_;
      // Act
      const auto& actual = attr_const.type();
      // Assert
      TS_ASSERT_EQUALS(expected, actual)
      TS_ASSERT_DIFFERS(&expected, &actual)
    }

    GUM_ACTIVE_TEST(Cpf) {
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
        TS_ASSERT_EQUALS(expected[i], actual[j])
      }
      TS_ASSERT(i.end())
      TS_ASSERT(j.end())
    }

    GUM_ACTIVE_TEST(CpfConst) {
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
        TS_ASSERT_EQUALS(expected[i], actual[j])
      }
      TS_ASSERT(i.end())
      TS_ASSERT(j.end())
    }

    /// @}

    /// Add parents and children
    /// @{
    GUM_ACTIVE_TEST(AddParentCheckChild) {
      // Arrange
      PRMAttribute parent("attr", *_boolean_);
      PRMAttribute child("attr", *_boolean_);
      auto         before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent))
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before + 1, after)
      TS_ASSERT(child.cpf().contains(parent.type().variable()))
    }

    GUM_ACTIVE_TEST(AddParentDupplicateError) {
      // Arrange
      PRMAttribute parent("attr", *_boolean_);
      PRMAttribute child("attr", *_boolean_);
      child.addParent(parent);
      auto before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS(child.addParent(parent), const gum::DuplicateElement&)
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
      TS_ASSERT(child.cpf().contains(parent.type().variable()))
    }

    GUM_ACTIVE_TEST(AddChild) {
      // Arrange
      PRMAttribute parent("attr", *_boolean_);
      PRMAttribute child("attr", *_boolean_);
      auto         before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(parent.addChild(child))
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
      TS_ASSERT(!parent.cpf().contains(child.type().variable()))
    }

    /// @}

    /// Get and set Cast Descendants
    /// @{
    GUM_ACTIVE_TEST(GetCastDescendant) {
      // Arrange
      PRMAttribute                      state("state", *_state_);
      gum::prm::PRMAttribute< double >* cast = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cast = state.getCastDescendant())
      // Assert
      TS_ASSERT_DIFFERS(cast, nullptr)
      TS_ASSERT_EQUALS(cast->type(), *_boolean_)
      TS_ASSERT_DIFFERS(&(cast->type().variable()), &(_boolean_->variable()))
      TS_ASSERT(cast->cpf().contains(cast->type().variable()))
      TS_ASSERT(cast->cpf().contains(state.type().variable()))
      TS_ASSERT_THROWS_NOTHING(delete cast)
    }

    GUM_ACTIVE_TEST(SetAsCastDescendant) {
      // Arrange
      PRMAttribute boolean("boolean", *_boolean_);
      PRMAttribute state("state", *_state_);
      auto         before = boolean.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(state.setAsCastDescendant(&boolean))
      // Assert
      auto after = boolean.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before + 1, after)
    }

    GUM_ACTIVE_TEST(SetAsCastDescendantOperationNotAllowed) {
      // Arrange
      PRMAttribute boolean("boolean", *_boolean_);
      PRMAttribute boolean_bis("boolean", *_boolean_);
      auto         before = boolean.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS(boolean_bis.setAsCastDescendant(&boolean), const gum::OperationNotAllowed&)
      // Assert
      auto after = boolean.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
    }

    GUM_ACTIVE_TEST(SetAsCastDescendantTypeError) {
      // Arrange
      gum::LabelizedVariable foovar{"Foo", "Bar", 5};
      PRMAttribute           foo("foobar", gum::prm::PRMType(foovar));
      PRMAttribute           state("state", *_state_);
      auto                   before = foo.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS(state.setAsCastDescendant(&foo), const gum::TypeError&)
      // Assert
      auto after = foo.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
    }

    /// @}
  };

}   // namespace gum_tests
