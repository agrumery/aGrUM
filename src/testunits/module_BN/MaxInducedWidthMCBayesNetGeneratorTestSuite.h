/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


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

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MaxInducedWidthMCBayesNetGenerator
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct MaxInducedWidthMCBayesNetGeneratorTestSuite {
    public:
    static void testCreationDeletionFloat() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxInducedWidthMCBayesNetGenerator< double >(30, 100, 3, 4, 30, 40, 50));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }   // namespace gum_tests

    static void testCreationDeletionFloatCPT() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxInducedWidthMCBayesNetGenerator< double >(30, 150, 4, 4, 30, 10, 30));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }

    static void testCreationDeletionDouble() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxInducedWidthMCBayesNetGenerator< double >(30, 100, 4, 4, 30, 10, 30));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }

    static void testCreationDeletionDoubleCPT() {
      gum::MaxInducedWidthMCBayesNetGenerator< double >* gen = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxInducedWidthMCBayesNetGenerator< double >(30, 100, 4, 4, 30, 10, 30));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }

    static void testGenerationBNFloat() {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(30, 100, 10, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      GUM_CHECK_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0) delete bn;
    }

    static void testGenerationBNDouble() {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(30, 100, 5, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      GUM_CHECK_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0) delete bn;
    }

    static void testGenerationfromBNFloat() {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(30, 100, 10, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gum::MaxInducedWidthMCBayesNetGenerator< double > gen2(*bn, 4, 30, 40, 50));

      if (bn != 0) delete bn;
    }

    static void testGenerationfromBNDouble() {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(30, 100, 5, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gum::MaxInducedWidthMCBayesNetGenerator< double > gen2(*bn, 4, 30, 40, 50));

      if (bn != 0) delete bn;
    }

    static void testDisturbBNFloatCPT() {
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
            CHECK(false);
            return;
          }
        }
      }
    }

    static void testGenerationBNDoubleCPT() {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(50, 100, 8, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // GUM_CHECK_ASSERT_THROWS_NOTHING(gen.disturbBN(*bn));

      if (bn != 0) delete bn;
    }

    static void testInferenceFloat() {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(50, 100, 8, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(lazyInf.makeInference());

      if (bn != 0) delete bn;
    }

    static void testInferenceDouble() {
      gum::MaxInducedWidthMCBayesNetGenerator< double > gen(50, 100, 8, 4, 30, 40, 50);
      gum::BayesNet< double >*                          bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(lazyInf.makeInference());

      if (bn != 0) delete bn;
    }
  };

  GUM_TEST_ACTIF(CreationDeletionFloat)
  GUM_TEST_ACTIF(CreationDeletionFloatCPT)
  GUM_TEST_ACTIF(CreationDeletionDouble)
  GUM_TEST_ACTIF(CreationDeletionDoubleCPT)
  GUM_TEST_ACTIF(GenerationBNFloat)
  GUM_TEST_ACTIF(GenerationBNDouble)
  GUM_TEST_ACTIF(GenerationfromBNFloat)
  GUM_TEST_ACTIF(GenerationfromBNDouble)
  GUM_TEST_ACTIF(DisturbBNFloatCPT)
  GUM_TEST_ACTIF(GenerationBNDoubleCPT)
  GUM_TEST_ACTIF(InferenceFloat)
  GUM_TEST_ACTIF(InferenceDouble)
}   // namespace gum_tests
