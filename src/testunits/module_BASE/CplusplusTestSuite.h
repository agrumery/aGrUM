/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <iostream>
#include <limits>
#include <string>

#include <gumtest/AgrumTestSuite.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  CPlusPlus
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct CPlusPlusTestSuite {
    public:
    static void test_CPP17_binding_array() {
      int arry[3]    = {3, 4, 5};
      auto [a, b, c] = arry;
      CHECK((a) == (3));
      CHECK((b) == (4));
      CHECK((c) == (5));
    }   // namespace gum_tests

    static void test_CPP17_autobinding() {
      std::tuple tplex(1, 'a', 3.14);
      auto [a, b, c] = tplex;
      CHECK((a) == (1));
      CHECK((b) == ('a'));
      CHECK((c) == (3.14));
    }

    static void test_CPP17_enumInit() {
      enum byte : unsigned char {};

      byte b0 [[maybe_unused]]{0};
      byte b1 [[maybe_unused]] = byte{255};
    }

    static void test_CPP17_ifWithInit() {
      if (int i = 3; i % 2 == 0) { FAIL("should not reach here"); }
    }

    static void test_CPP20_likelyAttributes() {
      bool b = false;
      if (b) [[likely]] {
        FAIL("should not reach here");
      } else {
        CHECK(!b);
      }
    }

    static void test_CPP20_array_size_deduction() {
      int* p2 = new int[]{1, 2, 3};
      delete[] (p2);
    }

    static void test_CPP20_int_two_cplt() {
      int i1 = -1;
      CHECK((i1 <<= 1)
            == (-2));   // left-shift for signed negative integers(previously undefined behavior)

      int i2 = std::numeric_limits< int >::max();
      CHECK((i2 <<= 1) == (-2));   // "unrepresentable" left-shift for signed integers(previously
                                   // undefined behavior)
    }

    static void test_CPP20_lambda_implicit_this_capture() {
      struct S {
        int x{1};
        int y{[&] { return x + 1; }()};   // OK, captures 'this'
      };

      S s;
      CHECK((s.y) == (2));
    }
  };

  GUM_TEST_ACTIF(_CPP17_binding_array)
  GUM_TEST_ACTIF(_CPP17_autobinding)
  GUM_TEST_ACTIF(_CPP17_enumInit)
  GUM_TEST_ACTIF(_CPP17_ifWithInit)
  GUM_TEST_ACTIF(_CPP20_likelyAttributes)
  GUM_TEST_ACTIF(_CPP20_array_size_deduction)
  GUM_TEST_ACTIF(_CPP20_int_two_cplt)
  GUM_TEST_ACTIF(_CPP20_lambda_implicit_this_capture)
}   // namespace gum_tests
