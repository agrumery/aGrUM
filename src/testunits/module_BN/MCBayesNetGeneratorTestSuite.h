
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
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/generator/MCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/core/set.h>

namespace gum_tests {

  class MCBayesNetGeneratorTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testCreationDeletionFloat() {
      gum::MCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
         gen = new gum::MCBayesNetGenerator< double >(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionFloatCPT() {
      gum::MCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
         gen = new gum::MCBayesNetGenerator< double >(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionDouble() {
      gum::MCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
         gen = new gum::MCBayesNetGenerator< double >(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionDoubleCPT() {
      gum::MCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
         gen = new gum::MCBayesNetGenerator< double >(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testGenerationBNFloat() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(bn));
    }

    void testGenerationBNDouble() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(bn));
    }

    void testGenerationfromBNFloat() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::MCBayesNetGenerator< double > gen2(bn, 30, 40, 50));
    }

    void testGenerationfromBNDouble() {
      int nbrtests = 0;
      try {
        gum::MCBayesNetGenerator< double > gen(30, 100, 5, 30, 40, 50);

        gum::BayesNet< double > bn;
        gen.generateBN(bn);
        gum::MCBayesNetGenerator< double > gen2(bn, 30, 40, 50);
      } catch (gum::Exception& e) {
        nbrtests++;
        if (nbrtests > 10) { TS_ASSERT(false); }
      }
    }

    void testDisturbBNFloatCPT() {
      int nbrtests = 0;
      try {
        gum::MCBayesNetGenerator< float > gen(30, 60, 3, 30, 40, 50);
        gum::BayesNet< float >            bn;
        gen.generateBN(bn);
        gen.disturbBN(bn);
      } catch (gum::Exception& e) {
        nbrtests++;
        if (nbrtests > 10) { TS_ASSERT(false); }
      }
    }

    void testGenerationBNDoubleCPT() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(bn));
    }

    void testInferenceFloat() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());
    }

    void testInferenceDouble() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());
    }
  };
}   // namespace gum_tests
