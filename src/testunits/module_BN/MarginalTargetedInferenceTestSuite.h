/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
#include <string>

#include <agrum/tools/core/math/math_utils.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/variableElimination.h>
#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class MarginalTargetedInferenceTestSuite: public CxxTest::TestSuite {
    public:
    void testAddTarget() {
      auto bn
         = gum::BayesNet< double >::fastPrototype("A->B->C->D;A->E->D;F->B;C->H;");

      gum::LazyPropagation< double > lazy(&bn);
      TS_ASSERT(lazy.targets() == gum::NodeSet({0, 1, 2, 3, 4, 5, 6}));
      lazy.addTarget("A");
      TS_ASSERT(lazy.targets() == gum::NodeSet({0}));
      lazy.addTarget("B");
      TS_ASSERT(lazy.targets() == gum::NodeSet({0, 1}));

      gum::ShaferShenoyInference< double > shafer(&bn);
      TS_ASSERT(shafer.targets() == gum::NodeSet({0, 1, 2, 3, 4, 5, 6}));
      shafer.addTarget("A");
      TS_ASSERT(shafer.targets() == gum::NodeSet({0}));
      shafer.addTarget("B");
      TS_ASSERT(shafer.targets() == gum::NodeSet({0, 1}));

      gum::VariableElimination< double > ve(&bn);
      TS_ASSERT(ve.targets() == gum::NodeSet({0, 1, 2, 3, 4, 5, 6}));
      ve.addTarget("A");
      TS_ASSERT(ve.targets() == gum::NodeSet({0}));
      ve.addTarget("B");
      TS_ASSERT(ve.targets() == gum::NodeSet({0, 1}));
    }
  };
}   // namespace gum_tests
