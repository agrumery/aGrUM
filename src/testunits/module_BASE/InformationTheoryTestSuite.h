/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>
#include <vector>
#include <string>
#include "cxxtest/TestSuite.h"

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/MRF/inference/ShaferShenoyMRFInference.h>
#include <agrum/MRF/MarkovRandomField.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/tools/graphicalModels/algorithms/informationTheory.h>

namespace gum_tests {

  class [[maybe_unused]] InformationTheoryTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Constructor1) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");

      gum::LazyPropagation ie(&bn);

      gum::NodeSet X   = bn.nodeset({"A", "C"});
      gum::NodeSet Y   = bn.nodeset({"B"});
      auto         it  = gum::InformationTheory(ie, X, Y);
      auto         it2 = gum::InformationTheory(ie, {"A", "C"}, {"B"});
      TS_ASSERT_EQUALS(it.entropyX(), it2.entropyX())
      TS_ASSERT_EQUALS(it.entropyY(), it2.entropyY())

      TS_GUM_ASSERT_QUASI_EQUALS(
         it.expectedValueXY([](const gum::Instantiation&) -> auto { return 1.0; }),
         1.0)
    }

    GUM_ACTIVE_TEST(CheckSimpleBN) {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B");
      bn.cpt("A").fillWith({0.8, 0.2});
      bn.cpt("B").fillWith({0.1, 0.9, 0.3, 0.7});

      gum::LazyPropagation ie(&bn);

      auto it = gum::InformationTheory(ie, {"A"}, {"B"});

      TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyXY(), 1.27338275)
      TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyX(), 0.72192809)
      TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyY(), 0.58423881)
      TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyXgivenY(), 0.68914394)
      TS_GUM_ASSERT_ALMOST_EQUALS(it.mutualInformationXY(), 0.03278416)
    }

    GUM_ACTIVE_TEST(CheckSimpleBNwithEvidence) {
      const auto bn = gum::BayesNet< double >::fastPrototype("C->A->B");
      bn.cpt("C").fillWith({0.5, 0.5});
      bn.cpt("A").fillWith(
         {0.8, 0.2, 0.2, 0.8});   // so that bn|ev={"C"=0} is the same as the last test
      bn.cpt("B").fillWith({0.1, 0.9, 0.3, 0.7});

      gum::LazyPropagation ie(&bn);
      ie.addEvidence("C", 0);
      {
        auto it = gum::InformationTheory(ie, {"A"}, {"B"});

        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyXY(), 1.27338275)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyX(), 0.72192809)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyY(), 0.58423881)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyXgivenY(), 0.68914394)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyYgivenX(), 0.551455)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.mutualInformationXY(), 0.03278416)
      }
      ie.chgEvidence("C", 1);
      {
        auto it = gum::InformationTheory(ie, {"A"}, {"B"});

        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyXY(), 1.52075993298977)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyX(), 0.72192809)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyY(), 0.826746372492618)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyXgivenY(), 0.694013560497155)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyYgivenX(), 0.798832)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.mutualInformationXY(), 0.0279145343902076)
      }
    }

    GUM_ACTIVE_TEST(ShafShenCheckSimpleBNwithEvidence) {
      const auto bn = gum::BayesNet< double >::fastPrototype("C->A->B");
      bn.cpt("C").fillWith({0.5, 0.5});
      bn.cpt("A").fillWith(
         {0.8, 0.2, 0.2, 0.8});   // so that bn|ev={"C"=0} is the same as the last test
      bn.cpt("B").fillWith({0.1, 0.9, 0.3, 0.7});

      gum::ShaferShenoyInference ie(&bn);
      ie.addEvidence("C", 0);
      {
        auto it = gum::InformationTheory(ie, {"A"}, {"B"});

        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyXY(), 1.27338275)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyX(), 0.72192809)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyY(), 0.58423881)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyXgivenY(), 0.68914394)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyYgivenX(), 0.551455)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.mutualInformationXY(), 0.03278416)
      }
      ie.chgEvidence("C", 1);
      {
        auto it = gum::InformationTheory(ie, {"A"}, {"B"});

        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyXY(), 1.52075993298977)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyX(), 0.72192809)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyY(), 0.826746372492618)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyXgivenY(), 0.694013560497155)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.entropyYgivenX(), 0.798832)
        TS_GUM_ASSERT_ALMOST_EQUALS(it.mutualInformationXY(), 0.0279145343902076)
      }
    }


    GUM_ACTIVE_TEST(CheckConsistency) {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");

      gum::LazyPropagation ie(&bn);

      auto it = gum::InformationTheory(ie, {"A", "C"}, {"B"});
      check_this_information_theory(it);
    }
    
    GUM_ACTIVE_TEST(ShafShenCheckConsistency) {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");

      gum::ShaferShenoyInference ie(&bn);

      auto it = gum::InformationTheory(ie, {"A", "C"}, {"B"});
      check_this_information_theory(it);
    }

    GUM_ACTIVE_TEST(MRFCheckConsistency) {
      const auto mrf = gum::MarkovRandomField< double >::fastPrototype("A--B--C;A--B");

      gum::ShaferShenoyMRFInference ie(&mrf);

      auto it = gum::InformationTheory(ie, {"A", "C"}, {"B"});
      check_this_information_theory(it);
    }

    private:
    template < class IT >
    static void check_this_information_theory(IT it) {
      // H(X|Y)=H(X,Y)-H(Y)
      TS_GUM_ASSERT_QUASI_EQUALS(it.entropyXgivenY(), it.entropyXY() - it.entropyY())


      // I(X,Y)=H(X)-H(X|Y)=H(Y)-H(Y|X)
      TS_GUM_ASSERT_QUASI_EQUALS(it.mutualInformationXY(), it.entropyX() - it.entropyXgivenY())
      TS_GUM_ASSERT_QUASI_EQUALS(it.mutualInformationXY(), it.entropyY() - it.entropyYgivenX())

      // I(X,Y)=H(X)+H(Y)-H(X,Y)
      TS_GUM_ASSERT_QUASI_EQUALS(it.mutualInformationXY(),
                                 it.entropyX() + it.entropyY() - it.entropyXY())

      // I(X,Y)=H(X,Y)-H(X|Y)-H(Y|X)
      TS_GUM_ASSERT_QUASI_EQUALS(it.mutualInformationXY(),
                                 it.entropyXY() - it.entropyXgivenY() - it.entropyYgivenX())
    }
  };
}   // namespace gum_tests
