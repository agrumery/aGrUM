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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/tools/graphicalModels/algorithms/informationTheory.h>

namespace gum_tests {

  class [[maybe_unused]] InformationTheoryTestSuite: public CxxTest::TestSuite {
    public:
    GUM_TEST(Constructor) {
      gum::NodeSet         X, Y;
      auto                 bn = gum::BayesNet< double >::fastPrototype("A->B");
      gum::LazyPropagation ie(&bn);
      auto                 ig = gum::InformationTheory(ie, X, Y);
    }
  };
}   // namespace gum_tests
