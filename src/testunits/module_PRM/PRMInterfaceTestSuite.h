/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

#include <sstream>

#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/tools/variables/labelizedVariable.h>

/**
 * This class is used to test gum::prm::PRMInterface, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMInterface.
 */
namespace gum_tests {

  class PRMInterfaceTestSuite: public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMInterface< double >       PRMInterface;
    typedef gum::prm::PRMType                      PRMType;
    typedef gum::prm::PRMScalarAttribute< double > PRMAttribute;
    typedef gum::prm::PRMReferenceSlot< double >   Reference;
    typedef gum::prm::PRMSlotChain< double >       PRMSlotChain;

    PRMType* boolean__;
    PRMType* state__;

    public:
    void setUp() {
      boolean__ = gum::prm::PRMType::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      state__ = new PRMType(*boolean__, map, state);
    }

    void tearDown() {
      delete boolean__;
      delete state__;
    }

    /// Constructor & Destructor
    /// @{
    void testConstructor() {
      // Arrange
      PRMInterface* c = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(c = new PRMInterface("class"));
      TS_ASSERT_THROWS_NOTHING(delete c);
    }

    void testConstructorInheritance() {
      // Arrange
      PRMInterface toRef("toRef");
      auto         a = new PRMAttribute("a", *boolean__);
      toRef.add(a);
      auto         ref = new Reference("rho", toRef);
      PRMInterface super("super");
      auto         b    = new PRMAttribute("b", *boolean__);
      auto         b_id = super.add(b);
      auto         c    = new PRMAttribute("c", *boolean__);
      auto         c_id = super.add(c);
      super.add(ref);
      PRMInterface* subclass = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(subclass = new PRMInterface("subclass", super));
      // Assert
      TS_ASSERT(subclass->exists("b"));
      TS_ASSERT_EQUALS(subclass->get(b_id).name(), "b");
      TS_ASSERT(subclass->exists("c"));
      TS_ASSERT_EQUALS(subclass->get(c_id).name(), "c");
      TS_ASSERT_EQUALS(subclass->attributes().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(subclass->referenceSlots().size(), (gum::Size)1);
      delete subclass;
    }
    /// @}
    /// Belongs and exists tests
    /// @{
    void testBelongsTo() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      c.add(attr);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.belongsTo(*attr));
      // Assert
      TS_ASSERT(actual);
    }

    void testBelongsToNot() {
      // Arrange
      PRMInterface c("class");
      PRMAttribute attr("attr", *boolean__);
      bool         actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.belongsTo(attr));
      // Assert
      TS_ASSERT(!actual);
    }

    void testExists() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      c.add(attr);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.exists("attr"));
      // Assert
      TS_ASSERT(actual);
    }

    void testExistsNot() {
      // Arrange
      PRMInterface c("class");
      bool         actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.exists("attr"));
      // Assert
      TS_ASSERT(!actual);
    }

    void testGet() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      c.add(attr);
      // Act
      auto& actual = c.get(attr->name());
      // Assert
      TS_ASSERT_EQUALS(&actual, attr);
    }

    void testGetConst() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      c.add(attr);
      const auto& const_c = c;
      // Act
      const auto& actual = const_c.get(attr->name());
      // Assert
      TS_ASSERT_EQUALS(&actual, attr);
    }

    void testGetNotFound() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      c.add(attr);
      // Act & Assert
      TS_ASSERT_THROWS(c.get("foo"), gum::NotFound);
    }

    void testGetConstNotFound() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      c.add(attr);
      const auto& const_c = c;
      // Act & Assert
      TS_ASSERT_THROWS(const_c.get("foo"), gum::NotFound);
    }

    void testAdd() {
      // Arra,ge
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      gum::NodeId   id   = 100;   // Id generation starts at 0
      // Act & assert
      TS_ASSERT_THROWS_NOTHING(id = c.add(attr));
      TS_ASSERT(c.exists(attr->name()));
      TS_ASSERT_EQUALS(&(c.get(attr->name())), attr);
      TS_ASSERT_EQUALS(id, attr->id());
      TS_ASSERT_DIFFERS(id, (gum::NodeId)100);
    }

    void testAddDuplicate() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      // Act & assert
      TS_ASSERT_THROWS_NOTHING(c.add(attr));
      TS_ASSERT_THROWS(c.add(attr), gum::DuplicateElement);
      TS_ASSERT_EQUALS(c.attributes().size(), (gum::Size)1);
    }

    /// @}
    /// Overloading
    /// @{

    void testOverloadOperationNotAllowed() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      // Act & assert
      TS_ASSERT_THROWS(c.overload(attr), gum::OperationNotAllowed);
      // Cleanup
      delete attr;
    }

    void testOverloadWrongInterface() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      c.add(attr);
      PRMInterface sub_c("sub c", c);
      Reference*   ref = new Reference("attr", c);
      // Act & Assert
      TS_ASSERT_THROWS(sub_c.overload(ref), gum::OperationNotAllowed);
      // Cleanup
      delete ref;
    }

    void testOverloadTypeError() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      c.add(attr);
      PRMInterface           sub_c("sub_c", c);
      gum::LabelizedVariable var("foo", "bar", 2);
      gum::prm::PRMType      type(var);
      PRMAttribute*          bttr = new PRMAttribute("attr", type);
      // Act & Assert
      TS_ASSERT_THROWS(sub_c.overload(bttr), gum::OperationNotAllowed);
      // Cleanup
      delete bttr;
    }

    void testOverloadAttribute() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      c.add(attr);
      PRMInterface  sub_c("sub_c", c);
      PRMAttribute* sub_attr = new PRMAttribute("attr", *boolean__);
      // Act
      TS_ASSERT_THROWS_NOTHING(sub_c.overload(sub_attr));
      // Assert
      TS_ASSERT(sub_c.exists(sub_attr->safeName()));
      TS_ASSERT(sub_c.exists(attr->safeName()));
      const auto& b = sub_c.get(attr->safeName());
      const auto& s = sub_c.get(sub_attr->safeName());
      TS_ASSERT_EQUALS(b.type(), s.type());
    }

    void testOverloadAttributeWithSubtype() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      c.add(attr);
      PRMInterface  sub_c("sub_c", c);
      PRMAttribute* state = new PRMAttribute("attr", *state__);
      // Act
      TS_ASSERT_THROWS_NOTHING(sub_c.overload(state));
      // Assert
      TS_ASSERT(sub_c.exists(state->safeName()));
      TS_ASSERT(sub_c.exists(attr->safeName()));
      const auto& b = sub_c.get(attr->safeName());
      const auto& s = sub_c.get(state->safeName());
      TS_ASSERT_DIFFERS(b.type(), s.type());
    }

    void testOverloadAttributeWithSeveralCastDescendants() {
      // Arrange
      int                     size = 10;
      std::vector< PRMType* > types;
      types.push_back(boolean__);
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
      TS_ASSERT_THROWS_NOTHING(sub_c.overload(state));
      // Assert
      for (int i = 0; i < size; i++) {
        std::string i_name = "(" + types[i]->name() + ")attr";
        TS_ASSERT(sub_c.exists(i_name));

        for (int j = i + 1; j < size; j++) {
          std::string j_name = "(" + types[j]->name() + ")attr";
          auto        i_attr = sub_c.get(i_name).type();
          auto        j_attr = sub_c.get(j_name).type();
          TS_ASSERT_DIFFERS(i_attr, j_attr);
        }
      }
      // Cleanup
      for (int i = 1; i < size; ++i) {
        delete types[i];
      }
    }

    void testOverloadAttributeDuplicateElement() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *boolean__);
      c.add(attr);
      PRMInterface  sub_c("sub_c", c);
      PRMAttribute* state = new PRMAttribute("attr", *state__);
      sub_c.overload(state);
      auto before = sub_c.attributes().size();
      // Act
      TS_ASSERT_THROWS(sub_c.overload(state), gum::DuplicateElement);
      // Assert
      auto after = sub_c.attributes().size();
      TS_ASSERT(sub_c.exists(attr->safeName()));
      TS_ASSERT(sub_c.exists(state->safeName()));
      TS_ASSERT_EQUALS(before, after);
    }

    void testOverloadAttributeWithSeveralCastDescendantsDuplicate() {
      // Arrange
      int                     size = 10;
      std::vector< PRMType* > types;
      types.push_back(boolean__);
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
      TS_ASSERT_THROWS(sub_c.overload(state), gum::DuplicateElement);
      // Assert
      auto after = sub_c.attributes().size();
      for (int i = 0; i < size; i++) {
        std::string i_name = "(" + types[i]->name() + ")attr";
        TS_ASSERT(sub_c.exists(i_name));

        for (int j = i + 1; j < size; j++) {
          std::string j_name = "(" + types[j]->name() + ")attr";
          auto        i_attr = sub_c.get(i_name).type();
          auto        j_attr = sub_c.get(j_name).type();
          TS_ASSERT_DIFFERS(i_attr, j_attr);
        }
      }
      TS_ASSERT_EQUALS(before, after);
      // Cleanup
      for (int i = 1; i < size; ++i) {
        delete types[i];
      }
    }

    void testOverloadReference() {
      // Arrange
      PRMInterface c_1("c_1");
      PRMInterface c_2("c_2", c_1);
      PRMInterface c_3("c_3");
      Reference*   ref = new Reference("ref", c_1, false);
      c_3.add(ref);
      PRMInterface c_4("c_4", c_3);
      Reference*   sub_ref = new Reference("ref", c_2, false);
      // Act
      TS_ASSERT_THROWS_NOTHING(c_4.overload(sub_ref));
      // Assert
      TS_ASSERT(!c_4.exists(ref->safeName()));
      TS_ASSERT(c_4.exists(sub_ref->name()));
      TS_ASSERT(c_4.exists(sub_ref->safeName()));
      TS_ASSERT_EQUALS(c_4.referenceSlots().size(), (gum::Size)1);
    }

    void testOverloadReferenceIllegal() {
      // Arrange
      PRMInterface c_1("c_1");
      PRMInterface c_2("c_2", c_1);
      PRMInterface c_3("c_3");
      Reference*   ref = new Reference("ref", c_1, false);
      c_3.add(ref);
      PRMInterface c_4("c_4", c_3);
      Reference*   sub_ref = new Reference("ref", c_3);
      // Act
      TS_ASSERT_THROWS(c_4.overload(sub_ref), gum::OperationNotAllowed);
      // Assert
      TS_ASSERT(c_4.exists(ref->safeName()));
      TS_ASSERT(c_4.exists(ref->name()));
      TS_ASSERT(!c_4.exists(sub_ref->safeName()));
      TS_ASSERT_EQUALS(c_4.referenceSlots().size(), (gum::Size)1);
      delete sub_ref;
    }
    /// @}
    /// Input, output and inner nodes methods.
    /// @{

    void testIsInputNode() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *boolean__);
      c.add(a);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.isInputNode(*a));
      // Assert
      TS_ASSERT(!actual);
    }

    void testSetInputNode() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *boolean__);
      c.add(a);
      bool before = c.isInputNode(*a);
      bool after  = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(c.setInputNode(*a, true));
      // Assert
      TS_ASSERT(after = c.isInputNode(*a));
      TS_ASSERT_DIFFERS(before, after);
      TS_ASSERT(after);
    }

    void testIsOutputNode() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *boolean__);
      c.add(a);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.isOutputNode(*a));
      // Assert
      TS_ASSERT(!actual);
    }

    void testSetOutputNode() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *boolean__);
      c.add(a);
      bool before = c.isOutputNode(*a);
      bool after  = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(c.setOutputNode(*a, true));
      // Assert
      TS_ASSERT(after = c.isOutputNode(*a));
      TS_ASSERT_DIFFERS(before, after);
      TS_ASSERT(after);
    }

    void testIsInnerNode() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *boolean__);
      c.add(a);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.isInnerNode(*a));
      // Assert
      TS_ASSERT(actual);
    }

    void testInnerNodeConsistency() {
      // Arrange
      PRMInterface  c("class");
      PRMAttribute* a = new PRMAttribute("attr", *boolean__);
      c.add(a);
      // Act & Assert
      TS_ASSERT(c.isInnerNode(*a));
      TS_ASSERT(!c.isInputNode(*a));
      TS_ASSERT(!c.isOutputNode(*a));
      TS_ASSERT_THROWS_NOTHING(c.setInputNode(*a, true));
      TS_ASSERT(!c.isInnerNode(*a));
      TS_ASSERT(c.isInputNode(*a));
      TS_ASSERT(!c.isOutputNode(*a));
      TS_ASSERT_THROWS_NOTHING(c.setOutputNode(*a, true));
      TS_ASSERT(!c.isInnerNode(*a));
      TS_ASSERT(c.isInputNode(*a));
      TS_ASSERT(c.isOutputNode(*a));
      TS_ASSERT_THROWS_NOTHING(c.setInputNode(*a, false));
      TS_ASSERT_THROWS_NOTHING(c.setOutputNode(*a, false));
      TS_ASSERT(c.isInnerNode(*a));
      TS_ASSERT(!c.isInputNode(*a));
      TS_ASSERT(!c.isOutputNode(*a));
    }
    /// @}
    /// Get operator
  };

}   // namespace gum_tests
