
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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

#include "testsuite_utils.h"
#include <cxxtest/AgrumTestSuite.h>

#include <agrum/PRM/gspan/DFSCode.h>

/*
 * Test of gum::prm::gspan::DFSCode and gum::prm::gspan::EdgeCode.
 *
 * @author Lionel TORTI
 */

namespace gum_tests {

  class DFSCodeTestSuite: public CxxTest::TestSuite {
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

    void __build_alpha() {
      alpha = new gum::prm::gspan::DFSCode();
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(0, 1, X, a, Y));
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(1, 2, Y, b, X));
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(2, 0, X, a, X));
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(2, 3, X, c, Z));
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(3, 1, Z, b, Y));
      alpha->codes.push_back(new gum::prm::gspan::EdgeCode(1, 4, Y, d, Z));
    }

    void __build_beta() {
      beta = new gum::prm::gspan::DFSCode();
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(0, 1, Y, a, X));
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(1, 2, X, a, X));
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(2, 0, X, b, Y));
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(2, 3, X, c, Z));
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(3, 0, Z, b, Y));
      beta->codes.push_back(new gum::prm::gspan::EdgeCode(0, 4, Y, d, Z));
    }

    void __build_gamma() {
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
      __build_alpha();
      __build_beta();
      __build_gamma();
    }

    void tearDown() {
      delete alpha;
      delete beta;
      delete gamma;
    }

    void testLesserThan() {
      TS_ASSERT((*gamma) < (*alpha));
      TS_ASSERT((*gamma) < (*beta));
      TS_ASSERT((*alpha) < (*beta));
      TS_ASSERT(!((*alpha) < (*alpha)));
      TS_ASSERT(!((*beta) < (*beta)));
      TS_ASSERT(!((*gamma) < (*gamma)));
      TS_ASSERT(!((*alpha) < (*gamma)));
      TS_ASSERT(!((*beta) < (*gamma)));
      TS_ASSERT(!((*beta) < (*alpha)));
    }

    void testLesserOrEqualThan() {
      TS_ASSERT((*gamma) <= (*alpha));
      TS_ASSERT((*gamma) <= (*beta));
      TS_ASSERT((*alpha) <= (*beta));
      TS_ASSERT((*alpha) <= (*alpha));
      TS_ASSERT((*beta) <= (*beta));
      TS_ASSERT((*gamma) <= (*gamma));
      TS_ASSERT(!((*alpha) <= (*gamma)));
      TS_ASSERT(!((*beta) <= (*gamma)));
      TS_ASSERT(!((*beta) <= (*alpha)));
    }

    void testAlphaNeighbors() {
      for (gum::prm::gspan::DFSCode::iterator iter = alpha->codes.begin() + 1;
           iter != alpha->codes.end();
           ++iter) {
        TS_ASSERT(gum::prm::gspan::DFSCode::validNeighbors(*(iter - 1), *iter));
      }
    }

    void testBetaNeighbors() {
      for (gum::prm::gspan::DFSCode::iterator iter = beta->codes.begin() + 1;
           iter != beta->codes.end();
           ++iter) {
        TS_ASSERT(gum::prm::gspan::DFSCode::validNeighbors(*(iter - 1), *iter));
      }
    }

    void testGammaNeighbors() {
      for (gum::prm::gspan::DFSCode::iterator iter = gamma->codes.begin() + 1;
           iter != gamma->codes.end();
           ++iter) {
        TS_ASSERT(gum::prm::gspan::DFSCode::validNeighbors(*(iter - 1), *iter));
      }
    }

    void testAlphaPrint() {
      TS_ASSERT_THROWS_NOTHING({
        std::stringstream ss;
        ss << std::endl << (*alpha) << std::endl;
      });
    }

    void testBetaPrint() {
      TS_ASSERT_THROWS_NOTHING({
        std::stringstream ss;
        ss << std::endl << (*beta) << std::endl;
      });
    }

    void testGammaPrint() {
      TS_ASSERT_THROWS_NOTHING({
        std::stringstream ss;
        ss << std::endl << (*gamma) << std::endl;
      });
    }
  };
}   // namespace gum_tests
