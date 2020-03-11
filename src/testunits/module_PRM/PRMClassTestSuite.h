
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <sstream>

#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/tools/variables/labelizedVariable.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class PRMClassTestSuite: public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMClass< double >           PRMClass;
    typedef gum::prm::PRMInterface< double >       PRMInterface;
    typedef gum::prm::PRMType                      PRMType;
    typedef gum::prm::PRMAggregate< double >       PRMAggregate;
    typedef gum::prm::PRMScalarAttribute< double > PRMAttribute;
    typedef gum::prm::PRMReferenceSlot< double >   Reference;
    typedef gum::prm::PRMSlotChain< double >       PRMSlotChain;

    PRMType* __boolean;
    PRMType* __state;

    public:
    void setUp() {
      __boolean = gum::prm::PRMType::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      __state = new PRMType(*__boolean, map, state);
    }

    void tearDown() {
      delete __boolean;
      delete __state;
    }

    /// Constructor & Destructor
    /// @{
    void testConstructor() {
      // Arrange
      PRMClass* c = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(c = new PRMClass("class"));
      TS_ASSERT_THROWS_NOTHING(delete c);
    }

    void testConstructorInheritance() {
      // Arrange
      PRMClass toRef("toRef");
      auto     a = new PRMAttribute("a", *__boolean);
      toRef.add(a);
      auto ref = new Reference("rho", toRef);
      gum::Sequence< gum::prm::PRMClassElement< double >* > seq;
      seq << ref << a;
      auto     chain = new PRMSlotChain("rho.a", seq);
      PRMClass super("super");
      auto     b = new PRMAttribute("b", *__boolean);
      auto     b_id = super.add(b);
      auto     c = new PRMAttribute("c", *__boolean);
      auto     c_id = super.add(c);
      super.addArc("b", "c");
      super.add(ref);
      super.add(chain);
      super.addArc("rho.a", "c");
      PRMClass* subclass = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(subclass = new PRMClass("subclass", super));
      // Assert
      TS_ASSERT(subclass->exists("b"));
      TS_ASSERT_EQUALS(subclass->get(b_id).name(), "b");
      TS_ASSERT(subclass->exists("c"));
      TS_ASSERT_EQUALS(subclass->get(c_id).name(), "c");
      TS_ASSERT_EQUALS(subclass->attributes().size(), (gum::Size)2);
      TS_ASSERT_EQUALS(subclass->referenceSlots().size(), (gum::Size)1);
      TS_ASSERT_EQUALS(subclass->slotChains().size(), (gum::Size)1);
      delete subclass;
    }

    void testConstructorImplementation() {
      // Arrange
      PRMClass toRef("toRef");
      auto     a = new PRMAttribute("a", *__boolean);
      toRef.add(a);
      auto         ref = new Reference("rho", toRef);
      PRMInterface i("i");
      auto         b = new PRMAttribute("b", *__boolean);
      auto         c = new PRMAttribute("c", *__boolean);
      i.add(ref);
      i.add(b);
      i.add(c);
      PRMClass*                 subclass = nullptr;
      gum::Set< PRMInterface* > set;
      set << &i;
      // Act
      TS_ASSERT_THROWS_NOTHING(subclass = new PRMClass("subclass", set));
      // Assert
      TS_ASSERT_EQUALS(subclass->attributes().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(subclass->referenceSlots().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(subclass->slotChains().size(), (gum::Size)0);
      delete subclass;
    }
    /// @}
    /// Belongs and exists tests
    /// @{
    void testBelongsTo() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c.add(attr);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.belongsTo(*attr));
      // Assert
      TS_ASSERT(actual);
    }

    void testBelongsToNot() {
      // Arrange
      PRMClass     c("class");
      PRMAttribute attr("attr", *__boolean);
      bool         actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.belongsTo(attr));
      // Assert
      TS_ASSERT(!actual);
    }

    void testExists() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c.add(attr);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.exists("attr"));
      // Assert
      TS_ASSERT(actual);
    }

    void testExistsNot() {
      // Arrange
      PRMClass c("class");
      bool     actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.exists("attr"));
      // Assert
      TS_ASSERT(!actual);
    }

    void testGet() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c.add(attr);
      // Act
      auto& actual = c.get(attr->name());
      // Assert
      TS_ASSERT_EQUALS(&actual, attr);
    }

    void testGetConst() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c.add(attr);
      const auto& const_c = c;
      // Act
      const auto& actual = const_c.get(attr->name());
      // Assert
      TS_ASSERT_EQUALS(&actual, attr);
    }

    void testGetNotFound() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c.add(attr);
      // Act & Assert
      TS_ASSERT_THROWS(c.get("foo"), gum::NotFound);
    }

    void testGetConstNotFound() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c.add(attr);
      const auto& const_c = c;
      // Act & Assert
      TS_ASSERT_THROWS(const_c.get("foo"), gum::NotFound);
    }

    void testAdd() {
      // Arra,ge
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      gum::NodeId   id = 100;   // Id generation starts at 0
      // Act & assert
      TS_ASSERT_THROWS_NOTHING(id = c.add(attr));
      TS_ASSERT(c.exists(attr->name()));
      TS_ASSERT_EQUALS(&(c.get(attr->name())), attr);
      TS_ASSERT_EQUALS(id, attr->id());
      TS_ASSERT_DIFFERS(id, (gum::NodeId)100);
    }

    void testAddDuplicate() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
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
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      // Act & assert
      TS_ASSERT_THROWS(c.overload(attr), gum::OperationNotAllowed);
      // Cleanup
      delete attr;
    }

    void testOverloadWrongClassElement() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c.add(attr);
      PRMClass   sub_c("sub c", c);
      Reference* ref = new Reference("attr", c);
      // Act & Assert
      TS_ASSERT_THROWS(sub_c.overload(ref), gum::OperationNotAllowed);
      // Cleanup
      delete ref;
    }

    void testOverloadTypeError() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c.add(attr);
      PRMClass               sub_c("sub_c", c);
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
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c.add(attr);
      PRMClass      sub_c("sub_c", c);
      PRMAttribute* sub_attr = new PRMAttribute("attr", *__boolean);
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
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c.add(attr);
      PRMClass      sub_c("sub_c", c);
      PRMAttribute* state = new PRMAttribute("attr", *__state);
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
      types.push_back(__boolean);
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      for (int i = 1; i < size; i++) {
        auto&             super = *(types[i - 1]);
        std::stringstream sbuff;
        sbuff << "type_" << i;
        auto name = sbuff.str();
        auto var = gum::LabelizedVariable(name, "", 2);
        auto t = new PRMType(super, map, var);
        types.push_back(t);
      }
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *(types[0]));
      c.add(attr);
      PRMClass      sub_c("sub_c", c);
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
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c.add(attr);
      PRMClass      sub_c("sub_c", c);
      PRMAttribute* state = new PRMAttribute("attr", *__state);
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
      types.push_back(__boolean);
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      for (int i = 1; i < size; i++) {
        auto&             super = *(types[i - 1]);
        std::stringstream sbuff;
        sbuff << "type_" << i;
        auto name = sbuff.str();
        auto var = gum::LabelizedVariable(name, "", 2);
        auto t = new PRMType(super, map, var);
        types.push_back(t);
      }
      PRMClass      c("class");
      PRMAttribute* attr = new PRMAttribute("attr", *(types[0]));
      c.add(attr);
      PRMClass      sub_c("sub_c", c);
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
      PRMClass   c_1("c_1");
      PRMClass   c_2("c_2", c_1);
      PRMClass   c_3("c_3");
      Reference* ref = new Reference("ref", c_1, false);
      c_3.add(ref);
      PRMClass   c_4("c_4", c_3);
      Reference* sub_ref = new Reference("ref", c_2, false);
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
      PRMClass   c_1("c_1");
      PRMClass   c_2("c_2", c_1);
      PRMClass   c_3("c_3");
      Reference* ref = new Reference("ref", c_1, false);
      c_3.add(ref);
      PRMClass   c_4("c_4", c_3);
      Reference* sub_ref = new Reference("ref", c_3);
      // Act
      TS_ASSERT_THROWS(c_4.overload(sub_ref), gum::OperationNotAllowed);
      // Assert
      TS_ASSERT(c_4.exists(ref->safeName()));
      TS_ASSERT(c_4.exists(ref->name()));
      TS_ASSERT(!c_4.exists(sub_ref->safeName()));
      TS_ASSERT_EQUALS(c_4.referenceSlots().size(), (gum::Size)1);
      delete sub_ref;
    }

    void testOverloadSlotChainOperantionNotAllowed() {
      // Arrange
      PRMClass   c_1("c_1");
      PRMClass   c_2("c_2");
      Reference* ref_1 = new Reference("ref_1", c_2, false);
      c_1.add(ref_1);
      PRMClass   c_3("c_3");
      Reference* ref_2 = new Reference("ref_2", c_3, false);
      c_2.add(ref_2);
      PRMAttribute* attr = new PRMAttribute("attr", *__boolean);
      c_3.add(attr);
      gum::Sequence< gum::prm::PRMClassElement< double >* > seq;
      seq.insert(ref_1);
      seq.insert(ref_2);
      seq.insert(attr);
      PRMSlotChain* chain = new PRMSlotChain("ref_1.ref_2.attr", seq);
      c_1.add(chain);
      PRMClass      c_4("c_4", c_1);
      PRMSlotChain* chain_copy = new PRMSlotChain("ref_1.ref_2.attr", seq);
      // Act
      TS_ASSERT_THROWS(c_4.overload(chain_copy), gum::OperationNotAllowed);
      // Assert
      TS_ASSERT(c_4.exists(chain->safeName()));
      TS_ASSERT(c_4.exists(chain->name()));
      TS_ASSERT_EQUALS(c_4.slotChains().size(), (gum::Size)1);
      delete chain_copy;
    }

    /// @}
    /// Input, output and inner nodes methods.
    /// @{

    void testIsInputNode() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* a = new PRMAttribute("attr", *__boolean);
      c.add(a);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.isInputNode(*a));
      // Assert
      TS_ASSERT(!actual);
    }

    void testSetInputNode() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* a = new PRMAttribute("attr", *__boolean);
      c.add(a);
      bool before = c.isInputNode(*a);
      bool after = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(c.setInputNode(*a, true));
      // Assert
      TS_ASSERT(after = c.isInputNode(*a));
      TS_ASSERT_DIFFERS(before, after);
      TS_ASSERT(after);
    }

    void testIsOutputNode() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* a = new PRMAttribute("attr", *__boolean);
      c.add(a);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.isOutputNode(*a));
      // Assert
      TS_ASSERT(!actual);
    }

    void testSetOutputNode() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* a = new PRMAttribute("attr", *__boolean);
      c.add(a);
      bool before = c.isOutputNode(*a);
      bool after = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(c.setOutputNode(*a, true));
      // Assert
      TS_ASSERT(after = c.isOutputNode(*a));
      TS_ASSERT_DIFFERS(before, after);
      TS_ASSERT(after);
    }

    void testIsInnerNode() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* a = new PRMAttribute("attr", *__boolean);
      c.add(a);
      bool actual = false;
      // Act
      TS_ASSERT_THROWS_NOTHING(actual = c.isInnerNode(*a));
      // Assert
      TS_ASSERT(actual);
    }

    void testInnerNodeConsistency() {
      // Arrange
      PRMClass      c("class");
      PRMAttribute* a = new PRMAttribute("attr", *__boolean);
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

    void testIONodesInheritance() {
      try {
        // Arrange
        // Event
        PRMInterface event("Event");
        auto         e_state = new PRMAttribute("state", *__boolean);
        event.add(e_state);
        // Gate
        PRMInterface gate("Gate", event);
        auto         g_inputs = new Reference("inputs", event, true);
        gate.add(g_inputs);
        gum::Set< gum::prm::PRMInterface< double >* > impl;
        impl << &gate;
        // OrGate
        PRMClass orGate("OrGate", impl);
        auto     or_inputs = new Reference("inputs", event, true);
        orGate.add(or_inputs);
        auto or_state = new PRMAggregate(
           "state", PRMAggregate::AggregateType::EXISTS, *__boolean, 1);
        orGate.add(or_state);
        gum::Sequence< gum::prm::PRMClassElement< double >* > or_seq;
        or_seq << &(orGate.get("inputs")) << &(event.get("state"));
        auto or_chain = new PRMSlotChain("inputs.state", or_seq);
        orGate.add(or_chain);
        orGate.addArc("inputs.state", "state");
        // AndGate
        PRMClass andGate("AndGate", impl);
        auto     and_inputs = new Reference("inputs", event, true);
        andGate.add(and_inputs);
        auto and_state = new PRMAggregate(
           "state", PRMAggregate::AggregateType::FORALL, *__boolean, 1);
        andGate.add(and_state);
        gum::Sequence< gum::prm::PRMClassElement< double >* > and_seq;
        and_seq << and_inputs << &(event.get("state"));
        auto and_chain = new PRMSlotChain("inputs.state", and_seq);
        andGate.add(and_chain);
        andGate.addArc("inputs.state", "state");
        // KNGate
        PRMClass knGate("k_nGate", impl);
        auto     kn_inputs = new Reference("inputs", event, true);
        knGate.add(kn_inputs);
        PRMType intervalle(gum::LabelizedVariable("intervalle", "", 6));
        auto    nb_true = new PRMAggregate(
           "Nb_true", PRMAggregate::AggregateType::COUNT, *__boolean, 1);
        knGate.add(nb_true);
        gum::Sequence< gum::prm::PRMClassElement< double >* > kn_seq;
        kn_seq << kn_inputs << &(event.get("state"));
        auto kn_chain = new PRMSlotChain("inputs.state", kn_seq);
        knGate.add(kn_chain);
        knGate.addArc("inputs.state", "Nb_true");
        PRMType intervalle_k(gum::LabelizedVariable("intervalle_K", "", 4));
        auto    k = new PRMAttribute("K", intervalle_k);
        knGate.add(k);
        std::vector< double > k_values{0.25, 0.25, 0.25, 0.25};
        k->cpf().fillWith(k_values);
        auto kn_state = new PRMAttribute("state", *__boolean);
        knGate.add(kn_state);
        knGate.addArc("K", "state");
        knGate.addArc("Nb_true", "state");
        // Assert
        TS_ASSERT(knGate.isOutputNode(knGate.get("state")));
      } catch (gum::Exception& e) { TS_FAIL(e.errorContent()); }
    }
    /// @}
  };

}   // namespace gum_tests
