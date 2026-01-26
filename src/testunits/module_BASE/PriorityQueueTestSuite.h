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


#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/hashFunc.h>
#include <agrum/base/core/multiPriorityQueue.h>
#include <agrum/base/core/priorityQueue.h>
#include <agrum/base/core/set.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  PriorityQueue
#define GUM_CURRENT_MODULE GUMBASE

namespace gum {
  template <>
  class HashFunc< gum::Set< int > >: public HashFuncBase< gum::Set< int > > {
    public:
    /// computes the hashed value of a key
    Size operator()(const gum::Set< int >& key) const {
      return ((key.size() * gum::HashFuncConst::gold) & hash_mask_);
    }
  };
}   // namespace gum

namespace gum_tests {

  struct PriorityQueueTestSuite {
    public:
    static void testConstruct() {
      gum::PriorityQueue< std::string > queue1;

      CHECK((queue1.size()) == (static_cast< gum::Size >(0)));
      CHECK((queue1.empty()) == (true));
      CHECK((queue1.contains("ZZZ")) == (false));
      CHECK_THROWS_AS(queue1.top(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.pop(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.topPriority(), const gum::NotFound&);

      queue1.insert("AAA", 8);
      queue1.insert("BBB", 10);
      queue1.insert("CCC", 2);
      queue1.emplace("DDD", 23);
      std::string xxx = "EEE";
      int         nb  = 24;
      queue1.insert(xxx, nb);

      CHECK((queue1.size()) == (static_cast< gum::Size >(5)));
      CHECK((queue1.empty()) == (false));
      CHECK((queue1.contains("AAA")) == (true));
      CHECK((queue1.contains("ZZZ")) == (false));

      const std::string& str = queue1.top();
      CHECK((str) == ("CCC"));
      CHECK((queue1.topPriority()) == (2));

      std::string str2 = queue1.pop();
      CHECK((str2) == ("CCC"));
      CHECK((queue1.top()) == ("AAA"));
      CHECK((queue1.topPriority()) == (8));

      queue1.setPriorityByPos(0, 4);
      CHECK((queue1.top()) == ("AAA"));
      CHECK((queue1.topPriority()) == (4));
      CHECK_THROWS_AS(queue1.setPriorityByPos(10, 4), const gum::NotFound&);

      queue1.setPriority("BBB", 100);
      nb = 2;
      queue1.setPriority("BBB", nb);
      CHECK((queue1.top()) == ("BBB"));
      CHECK((queue1.topPriority()) == (2));
      CHECK_THROWS_AS(queue1.setPriority("zzz", 4), const gum::NotFound&);

      CHECK_THROWS_AS(queue1.priority("zzz"), const gum::NotFound&);
      CHECK((queue1.priority("BBB")) == (2));

      queue1.resize(10);
      CHECK((queue1.capacity()) == (static_cast< gum::Size >(10)));

      const gum::HashTable< std::string, gum::Size >& vals = queue1.allValues();
      CHECK((vals.size()) == (static_cast< gum::Size >(4)));

      gum::PriorityQueue< std::string > queue10{std::pair< std::string, int >("aa", 3),
                                                std::pair< std::string, int >("bb", 2)};
      CHECK((queue10.size()) == (static_cast< gum::Size >(2)));
      CHECK((queue10.top()) == ("bb"));
    }   // namespace gum_tests

    static void testMoveScalar() {
      gum::PriorityQueue< int > queue1{std::pair< int, int >(1, 3),
                                       std::pair< int, int >(2, 1),
                                       std::pair< int, int >(3, 10)};

      gum::PriorityQueue< int, int, std::less< int > > queue3;
      queue3 = queue1;
      CHECK((queue3.top()) == (2));
      CHECK((queue3.topPriority()) == (1));
      CHECK((queue3.size()) == (static_cast< gum::Size >(3)));

      gum::PriorityQueue< int, int, std::less< int > > queue4(queue1);
      CHECK((queue4.top()) == (2));
      CHECK((queue4.topPriority()) == (1));
      CHECK((queue4.size()) == (static_cast< gum::Size >(3)));

      gum::PriorityQueue< int > queue2 = std::move(queue1);
      CHECK((queue2.top()) == (2));
      CHECK((queue2.topPriority()) == (1));
      CHECK((queue2.size()) == (static_cast< gum::Size >(3)));

      gum::PriorityQueue< int, int, std::less< int > > queue5(std::move(queue4));
      CHECK((queue5.top()) == (2));
      CHECK((queue5.topPriority()) == (1));
      CHECK((queue5.size()) == (static_cast< gum::Size >(3)));

      gum::PriorityQueue< int > queue6{std::pair< int, int >(2, 1)};
      queue6 = std::move(queue2);
      CHECK((queue6.top()) == (2));
      CHECK((queue6.topPriority()) == (1));
      CHECK((queue6.size()) == (static_cast< gum::Size >(3)));
    }

    static void testMoveGen() {
      gum::PriorityQueue< std::string > queue1{std::pair< std::string, int >("a", 3),
                                               std::pair< std::string, int >("b", 1),
                                               std::pair< std::string, int >("c", 10)};

      gum::PriorityQueue< std::string, int, std::less< int > > queue3;
      queue3 = queue1;
      CHECK((queue3.top()) == ("b"));
      CHECK((queue3.topPriority()) == (1));
      CHECK((queue3.size()) == (static_cast< gum::Size >(3)));

      gum::PriorityQueue< std::string, int, std::less< int > > queue4(queue1);
      CHECK((queue4.top()) == ("b"));
      CHECK((queue4.topPriority()) == (1));
      CHECK((queue4.size()) == (static_cast< gum::Size >(3)));

      gum::PriorityQueue< std::string > queue2 = std::move(queue1);
      CHECK((queue2.top()) == ("b"));
      CHECK((queue2.topPriority()) == (1));
      CHECK((queue2.size()) == (static_cast< gum::Size >(3)));

      gum::PriorityQueue< std::string, int, std::less< int > > queue5(std::move(queue4));
      CHECK((queue5.top()) == ("b"));
      CHECK((queue5.topPriority()) == (1));
      CHECK((queue5.size()) == (static_cast< gum::Size >(3)));

      gum::PriorityQueue< std::string > queue6{std::pair< std::string, int >("a", 1)};
      queue6 = std::move(queue2);
      CHECK((queue6.top()) == ("b"));
      CHECK((queue6.topPriority()) == (1));
      CHECK((queue6.size()) == (static_cast< gum::Size >(3)));
    }

    static void testScalar() {
      gum::PriorityQueue< int > queue1;

      CHECK((queue1.size()) == (static_cast< gum::Size >(0)));
      CHECK((queue1.empty()) == (true));
      CHECK((queue1.contains(32)) == (false));
      CHECK_THROWS_AS(queue1.top(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.pop(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.topPriority(), const gum::NotFound&);

      queue1.insert(1, 8);
      queue1.insert(2, 10);
      queue1.insert(3, 2);
      queue1.emplace(4, 23);
      int xxx = 5;
      int nb  = 24;
      queue1.insert(xxx, nb);

      CHECK((queue1.size()) == (static_cast< gum::Size >(5)));
      CHECK((queue1.empty()) == (false));
      CHECK((queue1.contains(1)) == (true));
      CHECK((queue1.contains(26)) == (false));

      int str = queue1.top();
      CHECK((str) == (3));
      CHECK((queue1.topPriority()) == (2));

      int str2 = queue1.pop();
      CHECK((str2) == (3));
      CHECK((queue1.top()) == (1));
      CHECK((queue1.topPriority()) == (8));

      queue1.setPriorityByPos(0, 4);
      CHECK((queue1.top()) == (1));
      CHECK((queue1.topPriority()) == (4));
      CHECK_THROWS_AS(queue1.setPriorityByPos(10, 4), const gum::NotFound&);

      queue1.setPriority(2, 100);
      nb = 2;
      queue1.setPriority(2, nb);
      CHECK((queue1.top()) == (2));
      CHECK((queue1.topPriority()) == (2));
      CHECK_THROWS_AS(queue1.setPriority(25, 4), const gum::NotFound&);

      CHECK_THROWS_AS(queue1.priority(25), const gum::NotFound&);
      CHECK((queue1.priority(2)) == (2));

      queue1.resize(10);
      CHECK((queue1.capacity()) == (static_cast< gum::Size >(10)));

      const gum::HashTable< int, gum::Size >& vals = queue1.allValues();
      CHECK((vals.size()) == (static_cast< gum::Size >(4)));
    }

    static void testCopy() {
      gum::PriorityQueue< std::string > queue1;
      queue1.insert("AAA", 8);
      queue1.insert("BBB", 10);
      queue1.insert("CCC", 2);
      queue1.insert("DDD", 23);
      queue1.insert("EEE", 24);

      gum::PriorityQueue< std::string > queue2 = queue1;
      CHECK((queue2.size()) == (static_cast< gum::Size >(5)));
      CHECK((queue2.empty()) == (false));
      CHECK((queue2.contains("AAA")) == (true));
      CHECK((queue2.contains("ZZZ")) == (false));

      queue2.clear();
      CHECK((queue2.size()) == (static_cast< gum::Size >(0)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(queue2.eraseByPos(10));
      GUM_CHECK_ASSERT_THROWS_NOTHING(queue2.erase("RRR"));

      queue2 = queue1;
      queue2.eraseTop();
      queue2 = queue1;
      CHECK((queue2.size()) == (static_cast< gum::Size >(5)));
      CHECK((queue2.empty()) == (false));
      CHECK((queue2.contains("AAA")) == (true));
      CHECK((queue2.contains("ZZZ")) == (false));
      queue2 = queue1;
      CHECK((queue2.size()) == (static_cast< gum::Size >(5)));
      CHECK((queue2.empty()) == (false));
      CHECK((queue2.contains("AAA")) == (true));
      CHECK((queue2.contains("ZZZ")) == (false));

      queue2.eraseTop();
      CHECK((queue2.topPriority()) == (8));
      queue2.eraseByPos(0);
      CHECK((queue2.topPriority()) == (10));
      queue2.eraseByPos(3);
      CHECK((queue2.size()) == (static_cast< gum::Size >(3)));
      queue2.erase("AAA");
      CHECK((queue2.size()) == (static_cast< gum::Size >(3)));

      const std::string& str1 = queue2[0];
      CHECK((str1) == ("BBB"));
    }

    static void testConstObj() {
      gum::Set< int > set1, set2, set3, set4, set5, set6;
      set1 << 1;
      set2 << 1 << 2;
      set3 << 1 << 2 << 3;
      set4 << 1 << 2 << 3 << 4;
      set5 << 1 << 2 << 3 << 4 << 5;

      gum::PriorityQueue< gum::Set< int > > queue1;
      CHECK((queue1.size()) == (static_cast< gum::Size >(0)));
      CHECK((queue1.empty()) == (true));
      CHECK((queue1.contains(set1)) == (false));
      CHECK_THROWS_AS(queue1.top(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.pop(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.topPriority(), const gum::NotFound&);

      queue1.insert(set1, 8);
      queue1.insert(set2, 10);
      queue1.insert(set3, 2);
      queue1.insert(set4, 23);
      queue1.insert(set5, 24);

      CHECK((queue1.size()) == (static_cast< gum::Size >(5)));
      CHECK((queue1.empty()) == (false));
      CHECK((queue1.contains(set1)) == (true));
      CHECK((queue1.contains(set6)) == (false));

      const gum::Set< int >& str = queue1.top();
      CHECK((str) == (set3));
      CHECK((queue1.topPriority()) == (2));

      gum::Set< int > str2 = queue1.pop();
      CHECK((str2) == (set3));
      CHECK((queue1.top()) == (set1));
      CHECK((queue1.topPriority()) == (8));

      queue1.setPriorityByPos(0, 4);
      CHECK((queue1.top()) == (set1));
      CHECK((queue1.topPriority()) == (4));
      CHECK_THROWS_AS(queue1.setPriorityByPos(10, 4), const gum::NotFound&);

      int nb = 2;
      queue1.setPriority(set2, nb);
      CHECK((queue1.top()) == (set2));
      CHECK((queue1.topPriority()) == (2));
      CHECK_THROWS_AS(queue1.setPriority(set6, 4), const gum::NotFound&);

      CHECK_THROWS_AS(queue1.priority(set6), const gum::NotFound&);
      CHECK((queue1.priority(set2)) == (2));

      queue1.resize(10);
      CHECK((queue1.capacity()) == (static_cast< gum::Size >(10)));

      CHECK_THROWS_AS(queue1.insert(set1, 10), const gum::DuplicateElement&);

      const gum::HashTable< gum::Set< int >, gum::Size >& vals = queue1.allValues();
      CHECK((vals.size()) == (static_cast< gum::Size >(4)));

      queue1.clear();
      queue1.insert(set1, 8);
      queue1.insert(set2, 10);
      queue1.insert(set3, 2);
      queue1.insert(set4, 23);
      queue1.insert(set5, 24);

      gum::PriorityQueue< gum::Set< int > > queue2 = queue1;
      queue2                                       = queue1;
      queue2.eraseTop();
      queue2 = queue1;

      CHECK((queue2.size()) == (static_cast< gum::Size >(5)));
      CHECK((queue2.empty()) == (false));
      CHECK((queue2.contains(set1)) == (true));
      CHECK((queue2.contains(set6)) == (false));
      queue2 = queue1;
      CHECK((queue2.size()) == (static_cast< gum::Size >(5)));
      CHECK((queue2.empty()) == (false));
      CHECK((queue2.contains(set1)) == (true));
      CHECK((queue2.contains(set6)) == (false));

      queue2.eraseTop();
      CHECK((queue2.topPriority()) == (8));
      queue2.eraseByPos(0);
      CHECK((queue2.topPriority()) == (10));
      queue2.eraseByPos(3);
      CHECK((queue2.size()) == (static_cast< gum::Size >(3)));
      queue2.erase(set1);
      CHECK((queue2.size()) == (static_cast< gum::Size >(3)));

      const gum::Set< int >& str3 = queue2[0];
      const gum::Set< int >& str4 = queue2[1];
      CHECK((str3) == (set2));
      CHECK((str4) == (set4));
    }

    static void testMultiConstruct() {
      gum::MultiPriorityQueue< std::string > queue1;
      CHECK((queue1.size()) == (static_cast< gum::Size >(0)));
      CHECK((queue1.empty()) == (true));
      CHECK((queue1.contains("ZZZ")) == (false));
      CHECK_THROWS_AS(queue1.top(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.pop(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.topPriority(), const gum::NotFound&);

      queue1.insert("AAA", 8);
      queue1.insert("BBB", 10);
      queue1.insert("CCC", 2);
      queue1.insert("DDD", 23);
      queue1.insert("EEE", 24);
      queue1.insert("AAA", 10);

      CHECK((queue1.size()) == (static_cast< gum::Size >(6)));
      CHECK((queue1.empty()) == (false));
      CHECK((queue1.contains("AAA")) == (true));
      CHECK((queue1.contains("ZZZ")) == (false));

      const std::string& str = queue1.top();
      CHECK((str) == ("CCC"));
      CHECK((queue1.topPriority()) == (2));

      std::string str2 = queue1.pop();
      CHECK((str2) == ("CCC"));
      CHECK((queue1.top()) == ("AAA"));
      CHECK((queue1.topPriority()) == (8));

      queue1.setPriorityByPos(0, 4);
      CHECK((queue1.top()) == ("AAA"));
      CHECK((queue1.topPriority()) == (4));
      CHECK_THROWS_AS(queue1.setPriorityByPos(10, 4), const gum::NotFound&);

      queue1.setPriority("BBB", 2);
      CHECK((queue1.top()) == ("BBB"));
      CHECK((queue1.topPriority()) == (2));
      CHECK_THROWS_AS(queue1.setPriority("zzz", 4), const gum::NotFound&);

      CHECK_THROWS_AS(queue1.priority("zzz"), const gum::NotFound&);
      CHECK((queue1.priority("BBB")) == (2));

      queue1.resize(10);
      CHECK((queue1.capacity()) == (static_cast< gum::Size >(10)));

      const gum::HashTable< std::string, std::vector< gum::Size > >& vals = queue1.allValues();
      CHECK((vals.size()) == (static_cast< gum::Size >(4)));
    }

    static void testMultiCopy() {
      gum::MultiPriorityQueue< std::string > queue1;
      queue1.insert("AAA", 8);
      queue1.insert("BBB", 10);
      queue1.insert("CCC", 2);
      queue1.insert("DDD", 23);
      queue1.insert("EEE", 24);
      queue1.insert("AAA", 20);

      gum::MultiPriorityQueue< std::string > queue2 = queue1;
      CHECK((queue2.size()) == (static_cast< gum::Size >(6)));
      CHECK((queue2.empty()) == (false));
      CHECK((queue2.contains("AAA")) == (true));
      CHECK((queue2.contains("ZZZ")) == (false));

      queue2.clear();
      CHECK((queue2.size()) == (static_cast< gum::Size >(0)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(queue2.eraseByPos(10));
      GUM_CHECK_ASSERT_THROWS_NOTHING(queue2.erase("RRR"));

      queue2 = queue1;
      queue2.eraseTop();
      queue2 = queue1;
      CHECK((queue2.size()) == (static_cast< gum::Size >(6)));
      CHECK((queue2.empty()) == (false));
      CHECK((queue2.contains("AAA")) == (true));
      CHECK((queue2.contains("ZZZ")) == (false));
      queue2 = queue1;
      CHECK((queue2.size()) == (static_cast< gum::Size >(6)));
      CHECK((queue2.empty()) == (false));
      CHECK((queue2.contains("AAA")) == (true));
      CHECK((queue2.contains("ZZZ")) == (false));

      queue2.eraseTop();
      CHECK((queue2.topPriority()) == (8));
      queue2.eraseByPos(0);
      CHECK((queue2.topPriority()) == (10));
      queue2.eraseByPos(3);
      CHECK((queue2.size()) == (static_cast< gum::Size >(3)));
      queue2.erase("AAA");
      CHECK((queue2.size()) == (static_cast< gum::Size >(2)));

      const std::string& str1 = queue2[0];
      const std::string& str2 = queue2[1];
      CHECK((str1) == ("BBB"));
      CHECK((str2) == ("DDD"));
    }

    static void testMultiConstObj() {
      gum::Set< int > set1, set2, set3, set4, set5, set6;
      set1 << 1;
      set2 << 1 << 2;
      set3 << 1 << 2 << 3;
      set4 << 1 << 2 << 3 << 4;
      set5 << 1 << 2 << 3 << 4 << 5;

      gum::MultiPriorityQueue< gum::Set< int > > queue1;
      CHECK((queue1.size()) == (static_cast< gum::Size >(0)));
      CHECK((queue1.empty()) == (true));
      CHECK((queue1.contains(set1)) == (false));
      CHECK_THROWS_AS(queue1.top(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.pop(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.topPriority(), const gum::NotFound&);

      queue1.insert(set1, 8);
      queue1.insert(set2, 10);
      queue1.insert(set3, 2);
      queue1.insert(set4, 23);
      queue1.insert(set5, 24);
      queue1.insert(set1, 10);

      CHECK((queue1.size()) == (static_cast< gum::Size >(6)));
      CHECK((queue1.empty()) == (false));
      CHECK((queue1.contains(set1)) == (true));
      CHECK((queue1.contains(set6)) == (false));

      const gum::Set< int >& str = queue1.top();
      CHECK((str) == (set3));
      CHECK((queue1.topPriority()) == (2));

      gum::Set< int > str2 = queue1.pop();
      CHECK((str2) == (set3));
      CHECK((queue1.top()) == (set1));
      CHECK((queue1.topPriority()) == (8));

      queue1.setPriorityByPos(0, 4);
      CHECK((queue1.top()) == (set1));
      CHECK((queue1.topPriority()) == (4));
      CHECK_THROWS_AS(queue1.setPriorityByPos(10, 4), const gum::NotFound&);

      queue1.setPriority(set2, 2);
      CHECK((queue1.top()) == (set2));
      CHECK((queue1.topPriority()) == (2));
      CHECK_THROWS_AS(queue1.setPriority(set6, 4), const gum::NotFound&);

      CHECK_THROWS_AS(queue1.priority(set6), const gum::NotFound&);
      CHECK((queue1.priority(set2)) == (2));

      queue1.resize(10);
      CHECK((queue1.capacity()) == (static_cast< gum::Size >(10)));

      const gum::HashTable< gum::Set< int >, std::vector< gum::Size > >& vals = queue1.allValues();
      CHECK((vals.size()) == (static_cast< gum::Size >(4)));

      queue1.clear();
      queue1.insert(set1, 8);
      queue1.insert(set2, 10);
      queue1.insert(set3, 2);
      queue1.insert(set4, 23);
      queue1.insert(set5, 24);
      queue1.insert(set1, 10);

      gum::MultiPriorityQueue< gum::Set< int > > queue2 = queue1;
      queue2                                            = queue1;
      queue2.eraseTop();
      queue2 = queue1;

      CHECK((queue2.size()) == (static_cast< gum::Size >(6)));
      CHECK((queue2.empty()) == (false));
      CHECK((queue2.contains(set1)) == (true));
      CHECK((queue2.contains(set6)) == (false));
      queue2 = queue1;
      CHECK((queue2.size()) == (static_cast< gum::Size >(6)));
      CHECK((queue2.empty()) == (false));
      CHECK((queue2.contains(set1)) == (true));
      CHECK((queue2.contains(set6)) == (false));

      queue2.eraseTop();
      CHECK((queue2.topPriority()) == (8));
      queue2.eraseByPos(0);
      CHECK((queue2.topPriority()) == (10));
      queue2.eraseByPos(3);
      CHECK((queue2.size()) == (static_cast< gum::Size >(3)));
      queue2.erase(set1);
      CHECK((queue2.size()) == (static_cast< gum::Size >(2)));

      const gum::Set< int >& str3 = queue2[0];
      const gum::Set< int >& str4 = queue2[1];
      CHECK((str3) == (set2));
      CHECK((str4) == (set4));
    }

    static void testMultiMoveScalar() {
      gum::MultiPriorityQueue< int > queue1{std::pair< int, int >(1, 3),
                                            std::pair< int, int >(2, 1),
                                            std::pair< int, int >(3, 10)};

      gum::MultiPriorityQueue< int, int, std::less< int > > queue3;
      queue3 = queue1;
      CHECK((queue3.top()) == (2));
      CHECK((queue3.topPriority()) == (1));
      CHECK((queue3.size()) == (static_cast< gum::Size >(3)));

      gum::MultiPriorityQueue< int, int, std::less< int > > queue4(queue1);
      CHECK((queue4.top()) == (2));
      CHECK((queue4.topPriority()) == (1));
      CHECK((queue4.size()) == (static_cast< gum::Size >(3)));

      gum::MultiPriorityQueue< int > queue2 = std::move(queue1);
      CHECK((queue2.top()) == (2));
      CHECK((queue2.topPriority()) == (1));
      CHECK((queue2.size()) == (static_cast< gum::Size >(3)));

      gum::MultiPriorityQueue< int, int, std::less< int > > queue5(std::move(queue4));
      CHECK((queue5.top()) == (2));
      CHECK((queue5.topPriority()) == (1));
      CHECK((queue5.size()) == (static_cast< gum::Size >(3)));

      gum::MultiPriorityQueue< int > queue6{std::pair< int, int >(2, 1)};
      queue6 = std::move(queue2);
      CHECK((queue6.top()) == (2));
      CHECK((queue6.topPriority()) == (1));
      CHECK((queue6.size()) == (static_cast< gum::Size >(3)));
    }
  };

  GUM_TEST_ACTIF(Construct)
  GUM_TEST_ACTIF(MoveScalar)
  GUM_TEST_ACTIF(MoveGen)
  GUM_TEST_ACTIF(Scalar)
  GUM_TEST_ACTIF(Copy)
  GUM_TEST_ACTIF(ConstObj)
  GUM_TEST_ACTIF(MultiConstruct)
  GUM_TEST_ACTIF(MultiCopy)
  GUM_TEST_ACTIF(MultiConstObj)
  GUM_TEST_ACTIF(MultiMoveScalar)

} /* namespace gum_tests */
