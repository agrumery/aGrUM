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


#ifndef AGRUM_TEST_SUITE_H
#define AGRUM_TEST_SUITE_H

#include <gumtest/utils.h>

#include <agrum/agrum.h>

#include <agrum/base/core/debug.h>

#include <cxxtest/TestSuite.h>

#define GUM_ACTIVE_TEST(nom)   [[maybe_unused]] void test##nom()
#define GUM_INACTIVE_TEST(nom) [[maybe_unused]] void inactive_test##nom()

#define TS_GUM_ASSERT_THROWS_NOTHING(block)       \
  TS_ASSERT_THROWS_NOTHING(                       \
      try { block; } catch (gum::Exception & e) { \
        GUM_TRACE_NEWLINE;                        \
        GUM_SHOWERROR(e);                         \
        TS_FAIL("gum::Exception thrown");         \
      })

#define TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(p1, p2, delta) \
  TS_ASSERT_LESS_THAN(((p1) - (p2)).abs().max(), delta)
#define TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(p1, p2) \
  TS_ASSERT_LESS_THAN(((p1) - (p2)).abs().max(), TS_GUM_SMALL_ERROR)
#define TS_GUM_TENSOR_ALMOST_EQUALS_DELTA(p1, p2, delta) \
  TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA((p1), (gum::Tensor(p1).fillWith(p2)), delta)
#define TS_GUM_TENSOR_ALMOST_EQUALS(p1, p2) \
  TS_GUM_TENSOR_ALMOST_EQUALS_DELTA(p1, p2, TS_GUM_SMALL_ERROR)
#define TS_GUM_TENSOR_QUASI_EQUALS(p1, p2) \
  TS_GUM_TENSOR_ALMOST_EQUALS_DELTA(p1, p2, TS_GUM_VERY_SMALL_ERROR)

#define TS_GUM_TENSOR_DIFFERS_SAMEVARS(p1, p2) \
  TS_ASSERT(((p1) - (p2)).abs().max() > TS_GUM_VERY_SMALL_ERROR)
#define TS_GUM_TENSOR_DIFFERS(p1, p2) \
  TS_GUM_TENSOR_DIFFERS_SAMEVARS((p1), (gum::Tensor(p1).fillWith(p2)))


#define TS_GUM_TENSOR_SHOW_DELTA(p1, p2, delta)           \
  {                                                       \
    GUM_TRACE_NEWLINE GUM_TRACE_VAR(p1)                   \
    GUM_TRACE_VAR(p2)                                     \
    TS_ASSERT_LESS_THAN(((p1) - (p2)).abs().max(), delta) \
  }

#define TS_GUM_SMALL_ERROR      (1e-5)
#define TS_GUM_VERY_SMALL_ERROR (1e-10)

// ASSERT_{EQUALS|DIFFERS} without copy
#define TS_GUM_ASSERT_EQUALS(x, y)  TS_ASSERT((x) == (y))
#define TS_GUM_ASSERT_DIFFERS(x, y) TS_ASSERT((x) != (y))

#define TS_GUM_ASSERT_ALMOST_EQUALS(x, y) TS_ASSERT_LESS_THAN(fabs((x) - (y)), TS_GUM_SMALL_ERROR)
#define TS_GUM_ASSERT_QUASI_EQUALS(x, y) \
  TS_ASSERT_LESS_THAN(fabs((x) - (y)), TS_GUM_VERY_SMALL_ERROR)
#endif   // AGRUM_TEST_SUITE_H
