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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/set.h>
#include <agrum/BN/generator/maxInducedWidthMCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

namespace gum_tests {

  class [[maybe_unused]] MaxInducedWidthMCBayesNetGeneratorTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    GUM_ACTIVE_TEST(CreationDeletionFloat) {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxInducedWidthMCBayesNetGenerator< double >(30, 100, 3, 4, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(CreationDeletionFloatCPT) {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxInducedWidthMCBayesNetGenerator< double >(30, 150, 4, 4, 30, 10, 30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(CreationDeletionDouble) {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxInducedWidthMCBayesNetGenerator< double >(30, 100, 4, 4, 30, 10, 30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(CreationDeletionDoubleCPT) {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxInducedWidthMCBayesNetGenerator< double >(30, 100, 4, 4, 30, 10, 30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(GenerationBNFloat) {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(30, 100, 10, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn))

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(GenerationBNDouble) {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(30, 100, 5, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn))

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(GenerationfromBNFloat) {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(30, 100, 10, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::MaxInducedWidthMCBayesNetGenerator< double > gen2(*bn, 4, 30, 40, 50));

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(GenerationfromBNDouble) {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(30, 100, 5, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::MaxInducedWidthMCBayesNetGenerator< double > gen2(*bn, 4, 30, 40, 50));

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(DisturbBNFloatCPT) {
      int nbrtests = 0;
      while (true) {
        try {
          gum::MaxInducedWidthMCBayesNetGenerator< double > gen(30, 100, 8, 30, 40, 50);
          gum::BayesNet< double >                           bn;
          gen.generateBN(bn);
          // gen.disturbBN(bn);
          GUM_UNUSED(bn);
          return;
        } catch (gum::Exception& e) {
          GUM_UNUSED(e);
          nbrtests++;
          if (nbrtests > 10) {
            TS_ASSERT(false)
            return;
          }
        }
      }
    }

    GUM_ACTIVE_TEST(GenerationBNDoubleCPT) {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(50, 100, 8, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(*bn))

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(InferenceFloat) {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(50, 100, 8, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference())

      if (bn != 0) delete bn;
    }

    GUM_ACTIVE_TEST(InferenceDouble) {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(50, 100, 8, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference())

      if (bn != 0) delete bn;
    }
  };
}   // namespace gum_tests
