/****************************************************************************
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
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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





#include <iostream>
#include <limits>
#include <string>

#include <gumtest/AgrumTestSuite.h>

namespace gum_tests {

  class [[maybe_unused]] CPlusPlusTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_CPP17_binding_array) {
      int arry[3]    = {3, 4, 5};
      auto [a, b, c] = arry;
      TS_ASSERT_EQUALS(a, 3);
      TS_ASSERT_EQUALS(b, 4);
      TS_ASSERT_EQUALS(c, 5);
    }

    GUM_ACTIVE_TEST(_CPP17_autobinding) {
      std::tuple tplex(1, 'a', 3.14);
      auto [a, b, c] = tplex;
      TS_ASSERT_EQUALS(a, 1);
      TS_ASSERT_EQUALS(b, 'a');
      TS_ASSERT_EQUALS(c, 3.14);
    }

    GUM_ACTIVE_TEST(_CPP17_enumInit) {
      enum byte : unsigned char {};

      byte b0 [[maybe_unused]]{0};
      byte b1 [[maybe_unused]] = byte{255};
    }

    GUM_ACTIVE_TEST(_CPP17_ifWithInit) {
      if (int i = 3; i % 2 == 0) { TS_ABORT(); }
    }

    GUM_ACTIVE_TEST(_CPP20_likelyAttributes) {
      bool b = false;
      if (b) [[likely]] {
        TS_ABORT();
      } else {
        TS_ASSERT(!b);
      }
    }

    GUM_ACTIVE_TEST(_CPP20_array_size_deduction) {
      int* p2 = new int[]{1, 2, 3};
      delete[] (p2);
    }

    GUM_ACTIVE_TEST(_CPP20_int_two_cplt) {
      int i1 = -1;
      TS_ASSERT_EQUALS(
          i1 <<= 1,
          -2)   // left-shift for signed negative integers(previously undefined behavior)

      int i2 = std::numeric_limits< int >::max();
      TS_ASSERT_EQUALS(
          i2 <<= 1,
          -2)   // "unrepresentable" left-shift for signed integers(previously undefined behavior)
    }

    GUM_ACTIVE_TEST(_CPP20_lambda_implicit_this_capture) {
      struct S {
        int x{1};
        int y{[&] { return x + 1; }()};   // OK, captures 'this'
      };

      S s;
      TS_ASSERT_EQUALS(s.y, 2);
    }
  };
}   // namespace gum_tests
