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
#include <agrum/PRM/elements/attribute.h>

#include <module_PRM/ClassElementTestSuite.h>

/**
 * This class is used to test gum::prm::ClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of 
 * gum::prm::ClassElement.
 */
namespace gum_tests {

  class PRMAttributeTestSuite : public CxxTest::TestSuite {
    private:
      typedef gum::prm::Attribute<double> Attribute;
      ClassElementTestSuite* __classEltTestSuite;
      gum::prm::Type<double> *__boolean;

    public:

      void setUp() {
        __classEltTestSuite = new ClassElementTestSuite;
        __boolean = gum::prm::Type<double>::boolean();
      }

      void tearDown() {
        delete __classEltTestSuite;
        delete __boolean;
      }

      /// ClassElement Tests 
      /// @{
      void testIsReferenceSlot() {
        // Arrange
        gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
        var.addLabel("False");
        var.addLabel("True");
        gum::prm::Type<double> type{var};
        gum::prm::Attribute<double> attr{"my_attr", type};
        bool expected = false;
        // Act & Assert
        __classEltTestSuite->testIsReferenceSlot(attr, expected);
      }

      void testIsAttribute() {
        // Arrange
        gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
        var.addLabel("False");
        var.addLabel("True");
        gum::prm::Type<double> type{var};
        gum::prm::Attribute<double> attr{"my_attr", type};
        bool expected = true;
        // Act & Assert
        __classEltTestSuite->testIsAttribute(attr, expected);
      }

      void testIsSlotChain() {
        // Arrange
        gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
        var.addLabel("False");
        var.addLabel("True");
        gum::prm::Type<double> type{var};
        gum::prm::Attribute<double> attr{"my_attr", type};
        bool expected = false;
        // Act & Assert
        __classEltTestSuite->testIsSlotChain(attr, expected);
      }

      void testSetNodeId() {
        // Arrange
        gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
        var.addLabel("False");
        var.addLabel("True");
        gum::prm::Type<double> type{var};
        gum::prm::Attribute<double> attr{"my_attr", type};
        // Act & Assert
        __classEltTestSuite->testSetNodeId(attr);
      }

      void testObjType() {
        // Arrange
        gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
        var.addLabel("False");
        var.addLabel("True");
        gum::prm::Type<double> type{var};
        gum::prm::Attribute<double> attr{"my_attr", type};
        // Act & Assert
        __classEltTestSuite->test_obj_type(attr);
      }

      void testSafeName() {
        // Arrange
        gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
        var.addLabel("False");
        var.addLabel("True");
        gum::prm::Type<double> type{var};
        gum::prm::Attribute<double> attr{"my_attr", type};

        // Act & Assert
        __classEltTestSuite->testSafeName(attr);
      }

      void testCast_NotAllowed() {
        // Arrange
        gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
        var.addLabel("False");
        var.addLabel("True");
        gum::prm::Type<double> type{var};
        gum::prm::Attribute<double> attr{"my_attr", type};

        // Act & Assert
        __classEltTestSuite->testCast_NotAllowed(attr);
      }

      void testCast() {
        // Arrange
        gum::LabelizedVariable boolean_var{"boolean", "A boolean discrete variable", 0};
        boolean_var.addLabel("False");
        boolean_var.addLabel("True");

        gum::LabelizedVariable state_var{"state", "A binary discete variable to represent a state", 0};
        state_var.addLabel("OK");
        state_var.addLabel("NOK");

        std::vector<gum::Idx> map;
        map.push_back(1);
        map.push_back(0);

        gum::prm::Type<double> boolean{boolean_var};
        gum::prm::Type<double> state{boolean, map, state_var};
        gum::prm::Attribute<double> attr{"my_attr", state};

        // Act & Assert
        __classEltTestSuite->testCast(attr, boolean);
      }
      /// @}

      /// Constructor & destructor
      /// @{
      void testClassConstructor() {
        // Arrange
        std::string name = "my_state";
        Attribute *attr = nullptr;
        // Act & Assert
        try {
          attr = new Attribute { name, *__boolean };
        } catch (...) { // TS_ASSERT_THROWS_NOTHING does not work
          TS_FAIL("Exception raised.");
        }
        TS_ASSERT_THROWS_NOTHING( delete attr );
      }

