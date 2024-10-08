/**
 *
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <agrum/tools/graphs/graphElements.h>

namespace gum_tests {

  class [[maybe_unused]] EdgeTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Constructor1){TS_GUM_ASSERT_THROWS_NOTHING(gum::Edge edge1(1, 2))
                                      TS_GUM_ASSERT_THROWS_NOTHING(gum::Edge edge1(1, 1))}

    GUM_ACTIVE_TEST(Constructor2) {
      gum::Edge edge1(1, 2);
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Edge copy(edge1))
    }

    GUM_ACTIVE_TEST(Equality) {
      gum::Edge edge1(1, 2);
      gum::Edge edge2(2, 1);
      gum::Edge copy(edge1);
      gum::Edge edge3(6, 7);

      TS_ASSERT_EQUALS(edge1, edge1)
      TS_ASSERT_EQUALS(edge1, edge2)
      TS_ASSERT_EQUALS(edge2, edge1)
      TS_ASSERT_EQUALS(copy, edge1)
      TS_ASSERT_EQUALS(edge2, copy)

      TS_ASSERT_DIFFERS(edge1, edge3)
      TS_ASSERT_DIFFERS(edge3, edge1)
      TS_ASSERT_DIFFERS(copy, edge3)
      TS_ASSERT_DIFFERS(edge2, edge3)
    }

    GUM_ACTIVE_TEST(Getters) {
      const gum::Edge edge1((gum::NodeId)1, (gum::NodeId)2);
      const gum::Edge edge2((gum::NodeId)2, (gum::NodeId)1);

      TS_ASSERT_EQUALS(edge1, edge1)
      TS_ASSERT_EQUALS(edge1, edge2)
      TS_ASSERT_EQUALS(edge2, edge1)

      const gum::Edge edge3((gum::NodeId)3, (gum::NodeId)4);
      const gum::Edge edge4((gum::NodeId)1, (gum::NodeId)2);

      gum::NodeId n;

      TS_GUM_ASSERT_THROWS_NOTHING(n = edge1.first())
      TS_GUM_ASSERT_THROWS_NOTHING(n = edge1.second())

      TS_GUM_ASSERT_THROWS_NOTHING(n = edge4.first())
      TS_GUM_ASSERT_THROWS_NOTHING(n = edge4.second())

      TS_ASSERT_EQUALS(n = edge2.first(), (gum::NodeId)1)
      TS_ASSERT_EQUALS(n = edge2.second(), (gum::NodeId)2)

      TS_GUM_ASSERT_THROWS_NOTHING(n = edge3.other((gum::NodeId)3))
      TS_GUM_ASSERT_THROWS_NOTHING(n = edge3.other((gum::NodeId)4))

      TS_ASSERT_EQUALS(n = edge3.other((gum::NodeId)3), (gum::NodeId)4)
      TS_ASSERT_EQUALS(n = edge3.other((gum::NodeId)4), (gum::NodeId)3)

      TS_ASSERT_THROWS_ANYTHING(n = edge4.other((gum::NodeId)42))
    }

    GUM_ACTIVE_TEST(Hash) {
      gum::HashTable< gum::Edge, gum::NodeId > hash;

      for (gum::NodeId x = 0; x < 1000; ++x) {
        for (gum::NodeId y = x; y < 1000; ++y) {
          gum::Edge edge(x, y);
          hash.insert(edge, x + y);
        }
      }

      for (gum::NodeId x = 0; x < 1000; ++x) {
        for (gum::NodeId y = x; y < 1000; ++y) {
          gum::Edge edge(x, y);
          TS_ASSERT_EQUALS(hash[edge], (x + y))
        }
      }
    }
  };
}   // namespace gum_tests
