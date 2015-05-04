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

#include <agrum/BN/generator/MCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/core/set.h>
#include <agrum/core/exceptions.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>

namespace gum_tests {

  class MCBayesNetGeneratorTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testCreationDeletionFloat() {
      gum::MCBayesNetGenerator<float> *gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MCBayesNetGenerator<float>(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionFloatCPT() {
      gum::MCBayesNetGenerator<float> *gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MCBayesNetGenerator<float>(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionDouble() {
      gum::MCBayesNetGenerator<double> *gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MCBayesNetGenerator<double>(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionDoubleCPT() {
      gum::MCBayesNetGenerator<double> *gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MCBayesNetGenerator<double>(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testGenerationBNFloat() {

      gum::MCBayesNetGenerator<float> gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0)
        delete bn;
    }

    void testGenerationBNDouble() {

      gum::MCBayesNetGenerator<double> gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet<double> *bn = new gum::BayesNet<double>();
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0)
        delete bn;
    }

    void testGenerationfromBNFloat() {

      gum::MCBayesNetGenerator<float> gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::MCBayesNetGenerator<float> gen2(*bn, 30, 40, 50));

      if (bn != 0)
        delete bn;
    }

    void testGenerationfromBNDouble() {

      gum::MCBayesNetGenerator<double> gen(30, 100, 5, 30, 40, 50);
      gum::BayesNet<double> *bn = new gum::BayesNet<double>();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::MCBayesNetGenerator<double> gen2(*bn, 30, 40, 50));

      if (bn != 0)
        delete bn;
    }

    void testDisturbBNFloatCPT() {

      gum::MCBayesNetGenerator<float> gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(*bn));

      if (bn != 0)
        delete bn;
    }

    void testGenerationBNDoubleCPT() {

      gum::MCBayesNetGenerator<double> gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet<double> *bn = new gum::BayesNet<double>();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(*bn));

      if (bn != 0)
        delete bn;
    }

    void testInferenceFloat() {

      gum::MCBayesNetGenerator<float> gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation<float> lazyInf(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());

      if (bn != 0)
        delete bn;
    }

    void testInferenceDouble() {

      gum::MCBayesNetGenerator<double> gen(30, 60, 3, 30, 40, 50);
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
