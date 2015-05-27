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

#include <agrum/PRM/elements/classElement.h>

/**
 * This class is used to test gum::prm::ClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of 
 * gum::prm::ClassElement.
 */
namespace gum_tests {

  class ClassElementTestSuite {
    public:

    void testIsReferenceSlot(const gum::prm::ClassElement<double> &elt, bool expected) {
      // Arrange
      // Act
      bool actual = gum::prm::ClassElement<double>::isReferenceSlot(elt);
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testIsAttribute(const gum::prm::ClassElement<double> &elt, bool expected) {
      // Arrange
      // Act
      bool actual = gum::prm::ClassElement<double>::isAttribute(elt);
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testIsSlotChain(const gum::prm::ClassElement<double> &elt, bool expected) {
      // Arrange
      // Act
      bool actual = gum::prm::ClassElement<double>::isSlotChain(elt);
      // Assert
      TS_ASSERT_EQUALS(actual, expected);
    }

    void testSetNodeId(gum::prm::ClassElement<double> &elt) {
      // Arrange
      auto old_id = elt.id();
      auto new_id = old_id + 1;
      // Act
      elt.setId(new_id);
      // Assert
      TS_ASSERT_DIFFERS(old_id, elt.id());
      TS_ASSERT_EQUALS(new_id, elt.id());
    }

  };

} // gum_tests
