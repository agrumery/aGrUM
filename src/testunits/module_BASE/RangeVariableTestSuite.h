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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/rangeVariable.h>

namespace gum_tests {

  class [[maybe_unused]] RangeVariableTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Copy) {
      gum::RangeVariable var1("var1", "this is var1");
      gum::RangeVariable var2("var2", "this is var2", 1, 4);

      gum::RangeVariable var3(var1);
      gum::RangeVariable var4("var4", "this is var4");
      TS_GUM_ASSERT_THROWS_NOTHING(var4 = var2)

      TS_ASSERT_EQUALS(var4.minVal(), var2.minVal())
      TS_ASSERT_EQUALS(var1.maxVal(), var3.maxVal())
      TS_ASSERT_DIFFERS(var4.minVal(), var1.minVal())
    }

    GUM_ACTIVE_TEST(Labels) {
      gum::RangeVariable var1("var1", "this is var1");
      TS_ASSERT_EQUALS(var1.domainSize(), (gum::Size)2)
      TS_ASSERT(!var1.empty())

      var1.setMinVal(1);
      var1.setMaxVal(0);
      TS_ASSERT(var1.empty())

      var1.setMaxVal(9);
      TS_ASSERT(!var1.empty())
      TS_ASSERT_EQUALS(var1.domainSize(), (gum::Size)9)
      TS_ASSERT(var1.belongs(3L))
      TS_ASSERT(!var1.belongs(0L))
      TS_ASSERT(!var1.belongs(10L))

      var1.setMinVal(3);

      TS_ASSERT_EQUALS(var1.label(1), "4")
      TS_ASSERT_EQUALS(var1["4"], (gum::Idx)1)

      gum::DiscreteVariable& v = var1;

      TS_ASSERT_EQUALS(v.label(1), "4")
      TS_ASSERT_EQUALS(v["4"], (gum::Idx)1)

      std::stringstream s;
      s << v;
      TS_ASSERT_EQUALS(s.str(), "var1:Range([3,9])")
      TS_ASSERT_EQUALS(v.toString(), "var1:Range([3,9])")
    }

    GUM_ACTIVE_TEST(Numerical) {
      gum::RangeVariable var1("var1", "this is var1", 10, 20);
      TS_ASSERT_EQUALS(var1.domainSize(), (gum::Size)11)

      for (gum::Idx i = 0; i < var1.domainSize(); i++) {
        TS_ASSERT_EQUALS(var1.numerical(i), var1.minVal() + i)
      }
    }
  };
}   // namespace gum_tests
