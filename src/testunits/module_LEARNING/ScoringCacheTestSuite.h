/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/learning/scores_and_tests/scoringCache.h>

namespace gum_tests {

  class ScoringCacheTestSuite : public CxxTest::TestSuite {
    public:
    void test1() {
      gum::learning::IdSet<>        set1;
      gum::learning::ScoringCache<> cache;

      cache.insert(set1, 2.0);
      TS_ASSERT(cache.exists(set1));
      TS_ASSERT(cache.score(set1) == 2.0);

      gum::NodeId node0 = 0;
      gum::NodeId node1 = 1;
      gum::NodeId node2 = 2;
      gum::NodeId node3 = 3;
      gum::NodeId node4 = 4;

      std::vector< gum::NodeId > empty;

      gum::learning::IdSet<> set2(node0, empty);
      cache.insert(set2, 3.2);
      TS_ASSERT(cache.exists(set2));
      TS_ASSERT(cache.score(set2) == 3.2);

      TS_ASSERT_THROWS(cache.insert(set2, 5.2), gum::DuplicateElement);
      TS_ASSERT(cache.size() == std::size_t(2));
      cache.clear();
      TS_ASSERT(cache.size() == std::size_t(0));

      gum::learning::IdSet<> xset2(set2);
      cache.insert(std::move(xset2), 7.3);
      TS_ASSERT(cache.exists(set2));
      TS_ASSERT(cache.score(set2) == 7.3);

      std::vector< gum::NodeId > vect{node4, node2, node3};
      gum::learning::IdSet<>     set3(node0, node1, vect, true, true);
      TS_ASSERT(!cache.exists(set3));
      cache.insert(set3, 1.3);
      TS_ASSERT(cache.exists(set3));
      TS_ASSERT(cache.score(set3) == 1.3);
      TS_ASSERT(cache.score(set2) == 7.3);
      TS_ASSERT(cache.size() == std::size_t(2));

      gum::learning::ScoringCache<> cache2(cache);
      TS_ASSERT(cache2.exists(set3));
      TS_ASSERT(cache2.score(set3) == 1.3);
      TS_ASSERT(cache2.score(set2) == 7.3);
      TS_ASSERT(cache2.size() == std::size_t(2));

      cache2.erase(set2);
      TS_ASSERT(cache2.exists(set3));
      TS_ASSERT(!cache2.exists(set2));
      TS_ASSERT(cache2.score(set3) == 1.3);
      TS_ASSERT_THROWS(cache2.score(set2), gum::NotFound);
      TS_ASSERT(cache2.size() == std::size_t(1));
      TS_ASSERT(cache.exists(set3));
      TS_ASSERT(cache.score(set3) == 1.3);
      TS_ASSERT(cache.score(set2) == 7.3);
      TS_ASSERT(cache.size() == std::size_t(2));

      gum::learning::ScoringCache<> cache3(std::move(cache2));
      TS_ASSERT(cache3.exists(set3));
      TS_ASSERT(!cache3.exists(set2));
      TS_ASSERT(cache3.score(set3) == 1.3);
      TS_ASSERT_THROWS(cache3.score(set2), gum::NotFound);
      TS_ASSERT(cache3.size() == std::size_t(1));
      TS_ASSERT(cache.exists(set3));
      TS_ASSERT(cache.score(set3) == 1.3);
      TS_ASSERT(cache.score(set2) == 7.3);
      TS_ASSERT(cache.size() == std::size_t(2));

      gum::learning::ScoringCache<>* cache4 = cache3.clone();
      TS_ASSERT(cache4->exists(set3));
      TS_ASSERT(!cache4->exists(set2));
      TS_ASSERT(cache4->score(set3) == 1.3);
      TS_ASSERT_THROWS(cache4->score(set2), gum::NotFound);
      TS_ASSERT(cache4->size() == std::size_t(1));
      TS_ASSERT(cache.exists(set3));
      TS_ASSERT(cache.score(set3) == 1.3);
      TS_ASSERT(cache.score(set2) == 7.3);
      TS_ASSERT(cache.size() == std::size_t(2));

      cache3 = cache;
      TS_ASSERT(cache3.exists(set3));
      TS_ASSERT(cache3.score(set3) == 1.3);
      TS_ASSERT(cache3.score(set2) == 7.3);
      TS_ASSERT(cache3.size() == std::size_t(2));

      gum::learning::IdSet<> set4(node1, vect, true);
      TS_GUM_ASSERT_THROWS_NOTHING(cache3.erase(set4));
      TS_ASSERT(cache3.score(set3) == 1.3);
      TS_ASSERT(cache3.score(set2) == 7.3);
      TS_ASSERT(cache3.size() == std::size_t(2));

      *cache4 = std::move(cache3);
      TS_ASSERT(cache4->exists(set3));
      TS_ASSERT(cache4->score(set3) == 1.3);
      TS_ASSERT(cache4->score(set2) == 7.3);
      TS_ASSERT(cache4->size() == std::size_t(2));

      delete cache4;
    }
  };


} /* namespace gum_tests */