
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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


#include <iostream>
#include <sstream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/variables/rangeVariable.h>

namespace gum_tests {

  class RangeVariableTestSuite: public CxxTest::TestSuite {
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
      TS_ASSERT(var1.belongs(3L));
      TS_ASSERT(!var1.belongs(0L));
      TS_ASSERT(!var1.belongs(10L));

      var1.setMinVal(3);

      TS_ASSERT_EQUALS(var1.label(1), "4");
      TS_ASSERT_EQUALS(var1["4"], (gum::Idx)1);

      gum::DiscreteVariable& v = var1;

      TS_ASSERT_EQUALS(v.label(1), "4");
      TS_ASSERT_EQUALS(v["4"], (gum::Idx)1);

      std::stringstream s;
      s << v;
      TS_ASSERT_EQUALS(s.str(), "var1[3,9]");
      TS_ASSERT_EQUALS(v.toString(), "var1[3,9]");
    }

    void testNumerical() {
      gum::RangeVariable var1("var1", "this is var1", 10, 20);
      TS_ASSERT_EQUALS(var1.domainSize(), (gum::Size)11);

      for (gum::Idx i = 0; i < var1.domainSize(); i++) {
        TS_ASSERT_EQUALS(var1.numerical(i), var1.minVal() + i);
      }
    }
  };
}   // namespace gum_tests
