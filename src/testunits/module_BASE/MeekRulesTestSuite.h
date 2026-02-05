/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <iostream>

#include <agrum/base/graphs/algorithms/MeekRules.h>
#include <agrum/base/graphs/PDAG.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MeekRules
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct MeekRulesTestSuite {
    public:
    static void testtest_cycle_PDAG() {
      gum::PDAG pdag;
      for (unsigned int i = 0; i < 6; ++i) {
        pdag.addNodeWithId(i);
      }
      GUM_CHECK_ASSERT_THROWS_NOTHING(pdag.addArc(0, 1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(pdag.addArc(1, 2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(pdag.addEdge(2, 3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(pdag.addEdge(3, 5));
      GUM_CHECK_ASSERT_THROWS_NOTHING(pdag.addEdge(5, 4));

      gum::MeekRules mr;

      const auto res_cpdag = mr.propagateToCPDAG(pdag);
      const auto res_dag   = mr.propagateToDAG(pdag);

      CHECK(res_cpdag.existsArc(2, 3));
      CHECK((res_cpdag.edges().size()) == (0u));
    }

    static void testcritereMinParents1() {
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

      CHECK(res_cpdag.existsArc(5, 4));
    }
  };   // MeekrulesTestSuite

  GUM_TEST_ACTIF(test_cycle_PDAG)
  GUM_TEST_ACTIF(critereMinParents1)
}   // namespace gum_tests
