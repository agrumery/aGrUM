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

#include <agrum/tools/multidim/aggregators/and.h>
#include <agrum/tools/multidim/aggregators/count.h>
#include <agrum/tools/multidim/aggregators/or.h>
#include <agrum/tools/variables/labelizedVariable.h>

#include <agrum/PRM/elements/PRMAggregate.h>
#include <agrum/PRM/elements/PRMFormAttribute.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class PRMAggregateTestSuite: public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMAggregate< double >                PRMAggregate;
    typedef gum::prm::PRMAggregate< double >::AggregateType AggType;
    ClassElementTestSuiteAbstract*                          classEltTestSuite__;
    gum::prm::PRMType*                                      boolean__;
    gum::prm::PRMType*                                      state__;
    std::vector< AggType >*                                 types__;
    gum::prm::PRMClass< double >*                           class__;

    public:
    void setUp() {
      classEltTestSuite__ = new ClassElementTestSuiteAbstract;
      boolean__ = gum::prm::PRMType::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      state__ = new gum::prm::PRMType{*boolean__, map, state};
      types__ = new std::vector< AggType >{AggType::MIN,
                                           AggType::MAX,
                                           AggType::COUNT,
                                           AggType::EXISTS,
                                           AggType::FORALL,
                                           AggType::OR,
                                           AggType::AND};
      class__ = new gum::prm::PRMClass< double >("dummy");
    }

    void tearDown() {
      delete classEltTestSuite__;
      delete boolean__;
      delete state__;
      delete types__;
      delete class__;
    }

    /// PRMClassElement Tests
    /// @{
    void testIsReferenceSlot() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      bool         expected = false;
      // Act & Assert
      classEltTestSuite__->testIsReferenceSlot(agg, expected);
    }

    void testIsAttribute() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      bool         expected = false;
      // Act & Assert
      classEltTestSuite__->testIsAttribute(agg, expected);
    }

    void testIsSlotChain() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      bool         expected = false;
      // Act & Assert
      classEltTestSuite__->testIsSlotChain(agg, expected);
    }

    void testSetNodeId() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      // Act & Assert
      classEltTestSuite__->testSetNodeId(agg);
    }

    void testObjType() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      // Act & Assert
      classEltTestSuite__->test_obj_type(agg);
    }

    void testSafeName() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};

      // Act & Assert
      classEltTestSuite__->testSafeName(agg);
    }

    void testCast_NotAllowed() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};

      // Act & Assert
      classEltTestSuite__->testCast_NotAllowed(agg);
    }

    void testCast() {
      // Arrange
      gum::LabelizedVariable boolean_var{
         "boolean", "A boolean discrete variable", 0};
      boolean_var.addLabel("False");
      boolean_var.addLabel("True");

      gum::LabelizedVariable state_var{
         "state", "A binary discete variable to represent a state", 0};
      state_var.addLabel("OK");
      state_var.addLabel("NOK");

      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);

      gum::prm::PRMType boolean{boolean_var};
      gum::prm::PRMType state{boolean, map, state_var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, state, 0};

      // Act & Assert
      classEltTestSuite__->testCast(agg, boolean);
    }
    /// @}

    /// Constructors & destructors
    /// @{
    void testFirstConstructor() {
      // Arrange
      PRMAggregate* agg = nullptr;
      for (auto t: *types__) {
        // Act
        TS_ASSERT_THROWS_NOTHING(agg = new PRMAggregate("my_agg", t, *boolean__));
        // Assert
        TS_ASSERT_THROWS_NOTHING(delete agg);
      }
    }

    void testSecondConstructor() {
      // Arrange
      PRMAggregate* agg = nullptr;
      for (auto t: *types__) {
        // Act
        TS_ASSERT_THROWS_NOTHING(agg =
                                    new PRMAggregate("my_agg", t, *boolean__, 0));
        // Assert
        TS_ASSERT_THROWS_NOTHING(delete agg);
      }
    }
    /// @}

    /// Getters & setters
    /// @{
    void testAggTypeNoLabel() {
      // Arrange
      for (auto t: *types__) {
        // Act
        PRMAggregate agg("my_agg", t, *boolean__);
        // Assert
        TS_ASSERT_EQUALS(agg.agg_type(), t);
      }
    }

    void testLabel() {
      // Arrange
      gum::Idx     actual = 1;
      PRMAggregate agg("my_agg", AggType::MIN, *boolean__, actual);
      gum::Idx     expected = 0;
      // Act
      TS_ASSERT_THROWS_NOTHING(expected = agg.label());
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testLabelOperationNotAllowed() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *boolean__);
      // Act
      TS_ASSERT_THROWS(agg.label(), gum::OperationNotAllowed);
      // Assert
    }

    void testAddParent() {
      // Arrange
      gum::prm::PRMFormAttribute< double > parent(*class__, "attr", *boolean__);
      PRMAggregate child("my_agg", AggType::MIN, *boolean__);
      auto         before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent));
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
      TS_ASSERT(!parent.cpf().contains(child.type().variable()));
    }

    void testAddChild() {
      // Arrange
      gum::prm::PRMFormAttribute< double > child(*class__, "attr", *boolean__);
      PRMAggregate parent("my_agg", AggType::MIN, *boolean__);
      auto         before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(parent.addChild(child));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
      TS_ASSERT(!child.cpf().contains(parent.type().variable()));
    }

    void testCpf() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *boolean__);
      // Act & Assert
      TS_ASSERT_THROWS(agg.cpf(), gum::OperationNotAllowed);
    }

    void testCpfConst() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *boolean__);
      const auto&  const_agg = agg;
      // Act & Assert
      TS_ASSERT_THROWS(const_agg.cpf(), gum::OperationNotAllowed);
    }

    void testBuildImplMin() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *boolean__);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Min< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplMax() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MAX, *boolean__);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Max< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplExists() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::EXISTS, *boolean__, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Exists< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplForAll() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::FORALL, *boolean__, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Forall< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplCount() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::COUNT, *boolean__, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Count< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplOr() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::OR, *boolean__, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Or< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplAnd() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::AND, *boolean__, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::And< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }
    /// @}
  };

}   // namespace gum_tests
