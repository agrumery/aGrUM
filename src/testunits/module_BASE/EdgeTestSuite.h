/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/graphs/graphElements.h>

namespace gum_tests {

  class EdgeTestSuite : public CxxTest::TestSuite {
    public:
    void testConstructor1() {
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Edge edge1(1, 2));
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Edge edge1(1, 1));
    }

    void testConstructor2() {
      gum::Edge edge1(1, 2);
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Edge copy(edge1));
    }

    void testEquality() {
      gum::Edge edge1(1, 2);
      gum::Edge edge2(2, 1);
      gum::Edge copy(edge1);
      gum::Edge edge3(6, 7);

      TS_ASSERT_EQUALS(edge1, edge1);
      TS_ASSERT_EQUALS(edge1, edge2);
      TS_ASSERT_EQUALS(edge2, edge1);
      TS_ASSERT_EQUALS(copy, edge1);
      TS_ASSERT_EQUALS(edge2, copy);

      TS_ASSERT(edge1 != edge3);
      TS_ASSERT(edge3 != edge1);
      TS_ASSERT(copy != edge3);
      TS_ASSERT(edge2 != edge3);
    }

    void testGetters() {
      gum::Edge edge1((gum::NodeId)1, (gum::NodeId)2);
      gum::Edge edge2((gum::NodeId)2, (gum::NodeId)1);
      gum::Edge edge3((gum::NodeId)3, (gum::NodeId)4);

      gum::Edge copy(edge1);

      TS_GUM_ASSERT_THROWS_NOTHING(edge1.first());
      TS_GUM_ASSERT_THROWS_NOTHING(edge1.second());

      TS_GUM_ASSERT_THROWS_NOTHING(copy.first());
      TS_GUM_ASSERT_THROWS_NOTHING(copy.second());

      TS_ASSERT_EQUALS(edge2.first(), (gum::NodeId)1);
      TS_ASSERT_EQUALS(edge2.second(), (gum::NodeId)2);

      TS_GUM_ASSERT_THROWS_NOTHING(edge3.other((gum::NodeId)3));
      TS_GUM_ASSERT_THROWS_NOTHING(edge3.other((gum::NodeId)4));

      TS_ASSERT_EQUALS(edge3.other((gum::NodeId)3), (gum::NodeId)4);
      TS_ASSERT_EQUALS(edge3.other((gum::NodeId)4), (gum::NodeId)3);

      TS_ASSERT_THROWS_ANYTHING(copy.other((gum::NodeId)42));
    }

    void testIsDirected() {
      gum::Edge edge((gum::NodeId)1, (gum::NodeId)2);
      TS_ASSERT(!edge.isDirected());
    }

    void testHash() {
      gum::HashTable<gum::Edge, unsigned long> hash;

      for (gum::NodeId x = 0; x < 1000; ++x) {
        for (gum::NodeId y = x; y < 1000; ++y) {
          gum::Edge edge(x, y);
          hash.insert(edge, x + y);
        }
      }

      for (gum::NodeId x = 0; x < 1000; ++x) {
        for (gum::NodeId y = x; y < 1000; ++y) {
          gum::Edge edge(x, y);
          TS_ASSERT(hash[edge] == (x + y));
        }
      }
    }
  };
}
