/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMFormAttribute.h>
#include <agrum/PRM/elements/PRMReferenceSlot.h>
#include <agrum/PRM/elements/PRMSlotChain.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class SlotChainTestSuite: public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMSlotChain< double >               PRMSlotChain;
    ClassElementTestSuiteAbstract*                         classEltTestSuite__;
    gum::prm::PRMClass< double >*                          A__;
    gum::prm::PRMClass< double >*                          B__;
    gum::prm::PRMClass< double >*                          C__;
    gum::prm::PRMType*                                     boolean__;
    gum::prm::PRMType*                                     state__;
    gum::Sequence< gum::prm::PRMClassElement< double >* >* booleanChain__;
    gum::Sequence< gum::prm::PRMClassElement< double >* >* stateChain__;

    // these will be dispodes by their repsective class
    gum::prm::PRMReferenceSlot< double >* refI__;
    gum::prm::PRMReferenceSlot< double >* refJ__;
    gum::prm::PRMReferenceSlot< double >* refK__;
    gum::prm::PRMAttribute< double >*     boolAttr__;
    gum::prm::PRMAttribute< double >*     stateAttr__;

    public:
    void setUp() {
      classEltTestSuite__ = new ClassElementTestSuiteAbstract();
      A__ = new gum::prm::PRMClass< double >("A");
      B__ = new gum::prm::PRMClass< double >("B");
      C__ = new gum::prm::PRMClass< double >("C");

      refI__ = new gum::prm::PRMReferenceSlot< double >("refA", *B__);
      A__->add(refI__);
      refJ__ = new gum::prm::PRMReferenceSlot< double >("refB", *C__);
      B__->add(refJ__);
      refK__ = new gum::prm::PRMReferenceSlot< double >("refK", *C__, true);
      B__->add(refK__);

      boolean__ = gum::prm::PRMType::boolean();
      boolAttr__ =
         new gum::prm::PRMScalarAttribute< double >("myBool", *boolean__);
      C__->add(boolAttr__);

      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      state__ = new gum::prm::PRMType(*boolean__, map, state);
      stateAttr__ =
         new gum::prm::PRMScalarAttribute< double >("myState", *state__);
      C__->add(stateAttr__);

      booleanChain__ = new gum::Sequence< gum::prm::PRMClassElement< double >* >();
      booleanChain__->insert(refI__);
      booleanChain__->insert(refJ__);
      booleanChain__->insert(boolAttr__);

      stateChain__ = new gum::Sequence< gum::prm::PRMClassElement< double >* >();
      stateChain__->insert(refI__);
      stateChain__->insert(refK__);
      stateChain__->insert(stateAttr__);
    }

    void tearDown() {
      delete classEltTestSuite__;
      delete A__;
      delete B__;
      delete C__;
      delete boolean__;
      delete state__;
      delete booleanChain__;
      delete stateChain__;
    }

    /// PRMClassElement Tests
    /// @{
    void testIsReferenceSlot() {
      // Arrange
      PRMSlotChain slot("slot", *booleanChain__);
      bool         expected = false;
      // Act & Assert
      classEltTestSuite__->testIsReferenceSlot(slot, expected);
    }

    void testIsAttribute() {
      // Arrange
      PRMSlotChain slot("slot", *booleanChain__);
      bool         expected = false;
      // Act & Assert
      classEltTestSuite__->testIsAttribute(slot, expected);
    }

    void testIsSlotChain() {
      // Arrange
      PRMSlotChain slot("slot", *booleanChain__);
      bool         expected = true;
      // Act & Assert
      classEltTestSuite__->testIsSlotChain(slot, expected);
    }

    void testSetNodeId() {
      // Arrange
      PRMSlotChain slot("slot", *booleanChain__);
      // Act & Assert
      classEltTestSuite__->testSetNodeId(slot);
    }

    void testObjType() {
      // Arrange
      PRMSlotChain slot("slot", *booleanChain__);
      // Act & Assert
      classEltTestSuite__->test_obj_type(slot);
    }

    void testSafeName() {
      // Arrange
      PRMSlotChain slot("slot", *booleanChain__);
      // Act & Assert
      classEltTestSuite__->testSafeName(slot);
    }

    void testCast_NotAllowed() {
      // Arrange
      PRMSlotChain slot("slot", *booleanChain__);
      // Act & Assert
      classEltTestSuite__->testCast_NotAllowed(slot);
    }

    void testCast() {
      // Arrange
      PRMSlotChain slot("slot", *stateChain__);
      // Act & Assert
      classEltTestSuite__->testCast(slot, *boolean__);
    }
    /// @}

    /// Constructors & destructors
    /// @{
    void testConstructorSimple() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(slot = new PRMSlotChain("slot", *booleanChain__));
      TS_ASSERT_THROWS_NOTHING(delete slot);
    }

    void testSecondConstructorSimple() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(slot = new PRMSlotChain(booleanChain__, "slot"));
      TS_ASSERT_THROWS_NOTHING(delete slot);
    }

    void testConstructorComplex() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(slot = new PRMSlotChain("slot", *stateChain__));
      TS_ASSERT_THROWS_NOTHING(delete slot);
    }

    void testSecondConstructorComplex() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(slot = new PRMSlotChain(stateChain__, "slot"));
      TS_ASSERT_THROWS_NOTHING(delete slot);
    }
    /// @}

    /// Methods
    /// @{
    void testIsMultiple() {
      // Arrange
      PRMSlotChain simple("simple", *booleanChain__);
      PRMSlotChain multiple("multiple", *stateChain__);
      // Act & Assert
      TS_ASSERT(!simple.isMultiple());
      TS_ASSERT(multiple.isMultiple());
    }

    void testLastEltSimple() {
      // Arrange
      PRMSlotChain                         simple("simple", *booleanChain__);
      gum::prm::PRMClassElement< double >* elt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(elt = &(simple.lastElt()));
      // Assert
      TS_ASSERT_EQUALS(boolAttr__->safeName(), elt->safeName());
    }

    void testLastEltSimpleConst() {
      // Arrange
      PRMSlotChain                               simple("simple", *booleanChain__);
      const auto&                                const_simple = simple;
      gum::prm::PRMClassElement< double > const* elt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(elt = &(const_simple.lastElt()));
      // Assert
      TS_ASSERT_EQUALS(boolAttr__->safeName(), elt->safeName());
    }

    void testChain() {
      // Arrange
      PRMSlotChain simple("simple", *booleanChain__);
      gum::Sequence< gum::prm::PRMClassElement< double >* > const* chain = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(chain = &(simple.chain()));
      // ASSERT
      TS_ASSERT_EQUALS(chain->size(), booleanChain__->size());
      TS_ASSERT_EQUALS(chain->atPos(0), booleanChain__->atPos(0));
      TS_ASSERT_EQUALS(chain->atPos(1), booleanChain__->atPos(1));
      TS_ASSERT_DIFFERS(chain->atPos(2), booleanChain__->atPos(2));
    }

    void testCpf() {
      // Arrange
      PRMSlotChain             slot("simple", *booleanChain__);
      gum::Potential< double > expected;
      expected << slot.type().variable();
      // Act
      auto& actual = slot.cpf();
      // Assert
      gum::Instantiation i(expected);
      gum::Instantiation j(actual);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(expected[i], actual[j]);
      }
      TS_ASSERT(i.end());
      TS_ASSERT(j.end());
    }

    void testCpfConst() {
      // Arrange
      PRMSlotChain             slot("simple", *booleanChain__);
      const auto&              slot_const = slot;
      gum::Potential< double > expected;
      expected << slot.type().variable();
      // Act
      const auto& actual = slot_const.cpf();
      // Assert
      gum::Instantiation i(expected);
      gum::Instantiation j(actual);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(expected[i], actual[j]);
      }
      TS_ASSERT(i.end());
      TS_ASSERT(j.end());
    }

    /// @}

    /// Add parents and children
    /// @{
    void testAddParentCheckChild() {
      // Arrange
      gum::prm::PRMScalarAttribute< double > parent("attr", *boolean__);
      PRMSlotChain                           child("child", *booleanChain__);
      auto before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
      TS_ASSERT(!child.cpf().contains(parent.type().variable()));
    }

    void testAddParentDupplicateError() {
      // Arrange
      gum::prm::PRMScalarAttribute< double > parent("attr", *boolean__);
      PRMSlotChain                           child("child", *booleanChain__);
      child.addParent(parent);
      auto before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
      TS_ASSERT(!child.cpf().contains(parent.type().variable()));
    }

    void testAddChild() {
      // Arrange
      PRMSlotChain                           parent("simple", *booleanChain__);
      gum::prm::PRMScalarAttribute< double > child("attr", *boolean__);
      auto before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(parent.addChild(child));
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
      TS_ASSERT(!parent.cpf().contains(child.type().variable()));
    }
    /// @}
  };

}   // namespace gum_tests
