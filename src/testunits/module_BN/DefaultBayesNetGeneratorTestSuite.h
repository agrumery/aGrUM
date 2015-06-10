/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/core/set.h>
#include <agrum/core/exceptions.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>

namespace gum_tests {

  class SimpleBayesNetGeneratorTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testCreationDeletion_1() {
      gum::SimpleBayesNetGenerator<float> *gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::SimpleBayesNetGenerator<float>(10, 20));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletion_2() {
      gum::SimpleBayesNetGenerator<float> *gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::SimpleBayesNetGenerator<float>(10, 20));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testGenerationFloat_1() {
      gum::SimpleBayesNetGenerator<float> gen(10, 20);
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();

      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0)
        delete bn;
    }

    void testGenerationFloat_2() {
      gum::SimpleBayesNetGenerator<float> gen(100, 500);
      ;

      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gen.generateBN(*bn);
      // Test for cicuits
      std::vector<gum::NodeId> stack;
      gum::Set<gum::NodeId> passed;
      const gum::DAG &dag = bn->dag();

      for (const auto node : dag.nodes()) {
        TS_ASSERT_THROWS(dag.directedPath(node, node), gum::NotFound);
      }

      if (bn != 0)
        delete bn;
    }

    void testGenerationFloat_3() {
      gum::SimpleBayesNetGenerator<float> gen(10, 45);

      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gen.generateBN(*bn);
      // Test for inference
      gum::ShaferShenoyInference<float> ssInf(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(ssInf.makeInference());

      if (bn != 0)
        delete bn;
    }

    void testGenerationFloat_4() {
      gum::SimpleBayesNetGenerator<float> gen(10, 35);

      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation<float> lazyInf(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());

      if (bn != 0)
        delete bn;
    }

    void testGenerationFloat_5() {
      gum::SimpleBayesNetGenerator<float> gen(10, 45);
      gum::BayesNet<float> *bn = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(bn = new gum::BayesNet<float>();
                                   gen.generateBN(*bn);
                                   gum::LazyPropagation<float> lazyInf(*bn);
                                   lazyInf.makeInference(); delete bn);
      TS_GUM_ASSERT_THROWS_NOTHING(bn = new gum::BayesNet<float>();
                                   gen.generateBN(*bn);
                                   gum::LazyPropagation<float> lazyInf(*bn);
                                   lazyInf.makeInference(); delete bn);
      TS_GUM_ASSERT_THROWS_NOTHING(bn = new gum::BayesNet<float>();
                                   gen.generateBN(*bn);
                                   gum::LazyPropagation<float> lazyInf(*bn);
                                   lazyInf.makeInference(); delete bn);
    }

    void testGenerationDouble_1() {
      gum::SimpleBayesNetGenerator<double> gen(10, 35);
      gum::BayesNet<double> *bn = new gum::BayesNet<double>();

      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0)
        delete bn;
    }

    void testGenerationDouble_2() {

      // Test for cicuits
      gum::SimpleBayesNetGenerator<double> gen(10, 45);
      gum::BayesNet<double> *bn = new gum::BayesNet<double>();
      gen.generateBN(*bn);

      std::vector<gum::NodeId> stack;
      gum::Set<gum::NodeId> passed;
      const gum::DAG &dag = bn->dag();

      for (const auto node : dag.nodes()) {
        TS_ASSERT_THROWS(dag.directedPath(node, node), gum::NotFound);
      }

      if (bn != 0)
        delete bn;
    }

    void testGenerationDouble_3() {
      gum::SimpleBayesNetGenerator<double> gen(10, 45);
      gum::BayesNet<double> *bn = new gum::BayesNet<double>();
      gen.generateBN(*bn);
      // Test for inference
      gum::ShaferShenoyInference<double> ssInf(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(ssInf.makeInference());

      if (bn != 0)
        delete bn;
    }

    void testGenerationDouble_4() {
      gum::SimpleBayesNetGenerator<double> gen(10, 45);
      gum::BayesNet<double> *bn = new gum::BayesNet<double>();
      gen.generateBN(*bn);

      // Test for inference
      gum::LazyPropagation<double> lazyInf(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());

      if (bn != 0)
        delete bn;
    }
  };
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
