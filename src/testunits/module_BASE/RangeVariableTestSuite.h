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

#include <iostream>
#include <sstream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/rangeVariable.h>

namespace gum_tests {

  class RangeVariableTestSuite : public CxxTest::TestSuite {

    public:
    void testCopy() {
      gum::RangeVariable var1("var1", "this is var1");
      gum::RangeVariable var2("var2", "this is var2", 1, 4);

      gum::RangeVariable var3(var1);
      gum::RangeVariable var4("var4", "this is var4");
      TS_GUM_ASSERT_THROWS_NOTHING(var4 = var2);

      TS_ASSERT_EQUALS(var4.minVal(), var2.minVal());
      TS_ASSERT_EQUALS(var1.maxVal(), var3.maxVal());
      TS_ASSERT_DIFFERS(var4.minVal(), var1.minVal());
    }

    void testLabels() {
      gum::RangeVariable var1("var1", "this is var1");
      TS_ASSERT_EQUALS(var1.domainSize(), (gum::Size)2);
      TS_ASSERT(!var1.empty());

      var1.setMinVal(1);
      var1.setMaxVal(0);
      TS_ASSERT(var1.empty());

      var1.setMaxVal(9);
      TS_ASSERT(!var1.empty());
      TS_ASSERT_EQUALS(var1.domainSize(), (gum::Size)9);
      TS_ASSERT(var1.belongs((gum::Idx)3));
      TS_ASSERT(!var1.belongs((gum::Idx)0));
      TS_ASSERT(!var1.belongs((gum::Idx)10));

      var1.setMinVal(3);

      TS_ASSERT_EQUALS(var1.label(1), "4");
      TS_ASSERT_EQUALS(var1["4"], (gum::Idx)1);

      gum::DiscreteVariable &v = var1;

      TS_ASSERT_EQUALS(v.label(1), "4");
      TS_ASSERT_EQUALS(v["4"], (gum::Idx)1);

      std::stringstream s;
      s << v;
      TS_ASSERT_EQUALS(s.str(), "var1[3-9]");
      TS_ASSERT_EQUALS(v.toString(), "var1[3-9]");
    }

    void testNumerical() {
      gum::RangeVariable var1("var1", "this is var1", 10, 20);
      TS_ASSERT_EQUALS(var1.domainSize(), (gum::Size)11);

      for (gum::Idx i = 0; i < var1.domainSize(); i++) {
        TS_ASSERT_EQUALS(var1.numerical(i), var1.minVal() + i);
      }
    }
  };
}
