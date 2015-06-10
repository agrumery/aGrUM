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

#include <agrum/BN/generator/maxParentsMCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/core/set.h>
#include <agrum/core/exceptions.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>

namespace gum_tests {

  class MaxParentsMCBayesNetGeneratorTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testCreationDeletionFloat() {
      gum::MaxParentsMCBayesNetGenerator<float> *gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxParentsMCBayesNetGenerator<float>(30, 70, 3, 4, 30, 40,
                                                              50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionFloatCPT() {
      gum::MaxParentsMCBayesNetGenerator<float> *gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxParentsMCBayesNetGenerator<float>(30, 70, 4, 4, 20, 10,
                                                              30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionDouble() {
      gum::MaxParentsMCBayesNetGenerator<double> *gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxParentsMCBayesNetGenerator<double>(30, 70, 4, 4, 20, 10,
                                                               30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionDoubleCPT() {
      gum::MaxParentsMCBayesNetGenerator<double> *gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          gen = new gum::MaxParentsMCBayesNetGenerator<double>(30, 70, 4, 4, 20, 10,
                                                               30));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testGenerationBNFloat() {

      gum::MaxParentsMCBayesNetGenerator<float> gen(30, 70, 5, 4, 30, 40, 50);
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0)
        delete bn;
    }

    void testGenerationBNDouble() {

      gum::MaxParentsMCBayesNetGenerator<double> gen(30, 70, 5, 4, 30, 40, 50);
      gum::BayesNet<double> *bn = new gum::BayesNet<double>();
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(*bn));

      if (bn != 0)
        delete bn;
    }

    void testGenerationfromBNFloat() {

      gum::MaxParentsMCBayesNetGenerator<float> gen(30, 70, 10, 4, 30, 40, 50);
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::MaxParentsMCBayesNetGenerator<float> gen2(*bn, 4, 30, 40, 50));

      if (bn != 0)
        delete bn;
    }

    void testGenerationfromBNDouble() {

      gum::MaxParentsMCBayesNetGenerator<double> gen(30, 70, 5, 4, 30, 40, 50);
      gum::BayesNet<double> *bn = new gum::BayesNet<double>();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::MaxParentsMCBayesNetGenerator<double> gen2(*bn, 4, 30, 40, 50));

      if (bn != 0)
        delete bn;
    }

    void testDisturbBNFloatCPT() {

      gum::MaxParentsMCBayesNetGenerator<float> gen(30, 70, 4, 30, 40, 50);
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(*bn));

      if (bn != 0)
        delete bn;
    }

    void testGenerationBNDoubleCPT() {

      gum::MaxParentsMCBayesNetGenerator<double> gen(30, 70, 3, 4, 30, 40, 50);
      gum::BayesNet<double> *bn = new gum::BayesNet<double>();
      gen.generateBN(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(*bn));

      if (bn != 0)
        delete bn;
    }

    void testInferenceFloat() {

      gum::MaxParentsMCBayesNetGenerator<float> gen(30, 70, 8, 4, 30, 40, 50);
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gen.generateBN(*bn);
      // Test for inference
      gum::LazyPropagation<float> lazyInf(*bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());

      if (bn != 0)
        delete bn;
    }

    void testInferenceDouble() {

      gum::MaxParentsMCBayesNetGenerator<double> gen(30, 70, 8, 4, 30, 40, 50);
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
