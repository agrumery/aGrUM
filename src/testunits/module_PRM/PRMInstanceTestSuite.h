
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <sstream>

#include <agrum/PRM/elements/PRMInstance.h>
#include <agrum/tools/variables/labelizedVariable.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class PRMInstanceTestSuite: public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMInstance< double >        PRMInstance;
    typedef gum::prm::PRMClass< double >           PRMClass;
    typedef gum::prm::PRMInterface< double >       PRMInterface;
    typedef gum::prm::PRMType                      PRMType;
    typedef gum::prm::PRMScalarAttribute< double > PRMAttribute;
    typedef gum::prm::PRMReferenceSlot< double >   Reference;
    typedef gum::prm::PRMSlotChain< double >       PRMSlotChain;
    typedef gum::prm::PRMObject                    PRMObject;
    typedef gum::Bijection< const gum::DiscreteVariable*,
                            const gum::DiscreteVariable* >
       Bijection;

    PRMType*  boolean__;
    PRMType*  state__;
    PRMClass* classA__;
    PRMClass* classB__;

    public:
    void setUp() {
      boolean__ = gum::prm::PRMType::boolean();
      gum::LabelizedVariable state{"state", "A state variable", 0};
      state.addLabel("OK");
      state.addLabel("NOK");
      std::vector< gum::Idx > map;
      map.push_back(1);
      map.push_back(0);
      state__ = new PRMType(*boolean__, map, state);

      classA__ = new PRMClass("A");
      classA__->add(new PRMAttribute("a", *boolean__));
      classA__->add(new PRMAttribute("b", *state__));
      classA__->addArc("a", "b");

      classB__ = new PRMClass("B");
      classB__->add(new PRMAttribute("a", *boolean__));
      classB__->add(new PRMAttribute("b", *state__));
      classB__->addArc("a", "b");
      classB__->add(new Reference("rho", *classA__));
      gum::Sequence< gum::prm::PRMClassElement< double >* > seq;
      seq << &(classB__->get("rho")) << &(classA__->get("a"));
      classB__->add(new PRMSlotChain("rho.a", seq));
      classB__->addArc("rho.a", "b");
    }

    void tearDown() {
      delete classA__;
      delete classB__;
      delete boolean__;
      delete state__;
    }

    /// Constructor & Destructor
    /// @{
    void testConstructor() {
      // Arrange
      PRMInstance* i = nullptr;
      // Act & Assert
      TS_ASSERT_THROWS_NOTHING(i = new PRMInstance("i", *classA__));
      TS_ASSERT_THROWS_NOTHING(delete i);
    }
    /// @}

    /// Getters & setters
    /// @{
    void testObjType() {
      // Arrange
      PRMInstance i("i", *classA__);
      auto        expected = PRMObject::prm_type::INSTANCE;
      // Act
      auto actual = i.obj_type();
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testType() {
      // Arrange
      PRMInstance i("i", *classA__);
      auto        expected = classA__;
      // Act
      auto actual = &(i.type());
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testTypeConst() {
      // Arrange
      PRMInstance i("i", *classA__);
      const auto& const_i = i;
      const auto  expected = classA__;
      // Act
      const auto actual = &(const_i.type());
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testExistsById() {
      // Arrange
      PRMInstance i("i", *classA__);
      auto        id = classA__->get("a").id();
      bool        expected = true;
      // Act
      bool actual = i.exists(id);
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testNotExistsById() {
      // Arrange
      PRMInstance i("i", *classA__);
      bool        expected = false;
      // Act
      bool actual = i.exists(666);
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testExistsByName() {
      // Arrange
      PRMInstance i("i", *classA__);
      bool        expected = true;
      // Act
      bool actual = i.exists("a");
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testNotExistsByName() {
      // Arrange
      PRMInstance i("i", *classA__);
      bool        expected = false;
      // Act
      bool actual = i.exists("aazeazeaze");
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testGetByName() {
      // Arrange
      PRMInstance                       i("i", *classA__);
      gum::prm::PRMAttribute< double >* attr = nullptr;
      auto&                             class_attr = classA__->get("a");
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
      PRMInstance                             i("i", *classA__);
      const auto&                             const_i = i;
      gum::prm::PRMAttribute< double > const* attr = nullptr;
      const auto&                             class_attr = classA__->get("a");
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
      PRMInstance i("i", *classA__);
      // Act & Assert
      TS_ASSERT_THROWS(i.get("azeaze"), gum::NotFound);
    }

    void testGetByNameConstNotFound() {
      // Arrange
      PRMInstance i("i", *classA__);
      const auto& const_i = i;
      // Act & Assert
      TS_ASSERT_THROWS(const_i.get("azeaze"), gum::NotFound);
    }

    void testGetById() {
      // Arrange
      PRMInstance                       i("i", *classA__);
      gum::prm::PRMAttribute< double >* attr = nullptr;
      auto&                             class_attr = classA__->get("a");
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
      PRMInstance                             i("i", *classA__);
      const auto&                             const_i = i;
      gum::prm::PRMAttribute< double > const* attr = nullptr;
      const auto&                             class_attr = classA__->get("a");
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
      PRMInstance i("i", *classA__);
      // Act & Assert
      TS_ASSERT_THROWS(i.get(666), gum::NotFound);
    }

    void testGetByIdConstNotFound() {
      // Arrange
      PRMInstance i("i", *classA__);
      const auto& const_i = i;
      // Act & Assert
      TS_ASSERT_THROWS(const_i.get(666), gum::NotFound);
    }

    void testSize() {
      // Arrage
      PRMInstance i("i", *classA__);
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
      PRMInstance i("i", *classA__);
      // Act && Assert
      TS_ASSERT_THROWS_NOTHING(i.instantiate());
    }

    void testBijection() {
      // Arrange
      PRMInstance      i("i", *classA__);
      Bijection const* bij = nullptr;
      auto             var_a = &(classA__->get("a").type().variable());
      auto             var_b = &(classA__->get("b").type().variable());
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
