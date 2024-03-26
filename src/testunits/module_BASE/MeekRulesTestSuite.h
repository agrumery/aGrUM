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

#include <agrum/tools/graphs/algorithms/MeekRules.h>
#include <agrum/tools/graphs/PDAG.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  class MeekRulesTestSuite: public CxxTest::TestSuite {
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

      const auto res_cpdag = mr.propagateToCPDAG(pdag);
      const auto res_dag   = mr.propagateToDAG(pdag);

      TS_ASSERT(res_cpdag.existsArc(2, 3));
      TS_ASSERT_EQUALS(res_cpdag.edges().size(), 0u);
    }

    GUM_ACTIVE_TEST(critereMinParents1) {
      gum::MixedGraph graph;
      gum::MixedGraph graph2;
      for (unsigned int i = 0; i < 7; ++i) {
        graph.addNodeWithId(i);
      }
      graph.addArc(0, 3);
      graph.addArc(1, 3);
      graph.addArc(2, 3);
      graph.addArc(5, 4);
      graph.addArc(6, 4);

      graph.addEdge(5, 4);

      gum::MeekRules mr;

      const auto res_cpdag = mr.propagateToCPDAG(graph);

      TS_ASSERT(res_cpdag.existsArc(5, 4));
    }
  };   // MeekrulesTestSuite
}   // namespace gum_tests
