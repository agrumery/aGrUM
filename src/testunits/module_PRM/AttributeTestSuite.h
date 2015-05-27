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

  class AttributeTestSuite : public CxxTest::TestSuite {
    private:
      ClassElementTestSuite* __classEltTestSuite;
    public:

    void setUp() {
      __classEltTestSuite = new ClassElementTestSuite;
    }

    void tearDown() {
      delete __classEltTestSuite;
    }

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

  };

} // gum_tests
