/**
 *
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/tools/core/math/gammaLog2.h>

#include <agrum/tools/core/math/math_utils.h>

namespace gum_tests {

  class [[maybe_unused]] GammaLog2TestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(1) {
      gum::GammaLog2 f;
      gum::GammaLog2 g(true);

      for (auto i = 0; i < 60; ++i) {
        for (auto j = 0; j < 100; ++j) {
          if (i + j > 0) {
            double index = i + j / 100.0;
            TS_ASSERT_DELTA(f(index), g(index), 0.1)

            double findex = i + j / 100.0;
            TS_ASSERT_DELTA(f(findex), g(findex), 0.1)
          }
        }
      }
    }
  };

}   // namespace gum_tests
