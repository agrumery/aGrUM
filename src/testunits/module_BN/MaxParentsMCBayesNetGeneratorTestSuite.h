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
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/core/set.h>

#include <agrum/BN/generator/maxParentsMCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

namespace gum_tests {

  class [[maybe_unused]] MaxParentsMCBayesNetGeneratorTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    GUM_ACTIVE_TEST(CreationDeletionFloat) {
      gum::MaxParentsMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
         gen = new gum::MaxParentsMCBayesNetGenerator< double >(30, 70, 3, 4, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(CreationDeletionFloatCPT) {
      gum::MaxParentsMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
         gen = new gum::MaxParentsMCBayesNetGenerator< double >(30, 70, 4, 4, 20, 10, 30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(CreationDeletionDouble) {
      gum::MaxParentsMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
         gen = new gum::MaxParentsMCBayesNetGenerator< double >(30, 70, 4, 4, 20, 10, 30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(CreationDeletionDoubleCPT) {
      gum::MaxParentsMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
         gen = new gum::MaxParentsMCBayesNetGenerator< double >(30, 70, 4, 4, 20, 10, 30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(GenerationBNFloat) {
      gum::MaxParentsMCBayesNetGenerator< double > gen(30, 70, 5, 4, 30, 40, 50);
      gum::BayesNet< double >*                     bn = new gum::BayesNet< double >();
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn))

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(GenerationBNDouble) {
      gum::MaxParentsMCBayesNetGenerator< double > gen(30, 70, 5, 4, 30, 40, 50);
      gum::BayesNet< double >*                     bn = new gum::BayesNet< double >();
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn))

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(GenerationfromBNFloat) {
      gum::MaxParentsMCBayesNetGenerator< double > gen(30, 70, 10, 4, 30, 40, 50);
      gum::BayesNet< double >*                     bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::MaxParentsMCBayesNetGenerator< double > gen2(*bn, 4, 30, 40, 50));

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(GenerationfromBNDouble) {
      gum::MaxParentsMCBayesNetGenerator< double > gen(30, 70, 5, 4, 30, 40, 50);
      gum::BayesNet< double >*                     bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
         gum::MaxParentsMCBayesNetGenerator< double > gen2(*bn, 4, 30, 40, 50));

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(DisturbBNFloatCPT) {
      gum::MaxParentsMCBayesNetGenerator< double > gen(30, 70, 4, 30, 40, 50);
      gum::BayesNet< double >*                     bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(*bn))

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(GenerationBNDoubleCPT) {
      gum::MaxParentsMCBayesNetGenerator< double > gen(30, 70, 3, 4, 30, 40, 50);
      gum::BayesNet< double >*                     bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(*bn))

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(InferenceFloat) {
      gum::MaxParentsMCBayesNetGenerator< double > gen(30, 70, 8, 4, 30, 40, 50);
      gum::BayesNet< double >*                     bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference())

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(InferenceDouble) {
      gum::MaxParentsMCBayesNetGenerator< double > gen(30, 70, 8, 4, 30, 40, 50);
      gum::BayesNet< double >*                     bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference())

      if (bn != 0) delete bn;
    }
  };
}   // namespace gum_tests
