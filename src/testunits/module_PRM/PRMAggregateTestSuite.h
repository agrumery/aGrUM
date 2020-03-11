
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
    ClassElementTestSuiteAbstract*                          __classEltTestSuite;
    gum::prm::PRMType*                                      __boolean;
    gum::prm::PRMType*                                      __state;
    std::vector< AggType >*                                 __types;
    gum::prm::PRMClass< double >*                           __class;

    public:
    void setUp() {
      __classEltTestSuite = new ClassElementTestSuiteAbstract;
      __boolean = gum::prm::PRMType::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      __state = new gum::prm::PRMType{*__boolean, map, state};
      __types = new std::vector< AggType >{AggType::MIN,
                                           AggType::MAX,
                                           AggType::COUNT,
                                           AggType::EXISTS,
                                           AggType::FORALL,
                                           AggType::OR,
                                           AggType::AND};
      __class = new gum::prm::PRMClass< double >("dummy");
    }

    void tearDown() {
      delete __classEltTestSuite;
      delete __boolean;
      delete __state;
      delete __types;
      delete __class;
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
      __classEltTestSuite->testIsReferenceSlot(agg, expected);
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
      __classEltTestSuite->testIsAttribute(agg, expected);
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
      __classEltTestSuite->testIsSlotChain(agg, expected);
    }

    void testSetNodeId() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      // Act & Assert
      __classEltTestSuite->testSetNodeId(agg);
    }

    void testObjType() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};
      // Act & Assert
      __classEltTestSuite->test_obj_type(agg);
    }

    void testSafeName() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};

      // Act & Assert
      __classEltTestSuite->testSafeName(agg);
    }

    void testCast_NotAllowed() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAggregate agg{"my_agg", PRMAggregate::AggregateType::FORALL, type, 0};

      // Act & Assert
      __classEltTestSuite->testCast_NotAllowed(agg);
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
      __classEltTestSuite->testCast(agg, boolean);
    }
    /// @}

    /// Constructors & destructors
    /// @{
    void testFirstConstructor() {
      // Arrange
      PRMAggregate* agg = nullptr;
      for (auto t: *__types) {
        // Act
        TS_ASSERT_THROWS_NOTHING(agg = new PRMAggregate("my_agg", t, *__boolean));
        // Assert
        TS_ASSERT_THROWS_NOTHING(delete agg);
      }
    }

    void testSecondConstructor() {
      // Arrange
      PRMAggregate* agg = nullptr;
      for (auto t: *__types) {
        // Act
        TS_ASSERT_THROWS_NOTHING(agg =
                                    new PRMAggregate("my_agg", t, *__boolean, 0));
        // Assert
        TS_ASSERT_THROWS_NOTHING(delete agg);
      }
    }
    /// @}

    /// Getters & setters
    /// @{
    void testAggTypeNoLabel() {
      // Arrange
      for (auto t: *__types) {
        // Act
        PRMAggregate agg("my_agg", t, *__boolean);
        // Assert
        TS_ASSERT_EQUALS(agg.agg_type(), t);
      }
    }

    void testLabel() {
      // Arrange
      gum::Idx     actual = 1;
      PRMAggregate agg("my_agg", AggType::MIN, *__boolean, actual);
      gum::Idx     expected = 0;
      // Act
      TS_ASSERT_THROWS_NOTHING(expected = agg.label());
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testLabelOperationNotAllowed() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *__boolean);
      // Act
      TS_ASSERT_THROWS(agg.label(), gum::OperationNotAllowed);
      // Assert
    }

    void testAddParent() {
      // Arrange
      gum::prm::PRMFormAttribute< double > parent(*__class, "attr", *__boolean);
      PRMAggregate child("my_agg", AggType::MIN, *__boolean);
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
      gum::prm::PRMFormAttribute< double > child(*__class, "attr", *__boolean);
      PRMAggregate parent("my_agg", AggType::MIN, *__boolean);
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
      PRMAggregate agg("my_agg", AggType::MIN, *__boolean);
      // Act & Assert
      TS_ASSERT_THROWS(agg.cpf(), gum::OperationNotAllowed);
    }

    void testCpfConst() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *__boolean);
      const auto&  const_agg = agg;
      // Act & Assert
      TS_ASSERT_THROWS(const_agg.cpf(), gum::OperationNotAllowed);
    }

    void testBuildImplMin() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MIN, *__boolean);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Min< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplMax() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::MAX, *__boolean);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Max< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplExists() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::EXISTS, *__boolean, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Exists< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplForAll() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::FORALL, *__boolean, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Forall< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplCount() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::COUNT, *__boolean, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Count< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplOr() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::OR, *__boolean, 0);
      gum::MultiDimImplementation< double >* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(impl = agg.buildImpl());
      // Assert
      TS_ASSERT(dynamic_cast< gum::aggregator::Or< double >* >(impl));
      TS_ASSERT_THROWS_NOTHING(delete impl);
    }

    void testBuildImplAnd() {
      // Arrange
      PRMAggregate agg("my_agg", AggType::AND, *__boolean, 0);
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
