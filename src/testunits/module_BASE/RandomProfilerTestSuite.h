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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/config.h>
#include <agrum/tools/core/utils_random.h>
#include <agrum/tools/core/timer.h>
#include <agrum/BN/generator/MCBayesNetGenerator.h>

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
