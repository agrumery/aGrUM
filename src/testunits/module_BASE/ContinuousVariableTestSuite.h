
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/variables/continuousVariable.h>

namespace gum_tests {

  class ContinuousVariableTestSuite : public CxxTest::TestSuite {
    public:
    void test_constructors() {
      gum::ContinuousVariable<> var1("x1", "");
      TS_ASSERT(var1.upperBound() == std::numeric_limits< double >::infinity());
      TS_ASSERT(var1.lowerBound() == -std::numeric_limits< double >::infinity());

      gum::ContinuousVariable<> var2("x2", "", -10, 10);
      TS_ASSERT(var2.lowerBound() == -10.0f);
      TS_ASSERT(var2.upperBound() == 10.0f);

      gum::ContinuousVariable<> var3(var2);
      TS_ASSERT(var3.lowerBound() == -10.0f);
      TS_ASSERT(var3.upperBound() == 10.0f);

      gum::ContinuousVariable< double > var4(var3);
      TS_ASSERT(var4.lowerBound() == -10.0);
      TS_ASSERT(var4.upperBound() == 10.0);

      gum::ContinuousVariable< double > var5(std::move(var4));
      TS_ASSERT(var4.lowerBound() == -10.0);
      TS_ASSERT(var4.upperBound() == 10.0);

      gum::ContinuousVariable< double >* var6 = var5.clone();
      TS_ASSERT(var6->lowerBound() == -10.0);
      TS_ASSERT(var6->upperBound() == 10.0);
      delete var6;

      var3 = var1;
      TS_ASSERT(var3.upperBound() == std::numeric_limits< double >::infinity());
      TS_ASSERT(var3.lowerBound() == -std::numeric_limits< double >::infinity());

      var3 = var5;
      TS_ASSERT(var3.lowerBound() == -10.0f);
      TS_ASSERT(var3.upperBound() == 10.0f);

      var4.setUpperBound(5);
      var4.setLowerBound(0);
      TS_ASSERT(var4.lowerBound() == 0.0);
      TS_ASSERT(var4.upperBound() == 5.0);
      var4 = std::move(var5);
      TS_ASSERT(var4.lowerBound() == -10.0);
      TS_ASSERT(var4.upperBound() == 10.0);
    }


    void test_methods() {
      gum::ContinuousVariable<> var1("x1", "");
      TS_ASSERT(var1.upperBound() == std::numeric_limits< double >::infinity());
      TS_ASSERT(var1.lowerBound() == -std::numeric_limits< double >::infinity());

      gum::ContinuousVariable<> var2("x2", "xxx", -10, 10);
      TS_ASSERT(var2.lowerBound() == -10.0f);
      TS_ASSERT(var2.upperBound() == 10.0f);

      TS_ASSERT(var1["44"] == 44.0f);
      TS_ASSERT(var2["4"] == 4.0f);
      TS_ASSERT_THROWS(var2["44"], gum::OutOfBounds);
      TS_ASSERT_THROWS(var2["4xx"], gum::TypeError);
      TS_ASSERT(var2[" \t\t4\t  "] == 4.0f);

      TS_ASSERT_THROWS(var2.setLowerBound(44.0f), gum::OutOfBounds);
      var2.setLowerBound(4);
      TS_ASSERT(var2.lowerBound() == 4.0f);
      TS_ASSERT(var2.upperBound() == 10.0f);

      TS_ASSERT_THROWS(var2.setUpperBound(2.0f), gum::OutOfBounds);
      var2.setUpperBound(8.5f);
      TS_ASSERT(var2.lowerBound() == 4.0f);
      TS_ASSERT(var2.upperBound() == 8.5f);

      TS_ASSERT(var2.varType() == gum::VarType::Continuous);
      TS_ASSERT(std::stof(var2.label(4.5)) == 4.5f);
      TS_ASSERT_THROWS(var2.label(10.0f), gum::OutOfBounds);
      TS_ASSERT(var2.belongs(5.6f));
      TS_ASSERT(!var2.belongs(15.6f));

      auto domain = var2.domain();
      /*
      std::string regexp = "\\[([0-9.\\-]+);([0-9.\\-]+)\\]";
      std::regex  reg(regexp);
      std::smatch match;
      TS_ASSERT(std::regex_match(domain, match, reg));
      TS_ASSERT(std::stof(match[1]) == 4.0f);
      TS_ASSERT(std::stof(match[2]) == 8.5f);
      */
      TS_ASSERT(domain == "[4;8.5]");

      TS_ASSERT(var2.toString() == ("x2" + domain));
      TS_ASSERT(var2.toStringWithDescription() == var2.description() + domain);
    }
  };
}   // namespace gum_tests
