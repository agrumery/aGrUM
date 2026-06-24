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


#include <agrum/base/core/approximations/linearApproximationPolicy.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  LinearApproximationPolicy
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct LinearApproximationPolicyTestSuite {
    using Policy = gum::LinearApproximationPolicy< double >;

    public:
    static void testConstruction() {
      Policy p(0.0, 10.0, 1.0);
      CHECK_EQ(p.lowLimit(), 0.0);
      CHECK_EQ(p.highLimit(), 10.0);
    }

    // regression tests for HIGH-4: all combine* were no-ops (computed new limits
    // into locals but never assigned them back to lowLimit_/highLimit_)
    static void testCombineAdd() {
      Policy p1(0.0, 10.0, 1.0);
      Policy p2(0.0, 5.0, 1.0);
      p1.combineAdd(&p2);
      // [0,10] + [0,5] -> low=0+0=0, high=10+5=15
      CHECK_EQ(p1.lowLimit(), doctest::Approx(0.0));
      CHECK_EQ(p1.highLimit(), doctest::Approx(15.0));
    }

    static void testCombineSub() {
      Policy p1(0.0, 10.0, 1.0);
      Policy p2(0.0, 5.0, 1.0);
      p1.combineSub(&p2);
      // [0,10] - [0,5] -> low=0-5=-5, high=10-0=10
      CHECK_EQ(p1.lowLimit(), doctest::Approx(-5.0));
      CHECK_EQ(p1.highLimit(), doctest::Approx(10.0));
    }

    static void testCombineMult() {
      Policy p1(1.0, 3.0, 0.5);
      Policy p2(2.0, 4.0, 0.5);
      p1.combineMult(&p2);
      // [1,3] * [2,4] -> low=2, high=12
      CHECK_EQ(p1.lowLimit(), doctest::Approx(2.0));
      CHECK_EQ(p1.highLimit(), doctest::Approx(12.0));
    }

    static void testCombineDiv() {
      Policy p1(2.0, 8.0, 0.5);
      Policy p2(1.0, 4.0, 0.5);
      p1.combineDiv(&p2);
      // [2,8] / [1,4] -> combos: 2/1=2, 2/4=0.5, 8/1=8, 8/4=2 -> low=0.5, high=8
      CHECK_EQ(p1.lowLimit(), doctest::Approx(0.5));
      CHECK_EQ(p1.highLimit(), doctest::Approx(8.0));
    }

    static void testCombineMax() {
      Policy p1(1.0, 5.0, 0.5);
      Policy p2(3.0, 7.0, 0.5);
      p1.combineMax(&p2);
      // max([1,5],[3,7]) -> low=max(1,3)=3, high=max(5,7)=7
      CHECK_EQ(p1.lowLimit(), doctest::Approx(3.0));
      CHECK_EQ(p1.highLimit(), doctest::Approx(7.0));
    }

    static void testCombineMin() {
      Policy p1(1.0, 5.0, 0.5);
      Policy p2(3.0, 7.0, 0.5);
      p1.combineMin(&p2);
      // min([1,5],[3,7]) -> low=min(1,3)=1, high=min(5,7)=5
      CHECK_EQ(p1.lowLimit(), doctest::Approx(1.0));
      CHECK_EQ(p1.highLimit(), doctest::Approx(5.0));
    }

    static void testEncodeDecodeRoundTrip() {
      Policy p(0.0, 10.0, 1.0);
      // decode(encode(x)) must equal fromExact(x) for all in-range x
      for (double x: {0.0, 0.5, 5.0, 9.9, 10.0}) {
        CHECK_EQ(p.decode(p.encode(x)), doctest::Approx(p.fromExact(x)));
      }
      // low and high limits are represented exactly
      CHECK_EQ(p.fromExact(0.0), doctest::Approx(0.0));
      CHECK_EQ(p.fromExact(10.0), doctest::Approx(10.0));
    }

    static void testSetLimits() {
      Policy p(0.0, 10.0, 1.0);
      p.setLimits(2.0, 8.0);
      CHECK_EQ(p.lowLimit(), doctest::Approx(2.0));
      CHECK_EQ(p.highLimit(), doctest::Approx(8.0));
      // new limits are reflected in encode/decode
      CHECK_EQ(p.fromExact(2.0), doctest::Approx(2.0));
      CHECK_EQ(p.fromExact(8.0), doctest::Approx(8.0));
      // setLimits with inverted bounds must throw
      CHECK_THROWS_AS(p.setLimits(9.0, 1.0), const gum::OutOfBounds&);
    }

    static void testSafeFromExactBounds() {
      Policy p(0.0, 10.0, 1.0);
      // values at exact bounds are accepted
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.safeFromExact(0.0));
      GUM_CHECK_ASSERT_THROWS_NOTHING(p.safeFromExact(10.0));
      // values outside bounds throw OutOfBounds
      CHECK_THROWS_AS(p.safeFromExact(-0.001), const gum::OutOfBounds&);
      CHECK_THROWS_AS(p.safeFromExact(10.001), const gum::OutOfBounds&);
    }

    static void testCombineChain() {
      Policy p1(0.0, 10.0, 1.0);
      Policy p2(0.0, 5.0, 1.0);
      // [0,10] + [0,5] -> [0,15]
      p1.combineAdd(&p2);
      CHECK_EQ(p1.lowLimit(), doctest::Approx(0.0));
      CHECK_EQ(p1.highLimit(), doctest::Approx(15.0));
      // [0,15] - [0,5] -> [-5,15]
      p1.combineSub(&p2);
      CHECK_EQ(p1.lowLimit(), doctest::Approx(-5.0));
      CHECK_EQ(p1.highLimit(), doctest::Approx(15.0));
    }

    static void testNbInterval() {
      Policy p(0.0, 10.0, 1.0);
      // nbInterval = 1 + (10-0)/1 = 11; decode at boundary works
      CHECK_EQ(p.decode(11), doctest::Approx(10.0));
      CHECK_THROWS_AS(p.decode(12), const gum::OutOfBounds&);
      // after setLimits, nbInterval is recomputed to 1 + (20-0)/1 = 21
      p.setLimits(0.0, 20.0);
      CHECK_EQ(p.decode(21), doctest::Approx(20.0));
      CHECK_THROWS_AS(p.decode(22), const gum::OutOfBounds&);
    }

    // MED-21: setEpsilon must validate epsilon > 0 (constructor does but setEpsilon did not)
    static void testSetEpsilonValidation() {
      Policy p(0.0, 10.0, 1.0);
      CHECK_THROWS_AS(p.setEpsilon(0.0), const gum::OutOfBounds&);
      CHECK_THROWS_AS(p.setEpsilon(-1.0), const gum::OutOfBounds&);
      // valid epsilon updates nbInterval correctly
      p.setEpsilon(2.0);
      CHECK_EQ(p.lowLimit(), doctest::Approx(0.0));
      CHECK_EQ(p.highLimit(), doctest::Approx(10.0));
      // nbInterval = 1 + (10-0)/2 = 6
      CHECK_EQ(p.decode(6), doctest::Approx(10.0));
    }
  };

  GUM_TEST_ACTIF(Construction)
  GUM_TEST_ACTIF(CombineAdd)
  GUM_TEST_ACTIF(CombineSub)
  GUM_TEST_ACTIF(CombineMult)
  GUM_TEST_ACTIF(CombineDiv)
  GUM_TEST_ACTIF(CombineMax)
  GUM_TEST_ACTIF(CombineMin)
  GUM_TEST_ACTIF(EncodeDecodeRoundTrip)
  GUM_TEST_ACTIF(SetLimits)
  GUM_TEST_ACTIF(SafeFromExactBounds)
  GUM_TEST_ACTIF(CombineChain)
  GUM_TEST_ACTIF(NbInterval)
  GUM_TEST_ACTIF(SetEpsilonValidation)
}   // namespace gum_tests
