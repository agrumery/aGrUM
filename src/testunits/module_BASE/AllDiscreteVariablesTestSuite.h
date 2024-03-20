/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <agrum/tools/variables/allDiscreteVariables.h>

namespace gum_tests {

  class [[maybe_unused]] AllVariablesTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(CreationRange) {
      try {
        {
          auto a = FASTVARDBL("A1", 2);
          TS_ASSERT_EQUALS(a->toString(), "A1:Range([0,1])");
        }
        {
          // a way to create a variable with only one value
          auto a = FASTVARDBL("A2", 1);
          TS_ASSERT_EQUALS(a->toString(), "A2:Range([0,0])");
        }
        {
          auto a = FASTVARDBL("A3", 4);
          TS_ASSERT_EQUALS(a->toString(), "A3:Range([0,3])");
        }
        {
          auto a = FASTVARDBL("A4[3]", 4);
          TS_ASSERT_EQUALS(a->toString(), "A4:Range([0,2])");
        }
        {
          auto a = FASTVARDBL("A5[3,7]", 4);
          TS_ASSERT_EQUALS(a->toString(), "A5:Range([3,7])");
        }
        {
          auto a = FASTVARDBL("A6[-7,-3]", 4);
          TS_ASSERT_EQUALS(a->toString(), "A6:Range([-7,-3])");
        }

        TS_ASSERT_THROWS(auto a = FASTVARDBL("A7[7,3]", 4), const gum::InvalidArgument&);

        TS_ASSERT_THROWS(auto a = FASTVARDBL("A8[-5]", 4), const gum::InvalidArgument&);

        TS_ASSERT_THROWS(auto a = FASTVARDBL("A9[1,1]", 4), const gum::InvalidArgument&);

        {
          auto a = FASTVARDBL("AA[1,1]", 1);
          TS_ASSERT_EQUALS(a->toString(), "AA:Range([1,1])");
        }
      } catch (gum::Exception const& e) GUM_SHOWERROR(e);
    }

