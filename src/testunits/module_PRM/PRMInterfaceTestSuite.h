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
#pragma once


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/PRM/elements/PRMClass.h>

/**
 * This class is used to test gum::prm::PRMInterface, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMInterface.
 */
namespace gum_tests {

  class GUM_TEST_SUITE(PRMInterface) {
    private:
    using PRMInterface = gum::prm::PRMInterface< double >;
    using PRMType      = gum::prm::PRMType;
    using PRMAttribute = gum::prm::PRMScalarAttribute< double >;
    using Reference    = gum::prm::PRMReferenceSlot< double >;
    using PRMSlotChain = gum::prm::PRMSlotChain< double >;

    PRMType* _boolean_;
    PRMType* _state_;

    public:
    void setUp() {
      _boolean_ = gum::prm::PRMType::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      _state_ = new PRMType(*_boolean_, map, state);
    }

    void tearDown() {
      delete _boolean_;
      delete _state_;
    }

    /// Constructor & Destructor
    /// @{
    GUM_ACTIVE_TEST(Constructor) {
      // Arrange
      PRMInterface* c = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(c = new PRMInterface("class"))
      TS_ASSERT_THROWS_NOTHING(delete c)
    }

    GUM_ACTIVE_TEST(ConstructorInheritance) {
      // Arrange
      PRMInterface toRef("toRef");
      auto         a = new PRMAttribute("a", *_boolean_);
      toRef.add(a);
      auto         ref = new Reference("rho", toRef);
      PRMInterface super("super");
      auto         b    = new PRMAttribute("b", *_boolean_);
      auto         b_id = super.add(b);
      auto         c    = new PRMAttribute("c", *_boolean_);
      auto         c_id = super.add(c);
      super.add(ref);
      PRMInterface* subclass = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(subclass = new PRMInterface("subclass", super))
      // Assert
      TS_ASSERT(subclass->exists("b"))
      TS_ASSERT_EQUALS(subclass->get(b_id).name(), "b")
      TS_ASSERT(subclass->exists("c"))
      TS_ASSERT_EQUALS(subclass->get(c_id).name(), "c")
      TS_ASSERT_EQUALS(subclass->attributes().size(), static_cast< gum::Size >(2))
      TS_ASSERT_EQUALS(subclass->referenceSlots().size(), static_cast< gum::Size >(1))
      delete subclass;
    }

