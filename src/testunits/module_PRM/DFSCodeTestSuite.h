/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <agrum/PRM/gspan/DFSCode.h>

#include "utils.h"

/*
 * Test of gum::prm::gspan::DFSCode and gum::prm::gspan::EdgeCode.
 *
 * @author Lionel TORTI
 */

namespace gum_tests {

  class [[maybe_unused]] DFSCodeTestSuite: public CxxTest::TestSuite {
    private:
    gum::prm::gspan::DFSCode* alpha;
    gum::prm::gspan::DFSCode* beta;
    gum::prm::gspan::DFSCode* gamma;

    int X;
    int Y;
    int Z;

    int a;
    int b;
    int c;
    int d;

    void _build_alpha_() {
      alpha = new gum::prm::gspan::DFSCode();
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(0, 1, X, a, Y));
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(1, 2, Y, b, X));
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(2, 0, X, a, X));
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(2, 3, X, c, Z));
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(3, 1, Z, b, Y));
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(1, 4, Y, d, Z));
    }

    void _build_beta_() {
      beta = new gum::prm::gspan::DFSCode();
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(0, 1, Y, a, X));
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(1, 2, X, a, X));
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(2, 0, X, b, Y));
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(2, 3, X, c, Z));
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(3, 0, Z, b, Y));
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(0, 4, Y, d, Z));
    }

    void _build_gamma_() {
      gamma = new gum::prm::gspan::DFSCode();
      gamma->codes.push_back(new gum::prm::gspan::EdgeCode(0, 1, X, a, X));
      gamma->codes.push_back(new gum::prm::gspan::EdgeCode(1, 2, X, a, Y));
      gamma->codes.push_back(new gum::prm::gspan::EdgeCode(2, 0, Y, b, X));
      gamma->codes.push_back(new gum::prm::gspan::EdgeCode(2, 3, Y, b, Z));
      gamma->codes.push_back(new gum::prm::gspan::EdgeCode(3, 0, Z, c, X));
      gamma->codes.push_back(new gum::prm::gspan::EdgeCode(2, 4, Y, d, Z));
    }

    public:
    void setUp() {
      X = 0;
      Y = 1;
      Z = 2;
      a = 0;
      b = 1;
      c = 2;
      d = 3;
      _build_alpha_();
      _build_beta_();
      _build_gamma_();
    }

    void tearDown() {
      delete alpha;
      delete beta;
      delete gamma;
    }

    GUM_ACTIVE_TEST(LesserThan) {
      TS_ASSERT((*gamma) < (*alpha))
      TS_ASSERT((*gamma) < (*beta))
      TS_ASSERT((*alpha) < (*beta))
      TS_ASSERT(!((*alpha) < (*alpha)))
      TS_ASSERT(!((*beta) < (*beta)))
      TS_ASSERT(!((*gamma) < (*gamma)))
      TS_ASSERT(!((*alpha) < (*gamma)))
      TS_ASSERT(!((*beta) < (*gamma)))
      TS_ASSERT(!((*beta) < (*alpha)))
    }

    GUM_ACTIVE_TEST(LesserOrEqualThan) {
      TS_ASSERT((*gamma) <= (*alpha))
      TS_ASSERT((*gamma) <= (*beta))
      TS_ASSERT((*alpha) <= (*beta))
      TS_ASSERT((*alpha) <= (*alpha))
      TS_ASSERT((*beta) <= (*beta))
      TS_ASSERT((*gamma) <= (*gamma))
      TS_ASSERT(!((*alpha) <= (*gamma)))
      TS_ASSERT(!((*beta) <= (*gamma)))
      TS_ASSERT(!((*beta) <= (*alpha)))
    }

    GUM_ACTIVE_TEST(AlphaNeighbors) {
      for (gum::prm::gspan::DFSCode::iterator iter = alpha->codes.begin() + 1;
           iter != alpha->codes.end();
           ++iter) {
        TS_ASSERT(gum::prm::gspan::DFSCode::validNeighbors(*(iter - 1), *iter))
      }
    }

    GUM_ACTIVE_TEST(BetaNeighbors) {
      for (gum::prm::gspan::DFSCode::iterator iter = beta->codes.begin() + 1;
           iter != beta->codes.end();
           ++iter) {
        TS_ASSERT(gum::prm::gspan::DFSCode::validNeighbors(*(iter - 1), *iter))
      }
    }

    GUM_ACTIVE_TEST(GammaNeighbors) {
      for (gum::prm::gspan::DFSCode::iterator iter = gamma->codes.begin() + 1;
           iter != gamma->codes.end();
           ++iter) {
        TS_ASSERT(gum::prm::gspan::DFSCode::validNeighbors(*(iter - 1), *iter))
      }
    }

    GUM_ACTIVE_TEST(AlphaPrint) {
      TS_ASSERT_THROWS_NOTHING({
        std::stringstream ss;
        ss << std::endl << (*alpha) << std::endl;
      });
    }

    GUM_ACTIVE_TEST(BetaPrint) {
      TS_ASSERT_THROWS_NOTHING({
        std::stringstream ss;
        ss << std::endl << (*beta) << std::endl;
      });
    }

    GUM_ACTIVE_TEST(GammaPrint) {
      TS_ASSERT_THROWS_NOTHING({
        std::stringstream ss;
        ss << std::endl << (*gamma) << std::endl;
      });
    }
  };
}   // namespace gum_tests
