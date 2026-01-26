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

#include <agrum/base/core/sortedPriorityQueue.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  SortedPriorityQueue
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct SortedPriorityQueueTestSuite {
    public:
    static void testString() {
      gum::SortedPriorityQueue< std::string > queue1;

      CHECK((queue1.size()) == (static_cast< gum::Size >(0)));
      CHECK((queue1.empty()) == (true));
      CHECK((queue1.contains("ZZZ")) == (false));
      CHECK_THROWS_AS(queue1.top(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.bottom(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.pop(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.popTop(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.popBottom(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.topPriority(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.bottomPriority(), const gum::NotFound&);

      queue1.insert("AAA", 8);
      queue1.insert("BBB", 10);
      queue1.insert("CCC", 2);
      queue1.emplace("DDD", 23);
      std::string xxx = "EEE";
      int         nb  = 24;
      queue1.insert(xxx, nb);

      {
        std::stringstream str;
        str << queue1.toString();
        std::string str2 = "[(EEE, 24) ; (DDD, 23) ; (BBB, 10) ; (AAA, 8) ; (CCC, 2)]";
        CHECK((str.str()) == (str2));
      }

      {
        std::vector< std::string > vect = {"EEE", "DDD", "BBB", "AAA", "CCC"};
        int                        i    = 0;
        for (const auto& val: queue1) {
          CHECK((val) == (vect[i++]));
        }
        i = 0;
        for (auto iter = queue1.beginSafe(); iter != queue1.endSafe(); ++iter)
          CHECK((*iter) == (vect[i++]));
        i = 4;
        for (auto iter = queue1.rbegin(); iter != queue1.rend(); ++iter)
          CHECK((*iter) == (vect[i--]));
        i = 4;
        for (auto iter = queue1.rbeginSafe(); iter != queue1.rendSafe(); ++iter)
          CHECK((*iter) == (vect[i--]));
      }

      CHECK((queue1.size()) == (static_cast< gum::Size >(5)));
      CHECK((queue1.empty()) == (false));
      CHECK((queue1.contains("AAA")) == (true));
      CHECK((queue1.contains("ZZZ")) == (false));

      queue1.eraseTop();
      std::string str = queue1.top();
      CHECK((str) == ("DDD"));
      CHECK((queue1.topPriority()) == (23));

      str = queue1.pop();
      CHECK((str) == ("DDD"));
      CHECK((queue1.top()) == ("BBB"));
      CHECK((queue1.topPriority()) == (10));

      queue1.insert("EEE", 24);   // queue = [(EEE, 24), (BBB, 10), (AAA, 8), (CCC, 2)]
      queue1.eraseBottom();       // queue = [(EEE, 24), (BBB, 10), (AAA, 8)]
      str = queue1.bottom();
      CHECK((str) == ("AAA"));
      CHECK((queue1.bottomPriority()) == (8));

      queue1.setPriority("BBB", 3);
      {
        std::string str2 = "[(EEE, 24) ; (AAA, 8) ; (BBB, 3)]";
        CHECK((queue1.toString()) == (str2));
      }

      nb = 2;
      queue1.setPriority("BBB", nb);
      CHECK((queue1.bottom()) == ("BBB"));

      CHECK((queue1.bottomPriority()) == (2));
      {
        std::string str2 = "[(EEE, 24) ; (AAA, 8) ; (BBB, 2)]";
        CHECK((queue1.toString()) == (str2));
      }

      queue1.setPriority("BBB", 10);
      {
        std::string str2 = "[(EEE, 24) ; (BBB, 10) ; (AAA, 8)]";
        CHECK((queue1.toString()) == (str2));
      }
      queue1.setPriority("BBB", 9);
      queue1.setPriority("BBB", 10);

      CHECK_NOTHROW(queue1.erase("EEE"));
      CHECK_THROWS_AS(queue1.setPriority("zzz", 4), const gum::NotFound&);

      CHECK_THROWS_AS(queue1.priority("zzz"), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.priority("zzz"), const gum::NotFound&);
      CHECK((queue1.priority("BBB")) == (10));

      queue1.clear();
      CHECK((queue1.size()) == (static_cast< gum::Size >(0)));
      CHECK((queue1.empty()) == (true));
      CHECK_THROWS_AS(queue1.top(), const gum::NotFound&);
      CHECK_THROWS_AS(queue1.bottom(), const gum::NotFound&);
    }   // namespace gum_tests

    static void testInt() {
      gum::SortedPriorityQueue< std::pair< int, int >, double > queue;
      CHECK((queue.size()) == (static_cast< gum::Size >(0)));
      CHECK((queue.empty()) == (true));

      std::vector< std::pair< std::pair< int, int >, double > > vect;
      for (int i = 0; i < 100; ++i) {
        const std::pair< int, int > x(i, i + 10);
        vect.emplace_back(x, i * 10.0);
      }
      std::reverse(vect.begin(), vect.end());
      auto vect2 = vect;
      std::shuffle(std::begin(vect2), std::end(vect2), gum::randomGenerator());
      for (const auto& elt: vect2) {
        if (queue.size() % 2 == 0) queue.insert(elt.first, elt.second);
        else {
          auto elt2 = elt;
          queue.insert(std::move(elt2.first), std::move(elt2.second));
        }
      }
      CHECK((vect2string(vect)) == (queue.toString()));

      CHECK((queue.size()) == (static_cast< gum::Size >(100)));
      CHECK((queue.empty()) == (false));
      CHECK((queue.contains(std::pair< int, int >(15, 25))) == (true));
      CHECK((queue.contains(std::pair< int, int >(15, 15))) == (false));

      queue.eraseTop();
      vect.erase(vect.begin());
      auto                  res = queue.top();
      std::pair< int, int > res2(98, 108);
      CHECK((res) == (res2));
      CHECK((queue.topPriority()) == (980.0));
      res2 = {0, 10};
      res  = queue.bottom();
      CHECK((res) == (res2));
      CHECK((queue.bottomPriority()) == (0.0));

      res = queue.pop();
      vect.erase(vect.begin());
      res2 = {98, 108};
      CHECK((res) == (res2));
      res2 = {97, 107};
      CHECK((queue.top()) == (res2));
      CHECK((queue.topPriority()) == (970));

      res = queue.popBottom();
      vect.pop_back();
      res2 = {0, 10};
      CHECK((res) == (res2));

      CHECK((vect2string(vect)) == (queue.toString()));

      vect2 = vect;
      std::shuffle(std::begin(vect2), std::end(vect2), gum::randomGenerator());
      for (const auto& elt: vect2) {
        const auto& value = elt.first;
        vect.erase(vect.begin() + pos2vect(vect, value));
        queue.erase(value);
        CHECK((vect2string(vect)) == (queue.toString()));
      }
    }

    static void test_erase() {
      gum::SortedPriorityQueue< std::pair< int, int >, double > queue;
      std::vector< std::pair< std::pair< int, int >, double > > vect;
      for (int i = 0; i < 100; ++i) {
        const std::pair< int, int > x(i, i + 10);
        vect.emplace_back(x, i * 10.0);
      }
      std::reverse(vect.begin(), vect.end());
      auto vect2 = vect;

      std::shuffle(std::begin(vect2), std::end(vect2), gum::randomGenerator());
      for (const auto& elt: vect2) {
        queue.insert(elt.first, elt.second);
      }
      CHECK((vect2string(vect)) == (queue.toString()));

      std::shuffle(std::begin(vect2), std::end(vect2), gum::randomGenerator());
      for (const auto& elt: vect2) {
        const auto& value = elt.first;

        auto iter1 = queue.beginSafe(), iter2 = queue.beginSafe(), iter3 = queue.beginSafe();
        while (*iter1 != elt.first)
          ++iter1;
        iter2 = iter1;
        --iter2;
        iter3 = iter1;
        ++iter3;
        int i2 = 0, i3 = 0;
        try {
          *iter2;
        } catch (gum::NotFound&) { i2 = 1; }
        try {
          *iter3;
        } catch (gum::NotFound&) { i3 = 1; }
        queue.erase(value);
        CHECK_THROWS_AS(*iter1, gum::NotFound&);
        if (i2 == 0) CHECK_NOTHROW(*iter2);
        if (i3 == 0) CHECK_NOTHROW(*iter3);

        vect.erase(vect.begin() + pos2vect(vect, value));
        CHECK((vect2string(vect)) == (queue.toString()));
      }
    }

    private:
    static std::size_t
        pos2vect(const std::vector< std::pair< std::pair< int, int >, double > >& vect,
                 const std::pair< int, int >&                                     elt) {
      for (std::size_t i = 0, size = vect.size(); i < size; ++i) {
        if (vect[i].first == elt) return i;
      }
      throw(gum::NotFound(0));
    }

    static std::string
        vect2string(const std::vector< std::pair< std::pair< int, int >, double > >& vect) {
      bool              deja = false;
      std::stringstream stream;
      stream << "[";
      for (const auto& elt: vect) {
        if (deja) stream << " ; ";
        else deja = true;
        stream << "(" << elt.first << ", " << elt.second << ")";
      }
      stream << "]";
      return stream.str();
    }
  };

  GUM_TEST_ACTIF(String)
  GUM_TEST_ACTIF(Int)
  GUM_TEST_ACTIF(_erase)

}   // namespace gum_tests
