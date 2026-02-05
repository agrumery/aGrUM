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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/utils_string.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  UtilsString
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct UtilsStringTestSuite {
    public:
    static void testIsInteger() {
      CHECK(gum::isInteger("12"));
      CHECK(!gum::isInteger("foo"));
      CHECK(!gum::isInteger("12foo"));
      CHECK(!gum::isInteger("foo12"));
      CHECK(!gum::isInteger("foo12bar"));

      CHECK(gum::isInteger("+12"));
      CHECK(!gum::isInteger("1+2"));

      CHECK(gum::isInteger("-12"));
      CHECK(!gum::isInteger("1-2"));
    }

    static void testisNumerical() {
      CHECK(gum::isNumerical("12"));
      CHECK(!gum::isNumerical("foo"));
      CHECK(!gum::isNumerical("12foo"));
      CHECK(!gum::isNumerical("foo12"));
      CHECK(!gum::isNumerical("foo12bar"));

      CHECK(gum::isNumerical("12.5"));
      CHECK(!gum::isNumerical("12.5foo"));
      CHECK(!gum::isNumerical("foo12.5"));
      CHECK(!gum::isNumerical("foo12.5bar"));

      CHECK(gum::isNumerical("+12"));
      CHECK(!gum::isNumerical("1+2"));

      CHECK(gum::isNumerical("-12"));
      CHECK(!gum::isNumerical("1-2"));

      CHECK(gum::isNumerical("-1.2"));
      CHECK(gum::isNumerical("1.2"));
      CHECK(gum::isNumerical("+1.2"));
      CHECK(gum::isNumerical("1.2e-8"));
    }

    static void testIsIntegerWithResult() {
      int res;
      CHECK(gum::isIntegerWithResult("12", &res));
      CHECK((res) == (12));
      CHECK(!gum::isIntegerWithResult("foo", &res));
      CHECK(!gum::isIntegerWithResult("12foo", &res));
      CHECK(!gum::isIntegerWithResult("foo12", &res));
      CHECK(!gum::isIntegerWithResult("foo12bar", &res));

      CHECK(gum::isIntegerWithResult("+12", &res));
      CHECK((res) == (12));
      CHECK(!gum::isIntegerWithResult("1+2", &res));

      CHECK(gum::isIntegerWithResult("-12", &res));
      CHECK((res) == (-12));
      CHECK(!gum::isIntegerWithResult("1-2", &res));
    }   // namespace gum_tests

    static void testisNumericalWithResWithResult() {
      double res;
      CHECK(gum::isNumericalWithResult("12", &res));
      CHECK((res) == (12.0));
      CHECK(!gum::isNumericalWithResult("foo", &res));
      CHECK(!gum::isNumericalWithResult("12foo", &res));
      CHECK(!gum::isNumericalWithResult("foo12", &res));
      CHECK(!gum::isNumericalWithResult("foo12bar", &res));

      CHECK(gum::isNumericalWithResult("12.5", &res));
      CHECK((res) == (12.5));
      CHECK(!gum::isNumericalWithResult("12.5foo", &res));
      CHECK(!gum::isNumericalWithResult("foo12.5", &res));
      CHECK(!gum::isNumericalWithResult("foo12.5bar", &res));

      CHECK(gum::isNumericalWithResult("+12", &res));
      CHECK((res) == (12.0));
      CHECK(!gum::isNumericalWithResult("1+2", &res));

      CHECK(gum::isNumericalWithResult("-12", &res));
      CHECK((res) == (-12.0));
      CHECK(!gum::isNumericalWithResult("1-2", &res));

      CHECK(gum::isNumericalWithResult("-1.2", &res));
      CHECK(gum::isNumericalWithResult("1.2", &res));
      CHECK((res) == (1.2));
      CHECK(gum::isNumericalWithResult("+1.2", &res));
      CHECK((res) == (1.2));
      CHECK(gum::isNumericalWithResult("1.2e-8", &res));
      CHECK((res) == (1.2e-8));
    }
  };

  GUM_TEST_ACTIF(IsInteger)
  GUM_TEST_ACTIF(isNumerical)
  GUM_TEST_ACTIF(IsIntegerWithResult)
  GUM_TEST_ACTIF(isNumericalWithResWithResult)
}   // namespace gum_tests
