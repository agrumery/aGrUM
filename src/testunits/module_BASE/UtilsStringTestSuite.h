/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/core/utils_string.h>

namespace gum_tests {

  class UtilsStringTestSuite: public CxxTest::TestSuite {
    public:
    void TestIsInteger() {
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
  };
}