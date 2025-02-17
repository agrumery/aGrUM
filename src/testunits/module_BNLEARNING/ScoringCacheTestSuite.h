/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/stattests/scoringCache.h>

namespace gum_tests {

  class [[maybe_unused]] ScoringCacheTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(1) {
      gum::learning::IdCondSet    set1;
      gum::learning::ScoringCache cache;

      cache.insert(set1, 2.0);
      TS_ASSERT(cache.exists(set1))
      TS_ASSERT_EQUALS(cache.score(set1), 2.0)

      gum::NodeId node0 = 0;
      gum::NodeId node1 = 1;
      gum::NodeId node2 = 2;
      gum::NodeId node3 = 3;
      gum::NodeId node4 = 4;

      std::vector< gum::NodeId > empty;

      gum::learning::IdCondSet set2(node0, empty);
      cache.insert(set2, 3.2);
      TS_ASSERT(cache.exists(set2))
      TS_ASSERT_EQUALS(cache.score(set2), 3.2)

      TS_ASSERT_THROWS(cache.insert(set2, 5.2), const gum::DuplicateElement&)
      TS_ASSERT_EQUALS(cache.size(), std::size_t(2))
      cache.clear();
      TS_ASSERT_EQUALS(cache.size(), std::size_t(0))

      gum::learning::IdCondSet xset2(set2);
      cache.insert(std::move(xset2), 7.3);
      TS_ASSERT(cache.exists(set2))
      TS_ASSERT_EQUALS(cache.score(set2), 7.3)

      std::vector< gum::NodeId > vect{node4, node2, node3};
      gum::learning::IdCondSet   set3(node0, node1, vect, true, true);
      TS_ASSERT(!cache.exists(set3))
      cache.insert(set3, 1.3);
      TS_ASSERT(cache.exists(set3))
      TS_ASSERT_EQUALS(cache.score(set3), 1.3)
      TS_ASSERT_EQUALS(cache.score(set2), 7.3)
      TS_ASSERT_EQUALS(cache.size(), std::size_t(2))

      gum::learning::ScoringCache cache2(cache);
      TS_ASSERT(cache2.exists(set3))
      TS_ASSERT_EQUALS(cache2.score(set3), 1.3)
      TS_ASSERT_EQUALS(cache2.score(set2), 7.3)
      TS_ASSERT_EQUALS(cache2.size(), std::size_t(2))

      cache2.erase(set2);
      TS_ASSERT(cache2.exists(set3))
      TS_ASSERT(!cache2.exists(set2))
      TS_ASSERT_EQUALS(cache2.score(set3), 1.3)
      TS_ASSERT_THROWS(cache2.score(set2), const gum::NotFound&)
      TS_ASSERT_EQUALS(cache2.size(), std::size_t(1))
      TS_ASSERT(cache.exists(set3))
      TS_ASSERT_EQUALS(cache.score(set3), 1.3)
      TS_ASSERT_EQUALS(cache.score(set2), 7.3)
      TS_ASSERT_EQUALS(cache.size(), std::size_t(2))

      gum::learning::ScoringCache cache3(std::move(cache2));
      TS_ASSERT(cache3.exists(set3))
      TS_ASSERT(!cache3.exists(set2))
      TS_ASSERT_EQUALS(cache3.score(set3), 1.3)
      TS_ASSERT_THROWS(cache3.score(set2), const gum::NotFound&)
      TS_ASSERT_EQUALS(cache3.size(), std::size_t(1))
      TS_ASSERT(cache.exists(set3))
      TS_ASSERT_EQUALS(cache.score(set3), 1.3)
      TS_ASSERT_EQUALS(cache.score(set2), 7.3)
      TS_ASSERT_EQUALS(cache.size(), std::size_t(2))

      gum::learning::ScoringCache* cache4 = cache3.clone();
      TS_ASSERT(cache4->exists(set3))
      TS_ASSERT(!cache4->exists(set2))
      TS_ASSERT_EQUALS(cache4->score(set3), 1.3)
      TS_ASSERT_THROWS(cache4->score(set2), const gum::NotFound&)
      TS_ASSERT_EQUALS(cache4->size(), std::size_t(1))
      TS_ASSERT(cache.exists(set3))
      TS_ASSERT_EQUALS(cache.score(set3), 1.3)
      TS_ASSERT_EQUALS(cache.score(set2), 7.3)
      TS_ASSERT_EQUALS(cache.size(), std::size_t(2))

      cache3 = cache;
      TS_ASSERT(cache3.exists(set3))
      TS_ASSERT_EQUALS(cache3.score(set3), 1.3)
      TS_ASSERT_EQUALS(cache3.score(set2), 7.3)
      TS_ASSERT_EQUALS(cache3.size(), std::size_t(2))

      gum::learning::IdCondSet set4(node1, vect, true);
      TS_GUM_ASSERT_THROWS_NOTHING(cache3.erase(set4))
      TS_ASSERT_EQUALS(cache3.score(set3), 1.3)
      TS_ASSERT_EQUALS(cache3.score(set2), 7.3)
      TS_ASSERT_EQUALS(cache3.size(), std::size_t(2))

      *cache4 = std::move(cache3);
      TS_ASSERT(cache4->exists(set3))
      TS_ASSERT_EQUALS(cache4->score(set3), 1.3)
      TS_ASSERT_EQUALS(cache4->score(set2), 7.3)
      TS_ASSERT_EQUALS(cache4->size(), std::size_t(2))

      delete cache4;
    }
  };


} /* namespace gum_tests */
