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
#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  SimpleBayesNetGenerator
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct SimpleBayesNetGeneratorTestSuite {
    public:
    static void testCreationDeletion_1() {
      gum::SimpleBayesNetGenerator< double >* gen = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(gen = new gum::SimpleBayesNetGenerator< double >(10, 20));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }   // namespace gum_tests

    static void testCreationDeletion_2() {
      gum::SimpleBayesNetGenerator< double >* gen = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(gen = new gum::SimpleBayesNetGenerator< double >(10, 20));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete gen);
    }

    static void testGenerationFloat_1() {
      gum::SimpleBayesNetGenerator< double > gen(10, 20);
      gum::BayesNet< double >*               bn = new gum::BayesNet< double >();

      GUM_CHECK_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0) delete bn;
    }

    static void testGenerationFloat_2() {
      gum::SimpleBayesNetGenerator< double > gen(100, 500);
      ;

      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for cicuits
      std::vector< gum::NodeId > stack;
      gum::Set< gum::NodeId >    passed;
      const gum::DAG&            dag = bn->dag();

      for (const auto node: dag.nodes()) {
        CHECK_THROWS_AS(dag.directedPath(node, node), const gum::NotFound&);
      }

      if (bn != 0) delete bn;
    }

    static void testGenerationFloat_3() {
      gum::SimpleBayesNetGenerator< double > gen(10, 45);

      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for inference
      gum::ShaferShenoyInference< double > ssInf(bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(ssInf.makeInference());

      if (bn != 0) delete bn;
    }

    static void testGenerationFloat_4() {
      gum::SimpleBayesNetGenerator< double > gen(10, 35);

      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(lazyInf.makeInference());

      if (bn != 0) delete bn;
    }

    static void testGenerationFloat_5() {
      gum::SimpleBayesNetGenerator< double > gen(10, 45);
      gum::BayesNet< double >*               bn = 0;

      GUM_CHECK_ASSERT_THROWS_NOTHING(bn = new gum::BayesNet< double >(); gen.generateBN(*bn);
                                      gum::LazyPropagation< double > lazyInf(bn);
                                      lazyInf.makeInference();
                                      delete bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn = new gum::BayesNet< double >(); gen.generateBN(*bn);
                                      gum::LazyPropagation< double > lazyInf(bn);
                                      lazyInf.makeInference();
                                      delete bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(bn = new gum::BayesNet< double >(); gen.generateBN(*bn);
                                      gum::LazyPropagation< double > lazyInf(bn);
                                      lazyInf.makeInference();
                                      delete bn);
    }

    static void testGenerationDouble_1() {
      gum::SimpleBayesNetGenerator< double > gen(10, 35);
      gum::BayesNet< double >*               bn = new gum::BayesNet< double >();

      GUM_CHECK_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0) delete bn;
    }

    static void testGenerationDouble_2() {
      // Test for cicuits
      gum::SimpleBayesNetGenerator< double > gen(10, 45);
      gum::BayesNet< double >*               bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);

      std::vector< gum::NodeId > stack;
      gum::Set< gum::NodeId >    passed;
      const gum::DAG&            dag = bn->dag();

      for (const auto node: dag.nodes()) {
        CHECK_THROWS_AS(dag.directedPath(node, node), const gum::NotFound&);
      }

      if (bn != 0) delete bn;
    }

    static void testGenerationDouble_3() {
      gum::SimpleBayesNetGenerator< double > gen(10, 45);
      gum::BayesNet< double >*               bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);
      // Test for inference
      gum::ShaferShenoyInference< double > ssInf(bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(ssInf.makeInference());

      if (bn != 0) delete bn;
    }

    static void testGenerationDouble_4() {
      gum::SimpleBayesNetGenerator< double > gen(10, 45);
      gum::BayesNet< double >*               bn = new gum::BayesNet< double >();
      gen.generateBN(*bn);

      // Test for inference
      gum::LazyPropagation< double > lazyInf(bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(lazyInf.makeInference());

      if (bn != 0) delete bn;
    }
  };

  GUM_TEST_ACTIF(CreationDeletion_1)
  GUM_TEST_ACTIF(CreationDeletion_2)
  GUM_TEST_ACTIF(GenerationFloat_1)
  GUM_TEST_ACTIF(GenerationFloat_2)
  GUM_TEST_ACTIF(GenerationFloat_3)
  GUM_TEST_ACTIF(GenerationFloat_4)
  GUM_TEST_ACTIF(GenerationFloat_5)
  GUM_TEST_ACTIF(GenerationDouble_1)
  GUM_TEST_ACTIF(GenerationDouble_2)
  GUM_TEST_ACTIF(GenerationDouble_3)
  GUM_TEST_ACTIF(GenerationDouble_4)
}   // namespace gum_tests
