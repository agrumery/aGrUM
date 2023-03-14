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


#include <string>
#include <chrono>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/core/sortedPriorityQueue.h>

namespace gum_tests {

  class [[maybe_unused]] SortedPriorityQueueTestSuite: public CxxTest::TestSuite {
    public:
    GUM_TEST(String) {
      gum::SortedPriorityQueue< std::string > queue1;

      TS_ASSERT_EQUALS(queue1.size(), (gum::Size)0)
      TS_ASSERT_EQUALS(queue1.empty(), true)
      TS_ASSERT_EQUALS(queue1.contains("ZZZ"), false)
      TS_ASSERT_THROWS(queue1.top(), const gum::NotFound&)
      TS_ASSERT_THROWS(queue1.bottom(), const gum::NotFound&)
      TS_ASSERT_THROWS(queue1.pop(), const gum::NotFound&)
      TS_ASSERT_THROWS(queue1.popTop(), const gum::NotFound&)
      TS_ASSERT_THROWS(queue1.popBottom(), const gum::NotFound&)
      TS_ASSERT_THROWS(queue1.topPriority(), const gum::NotFound&)
      TS_ASSERT_THROWS(queue1.bottomPriority(), const gum::NotFound&)

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
        TS_GUM_ASSERT_EQUALS(str.str(), str2)
      }

      {
        std::vector< std::string > vect = {"EEE", "DDD", "BBB", "AAA", "CCC"};
        int                        i    = 0;
        for (const auto& val: queue1) {
          TS_GUM_ASSERT_EQUALS(val, vect[i++])
        }
        i = 0;
        for (auto iter = queue1.beginSafe(); iter != queue1.endSafe(); ++iter)
          TS_GUM_ASSERT_EQUALS(*iter, vect[i++])
        i = 4;
        for (auto iter = queue1.rbegin(); iter != queue1.rend(); ++iter)
          TS_GUM_ASSERT_EQUALS(*iter, vect[i--])
        i = 4;
        for (auto iter = queue1.rbeginSafe(); iter != queue1.rendSafe(); ++iter)
          TS_GUM_ASSERT_EQUALS(*iter, vect[i--])
      }

      TS_ASSERT_EQUALS(queue1.size(), (gum::Size)5)
      TS_ASSERT_EQUALS(queue1.empty(), false)
      TS_ASSERT_EQUALS(queue1.contains("AAA"), true)
      TS_ASSERT_EQUALS(queue1.contains("ZZZ"), false)

      queue1.eraseTop();
      std::string str = queue1.top();
      TS_ASSERT_EQUALS(str, "DDD")
      TS_ASSERT_EQUALS(queue1.topPriority(), 23)

      str = queue1.pop();
      TS_ASSERT_EQUALS(str, "DDD")
      TS_ASSERT_EQUALS(queue1.top(), "BBB")
      TS_ASSERT_EQUALS(queue1.topPriority(), 10)

      queue1.insert("EEE", 24);   // queue = [(EEE, 24), (BBB, 10), (AAA, 8), (CCC, 2)]
      queue1.eraseBottom();       // queue = [(EEE, 24), (BBB, 10), (AAA, 8)]
      str = queue1.bottom();
      TS_ASSERT_EQUALS(str, "AAA")
      TS_ASSERT_EQUALS(queue1.bottomPriority(), 8)

      queue1.setPriority("BBB", 3);
      {
        std::string str2 = "[(EEE, 24) ; (AAA, 8) ; (BBB, 3)]";
        TS_GUM_ASSERT_EQUALS(queue1.toString(), str2)
      }

      nb = 2;
      queue1.setPriority("BBB", nb);
      TS_ASSERT_EQUALS(queue1.bottom(), "BBB")
      TS_ASSERT_EQUALS(queue1.bottomPriority(), 2) {
        std::string str2 = "[(EEE, 24) ; (AAA, 8) ; (BBB, 2)]";
        TS_GUM_ASSERT_EQUALS(queue1.toString(), str2)
      }
      queue1.setPriority("BBB", 10);
      {
        std::string str2 = "[(EEE, 24) ; (BBB, 10) ; (AAA, 8)]";
        TS_GUM_ASSERT_EQUALS(queue1.toString(), str2)
      }
      queue1.setPriority("BBB", 9);
      queue1.setPriority("BBB", 10);

