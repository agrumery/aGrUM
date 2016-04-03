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
#include <cxxtest/testsuite_utils.h>

#include <agrum/PRM/elements/slotChain.h>
#include <agrum/PRM/elements/formAttribute.h>
#include <agrum/PRM/elements/referenceSlot.h>
#include <agrum/PRM/elements/class.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class SlotChainTestSuite : public CxxTest::TestSuite {
    private:
    typedef gum::prm::SlotChain<double> SlotChain;
    ClassElementTestSuiteAbstract* __classEltTestSuite;
    gum::prm::PRMClass<double>* __A;
    gum::prm::PRMClass<double>* __B;
    gum::prm::PRMClass<double>* __C;
    gum::prm::Type<double>* __boolean;
    gum::prm::Type<double>* __state;
    gum::Sequence<gum::prm::PRMClassElement<double>*>* __booleanChain;
    gum::Sequence<gum::prm::PRMClassElement<double>*>* __stateChain;

    // these will be dispodes by their repsective class
    gum::prm::ReferenceSlot<double>* __refI;
    gum::prm::ReferenceSlot<double>* __refJ;
    gum::prm::ReferenceSlot<double>* __refK;
    gum::prm::PRMAttribute<double>* __boolAttr;
    gum::prm::PRMAttribute<double>* __stateAttr;

    public:
    void setUp() {
      __classEltTestSuite = new ClassElementTestSuiteAbstract();
      __A = new gum::prm::PRMClass<double>( "A" );
      __B = new gum::prm::PRMClass<double>( "B" );
      __C = new gum::prm::PRMClass<double>( "C" );

      __refI = new gum::prm::ReferenceSlot<double>( "refA", *__B );
      __A->add( __refI );
      __refJ = new gum::prm::ReferenceSlot<double>( "refB", *__C );
      __B->add( __refJ );
      __refK = new gum::prm::ReferenceSlot<double>( "refK", *__C, true );
      __B->add( __refK );

      __boolean = gum::prm::Type<double>::boolean();
      __boolAttr =
          new gum::prm::ScalarAttribute<double>( "myBool", *__boolean );
      __C->add( __boolAttr );

      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel( "OK" );
      state.addLabel( "NOK" );
      std::vector<gum::Idx> map;
      map.push_back( 1 );
      map.push_back( 0 );
      __state = new gum::prm::Type<double>( *__boolean, map, state );
      __stateAttr =
          new gum::prm::ScalarAttribute<double>( "myState", *__state );
      __C->add( __stateAttr );

      __booleanChain = new gum::Sequence<gum::prm::PRMClassElement<double>*>();
      __booleanChain->insert( __refI );
      __booleanChain->insert( __refJ );
      __booleanChain->insert( __boolAttr );

      __stateChain = new gum::Sequence<gum::prm::PRMClassElement<double>*>();
      __stateChain->insert( __refI );
      __stateChain->insert( __refK );
      __stateChain->insert( __stateAttr );
    }

    void tearDown() {
      delete __classEltTestSuite;
      delete __A;
      delete __B;
      delete __C;
      delete __boolean;
      delete __state;
      delete __booleanChain;
      delete __stateChain;
    }

    /// PRMClassElement Tests
    /// @{
    void testIsReferenceSlot() {
      // Arrange
      SlotChain slot( "slot", *__booleanChain );
      bool expected = false;
      // Act & Assert
      __classEltTestSuite->testIsReferenceSlot( slot, expected );
    }

    void testIsAttribute() {
      // Arrange
      SlotChain slot( "slot", *__booleanChain );
      bool expected = false;
      // Act & Assert
      __classEltTestSuite->testIsAttribute( slot, expected );
    }

    void testIsSlotChain() {
      // Arrange
      SlotChain slot( "slot", *__booleanChain );
      bool expected = true;
      // Act & Assert
      __classEltTestSuite->testIsSlotChain( slot, expected );
    }

    void testSetNodeId() {
      // Arrange
      SlotChain slot( "slot", *__booleanChain );
      // Act & Assert
      __classEltTestSuite->testSetNodeId( slot );
    }

    void testObjType() {
      // Arrange
      SlotChain slot( "slot", *__booleanChain );
      // Act & Assert
      __classEltTestSuite->test_obj_type( slot );
    }

    void testSafeName() {
      // Arrange
      SlotChain slot( "slot", *__booleanChain );
      // Act & Assert
      __classEltTestSuite->testSafeName( slot );
    }

    void testCast_NotAllowed() {
      // Arrange
      SlotChain slot( "slot", *__booleanChain );
      // Act & Assert
      __classEltTestSuite->testCast_NotAllowed( slot );
    }

    void testCast() {
      // Arrange
      SlotChain slot( "slot", *__stateChain );
      // Act & Assert
      __classEltTestSuite->testCast( slot, *__boolean );
    }
    /// @}

    /// Constructors & destructors
    /// @{
    void testConstructorSimple() {
      // Arrange
      SlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING( slot =
                                    new SlotChain( "slot", *__booleanChain ) );
      TS_ASSERT_THROWS_NOTHING( delete slot );
    }

    void testSecondConstructorSimple() {
      // Arrange
      SlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING( slot =
                                    new SlotChain( __booleanChain, "slot" ) );
      TS_ASSERT_THROWS_NOTHING( delete slot );
    }

    void testConstructorComplex() {
      // Arrange
      SlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING( slot = new SlotChain( "slot", *__stateChain ) );
      TS_ASSERT_THROWS_NOTHING( delete slot );
    }

    void testSecondConstructorComplex() {
      // Arrange
      SlotChain* slot = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING( slot = new SlotChain( __stateChain, "slot" ) );
      TS_ASSERT_THROWS_NOTHING( delete slot );
    }
    /// @}

    /// Methods
    /// @{
    void testIsMultiple() {
      // Arrange
      SlotChain simple( "simple", *__booleanChain );
      SlotChain multiple( "multiple", *__stateChain );
      // Act & Assert
      TS_ASSERT( not simple.isMultiple() );
      TS_ASSERT( multiple.isMultiple() );
    }

    void testLastEltSimple() {
      // Arrange
      SlotChain simple( "simple", *__booleanChain );
      gum::prm::PRMClassElement<double>* elt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( elt = &( simple.lastElt() ) );
      // Assert
      TS_ASSERT_EQUALS( __boolAttr->safeName(), elt->safeName() );
    }

    void testLastEltSimpleConst() {
      // Arrange
      SlotChain simple( "simple", *__booleanChain );
      const auto& const_simple = simple;
      gum::prm::PRMClassElement<double> const* elt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( elt = &( const_simple.lastElt() ) );
      // Assert
      TS_ASSERT_EQUALS( __boolAttr->safeName(), elt->safeName() );
    }

    void testChain() {
      // Arrange
      SlotChain simple( "simple", *__booleanChain );
      gum::Sequence<gum::prm::PRMClassElement<double>*> const* chain = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( chain = &( simple.chain() ) );
      // ASSERT
      TS_ASSERT_EQUALS( chain->size(), __booleanChain->size() );
      TS_ASSERT_EQUALS( chain->atPos( 0 ), __booleanChain->atPos( 0 ) );
      TS_ASSERT_EQUALS( chain->atPos( 1 ), __booleanChain->atPos( 1 ) );
      TS_ASSERT_DIFFERS( chain->atPos( 2 ), __booleanChain->atPos( 2 ) );
    }

    void testCpf() {
      // Arrange
      SlotChain slot( "simple", *__booleanChain );
      gum::Potential<double> expected;
      expected << slot.type().variable();
      // Act
      auto& actual = slot.cpf();
      // Assert
      gum::Instantiation i( expected );
      gum::Instantiation j( actual );
      for ( i.setFirst(), j.setFirst(); not( i.end() or j.end() );
            i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( expected[i], actual[j] );
      }
      TS_ASSERT( i.end() );
      TS_ASSERT( j.end() );
    }

    void testCpfConst() {
      // Arrange
      SlotChain slot( "simple", *__booleanChain );
      const auto& slot_const = slot;
      gum::Potential<double> expected;
      expected << slot.type().variable();
      // Act
      const auto& actual = slot_const.cpf();
      // Assert
      gum::Instantiation i( expected );
      gum::Instantiation j( actual );
      for ( i.setFirst(), j.setFirst(); not( i.end() or j.end() );
            i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( expected[i], actual[j] );
      }
      TS_ASSERT( i.end() );
      TS_ASSERT( j.end() );
    }

    /// @}

    /// Add parents and children
    /// @{
    void testAddParentCheckChild() {
      // Arrange
      gum::prm::ScalarAttribute<double> parent( "attr", *__boolean );
      SlotChain child( "child", *__booleanChain );
      auto before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING( child.addParent( parent ) );
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS( before, after );
      TS_ASSERT( not child.cpf().contains( parent.type().variable() ) );
    }

    void testAddParentDupplicateError() {
      // Arrange
      gum::prm::ScalarAttribute<double> parent( "attr", *__boolean );
      SlotChain child( "child", *__booleanChain );
      child.addParent( parent );
      auto before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING( child.addParent( parent ) );
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS( before, after );
      TS_ASSERT( not child.cpf().contains( parent.type().variable() ) );
    }

    void testAddChild() {
      // Arrange
      SlotChain parent( "simple", *__booleanChain );
      gum::prm::ScalarAttribute<double> child( "attr", *__boolean );
      auto before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING( parent.addChild( child ) );
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS( before, after );
      TS_ASSERT( not parent.cpf().contains( child.type().variable() ) );
    }
    /// @}
  };

}  // gum_tests
