/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/aggregators/or.h>
#include <agrum/multidim/aggregators/and.h>
#include <agrum/multidim/aggregators/count.h>

#include <agrum/PRM/elements/formAttribute.h>
#include <agrum/PRM/elements/aggregate.h>

#include <module_PRM/ClassElementTestSuite.h>

/**
 * This class is used to test gum::prm::ClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::ClassElement.
 */
namespace gum_tests {

  class PRMAggregateTestSuite : public CxxTest::TestSuite {
    private:
    typedef gum::prm::Aggregate<double> Aggregate;
    typedef gum::prm::Aggregate<double>::AggregateType AggType;
    ClassElementTestSuite* __classEltTestSuite;
    gum::prm::Type<double>* __boolean;
    gum::prm::Type<double>* __state;
    std::vector<AggType>* __types;
    gum::prm::Class<double>* __class;

    public:
    void setUp() {
      __classEltTestSuite = new ClassElementTestSuite;
      __boolean = gum::prm::Type<double>::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel( "OK" );
      state.addLabel( "NOK" );
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      __state = new gum::prm::Type<double>{*__boolean, map, state};
      __types = new std::vector<AggType>{
          AggType::MIN,    AggType::MAX,    AggType::MEAN, AggType::COUNT,
          AggType::EXISTS, AggType::FORALL, AggType::OR,   AggType::AND};
      __class = new gum::prm::Class<double>( "dummy" );
    }

    void tearDown() {
      delete __classEltTestSuite;
      delete __boolean;
      delete __state;
      delete __types;
      delete __class;
    }

    /// ClassElement Tests
    /// @{
    void testIsReferenceSlot() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel( "False" );
      var.addLabel( "True" );
      gum::prm::Type<double> type{var};
      Aggregate agg{"my_agg", Aggregate::AggregateType::FORALL, type, 0};
      bool expected = false;
      // Act & Assert
      __classEltTestSuite->testIsReferenceSlot( agg, expected );
    }

