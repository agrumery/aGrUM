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
#include <agrum/BN/generator/MCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MCBayesNetGenerator
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct MCBayesNetGeneratorTestSuite {
    public:
    static void testCreationDeletionFloat() {
      gum::MCBayesNetGenerator< float >* gen = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gen = new gum::MCBayesNetGenerator< float >(10, 15, 3, 15, 20, 25));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }   // namespace gum_tests

    static void testCreationDeletionDouble() {
      gum::MCBayesNetGenerator< double >* gen = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gen = new gum::MCBayesNetGenerator< double >(10, 15, 3, 15, 20, 25));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }

    static void testGenerationBNFloat() {
      gum::MCBayesNetGenerator< float > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< float >            bn;
      GUM_CHECK_ASSERT_THROWS_NOTHING(gen.generateBN(bn));
    }

    static void testGenerationBNDouble() {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      GUM_CHECK_ASSERT_THROWS_NOTHING(gen.generateBN(bn));
    }

    static void testGenerationfromBNDouble() {
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
            CHECK(false);
            return;
          }
        }
        GUM_UNUSED(nbrtests);
      }
    }

    static void testDisturbBNFloatCPT() {
      int nbrtests = 0;
      while (true) {
        try {
          gum::MCBayesNetGenerator< float > gen(10, 15, 3, 15, 20, 25);
          gum::BayesNet< float >            bn;
          gen.generateBN(bn);
          // gen.disturbBN(bn);
          GUM_UNUSED(bn);
          return;   // first OK : go
        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          nbrtests++;
          if (nbrtests > 3) {
            CHECK(false);
            return;
          }
        }
      }
    }

    static void testGenerationBNDoubleCPT() {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // GUM_CHECK_ASSERT_THROWS_NOTHING(gen.disturbBN(bn));
    }

    static void testInferenceFloat() {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(&bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(lazyInf.makeInference());
    }

    static void testInferenceDouble() {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(&bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(lazyInf.makeInference());
    }
  };

  GUM_TEST_ACTIF(CreationDeletionFloat)
  GUM_TEST_ACTIF(CreationDeletionDouble)
  GUM_TEST_ACTIF(GenerationBNFloat)
  GUM_TEST_ACTIF(GenerationBNDouble)
  GUM_TEST_ACTIF(GenerationfromBNDouble)
  GUM_TEST_ACTIF(DisturbBNFloatCPT)
  GUM_TEST_ACTIF(GenerationBNDoubleCPT)
  GUM_TEST_ACTIF(InferenceFloat)
  GUM_TEST_ACTIF(InferenceDouble)
}   // namespace gum_tests
