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


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/PRM/elements/PRMClass.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  PRMInterface
#define GUM_CURRENT_MODULE PRM

/**
 * This class is used to test gum::prm::PRMInterface, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMInterface.
 */
namespace gum_tests {

  struct PRMInterfaceTestSuite {
    private:
    using PRMInterface = gum::prm::PRMInterface< double >;
    using PRMType      = gum::prm::PRMType;
    using PRMAttribute = gum::prm::PRMScalarAttribute< double >;
    using Reference    = gum::prm::PRMReferenceSlot< double >;
    using PRMSlotChain = gum::prm::PRMSlotChain< double >;

    PRMType* _boolean_;
    PRMType* _state_;

    public:
    /// Constructor & Destructor
    /// @{
    PRMInterfaceTestSuite() {
      _boolean_ = gum::prm::PRMType::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      _state_ = new PRMType(*_boolean_, map, state);
    }

    ~PRMInterfaceTestSuite() {
      delete _boolean_;
      delete _state_;
    }

    static void testConstructor() {
      // Arrange
      PRMInterface* c = nullptr;
      // Act & Assert
      CHECK_NOTHROW(c = new PRMInterface("class"));
      CHECK_NOTHROW(delete c);
    }

    static void testConstructorInheritance() {
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
      CHECK_NOTHROW(subclass = new PRMInterface("subclass", super));
      // Assert
      CHECK(subclass->exists("b"));
      CHECK((subclass->get(b_id).name()) == ("b"));
      CHECK(subclass->exists("c"));
      CHECK((subclass->get(c_id).name()) == ("c"));
      CHECK((subclass->attributes().size()) == (static_cast< gum::Size >(2)));
      CHECK((subclass->referenceSlots().size()) == (static_cast< gum::Size >(1)));
      delete subclass;
    }

    /// @}
    /// Belongs and exists tests
    /// @{
    static void testBelongsTo() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      bool actual = false;
      // Act
      CHECK_NOTHROW(actual = c.belongsTo(*attr));
      // Assert
      CHECK(actual);
    }

    static void testBelongsToNot() {
      // Arrange
      PRMInterface c("class");
      PRMAttribute attr("attr", *_boolean_);
      bool         actual = false;
      // Act
      CHECK_NOTHROW(actual = c.belongsTo(attr));
      // Assert
      CHECK(!actual);
    }