    void testIsAttribute() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel( "False" );
      var.addLabel( "True" );
      gum::prm::Type<double> type{var};
      Aggregate agg{"my_agg", Aggregate::AggregateType::FORALL, type, 0};
      bool expected = false;
      // Act & Assert
      __classEltTestSuite->testIsAttribute( agg, expected );
    }

    void testIsSlotChain() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel( "False" );
      var.addLabel( "True" );
      gum::prm::Type<double> type{var};
      Aggregate agg{"my_agg", Aggregate::AggregateType::FORALL, type, 0};
      bool expected = false;
      // Act & Assert
      __classEltTestSuite->testIsSlotChain( agg, expected );
    }

    void testSetNodeId() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel( "False" );
      var.addLabel( "True" );
      gum::prm::Type<double> type{var};
      Aggregate agg{"my_agg", Aggregate::AggregateType::FORALL, type, 0};
      // Act & Assert
      __classEltTestSuite->testSetNodeId( agg );
    }

    void testObjType() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel( "False" );
      var.addLabel( "True" );
      gum::prm::Type<double> type{var};
      Aggregate agg{"my_agg", Aggregate::AggregateType::FORALL, type, 0};
      // Act & Assert
      __classEltTestSuite->test_obj_type( agg );
    }

    void testSafeName() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel( "False" );
      var.addLabel( "True" );
      gum::prm::Type<double> type{var};
      Aggregate agg{"my_agg", Aggregate::AggregateType::FORALL, type, 0};

      // Act & Assert
      __classEltTestSuite->testSafeName( agg );
    }

    void testCast_NotAllowed() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel( "False" );
      var.addLabel( "True" );
      gum::prm::Type<double> type{var};
      Aggregate agg{"my_agg", Aggregate::AggregateType::FORALL, type, 0};

      // Act & Assert
      __classEltTestSuite->testCast_NotAllowed( agg );
    }

    void testCast() {
      // Arrange
      gum::LabelizedVariable boolean_var{"boolean",
                                         "A boolean discrete variable", 0};
      boolean_var.addLabel( "False" );
      boolean_var.addLabel( "True" );

      gum::LabelizedVariable state_var{
          "state", "A binary discete variable to represent a state", 0};
      state_var.addLabel( "OK" );
      state_var.addLabel( "NOK" );

      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );

      gum::prm::Type<double> boolean{boolean_var};
      gum::prm::Type<double> state{boolean, map, state_var};
      Aggregate agg{"my_agg", Aggregate::AggregateType::FORALL, state, 0};

      // Act & Assert
      __classEltTestSuite->testCast( agg, boolean );
    }
    /// @}


    /// Constructors & destructors
    /// @{
    void testFirstConstructor() {
      // Arrange
      Aggregate* agg = nullptr;
      for ( auto t : *__types ) {
        // Act
        TS_ASSERT_THROWS_NOTHING(
            agg = new Aggregate( "my_agg", t, *__boolean ) );
        // Assert
        TS_ASSERT_THROWS_NOTHING( delete agg );
      }
    }

    void testSecondConstructor() {
      // Arrange
      Aggregate* agg = nullptr;
      for ( auto t : *__types ) {
        // Act
        TS_ASSERT_THROWS_NOTHING(
            agg = new Aggregate( "my_agg", t, *__boolean, 0 ) );
        // Assert
        TS_ASSERT_THROWS_NOTHING( delete agg );
      }
    }
    /// @}

    /// Getters & setters
    /// @{
    void testAggTypeNoLabel() {
      // Arrange
      for ( auto t : *__types ) {
        // Act
        Aggregate agg( "my_agg", t, *__boolean );
        // Assert
        TS_ASSERT_EQUALS( agg.agg_type(), t );
      }
    }

    void testLabel() {
      // Arrange
      gum::Idx actual = 1;
      Aggregate agg( "my_agg", AggType::MIN, *__boolean, actual );
      gum::Idx expected = 0;
      // Act
      TS_ASSERT_THROWS_NOTHING( expected = agg.label() );
      // Assert
      TS_ASSERT_EQUALS( actual, expected );
    }

    void testLabelOperationNotAllowed() {
      // Arrange
      Aggregate agg( "my_agg", AggType::MIN, *__boolean );
      // Act
      TS_ASSERT_THROWS( agg.label(), gum::OperationNotAllowed );
      // Assert
    }

    void testAddParent() {
      // Arrange
      gum::prm::FormAttribute<double> parent( *__class, "attr", *__boolean );
      Aggregate child( "my_agg", AggType::MIN, *__boolean );
      auto before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING( child.addParent( parent ) );
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS( before, after );
      TS_ASSERT( not parent.cpf().contains( child.type().variable() ) );
    }

    void testAddChild() {
      // Arrange
      gum::prm::FormAttribute<double> child( *__class, "attr", *__boolean );
      Aggregate parent( "my_agg", AggType::MIN, *__boolean );
      auto before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING( parent.addChild( child ) );
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS( before, after );
      TS_ASSERT( not child.cpf().contains( parent.type().variable() ) );
    }

    void testCpf() {
      // Arrange
      Aggregate agg( "my_agg", AggType::MIN, *__boolean );
      // Act & Assert
      TS_ASSERT_THROWS( agg.cpf(), gum::OperationNotAllowed );
    }

    void testCpfConst() {
      // Arrange
      Aggregate agg( "my_agg", AggType::MIN, *__boolean );
      const auto& const_agg = agg;
      // Act & Assert
      TS_ASSERT_THROWS( const_agg.cpf(), gum::OperationNotAllowed );
    }

    void testBuildImplMin() {
      // Arrange
      Aggregate agg( "my_agg", AggType::MIN, *__boolean );
      gum::MultiDimImplementation<double>* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( impl = agg.buildImpl() );
      // Assert
      TS_ASSERT( dynamic_cast<gum::aggregator::Min<double>*>( impl ) );
      TS_ASSERT_THROWS_NOTHING( delete impl );
    }

    void testBuildImplMax() {
      // Arrange
      Aggregate agg( "my_agg", AggType::MAX, *__boolean );
      gum::MultiDimImplementation<double>* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( impl = agg.buildImpl() );
      // Assert
      TS_ASSERT( dynamic_cast<gum::aggregator::Max<double>*>( impl ) );
      TS_ASSERT_THROWS_NOTHING( delete impl );
    }

    void testBuildImplExists() {
      // Arrange
      Aggregate agg( "my_agg", AggType::EXISTS, *__boolean, 0 );
      gum::MultiDimImplementation<double>* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( impl = agg.buildImpl() );
      // Assert
      TS_ASSERT( dynamic_cast<gum::aggregator::Exists<double>*>( impl ) );
      TS_ASSERT_THROWS_NOTHING( delete impl );
    }

    void testBuildImplForAll() {
      // Arrange
      Aggregate agg( "my_agg", AggType::FORALL, *__boolean, 0 );
      gum::MultiDimImplementation<double>* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( impl = agg.buildImpl() );
      // Assert
      TS_ASSERT( dynamic_cast<gum::aggregator::Forall<double>*>( impl ) );
      TS_ASSERT_THROWS_NOTHING( delete impl );
    }

    void testBuildImplCount() {
      // Arrange
      Aggregate agg( "my_agg", AggType::COUNT, *__boolean, 0 );
      gum::MultiDimImplementation<double>* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( impl = agg.buildImpl() );
      // Assert
      TS_ASSERT( dynamic_cast<gum::aggregator::Count<double>*>( impl ) );
      TS_ASSERT_THROWS_NOTHING( delete impl );
    }

    void testBuildImplMean() {
      // Arrange
      Aggregate agg( "my_agg", AggType::MEAN, *__boolean, 0 );
      gum::MultiDimImplementation<double>* impl = nullptr;
      // Act
      TS_ASSERT_THROWS( impl = agg.buildImpl(), gum::OperationNotAllowed );
      // Assert
      TS_ASSERT_EQUALS( impl, nullptr );
    }

    void testBuildImplOr() {
      // Arrange
      Aggregate agg( "my_agg", AggType::OR, *__boolean, 0 );
      gum::MultiDimImplementation<double>* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( impl = agg.buildImpl() );
      // Assert
      TS_ASSERT( dynamic_cast<gum::aggregator::Or<double>*>( impl ) );
      TS_ASSERT_THROWS_NOTHING( delete impl );
    }

    void testBuildImplAnd() {
      // Arrange
      Aggregate agg( "my_agg", AggType::AND, *__boolean, 0 );
      gum::MultiDimImplementation<double>* impl = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( impl = agg.buildImpl() );
      // Assert
      TS_ASSERT( dynamic_cast<gum::aggregator::And<double>*>( impl ) );
      TS_ASSERT_THROWS_NOTHING( delete impl );
    }
    /// @}
  };

}  // gum_tests
