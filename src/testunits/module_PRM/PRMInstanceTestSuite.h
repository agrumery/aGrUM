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


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/PRM/elements/PRMInstance.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  PRMInstance
#define GUM_CURRENT_MODULE PRM

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  struct PRMInstanceTestSuite {
    private:
    using PRMInstance  = gum::prm::PRMInstance< double >;
    using PRMClass     = gum::prm::PRMClass< double >;
    using PRMInterface = gum::prm::PRMInterface< double >;
    using PRMType      = gum::prm::PRMType;
    using PRMAttribute = gum::prm::PRMScalarAttribute< double >;
    using Reference    = gum::prm::PRMReferenceSlot< double >;
    using PRMSlotChain = gum::prm::PRMSlotChain< double >;
    using PRMObject    = gum::prm::PRMObject;

    using Bijection = gum::Bijection< const gum::DiscreteVariable*, const gum::DiscreteVariable* >;

    PRMType*  _boolean_;
    PRMType*  _state_;
    PRMClass* _classA_;
    PRMClass* _classB_;

    public:
    /// Constructor & Destructor
    /// @{
    PRMInstanceTestSuite() {
      _boolean_ = gum::prm::PRMType::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      _state_  = new PRMType(*_boolean_, map, state);
      _classA_ = new PRMClass("A");
      _classA_->add(new PRMAttribute("a", *_boolean_));
      _classA_->add(new PRMAttribute("b", *_state_));
      _classA_->addArc("a", "b");
      _classB_ = new PRMClass("B");
      _classB_->add(new PRMAttribute("a", *_boolean_));
      _classB_->add(new PRMAttribute("b", *_state_));
      _classB_->addArc("a", "b");
      _classB_->add(new Reference("rho", *_classA_));
      gum::Sequence< gum::prm::PRMClassElement< double >* > seq;
      seq << &(_classB_->get("rho")) << &(_classA_->get("a"));
      _classB_->add(new PRMSlotChain("rho.a", seq));
      _classB_->addArc("rho.a", "b");
    }

    ~PRMInstanceTestSuite() {
      delete _classA_;
      delete _classB_;
      delete _boolean_;
      delete _state_;
    }

    static void testConstructor() {
      // Arrange
      PRMInstance* i = nullptr;
      // Act & Assert
      CHECK_NOTHROW(i = new PRMInstance("i", *_classA_));
      CHECK_NOTHROW(delete i);
    }

    /// @}

    /// Getters & setters
    /// @{
    static void testObjType() {
      // Arrange
      PRMInstance i("i", *_classA_);
      auto        expected = PRMObject::prm_type::INSTANCE;
      // Act
      auto actual = i.obj_type();
      // Assert
      CHECK((actual) == (expected));
    }

    static void testType() {
      // Arrange
      PRMInstance i("i", *_classA_);
      auto        expected = _classA_;
      // Act
      auto actual = &(i.type());
      // Assert
      CHECK((actual) == (expected));
    }

    static void testTypeConst() {
      // Arrange
      PRMInstance i("i", *_classA_);
      const auto& const_i  = i;
      const auto  expected = _classA_;
      // Act
      const auto actual = &(const_i.type());
      // Assert
      CHECK((actual) == (expected));
    }

    static void testExistsById() {
      // Arrange
      PRMInstance i("i", *_classA_);
      auto        id       = _classA_->get("a").id();
      bool        expected = true;
      // Act
      bool actual = i.exists(id);
      // Assert
      CHECK((actual) == (expected));
    }

    static void testNotExistsById() {
      // Arrange
      PRMInstance i("i", *_classA_);
      bool        expected = false;
      // Act
      bool actual = i.exists(666);
      // Assert
      CHECK((actual) == (expected));
    }

    static void testExistsByName() {
      // Arrange
      PRMInstance i("i", *_classA_);
      bool        expected = true;
      // Act
      bool actual = i.exists("a");
      // Assert
      CHECK((actual) == (expected));
    }

    static void testNotExistsByName() {
      // Arrange
      PRMInstance i("i", *_classA_);
      bool        expected = false;
      // Act
      bool actual = i.exists("aazeazeaze");
      // Assert
      CHECK((actual) == (expected));
    }

    static void testGetByName() {
      // Arrange
      PRMInstance                       i("i", *_classA_);
      gum::prm::PRMAttribute< double >* attr       = nullptr;
      auto&                             class_attr = _classA_->get("a");
      // Act
      CHECK_NOTHROW(attr = &(i.get("a")));
      // Assert
      CHECK((attr->name()) == ("a"));
      CHECK((&(attr->type().variable())) != (&(class_attr.type().variable())));
      CHECK((attr->cpf().domainSize()) == (class_attr.cpf().domainSize()));
      CHECK((attr->cpf().nbrDim()) == (class_attr.cpf().nbrDim()));
    }

    static void testGetByNameConst() {
      // Arrange
      PRMInstance                             i("i", *_classA_);
      const auto&                             const_i    = i;
      gum::prm::PRMAttribute< double > const* attr       = nullptr;
      const auto&                             class_attr = _classA_->get("a");
      // Act
      CHECK_NOTHROW(attr = &(const_i.get("a")));
      // Assert
      CHECK((attr->name()) == ("a"));
      CHECK((&(attr->type().variable())) != (&(class_attr.type().variable())));
      CHECK((attr->cpf().domainSize()) == (class_attr.cpf().domainSize()));
      CHECK((attr->cpf().nbrDim()) == (class_attr.cpf().nbrDim()));
    }

    static void testGetByNameNotFound() {
      // Arrange
      PRMInstance i("i", *_classA_);
      // Act & Assert
      CHECK_THROWS_AS(i.get("azeaze"), const gum::NotFound&);
    }

    static void testGetByNameConstNotFound() {
      // Arrange
      PRMInstance i("i", *_classA_);
      const auto& const_i = i;
      // Act & Assert
      CHECK_THROWS_AS(const_i.get("azeaze"), const gum::NotFound&);
    }

    static void testGetById() {
      // Arrange
      PRMInstance                       i("i", *_classA_);
      gum::prm::PRMAttribute< double >* attr       = nullptr;
      auto&                             class_attr = _classA_->get("a");
      // Act
      CHECK_NOTHROW(attr = &(i.get(class_attr.id())));
      // Assert
      CHECK((attr->name()) == ("a"));
      CHECK((&(attr->type().variable())) != (&(class_attr.type().variable())));
      CHECK((attr->cpf().domainSize()) == (class_attr.cpf().domainSize()));
      CHECK((attr->cpf().nbrDim()) == (class_attr.cpf().nbrDim()));
    }

    static void testGetByIdConst() {
      // Arrange
      PRMInstance                             i("i", *_classA_);
      const auto&                             const_i    = i;
      gum::prm::PRMAttribute< double > const* attr       = nullptr;
      const auto&                             class_attr = _classA_->get("a");
      // Act
      CHECK_NOTHROW(attr = &(const_i.get(class_attr.id())));
      // Assert
      CHECK((attr->name()) == ("a"));
      CHECK((&(attr->type().variable())) != (&(class_attr.type().variable())));
      CHECK((attr->cpf().domainSize()) == (class_attr.cpf().domainSize()));
      CHECK((attr->cpf().nbrDim()) == (class_attr.cpf().nbrDim()));
    }

    static void testGetByIdNotFound() {
      // Arrange
      PRMInstance i("i", *_classA_);
      // Act & Assert
      CHECK_THROWS_AS(i.get(666), const gum::NotFound&);
    }

    static void testGetByIdConstNotFound() {
      // Arrange
      PRMInstance i("i", *_classA_);
      const auto& const_i = i;
      // Act & Assert
      CHECK_THROWS_AS(const_i.get(666), const gum::NotFound&);
    }

    static void testSize() {
      // Arrage
      PRMInstance i("i", *_classA_);
      gum::Size   expected = 3;   // 2 Attributes + 1 cast descendant
      // Act
      auto actual = i.size();
      // Assert
      CHECK((expected) == (actual));
    }

    /// @}

    /// Instantiation methods
    /// @{
    static void testInstiate() {
      // Arrange
      PRMInstance i("i", *_classA_);
      // Act && Assert
      CHECK_NOTHROW(i.instantiate());
    }

    static void testBijection() {
      // Arrange
      PRMInstance      i("i", *_classA_);
      Bijection const* bij   = nullptr;
      auto             var_a = &(_classA_->get("a").type().variable());
      auto             var_b = &(_classA_->get("b").type().variable());
      auto             i_a   = &(i.get("a").type().variable());
      auto             i_b   = &(i.get("b").type().variable());
      // Act
      CHECK_NOTHROW(bij = &(i.bijection()));
      // Assert
      CHECK((bij->second(var_a)) == (i_a));
      CHECK((bij->second(var_b)) == (i_b));
      CHECK((bij->first(i_a)) == (var_a));
      CHECK((bij->first(i_b)) == (var_b));
    }

    /// @}

    /// Reference
    /// @{

    /// @}
  };

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(ObjType)
  GUM_TEST_ACTIF(Type)
  GUM_TEST_ACTIF(TypeConst)
  GUM_TEST_ACTIF(ExistsById)
  GUM_TEST_ACTIF(NotExistsById)
  GUM_TEST_ACTIF(ExistsByName)
  GUM_TEST_ACTIF(NotExistsByName)
  GUM_TEST_ACTIF(GetByName)
  GUM_TEST_ACTIF(GetByNameConst)
  GUM_TEST_ACTIF(GetByNameNotFound)
  GUM_TEST_ACTIF(GetByNameConstNotFound)
  GUM_TEST_ACTIF(GetById)
  GUM_TEST_ACTIF(GetByIdConst)
  GUM_TEST_ACTIF(GetByIdNotFound)
  GUM_TEST_ACTIF(GetByIdConstNotFound)
  GUM_TEST_ACTIF(Size)
  GUM_TEST_ACTIF(Instiate)
  GUM_TEST_ACTIF(Bijection)

}   // namespace gum_tests
