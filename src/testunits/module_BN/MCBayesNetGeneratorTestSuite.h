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
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/generator/MCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/set.h>

namespace gum_tests {

  class MCBayesNetGeneratorTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testCreationDeletionFloat() {
      gum::MCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
        gen = new gum::MCBayesNetGenerator< double >(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionFloatCPT() {
      gum::MCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
        gen = new gum::MCBayesNetGenerator< double >(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionDouble() {
      gum::MCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
        gen = new gum::MCBayesNetGenerator< double >(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testCreationDeletionDoubleCPT() {
      gum::MCBayesNetGenerator< double >* gen = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
        gen = new gum::MCBayesNetGenerator< double >(30, 60, 3, 30, 40, 50));
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testGenerationBNFloat() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(bn));
    }

    void testGenerationBNDouble() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      TS_GUM_ASSERT_THROWS_NOTHING(gen.generateBN(bn));
    }

    void testGenerationfromBNFloat() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
        gum::MCBayesNetGenerator< double > gen2(bn, 30, 40, 50));
    }

    void testGenerationfromBNDouble() {
      gum::MCBayesNetGenerator< double > gen(30, 100, 5, 30, 40, 50);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(
        gum::MCBayesNetGenerator< double > gen2(bn, 30, 40, 50));
    }

    void testDisturbBNFloatCPT() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(bn));
    }

    void /*test*/ GenerationBNDoubleCPT() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(gen.disturbBN(bn));
    }

    void /*test*/ InferenceFloat() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());
    }

    void /*test*/ InferenceDouble() {
      gum::MCBayesNetGenerator< double > gen(30, 60, 3, 30, 40, 50);
      gum::BayesNet< double >            bn;
      gen.generateBN(bn);
      // Test for inference
      gum::LazyPropagation< double > lazyInf(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(lazyInf.makeInference());
    }
  };
}   // namespace gum_tests
