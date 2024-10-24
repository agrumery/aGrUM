/**************************************************************************
 *   Copyright (c) 2017-2024 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)  *
 *   info_at_agrum_dot_org                                               *
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

#ifndef GUM_CLASSELEMENT_TEST_SUITE_H
#define GUM_CLASSELEMENT_TEST_SUITE_H

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/tools/core/exceptions.h>
#include <agrum/PRM/elements/PRMClassElement.h>

/*
 * This class is used to test gum::prm::PRMClassElement, since it is an abstract
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class ClassElementTestSuiteAbstract {
    using ClassElt = gum::prm::PRMClassElement< double >;

    public:
    void testIsReferenceSlot(const ClassElt& elt, bool expected) {
      // Arrange
      // Act
      bool actual = ClassElt::isReferenceSlot(elt);
      // Assert
      TS_ASSERT_EQUALS(actual, expected)
    }

    void testIsAttribute(const ClassElt& elt, bool expected) {
      // Arrange
      // Act
      bool actual = ClassElt::isAttribute(elt);
      // Assert
      TS_ASSERT_EQUALS(actual, expected)
    }

    void testIsSlotChain(const ClassElt& elt, bool expected) {
      // Arrange
      // Act
      bool actual = ClassElt::isSlotChain(elt);
      // Assert
      TS_ASSERT_EQUALS(actual, expected)
    }

    void testSetNodeId(ClassElt& elt) {
      // Arrange
      auto old_id = elt.id();
      auto new_id = old_id + 1;
      // Act
      elt.setId(new_id);
      // Assert
      TS_ASSERT_DIFFERS(old_id, elt.id())
      TS_ASSERT_EQUALS(new_id, elt.id())
    }

    void test_obj_type(const ClassElt& elt) {
      // Arrange
      auto expected = gum::prm::PRMObject::prm_type::CLASS_ELT;
      // Act
      auto actual = elt.obj_type();
      // Assert
      TS_ASSERT_EQUALS(expected, actual)
    }

    void testSafeName(const ClassElt& elt) {
      // Arrange
      std::string expected;
      if (ClassElt::isReferenceSlot(elt)) {
        expected = gum::prm::PRMObject::LEFT_CAST()
                 + static_cast< const gum::prm::PRMReferenceSlot< double >& >(elt).slotType().name()
                 + gum::prm::PRMObject::RIGHT_CAST() + elt.name();
      } else {
        expected = gum::prm::PRMObject::LEFT_CAST() + elt.type().name()
                 + gum::prm::PRMObject::RIGHT_CAST() + elt.name();
      }
      // Act
      auto actual = elt.safeName();
      // Assert
      TS_ASSERT_EQUALS(expected, actual)
    }

    void testCast_NotAllowed(const ClassElt& elt) {
      // Arrange
      gum::LabelizedVariable foo{"foo", "A dummy variable"};
      gum::prm::PRMType      bar{foo};
      // Assert
      try {
        TS_ASSERT_THROWS(elt.cast(bar), const gum::OperationNotAllowed&)
      } catch (gum::OperationNotAllowed&) {}
    }

    void testCast(const ClassElt& elt, const gum::prm::PRMType& type) {
      try {
        // Arrange
        auto expected = gum::prm::PRMObject::LEFT_CAST() + type.name()
                      + gum::prm::PRMObject::RIGHT_CAST() + elt.name();
        std::string actual;
        // Act
        TS_ASSERT_THROWS_NOTHING(actual = elt.cast(type))
        // Assert
        TS_ASSERT_EQUALS(expected, actual)
      } catch (gum::OperationNotAllowed&) { TS_FAIL("Exception raised"); }
    }
  };

}   // namespace gum_tests

#endif   // GUM_CLASSELEMENT_TEST_SUITE_H