    GUM_ACTIVE_TEST(CreationLabelized) {
      try {
        {
          auto a = FASTVARDBL("A{a|b|c}", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Labelized({a|b|c})");
        }
        TS_ASSERT_THROWS(auto a = FASTVARDBL("A{a}", 4), const gum::InvalidArgument&);

        {
          auto a = FASTVARDBL("A{a}", 1);
          TS_ASSERT_EQUALS(a->toString(), "A:Labelized({a})");
        }
      } catch (gum::Exception const& e) { GUM_SHOWERROR(e); }
    }

    GUM_ACTIVE_TEST(CreationInteger) {
      {
        auto a = FASTVARDBL("A{0|3|5}", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Integer({0|3|5})");
      }
      {
        auto a = FASTVARDBL("A{-3|0|3}", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Integer({-3|0|3})");
      }
      {
        auto a = FASTVARDBL("A{15|-3|0|3}", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Integer({-3|0|3|15})");
      }
      TS_ASSERT_THROWS(auto a = FASTVARDBL("A{15}", 4), gum::InvalidArgument&);

      {
        auto a = FASTVARDBL("A{15}", 1);
        TS_ASSERT_EQUALS(a->toString(), "A:Integer({15})");
      }
      {
        auto a = FASTVARDBL("A{15|14|12|13|11}");
        TS_ASSERT_EQUALS(a->toString(), "A:Range([11,15])");
      }
    }

    GUM_ACTIVE_TEST(CreationDiscretized) {
      {
        auto a = FASTVARDBL("A[1,2,3,4,5,6]", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)");
        TS_ASSERT(!a->isEmpirical())
      }
      {
        auto a = FASTVARDBL("A+[1,2,3,4,5,6]", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Discretized(<(1;2[,[2;3[,[3;4[,[4;5[,[5;6)>)");
        TS_ASSERT(a->isEmpirical())
      }

      TS_ASSERT_THROWS(auto a = FASTVARDBL("A[0.3]", 4), gum::InvalidArgument&);
      TS_ASSERT_THROWS(auto a = FASTVARDBL("A[0.3]", 1), gum::InvalidArgument&);
    }

    GUM_ACTIVE_TEST(CreationDiscretized2) {
      {
        auto a = FASTVARDBL("A[1:6:5]", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)");
        TS_ASSERT(!a->isEmpirical())
      }
      {
        auto a = FASTVARDBL("A[1:6:2]", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Discretized(<[1;3.5[,[3.5;6]>)");
        TS_ASSERT(!a->isEmpirical())
      }
      {
        auto a = FASTVARDBL("A+[1:6:5]", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Discretized(<(1;2[,[2;3[,[3;4[,[4;5[,[5;6)>)");
        TS_ASSERT(a->isEmpirical())
      }
      {
        auto a = FASTVARDBL("A+[1:6:2]", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Discretized(<(1;3.5[,[3.5;6)>)");
        TS_ASSERT(a->isEmpirical())
      }

      TS_ASSERT_THROWS(auto a = FASTVARDBL("A[1:6:1]", 4), gum::InvalidArgument&);
    }

    GUM_ACTIVE_TEST(CreationNumerical) {
      try {
        {
          auto a = FASTVARDBL("A{1|1.5|6|2.9|3.14}", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:NumericalDiscrete({1|1.5|2.9|3.14|6})");
        }
        TS_ASSERT_THROWS(auto a = FASTVARDBL("A{3.14}", 4), const gum::InvalidArgument&);
        {
          auto a = FASTVARDBL("A{3.14}", 1);
          TS_ASSERT_EQUALS(a->toString(), "A:NumericalDiscrete({3.14})");
        }
        {
          auto a = FASTVARDBL("A{1.2:5.2:5}", 5);
          TS_ASSERT_EQUALS(a->toString(), "A:NumericalDiscrete({1.2|2.2|3.2|4.2|5.2})");
        }
        {
          auto a = FASTVARDBL("A{1.2:5.2:2}", 2);
          TS_ASSERT_EQUALS(a->toString(), "A:NumericalDiscrete({1.2|5.2})");
          auto b = FASTVARDBL("A{1.2|5.2}", 2);
          TS_ASSERT_EQUALS(b->toString(), "A:NumericalDiscrete({1.2|5.2})");
        }
        {
          auto a = FASTVARDBL("A{1.2:5.2:2.8}", 2);
          TS_ASSERT_EQUALS(a->toString(), "A:NumericalDiscrete({1.2|5.2})");
        }
      } catch (gum::Exception const& e) {
        GUM_SHOWERROR(e);
        GUM_TRACE(e.errorCallStack());
      }
    }

    GUM_ACTIVE_TEST(ToFastMethod) {
      std::string s;
      s = "A{On|Off|Defun}";
      TS_ASSERT_EQUALS(s, (FASTVARDBL(s)->toFast()));             // labelized
      s = "A{0|1.15|2.3}";
      TS_ASSERT_EQUALS(s, (FASTVARDBL(s)->toFast()));             // NumericalDiscrete
      TS_ASSERT_EQUALS(s,
                       (FASTVARDBL("A{0:2.3:3}")->toFast()));     // NumericalDiscrete
      s = "A{1|3|5}";
      TS_ASSERT_EQUALS(s, (FASTVARDBL(s)->toFast()));             // Integer
      TS_ASSERT_EQUALS(s, (FASTVARDBL("A{1:5:3}")->toFast()));    // Integer
      s = "A[3,5]";
      TS_ASSERT_EQUALS(s, (FASTVARDBL(s)->toFast()));             // Range
      s = "A[5]";
      TS_ASSERT_EQUALS(s, (FASTVARDBL(s)->toFast()));             // Range
      s = "A[1,2,3,4,5,6]";
      TS_ASSERT_EQUALS(s, (FASTVARDBL(s)->toFast()));             // Discretized
      TS_ASSERT_EQUALS(s, (FASTVARDBL("A[1:6:5]")->toFast()));    // Discretized
      s = "A+[1,2,3,4,5,6]";
      TS_ASSERT_EQUALS(s, (FASTVARDBL(s)->toFast()));             // Discretized
      TS_ASSERT_EQUALS(s, (FASTVARDBL("A+[1:6:5]")->toFast()));   // Discretized
    }

    GUM_ACTIVE_TEST(FastNumericalWithoutInfinity) {
      std::string s;
      s = "A{0|1.15|inf}";   // NumericalDiscrete
      TS_ASSERT_THROWS(FASTVARDBL(s), const gum::DefaultInLabel&)
      s = "A{1|3|inf}";      // Integer
      TS_ASSERT_THROWS(FASTVARDBL(s), const gum::DefaultInLabel&)
      s = "A[3,2.5,inf]";    // Discretized
      // TS_ASSERT_THROWS(FASTVARDBL(s), const gum::DefaultInLabel&)
      s = "A[inf]";             // Range
      TS_ASSERT_THROWS(FASTVARDBL(s), const std::invalid_argument&)
      s = "A[2,inf]";           // Range
      TS_ASSERT_THROWS(FASTVARDBL(s), const std::invalid_argument&)
      s = "A[inf,2,3,4,5,6]";   // Integer
      // TS_ASSERT_THROWS(FASTVARDBL(s), const gum::DefaultInLabel&)
    }

    GUM_ACTIVE_TEST(LabelizedComparison) {
      TS_ASSERT(*FASTVARDBL("A{On|Off|Defun}") == *FASTVARDBL("A{On|Off|Defun}"))
      TS_ASSERT(*FASTVARDBL("A{On|Off|Defun}") != *FASTVARDBL("A{Off|On|Defun}"))
      TS_ASSERT(*FASTVARDBL("A{On|Off|Defun}") != *FASTVARDBL("B{On|Off|Defun}"))
      TS_ASSERT(*FASTVARDBL("A{On|Off|Defun}") != *FASTVARDBL("A{On|Off}"))
      TS_ASSERT(*FASTVARDBL("A{On|Off|Defun}") != *FASTVARDBL("A{On|Off|Defun|Beurk}"))
    }

    GUM_ACTIVE_TEST(RangeComparison) {
      TS_ASSERT(*FASTVARDBL("A[3,5]") == *FASTVARDBL("A[3,5]"))
      TS_ASSERT(*FASTVARDBL("A[5]  ") == *FASTVARDBL("A[0,4]"))
      TS_ASSERT(*FASTVARDBL("A[3,5]") != *FASTVARDBL("B[3,5]"))
      TS_ASSERT(*FASTVARDBL("A[3,5]") != *FASTVARDBL("A[2,5]"))
      TS_ASSERT(*FASTVARDBL("A[3,5]") != *FASTVARDBL("A[3,6]"))
    }

    GUM_ACTIVE_TEST(IntegerComparison) {
      TS_ASSERT(*FASTVARDBL("A{3|5|9}") == *FASTVARDBL("A{3|5|9}"))
      TS_ASSERT(*FASTVARDBL("A{3|5|9}") == *FASTVARDBL("A{3|9|5}"))
      TS_ASSERT(*FASTVARDBL("A{3|5|9}") != *FASTVARDBL("B{3|9|5}"))
      TS_ASSERT(*FASTVARDBL("A{3|5|9}") != *FASTVARDBL("A{3|9}"))
      TS_ASSERT(*FASTVARDBL("A{3|5|9}") != *FASTVARDBL("A{3|9|5|12}"))
      TS_ASSERT(*FASTVARDBL("A{1:7:4}") == *FASTVARDBL("A{1|3|5|7}"))
    }

    GUM_ACTIVE_TEST(NumericalDiscreteComparison) {
      TS_ASSERT(*FASTVARDBL("A{3.4|5.4|9.4}") == *FASTVARDBL("A{3.4|5.4|9.4}"))
      TS_ASSERT(*FASTVARDBL("A{3.4|5.4|9.4}") == *FASTVARDBL("A{3.4|9.4|5.4}"))
      TS_ASSERT(*FASTVARDBL("A{3.4|5.4|9.4}") != *FASTVARDBL("B{3.4|9.4|5.4}"))
      TS_ASSERT(*FASTVARDBL("A{3.4|5.4|9.4}") != *FASTVARDBL("A{3.4|9.4}"))
      TS_ASSERT(*FASTVARDBL("A{3.4|5.4|9.4}") != *FASTVARDBL("A{3.4|9.4|5|12.4}"))
      TS_ASSERT(*FASTVARDBL("A{1.4:7.4:4}") == *FASTVARDBL("A{1.4|3.4|5.4|7.4}"))
    }

    GUM_ACTIVE_TEST(ClosestIndex) {
      TS_ASSERT_THROWS(FASTVARDBL("A{On|Off|Defun}")->closestIndex(1.5), gum::NotImplementedYet);

      TS_ASSERT_EQUALS(FASTVARDBL("A{0|1.15|2.3}")->closestIndex(-1.0), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A{0|1.15|2.3}")->closestIndex(0.0), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A{0|1.15|2.3}")->closestIndex(0.575), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A{0|1.15|2.3}")->closestIndex(1.2), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A{0|1.15|2.3}")->closestIndex(1.15), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A{0|1.15|2.3}")->closestIndex(3.0), 2)
      TS_ASSERT_EQUALS(FASTVARDBL("A{0|1.15|2.3}")->closestIndex(2.0), 2)
      TS_ASSERT_EQUALS(FASTVARDBL("A{0|1.15|2.3}")->closestIndex(2.3), 2)

      TS_ASSERT_EQUALS(FASTVARDBL("A{1|3|5}")->closestIndex(0.1), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A{1|3|5}")->closestIndex(1.0), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A{1|3|5}")->closestIndex(1.5), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A{1|3|5}")->closestIndex(2.0), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A{1|3|5}")->closestIndex(2.5), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A{1|3|5}")->closestIndex(3.1), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A{1|3|5}")->closestIndex(3.0), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A{1|3|5}")->closestIndex(3.9), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A{1|3|5}")->closestIndex(4.9), 2)
      TS_ASSERT_EQUALS(FASTVARDBL("A{1|3|5}")->closestIndex(5.0), 2)
      TS_ASSERT_EQUALS(FASTVARDBL("A{1|3|5}")->closestIndex(9.9), 2)

      TS_ASSERT_EQUALS(FASTVARDBL("A[3,5]")->closestIndex(1.5), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[3,5]")->closestIndex(3.0), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[3,5]")->closestIndex(3.2), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[3,5]")->closestIndex(3.5), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[3,5]")->closestIndex(3.8), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A[3,5]")->closestIndex(4.0), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A[3,5]")->closestIndex(4.2), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A[3,5]")->closestIndex(4.5), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A[3,5]")->closestIndex(4.7), 2)
      TS_ASSERT_EQUALS(FASTVARDBL("A[3,5]")->closestIndex(5.0), 2)
      TS_ASSERT_EQUALS(FASTVARDBL("A[3,5]")->closestIndex(9.0), 2)

      TS_ASSERT_EQUALS(FASTVARDBL("A[-5,-3]")->closestIndex(-1.5), 2)
      TS_ASSERT_EQUALS(FASTVARDBL("A[-5,-3]")->closestIndex(-3.2), 2)
      TS_ASSERT_EQUALS(FASTVARDBL("A[-5,-3]")->closestIndex(-3.5), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A[-5,-3]")->closestIndex(-3.8), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A[-5,-3]")->closestIndex(-4.0), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A[-5,-3]")->closestIndex(-4.2), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A[-5,-3]")->closestIndex(-4.5), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[-5,-3]")->closestIndex(-4.7), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[-5,-3]")->closestIndex(-5.0), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[-5,-3]")->closestIndex(-9.0), 0)

      TS_ASSERT_EQUALS(FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(0.5), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(1.0), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(1.3), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(1.5), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(1.7), 0)
      TS_ASSERT_EQUALS(FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(2.0), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(2.5), 1)
      TS_ASSERT_EQUALS(FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(3.0), 2)
      TS_ASSERT_EQUALS(FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(5.5), 4)
      TS_ASSERT_EQUALS(FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(6.0), 4)
      TS_ASSERT_EQUALS(FASTVARDBL("A[1,2,3,4,5,6]")->closestIndex(9.9), 4)
    }
  };
}   // namespace gum_tests
