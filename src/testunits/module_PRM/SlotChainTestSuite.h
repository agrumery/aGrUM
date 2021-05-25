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
    void setUp() {
      _classEltTestSuite_ = new ClassElementTestSuiteAbstract();
      _A_                 = new gum::prm::PRMClass< double >("A");
      _B_                 = new gum::prm::PRMClass< double >("B");
      _C_                 = new gum::prm::PRMClass< double >("C");

      _refI_ = new gum::prm::PRMReferenceSlot< double >("refA", *_B_);
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

    void tearDown() {
      delete _classEltTestSuite_;
      delete _A_;
      delete _B_;
      delete _C_;
      delete _boolean_;
      delete _state_;
      delete _booleanChain_;
      delete _stateChain_;
    }

    /// PRMClassElement Tests
    /// @{
    void testIsReferenceSlot() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      bool         expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsReferenceSlot(slot, expected);
    }

    void testIsAttribute() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      bool         expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsAttribute(slot, expected);
    }

    void testIsSlotChain() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      bool         expected = true;
      // Act & Assert
      _classEltTestSuite_->testIsSlotChain(slot, expected);
    }

    void testSetNodeId() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      // Act & Assert
      _classEltTestSuite_->testSetNodeId(slot);
    }

    void testObjType() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      // Act & Assert
      _classEltTestSuite_->test_obj_type(slot);
    }

    void testSafeName() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      // Act & Assert
      _classEltTestSuite_->testSafeName(slot);
    }

    void testCast_NotAllowed() {
      // Arrange
      PRMSlotChain slot("slot", *_booleanChain_);
      // Act & Assert
      _classEltTestSuite_->testCast_NotAllowed(slot);
    }

    void testCast() {
      // Arrange
      PRMSlotChain slot("slot", *_stateChain_);
      // Act & Assert
      _classEltTestSuite_->testCast(slot, *_boolean_);
    }
    /// @}

    /// Constructors & destructors
    /// @{
    void testConstructorSimple() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(slot = new PRMSlotChain("slot", *_booleanChain_));
      TS_ASSERT_THROWS_NOTHING(delete slot);
    }

    void testSecondConstructorSimple() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(slot = new PRMSlotChain(_booleanChain_, "slot"));
      TS_ASSERT_THROWS_NOTHING(delete slot);
    }

    void testConstructorComplex() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(slot = new PRMSlotChain("slot", *_stateChain_));
      TS_ASSERT_THROWS_NOTHING(delete slot);
    }

    void testSecondConstructorComplex() {
      // Arrange
      PRMSlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(slot = new PRMSlotChain(_stateChain_, "slot"));
      TS_ASSERT_THROWS_NOTHING(delete slot);
    }
    /// @}

    /// Methods
    /// @{
    void testIsMultiple() {
      // Arrange
      PRMSlotChain simple("simple", *_booleanChain_);
      PRMSlotChain multiple("multiple", *_stateChain_);
      // Act & Assert
      TS_ASSERT(!simple.isMultiple());
      TS_ASSERT(multiple.isMultiple());
    }

    void testLastEltSimple() {
      // Arrange
      PRMSlotChain                         simple("simple", *_booleanChain_);
      gum::prm::PRMClassElement< double >* elt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(elt = &(simple.lastElt()));
      // Assert
      TS_ASSERT_EQUALS(_boolAttr_->safeName(), elt->safeName());
    }

    void testLastEltSimpleConst() {
      // Arrange
      PRMSlotChain                               simple("simple", *_booleanChain_);
      const auto&                                const_simple = simple;
      gum::prm::PRMClassElement< double > const* elt          = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(elt = &(const_simple.lastElt()));
      // Assert
      TS_ASSERT_EQUALS(_boolAttr_->safeName(), elt->safeName());
    }

    void testChain() {
      // Arrange
      PRMSlotChain simple("simple", *_booleanChain_);
      gum::Sequence< gum::prm::PRMClassElement< double >* > const* chain = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(chain = &(simple.chain()));
      // ASSERT
      TS_ASSERT_EQUALS(chain->size(), _booleanChain_->size());
      TS_ASSERT_EQUALS(chain->atPos(0), _booleanChain_->atPos(0));
      TS_ASSERT_EQUALS(chain->atPos(1), _booleanChain_->atPos(1));
      TS_ASSERT_DIFFERS(chain->atPos(2), _booleanChain_->atPos(2));
    }

    void testCpf() {
      // Arrange
      PRMSlotChain             slot("simple", *_booleanChain_);
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
      PRMSlotChain             slot("simple", *_booleanChain_);
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
      gum::prm::PRMScalarAttribute< double > parent("attr", *_boolean_);
      PRMSlotChain                           child("child", *_booleanChain_);
      auto                                   before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
      TS_ASSERT(!child.cpf().contains(parent.type().variable()));
    }

    void testAddParentDupplicateError() {
      // Arrange
      gum::prm::PRMScalarAttribute< double > parent("attr", *_boolean_);
      PRMSlotChain                           child("child", *_booleanChain_);
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
      PRMSlotChain                           parent("simple", *_booleanChain_);
      gum::prm::PRMScalarAttribute< double > child("attr", *_boolean_);
      auto                                   before = parent.cpf().variablesSequence().size();
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
