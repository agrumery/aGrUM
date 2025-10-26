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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/timer.h>

namespace gum_tests {

  class GUM_TEST_SUITE(Timer) {
    public:
    GUM_ACTIVE_TEST(Constructors) {
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Timer t1)
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Timer* t2 = new gum::Timer(); delete (t2);) {
        gum::Timer t1;
        t1.reset();

        gum::Timer* t2 = new gum::Timer();
        t2->reset();

        TS_GUM_ASSERT_THROWS_NOTHING(gum::Timer t3(*t2))

        gum::Timer* t4 = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(t4 = new gum::Timer(t1))
        TS_ASSERT_DELTA(t4->step(), t1.step(), 1e-3)

        gum::Timer t3(*t2);
        TS_ASSERT_DELTA(t2->step(), t3.step(), 1e-3)

        gum::Timer t5 = t3;
        TS_ASSERT_DELTA(t5.step(), t3.step(), 1e-3)

        delete t2;
        delete t4;
      }   // namespace gum_tests
    }

    GUM_ACTIVE_TEST(Trivial) {
      bool test_pass = false;
      // The test randomly fails for reasons, it have to fails three times in
      // a row to be considered trully as failed
      for (int i = 0; (i < 3) && !test_pass; ++i) {
        gum::Timer t;
        gum::Timer tt;
        int        w = 0;

        begin_test_waiting();
        t.reset();
        double t1 = t.step();
        test_waiting(++w);
        tt.reset();
        double t5 = t.step();
        simpleSleep(1);
        test_waiting(++w);
        double t2 = t.pause();
        simpleSleep(1);
        test_waiting(++w);
        double t3 = t.resume();
        simpleSleep(1);
        test_waiting(++w);
        double t4 = t.step();
        double t6 = tt.step();
        end_test_waiting();

        // TS_ASSERT_DELTA( t6 - t5, 3.0, 1e-3 )
        test_pass = std::abs(std::abs(t6 - t5) - 3.0) <= 1e-3;
        // TS_ASSERT_DELTA( t4 - t1, 2.0, 1e-3 )
        test_pass = test_pass && std::abs(std::abs(t4 - t1) - 2.0) <= 1e-3;
        // TS_ASSERT_DELTA( t3 - t2, 0.0, 1e-3 )
        test_pass = test_pass && std::abs(std::abs(t3 - t2) - 0.0) <= 1e-3;
      }
      GUM_ASSERT(test_pass);
    }

    inline void simpleSleep(double second) {
      gum::Timer t;
      t.reset();

      while (t.step() <= second)
        ;
    }
  };
}   // namespace gum_tests
