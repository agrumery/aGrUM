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

#include <agrum/base/multidim/aggregators/and.h>
#include <agrum/base/multidim/aggregators/count.h>
#include <agrum/base/multidim/aggregators/or.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/PRM/elements/PRMAggregate.h>
#include <agrum/PRM/elements/PRMFormAttribute.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class [[maybe_unused]] PRMAggregateTestSuite: public CxxTest::TestSuite {
    private:
    using PRMAggregate = gum::prm::PRMAggregate< double >;
    using AggType      = gum::prm::PRMAggregate< double >::AggregateType;

    ClassElementTestSuiteAbstract* _classEltTestSuite_;
    gum::prm::PRMType*             _boolean_;
    gum::prm::PRMType*             _state_;
    std::vector< AggType >*        _types_;
    gum::prm::PRMClass< double >*  _class_;

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
      _types_ = new std::vector< AggType >{AggType::MIN,
                                           AggType::MAX,
                                           AggType::COUNT,
                                           AggType::EXISTS,
                                           AggType::FORALL,
                                           AggType::OR,
                                           AggType::AND};
      _class_ = new gum::prm::PRMClass< double >("dummy");
    }

    void tearDown() {
      delete _classEltTestSuite_;
      delete _boolean_;
      delete _state_;
      delete _types_;
      delete _class_;
    }

    /// PRMClassElement Tests
    /// @{
    GUM_ACTIVE_TEST(IsReferenceSlot) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      bool              expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsReferenceSlot(agg, expected);
    }

    GUM_ACTIVE_TEST(IsAttribute) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      bool              expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsAttribute(agg, expected);
    }

    GUM_ACTIVE_TEST(IsSlotChain) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      bool              expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsSlotChain(agg, expected);
    }

    GUM_ACTIVE_TEST(SetNodeId) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      // Act & Assert
      _classEltTestSuite_->testSetNodeId(agg);
    }

    GUM_ACTIVE_TEST(ObjType) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      // Act & Assert
      _classEltTestSuite_->test_obj_type(agg);
    }

    GUM_ACTIVE_TEST(SafeName) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};

      // Act & Assert
      _classEltTestSuite_->testSafeName(agg);
    }

    GUM_ACTIVE_TEST(Cast_NotAllowed) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};

      // Act & Assert
      _classEltTestSuite_->testCast_NotAllowed(agg);
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
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, state, 0};

      // Act & Assert
      _classEltTestSuite_->testCast(agg, boolean);
    }

    /// @}

    /// Constructors & destructors
    /// @{
    GUM_ACTIVE_TEST(FirstConstructor) {
      // Arrange
      PRMAggregate* agg = nullptr;
      for (auto t: *_types_) {
        // Act
        TS_ASSERT_THROWS_NOTHING(agg = new PRMAggregate("my_agg", t, *_boolean_))
        // Assert
        TS_ASSERT_THROWS_NOTHING(delete agg)
      }
    }

    GUM_ACTIVE_TEST(SecondConstructor) {
      // Arrange
      PRMAggregate* agg = nullptr;
      for (auto t: *_types_) {
        // Act
        TS_ASSERT_THROWS_NOTHING(agg = new PRMAggregate("my_agg", t, *_boolean_, 0))
        // Assert
        TS_ASSERT_THROWS_NOTHING(delete agg)
      }
    }

    /// @}

    /// Getters & setters
    /// @{
    GUM_ACTIVE_TEST(AggTypeNoLabel) {
      // Arrange
      for (auto t: *_types_) {
        // Act
        PRMAggregate agg("my_agg", t, *_boolean_);
        // Assert
        TS_ASSERT_EQUALS(agg.agg_type(), t)
      }
    }

    GUM_ACTIVE_TEST(Label) {
      // Arrange
      gum::Idx     actual = 1;
      PRMAggregate agg("my_agg", AggType::MIN, *_boolean_, actual);
      gum::Idx     expected = 0;
      // Act
      TS_ASSERT_THROWS_NOTHING(expected = agg.label())
      // Assert
      TS_ASSERT_EQUALS(actual, expected)
    }

    GUM_ACTIVE_TEST(LabelOperationNotAllowed) {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *_boolean_);
      // Act
      TS_ASSERT_THROWS(agg.label(), const gum::OperationNotAllowed&)
      // Assert
    }

    GUM_ACTIVE_TEST(AddParent) {
      // Arrange
      gum::prm::PRMFormAttribute< double > parent(*_class_, "attr", *_boolean_);
      PRMAggregate                         child("my_agg", AggType::MIN, *_boolean_);
      auto                                 before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent))
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
      TS_ASSERT(!parent.cpf().contains(child.type().variable()))
    }

    GUM_ACTIVE_TEST(AddChild) {
      // Arrange
      gum::prm::PRMFormAttribute< double > child(*_class_, "attr", *_boolean_);
      PRMAggregate                         parent("my_agg", AggType::MIN, *_boolean_);
      auto                                 before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(parent.addChild(child))
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
      TS_ASSERT(!child.cpf().contains(parent.type().variable()))
    }

    GUM_ACTIVE_TEST(Cpf) {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *_boolean_);
      // Act & Assert
      TS_ASSERT_THROWS(agg.cpf(), const gum::OperationNotAllowed&)
    }

    GUM_ACTIVE_TEST(CpfConst) {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *_boolean_);
      const auto&  const_agg = agg;
      // Act & Assert
      TS_ASSERT_THROWS(const_agg.cpf(), const gum::OperationNotAllowed&)
    }

    GUM_ACTIVE_TEST(BuildImplMin) {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::MIN, *_boolean_);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl())
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Min< double >* >(impl))
      TS_ASSERT_THROWS_NOTHING(delete impl)
    }

    GUM_ACTIVE_TEST(BuildImplMax) {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::MAX, *_boolean_);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl())
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Max< double >* >(impl))
      TS_ASSERT_THROWS_NOTHING(delete impl)
    }

    GUM_ACTIVE_TEST(BuildImplExists) {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::EXISTS, *_boolean_, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl())
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Exists< double >* >(impl))
      TS_ASSERT_THROWS_NOTHING(delete impl)
    }

    GUM_ACTIVE_TEST(BuildImplForAll) {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::FORALL, *_boolean_, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl())
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Forall< double >* >(impl))
      TS_ASSERT_THROWS_NOTHING(delete impl)
    }

    GUM_ACTIVE_TEST(BuildImplCount) {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::COUNT, *_boolean_, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl())
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Count< double >* >(impl))
      TS_ASSERT_THROWS_NOTHING(delete impl)
    }

    GUM_ACTIVE_TEST(BuildImplOr) {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::OR, *_boolean_, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl())
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Or< double >* >(impl))
      TS_ASSERT_THROWS_NOTHING(delete impl)
    }

    GUM_ACTIVE_TEST(BuildImplAnd) {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::AND, *_boolean_, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl())
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::And< double >* >(impl))
      TS_ASSERT_THROWS_NOTHING(delete impl)
    }

    /// @}
  };

}   // namespace gum_tests
