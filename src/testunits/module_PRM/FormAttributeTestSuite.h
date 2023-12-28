/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <gumtest/utils.h>

#include <agrum/tools/variables/labelizedVariable.h>

#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMFormAttribute.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class [[maybe_unused]] FormAttributeTestSuite: public CxxTest::TestSuite {
    private:
    using PRMAttribute = gum::prm::PRMFormAttribute< double >;

    ClassElementTestSuiteAbstract* _classEltTestSuite_;
    gum::prm::PRMType*             _boolean_;
    gum::prm::PRMType*             _state_;
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
      _class_ = new gum::prm::PRMClass< double >("dummy");
    }

    void tearDown() {
      delete _classEltTestSuite_;
      delete _boolean_;
      delete _state_;
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
      PRMAttribute      attr(*_class_, "my_attr", type);
      bool              expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsReferenceSlot(attr, expected);
    }

    GUM_ACTIVE_TEST(IsAttribute) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);
      bool              expected = true;
      // Act & Assert
      _classEltTestSuite_->testIsAttribute(attr, expected);
    }

    GUM_ACTIVE_TEST(IsSlotChain) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);
      bool              expected = false;
      // Act & Assert
      _classEltTestSuite_->testIsSlotChain(attr, expected);
    }

    GUM_ACTIVE_TEST(SetNodeId) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);
      // Act & Assert
      _classEltTestSuite_->testSetNodeId(attr);
    }

    GUM_ACTIVE_TEST(ObjType) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);
      // Act & Assert
      _classEltTestSuite_->test_obj_type(attr);
    }

    GUM_ACTIVE_TEST(SafeName) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);

      // Act & Assert
      _classEltTestSuite_->testSafeName(attr);
    }

    GUM_ACTIVE_TEST(Cast_NotAllowed) {
      // Arrange
      gum::LabelizedVariable var{"boolean", "A boolean discrete variable", 0};
      var.addLabel("False");
      var.addLabel("True");
      gum::prm::PRMType type{var};
      PRMAttribute      attr(*_class_, "my_attr", type);

      // Act & Assert
      _classEltTestSuite_->testCast_NotAllowed(attr);
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
      PRMAttribute      attr(*_class_, "my_attr", state);

      // Act & Assert
      _classEltTestSuite_->testCast(attr, boolean);
    }

    /// @}

    /// Constructor & destructor
    /// @{
    GUM_ACTIVE_TEST(ClassConstructor) {
      // Arrange
      std::string   name = "my_state";
      PRMAttribute* attr = nullptr;
      // Act & Assert
      try {
        attr = new PRMAttribute(*_class_, name, *_boolean_);
      } catch (...) {   // TS_ASSERT_THROWS_NOTHING does not work
        TS_FAIL("Exception raised.");
      }
      TS_ASSERT_THROWS_NOTHING(delete attr)
    }

    /// }

    /// Getters & setters
    /// @{
    GUM_ACTIVE_TEST(Type) {
      // Arrange
      PRMAttribute attr(*_class_, "attr", *_boolean_);
      auto&        expected = *_boolean_;
      // Act
      auto& actual = attr.type();
      // Assert
      TS_ASSERT_EQUALS(expected, actual)
      TS_ASSERT_DIFFERS(&expected, &actual)
    }

    GUM_ACTIVE_TEST(TypeConst) {
      // Arrange
      PRMAttribute attr(*_class_, "attr", *_boolean_);
      const auto&  attr_const = attr;
      const auto&  expected   = *_boolean_;
      // Act
      const auto& actual = attr_const.type();
      // Assert
      TS_ASSERT_EQUALS(expected, actual)
      TS_ASSERT_DIFFERS(&expected, &actual)
    }

    GUM_ACTIVE_TEST(Cpf) {
      // Arrange
      PRMAttribute             attr(*_class_, "attr", *_boolean_);
      gum::Potential< double > expected;
      expected << attr.type().variable();
      // Act
      auto& actual = attr.cpf();
      // Assert
      gum::Instantiation i(expected);
      gum::Instantiation j(actual);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(expected[i], actual[j])
      }
      TS_ASSERT(i.end())
      TS_ASSERT(j.end())
    }

    GUM_ACTIVE_TEST(CpfConst) {
      // Arrange
      PRMAttribute             attr(*_class_, "attr", *_boolean_);
      const auto&              attr_const = attr;
      gum::Potential< double > expected;
      expected << attr.type().variable();
      // Act
      const auto& actual = attr_const.cpf();
      // Assert
      gum::Instantiation i(expected);
      gum::Instantiation j(actual);
      for (i.setFirst(), j.setFirst(); !(i.end() || j.end()); i.inc(), j.inc()) {
        TS_ASSERT_EQUALS(expected[i], actual[j])
      }
      TS_ASSERT(i.end())
      TS_ASSERT(j.end())
    }

    /// @}

    /// Add parents and children
    /// @{
    GUM_ACTIVE_TEST(AddParentCheckChild) {
      // Arrange
      PRMAttribute parent(*_class_, "attr", *_boolean_);
      PRMAttribute child(*_class_, "attr", *_boolean_);
      auto         before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(child.addParent(parent))
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before + 1, after)
      TS_ASSERT(child.cpf().contains(parent.type().variable()))
    }

    GUM_ACTIVE_TEST(AddParentDupplicateError) {
      // Arrange
      PRMAttribute parent(*_class_, "attr", *_boolean_);
      PRMAttribute child(*_class_, "attr", *_boolean_);
      child.addParent(parent);
      auto before = child.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS(child.addParent(parent), const gum::DuplicateElement&)
      // Assert
      auto after = child.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
      TS_ASSERT(child.cpf().contains(parent.type().variable()))
    }

    GUM_ACTIVE_TEST(AddChild) {
      // Arrange
      PRMAttribute parent(*_class_, "attr", *_boolean_);
      PRMAttribute child(*_class_, "attr", *_boolean_);
      auto         before = parent.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(parent.addChild(child))
      // Assert
      auto after = parent.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
      TS_ASSERT(!parent.cpf().contains(child.type().variable()))
    }

    /// @}

    /// Get and set Cast Descendants
    /// @{
    GUM_ACTIVE_TEST(GetCastDescendant) {
      try {
        // Arrange
        PRMAttribute                      state(*_class_, "state", *_state_);
        gum::prm::PRMAttribute< double >* cast = nullptr;
        // Act
        TS_ASSERT_THROWS_NOTHING(cast = state.getCastDescendant())
        // Assert
        TS_ASSERT_DIFFERS(cast, nullptr)
        if (cast != nullptr) {
          TS_ASSERT_EQUALS(cast->type(), *_boolean_)
          TS_ASSERT_DIFFERS(&(cast->type().variable()), &(_boolean_->variable()))
          TS_ASSERT(cast->cpf().contains(cast->type().variable()))
          TS_ASSERT(cast->cpf().contains(state.type().variable()))
          TS_ASSERT_THROWS_NOTHING(delete cast)
        }
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT_THROWS_NOTHING(throw e)
      }
    }

    GUM_ACTIVE_TEST(SetAsCastDescendant) {
      // Arrange
      PRMAttribute boolean(*_class_, "boolean", *_boolean_);
      PRMAttribute state(*_class_, "state", *_state_);
      auto         before = boolean.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS_NOTHING(state.setAsCastDescendant(&boolean))
      // Assert
      auto after = boolean.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before + 1, after)
    }

    GUM_ACTIVE_TEST(SetAsCastDescendantOperationNotAllowed) {
      // Arrange
      PRMAttribute boolean(*_class_, "boolean", *_boolean_);
      PRMAttribute boolean_bis(*_class_, "boolean", *_boolean_);
      auto         before = boolean.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS(boolean_bis.setAsCastDescendant(&boolean), const gum::OperationNotAllowed&)
      // Assert
      auto after = boolean.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
    }

    GUM_ACTIVE_TEST(SetAsCastDescendantTypeError) {
      // Arrange
      gum::LabelizedVariable foovar{"Foo", "Bar", 5};
      PRMAttribute           foo(*_class_, "foobar", gum::prm::PRMType(foovar));
      PRMAttribute           state(*_class_, "state", *_state_);
      auto                   before = foo.cpf().variablesSequence().size();
      // Act
      TS_ASSERT_THROWS(state.setAsCastDescendant(&foo), const gum::TypeError&)
      // Assert
      auto after = foo.cpf().variablesSequence().size();
      TS_ASSERT_EQUALS(before, after)
    }

    /// @}
  };

}   // namespace gum_tests
