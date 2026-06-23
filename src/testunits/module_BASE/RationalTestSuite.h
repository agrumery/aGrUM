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


#include <cmath>
#include <limits>

#include <agrum/agrum.h>

#include <agrum/base/core/math/rational.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Rational
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct RationalTestSuite {
    public:
    static void testFareyBasic() {
      int64_t num = 0, den = 0;

      // 0.5 -> 1/2
      gum::Rational< double >::farey(num, den, 0.5, 100LL, 1e-6);
      CHECK_EQ(num, int64_t(1));
      CHECK_EQ(den, int64_t(2));

      // 0.333... -> 1/3
      gum::Rational< double >::farey(num, den, 1.0 / 3.0, 100LL, 1e-6);
      CHECK_EQ(num, int64_t(1));
      CHECK_EQ(den, int64_t(3));

      // negative
      gum::Rational< double >::farey(num, den, -0.25, 100LL, 1e-6);
      CHECK_EQ(num, int64_t(-1));
      CHECK_EQ(den, int64_t(4));

      // zero
      gum::Rational< double >::farey(num, den, 0.0, 100LL, 1e-10);
      CHECK_EQ(num, int64_t(0));
      CHECK_EQ(den, int64_t(1));
    }

    // MED-7: farey must not invoke UB (int64_t overflow) with very large den_max.
    // Before the fix, a+c or b+d could overflow int64_t silently.
    static void testFareyLargeDenMaxNoOverflow() {
      int64_t num = 0, den = 0;
      // Use den_max close to INT64_MAX/2 to trigger the overflow guard.
      const int64_t big = std::numeric_limits< int64_t >::max() / 2;

      // sqrt(2)-1 is irrational; with large den_max the Farey sequence will
      // iterate many times and previously could overflow before the guard.
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gum::Rational< double >::farey(num, den, std::sqrt(2.0) - 1.0, big, 1e-9));
      // result must be a valid (non-degenerate) fraction
      CHECK(den > int64_t(0));
      CHECK(std::abs(static_cast< double >(num) / static_cast< double >(den)
                     - (std::sqrt(2.0) - 1.0))
            < 1e-6);
    }
  };

  GUM_TEST_ACTIF(FareyBasic)
  GUM_TEST_ACTIF(FareyLargeDenMaxNoOverflow)

}   // namespace gum_tests
