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

#ifndef AGRUM_TEST_SUITE_H
#  define AGRUM_TEST_SUITE_H

#  include <cmath>

#  include <gumtest/utils.h>

#  include <agrum/agrum.h>

#  include <agrum/base/core/debug.h>

#  include <doctest/doctest.h>

// ============================================================================
// Error tolerance constants
// ============================================================================
#  define GUM_SMALL_ERROR      (1e-5)
#  define GUM_VERY_SMALL_ERROR (1e-10)

// ============================================================================
// aGrUM-specific assertion macro for exception handling with error display
// ============================================================================
#  define GUM_CHECK_ASSERT_THROWS_NOTHING(block)    \
    CHECK_NOTHROW(                                  \
        try { block; } catch (gum::Exception & e) { \
          GUM_TRACE_NEWLINE;                        \
          GUM_SHOWERROR(e);                         \
          FAIL("gum::Exception thrown");            \
        })

// ============================================================================
// Tensor comparison macros (aGrUM-specific)
// ============================================================================
#  define GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA(p1, p2, delta) \
    CHECK(((p1) - (p2)).abs().max() < delta)
#  define GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(p1, p2) \
    CHECK(((p1) - (p2)).abs().max() < GUM_SMALL_ERROR)
#  define GUM_CHECK_TENSOR_ALMOST_EQUALS_DELTA(p1, p2, delta) \
    GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS_DELTA((p1), (gum::Tensor(p1).fillWith(p2)), delta)
#  define GUM_CHECK_TENSOR_ALMOST_EQUALS(p1, p2) \
    GUM_CHECK_TENSOR_ALMOST_EQUALS_DELTA(p1, p2, GUM_SMALL_ERROR)
#  define GUM_CHECK_TENSOR_QUASI_EQUALS(p1, p2) \
    GUM_CHECK_TENSOR_ALMOST_EQUALS_DELTA(p1, p2, GUM_VERY_SMALL_ERROR)

#  define GUM_CHECK_TENSOR_DIFFERS_SAMEVARS(p1, p2) \
    CHECK(((p1) - (p2)).abs().max() > GUM_VERY_SMALL_ERROR)
#  define GUM_CHECK_TENSOR_DIFFERS(p1, p2) \
    GUM_CHECK_TENSOR_DIFFERS_SAMEVARS((p1), (gum::Tensor(p1).fillWith(p2)))

#  define GUM_CHECK_TENSOR_SHOW_DELTA(p1, p2, delta) \
    {                                                \
      GUM_TRACE_NEWLINE GUM_TRACE_VAR(p1)            \
      GUM_TRACE_VAR(p2)                              \
      CHECK(((p1) - (p2)).abs().max() < delta);      \
    }

// ============================================================================
// Test registration macros to reduce boilerplate
// ============================================================================
//
// Usage: Define GUM_CURRENT_SUITE and GUM_CURRENT_MODULE at the top of your test file,
// then use GUM_TEST_ACTIF(TestName) for each test.
//
// Example (in BayesNetTestSuite.h):
//   #define GUM_CURRENT_SUITE BayesNet
//   #define GUM_CURRENT_MODULE BN
//   ...
//   GUM_TEST_ACTIF(Constructor)
//   GUM_TEST_ACTIF(CopyConstructor)
//
// GUM_TEST_ACTIF(Constructor) expands to:
//   TEST_CASE_FIXTURE(BayesNetTestSuite, "[BN][BayesNet] Constructor") { testConstructor(); }
//
// Use GUM_TEST_INACTIF(TestName) to temporarily disable a test (generates a compilation warning)
//
#  define GUM_TEST_IMPL_(Suite, Module, TestName)                                 \
    TEST_CASE_FIXTURE(Suite##TestSuite, "[" #Module "][" #Suite "] " #TestName) { \
      test##TestName();                                                           \
    }
#  define GUM_TEST_IMPL(Suite, Module, TestName) GUM_TEST_IMPL_(Suite, Module, TestName)
#  define GUM_TEST_ACTIF(TestName)               GUM_TEST_IMPL(GUM_CURRENT_SUITE, GUM_CURRENT_MODULE, TestName)
// Helper macros for generating warnings with file and line information
#  define GUM_STRINGIFY_(x) #x
#  define GUM_STRINGIFY(x)  GUM_STRINGIFY_(x)
#  define GUM_WARNING_AT(msg)                                                           \
    _Pragma(GUM_STRINGIFY(message(__FILE__ ":" GUM_STRINGIFY(__LINE__) ": INACTIVATED " \
                                                                       "TEST: " msg)))

#  define GUM_TEST_INACTIF(TestName) GUM_WARNING_AT(#TestName)

#endif   // AGRUM_TEST_SUITE_H
