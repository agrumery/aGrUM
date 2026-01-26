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
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/rangeVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  RangeVariable
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct RangeVariableTestSuite {
    public:
    static void testCopy() {
      gum::RangeVariable var1("var1", "this is var1");
      gum::RangeVariable var2("var2", "this is var2", 1, 4);

      gum::RangeVariable var3(var1);
      gum::RangeVariable var4("var4", "this is var4");
      GUM_CHECK_ASSERT_THROWS_NOTHING(var4 = var2);

      CHECK((var4.minVal()) == (var2.minVal()));
      CHECK((var1.maxVal()) == (var3.maxVal()));
      CHECK((var4.minVal()) != (var1.minVal()));
    }   // namespace gum_tests

    static void testLabels() {
      gum::RangeVariable var1("var1", "this is var1");
      CHECK((var1.domainSize()) == (static_cast< gum::Size >(2)));
      CHECK(!var1.empty());

      var1.setMinVal(1);
      var1.setMaxVal(0);
      CHECK(var1.empty());

      var1.setMaxVal(9);
      CHECK(!var1.empty());
      CHECK((var1.domainSize()) == (static_cast< gum::Size >(9)));
      CHECK(var1.belongs(3L));
      CHECK(!var1.belongs(0L));
      CHECK(!var1.belongs(10L));

      var1.setMinVal(3);

      CHECK((var1.label(1)) == ("4"));
      CHECK((var1["4"]) == (static_cast< gum::Idx >(1)));

      gum::DiscreteVariable& v = var1;

      CHECK((v.label(1)) == ("4"));
      CHECK((v["4"]) == (static_cast< gum::Idx >(1)));

      std::stringstream s;
      s << v;
      CHECK((s.str()) == ("var1:Range([3,9])"));
      CHECK((v.toString()) == ("var1:Range([3,9])"));
    }

    static void testNumerical() {
      gum::RangeVariable var1("var1", "this is var1", 10, 20);
      CHECK((var1.domainSize()) == (static_cast< gum::Size >(11)));

      for (gum::Idx i = 0; i < var1.domainSize(); i++) {
        CHECK((var1.numerical(i)) == (var1.minVal() + i));
      }
    }
  };

  GUM_TEST_ACTIF(Copy)
  GUM_TEST_ACTIF(Labels)
  GUM_TEST_ACTIF(Numerical)
}   // namespace gum_tests
