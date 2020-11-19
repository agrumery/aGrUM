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


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/tools/graphs/graphElements.h>

namespace gum_tests {

  class ArcTestSuite: public CxxTest::TestSuite {
    public:
    void testConstructor1() {
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Arc arc1(1, 2));
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Arc arc1(1, 1));
    }

    void testConstructor2() {
      gum::Arc arc1(1, 2);
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Arc copy(arc1));
    }

    void testEquality() {
      gum::Arc arc1(1, 2);
      gum::Arc arc2(2, 1);
      gum::Arc copy(arc1);
      gum::Arc arc3(6, 7);

      TS_ASSERT_EQUALS(arc1, arc1);
      TS_ASSERT_EQUALS(copy, arc1);
      TS_ASSERT_EQUALS(arc1, copy);

      TS_ASSERT(arc1 != arc3);
      TS_ASSERT(arc3 != arc1);
      TS_ASSERT(copy != arc3);
      TS_ASSERT(arc2 != arc3);
      TS_ASSERT(arc1 != arc2);
    }

    void testGetters1() {
      gum::Arc arc1((gum::NodeId)1, (gum::NodeId)2);
      gum::Arc arc2((gum::NodeId)2, (gum::NodeId)1);
      gum::Arc arc3((gum::NodeId)3, (gum::NodeId)4);

      gum::Arc copy(arc1);

      TS_GUM_ASSERT_THROWS_NOTHING(arc1.first());
      TS_GUM_ASSERT_THROWS_NOTHING(arc1.second());

      TS_GUM_ASSERT_THROWS_NOTHING(copy.first());
      TS_GUM_ASSERT_THROWS_NOTHING(copy.second());

      TS_ASSERT_EQUALS(arc2.first(), (gum::NodeId)2);
      TS_ASSERT_EQUALS(arc2.second(), (gum::NodeId)1);

      TS_GUM_ASSERT_THROWS_NOTHING(arc3.other((gum::NodeId)3));
      TS_GUM_ASSERT_THROWS_NOTHING(arc3.other((gum::NodeId)4));

      TS_ASSERT_EQUALS(arc3.other((gum::NodeId)3), (gum::NodeId)4);
      TS_ASSERT_EQUALS(arc3.other((gum::NodeId)4), (gum::NodeId)3);

      TS_ASSERT_THROWS_ANYTHING(copy.other((gum::NodeId)42));
    }

    void testGetters2() {
      gum::Arc arc1(1, 2);

      TS_ASSERT_EQUALS(arc1.tail(), (gum::NodeId)1);
      TS_ASSERT_EQUALS(arc1.head(), (gum::NodeId)2);
    }

    void testHash() {
      gum::HashTable< gum::Arc, gum::NodeId > hash;

      for (gum::NodeId x = 0; x < 1000; ++x) {
        for (gum::NodeId y = x; y < 1000; ++y) {
          gum::Arc arc(x, y);
          hash.insert(arc, x + y);
        }
      }

      for (gum::NodeId x = 0; x < 1000; ++x) {
        for (gum::NodeId y = x; y < 1000; ++y) {
          gum::Arc arc(x, y);
          TS_ASSERT(hash[arc] == (x + y));
        }
      }
    }
  };
}   // namespace gum_tests
