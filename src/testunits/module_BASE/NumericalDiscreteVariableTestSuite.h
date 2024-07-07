/**
 *
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <gumtest/utils.h>

#include <agrum/base/variables/numericalDiscreteVariable.h>

namespace gum_tests {

  class [[maybe_unused]] NumericalDiscreteVariableTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(All) {
      gum::NumericalDiscreteVariable var1("var1", "this is var1");
      gum::NumericalDiscreteVariable var2("var2", "this is var2", {1.0, 9.45, 7.0});

      TS_ASSERT_EQUALS(var1.domainSize(), (gum::Size)0)
      const auto& vect1 = var1.numericalDomain();
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)(gum::Size)0)
      TS_ASSERT_EQUALS(var2.domainSize(), (gum::Size)3)
      const auto& vect2 = var2.numericalDomain();
      TS_ASSERT_EQUALS(vect2[0], 1.0)
      TS_ASSERT_EQUALS(vect2[1], 7.0)
      TS_ASSERT_EQUALS(vect2[2], 9.45)

      gum::NumericalDiscreteVariable var3(var2);
      TS_ASSERT_EQUALS(var3.domainSize(), (gum::Size)3)
      const auto& vect3 = var3.numericalDomain();
      TS_ASSERT_EQUALS(vect3[0], 1.0)
      TS_ASSERT_EQUALS(vect3[1], 7.0)
      TS_ASSERT_EQUALS(vect3[2], 9.45)

      gum::NumericalDiscreteVariable var4(std::move(var3));
      TS_ASSERT_EQUALS(var4.domainSize(), (gum::Size)3)
      const auto& vect4 = var4.numericalDomain();
      TS_ASSERT_EQUALS(vect4[0], 1.0)
      TS_ASSERT_EQUALS(vect4[1], 7.0)
      TS_ASSERT_EQUALS(vect4[2], 9.45)
      TS_ASSERT(vect3.empty())
      TS_ASSERT_EQUALS(var3.domainSize(), (gum::Size)0)

      gum::NumericalDiscreteVariable* var5 = var4.clone();
      TS_ASSERT_EQUALS(var5->domainSize(), (gum::Size)3)
      const auto& vect5 = var5->numericalDomain();
      TS_ASSERT_EQUALS(vect5[0], 1.0)
      TS_ASSERT_EQUALS(vect5[1], 7.0)
      TS_ASSERT_EQUALS(vect5[2], 9.45)

      gum::NumericalDiscreteVariable var6("var6", "", {-2, -1, 4.25, 8});
      gum::NumericalDiscreteVariable var7 = var2;
      TS_ASSERT_EQUALS(var7.domainSize(), (gum::Size)3)
      var7 = var6;
      TS_ASSERT_EQUALS(var7.domainSize(), (gum::Size)4)
      const auto& vect7 = var7.numericalDomain();
      TS_ASSERT_EQUALS(vect7[0], -2)
      TS_ASSERT_EQUALS(vect7[1], -1)
      TS_ASSERT_EQUALS(vect7[2], 4.25)
      TS_ASSERT_EQUALS(vect7[3], 8)

      var3 = std::move(var7);
      TS_ASSERT_EQUALS(vect3[0], -2)
      TS_ASSERT_EQUALS(vect3[1], -1)
      TS_ASSERT_EQUALS(vect3[2], 4.25)
      TS_ASSERT_EQUALS(vect3[3], 8)
      TS_ASSERT_EQUALS(var7.domainSize(), (gum::Size)0)

      TS_ASSERT_DIFFERS(var7, var6)
      TS_ASSERT_EQUALS(var3, var6)

      TS_ASSERT_EQUALS(var3.varType(), gum::VarType::NUMERICAL)
      TS_ASSERT_THROWS(var3.index("0"), const gum::NotFound&)
      TS_GUM_ASSERT_THROWS_NOTHING(var3.index("-1"))
      TS_ASSERT_EQUALS(var3.index("-2"), gum::Idx(0))
      TS_ASSERT_EQUALS(var3.index("-1"), gum::Idx(1))
      TS_ASSERT_EQUALS(var3.index("4.25"), gum::Idx(2))
      TS_ASSERT_EQUALS(var3.index("8"), gum::Idx(3))
      TS_ASSERT_EQUALS(var3.index("  8.0  "), gum::Idx(3))
      TS_ASSERT_EQUALS(var3.index("  8"), gum::Idx(3))

      TS_ASSERT_EQUALS(var3.label(0), "-2")
      TS_ASSERT_EQUALS(var3.label(1), "-1")
      TS_ASSERT_EQUALS(var3.label(2), "4.25")
      TS_ASSERT_EQUALS(var3.label(3), "8")
      TS_ASSERT_THROWS(var3.label(4), const gum::OutOfBounds&)

      TS_ASSERT_EQUALS(var3.numerical(0), -2.0)
      TS_ASSERT_EQUALS(var3.numerical(1), -1.0)
      TS_ASSERT_EQUALS(var3.numerical(2), 4.25)
      TS_ASSERT_EQUALS(var3.numerical(3), 8.0)
      TS_ASSERT_THROWS(var3.numerical(4), const gum::OutOfBounds&)

      TS_ASSERT_EQUALS(var3.domain(), "{-2|-1|4.25|8}")
      TS_ASSERT_EQUALS(var1.domain(), "{}")

      var1.addValue(4.25);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)1)
      TS_ASSERT_EQUALS(vect1[0], 4.25)
      var1.addValue(8);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)2)
      TS_ASSERT_EQUALS(vect1[0], 4.25)
      TS_ASSERT_EQUALS(vect1[1], 8)
      var1.addValue(6);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)3)
      TS_ASSERT_EQUALS(vect1[0], 4.25)
      TS_ASSERT_EQUALS(vect1[1], 6)
      TS_ASSERT_EQUALS(vect1[2], 8)
      var1.addValue(2);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(vect1[0], 2)
      TS_ASSERT_EQUALS(vect1[1], 4.25)
      TS_ASSERT_EQUALS(vect1[2], 6)
      TS_ASSERT_EQUALS(vect1[3], 8)
      TS_ASSERT_THROWS(var1.addValue(2), const gum::DuplicateElement&)
      TS_ASSERT_THROWS(var1.addValue(4.25), const gum::DuplicateElement&)
      TS_ASSERT_THROWS(var1.addValue(6), const gum::DuplicateElement&)
      TS_ASSERT_THROWS(var1.addValue(8), const gum::DuplicateElement&)

      var1.changeValue(6, 7);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(vect1[0], 2)
      TS_ASSERT_EQUALS(vect1[1], 4.25)
      TS_ASSERT_EQUALS(vect1[2], 7)
      TS_ASSERT_EQUALS(vect1[3], 8)

      var1.changeValue(7, 3);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(vect1[0], 2)
      TS_ASSERT_EQUALS(vect1[1], 3)
      TS_ASSERT_EQUALS(vect1[2], 4.25)
      TS_ASSERT_EQUALS(vect1[3], 8)

      var1.changeValue(3, 0);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(vect1[0], 0)
      TS_ASSERT_EQUALS(vect1[1], 2)
      TS_ASSERT_EQUALS(vect1[2], 4.25)
      TS_ASSERT_EQUALS(vect1[3], 8)

      var1.changeValue(0, 10);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(vect1[0], 2)
      TS_ASSERT_EQUALS(vect1[1], 4.25)
      TS_ASSERT_EQUALS(vect1[2], 8)
      TS_ASSERT_EQUALS(vect1[3], 10)

      var1.changeValue(4.25, 12.25);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(vect1[0], 2)
      TS_ASSERT_EQUALS(vect1[1], 8)
      TS_ASSERT_EQUALS(vect1[2], 10)
      TS_ASSERT_EQUALS(vect1[3], 12.25)

      TS_ASSERT_THROWS(var1.changeValue(12.25, 10), const gum::DuplicateElement&)
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)4)
      TS_ASSERT_EQUALS(vect1[0], 2)
      TS_ASSERT_EQUALS(vect1[1], 8)
      TS_ASSERT_EQUALS(vect1[2], 10)
      TS_ASSERT_EQUALS(vect1[3], 12.25)

      TS_ASSERT_THROWS_NOTHING(var1.changeValue(22, 10))

      var1.eraseValue(8);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)3)
      TS_ASSERT_EQUALS(vect1[0], 2)
      TS_ASSERT_EQUALS(vect1[1], 10)
      TS_ASSERT_EQUALS(vect1[2], 12.25)

      TS_GUM_ASSERT_THROWS_NOTHING(var1.eraseValue(22))
      var1.eraseValue(2);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)2)
      TS_ASSERT_EQUALS(vect1[0], 10)
      TS_ASSERT_EQUALS(vect1[1], 12.25)
      var1.eraseValue(12.25);
      TS_ASSERT_EQUALS(vect1.size(), (gum::Size)1)
      TS_ASSERT_EQUALS(vect1[0], 10)

      TS_ASSERT_EQUALS(var6.domainSize(), (gum::Size)4)
      var6.eraseValues();
      TS_ASSERT_EQUALS(var6.domainSize(), (gum::Size)0)

      delete var5;
    }

    GUM_ACTIVE_TEST(SecondConstructor) {
      {
        gum::NumericalDiscreteVariable var("var", "this is var2", 0.0, 2.5, gum::Size(6));

        TS_ASSERT_EQUALS(var.domainSize(), (gum::Size)6)
        const auto& vect = var.numericalDomain();
        TS_ASSERT_EQUALS(vect.size(), (gum::Size)(gum::Size)6)
        TS_ASSERT_EQUALS(vect[0], 0.0)
        TS_ASSERT_EQUALS(vect[1], 0.5)
        TS_ASSERT_EQUALS(vect[2], 1.0)
        TS_ASSERT_EQUALS(vect[3], 1.5)
        TS_ASSERT_EQUALS(vect[4], 2.0)
        TS_ASSERT_EQUALS(vect[5], 2.5)
      }
      {
        gum::NumericalDiscreteVariable var("var", "this is var2", 0.0, 2.5, gum::Size(7));

        TS_ASSERT_EQUALS(var.domainSize(), (gum::Size)7)
        const auto& vect = var.numericalDomain();
        TS_ASSERT_EQUALS(vect.size(), (gum::Size)(gum::Size)7)
        TS_ASSERT_EQUALS(vect[0], 0.0)
        TS_ASSERT_EQUALS(vect[1], 0.4167)
        TS_ASSERT_EQUALS(vect[2], 0.8333)
        TS_ASSERT_EQUALS(vect[3], 1.25)
        TS_ASSERT_EQUALS(vect[4], 1.6667)
        TS_ASSERT_EQUALS(vect[5], 2.0833)
        TS_ASSERT_EQUALS(vect[6], 2.5)
      }
    }

    GUM_ACTIVE_TEST(InfiniteValue) {
      TS_ASSERT_THROWS(gum::NumericalDiscreteVariable(
                           "var",
                           "this is var2",
                           {-std::numeric_limits< double >::infinity(), 0.0, 2.5, 3.7, 4.9}),
                       const gum::DefaultInLabel&);
    }

    GUM_ACTIVE_TEST(Positions) {
      gum::NumericalDiscreteVariable var("var", "this is var2", {-2.1, 0.0, 2.5, 3.7, 4.9});
      TS_ASSERT_EQUALS(var.closestIndex(-3.9), gum::Idx(0))
      TS_ASSERT_EQUALS(var.closestIndex(532.2), gum::Idx(4))
      TS_ASSERT_EQUALS(var.closestIndex(0), gum::Idx(1))
      TS_ASSERT_EQUALS(var.closestIndex(-2.05), gum::Idx(0))
      TS_ASSERT_EQUALS(var.closestIndex(-0.05), gum::Idx(1))
      TS_ASSERT_EQUALS(var.closestIndex(-1.05), gum::Idx(0))   // arbitrary
      TS_ASSERT_EQUALS(var.closestIndex(4.8), gum::Idx(4))
      TS_ASSERT_EQUALS(var.closestIndex(3.8), gum::Idx(3))
      TS_ASSERT_EQUALS(var.closestIndex(4.3), gum::Idx(3))     // arbitrary

      gum::NumericalDiscreteVariable v("v", "v", 4, 14, 300);
      TS_ASSERT_EQUALS(v.closestLabel(9.0), "8.9833")
      TS_ASSERT_EQUALS(v.closestLabel(8.983), "8.9833")
      TS_ASSERT_EQUALS(v.closestLabel(8.984), "8.9833")
      TS_ASSERT_EQUALS(v.closestLabel(8.9), "8.9164")
      TS_ASSERT_EQUALS(v.closestLabel(9.1), "9.0836")
      TS_ASSERT_EQUALS(v.closestLabel(1.0), "4")
      TS_ASSERT_EQUALS(v.closestLabel(15.0), "14")
    }
  };
}   // namespace gum_tests
