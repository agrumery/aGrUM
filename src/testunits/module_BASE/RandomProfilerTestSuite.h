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





#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/config.h>

#include <agrum/base/core/timer.h>
#include <agrum/BN/generator/MCBayesNetGenerator.h>

#include <agrum/base/core/utils_random.h>

namespace gum_tests {

  // a test to see if GUM_RANDOMSEED is working
  class [[maybe_unused]] RandomProfilerTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(RandomSeed) {
      TS_ASSERT((GUM_RANDOMSEED == 0) || (GUM_RANDOMSEED == 10))

      TS_ASSERT_EQUALS(GUM_RANDOMSEED, 0);

      gum::initRandom(20);
      auto x1 = gum::randomProba();
      auto x2 = gum::randomProba();
      auto x3 = gum::randomProba();

      gum::initRandom(0);
      // can falsely fail in rare occasion
      if (fabs((gum::randomProba()) - (x1)) < TS_GUM_SMALL_ERROR)
        if (fabs((gum::randomProba()) - (x2)) < TS_GUM_SMALL_ERROR)
          if (fabs((gum::randomProba()) - (x3)) < TS_GUM_SMALL_ERROR) TS_ASSERT(false);
      gum::initRandom(20);
      TS_GUM_ASSERT_ALMOST_EQUALS(gum::randomProba(), x1)
      TS_GUM_ASSERT_ALMOST_EQUALS(gum::randomProba(), x2)
      TS_GUM_ASSERT_ALMOST_EQUALS(gum::randomProba(), x3)

      gum::initRandom(GUM_RANDOMSEED);
    }

    GUM_ACTIVE_TEST(RandomSeeForStructure) {
      try {
        auto n_nodes  = 100;
        auto n_arcs   = 150;
        auto n_modmax = 3;

        gum::MCBayesNetGenerator< double > gen(n_nodes, n_arcs, n_modmax);
        gum::BayesNet< double >            bn;

        gum::initRandom(20);
        gen.generateBN(bn);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
      /*
            auto s1 = bn.toDot();

            gum::initRandom(0);
            gen.generateBN(bn);
            auto s2 = bn.toDot();

            gum::initRandom(20);
            gen.generateBN(bn);
            auto s3 = bn.toDot();

            TS_ASSERT_EQUALS(s1, s1);
            TS_ASSERT_DIFFERS(s1, s2);
            TS_ASSERT_EQUALS(s1, s3);

            gum::initRandom(GUM_RANDOMSEED);*/
    }
  };
}   // namespace gum_tests
