
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

#include <sstream>

#include <agrum/PRM/elements/PRMInstance.h>
#include <agrum/variables/labelizedVariable.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class PRMInstanceTestSuite : public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMInstance< double >        PRMInstance;
    typedef gum::prm::PRMClass< double >           PRMClass;
    typedef gum::prm::PRMInterface< double >       PRMInterface;
    typedef gum::prm::PRMType< double >            PRMType;
    typedef gum::prm::PRMScalarAttribute< double > PRMAttribute;
    typedef gum::prm::PRMReferenceSlot< double >   Reference;
    typedef gum::prm::PRMSlotChain< double >       PRMSlotChain;
    typedef gum::prm::PRMObject                    PRMObject;
    typedef gum::Bijection< const gum::DiscreteVariable*,
                            const gum::DiscreteVariable* >
      Bijection;

    PRMType*  __boolean;
    PRMType*  __state;
    PRMClass* __classA;
    PRMClass* __classB;

    public:
    void setUp() {
      __boolean = gum::prm::PRMType< double >::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      __state = new PRMType(*__boolean, map, state);

      __classA = new PRMClass("A");
      __classA->add(new PRMAttribute("a", *__boolean));
      __classA->add(new PRMAttribute("b", *__state));
      __classA->addArc("a", "b");

      __classB = new PRMClass("B");
      __classB->add(new PRMAttribute("a", *__boolean));
      __classB->add(new PRMAttribute("b", *__state));
      __classB->addArc("a", "b");
      __classB->add(new Reference("rho", *__classA));
      gum::Sequence< gum::prm::PRMClassElement< double >* > seq;
      seq << &(__classB->get("rho")) << &(__classA->get("a"));
      __classB->add(new PRMSlotChain("rho.a", seq));
      __classB->addArc("rho.a", "b");
    }

    void tearDown() {
      delete __classA;
      delete __classB;
      delete __boolean;
      delete __state;
    }

    /// Constructor & Destructor
    /// @{
    void testConstructor() {
      // Arrange
      PRMInstance* i = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(i = new PRMInstance("i", *__classA));
      TS_ASSERT_THROWS_NOTHING(delete i);
    }
    /// @}

    /// Getters & setters
    /// @{
    void testObjType() {
      // Arrange
      PRMInstance i("i", *__classA);
      auto        expected = PRMObject::prm_type::INSTANCE;
      // Act
      auto actual = i.obj_type();
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testType() {
      // Arrange
      PRMInstance i("i", *__classA);
      auto        expected = __classA;
      // Act
      auto actual = &(i.type());
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testTypeConst() {
      // Arrange
      PRMInstance i("i", *__classA);
      const auto& const_i = i;
      const auto  expected = __classA;
      // Act
      const auto actual = &(const_i.type());
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testExistsById() {
      // Arrange
      PRMInstance i("i", *__classA);
      auto        id = __classA->get("a").id();
      bool        expected = true;
      // Act
      bool actual = i.exists(id);
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testNotExistsById() {
      // Arrange
      PRMInstance i("i", *__classA);
      bool        expected = false;
      // Act
      bool actual = i.exists(666);
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testExistsByName() {
      // Arrange
      PRMInstance i("i", *__classA);
      bool        expected = true;
      // Act
      bool actual = i.exists("a");
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testNotExistsByName() {
      // Arrange
      PRMInstance i("i", *__classA);
      bool        expected = false;
      // Act
      bool actual = i.exists("aazeazeaze");
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testGetByName() {
      // Arrange
      PRMInstance                       i("i", *__classA);
      gum::prm::PRMAttribute< double >* attr = nullptr;
      auto&                             class_attr = __classA->get("a");
      // Act
      TS_ASSERT_THROWS_NOTHING(attr = &(i.get("a")));
      // Assert
      TS_ASSERT_EQUALS(attr->name(), "a");
      TS_ASSERT_DIFFERS(&(attr->type().variable()),
                        &(class_attr.type().variable()));
      TS_ASSERT_EQUALS(attr->cpf().domainSize(), class_attr.cpf().domainSize());
      TS_ASSERT_EQUALS(attr->cpf().nbrDim(), class_attr.cpf().nbrDim());
    }

    void testGetByNameConst() {
      // Arrange
      PRMInstance                             i("i", *__classA);
      const auto&                             const_i = i;
      gum::prm::PRMAttribute< double > const* attr = nullptr;
      const auto&                             class_attr = __classA->get("a");
      // Act
      TS_ASSERT_THROWS_NOTHING(attr = &(const_i.get("a")));
      // Assert
      TS_ASSERT_EQUALS(attr->name(), "a");
      TS_ASSERT_DIFFERS(&(attr->type().variable()),
                        &(class_attr.type().variable()));
      TS_ASSERT_EQUALS(attr->cpf().domainSize(), class_attr.cpf().domainSize());
      TS_ASSERT_EQUALS(attr->cpf().nbrDim(), class_attr.cpf().nbrDim());
    }

    void testGetByNameNotFound() {
      // Arrange
      PRMInstance i("i", *__classA);
      // Act & Assert
      TS_ASSERT_THROWS(i.get("azeaze"), gum::NotFound);
    }

    void testGetByNameConstNotFound() {
      // Arrange
      PRMInstance i("i", *__classA);
      const auto& const_i = i;
      // Act & Assert
      TS_ASSERT_THROWS(const_i.get("azeaze"), gum::NotFound);
    }

    void testGetById() {
      // Arrange
      PRMInstance                       i("i", *__classA);
      gum::prm::PRMAttribute< double >* attr = nullptr;
      auto&                             class_attr = __classA->get("a");
      // Act
      TS_ASSERT_THROWS_NOTHING(attr = &(i.get(class_attr.id())));
      // Assert
      TS_ASSERT_EQUALS(attr->name(), "a");
      TS_ASSERT_DIFFERS(&(attr->type().variable()),
                        &(class_attr.type().variable()));
      TS_ASSERT_EQUALS(attr->cpf().domainSize(), class_attr.cpf().domainSize());
      TS_ASSERT_EQUALS(attr->cpf().nbrDim(), class_attr.cpf().nbrDim());
    }

    void testGetByIdConst() {
      // Arrange
      PRMInstance                             i("i", *__classA);
      const auto&                             const_i = i;
      gum::prm::PRMAttribute< double > const* attr = nullptr;
      const auto&                             class_attr = __classA->get("a");
      // Act
      TS_ASSERT_THROWS_NOTHING(attr = &(const_i.get(class_attr.id())));
      // Assert
      TS_ASSERT_EQUALS(attr->name(), "a");
      TS_ASSERT_DIFFERS(&(attr->type().variable()),
                        &(class_attr.type().variable()));
      TS_ASSERT_EQUALS(attr->cpf().domainSize(), class_attr.cpf().domainSize());
      TS_ASSERT_EQUALS(attr->cpf().nbrDim(), class_attr.cpf().nbrDim());
    }

    void testGetByIdNotFound() {
      // Arrange
      PRMInstance i("i", *__classA);
      // Act & Assert
      TS_ASSERT_THROWS(i.get(666), gum::NotFound);
    }

    void testGetByIdConstNotFound() {
      // Arrange
      PRMInstance i("i", *__classA);
      const auto& const_i = i;
      // Act & Assert
      TS_ASSERT_THROWS(const_i.get(666), gum::NotFound);
    }

    void testSize() {
      // Arrage
      PRMInstance i("i", *__classA);
      gum::Size   expected = 3;   // 2 Attributes + 1 cast descendant
      // Act
      auto actual = i.size();
      // Assert
      TS_ASSERT_EQUALS(expected, actual);
    }
    /// @}

    /// Instantiation methods
    /// @{
    void testInstiate() {
      // Arrange
      PRMInstance i("i", *__classA);
      // Act && Assert
      TS_ASSERT_THROWS_NOTHING(i.instantiate());
    }

    void testBijection() {
      // Arrange
      PRMInstance      i("i", *__classA);
      Bijection const* bij = nullptr;
      auto             var_a = &(__classA->get("a").type().variable());
      auto             var_b = &(__classA->get("b").type().variable());
      auto             i_a = &(i.get("a").type().variable());
      auto             i_b = &(i.get("b").type().variable());
      // Act
      TS_ASSERT_THROWS_NOTHING(bij = &(i.bijection()));
      // Assert
      TS_ASSERT_EQUALS(bij->second(var_a), i_a);
      TS_ASSERT_EQUALS(bij->second(var_b), i_b);
      TS_ASSERT_EQUALS(bij->first(i_a), var_a);
      TS_ASSERT_EQUALS(bij->first(i_b), var_b);
    }
    /// @}

    /// Reference
    /// @{

    /// @}
  };

}   // namespace gum_tests
