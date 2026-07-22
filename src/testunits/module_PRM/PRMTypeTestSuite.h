/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <string>

#include <agrum/base/core/exceptions.h>
#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMType.h>

#include <module_PRM/ClassElementTestSuiteAbstract.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  struct PRMTypeTestSuite {
    using PRMType = gum::prm::PRMType;

    gum::LabelizedVariable* _boolean_;
    gum::LabelizedVariable* _state_;

    public:
    PRMTypeTestSuite() {
      _boolean_ = new gum::LabelizedVariable{"boolean", "Boolean variable", 0};

      _boolean_->addLabel("false");

      _boolean_->addLabel("true");

      _state_ = new gum::LabelizedVariable{"state", "State variable", 0};

      _state_->addLabel("OK");

      _state_->addLabel("NOK");
    }

    ~PRMTypeTestSuite() {
      delete _boolean_;

      delete _state_;
    }
  };

  GUM_TEST(StaticBoolean) {
    // Arrange
    std::string labels[]   = {"false", "true"};
    std::string name       = "boolean";
    std::string decription = "Boolean variable";
    // Act
    auto boolean = PRMType::boolean();
    // Assert
    CHECK_EQ(boolean->variable().domainSize(), static_cast< gum::Size >(2));
    CHECK_EQ(boolean->variable().label(0), labels[0]);
    CHECK_EQ(boolean->variable().label(1), labels[1]);
    delete boolean;
  }

  GUM_TEST(Constructor) {
    // Arrange
    PRMType* type = nullptr;
    // Act
    CHECK_NOTHROW(type = new PRMType{*_boolean_});
    // Assert
    CHECK_NOTHROW(delete type);
  }

  GUM_TEST(SuperConstructor) {
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
      // CHECK_NOTHROW does ! work here
      FAIL("Exception thrown");
    }
    // Assert
    CHECK_NOTHROW(delete state);
  }

  GUM_TEST(CopyConstructor) {
    // Arrange
    PRMType  boolean{*_boolean_};
    PRMType* copy = nullptr;
    // Act
    CHECK_NOTHROW(copy = new PRMType{boolean});
    // Assert
    CHECK_NOTHROW(delete copy);
  }

  GUM_TEST(GetVariable) {
    // Arrange
    PRMType                boolean{*_boolean_};
    gum::DiscreteVariable* variable = nullptr;
    // Act
    CHECK_NOTHROW(variable = &(boolean.variable()));
    // Assert
    CHECK_NE(variable->name().find(_boolean_->name()), std::string::npos);
    CHECK_EQ(variable->description(), _boolean_->description());
    CHECK_EQ(variable->label(0), _boolean_->label(0));
    CHECK_EQ(variable->label(1), _boolean_->label(1));
    CHECK_EQ(variable->domainSize(), _boolean_->domainSize());
    CHECK_NE(variable, _boolean_);
  }

  GUM_TEST(GetVariableConst) {
    // Arrange
    PRMType                      boolean{*_boolean_};
    const PRMType&               const_boolean = boolean;
    gum::DiscreteVariable const* variable      = nullptr;
    // Act
    CHECK_NOTHROW(variable = &(const_boolean.variable()));
    // Assert
    CHECK_NE(variable->name().find(_boolean_->name()), std::string::npos);
    CHECK_EQ(variable->description(), _boolean_->description());
    CHECK_EQ(variable->label(0), _boolean_->label(0));
    CHECK_EQ(variable->label(1), _boolean_->label(1));
    CHECK_EQ(variable->domainSize(), _boolean_->domainSize());
    CHECK_NE(variable, _boolean_);
  }

  GUM_TEST(IndirectionOperator) {
    // Arrange
    PRMType                boolean{*_boolean_};
    gum::DiscreteVariable* variable = nullptr;
    // Act
    CHECK_NOTHROW(variable = &(*boolean));
    // Assert
    CHECK_NE(variable->name().find(_boolean_->name()), std::string::npos);
    CHECK_EQ(variable->description(), _boolean_->description());
    CHECK_EQ(variable->label(0), _boolean_->label(0));
    CHECK_EQ(variable->label(1), _boolean_->label(1));
    CHECK_EQ(variable->domainSize(), _boolean_->domainSize());
    CHECK_NE(variable, _boolean_);
  }

  GUM_TEST(IndirectionOperatorConst) {
    // Arrange
    PRMType                      boolean{*_boolean_};
    const PRMType&               const_boolean = boolean;
    gum::DiscreteVariable const* variable      = nullptr;
    // Act
    CHECK_NOTHROW(variable = &(*const_boolean));
    // Assert
    CHECK_NE(variable->name().find(_boolean_->name()), std::string::npos);
    CHECK_EQ(variable->description(), _boolean_->description());
    CHECK_EQ(variable->label(0), _boolean_->label(0));
    CHECK_EQ(variable->label(1), _boolean_->label(1));
    CHECK_EQ(variable->domainSize(), _boolean_->domainSize());
    CHECK_NE(variable, _boolean_);
  }

  GUM_TEST(EqualityOperator) {
    // Arrange
    PRMType a{*_boolean_};
    PRMType b{*_boolean_};
    PRMType c{*_state_};
    // Act & Assert
    CHECK_EQ(a, a);
    CHECK_EQ(a, b);
    CHECK_EQ(b, a);

    CHECK_NE(c, a);
    CHECK_NE(a, c);
    CHECK_NE(c, b);
    CHECK_NE(b, c);
  }

  GUM_TEST(InequalityOperator) {
    // Arrange
    PRMType a{*_boolean_};
    PRMType b{*_boolean_};
    PRMType c{*_state_};

    // Act & Assert
    CHECK_EQ(a, a);
    CHECK_EQ(a, b);
    CHECK_EQ(b, a);

    CHECK_NE(c, a);
    CHECK_NE(a, c);
    CHECK_NE(c, b);
    CHECK_NE(b, c);
  }

  GUM_TEST(ObjType) {
    // Arrange
    PRMType boolean{*_boolean_};
    auto    expected = gum::prm::PRMObject::prm_type::TYPE;
    // Act
    auto actual = boolean.obj_type();
    // Assert
    CHECK_EQ(expected, actual);
  }

  GUM_TEST(Name) {
    // Arrange
    PRMType boolean{*_boolean_};
    auto    expected = _boolean_->name();
    // Act
    auto actual = boolean.name();
    // Assert
    CHECK_EQ(expected, actual);
  }

  GUM_TEST(IsSubType) {
    // Arrange
    PRMType                 boolean{*_boolean_};
    std::vector< gum::Idx > map;
    map.push_back(1);
    map.push_back(0);
    PRMType state{boolean, map, *_state_};
    // Act & Assert
    CHECK(state.isSubType());
    CHECK(!boolean.isSubType());
  }

  GUM_TEST(IsSubTypeOf) {
    // Arrange
    PRMType                 boolean{*_boolean_};
    std::vector< gum::Idx > map;
    map.push_back(1);
    map.push_back(0);
    PRMType state{boolean, map, *_state_};
    PRMType dummy{*_state_};
    // Act & Assert
    CHECK(state.isSubTypeOf(boolean));
    CHECK(state.isSubTypeOf(state));
    CHECK(!boolean.isSubTypeOf(state));
    CHECK(!dummy.isSubTypeOf(boolean));
    CHECK(!boolean.isSubTypeOf(dummy));
    CHECK(dummy.isSubTypeOf(state));
    CHECK(state.isSubTypeOf(dummy));
  }

  GUM_TEST(IsSuperTypeOf) {
    // Arrange
    PRMType                 boolean{*_boolean_};
    std::vector< gum::Idx > map;
    map.push_back(1);
    map.push_back(0);
    PRMType state{boolean, map, *_state_};
    PRMType dummy{*_state_};
    // Act & Assert
    CHECK(boolean.isSuperTypeOf(state));
    CHECK(state.isSuperTypeOf(state));
    CHECK(!state.isSuperTypeOf(boolean));
    CHECK(!dummy.isSuperTypeOf(boolean));
    CHECK(!boolean.isSuperTypeOf(dummy));
    CHECK(dummy.isSuperTypeOf(state));
    CHECK(state.isSuperTypeOf(dummy));
  }

  GUM_TEST(Super) {
    // Arrange
    PRMType                 boolean{*_boolean_};
    std::vector< gum::Idx > map;
    map.push_back(1);
    map.push_back(0);
    PRMType  state{boolean, map, *_state_};
    PRMType* super = nullptr;
    // Act
    CHECK_NOTHROW(super = &(state.superType()));
    // Act & Assert
    CHECK_EQ(*super, boolean);
  }

  GUM_TEST(SuperConst) {
    // Arrange
    PRMType                 boolean{*_boolean_};
    std::vector< gum::Idx > map;
    map.push_back(1);
    map.push_back(0);
    PRMType        state{boolean, map, *_state_};
    const auto&    dummy = state;
    PRMType const* super = nullptr;
    // Act
    CHECK_NOTHROW(super = &(dummy.superType()));
    // Act & Assert
    CHECK_EQ(*super, boolean);
  }

  GUM_TEST(SuperNotFound) {
    // Arrange
    PRMType boolean{*_boolean_};
    // Act & Assert
    CHECK_THROWS_AS(boolean.superType(), const gum::NotFound&);
  }

  GUM_TEST(SetSuper) {
    // Arrange
    PRMType                 boolean{*_boolean_};
    std::vector< gum::Idx > map;
    map.push_back(1);
    map.push_back(0);
    PRMType state{boolean, map, *_state_};
    PRMType boolean_bis{*_boolean_};
    // Act
    CHECK_NOTHROW(state.setSuper(boolean_bis));
    // Assert
    CHECK_EQ(state.superType(), boolean);
    CHECK_EQ(state.superType(), boolean_bis);
    CHECK_NE(&(state.superType()), &boolean);
    CHECK_EQ(&(state.superType()), &boolean_bis);
  }

  GUM_TEST(SetSuperWrongtype) {
    // Arrange
    PRMType                 boolean{*_boolean_};
    std::vector< gum::Idx > map;
    map.push_back(1);
    map.push_back(0);
    PRMType state{boolean, map, *_state_};
    PRMType state_bis{*_state_};
    // Act & Assert
    CHECK_THROWS_AS(state.setSuper(state_bis), const gum::TypeError&);
  }

  GUM_TEST(SetSuperOperationNotAllowed) {
    // Arrange
    PRMType boolean{*_boolean_};
    PRMType state{*_state_};
    // Act & Assert
    CHECK_THROWS_AS(state.setSuper(boolean), const gum::OperationNotAllowed&);
  }

  GUM_TEST(LabelMap) {
    // Arrange
    PRMType                 boolean{*_boolean_};
    std::vector< gum::Idx > map;
    map.push_back(1);
    map.push_back(0);
    PRMType state{boolean, map, *_state_};
    // Act
    auto actual = state.label_map();
    // Assert
    CHECK_EQ(map, actual);
  }

  GUM_TEST(LabelMapNotFound) {
    // Arrange
    PRMType boolean{*_boolean_};
    // Act & Assert
    CHECK_THROWS_AS(boolean.label_map(), const gum::NotFound&);
  }

}   // namespace gum_tests
