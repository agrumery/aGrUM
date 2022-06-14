/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <agrum/tools/variables/allDiscreteVariables.h>

namespace gum_tests {

  class AllVariablesTestSuite: public CxxTest::TestSuite {
    public:
    void testCreationRange() {
      try {
        {
          auto a = gum::fastVariable< double >("A", 2);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([0,1])")
        }
        {
          // a way to create a variable with only one value
          auto a = gum::fastVariable< double >("A", 1);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([0,0])")
        }
        {
          auto a = gum::fastVariable< double >("A", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([0,3])")
        }
        {
          auto a = gum::fastVariable< double >("A[3]", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([0,2])")
        }
        {
          auto a = gum::fastVariable< double >("A[3,7]", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([3,7])")
        }
        {
          auto a = gum::fastVariable< double >("A[-7,-3]", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([-7,-3])")
        }

        TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A[7,3]", 4), const gum::InvalidArgument&)

        TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A[1,1]", 4), const gum::InvalidArgument&)

        {
          auto a = gum::fastVariable< double >("A[1,1]", 1);
          TS_ASSERT_EQUALS(a->toString(), "A:Range([1,1])")
        }
      } catch (gum::Exception const& e) GUM_SHOWERROR(e)
    }

      void testCreationLabelized() {
        try {
          {
            auto a = gum::fastVariable< double >("A{a|b|c}", 4);
            TS_ASSERT_EQUALS(a->toString(), "A:Labelized({a|b|c})")
          }
          TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A{a}", 4), const gum::InvalidArgument&)

          {
            auto a = gum::fastVariable< double >("A{a}", 1);
            TS_ASSERT_EQUALS(a->toString(), "A:Labelized({a})")
          }
        } catch (gum::Exception const& e) { GUM_SHOWERROR(e) }
      }

      void testCreationInteger() {
        {
          auto a = gum::fastVariable< double >("A{0|3|5}", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Integer({0|3|5})")
        }
        {
          auto a = gum::fastVariable< double >("A{-3|0|3}", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Integer({-3|0|3})")
        }
        {
          auto a = gum::fastVariable< double >("A{15|-3|0|3}", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Integer({-3|0|3|15})")
        }
        TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A{15}", 4), gum::InvalidArgument&)

        {
          auto a = gum::fastVariable< double >("A{15}", 1);
          TS_ASSERT_EQUALS(a->toString(), "A:Integer({15})")
        }
      }

      void testCreationDiscretized() {
        {
          auto a = gum::fastVariable< double >("A[1,2,3,4,5,6]", 4);
          TS_ASSERT_EQUALS(a->toString(), "A:Discretized(<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>)")
        }

        TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A[0.3]", 4), gum::InvalidArgument&)
        TS_ASSERT_THROWS(auto a = gum::fastVariable< double >("A[0.3]", 1), gum::InvalidArgument&)
      }
  };
}   // namespace gum_tests