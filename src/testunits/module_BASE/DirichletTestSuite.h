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

#include <agrum/tools/core/math/Dirichlet.h>

namespace gum_tests {

  class [[maybe_unused]] DirichletTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(XX) {
      std::vector< float > param{1.0f, 1.0f};
      gum::Dirichlet       dir(param);

      std::vector< float > res(2, 0);
      float                sum = 0;
      for (gum::Idx i = 0; i < 10000; ++i) {
        std::vector< float > sample = dir();
        for (gum::Idx j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (gum::Idx j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }

      TS_ASSERT(fabs(res[0] - res[1]) < 0.03)


      param[0] = 10000;
      param[1] = 1;
      sum      = 0;
      res[0] = res[1] = 0;
      for (gum::Idx i = 0; i < 10000; ++i) {
        std::vector< float > sample = dir(param);
        for (gum::Idx j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (gum::Idx j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }
      TS_ASSERT(1 - res[0] < 0.01)

      sum    = 0;
      res[0] = res[1] = 0;
      for (gum::Idx i = 0; i < 10000; ++i) {
        std::vector< float > sample = dir(param);
        for (gum::Idx j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (gum::Idx j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }
      TS_ASSERT(1 - res[0] < 0.01)

      sum    = 0;
      res[0] = res[1] = 0;
      for (gum::Idx i = 0; i < 10000; ++i) {
        std::vector< float > sample = dir(dir.param());
        for (gum::Idx j = 0; j < sample.size(); ++j) {
          res[j] += sample[j];
          sum += sample[j];
        }
      }
      for (gum::Idx j = 0; j < res.size(); ++j) {
        res[j] /= sum;
      }
      TS_ASSERT(fabs(res[0] - res[1]) < 0.03)
    }
  };

} /* namespace gum_tests */
