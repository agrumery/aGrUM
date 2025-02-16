/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#ifndef GUM_CLASSELEMENT_TEST_SUITE_H
#define GUM_CLASSELEMENT_TEST_SUITE_H

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/exceptions.h>
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
