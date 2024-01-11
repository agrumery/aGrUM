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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/tools/core/approximations/approximationScheme.h>
#include <agrum/tools/core/approximations/approximationSchemeListener.h>
#include <agrum/tools/database/databaseTable.h>
#include <agrum/tools/database/DBInitializerFromCSV.h>
#include <agrum/tools/database/DBRowGeneratorIdentity.h>
#include <agrum/tools/database/DBRowGeneratorParser.h>
#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBTranslatorSet.h>
#include <agrum/tools/graphs/algorithms/MeekRules.h>
#include <agrum/tools/graphs/DAG.h>
#include <agrum/tools/graphs/mixedGraph.h>
#include <agrum/tools/graphs/PDAG.h>
#include <agrum/tools/variables/labelizedVariable.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>
#include <agrum/BN/learning/priors/noPrior.h>

namespace gum_tests {

  class MeekrulesTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(test_cycle_PDAG) {
      gum::PDAG pdag;
      for (unsigned int i = 0; i < 6; ++i) {
        pdag.addNodeWithId(i);
      }
      pdag.addArc(0, 1);
      pdag.addArc(1, 2);

      pdag.addEdge(2, 3);
      pdag.addEdge(3, 5);
      pdag.addEdge(5, 4);
      pdag.addEdge(2, 1);
      pdag.addEdge(4, 1);

      gum::MeekRules mr;

      const auto res1 = mr.propagatesToPDAG(pdag);
      const auto res2 = mr.propagatesToDAG(pdag);
    }
  };   // MeekrulesTestSuite
}   // namespace gum_tests