      TS_ASSERT_THROWS_NOTHING(queue1.erase("EEE"))
      TS_ASSERT_THROWS(queue1.setPriority("zzz", 4), const gum::NotFound&)

      TS_ASSERT_THROWS(queue1.priority("zzz"), const gum::NotFound&)
      TS_ASSERT_THROWS(queue1.priority("zzz"), const gum::NotFound&)
      TS_ASSERT_EQUALS(queue1.priority("BBB"), 10)

      queue1.clear();
      TS_ASSERT_EQUALS(queue1.size(), (gum::Size)0)
      TS_ASSERT_EQUALS(queue1.empty(), true)
      TS_ASSERT_THROWS(queue1.top(), const gum::NotFound&)
      TS_ASSERT_THROWS(queue1.bottom(), const gum::NotFound&)
    }

    GUM_TEST(Int) {
      gum::SortedPriorityQueue< std::pair< int, int >, double > queue;
      TS_ASSERT_EQUALS(queue.size(), (gum::Size)0)
      TS_ASSERT_EQUALS(queue.empty(), true)

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
      TS_GUM_ASSERT_EQUALS(vect2string(vect), queue.toString())

      TS_ASSERT_EQUALS(queue.size(), (gum::Size)100)
      TS_ASSERT_EQUALS(queue.empty(), false)
      TS_ASSERT_EQUALS(queue.contains(std::pair< int, int >(15, 25)), true)
      TS_ASSERT_EQUALS(queue.contains(std::pair< int, int >(15, 15)), false)

      queue.eraseTop();
      vect.erase(vect.begin());
      auto                  res = queue.top();
      std::pair< int, int > res2(98, 108);
      TS_ASSERT_EQUALS(res, res2)
      TS_ASSERT_EQUALS(queue.topPriority(), 980.0)
      res2 = {0, 10};
      res  = queue.bottom();
      TS_ASSERT_EQUALS(res, res2)
      TS_ASSERT_EQUALS(queue.bottomPriority(), 0.0)

      res = queue.pop();
      vect.erase(vect.begin());
      res2 = {98, 108};
      TS_ASSERT_EQUALS(res, res2)
      res2 = {97, 107};
      TS_ASSERT_EQUALS(queue.top(), res2)
      TS_ASSERT_EQUALS(queue.topPriority(), 970)

      res = queue.popBottom();
      vect.pop_back();
      res2 = {0, 10};
      TS_ASSERT_EQUALS(res, res2)

      TS_GUM_ASSERT_EQUALS(vect2string(vect), queue.toString())

      vect2 = vect;
      std::shuffle(std::begin(vect2), std::end(vect2), rng);
      for (const auto& elt: vect2) {
        const auto& value = elt.first;
        vect.erase(vect.begin() + pos2vect(vect, value));
        queue.erase(value);
        TS_GUM_ASSERT_EQUALS(vect2string(vect), queue.toString())
      }
    }

    GUM_TEST(_erase) {
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
      TS_GUM_ASSERT_EQUALS(vect2string(vect), queue.toString())

      std::shuffle(std::begin(vect2), std::end(vect2), rng);
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
        TS_ASSERT_THROWS(*iter1, gum::NotFound&)
        if (i2 == 0) TS_ASSERT_THROWS_NOTHING(*iter2);
        if (i3 == 0) TS_ASSERT_THROWS_NOTHING(*iter3);

        vect.erase(vect.begin() + pos2vect(vect, value));
        TS_GUM_ASSERT_EQUALS(vect2string(vect), queue.toString())
      }
    }

    private:
    std::size_t pos2vect(const std::vector< std::pair< std::pair< int, int >, double > >& vect,
                         const std::pair< int, int >&                                     elt) {
      for (std::size_t i = 0, size = vect.size(); i < size; ++i) {
        if (vect[i].first == elt) return i;
      }
      throw(gum::NotFound(0));
    }

    std::string vect2string(const std::vector< std::pair< std::pair< int, int >, double > >& vect) {
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

}   // namespace gum_tests
