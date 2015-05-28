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

#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/PRM/elements/type.h>

#include <module_PRM/ClassElementTestSuite.h>

/**
 * This class is used to test gum::prm::ClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of 
 * gum::prm::ClassElement.
 */
namespace gum_tests {

  class PRMTypeTestSuite : public CxxTest::TestSuite {
    typedef gum::prm::Type<double> Type;

    public:

    void setUp() {
    }

    void tearDown() {
    }

    void testStaticBoolean() {
      // Arrange
      std::string labels[] = { "false", "true" };
      std::string name = "boolean";
      std::string decription = "Boolean variable";
      // Act
      auto boolean = Type::boolean();
      // Assert
      TS_ASSERT_EQUALS(boolean->variable().domainSize(), (gum::Size) 2);
      TS_ASSERT_EQUALS(boolean->variable().label(0), labels[0]);
      TS_ASSERT_EQUALS(boolean->variable().label(1), labels[1]);
      delete boolean;
    }

  };

} // gum_tests
