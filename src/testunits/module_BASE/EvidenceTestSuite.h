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
      TS_ASSERT_DELTA(10 - 1, (~p1).sum(), TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(Likelihood) {
      const auto bn
          = gum::BayesNet< double >::fastPrototype("A[10];B[1,10];C{1.0:20.0:10};D{1:100:10};E[1.0:"
                                                   "20.0:10];X{A1|A2|A3|A4|A5|A6|A7|A8|A9|A10}");
      for (auto i: bn.nodes()) {
        TS_ASSERT_EQUALS(bn.variable(i).domainSize(), 10)
      }


      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evEq(bn.variable("A"), 4.0)),
                       "....1.....")
      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evEq(bn.variable("B"), 4.0)),
                       "...1......")
      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evEq(bn.variable("C"), 4.0)),
                       ".1........")
      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evEq(bn.variable("D"), 4.0)),
                       "1.........")
      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evEq(bn.variable("E"), 4.0)),
                       ".1........")

      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evEq(bn.variable("A"), 4.0)
                                    | gum::Potential< double >::evEq(bn.variable("A"), 8.0)),
                       "....1...1.")
      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evIn(bn.variable("A"), 4.0, 8.0)),
                       "....11111.")
      TS_ASSERT_EQUALS(toBoolString(~gum::Potential< double >::evIn(bn.variable("A"), 4.0, 8.0)),
                       "1111.....1")

      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evLt(bn.variable("A"), 4.0)),
                       "1111......")
      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evLt(bn.variable("A"), 0.0)),
                       "1.........")

      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evGt(bn.variable("A"), 4.0)),
                       ".....11111")
      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evGt(bn.variable("A"), 8.0)),
                       ".........1")

      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evLt(bn.variable("C"), 7.0)),
                       "111.......")
      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evLt(bn.variable("C"), 1.0)),
                       "1.........")

      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evGt(bn.variable("C"), 7.0)),
                       "....111111")
      TS_ASSERT_EQUALS(toBoolString(gum::Potential< double >::evGt(bn.variable("C"), 20.0)),
                       ".........1")
    }

    private:
    std::string toBoolString(const gum::Potential< double >& p) {
      std::string        s = "";
      gum::Instantiation I(p);
      for (I.setFirst(); !I.end(); I.inc())
        s += p.get(I) == 0 ? "." : "1";
      return s;
    }
  };
}   // namespace gum_tests
