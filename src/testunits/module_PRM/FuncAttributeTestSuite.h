
/***************************************************************************
*   Copyright (C) 2017 by Pierre-Henri WUILLEMIN and Christophe GONZALES   *
*   {prenom.nom}_at_lip6.fr                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
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

#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMFuncAttribute.h>
#include <agrum/variables/labelizedVariable.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class FuncAttributeTestSuite : public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMFuncAttribute< double > PRMAttribute;
    ClassElementTestSuiteAbstract*               __classEltTestSuite;
    gum::prm::PRMType< double >*                 __boolean;
    gum::prm::PRMType< double >*                 __state;

    public:
    void setUp() {
      __classEltTestSuite = new ClassElementTestSuiteAbstract;
      __boolean = gum::prm::PRMType< double >::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      __state = new gum::prm::PRMType< double >{*__boolean, map, state};
    }

    void tearDown() {
      delete __classEltTestSuite;
      delete __boolean;
      delete __state;
    }

    /// PRMClassElement Tests
    /// @{
    void testIsReferenceSlot() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType< double > type{var};
      PRMAttribute                attr("my_attr", type);
      bool                        expected = false;
      // Act & Assert
      __classEltTestSuite->testIsReferenceSlot(attr, expected);
    }

    void testIsAttribute() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType< double > type{var};
      PRMAttribute                attr("my_attr", type);
      bool                        expected = true;
      // Act & Assert
      __classEltTestSuite->testIsAttribute(attr, expected);
    }

    void testIsSlotChain() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType< double > type{var};
      PRMAttribute                attr("my_attr", type);
      bool                        expected = false;
      // Act & Assert
      __classEltTestSuite->testIsSlotChain(attr, expected);
    }

    void testSetNodeId() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType< double > type{var};
      PRMAttribute                attr("my_attr", type);
      // Act & Assert
      __classEltTestSuite->testSetNodeId(attr);
    }

    void testObjType() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType< double > type{var};
      PRMAttribute                attr("my_attr", type);
      // Act & Assert
      __classEltTestSuite->test_obj_type(attr);
    }

    void testSafeName() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType< double > type{var};
      PRMAttribute                attr("my_attr", type);

      // Act & Assert
      __classEltTestSuite->testSafeName(attr);
    }

    void testCast_NotAllowed() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType< double > type{var};
      PRMAttribute                attr("my_attr", type);

      // Act & Assert
      __classEltTestSuite->testCast_NotAllowed(attr);
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

      gum::prm::PRMType< double > boolean{boolean_var};
      gum::prm::PRMType< double > state{boolean, map, state_var};
      PRMAttribute                attr("my_attr", state);

      // Act & Assert
      __classEltTestSuite->testCast(attr, boolean);
    }
    /// @}

    /// Constructor & destructor
    /// @{
    void testClassConstructor() {
      // Arrange
      std::string   name = "my_state";
      PRMAttribute* attr = nullptr;
      // Act & Assert
      try {
        attr = new PRMAttribute(name, *__boolean);
      } catch (...) {  // TS_ASSERT_THROWS_NOTHING does ! work
        TS_FAIL("Exception raised.");
      }
      TS_ASSERT_THROWS_NOTHING(delete attr);
    }
    /// }

    /// Getters & setters
    /// @{
    void testType() {
      // Arrange
      PRMAttribute attr("attr", *__boolean);
      auto&        expected = *__boolean;
      // Act
      auto& actual = attr.type();
      // Assert
      TS_ASSERT_EQUALS(expected, actual);
      TS_ASSERT_DIFFERS(&expected, &actual);
    }

    void testTypeConst() {
      // Arrange
      PRMAttribute attr("attr", *__boolean);
      const auto&  attr_const = attr;
      const auto&  expected = *__boolean;
      // Act
      const auto& actual = attr_const.type();
      // Assert
      TS_ASSERT_EQUALS(expected, actual);
      TS_ASSERT_DIFFERS(&expected, &actual);
    }

    void testCpf() {
      // Arrange
      PRMAttribute             attr("attr", *__boolean);
      gum::Potential< double > expected;
      expected << attr.type().variable();
      // Act
      auto& actual = attr.cpf();
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
      PRMAttribute             attr("attr", *__boolean);
      const auto&              attr_const = attr;
      gum::Potential< double > expected;
      expected << attr.type().variable();
      // Act
      const auto& actual = attr_const.cpf();
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
      PRMAttribute parent("attr", *__boolean);
      PRMAttribute child("attr", *__boolean);
      auto         before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent));
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
      TS_ASSERT(!child.cpf().contains(parent.type().variable()));
    }

    void testAddParentDupplicateError() {
      // Arrange
      PRMAttribute parent("attr", *__boolean);
      PRMAttribute child("attr", *__boolean);
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
      PRMAttribute parent("attr", *__boolean);
      PRMAttribute child("attr", *__boolean);
      auto         before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(parent.addChild(child));
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
      TS_ASSERT(!parent.cpf().contains(child.type().variable()));
    }
    /// @}

    /// Get and set Cast Descendants
    /// @{
    void testGetCastDescendant() {
      // Arrange
      PRMAttribute                      state("state", *__state);
      gum::prm::PRMAttribute< double >* cast = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cast = state.getCastDescendant());
      // Assert
      TS_ASSERT_DIFFERS(cast, nullptr);
      TS_ASSERT_EQUALS(cast->type(), *__boolean);
      TS_ASSERT_DIFFERS(&(cast->type().variable()), &(__boolean->variable()));
      TS_ASSERT(cast->cpf().contains(cast->type().variable()));
      TS_ASSERT(cast->cpf().contains(state.type().variable()));
      TS_ASSERT_THROWS_NOTHING(delete cast);
    }

    void testSetAsCastDescendant() {
      // Arrange
      PRMAttribute boolean("boolean", *__boolean);
      PRMAttribute state("state", *__state);
      auto         before = boolean.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(state.setAsCastDescendant(&boolean));
      // Assert
      auto after = boolean.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before + 1, after);
    }

    void testSetAsCastDescendantOperationNotAllowed() {
      // Arrange
      PRMAttribute boolean("boolean", *__boolean);
      PRMAttribute boolean_bis("boolean", *__boolean);
      auto         before = boolean.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS(boolean_bis.setAsCastDescendant(&boolean),
                       gum::OperationNotAllowed);
      // Assert
      auto after = boolean.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
    }

    void testSetAsCastDescendantTypeError() {
      // Arrange
      gum::LabelizedVariable foovar{"Foo", "Bar", 5};
      PRMAttribute           foo("foobar", gum::prm::PRMType< double >(foovar));
      PRMAttribute           state("state", *__state);
      auto                   before = foo.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS(state.setAsCastDescendant(&foo), gum::WrongType);
      // Assert
      auto after = foo.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
    }

    /// @}
  };

}  // gum_tests
