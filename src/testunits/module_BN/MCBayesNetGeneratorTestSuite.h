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
      gum::MCBayesNetGenerator< float >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(gen
                                   = new gum::MCBayesNetGenerator< float >(10, 15, 3, 15, 20, 25));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionDouble() {
      gum::MCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(gen
                                   = new gum::MCBayesNetGenerator< double >(10, 15, 3, 15, 20, 25));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testGenerationBNFloat() {
      gum::MCBayesNetGenerator< float > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< float >            bn;
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(bn));
    }

    void testGenerationBNDouble() {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(bn));
    }

    void testGenerationfromBNDouble() {
      int nbrtests = 0;
      while (true) {
        try {
          gum::MCBayesNetGenerator< double > gen(10, 15, 5, 15, 20, 25);

          gum::BayesNet< double > bn;
          gen.generateBN(bn);

          gum::BayesNet< double >            bn2;
          gum::MCBayesNetGenerator< double > gen2(15, 20, 25, 15);
          gen2.generateBN(bn2);
          GUM_UNUSED(bn);
          GUM_UNUSED(bn2);
          return;
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          nbrtests++;
          if (nbrtests > 3) {
            TS_ASSERT(false);
            return;
          }
        }
        GUM_UNUSED(nbrtests);
      }
    }

    void testDisturbBNFloatCPT() {
      int nbrtests = 0;
      while (true) {
        try {
          gum::MCBayesNetGenerator< float > gen(10, 15, 3, 15, 20, 25);
          gum::BayesNet< float >            bn;
          gen.generateBN(bn);
          gen.disturbBN(bn);
          GUM_UNUSED(bn);
          return;   // first OK : go
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          nbrtests++;
          if (nbrtests > 3) {
            TS_ASSERT(false);
            return;
          }
        }
      }
    }

    void testGenerationBNDoubleCPT() {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(bn));
    }

    void testInferenceFloat() {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());
    }

    void testInferenceDouble() {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());
    }
  };
}   // namespace gum_tests
