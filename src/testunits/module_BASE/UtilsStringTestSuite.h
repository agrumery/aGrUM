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

#include <agrum/base/core/utils_string.h>

namespace gum_tests {

  class GUM_TEST_SUITE(UtilsString) {
    public:
    GUM_ACTIVE_TEST(IsInteger) {
      TS_ASSERT(gum::isInteger("12"))
      TS_ASSERT(!gum::isInteger("foo"))
      TS_ASSERT(!gum::isInteger("12foo"))
      TS_ASSERT(!gum::isInteger("foo12"))
      TS_ASSERT(!gum::isInteger("foo12bar"))

      TS_ASSERT(gum::isInteger("+12"))
      TS_ASSERT(!gum::isInteger("1+2"))

      TS_ASSERT(gum::isInteger("-12"))
      TS_ASSERT(!gum::isInteger("1-2"))
    }

    GUM_ACTIVE_TEST(isNumerical) {
      TS_ASSERT(gum::isNumerical("12"))
      TS_ASSERT(!gum::isNumerical("foo"))
      TS_ASSERT(!gum::isNumerical("12foo"))
      TS_ASSERT(!gum::isNumerical("foo12"))
      TS_ASSERT(!gum::isNumerical("foo12bar"))

      TS_ASSERT(gum::isNumerical("12.5"))
      TS_ASSERT(!gum::isNumerical("12.5foo"))
      TS_ASSERT(!gum::isNumerical("foo12.5"))
      TS_ASSERT(!gum::isNumerical("foo12.5bar"))

      TS_ASSERT(gum::isNumerical("+12"))
      TS_ASSERT(!gum::isNumerical("1+2"))

      TS_ASSERT(gum::isNumerical("-12"))
      TS_ASSERT(!gum::isNumerical("1-2"))

      TS_ASSERT(gum::isNumerical("-1.2"))
      TS_ASSERT(gum::isNumerical("1.2"))
      TS_ASSERT(gum::isNumerical("+1.2"))
      TS_ASSERT(gum::isNumerical("1.2e-8"))
    }

    GUM_ACTIVE_TEST(IsIntegerWithResult) {
      int res;
      TS_ASSERT(gum::isIntegerWithResult("12", &res))
      TS_ASSERT_EQUALS(res, 12)
      TS_ASSERT(!gum::isIntegerWithResult("foo", &res))
      TS_ASSERT(!gum::isIntegerWithResult("12foo", &res))
      TS_ASSERT(!gum::isIntegerWithResult("foo12", &res))
      TS_ASSERT(!gum::isIntegerWithResult("foo12bar", &res))

      TS_ASSERT(gum::isIntegerWithResult("+12", &res))
      TS_ASSERT_EQUALS(res, 12)
      TS_ASSERT(!gum::isIntegerWithResult("1+2", &res))

      TS_ASSERT(gum::isIntegerWithResult("-12", &res))
      TS_ASSERT_EQUALS(res, -12)
      TS_ASSERT(!gum::isIntegerWithResult("1-2", &res))
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(isNumericalWithResWithResult) {
      double res;
      TS_ASSERT(gum::isNumericalWithResult("12", &res))
      TS_ASSERT_EQUALS(res, 12.0)
      TS_ASSERT(!gum::isNumericalWithResult("foo", &res))
      TS_ASSERT(!gum::isNumericalWithResult("12foo", &res))
      TS_ASSERT(!gum::isNumericalWithResult("foo12", &res))
      TS_ASSERT(!gum::isNumericalWithResult("foo12bar", &res))

      TS_ASSERT(gum::isNumericalWithResult("12.5", &res))
      TS_ASSERT_EQUALS(res, 12.5)
      TS_ASSERT(!gum::isNumericalWithResult("12.5foo", &res))
      TS_ASSERT(!gum::isNumericalWithResult("foo12.5", &res))
      TS_ASSERT(!gum::isNumericalWithResult("foo12.5bar", &res))

      TS_ASSERT(gum::isNumericalWithResult("+12", &res))
      TS_ASSERT_EQUALS(res, 12.0)
      TS_ASSERT(!gum::isNumericalWithResult("1+2", &res))

      TS_ASSERT(gum::isNumericalWithResult("-12", &res))
      TS_ASSERT_EQUALS(res, -12.0)
      TS_ASSERT(!gum::isNumericalWithResult("1-2", &res))

      TS_ASSERT(gum::isNumericalWithResult("-1.2", &res))
      TS_ASSERT(gum::isNumericalWithResult("1.2", &res))
      TS_ASSERT_EQUALS(res, 1.2)
      TS_ASSERT(gum::isNumericalWithResult("+1.2", &res))
      TS_ASSERT_EQUALS(res, 1.2)
      TS_ASSERT(gum::isNumericalWithResult("1.2e-8", &res))
      TS_ASSERT_EQUALS(res, 1.2e-8)
    }
  };
}   // namespace gum_tests
