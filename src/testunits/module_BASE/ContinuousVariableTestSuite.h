/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
#include <regex>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/continuousVariable.h>

namespace gum_tests {

  class [[maybe_unused]] ContinuousVariableTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_constructors) {
      gum::ContinuousVariable<> var1("x1", "");
      TS_ASSERT_EQUALS(var1.upperBound(), std::numeric_limits< double >::infinity())
      TS_ASSERT_EQUALS(var1.lowerBound(), -std::numeric_limits< double >::infinity())

      gum::ContinuousVariable<> var2("x2", "", -10, 10);
      TS_ASSERT_EQUALS(var2.lowerBound(), -10.0f)
      TS_ASSERT_EQUALS(var2.upperBound(), 10.0f)

      gum::ContinuousVariable<> var3(var2);
      TS_ASSERT_EQUALS(var3.lowerBound(), -10.0f)
      TS_ASSERT_EQUALS(var3.upperBound(), 10.0f)

      gum::ContinuousVariable< double > var4(var3);
      TS_ASSERT_EQUALS(var4.lowerBound(), -10.0)
      TS_ASSERT_EQUALS(var4.upperBound(), 10.0)

      gum::ContinuousVariable< double > var5(std::move(var4));
      TS_ASSERT_EQUALS(var4.lowerBound(), -10.0)
      TS_ASSERT_EQUALS(var4.upperBound(), 10.0)

      gum::ContinuousVariable< double >* var6 = var5.clone();
      TS_ASSERT_EQUALS(var6->lowerBound(), -10.0)
      TS_ASSERT_EQUALS(var6->upperBound(), 10.0)
      delete var6;

      var3 = var1;
      TS_ASSERT_EQUALS(var3.upperBound(), std::numeric_limits< double >::infinity())
      TS_ASSERT_EQUALS(var3.lowerBound(), -std::numeric_limits< double >::infinity())

      var3 = var5;
      TS_ASSERT_EQUALS(var3.lowerBound(), -10.0f)
      TS_ASSERT_EQUALS(var3.upperBound(), 10.0f)

      var4.setUpperBound(5);
      var4.setLowerBound(0);
      TS_ASSERT_EQUALS(var4.lowerBound(), 0.0)
      TS_ASSERT_EQUALS(var4.upperBound(), 5.0)
      var4 = std::move(var5);
      TS_ASSERT_EQUALS(var4.lowerBound(), -10.0)
      TS_ASSERT_EQUALS(var4.upperBound(), 10.0)
    }

    GUM_ACTIVE_TEST(_methods) {
      gum::ContinuousVariable<> var1("x1", "");
      TS_ASSERT_EQUALS(var1.upperBound(), std::numeric_limits< double >::infinity())
      TS_ASSERT_EQUALS(var1.lowerBound(), -std::numeric_limits< double >::infinity())

      gum::ContinuousVariable<> var2("x2", "xxx", -10, 10);
      TS_ASSERT_EQUALS(var2.lowerBound(), -10.0f)
      TS_ASSERT_EQUALS(var2.upperBound(), 10.0f)

      TS_ASSERT_EQUALS(var1["44"], 44.0f)
      TS_ASSERT_EQUALS(var2["4"], 4.0f)
      TS_ASSERT_THROWS(var2["44"], const gum::OutOfBounds&)
      TS_ASSERT_THROWS(var2["4xx"], const gum::TypeError&)
      TS_ASSERT_EQUALS(var2[" \t\t4\t  "], 4.0f)

      TS_ASSERT_THROWS(var2.setLowerBound(44.0f), const gum::OutOfBounds&)
      var2.setLowerBound(4);
      TS_ASSERT_EQUALS(var2.lowerBound(), 4.0f)
      TS_ASSERT_EQUALS(var2.upperBound(), 10.0f)

      TS_ASSERT_THROWS(var2.setUpperBound(2.0f), const gum::OutOfBounds&)
      var2.setUpperBound(8.5f);
      TS_ASSERT_EQUALS(var2.lowerBound(), 4.0f)
      TS_ASSERT_EQUALS(var2.upperBound(), 8.5f)

      TS_ASSERT_EQUALS(var2.varType(), gum::VarType::CONTINUOUS)
      TS_ASSERT_EQUALS(std::stof(var2.label(4.5)), 4.5f)
      TS_ASSERT_THROWS(var2.label(10.0f), const gum::OutOfBounds&)
      TS_ASSERT(var2.belongs(5.6f))
      TS_ASSERT(!var2.belongs(15.6f))

      auto domain = var2.domain();
      /*
      std::string regexp = "\\[([0-9.\\-]+);([0-9.\\-]+)\\]";
      std::regex  reg(regexp);
      std::smatch match;
      TS_ASSERT(std::regex_match(domain, match, reg))
      TS_ASSERT_EQUALS(std::stof(match[1]) , 4.0f)
      TS_ASSERT_EQUALS(std::stof(match[2]) , 8.5f)
      */
      TS_ASSERT_EQUALS(domain, "[4;8.5]")

      TS_ASSERT_EQUALS(var2.toString(), ("x2" + domain))
      TS_ASSERT_EQUALS(var2.toStringWithDescription(), var2.description() + domain)
    }
  };
}   // namespace gum_tests
