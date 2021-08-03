/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class LabelizedVariableTestSuite: public CxxTest::TestSuite {
    public:
    void testCopy() {
      gum::LabelizedVariable var1("var1", "this is var1", 2);
      gum::LabelizedVariable var2("var2", "this is var2", 2);

      gum::LabelizedVariable var3(var1);
      gum::LabelizedVariable var4("var4", "this is var4");
      TS_GUM_ASSERT_THROWS_NOTHING(var4 = var2);

      TS_ASSERT_EQUALS(var4, var2);
      TS_ASSERT_EQUALS(var1, var3);
      TS_ASSERT_DIFFERS(var4, var1);
    }

    void testLabels() {
      gum::LabelizedVariable var1("var1", "this is var1", 0);
      TS_ASSERT_EQUALS(var1.domainSize(), (gum::Size)0);
      TS_ASSERT(var1.empty());
      var1.addLabel("4").addLabel("3").addLabel("2").addLabel("1");

      TS_ASSERT_EQUALS(var1.domainSize(), (gum::Size)4);
      TS_ASSERT_EQUALS(var1.label(1), "3");
      TS_ASSERT_EQUALS(var1["3"], (gum::Idx)1);

      TS_ASSERT_THROWS(var1.addLabel("3"), gum::DuplicateElement)

      std::stringstream s;
      s << var1;
      TS_ASSERT_EQUALS(s.str(), "var1:Labelized(<4,3,2,1>)");

      TS_ASSERT_EQUALS(var1.toString(), "var1:Labelized(<4,3,2,1>)");
    }

    void testChangeLabel() {
      gum::LabelizedVariable var1("var1", "this is var1", 0);
      var1.addLabel("4").addLabel("3").addLabel("2").addLabel("1");

      TS_ASSERT_EQUALS(var1.toString(), "var1:Labelized(<4,3,2,1>)");

      var1.changeLabel(1, "x");
      TS_ASSERT_EQUALS(var1.toString(), "var1:Labelized(<4,x,2,1>)");

      const gum::LabelizedVariable& var2 = var1;
      TS_ASSERT_EQUALS(var2.toString(), "var1:Labelized(<4,x,2,1>)");
      var2.changeLabel(1, "y");
      TS_ASSERT_EQUALS(var2.toString(), "var1:Labelized(<4,y,2,1>)");

      TS_GUM_ASSERT_THROWS_NOTHING(
         var1.changeLabel(1, "x"));   // should be OK since label 1 is already "x"
      TS_ASSERT_THROWS(var1.changeLabel(0, "x"), gum::DuplicateElement);
      TS_ASSERT_THROWS(var1.changeLabel(1000, "x"), gum::OutOfBounds);
    }

    void testNumerical() {
      gum::LabelizedVariable var1("var1", "this is var1", 0);
      var1.addLabel("4").addLabel("3").addLabel("2").addLabel("1");

      TS_ASSERT_EQUALS(var1.numerical(0), 0);
      TS_ASSERT_EQUALS(var1.numerical(1), 1);
      TS_ASSERT_EQUALS(var1.numerical(2), 2);
      TS_ASSERT_EQUALS(var1.numerical(3), 3);
    }


    void testAndConstructorWithLabels() {
      gum::LabelizedVariable var1("var1", "this is var1", {"rouge", "vert", "bleu"});
      TS_ASSERT_EQUALS(var1.toString(), "var1:Labelized(<rouge,vert,bleu>)");
      TS_ASSERT_EQUALS(var1.posLabel("vert"), gum::Idx(1));
    }
  };
}   // namespace gum_tests