      void testInstanceConstructor() {
        // Arrange
        std::string name = "my_state";
        Attribute *attr = nullptr;
        auto *cpf = new gum::Potential<double>();
        cpf->add( __boolean->variable() );
        gum::Instantiation i { *cpf };
        for ( i.setFirst(); not i.end(); i.inc() ) {
          cpf->set( i, 1 );
        }
        cpf->normalize();
        // Act & Assert
        try {
          attr = new Attribute { name, __boolean, cpf, false };
        } catch (...) { // TS_ASSERT_THROWS_NOTHING does not work
          TS_FAIL("Exception raised.");
        }
        TS_ASSERT_THROWS_NOTHING( delete attr );
      }

      void testInstanceConstructorEraseType() {
        // Arrange
        std::string name = "my_state";
        auto boolean = gum::prm::Type<double>::boolean();
        auto *cpf = new gum::Potential<double>();
        cpf->add( boolean->variable() );
        gum::Instantiation i { *cpf };
        for ( i.setFirst(); not i.end(); i.inc() ) {
          cpf->set( i, 1 );
        }
        cpf->normalize();
        Attribute *attr = nullptr;
        // Act & Assert
        TS_ASSERT_THROWS_NOTHING( attr = new Attribute( name, boolean, cpf, true ) );
        TS_ASSERT_THROWS_NOTHING( delete attr );
      }

      void testInstanceConstructorInvalidCpf() {
        // Arrange
        std::string name = "my_state";
        auto *cpf = new gum::Potential<double>();
        cpf->add( __boolean->variable() );
        gum::Instantiation i { *cpf };
        for ( i.setFirst(); not i.end(); i.inc() ) {
          cpf->set( i, 1 );
        }
        cpf->normalize();
        auto type = gum::prm::Type<double>::boolean();
        Attribute *attr = nullptr;
        // Act & Assert
        TS_ASSERT_THROWS( attr = new Attribute(name, type, cpf, true), gum::InvalidArgument );
        // Clean up
        TS_ASSERT_EQUALS( attr, nullptr );
        delete cpf;
        delete type;
      }
      /// @}

    /// Getters & setters
    /// @{
    void testType() {
      // Arrange
      Attribute attr { "attr", *__boolean };
      auto &expected = *__boolean;
      // Act
      auto &actual = attr.type();
      // Assert
      TS_ASSERT_EQUALS( expected, actual );
      TS_ASSERT_DIFFERS( &expected, &actual );
    }

    void testTypeConst() {
      // Arrange
      Attribute attr { "attr", *__boolean };
      const auto &attr_const = attr;
      const auto &expected = *__boolean;
      // Act
      const auto &actual = attr_const.type();
      // Assert
      TS_ASSERT_EQUALS( expected, actual );
      TS_ASSERT_DIFFERS( &expected, &actual );
    }

    void testCpf() {
      // Arrange
      Attribute attr { "attr", *__boolean };
      gum::Potential<double> expected;
      expected << attr.type().variable();
      // Act
      auto &actual = attr.cpf();
      // Assert
      gum::Instantiation i(expected);
      gum::Instantiation j(actual);
      for ( i.setFirst(), j.setFirst(); not (i.end() or j.end()); i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( expected[i], actual[j] );
      }
      TS_ASSERT( i.end() );
      TS_ASSERT( j.end() );
    }

    void testCpfConst() {
      // Arrange
      Attribute attr { "attr", *__boolean };
      const auto &attr_const = attr;
      gum::Potential<double> expected;
      expected << attr.type().variable();
      // Act
      const auto &actual = attr_const.cpf();
      // Assert
      gum::Instantiation i(expected);
      gum::Instantiation j(actual);
      for ( i.setFirst(), j.setFirst(); not (i.end() or j.end()); i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( expected[i], actual[j] );
      }
      TS_ASSERT( i.end() );
      TS_ASSERT( j.end() );
    }

    /// @}
  };

} // gum_tests
