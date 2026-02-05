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

#include <agrum/base/multidim/aggregators/and.h>
#include <agrum/base/multidim/aggregators/count.h>
#include <agrum/base/multidim/aggregators/or.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/PRM/elements/PRMAggregate.h>
#include <agrum/PRM/elements/PRMFormAttribute.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  PRMAggregate
#define GUM_CURRENT_MODULE PRM

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  struct PRMAggregateTestSuite {
    private:
    using PRMAggregate = gum::prm::PRMAggregate< double >;
    using AggType      = gum::prm::PRMAggregate< double >::AggregateType;

    ClassElementTestSuiteAbstract* _classEltTestSuite_;
    gum::prm::PRMType*             _boolean_;
    gum::prm::PRMType*             _state_;
    std::vector< AggType >*        _types_;
    gum::prm::PRMClass< double >*  _class_;

    public:
    /// PRMClassElement Tests
    /// @{
    PRMAggregateTestSuite() {
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

    ~PRMAggregateTestSuite() {
      delete _classEltTestSuite_;
      delete _boolean_;
      delete _state_;
      delete _types_;
      delete _class_;
    }

    static void testIsReferenceSlot() {
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

    static void testIsAttribute() {
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

    static void testIsSlotChain() {
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

    static void testSetNodeId() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      // Act & Assert
      _classEltTestSuite_->testSetNodeId(agg);
    }

    static void testObjType() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      // Act & Assert
      _classEltTestSuite_->test_obj_type(agg);
    }

    static void testSafeName() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};

      // Act & Assert
      _classEltTestSuite_->testSafeName(agg);
    }

    static void testCast_NotAllowed() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};

      // Act & Assert
      _classEltTestSuite_->testCast_NotAllowed(agg);
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
      PRMAggregate      agg{"my_agg", PRMAggregate::AggregateType::FORALL, state, 0};

      // Act & Assert
      _classEltTestSuite_->testCast(agg, boolean);
    }

    /// @}

    /// Constructors & destructors
    /// @{
    static void testFirstConstructor() {
      // Arrange
      PRMAggregate* agg = nullptr;
      for (auto t: *_types_) {
        // Act
        CHECK_NOTHROW(agg = new PRMAggregate("my_agg", t, *_boolean_));
        // Assert
        CHECK_NOTHROW(delete agg);
      }
    }

    static void testSecondConstructor() {
      // Arrange
      PRMAggregate* agg = nullptr;
      for (auto t: *_types_) {
        // Act
        CHECK_NOTHROW(agg = new PRMAggregate("my_agg", t, *_boolean_, 0));
        // Assert
        CHECK_NOTHROW(delete agg);
      }
    }

    /// @}

    /// Getters & setters
    /// @{
    static void testAggTypeNoLabel() {
      // Arrange
      for (auto t: *_types_) {
        // Act
        PRMAggregate agg("my_agg", t, *_boolean_);
        // Assert
        CHECK((agg.agg_type()) == (t));
      }
    }

    static void testLabel() {
      // Arrange
      gum::Idx     actual = 1;
      PRMAggregate agg("my_agg", AggType::MIN, *_boolean_, actual);
      gum::Idx     expected = 0;
      // Act
      CHECK_NOTHROW(expected = agg.label());
      // Assert
      CHECK((actual) == (expected));
    }

    static void testLabelOperationNotAllowed() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *_boolean_);
      // Act
      CHECK_THROWS_AS(agg.label(), const gum::OperationNotAllowed&);
      // Assert
    }

    static void testAddParent() {
      // Arrange
      gum::prm::PRMFormAttribute< double > parent(*_class_, "attr", *_boolean_);
      PRMAggregate                         child("my_agg", AggType::MIN, *_boolean_);
      auto                                 before = parent.cpf().variablesSequence().size();
      // Act
      CHECK_NOTHROW(child.addParent(parent));
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      CHECK((before) == (after));
      CHECK(!parent.cpf().contains(child.type().variable()));
    }

    static void testAddChild() {
      // Arrange
      gum::prm::PRMFormAttribute< double > child(*_class_, "attr", *_boolean_);
      PRMAggregate                         parent("my_agg", AggType::MIN, *_boolean_);
      auto                                 before = child.cpf().variablesSequence().size();
      // Act
      CHECK_NOTHROW(parent.addChild(child));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      CHECK((before) == (after));
      CHECK(!child.cpf().contains(parent.type().variable()));
    }

    static void testCpf() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *_boolean_);
      // Act & Assert
      CHECK_THROWS_AS(agg.cpf(), const gum::OperationNotAllowed&);
    }

    static void testCpfConst() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *_boolean_);
      const auto&  const_agg = agg;
      // Act & Assert
      CHECK_THROWS_AS(const_agg.cpf(), const gum::OperationNotAllowed&);
    }

    static void testBuildImplMin() {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::MIN, *_boolean_);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      CHECK_NOTHROW(impl = agg.buildImpl());
      // Assert
      CHECK(dynamic_cast< gum::aggregator::Min< double >* >(impl));
      CHECK_NOTHROW(delete impl);
    }

    static void testBuildImplMax() {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::MAX, *_boolean_);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      CHECK_NOTHROW(impl = agg.buildImpl());
      // Assert
      CHECK(dynamic_cast< gum::aggregator::Max< double >* >(impl));
      CHECK_NOTHROW(delete impl);
    }

    static void testBuildImplExists() {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::EXISTS, *_boolean_, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      CHECK_NOTHROW(impl = agg.buildImpl());
      // Assert
      CHECK(dynamic_cast< gum::aggregator::Exists< double >* >(impl));
      CHECK_NOTHROW(delete impl);
    }

    static void testBuildImplForAll() {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::FORALL, *_boolean_, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      CHECK_NOTHROW(impl = agg.buildImpl());
      // Assert
      CHECK(dynamic_cast< gum::aggregator::Forall< double >* >(impl));
      CHECK_NOTHROW(delete impl);
    }

    static void testBuildImplCount() {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::COUNT, *_boolean_, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      CHECK_NOTHROW(impl = agg.buildImpl());
      // Assert
      CHECK(dynamic_cast< gum::aggregator::Count< double >* >(impl));
      CHECK_NOTHROW(delete impl);
    }

    static void testBuildImplOr() {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::OR, *_boolean_, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      CHECK_NOTHROW(impl = agg.buildImpl());
      // Assert
      CHECK(dynamic_cast< gum::aggregator::Or< double >* >(impl));
      CHECK_NOTHROW(delete impl);
    }

    static void testBuildImplAnd() {
      // Arrange
      PRMAggregate                           agg("my_agg", AggType::AND, *_boolean_, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      CHECK_NOTHROW(impl = agg.buildImpl());
      // Assert
      CHECK(dynamic_cast< gum::aggregator::And< double >* >(impl));
      CHECK_NOTHROW(delete impl);
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
  GUM_TEST_ACTIF(FirstConstructor)
  GUM_TEST_ACTIF(SecondConstructor)
  GUM_TEST_ACTIF(AggTypeNoLabel)
  GUM_TEST_ACTIF(Label)
  GUM_TEST_ACTIF(LabelOperationNotAllowed)
  GUM_TEST_ACTIF(AddParent)
  GUM_TEST_ACTIF(AddChild)
  GUM_TEST_ACTIF(Cpf)
  GUM_TEST_ACTIF(CpfConst)
  GUM_TEST_ACTIF(BuildImplMin)
  GUM_TEST_ACTIF(BuildImplMax)
  GUM_TEST_ACTIF(BuildImplExists)
  GUM_TEST_ACTIF(BuildImplForAll)
  GUM_TEST_ACTIF(BuildImplCount)
  GUM_TEST_ACTIF(BuildImplOr)
  GUM_TEST_ACTIF(BuildImplAnd)

}   // namespace gum_tests
