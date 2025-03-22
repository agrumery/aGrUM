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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/variableElimination.h>

#include <agrum/base/core/math/math_utils.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class [[maybe_unused]] MarginalTargetedInferenceTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(AddTarget) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D;A->E->D;F->B;C->H;");

      gum::LazyPropagation< double > lazy(&bn);
      TS_ASSERT_EQUALS(lazy.targets(), gum::NodeSet({0, 1, 2, 3, 4, 5, 6}))
      lazy.addTarget("A");
      TS_ASSERT_EQUALS(lazy.targets(), gum::NodeSet({0}))
      lazy.addTarget("B");
      TS_ASSERT_EQUALS(lazy.targets(), gum::NodeSet({0, 1}))

      gum::ShaferShenoyInference< double > shafer(&bn);
      TS_ASSERT_EQUALS(shafer.targets(), gum::NodeSet({0, 1, 2, 3, 4, 5, 6}))
      shafer.addTarget("A");
      TS_ASSERT_EQUALS(shafer.targets(), gum::NodeSet({0}))
      shafer.addTarget("B");
      TS_ASSERT_EQUALS(shafer.targets(), gum::NodeSet({0, 1}))

      gum::VariableElimination< double > ve(&bn);
      TS_ASSERT_EQUALS(ve.targets(), gum::NodeSet({0, 1, 2, 3, 4, 5, 6}))
      ve.addTarget("A");
      TS_ASSERT_EQUALS(ve.targets(), gum::NodeSet({0}))
      ve.addTarget("B");
      TS_ASSERT_EQUALS(ve.targets(), gum::NodeSet({0, 1}))
    }
  };
}   // namespace gum_tests
