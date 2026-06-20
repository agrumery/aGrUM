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


#include <agrum/base/core/math/chi2.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Chi2
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct Chi2TestSuite {
    public:
    // probaChi2: boundary conditions guaranteed by the implementation
    static void test_proba_boundary() {
      // x <= 0 always returns 1.0
      CHECK(gum::Chi2::probaChi2(0.0, 1) == doctest::Approx(1.0).epsilon(1e-9));
      CHECK(gum::Chi2::probaChi2(-1.0, 2) == doctest::Approx(1.0).epsilon(1e-9));
      // df < 1 always returns 1.0
      CHECK(gum::Chi2::probaChi2(5.0, 0) == doctest::Approx(1.0).epsilon(1e-9));
    }

    // probaChi2: reference values from standard chi2 tables
    // P(X² > x | df) at standard significance levels
    static void test_proba_known_values() {
      // alpha = 0.05 critical values (df = 1 to 4)
      CHECK(gum::Chi2::probaChi2(3.8415, 1) == doctest::Approx(0.05).epsilon(1e-3));
      CHECK(gum::Chi2::probaChi2(5.9915, 2) == doctest::Approx(0.05).epsilon(1e-3));
      CHECK(gum::Chi2::probaChi2(7.8147, 3) == doctest::Approx(0.05).epsilon(1e-3));
      CHECK(gum::Chi2::probaChi2(9.4877, 4) == doctest::Approx(0.05).epsilon(1e-3));

      // alpha = 0.01
      CHECK(gum::Chi2::probaChi2(6.6349, 1) == doctest::Approx(0.01).epsilon(1e-3));
      CHECK(gum::Chi2::probaChi2(9.2104, 2) == doctest::Approx(0.01).epsilon(1e-3));

      // alpha = 0.001
      CHECK(gum::Chi2::probaChi2(10.8276, 1) == doctest::Approx(0.001).epsilon(1e-3));

      // large x -> p-value near 0 (below any standard threshold)
      CHECK(gum::Chi2::probaChi2(100.0, 1) < 1e-10);

      // small positive x -> p-value near 1
      CHECK(gum::Chi2::probaChi2(0.001, 1) > 0.97);
    }

    // criticalValue: boundary conditions guaranteed by the implementation
    static void test_critical_value_boundary() {
      // proba <= 0 returns GUM_CHI_MAX = 99999.0
      CHECK(gum::Chi2::criticalValue(0.0, 1) == doctest::Approx(99999.0).epsilon(1e-9));
      // proba >= 1 returns 0.0
      CHECK(gum::Chi2::criticalValue(1.0, 1) == doctest::Approx(0.0).epsilon(1e-9));
      CHECK(gum::Chi2::criticalValue(1.0, 4) == doctest::Approx(0.0).epsilon(1e-9));
    }

    // criticalValue: reference values from standard chi2 tables
    static void test_critical_value_known() {
      // alpha = 0.05
      CHECK(gum::Chi2::criticalValue(0.05, 1) == doctest::Approx(3.8415).epsilon(1e-3));
      CHECK(gum::Chi2::criticalValue(0.05, 2) == doctest::Approx(5.9915).epsilon(1e-3));
      CHECK(gum::Chi2::criticalValue(0.05, 3) == doctest::Approx(7.8147).epsilon(1e-3));
      CHECK(gum::Chi2::criticalValue(0.05, 4) == doctest::Approx(9.4877).epsilon(1e-3));

      // alpha = 0.01
      CHECK(gum::Chi2::criticalValue(0.01, 1) == doctest::Approx(6.6349).epsilon(1e-3));
      CHECK(gum::Chi2::criticalValue(0.01, 2) == doctest::Approx(9.2104).epsilon(1e-3));
    }

    // round-trip: criticalValue and probaChi2 are consistent inverses
    static void test_roundtrip() {
      // probaChi2(criticalValue(alpha, df), df) == alpha
      for (gum::Size df = 1; df <= 6; ++df) {
        CHECK(gum::Chi2::probaChi2(gum::Chi2::criticalValue(0.05, df), df)
              == doctest::Approx(0.05).epsilon(1e-4));
        CHECK(gum::Chi2::probaChi2(gum::Chi2::criticalValue(0.01, df), df)
              == doctest::Approx(0.01).epsilon(1e-4));
      }

      // criticalValue(probaChi2(x, df), df) == x
      for (gum::Size df = 1; df <= 4; ++df) {
        CHECK(gum::Chi2::criticalValue(gum::Chi2::probaChi2(5.0, df), df)
              == doctest::Approx(5.0).epsilon(1e-3));
        CHECK(gum::Chi2::criticalValue(gum::Chi2::probaChi2(1.0, df), df)
              == doctest::Approx(1.0).epsilon(1e-3));
      }
    }
  };

  GUM_TEST_ACTIF(_proba_boundary)
  GUM_TEST_ACTIF(_proba_known_values)
  GUM_TEST_ACTIF(_critical_value_boundary)
  GUM_TEST_ACTIF(_critical_value_known)
  GUM_TEST_ACTIF(_roundtrip)

} /* namespace gum_tests */
