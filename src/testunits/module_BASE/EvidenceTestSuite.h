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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/tools/multidim/potential.h>

#include <agrum/BN/BayesNet.h>

namespace gum_tests {

  class [[maybe_unused]] EvidenceTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(IsEvidence) {
      const auto bn = gum::BayesNet< double >::fastPrototype("A[10]->B[3]");
      TS_ASSERT(bn.cpt("A").isEvidence())
      TS_ASSERT(!((bn.cpt("B").isEvidence())))
      TS_ASSERT(!((bn.cpt("A").scale(100)).isEvidence()))
      TS_ASSERT(!((bn.cpt("A").translate(1)).isEvidence()))

      auto p = gum::Potential(bn.cpt("A"));
      p.fillWith(0);
      TS_ASSERT(!(p.isEvidence()))
      p.fillWith(1);
      TS_ASSERT((p.isEvidence()))
      p.random();
      TS_ASSERT((p.isEvidence()))
    }

    GUM_ACTIVE_TEST(CombineEvidence) {
      const auto bn = gum::BayesNet< double >::fastPrototype("A[10]->C;B[10]");
      auto       p1 = gum::Potential(bn.cpt("A"));
      auto       p2 = gum::Potential(bn.cpt("A"));
      p2.randomCPT();
      auto q = gum::Potential(bn.cpt("B"));

      TS_ASSERT_THROWS(p1 | q, gum::InvalidArgument)
      TS_ASSERT_THROWS(p1 & q, gum::InvalidArgument)
      TS_ASSERT_THROWS(p1 & bn.cpt("C"), gum::InvalidArgument)

      auto Mp1 = p1.max();
      auto Mp2 = p2.max();
      auto mp1 = p1.min();
      auto mp2 = p2.min();

      TS_ASSERT_LESS_THAN_EQUALS(Mp1, (p1 | p2).max())
      TS_ASSERT_LESS_THAN_EQUALS(Mp2, (p1 | p2).max())
      TS_ASSERT_EQUALS(std::max(Mp1, Mp2), (p1 | p2).max())
      TS_ASSERT_LESS_THAN_EQUALS((p1 & p2).min(), mp1)
      TS_ASSERT_LESS_THAN_EQUALS((p1 & p2).min(), mp2)
      TS_ASSERT_EQUALS(std::min(mp1, mp2), (p1 & p2).min())

      TS_ASSERT_LESS_THAN_EQUALS(1, (p1 | p2).sum())
      TS_ASSERT_LESS_THAN_EQUALS((p1 & p2).sum(), 1)

      TS_ASSERT_EQUALS(1 - p1.max(), (~p1).min())
      TS_ASSERT_EQUALS(1 - p1.min(), (~p1).max())
      GUM_TRACE_VAR(p1)
      TS_ASSERT_EQUALS(10 - 1, (~p1).sum())
    }
  };
}   // namespace gum_tests
