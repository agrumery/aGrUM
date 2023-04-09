/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMType.h>
#include <agrum/tools/core/exceptions.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class [[maybe_unused]] PRMTypeTestSuite: public CxxTest::TestSuite {
    using PRMType = gum::prm::PRMType;

    gum::LabelizedVariable* _boolean_;
    gum::LabelizedVariable* _state_;

    public:
    void setUp() {
      _boolean_ = new gum::LabelizedVariable{"boolean", "Boolean variable", 0};
      _boolean_->addLabel("false");
      _boolean_->addLabel("true");

      _state_ = new gum::LabelizedVariable{"state", "State variable", 0};
      _state_->addLabel("OK");
      _state_->addLabel("NOK");
    }

    void tearDown() {
      delete _boolean_;
      delete _state_;
    }

    GUM_ACTIVE_TEST(StaticBoolean) {
      // Arrange
      std::string labels[]   = {"false", "true"};
      std::string name       = "boolean";
      std::string decription = "Boolean variable";
      // Act
      auto boolean = PRMType::boolean();
      // Assert
      TS_ASSERT_EQUALS(boolean->variable().domainSize(), (gum::Size)2)
      TS_ASSERT_EQUALS(boolean->variable().label(0), labels[0])
      TS_ASSERT_EQUALS(boolean->variable().label(1), labels[1])
      delete boolean;
    }

    GUM_ACTIVE_TEST(Constructor) {
      // Arrange
      PRMType* type = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(type = new PRMType{*_boolean_})
      // Assert
      TS_ASSERT_THROWS_NOTHING(delete type)
    }

    GUM_ACTIVE_TEST(SuperConstructor) {
      // Arrange
      PRMType                 boolean{*_boolean_};
      PRMType*                state = nullptr;
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      // Act
      try {
        state = new PRMType{boolean, map, *_state_};
      } catch (...) {
        // TS_ASSERT_THROWS_NOTHING does ! work here
        TS_FAIL("Exception thrown");
      }
      // Assert
      TS_ASSERT_THROWS_NOTHING(delete state)
    }

    GUM_ACTIVE_TEST(CopyConstructor) {
      // Arrange
      PRMType  boolean{*_boolean_};
      PRMType* copy = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(copy = new PRMType{boolean})
      // Assert
      TS_ASSERT_THROWS_NOTHING(delete copy)
    }

    GUM_ACTIVE_TEST(GetVariable) {
      // Arrange
      PRMType                boolean{*_boolean_};
      gum::DiscreteVariable* variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(variable = &(boolean.variable()))
      // Assert
      TS_ASSERT_DIFFERS(variable->name().find(_boolean_->name()), std::string::npos)
      TS_ASSERT_EQUALS(variable->description(), _boolean_->description())
      TS_ASSERT_EQUALS(variable->label(0), _boolean_->label(0))
      TS_ASSERT_EQUALS(variable->label(1), _boolean_->label(1))
      TS_ASSERT_EQUALS(variable->domainSize(), _boolean_->domainSize())
      TS_ASSERT_DIFFERS(variable, _boolean_)
    }

    GUM_ACTIVE_TEST(GetVariableConst) {
      // Arrange
      PRMType                      boolean{*_boolean_};
      const PRMType&               const_boolean = boolean;
      gum::DiscreteVariable const* variable      = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(variable = &(const_boolean.variable()))
      // Assert
      TS_ASSERT_DIFFERS(variable->name().find(_boolean_->name()), std::string::npos)
      TS_ASSERT_EQUALS(variable->description(), _boolean_->description())
      TS_ASSERT_EQUALS(variable->label(0), _boolean_->label(0))
      TS_ASSERT_EQUALS(variable->label(1), _boolean_->label(1))
      TS_ASSERT_EQUALS(variable->domainSize(), _boolean_->domainSize())
      TS_ASSERT_DIFFERS(variable, _boolean_)
    }

    GUM_ACTIVE_TEST(IndirectionOperator) {
      // Arrange
      PRMType                boolean{*_boolean_};
      gum::DiscreteVariable* variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(variable = &(*boolean))
      // Assert
      TS_ASSERT_DIFFERS(variable->name().find(_boolean_->name()), std::string::npos)
      TS_ASSERT_EQUALS(variable->description(), _boolean_->description())
      TS_ASSERT_EQUALS(variable->label(0), _boolean_->label(0))
      TS_ASSERT_EQUALS(variable->label(1), _boolean_->label(1))
      TS_ASSERT_EQUALS(variable->domainSize(), _boolean_->domainSize())
      TS_ASSERT_DIFFERS(variable, _boolean_)
    }

    GUM_ACTIVE_TEST(IndirectionOperatorConst) {
      // Arrange
      PRMType                      boolean{*_boolean_};
      const PRMType&               const_boolean = boolean;
      gum::DiscreteVariable const* variable      = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(variable = &(*const_boolean))
      // Assert
      TS_ASSERT_DIFFERS(variable->name().find(_boolean_->name()), std::string::npos)
      TS_ASSERT_EQUALS(variable->description(), _boolean_->description())
      TS_ASSERT_EQUALS(variable->label(0), _boolean_->label(0))
      TS_ASSERT_EQUALS(variable->label(1), _boolean_->label(1))
      TS_ASSERT_EQUALS(variable->domainSize(), _boolean_->domainSize())
      TS_ASSERT_DIFFERS(variable, _boolean_)
    }

    GUM_ACTIVE_TEST(EqualityOperator) {
      // Arrange
      PRMType a{*_boolean_};
      PRMType b{*_boolean_};
      PRMType c{*_state_};
      // Act & Assert
      TS_ASSERT_EQUALS(a, a)
      TS_ASSERT_EQUALS(a, b)
      TS_ASSERT_EQUALS(b, a)

      TS_ASSERT_DIFFERS(c, a)
      TS_ASSERT_DIFFERS(a, c)
      TS_ASSERT_DIFFERS(c, b)
      TS_ASSERT_DIFFERS(b, c)
    }

    GUM_ACTIVE_TEST(InequalityOperator) {
      // Arrange
      PRMType a{*_boolean_};
      PRMType b{*_boolean_};
      PRMType c{*_state_};

      // Act & Assert
      TS_ASSERT_EQUALS(a, a)
      TS_ASSERT_EQUALS(a, b)
      TS_ASSERT_EQUALS(b, a)

      TS_ASSERT_DIFFERS(c, a)
      TS_ASSERT_DIFFERS(a, c)
      TS_ASSERT_DIFFERS(c, b)
      TS_ASSERT_DIFFERS(b, c)
    }

    GUM_ACTIVE_TEST(ObjType) {
      // Arrange
      PRMType boolean{*_boolean_};
      auto    expected = gum::prm::PRMObject::prm_type::TYPE;
      // Act
      auto actual = boolean.obj_type();
      // Assert
      TS_ASSERT_EQUALS(expected, actual)
    }

    GUM_ACTIVE_TEST(Name) {
      // Arrange
      PRMType boolean{*_boolean_};
      auto    expected = _boolean_->name();
      // Act
      auto actual = boolean.name();
      // Assert
      TS_ASSERT_EQUALS(expected, actual)
    }

    GUM_ACTIVE_TEST(IsSubType) {
      // Arrange
      PRMType                 boolean{*_boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, *_state_};
      // Act & Assert
      TS_ASSERT(state.isSubType())
      TS_ASSERT(!boolean.isSubType())
    }

    GUM_ACTIVE_TEST(IsSubTypeOf) {
      // Arrange
      PRMType                 boolean{*_boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, *_state_};
      PRMType dummy{*_state_};
      // Act & Assert
      TS_ASSERT(state.isSubTypeOf(boolean))
      TS_ASSERT(state.isSubTypeOf(state))
      TS_ASSERT(!boolean.isSubTypeOf(state))
      TS_ASSERT(!dummy.isSubTypeOf(boolean))
      TS_ASSERT(!boolean.isSubTypeOf(dummy))
      TS_ASSERT(dummy.isSubTypeOf(state))
      TS_ASSERT(state.isSubTypeOf(dummy))
    }

    GUM_ACTIVE_TEST(IsSuperTypeOf) {
      // Arrange
      PRMType                 boolean{*_boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, *_state_};
      PRMType dummy{*_state_};
      // Act & Assert
      TS_ASSERT(boolean.isSuperTypeOf(state))
      TS_ASSERT(state.isSuperTypeOf(state))
      TS_ASSERT(!state.isSuperTypeOf(boolean))
      TS_ASSERT(!dummy.isSuperTypeOf(boolean))
      TS_ASSERT(!boolean.isSuperTypeOf(dummy))
      TS_ASSERT(dummy.isSuperTypeOf(state))
      TS_ASSERT(state.isSuperTypeOf(dummy))
    }

    GUM_ACTIVE_TEST(Super) {
      // Arrange
      PRMType                 boolean{*_boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType  state{boolean, map, *_state_};
      PRMType* super = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(super = &(state.superType()))
      // Act & Assert
      TS_ASSERT_EQUALS(*super, boolean)
    }

    GUM_ACTIVE_TEST(SuperConst) {
      // Arrange
      PRMType                 boolean{*_boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType        state{boolean, map, *_state_};
      const auto&    dummy = state;
      PRMType const* super = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(super = &(dummy.superType()))
      // Act & Assert
      TS_ASSERT_EQUALS(*super, boolean)
    }

    GUM_ACTIVE_TEST(SuperNotFound) {
      // Arrange
      PRMType boolean{*_boolean_};
      // Act & Assert
      TS_ASSERT_THROWS(boolean.superType(), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(SetSuper) {
      // Arrange
      PRMType                 boolean{*_boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, *_state_};
      PRMType boolean_bis{*_boolean_};
      // Act
      TS_ASSERT_THROWS_NOTHING(state.setSuper(boolean_bis))
      // Assert
      TS_ASSERT_EQUALS(state.superType(), boolean)
      TS_ASSERT_EQUALS(state.superType(), boolean_bis)
      TS_ASSERT_DIFFERS(&(state.superType()), &boolean)
      TS_ASSERT_EQUALS(&(state.superType()), &boolean_bis)
    }

    GUM_ACTIVE_TEST(SetSuperWrongtype) {
      // Arrange
      PRMType                 boolean{*_boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, *_state_};
      PRMType state_bis{*_state_};
      // Act & Assert
      TS_ASSERT_THROWS(state.setSuper(state_bis), const gum::TypeError&)
    }

    GUM_ACTIVE_TEST(SetSuperOperationNotAllowed) {
      // Arrange
      PRMType boolean{*_boolean_};
      PRMType state{*_state_};
      // Act & Assert
      TS_ASSERT_THROWS(state.setSuper(boolean), const gum::OperationNotAllowed&)
    }

    GUM_ACTIVE_TEST(LabelMap) {
      // Arrange
      PRMType                 boolean{*_boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, *_state_};
      // Act
      auto actual = state.label_map();
      // Assert
      TS_ASSERT_EQUALS(map, actual)
    }

    GUM_ACTIVE_TEST(LabelMapNotFound) {
      // Arrange
      PRMType boolean{*_boolean_};
      // Act & Assert
      TS_ASSERT_THROWS(boolean.label_map(), const gum::NotFound&)
    }
  };

}   // namespace gum_tests
