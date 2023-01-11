/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <string>
#include <limits>

#include <gumtest/AgrumTestSuite.h>

namespace gum_tests {

  class [[maybe_unused]] CPlusPlusTestSuite: public CxxTest::TestSuite {
    public:
    GUM_TEST(_CPP17_binding_array) {
      int arry[3]    = {3, 4, 5};
      auto [a, b, c] = arry;
      TS_ASSERT_EQUALS(a, 3);
      TS_ASSERT_EQUALS(b, 4);
      TS_ASSERT_EQUALS(c, 5);
    }

    GUM_TEST(_CPP17_autobinding) {
      std::tuple tplex(1, 'a', 3.14);
      auto [a, b, c] = tplex;
      TS_ASSERT_EQUALS(a, 1);
      TS_ASSERT_EQUALS(b, 'a');
      TS_ASSERT_EQUALS(c, 3.14);
    }

    GUM_TEST(_CPP17_enumInit) {
      enum byte : unsigned char {
      };
      byte b0 [[maybe_unused]]{0};
      byte b1 [[maybe_unused]] = byte{255};
    }

    GUM_TEST(_CPP17_ifWithInit) {
      if (int i = 3; i % 2 == 0) { TS_ABORT(); }
    }

    GUM_TEST(_CPP20_likelyAttributes) {
      bool b = false;
      if (b) [[likely]] {
        TS_ABORT();
      } else {
        TS_ASSERT(!b);
      }
    }

    GUM_TEST(_CPP20_array_size_deduction) {
      int* p2 = new int[]{1, 2, 3};
      delete[](p2);
    }

    GUM_TEST(_CPP20_int_two_cplt) {
      int i1 = -1;
      TS_ASSERT_EQUALS(
         i1 <<= 1,
         -2)   // left-shift for signed negative integers(previously undefined behavior)

      int i2 = std::numeric_limits< int >::max();
      TS_ASSERT_EQUALS(
         i2 <<= 1,
         -2)   // "unrepresentable" left-shift for signed integers(previously undefined behavior)
    }

    GUM_TEST(_CPP20_lambda_implicit_this_capture) {
      struct S {
        int x{1};
        int y{[&] { return x + 1; }()};   // OK, captures 'this'
      };
      S s;
      TS_ASSERT_EQUALS(s.y, 2);
    }
  };
}   // namespace gum_tests
