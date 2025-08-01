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
#include <agrum/BN/generator/MCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

namespace gum_tests {

  class [[maybe_unused]] MCBayesNetGeneratorTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(CreationDeletionFloat) {
      gum::MCBayesNetGenerator< float >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(gen
                                   = new gum::MCBayesNetGenerator< float >(10, 15, 3, 15, 20, 25));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(CreationDeletionDouble) {
      gum::MCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(gen
                                   = new gum::MCBayesNetGenerator< double >(10, 15, 3, 15, 20, 25));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }

    GUM_ACTIVE_TEST(GenerationBNFloat) {
      gum::MCBayesNetGenerator< float > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< float >            bn;
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(bn))
    }

    GUM_ACTIVE_TEST(GenerationBNDouble) {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(bn))
    }

    GUM_ACTIVE_TEST(GenerationfromBNDouble) {
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
            TS_ASSERT(false)
            return;
          }
        }
        GUM_UNUSED(nbrtests);
      }
    }

    GUM_ACTIVE_TEST(DisturbBNFloatCPT) {
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
            TS_ASSERT(false)
            return;
          }
        }
      }
    }

    GUM_ACTIVE_TEST(GenerationBNDoubleCPT) {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(bn))
    }

    GUM_ACTIVE_TEST(InferenceFloat) {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference())
    }

    GUM_ACTIVE_TEST(InferenceDouble) {
      gum::MCBayesNetGenerator< double > gen(10, 15, 3, 15, 20, 25);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference())
    }
  };
}   // namespace gum_tests