    static void testExists() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      bool actual = false;
      // Act
      CHECK_NOTHROW(actual = c.exists("attr"));
      // Assert
      CHECK(actual);
    }

    static void testExistsNot() {
      // Arrange
      PRMInterface c("class");
      bool         actual = false;
      // Act
      CHECK_NOTHROW(actual = c.exists("attr"));
      // Assert
      CHECK(!actual);
    }

    static void testGet() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      // Act
      auto& actual = c.get(attr->name());
      // Assert
      CHECK((&actual) == (attr));
    }

    static void testGetConst() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      const auto& const_c = c;
      // Act
      const auto& actual = const_c.get(attr->name());
      // Assert
      CHECK((&actual) == (attr));
    }

    static void testGetNotFound() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      // Act & Assert
      CHECK_THROWS_AS(c.get("foo"), const gum::NotFound&);
    }

    static void testGetConstNotFound() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      const auto& const_c = c;
      // Act & Assert
      CHECK_THROWS_AS(const_c.get("foo"), const gum::NotFound&);
    }

    static void testAdd() {
      // Arra,ge
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      gum::NodeId   id   = 100;   // Id generation starts at 0
      // Act & assert
      CHECK_NOTHROW(id = c.add(attr));
      CHECK(c.exists(attr->name()));
      CHECK((&(c.get(attr->name()))) == (attr));
      CHECK((id) == (attr->id()));
      CHECK((id) != ((gum::NodeId)100));
    }

    static void testAddDuplicate() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      // Act & assert
      CHECK_NOTHROW(c.add(attr));
      CHECK_THROWS_AS(c.add(attr), const gum::DuplicateElement&);
      CHECK((c.attributes().size()) == (static_cast< gum::Size >(1)));
    }

    /// @}
    /// Overloading
    /// @{

    static void testOverloadOperationNotAllowed() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      // Act & assert
      CHECK_THROWS_AS(c.overload(attr), const gum::OperationNotAllowed&);
      // Cleanup
      delete attr;
    }

    static void testOverloadWrongInterface() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      PRMInterface sub_c("sub c", c);
      Reference*   ref = new Reference("attr", c);
      // Act & Assert
      CHECK_THROWS_AS(sub_c.overload(ref), const gum::OperationNotAllowed&);
      // Cleanup
      delete ref;
    }

    static void testOverloadTypeError() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      PRMInterface           sub_c("sub_c", c);
      gum::LabelizedVariable var("foo", "bar", 2);
      gum::prm::PRMType      type(var);
      PRMAttribute*          bttr = new PRMAttribute("attr", type);
      // Act & Assert
      CHECK_THROWS_AS(sub_c.overload(bttr), const gum::OperationNotAllowed&);
      // Cleanup
      delete bttr;
    }

    static void testOverloadAttribute() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      PRMInterface  sub_c("sub_c", c);
      PRMAttribute* sub_attr = new PRMAttribute("attr", *_boolean_);
      // Act
      CHECK_NOTHROW(sub_c.overload(sub_attr));
      // Assert
      CHECK(sub_c.exists(sub_attr->safeName()));
      CHECK(sub_c.exists(attr->safeName()));
      const auto& b = sub_c.get(attr->safeName());
      const auto& s = sub_c.get(sub_attr->safeName());
      CHECK((b.type()) == (s.type()));
    }

    static void testOverloadAttributeWithSubtype() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      PRMInterface  sub_c("sub_c", c);
      PRMAttribute* state = new PRMAttribute("attr", *_state_);
      // Act
      CHECK_NOTHROW(sub_c.overload(state));
      // Assert
      CHECK(sub_c.exists(state->safeName()));
      CHECK(sub_c.exists(attr->safeName()));
      const auto& b = sub_c.get(attr->safeName());
      const auto& s = sub_c.get(state->safeName());
      CHECK((b.type()) != (s.type()));
    }

    static void testOverloadAttributeWithSeveralCastDescendants() {
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
      CHECK_NOTHROW(sub_c.overload(state));
      // Assert
      for (int i = 0; i < size; i++) {
        std::string i_name = "(" + types[i]->name() + ")attr";
        CHECK(sub_c.exists(i_name));

        for (int j = i + 1; j < size; j++) {
          std::string j_name = "(" + types[j]->name() + ")attr";
          auto        i_attr = sub_c.get(i_name).type();
          auto        j_attr = sub_c.get(j_name).type();
          CHECK((i_attr) != (j_attr));
        }
      }
      // Cleanup
      for (int i = 1; i < size; ++i) {
        delete types[i];
      }
    }

    static void testOverloadAttributeDuplicateElement() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *_boolean_);
      c.add(attr);
      PRMInterface  sub_c("sub_c", c);
      PRMAttribute* state = new PRMAttribute("attr", *_state_);
      sub_c.overload(state);
      auto before = sub_c.attributes().size();
      // Act
      CHECK_THROWS_AS(sub_c.overload(state), const gum::DuplicateElement&);
      // Assert
      auto after = sub_c.attributes().size();
      CHECK(sub_c.exists(attr->safeName()));
      CHECK(sub_c.exists(state->safeName()));
      CHECK((before) == (after));
    }

    static void testOverloadAttributeWithSeveralCastDescendantsDuplicate() {
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
      CHECK_THROWS_AS(sub_c.overload(state), const gum::DuplicateElement&);
      // Assert
      auto after = sub_c.attributes().size();
      for (int i = 0; i < size; i++) {
        std::string i_name = "(" + types[i]->name() + ")attr";
        CHECK(sub_c.exists(i_name));

        for (int j = i + 1; j < size; j++) {
          std::string j_name = "(" + types[j]->name() + ")attr";
          auto        i_attr = sub_c.get(i_name).type();
          auto        j_attr = sub_c.get(j_name).type();
          CHECK((i_attr) != (j_attr));
        }
      }
      CHECK((before) == (after));
      // Cleanup
      for (int i = 1; i < size; ++i) {
        delete types[i];
      }
    }

    static void testOverloadReference() {
      // Arrange
      PRMInterface c_1("c_1");
      PRMInterface c_2("c_2", c_1);
      PRMInterface c_3("c_3");
      Reference*   ref = new Reference("ref", c_1, false);
      c_3.add(ref);
      PRMInterface c_4("c_4", c_3);
      Reference*   sub_ref = new Reference("ref", c_2, false);
      // Act
      CHECK_NOTHROW(c_4.overload(sub_ref));
      // Assert
      CHECK(!c_4.exists(ref->safeName()));
      CHECK(c_4.exists(sub_ref->name()));
      CHECK(c_4.exists(sub_ref->safeName()));
      CHECK((c_4.referenceSlots().size()) == (static_cast< gum::Size >(1)));
    }

    static void testOverloadReferenceIllegal() {
      // Arrange
      PRMInterface c_1("c_1");
      PRMInterface c_2("c_2", c_1);
      PRMInterface c_3("c_3");
      Reference*   ref = new Reference("ref", c_1, false);
      c_3.add(ref);
      PRMInterface c_4("c_4", c_3);
      Reference*   sub_ref = new Reference("ref", c_3);
      // Act
      CHECK_THROWS_AS(c_4.overload(sub_ref), const gum::OperationNotAllowed&);
      // Assert
      CHECK(c_4.exists(ref->safeName()));
      CHECK(c_4.exists(ref->name()));
      CHECK(!c_4.exists(sub_ref->safeName()));
      CHECK((c_4.referenceSlots().size()) == (static_cast< gum::Size >(1)));
      delete sub_ref;
    }

    /// @}
    /// Input, output and inner nodes methods.
    /// @{

    static void testIsInputNode() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      bool actual = false;
      // Act
      CHECK_NOTHROW(actual = c.isInputNode(*a));
      // Assert
      CHECK(!actual);
    }

    static void testSetInputNode() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      bool before = c.isInputNode(*a);
      bool after  = false;
      // Act
      CHECK_NOTHROW(c.setInputNode(*a, true));
      // Assert
      CHECK(after = c.isInputNode(*a));
      CHECK((before) != (after));
      CHECK(after);
    }

    static void testIsOutputNode() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      bool actual = false;
      // Act
      CHECK_NOTHROW(actual = c.isOutputNode(*a));
      // Assert
      CHECK(!actual);
    }

    static void testSetOutputNode() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      bool before = c.isOutputNode(*a);
      bool after  = false;
      // Act
      CHECK_NOTHROW(c.setOutputNode(*a, true));
      // Assert
      CHECK(after = c.isOutputNode(*a));
      CHECK((before) != (after));
      CHECK(after);
    }

    static void testIsInnerNode() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      bool actual = false;
      // Act
      CHECK_NOTHROW(actual = c.isInnerNode(*a));
      // Assert
      CHECK(actual);
    }

    static void testInnerNodeConsistency() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *_boolean_);
      c.add(a);
      // Act & Assert
      CHECK(c.isInnerNode(*a));
      CHECK(!c.isInputNode(*a));
      CHECK(!c.isOutputNode(*a));
      CHECK_NOTHROW(c.setInputNode(*a, true));
      CHECK(!c.isInnerNode(*a));
      CHECK(c.isInputNode(*a));
      CHECK(!c.isOutputNode(*a));
      CHECK_NOTHROW(c.setOutputNode(*a, true));
      CHECK(!c.isInnerNode(*a));
      CHECK(c.isInputNode(*a));
      CHECK(c.isOutputNode(*a));
      CHECK_NOTHROW(c.setInputNode(*a, false));
      CHECK_NOTHROW(c.setOutputNode(*a, false));
      CHECK(c.isInnerNode(*a));
      CHECK(!c.isInputNode(*a));
      CHECK(!c.isOutputNode(*a));
    }

    /// @}
    /// Get operator
  };

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(ConstructorInheritance)
  GUM_TEST_ACTIF(BelongsTo)
  GUM_TEST_ACTIF(BelongsToNot)
  GUM_TEST_ACTIF(Exists)
  GUM_TEST_ACTIF(ExistsNot)
  GUM_TEST_ACTIF(Get)
  GUM_TEST_ACTIF(GetConst)
  GUM_TEST_ACTIF(GetNotFound)
  GUM_TEST_ACTIF(GetConstNotFound)
  GUM_TEST_ACTIF(Add)
  GUM_TEST_ACTIF(AddDuplicate)
  GUM_TEST_ACTIF(OverloadOperationNotAllowed)
  GUM_TEST_ACTIF(OverloadWrongInterface)
  GUM_TEST_ACTIF(OverloadTypeError)
  GUM_TEST_ACTIF(OverloadAttribute)
  GUM_TEST_ACTIF(OverloadAttributeWithSubtype)
  GUM_TEST_ACTIF(OverloadAttributeWithSeveralCastDescendants)
  GUM_TEST_ACTIF(OverloadAttributeDuplicateElement)
  GUM_TEST_ACTIF(OverloadAttributeWithSeveralCastDescendantsDuplicate)
  GUM_TEST_ACTIF(OverloadReference)
  GUM_TEST_ACTIF(OverloadReferenceIllegal)
  GUM_TEST_ACTIF(IsInputNode)
  GUM_TEST_ACTIF(SetInputNode)
  GUM_TEST_ACTIF(IsOutputNode)
  GUM_TEST_ACTIF(SetOutputNode)
  GUM_TEST_ACTIF(IsInnerNode)
  GUM_TEST_ACTIF(InnerNodeConsistency)

}   // namespace gum_tests