    /// @}
    /// Belongs and exists tests
    /// @{
    GUM_ACTIVE_TEST(BelongsTo) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.belongsTo(*attr))
      // Assert
      TS_ASSERT(actual)
    }

    GUM_ACTIVE_TEST(BelongsToNot) {
      // Arrange
      PRMInterface c("class");
      PRMAttribute attr("attr", *_boolean_);
      bool         actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.belongsTo(attr))
      // Assert
      TS_ASSERT(!actual)
    }

    GUM_ACTIVE_TEST(Exists) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.exists("attr"))
      // Assert
      TS_ASSERT(actual)
    }

    GUM_ACTIVE_TEST(ExistsNot) {
      // Arrange
      PRMInterface c("class");
      bool         actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.exists("attr"))
      // Assert
      TS_ASSERT(!actual)
    }

    GUM_ACTIVE_TEST(Get) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      // Act
      auto& actual = c.get(attr->name());
      // Assert
      TS_ASSERT_EQUALS(&actual, attr)
    }

    GUM_ACTIVE_TEST(GetConst) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      const auto& const_c = c;
      // Act
      const auto& actual = const_c.get(attr->name());
      // Assert
      TS_ASSERT_EQUALS(&actual, attr)
    }

    GUM_ACTIVE_TEST(GetNotFound) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      // Act & Assert
      TS_ASSERT_THROWS(c.get("foo"), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(GetConstNotFound) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      const auto& const_c = c;
      // Act & Assert
      TS_ASSERT_THROWS(const_c.get("foo"), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(Add) {
      // Arra,ge
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      gum::NodeId   id   = 100;   // Id generation starts at 0
      // Act & assert
      TS_ASSERT_THROWS_NOTHING(id = c.add(attr))
      TS_ASSERT(c.exists(attr->name()))
      TS_ASSERT_EQUALS(&(c.get(attr->name())), attr)
      TS_ASSERT_EQUALS(id, attr->id())
      TS_ASSERT_DIFFERS(id, (gum::NodeId)100)
    }

    GUM_ACTIVE_TEST(AddDuplicate) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      // Act & assert
      TS_ASSERT_THROWS_NOTHING(c.add(attr))
      TS_ASSERT_THROWS(c.add(attr), const gum::DuplicateElement&)
      TS_ASSERT_EQUALS(c.attributes().size(), static_cast< gum::Size >(1))
    }

    /// @}
    /// Overloading
    /// @{

    GUM_ACTIVE_TEST(OverloadOperationNotAllowed) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      // Act & assert
      TS_ASSERT_THROWS(c.overload(attr), const gum::OperationNotAllowed&)
      // Cleanup
      delete attr;
    }

    GUM_ACTIVE_TEST(OverloadWrongInterface) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      PRMInterface sub_c("sub c", c);
      Reference*   ref = new Reference("attr", c);
      // Act & Assert
      TS_ASSERT_THROWS(sub_c.overload(ref), const gum::OperationNotAllowed&)
      // Cleanup
      delete ref;
    }

    GUM_ACTIVE_TEST(OverloadTypeError) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      PRMInterface           sub_c("sub_c", c);
      gum::LabelizedVariable var("foo", "bar", 2);
      gum::prm::PRMType      type(var);
      PRMAttribute*          bttr = new PRMAttribute("attr", type);
      // Act & Assert
      TS_ASSERT_THROWS(sub_c.overload(bttr), const gum::OperationNotAllowed&)
      // Cleanup
      delete bttr;
    }

    GUM_ACTIVE_TEST(OverloadAttribute) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      PRMInterface  sub_c("sub_c", c);
      PRMAttribute* sub_attr = new PRMAttribute("attr", *_boolean_);
      // Act
      TS_ASSERT_THROWS_NOTHING(sub_c.overload(sub_attr))
      // Assert
      TS_ASSERT(sub_c.exists(sub_attr->safeName()))
      TS_ASSERT(sub_c.exists(attr->safeName()))
      const auto& b = sub_c.get(attr->safeName());
      const auto& s = sub_c.get(sub_attr->safeName());
      TS_ASSERT_EQUALS(b.type(), s.type())
    }

    GUM_ACTIVE_TEST(OverloadAttributeWithSubtype) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      PRMInterface  sub_c("sub_c", c);
      PRMAttribute* state = new PRMAttribute("attr", *_state_);
      // Act
      TS_ASSERT_THROWS_NOTHING(sub_c.overload(state))
      // Assert
      TS_ASSERT(sub_c.exists(state->safeName()))
      TS_ASSERT(sub_c.exists(attr->safeName()))
      const auto& b = sub_c.get(attr->safeName());
      const auto& s = sub_c.get(state->safeName());
      TS_ASSERT_DIFFERS(b.type(), s.type())
    }

    GUM_ACTIVE_TEST(OverloadAttributeWithSeveralCastDescendants) {
      // Arrange
      int                     size = 10;
      std::vector< PRMType* > types;
      types.push_back(_boolean_);
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      for (int i = 1; i < size; i++) {
        auto&             super = *(types[i - 1]);
        std::stringstream sbuff;
        sbuff << "type_" << i;
        auto name = sbuff.str();
        auto var  = gum::LabelizedVariable(name, "", 2);
        auto t    = new PRMType(super, map, var);
        types.push_back(t);
      }
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *(types[0]));
      c.add(attr);
      PRMInterface  sub_c("sub_c", c);
      PRMAttribute* state = new PRMAttribute("attr", *(types[size - 1]));
      // Act
      TS_ASSERT_THROWS_NOTHING(sub_c.overload(state))
      // Assert
      for (int i = 0; i < size; i++) {
        std::string i_name = "(" + types[i]->name() + ")attr";
        TS_ASSERT(sub_c.exists(i_name))

        for (int j = i + 1; j < size; j++) {
          std::string j_name = "(" + types[j]->name() + ")attr";
          auto        i_attr = sub_c.get(i_name).type();
          auto        j_attr = sub_c.get(j_name).type();
          TS_ASSERT_DIFFERS(i_attr, j_attr)
        }
      }
      // Cleanup
      for (int i = 1; i < size; ++i) {
        delete types[i];
      }
    }

    GUM_ACTIVE_TEST(OverloadAttributeDuplicateElement) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      PRMInterface  sub_c("sub_c", c);
      PRMAttribute* state = new PRMAttribute("attr", *_state_);
      sub_c.overload(state);
      auto before = sub_c.attributes().size();
      // Act
      TS_ASSERT_THROWS(sub_c.overload(state), const gum::DuplicateElement&)
      // Assert
      auto after = sub_c.attributes().size();
      TS_ASSERT(sub_c.exists(attr->safeName()))
      TS_ASSERT(sub_c.exists(state->safeName()))
      TS_ASSERT_EQUALS(before, after)
    }

    GUM_ACTIVE_TEST(OverloadAttributeWithSeveralCastDescendantsDuplicate) {
      // Arrange
      int                     size = 10;
      std::vector< PRMType* > types;
      types.push_back(_boolean_);
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      for (int i = 1; i < size; i++) {
        auto&             super = *(types[i - 1]);
        std::stringstream sbuff;
        sbuff << "type_" << i;
        auto name = sbuff.str();
        auto var  = gum::LabelizedVariable(name, "", 2);
        auto t    = new PRMType(super, map, var);
        types.push_back(t);
      }
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *(types[0]));
      c.add(attr);
      PRMInterface  sub_c("sub_c", c);
      PRMAttribute* state = new PRMAttribute("attr", *(types[size - 1]));
      sub_c.overload(state);
      auto before = sub_c.attributes().size();
      // Act
      TS_ASSERT_THROWS(sub_c.overload(state), const gum::DuplicateElement&)
      // Assert
      auto after = sub_c.attributes().size();
      for (int i = 0; i < size; i++) {
        std::string i_name = "(" + types[i]->name() + ")attr";
        TS_ASSERT(sub_c.exists(i_name))

        for (int j = i + 1; j < size; j++) {
          std::string j_name = "(" + types[j]->name() + ")attr";
          auto        i_attr = sub_c.get(i_name).type();
          auto        j_attr = sub_c.get(j_name).type();
          TS_ASSERT_DIFFERS(i_attr, j_attr)
        }
      }
      TS_ASSERT_EQUALS(before, after)
      // Cleanup
      for (int i = 1; i < size; ++i) {
        delete types[i];
      }
    }

    GUM_ACTIVE_TEST(OverloadReference) {
      // Arrange
      PRMInterface c_1("c_1");
      PRMInterface c_2("c_2", c_1);
      PRMInterface c_3("c_3");
      Reference*   ref = new Reference("ref", c_1, false);
      c_3.add(ref);
      PRMInterface c_4("c_4", c_3);
      Reference*   sub_ref = new Reference("ref", c_2, false);
      // Act
      TS_ASSERT_THROWS_NOTHING(c_4.overload(sub_ref))
      // Assert
      TS_ASSERT(!c_4.exists(ref->safeName()))
      TS_ASSERT(c_4.exists(sub_ref->name()))
      TS_ASSERT(c_4.exists(sub_ref->safeName()))
      TS_ASSERT_EQUALS(c_4.referenceSlots().size(), static_cast< gum::Size >(1))
    }

    GUM_ACTIVE_TEST(OverloadReferenceIllegal) {
      // Arrange
      PRMInterface c_1("c_1");
      PRMInterface c_2("c_2", c_1);
      PRMInterface c_3("c_3");
      Reference*   ref = new Reference("ref", c_1, false);
      c_3.add(ref);
      PRMInterface c_4("c_4", c_3);
      Reference*   sub_ref = new Reference("ref", c_3);
      // Act
      TS_ASSERT_THROWS(c_4.overload(sub_ref), const gum::OperationNotAllowed&)
      // Assert
      TS_ASSERT(c_4.exists(ref->safeName()))
      TS_ASSERT(c_4.exists(ref->name()))
      TS_ASSERT(!c_4.exists(sub_ref->safeName()))
      TS_ASSERT_EQUALS(c_4.referenceSlots().size(), static_cast< gum::Size >(1))
      delete sub_ref;
    }

    /// @}
    /// Input, output and inner nodes methods.
    /// @{

    GUM_ACTIVE_TEST(IsInputNode) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.isInputNode(*a))
      // Assert
      TS_ASSERT(!actual)
    }

    GUM_ACTIVE_TEST(SetInputNode) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      bool before = c.isInputNode(*a);
      bool after  = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(c.setInputNode(*a, true))
      // Assert
      TS_ASSERT(after = c.isInputNode(*a))
      TS_ASSERT_DIFFERS(before, after)
      TS_ASSERT(after)
    }

    GUM_ACTIVE_TEST(IsOutputNode) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.isOutputNode(*a))
      // Assert
      TS_ASSERT(!actual)
    }

    GUM_ACTIVE_TEST(SetOutputNode) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      bool before = c.isOutputNode(*a);
      bool after  = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(c.setOutputNode(*a, true))
      // Assert
      TS_ASSERT(after = c.isOutputNode(*a))
      TS_ASSERT_DIFFERS(before, after)
      TS_ASSERT(after)
    }

    GUM_ACTIVE_TEST(IsInnerNode) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.isInnerNode(*a))
      // Assert
      TS_ASSERT(actual)
    }

    GUM_ACTIVE_TEST(InnerNodeConsistency) {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      // Act & Assert
      TS_ASSERT(c.isInnerNode(*a))
      TS_ASSERT(!c.isInputNode(*a))
      TS_ASSERT(!c.isOutputNode(*a))
      TS_ASSERT_THROWS_NOTHING(c.setInputNode(*a, true))
      TS_ASSERT(!c.isInnerNode(*a))
      TS_ASSERT(c.isInputNode(*a))
      TS_ASSERT(!c.isOutputNode(*a))
      TS_ASSERT_THROWS_NOTHING(c.setOutputNode(*a, true))
      TS_ASSERT(!c.isInnerNode(*a))
      TS_ASSERT(c.isInputNode(*a))
      TS_ASSERT(c.isOutputNode(*a))
      TS_ASSERT_THROWS_NOTHING(c.setInputNode(*a, false))
      TS_ASSERT_THROWS_NOTHING(c.setOutputNode(*a, false))
      TS_ASSERT(c.isInnerNode(*a))
      TS_ASSERT(!c.isInputNode(*a))
      TS_ASSERT(!c.isOutputNode(*a))
    }

    /// @}
    /// Get operator
  };

}   // namespace gum_tests
