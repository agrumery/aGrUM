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


#include <cstdlib>   // rand
#include <iostream>
#include <string>

#include <agrum/CN/polytope/LpInterface.h>
#include <agrum/CN/polytope/LrsWrapper.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  LpInterface
#define GUM_CURRENT_MODULE CN

namespace gum_tests {

  struct LpInterfaceTestSuite {
    public:
    static void testLpExpr() {
      gum::credal::lp::LpInterface< double > pl;
      auto                                   v = pl.addCols(3);
      const gum::credal::lp::LpCol&          a = v[0];

      gum::credal::lp::LpExpr expr = 2 + a <= 5;
      gum::credal::lp::LpRow  row(expr, v);
    }

    static void testLpCreation() {
      gum::credal::lp::LpInterface< double > pl;

      auto A = pl.addCols(3);
      pl.addRow(0.1 <= A[0] <= 0.7);
      pl.addRow(0.1 <= A[1] <= 0.5);
      pl.addRow(0.1 <= A[2] <= 0.6);
    }

    static void testLpSolve() {
      gum::credal::lp::LpInterface< double > pl;
      auto                                   A = pl.addCols(2);
      pl.addRow(0.2 <= A[0] <= 0.8);
      pl.addRow(0.2 <= A[1] <= 0.8);

      auto vertices = pl.solve();

      CHECK(!vertices.empty());
      for (const auto& v: vertices)
        for (double x: v) {
          CHECK(x >= 0.0);
          CHECK(x <= 1.0);
        }
    }

    static void testLrsWrapperH2V() {
      gum::credal::LRSWrapper< double > lrs;
      lrs.setUpH(2);
      lrs.fillH(0.3, 0.7, 0);
      lrs.fillH(0.3, 0.7, 1);
      lrs.H2V();

      const auto& output = lrs.getOutput();
      CHECK_EQ(output.size(), (std::size_t)2);

      bool found_low = false, found_high = false;
      for (const auto& v: output) {
        CHECK_EQ(v.size(), (std::size_t)2);
        if (std::abs(v[0] - 0.3) < 1e-6) found_low = true;
        if (std::abs(v[0] - 0.7) < 1e-6) found_high = true;
      }
      CHECK(found_low);
      CHECK(found_high);
    }
  };   // end of class LpInterfaceTestSuite

  GUM_TEST_ACTIF(LpExpr)
  GUM_TEST_ACTIF(LpCreation)
  GUM_TEST_ACTIF(LpSolve)
  GUM_TEST_ACTIF(LrsWrapperH2V)

}   // end of namespace gum_tests
