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
          auto a = gum::fastVariable< double >("A", 2);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([0,1])");
        }
        {
          // a way to create a variable with only one value
          auto a = gum::fastVariable< double >("A", 1);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([0,0])");
        }
        {
          auto a = gum::fastVariable< double >("A", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([0,3])");
        }
        {
          auto a = gum::fastVariable< double >("A[3]", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([0,2])");
        }
        {
          auto a = gum::fastVariable< double >("A[3,7]", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([3,7])");
        }
        {
          auto a = gum::fastVariable< double >("A[-7,-3]", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([-7,-3])");
        }

        TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A[7,3]", 4),
                         const gum::InvalidArgument&);

        TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A[-5]", 4),
                         const gum::InvalidArgument&);

        TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A[1,1]", 4),
                         const gum::InvalidArgument&);

        {
          auto a = gum::fastVariable< double >("A[1,1]", 1);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([1,1])");
        }
      } catch (gum::Exception const& e) GUM_SHOWERROR(e);
    }

    GUM_ACTIVE_TEST(CreationLabelized) {
      try {
        {
          auto a = gum::fastVariable< double >("A{a|b|c}", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Labelized({a|b|c})");
        }
        TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A{a}", 4),
                         const gum::InvalidArgument&);

        {
          auto a = gum::fastVariable< double >("A{a}", 1);
          TS_ASSERT_EQUALS(a->toString(), "A:Labelized({a})");
        }
      } catch (gum::Exception const& e) { GUM_SHOWERROR(e); }
    }

    GUM_ACTIVE_TEST(CreationInteger) {
      {
        auto a = gum::fastVariable< double >("A{0|3|5}", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Integer({0|3|5})");
      }
      {
        auto a = gum::fastVariable< double >("A{-3|0|3}", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Integer({-3|0|3})");
      }
      {
        auto a = gum::fastVariable< double >("A{15|-3|0|3}", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Integer({-3|0|3|15})");
      }
      TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A{15}", 4), gum::InvalidArgument&);

      {
        auto a = gum::fastVariable< double >("A{15}", 1);
        TS_ASSERT_EQUALS(a->toString(), "A:Integer({15})");
      }
      {
        auto a = gum::fastVariable< double >("A{15|14|12|13|11}");
        TS_ASSERT_EQUALS(a->toString(), "A:Range([11,15])");
      }
    }

    GUM_ACTIVE_TEST(CreationDiscretized) {
      {
        auto a = gum::fastVariable< double >("A[1,2,3,4,5,6]", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)");
      }

      TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A[0.3]", 4), gum::InvalidArgument&);
      TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A[0.3]", 1), gum::InvalidArgument&);
    }

    GUM_ACTIVE_TEST(CreationDiscretized2) {
      {
        auto a = gum::fastVariable< double >("A[1:6:5]", 4);
        TS_ASSERT_EQUALS(a->toString(), "A:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)");
      }
      {
        auto a = gum::fastVariable< double >("A[1:6:2]", 4);
        GUM_TRACE(a->toString());
        TS_ASSERT_EQUALS(a->toString(), "A:Discretized(<[1;3.5[,[3.5;6]>)");
      }

      TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A[1:6:1]", 4), gum::InvalidArgument&);
    }

    GUM_ACTIVE_TEST(CreationNumerical) {
      try {
        {
          auto a = gum::fastVariable< double >("A{1|1.5|6|2.9|3.14}", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:NumericalDiscrete({1|1.5|2.9|3.14|6})");
        }
        TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A{3.14}", 4),
                         const gum::InvalidArgument&);
        {
          auto a = gum::fastVariable< double >("A{3.14}", 1);
          TS_ASSERT_EQUALS(a->toString(), "A:NumericalDiscrete({3.14})");
        }
        {
          auto a = gum::fastVariable< double >("A{1.2:5.2:5}", 5);
          TS_ASSERT_EQUALS(a->toString(), "A:NumericalDiscrete({1.2|2.2|3.2|4.2|5.2})");
        }
        {
          auto a = gum::fastVariable< double >("A{1.2:5.2:2}", 2);
          TS_ASSERT_EQUALS(a->toString(), "A:NumericalDiscrete({1.2|5.2})");
          auto b = gum::fastVariable< double >("A{1.2|5.2}", 2);
          TS_ASSERT_EQUALS(b->toString(), "A:NumericalDiscrete({1.2|5.2})");
        }
        {
          auto a = gum::fastVariable< double >("A{1.2:5.2:2.8}", 2);
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
      GUM_TRACE_VAR(s);
      GUM_TRACE_VAR(gum::fastVariable< double >(s)->stype());
      TS_ASSERT_EQUALS(s, (gum::fastVariable< double >(s)->toFast()));   // labelized
      s = "A{0|1.15|2.3}";
      GUM_TRACE_VAR(s);
      GUM_TRACE_VAR(gum::fastVariable< double >(s)->stype());
      TS_ASSERT_EQUALS(s, (gum::fastVariable< double >(s)->toFast()));   // NumericalDiscrete
      TS_ASSERT_EQUALS(s, (gum::fastVariable< double >("A{0:2.3:3}")->toFast()));   // NumericalDiscrete
      s = "A{1|3|5}";
      GUM_TRACE_VAR(s);
      GUM_TRACE_VAR(gum::fastVariable< double >(s)->stype());
      TS_ASSERT_EQUALS(s, (gum::fastVariable< double >(s)->toFast()));   // Integer
      TS_ASSERT_EQUALS(s, (gum::fastVariable< double >("A{1:5:3}")->toFast()));   // Integer
      s = "A[3,5]";
      GUM_TRACE_VAR(s);
      GUM_TRACE_VAR(gum::fastVariable< double >(s)->stype());
      TS_ASSERT_EQUALS(s, (gum::fastVariable< double >(s)->toFast()));   // Range
      s = "A[5]";
      GUM_TRACE_VAR(s);
      GUM_TRACE_VAR(gum::fastVariable< double >(s)->stype());
      TS_ASSERT_EQUALS(s, (gum::fastVariable< double >(s)->toFast()));   // Range
      s = "A[1,2,3,4,5,6]";
      GUM_TRACE_VAR(s);
      GUM_TRACE_VAR(gum::fastVariable< double >(s)->stype());
      TS_ASSERT_EQUALS(s, (gum::fastVariable< double >(s)->toFast()));   // Discretized
      TS_ASSERT_EQUALS(s, (gum::fastVariable< double >("A[1:6:5]")->toFast()));   // Discretized
    }

    GUM_ACTIVE_TEST(FastNumericalWithoutInfinity) {
      std::string s;
      s = "A{0|1.15|inf}";
      TS_ASSERT_THROWS(gum::fastVariable< double >(s),
                       const gum::DefaultInLabel&);   // NumericalDiscrete
      s = "A{1|3|inf}";
      TS_ASSERT_THROWS(gum::fastVariable< double >(s), const gum::DefaultInLabel&);   // Integer
      s = "A[3,2.5,inf]";
      TS_ASSERT_THROWS(gum::fastVariable< double >(s), const gum::DefaultInLabel&);   // Discretized
      s = "A[inf]";
      TS_ASSERT_THROWS(gum::fastVariable< double >(s), const std::invalid_argument&);   // Range
      s = "A[2,inf]";
      TS_ASSERT_THROWS(gum::fastVariable< double >(s), const std::invalid_argument&);   // Range
      s = "A[inf,2,3,4,5,6]";
      TS_ASSERT_THROWS(gum::fastVariable< double >(s), const gum::DefaultInLabel&);     // Integer
    }

    GUM_ACTIVE_TEST(Comparison) {
      TS_ASSERT(*gum::fastVariable< double >("A{On|Off|Defun}")
                == *gum::fastVariable< double >("A{On|Off|Defun}"));
      TS_ASSERT(*gum::fastVariable< double >("A{On|Off|Defun}")
                != *gum::fastVariable< double >("B{On|Off|Defun}"));
      TS_ASSERT(*gum::fastVariable< double >("A{On|Off|Defun}")
                != *gum::fastVariable< double >("A{Off|On|Defun}"));
      TS_ASSERT(*gum::fastVariable< double >("A{On|Off|Defun}")
                != *gum::fastVariable< double >("A{On|Off|beurk}"));
    }

    GUM_ACTIVE_TEST(ClosestIndex) {

    }
  };
}   // namespace gum_tests
