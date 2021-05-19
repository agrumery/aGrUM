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
#include <agrum/PRM/elements/PRMFuncAttribute.h>
#include <agrum/tools/variables/labelizedVariable.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class FuncAttributeTestSuite: public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMFuncAttribute< double > PRMAttribute;
    ClassElementTestSuiteAbstract*               classEltTestSuite__;
    gum::prm::PRMType*                           boolean__;
    gum::prm::PRMType*                           state__;

    public:
    void setUp() {
      classEltTestSuite__ = new ClassElementTestSuiteAbstract;
      boolean__           = gum::prm::PRMType::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      state__ = new gum::prm::PRMType{*boolean__, map, state};
    }

    void tearDown() {
      delete classEltTestSuite__;
      delete boolean__;
      delete state__;
    }

    /// PRMClassElement Tests
    /// @{
    void testIsReferenceSlot() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr("my_attr", type);
      bool              expected = false;
      // Act & Assert
      classEltTestSuite__->testIsReferenceSlot(attr, expected);
    }

    void testIsAttribute() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr("my_attr", type);
      bool              expected = true;
      // Act & Assert
      classEltTestSuite__->testIsAttribute(attr, expected);
    }

    void testIsSlotChain() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr("my_attr", type);
      bool              expected = false;
      // Act & Assert
      classEltTestSuite__->testIsSlotChain(attr, expected);
    }

    void testSetNodeId() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr("my_attr", type);
      // Act & Assert
      classEltTestSuite__->testSetNodeId(attr);
    }

    void testObjType() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr("my_attr", type);
      // Act & Assert
      classEltTestSuite__->test_obj_type(attr);
    }

    void testSafeName() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr("my_attr", type);

      // Act & Assert
      classEltTestSuite__->testSafeName(attr);
    }

    void testCast_NotAllowed() {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr("my_attr", type);

      // Act & Assert
      classEltTestSuite__->testCast_NotAllowed(attr);
    }

    void testCast() {
      // Arrange
      gum::LabelizedVariable boolean_var{"boolean",
                                         "A boolean discrete variable",
                                         0};
      boolean_var.addLabel("False");
      boolean_var.addLabel("True");

      gum::LabelizedVariable state_var{
         "state",
         "A binary discete variable to represent a state",
         0};
      state_var.addLabel("OK");
      state_var.addLabel("NOK");

      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);

      gum::prm::PRMType boolean{boolean_var};
      gum::prm::PRMType state{boolean, map, state_var};
      PRMAttribute      attr("my_attr", state);

      // Act & Assert
      classEltTestSuite__->testCast(attr, boolean);
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
        attr = new PRMAttribute(name, *boolean__);
      } catch (...) {   // TS_ASSERT_THROWS_NOTHING does ! work
        TS_FAIL("Exception raised.");
      }
      TS_ASSERT_THROWS_NOTHING(delete attr);
    }
    /// }

    /// Getters & setters
    /// @{
    void testType() {
      // Arrange
      PRMAttribute attr("attr", *boolean__);
      auto&        expected = *boolean__;
      // Act
      auto& actual = attr.type();
      // Assert
      TS_ASSERT_EQUALS(expected, actual);
      TS_ASSERT_DIFFERS(&expected, &actual);
    }

    void testTypeConst() {
      // Arrange
      PRMAttribute attr("attr", *boolean__);
      const auto&  attr_const = attr;
      const auto&  expected   = *boolean__;
      // Act
      const auto& actual = attr_const.type();
      // Assert
      TS_ASSERT_EQUALS(expected, actual);
      TS_ASSERT_DIFFERS(&expected, &actual);
    }

    void testCpf() {
      // Arrange
      PRMAttribute             attr("attr", *boolean__);
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
      PRMAttribute             attr("attr", *boolean__);
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
      PRMAttribute parent("attr", *boolean__);
      PRMAttribute child("attr", *boolean__);
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
      PRMAttribute parent("attr", *boolean__);
      PRMAttribute child("attr", *boolean__);
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
      PRMAttribute parent("attr", *boolean__);
      PRMAttribute child("attr", *boolean__);
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
      PRMAttribute                      state("state", *state__);
      gum::prm::PRMAttribute< double >* cast = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(cast = state.getCastDescendant());
      // Assert
      TS_ASSERT_DIFFERS(cast, nullptr);
      TS_ASSERT_EQUALS(cast->type(), *boolean__);
      TS_ASSERT_DIFFERS(&(cast->type().variable()), &(boolean__->variable()));
      TS_ASSERT(cast->cpf().contains(cast->type().variable()));
      TS_ASSERT(cast->cpf().contains(state.type().variable()));
      TS_ASSERT_THROWS_NOTHING(delete cast);
    }

    void testSetAsCastDescendant() {
      // Arrange
      PRMAttribute boolean("boolean", *boolean__);
      PRMAttribute state("state", *state__);
      auto         before = boolean.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(state.setAsCastDescendant(&boolean));
      // Assert
      auto after = boolean.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before + 1, after);
    }

    void testSetAsCastDescendantOperationNotAllowed() {
      // Arrange
      PRMAttribute boolean("boolean", *boolean__);
      PRMAttribute boolean_bis("boolean", *boolean__);
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
      PRMAttribute           foo("foobar", gum::prm::PRMType(foovar));
      PRMAttribute           state("state", *state__);
      auto                   before = foo.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS(state.setAsCastDescendant(&foo), gum::TypeError);
      // Assert
      auto after = foo.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after);
    }

    /// @}
  };

}   // namespace gum_tests
