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
#pragma once


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/set.h>
#include <agrum/BN/generator/maxParentsMCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

namespace gum_tests {

  class GUM_TEST_SUITE(MaxParentsMCBayesNetGenerator) {
    public:
    void setUp() {}

    void tearDown() {}

    GUM_ACTIVE_TEST(CreationDeletionFloat) {
      gum::MaxParentsMCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxParentsMCBayesNetGenerator< double >(30, 70, 3, 4, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen)
    }   // namespace gum_tests

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
