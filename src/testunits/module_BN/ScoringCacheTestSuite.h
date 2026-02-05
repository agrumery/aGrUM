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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/stattests/scoringCache.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  ScoringCache
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct ScoringCacheTestSuite {
    public:
    static void test1() {
      gum::learning::IdCondSet    set1;
      gum::learning::ScoringCache cache;

      cache.insert(set1, 2.0);
      CHECK(cache.exists(set1));
      CHECK((cache.score(set1)) == (2.0));

      gum::NodeId node0 = 0;
      gum::NodeId node1 = 1;
      gum::NodeId node2 = 2;
      gum::NodeId node3 = 3;
      gum::NodeId node4 = 4;

      std::vector< gum::NodeId > empty;

      gum::learning::IdCondSet set2(node0, empty);
      cache.insert(set2, 3.2);
      CHECK(cache.exists(set2));
      CHECK((cache.score(set2)) == (3.2));

      CHECK_THROWS_AS(cache.insert(set2, 5.2), const gum::DuplicateElement&);
      CHECK((cache.size()) == (std::size_t(2)));
      cache.clear();
      CHECK((cache.size()) == (std::size_t(0)));

      gum::learning::IdCondSet xset2(set2);
      cache.insert(std::move(xset2), 7.3);
      CHECK(cache.exists(set2));
      CHECK((cache.score(set2)) == (7.3));

      std::vector< gum::NodeId > vect{node4, node2, node3};
      gum::learning::IdCondSet   set3(node0, node1, vect, true, true);
      CHECK(!cache.exists(set3));
      cache.insert(set3, 1.3);
      CHECK(cache.exists(set3));
      CHECK((cache.score(set3)) == (1.3));
      CHECK((cache.score(set2)) == (7.3));
      CHECK((cache.size()) == (std::size_t(2)));

      gum::learning::ScoringCache cache2(cache);
      CHECK(cache2.exists(set3));
      CHECK((cache2.score(set3)) == (1.3));
      CHECK((cache2.score(set2)) == (7.3));
      CHECK((cache2.size()) == (std::size_t(2)));

      cache2.erase(set2);
      CHECK(cache2.exists(set3));
      CHECK(!cache2.exists(set2));
      CHECK((cache2.score(set3)) == (1.3));
      CHECK_THROWS_AS(cache2.score(set2), const gum::NotFound&);
      CHECK((cache2.size()) == (std::size_t(1)));
      CHECK(cache.exists(set3));
      CHECK((cache.score(set3)) == (1.3));
      CHECK((cache.score(set2)) == (7.3));
      CHECK((cache.size()) == (std::size_t(2)));

      gum::learning::ScoringCache cache3(std::move(cache2));
      CHECK(cache3.exists(set3));
      CHECK(!cache3.exists(set2));
      CHECK((cache3.score(set3)) == (1.3));
      CHECK_THROWS_AS(cache3.score(set2), const gum::NotFound&);
      CHECK((cache3.size()) == (std::size_t(1)));
      CHECK(cache.exists(set3));
      CHECK((cache.score(set3)) == (1.3));
      CHECK((cache.score(set2)) == (7.3));
      CHECK((cache.size()) == (std::size_t(2)));

      gum::learning::ScoringCache* cache4 = cache3.clone();
      CHECK(cache4->exists(set3));
      CHECK(!cache4->exists(set2));
      CHECK((cache4->score(set3)) == (1.3));
      CHECK_THROWS_AS(cache4->score(set2), const gum::NotFound&);
      CHECK((cache4->size()) == (std::size_t(1)));
      CHECK(cache.exists(set3));
      CHECK((cache.score(set3)) == (1.3));
      CHECK((cache.score(set2)) == (7.3));
      CHECK((cache.size()) == (std::size_t(2)));

      cache3 = cache;
      CHECK(cache3.exists(set3));
      CHECK((cache3.score(set3)) == (1.3));
      CHECK((cache3.score(set2)) == (7.3));
      CHECK((cache3.size()) == (std::size_t(2)));

      gum::learning::IdCondSet set4(node1, vect, true);
      GUM_CHECK_ASSERT_THROWS_NOTHING(cache3.erase(set4));
      CHECK((cache3.score(set3)) == (1.3));
      CHECK((cache3.score(set2)) == (7.3));
      CHECK((cache3.size()) == (std::size_t(2)));

      *cache4 = std::move(cache3);
      CHECK(cache4->exists(set3));
      CHECK((cache4->score(set3)) == (1.3));
      CHECK((cache4->score(set2)) == (7.3));
      CHECK((cache4->size()) == (std::size_t(2)));

      delete cache4;
    }   // namespace gum_tests
  };

  GUM_TEST_ACTIF(1)

} /* namespace gum_tests */
