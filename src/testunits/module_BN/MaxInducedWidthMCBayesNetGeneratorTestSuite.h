/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 * (@AMU) info_at_agrum_dot_org
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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/generator/maxInducedWidthMCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/core/set.h>

namespace gum_tests {

  class MaxInducedWidthMCBayesNetGeneratorTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testCreationDeletionFloat() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(gen
                                   = new gum::MaxInducedWidthMCBayesNetGenerator<
                                      double >(30, 100, 3, 4, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionFloatCPT() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(gen
                                   = new gum::MaxInducedWidthMCBayesNetGenerator<
                                      double >(30, 150, 4, 4, 30, 10, 30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionDouble() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(gen
                                   = new gum::MaxInducedWidthMCBayesNetGenerator<
                                      double >(30, 100, 4, 4, 30, 10, 30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionDoubleCPT() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(gen
                                   = new gum::MaxInducedWidthMCBayesNetGenerator<
                                      double >(30, 100, 4, 4, 30, 10, 30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testGenerationBNFloat() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >
                               gen(30, 100, 10, 4, 30, 40, 50);
      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0) delete bn;
    }

    void testGenerationBNDouble() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >
                               gen(30, 100, 5, 4, 30, 40, 50);
      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0) delete bn;
    }

    void testGenerationfromBNFloat() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >
                               gen(30, 100, 10, 4, 30, 40, 50);
      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::MaxInducedWidthMCBayesNetGenerator< double >
            gen2(*bn, 4, 30, 40, 50));

      if (bn != 0) delete bn;
    }

    void testGenerationfromBNDouble() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >
                               gen(30, 100, 5, 4, 30, 40, 50);
      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::MaxInducedWidthMCBayesNetGenerator< double >
            gen2(*bn, 4, 30, 40, 50));

      if (bn != 0) delete bn;
    }

    void testDisturbBNFloatCPT() {
      int nbrtests = 0;
      while (true) {
        try {
          gum::MaxInducedWidthMCBayesNetGenerator< double > gen(30,
                                                                100,
                                                                8,
                                                                30,
                                                                40,
                                                                50);
          gum::BayesNet< double >                           bn;
          gen.generateBN(bn);
          gen.disturbBN(bn);
          GUM_UNUSED(bn);
          return;
        } catch (gum::Exception& e) {
          GUM_UNUSED(e);
          nbrtests++;
          if (nbrtests > 10) {
            TS_ASSERT(false);
            return;
          }
        }
      }
    }

    void testGenerationBNDoubleCPT() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >
                               gen(50, 100, 8, 4, 30, 40, 50);
      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(*bn));

      if (bn != 0) delete bn;
    }

    void testInferenceFloat() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >
                               gen(50, 100, 8, 4, 30, 40, 50);
      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());

      if (bn != 0) delete bn;
    }

    void testInferenceDouble() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >
                               gen(50, 100, 8, 4, 30, 40, 50);
      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());

      if (bn != 0) delete bn;
    }
  };
}   // namespace gum_tests
