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

  class PRMTypeTestSuite: public CxxTest::TestSuite {
    typedef gum::prm::PRMType PRMType;

    gum::LabelizedVariable*  _boolean_;
    gum::LabelizedVariable*  _state_;

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
      delete  _boolean_;
      delete  _state_;
    }

    void testStaticBoolean() {
      // Arrange
      std::string labels[]   = {"false", "true"};
      std::string name       = "boolean";
      std::string decription = "Boolean variable";
      // Act
      auto boolean = PRMType::boolean();
      // Assert
      TS_ASSERT_EQUALS(boolean->variable().domainSize(), (gum::Size)2);
      TS_ASSERT_EQUALS(boolean->variable().label(0), labels[0]);
      TS_ASSERT_EQUALS(boolean->variable().label(1), labels[1]);
      delete boolean;
    }

    void testConstructor() {
      // Arrange
      PRMType* type = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(type = new PRMType{* _boolean_});
      // Assert
      TS_ASSERT_THROWS_NOTHING(delete type);
    }

    void testSuperConstructor() {
      // Arrange
      PRMType                 boolean{* _boolean_};
      PRMType*                state = nullptr;
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      // Act
      try {
        state = new PRMType{boolean, map, * _state_};
      } catch (...) {
        // TS_ASSERT_THROWS_NOTHING does ! work here
        TS_FAIL("Exception thrown");
      }
      // Assert
      TS_ASSERT_THROWS_NOTHING(delete state);
    }

    void testCopyConstructor() {
      // Arrange
      PRMType  boolean{* _boolean_};
      PRMType* copy = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(copy = new PRMType{boolean});
      // Assert
      TS_ASSERT_THROWS_NOTHING(delete copy);
    }

    void testGetVariable() {
      // Arrange
      PRMType                boolean{* _boolean_};
      gum::DiscreteVariable* variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(variable = &(boolean.variable()));
      // Assert
      TS_ASSERT_DIFFERS(variable->name().find( _boolean_->name()),
                        std::string::npos);
      TS_ASSERT_EQUALS(variable->description(),  _boolean_->description());
      TS_ASSERT_EQUALS(variable->label(0),  _boolean_->label(0));
      TS_ASSERT_EQUALS(variable->label(1),  _boolean_->label(1));
      TS_ASSERT_EQUALS(variable->domainSize(),  _boolean_->domainSize());
      TS_ASSERT_DIFFERS(variable,  _boolean_);
    }

    void testGetVariableConst() {
      // Arrange
      PRMType                      boolean{* _boolean_};
      const PRMType&               const_boolean = boolean;
      gum::DiscreteVariable const* variable      = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(variable = &(const_boolean.variable()));
      // Assert
      TS_ASSERT_DIFFERS(variable->name().find( _boolean_->name()),
                        std::string::npos);
      TS_ASSERT_EQUALS(variable->description(),  _boolean_->description());
      TS_ASSERT_EQUALS(variable->label(0),  _boolean_->label(0));
      TS_ASSERT_EQUALS(variable->label(1),  _boolean_->label(1));
      TS_ASSERT_EQUALS(variable->domainSize(),  _boolean_->domainSize());
      TS_ASSERT_DIFFERS(variable,  _boolean_);
    }

    void testIndirectionOperator() {
      // Arrange
      PRMType                boolean{* _boolean_};
      gum::DiscreteVariable* variable = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(variable = &(*boolean));
      // Assert
      TS_ASSERT_DIFFERS(variable->name().find( _boolean_->name()),
                        std::string::npos);
      TS_ASSERT_EQUALS(variable->description(),  _boolean_->description());
      TS_ASSERT_EQUALS(variable->label(0),  _boolean_->label(0));
      TS_ASSERT_EQUALS(variable->label(1),  _boolean_->label(1));
      TS_ASSERT_EQUALS(variable->domainSize(),  _boolean_->domainSize());
      TS_ASSERT_DIFFERS(variable,  _boolean_);
    }

    void testIndirectionOperatorConst() {
      // Arrange
      PRMType                      boolean{* _boolean_};
      const PRMType&               const_boolean = boolean;
      gum::DiscreteVariable const* variable      = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(variable = &(*const_boolean));
      // Assert
      TS_ASSERT_DIFFERS(variable->name().find( _boolean_->name()),
                        std::string::npos);
      TS_ASSERT_EQUALS(variable->description(),  _boolean_->description());
      TS_ASSERT_EQUALS(variable->label(0),  _boolean_->label(0));
      TS_ASSERT_EQUALS(variable->label(1),  _boolean_->label(1));
      TS_ASSERT_EQUALS(variable->domainSize(),  _boolean_->domainSize());
      TS_ASSERT_DIFFERS(variable,  _boolean_);
    }

    void testEqualityOperator() {
      // Arrange
      PRMType a{* _boolean_};
      PRMType b{* _boolean_};
      PRMType c{* _state_};
      // Act & Assert
      TS_ASSERT(a == a);
      TS_ASSERT(a == b);
      TS_ASSERT(b == a);

      TS_ASSERT(!(c == a));
      TS_ASSERT(!(a == c));
      TS_ASSERT(!(c == b));
      TS_ASSERT(!(b == c));
    }

    void testInequalityOperator() {
      // Arrange
      PRMType a{* _boolean_};
      PRMType b{* _boolean_};
      PRMType c{* _state_};

      // Act & Assert
      TS_ASSERT(!(a != a));
      TS_ASSERT(!(a != b));
      TS_ASSERT(!(b != a));

      TS_ASSERT(c != a);
      TS_ASSERT(a != c);
      TS_ASSERT(c != b);
      TS_ASSERT(b != c);
    }

    void testObjType() {
      // Arrange
      PRMType boolean{* _boolean_};
      auto    expected = gum::prm::PRMObject::prm_type::TYPE;
      // Act
      auto actual = boolean.obj_type();
      // Assert
      TS_ASSERT_EQUALS(expected, actual);
    }

    void testName() {
      // Arrange
      PRMType boolean{* _boolean_};
      auto    expected =  _boolean_->name();
      // Act
      auto actual = boolean.name();
      // Assert
      TS_ASSERT_EQUALS(expected, actual);
    }

    void testIsSubType() {
      // Arrange
      PRMType                 boolean{* _boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, * _state_};
      // Act & Assert
      TS_ASSERT(state.isSubType());
      TS_ASSERT(!boolean.isSubType());
    }

    void testIsSubTypeOf() {
      // Arrange
      PRMType                 boolean{* _boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, * _state_};
      PRMType dummy{* _state_};
      // Act & Assert
      TS_ASSERT(state.isSubTypeOf(boolean));
      TS_ASSERT(state.isSubTypeOf(state));
      TS_ASSERT(!boolean.isSubTypeOf(state));
      TS_ASSERT(!dummy.isSubTypeOf(boolean));
      TS_ASSERT(!boolean.isSubTypeOf(dummy));
      TS_ASSERT(dummy.isSubTypeOf(state));
      TS_ASSERT(state.isSubTypeOf(dummy));
    }

    void testIsSuperTypeOf() {
      // Arrange
      PRMType                 boolean{* _boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, * _state_};
      PRMType dummy{* _state_};
      // Act & Assert
      TS_ASSERT(boolean.isSuperTypeOf(state));
      TS_ASSERT(state.isSuperTypeOf(state));
      TS_ASSERT(!state.isSuperTypeOf(boolean));
      TS_ASSERT(!dummy.isSuperTypeOf(boolean));
      TS_ASSERT(!boolean.isSuperTypeOf(dummy));
      TS_ASSERT(dummy.isSuperTypeOf(state));
      TS_ASSERT(state.isSuperTypeOf(dummy));
    }

    void testSuper() {
      // Arrange
      PRMType                 boolean{* _boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType  state{boolean, map, * _state_};
      PRMType* super = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(super = &(state.superType()));
      // Act & Assert
      TS_ASSERT_EQUALS(*super, boolean);
    }

    void testSuperConst() {
      // Arrange
      PRMType                 boolean{* _boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType        state{boolean, map, * _state_};
      const auto&    dummy = state;
      PRMType const* super = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING(super = &(dummy.superType()));
      // Act & Assert
      TS_ASSERT_EQUALS(*super, boolean);
    }

    void testSuperNotFound() {
      // Arrange
      PRMType boolean{* _boolean_};
      // Act & Assert
      TS_ASSERT_THROWS(boolean.superType(), gum::NotFound);
    }

    void testSetSuper() {
      // Arrange
      PRMType                 boolean{* _boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, * _state_};
      PRMType boolean_bis{* _boolean_};
      // Act
      TS_ASSERT_THROWS_NOTHING(state.setSuper(boolean_bis));
      // Assert
      TS_ASSERT_EQUALS(state.superType(), boolean);
      TS_ASSERT_EQUALS(state.superType(), boolean_bis);
      TS_ASSERT_DIFFERS(&(state.superType()), &boolean);
      TS_ASSERT_EQUALS(&(state.superType()), &boolean_bis);
    }

    void testSetSuperWrongtype() {
      // Arrange
      PRMType                 boolean{* _boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, * _state_};
      PRMType state_bis{* _state_};
      // Act & Assert
      TS_ASSERT_THROWS(state.setSuper(state_bis), gum::TypeError);
    }

    void testSetSuperOperationNotAllowed() {
      // Arrange
      PRMType boolean{* _boolean_};
      PRMType state{* _state_};
      // Act & Assert
      TS_ASSERT_THROWS(state.setSuper(boolean), gum::OperationNotAllowed);
    }

    void testLabelMap() {
      // Arrange
      PRMType                 boolean{* _boolean_};
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      PRMType state{boolean, map, * _state_};
      // Act
      auto actual = state.label_map();
      // Assert
      TS_ASSERT_EQUALS(map, actual);
    }

    void testLabelMapNotFound() {
      // Arrange
      PRMType boolean{* _boolean_};
      // Act & Assert
      TS_ASSERT_THROWS(boolean.label_map(), gum::NotFound);
    }
  };

}   // namespace gum_tests
