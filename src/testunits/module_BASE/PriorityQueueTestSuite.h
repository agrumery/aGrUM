/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
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
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
#include <ressources/myalloc.h>

#include <agrum/core/priorityQueue.h>
#include <agrum/core/multiPriorityQueue.h>
#include <agrum/core/set.h>
#include <agrum/core/hashFunc.h>

namespace gum {
  template <> class HashFunc<gum::Set<int>> : public HashFuncBase<gum::Set<int>> {
    public:
    /// computes the hashed value of a key
    Size operator()(const gum::Set<int> &key) const {
      return ((key.size() * gum::HashFuncConst::gold) & _hash_mask);
    }
  };
}

namespace gum_tests {

  class PriorityQueueTestSuite : public CxxTest::TestSuite {
    public:
    void testConstruct() {
      gum::PriorityQueue<std::string> queue1;

      TS_ASSERT(queue1.size() == 0);
      TS_ASSERT(queue1.empty() == true);
      TS_ASSERT(queue1.contains("ZZZ") == false);
      TS_ASSERT_THROWS(queue1.top(), gum::NotFound);
      TS_ASSERT_THROWS(queue1.pop(), gum::NotFound);
      TS_ASSERT_THROWS(queue1.topPriority(), gum::NotFound);

      queue1.insert("AAA", 8);
      queue1.insert("BBB", 10);
      queue1.insert("CCC", 2);
      queue1.emplace("DDD", 23);
      std::string xxx = "EEE";
      int nb = 24;
      queue1.insert(xxx, nb);

      TS_ASSERT(queue1.size() == 5);
      TS_ASSERT(queue1.empty() == false);
      TS_ASSERT(queue1.contains("AAA") == true);
      TS_ASSERT(queue1.contains("ZZZ") == false);

      const std::string &str = queue1.top();
      TS_ASSERT(str == "CCC");
      TS_ASSERT(queue1.topPriority() == 2);

      std::string str2 = queue1.pop();
      TS_ASSERT(str2 == "CCC");
      TS_ASSERT(queue1.top() == "AAA");
      TS_ASSERT(queue1.topPriority() == 8);

      queue1.setPriorityByPos(0, 4);
      TS_ASSERT(queue1.top() == "AAA");
      TS_ASSERT(queue1.topPriority() == 4);
      TS_ASSERT_THROWS(queue1.setPriorityByPos(10, 4), gum::NotFound);

      queue1.setPriority("BBB", 100);
      nb = 2;
      queue1.setPriority("BBB", nb);
      TS_ASSERT(queue1.top() == "BBB");
      TS_ASSERT(queue1.topPriority() == 2);
      TS_ASSERT_THROWS(queue1.setPriority("zzz", 4), gum::NotFound);

      TS_ASSERT_THROWS(queue1.priority("zzz"), gum::NotFound);
      TS_ASSERT(queue1.priority("BBB") == 2);

      queue1.resize(10);
      TS_ASSERT(queue1.capacity() == 10);

      const gum::HashTable<std::string, gum::Size> &vals = queue1.allValues();
      TS_ASSERT(vals.size() == 4);

      gum::PriorityQueue<std::string> queue10{std::pair<std::string, int>("aa", 3),
                                              std::pair<std::string, int>("bb", 2)};
      TS_ASSERT(queue10.size() == 2);
      TS_ASSERT(queue10.top() == "bb");
    }

    void testMoveScalar() {
      gum::PriorityQueue<int> queue1{std::pair<int, int>(1, 3),
                                     std::pair<int, int>(2, 1),
                                     std::pair<int, int>(3, 10)};

      gum::PriorityQueue<int, int, std::less<int>, MyAlloc<int>> queue3;
      queue3 = queue1;
      TS_ASSERT(queue3.top() == 2);
      TS_ASSERT(queue3.topPriority() == 1);
      TS_ASSERT(queue3.size() == 3);

      gum::PriorityQueue<int, int, std::less<int>, MyAlloc<int>> queue4(queue1);
      TS_ASSERT(queue4.top() == 2);
      TS_ASSERT(queue4.topPriority() == 1);
      TS_ASSERT(queue4.size() == 3);

      gum::PriorityQueue<int> queue2 = std::move(queue1);
      TS_ASSERT(queue2.top() == 2);
      TS_ASSERT(queue2.topPriority() == 1);
      TS_ASSERT(queue2.size() == 3);

      gum::PriorityQueue<int, int, std::less<int>, MyAlloc<int>> queue5(
          std::move(queue4));
      TS_ASSERT(queue5.top() == 2);
      TS_ASSERT(queue5.topPriority() == 1);
      TS_ASSERT(queue5.size() == 3);

      gum::PriorityQueue<int> queue6{std::pair<int, int>(2, 1)};
      queue6 = std::move(queue2);
      TS_ASSERT(queue6.top() == 2);
      TS_ASSERT(queue6.topPriority() == 1);
      TS_ASSERT(queue6.size() == 3);
    }

    void testMoveGen() {
      gum::PriorityQueue<std::string> queue1{std::pair<std::string, int>("a", 3),
                                             std::pair<std::string, int>("b", 1),
                                             std::pair<std::string, int>("c", 10)};

      gum::PriorityQueue<std::string, int, std::less<int>, MyAlloc<int>> queue3;
      queue3 = queue1;
      TS_ASSERT(queue3.top() == "b");
      TS_ASSERT(queue3.topPriority() == 1);
      TS_ASSERT(queue3.size() == 3);

      gum::PriorityQueue<std::string, int, std::less<int>, MyAlloc<int>> queue4(
          queue1);
      TS_ASSERT(queue4.top() == "b");
      TS_ASSERT(queue4.topPriority() == 1);
      TS_ASSERT(queue4.size() == 3);

      gum::PriorityQueue<std::string> queue2 = std::move(queue1);
      TS_ASSERT(queue2.top() == "b");
      TS_ASSERT(queue2.topPriority() == 1);
      TS_ASSERT(queue2.size() == 3);

      gum::PriorityQueue<std::string, int, std::less<int>, MyAlloc<int>> queue5(
          std::move(queue4));
      TS_ASSERT(queue5.top() == "b");
      TS_ASSERT(queue5.topPriority() == 1);
      TS_ASSERT(queue5.size() == 3);

      gum::PriorityQueue<std::string> queue6{std::pair<std::string, int>("a", 1)};
      queue6 = std::move(queue2);
      TS_ASSERT(queue6.top() == "b");
      TS_ASSERT(queue6.topPriority() == 1);
      TS_ASSERT(queue6.size() == 3);
    }

    void testScalar() {
      gum::PriorityQueue<int> queue1;

      TS_ASSERT(queue1.size() == 0);
      TS_ASSERT(queue1.empty() == true);
      TS_ASSERT(queue1.contains(32) == false);
      TS_ASSERT_THROWS(queue1.top(), gum::NotFound);
      TS_ASSERT_THROWS(queue1.pop(), gum::NotFound);
      TS_ASSERT_THROWS(queue1.topPriority(), gum::NotFound);

      queue1.insert(1, 8);
      queue1.insert(2, 10);
      queue1.insert(3, 2);
      queue1.emplace(4, 23);
      int xxx = 5;
      int nb = 24;
      queue1.insert(xxx, nb);

      TS_ASSERT(queue1.size() == 5);
      TS_ASSERT(queue1.empty() == false);
      TS_ASSERT(queue1.contains(1) == true);
      TS_ASSERT(queue1.contains(26) == false);

      int str = queue1.top();
      TS_ASSERT(str == 3);
      TS_ASSERT(queue1.topPriority() == 2);

      int str2 = queue1.pop();
      TS_ASSERT(str2 == 3);
      TS_ASSERT(queue1.top() == 1);
      TS_ASSERT(queue1.topPriority() == 8);

      queue1.setPriorityByPos(0, 4);
      TS_ASSERT(queue1.top() == 1);
      TS_ASSERT(queue1.topPriority() == 4);
      TS_ASSERT_THROWS(queue1.setPriorityByPos(10, 4), gum::NotFound);

      queue1.setPriority(2, 100);
      nb = 2;
      queue1.setPriority(2, nb);
      TS_ASSERT(queue1.top() == 2);
      TS_ASSERT(queue1.topPriority() == 2);
      TS_ASSERT_THROWS(queue1.setPriority(25, 4), gum::NotFound);

      TS_ASSERT_THROWS(queue1.priority(25), gum::NotFound);
      TS_ASSERT(queue1.priority(2) == 2);

      queue1.resize(10);
      TS_ASSERT(queue1.capacity() == 10);

      const gum::HashTable<int, gum::Size> &vals = queue1.allValues();
      TS_ASSERT(vals.size() == 4);
    }

    void testCopy() {
      gum::PriorityQueue<std::string> queue1;
      queue1.insert("AAA", 8);
      queue1.insert("BBB", 10);
      queue1.insert("CCC", 2);
      queue1.insert("DDD", 23);
      queue1.insert("EEE", 24);

      gum::PriorityQueue<std::string> queue2 = queue1;
      TS_ASSERT(queue2.size() == 5);
      TS_ASSERT(queue2.empty() == false);
      TS_ASSERT(queue2.contains("AAA") == true);
      TS_ASSERT(queue2.contains("ZZZ") == false);

      queue2.clear();
      TS_ASSERT(queue2.size() == 0);
      TS_GUM_ASSERT_THROWS_NOTHING(queue2.eraseByPos(10));
      TS_GUM_ASSERT_THROWS_NOTHING(queue2.erase("RRR"));

      queue2 = queue1;
      queue2.eraseTop();
      queue2 = queue1;
      TS_ASSERT(queue2.size() == 5);
      TS_ASSERT(queue2.empty() == false);
      TS_ASSERT(queue2.contains("AAA") == true);
      TS_ASSERT(queue2.contains("ZZZ") == false);
      queue2 = queue1;
      TS_ASSERT(queue2.size() == 5);
      TS_ASSERT(queue2.empty() == false);
      TS_ASSERT(queue2.contains("AAA") == true);
      TS_ASSERT(queue2.contains("ZZZ") == false);

      queue2.eraseTop();
      TS_ASSERT(queue2.topPriority() == 8);
      queue2.eraseByPos(0);
      TS_ASSERT(queue2.topPriority() == 10);
      queue2.eraseByPos(3);
      TS_ASSERT(queue2.size() == 3);
      queue2.erase("AAA");
      TS_ASSERT(queue2.size() == 3);

      const std::string &str1 = queue2[0];
      TS_ASSERT(str1 == "BBB");
    }

    void testConstObj() {
      gum::Set<int> set1, set2, set3, set4, set5, set6;
      set1 << 1;
      set2 << 1 << 2;
      set3 << 1 << 2 << 3;
      set4 << 1 << 2 << 3 << 4;
      set5 << 1 << 2 << 3 << 4 << 5;

      gum::PriorityQueue<gum::Set<int>> queue1;
      TS_ASSERT(queue1.size() == 0);
      TS_ASSERT(queue1.empty() == true);
      TS_ASSERT(queue1.contains(set1) == false);
      TS_ASSERT_THROWS(queue1.top(), gum::NotFound);
      TS_ASSERT_THROWS(queue1.pop(), gum::NotFound);
      TS_ASSERT_THROWS(queue1.topPriority(), gum::NotFound);

      queue1.insert(set1, 8);
      queue1.insert(set2, 10);
      queue1.insert(set3, 2);
      queue1.insert(set4, 23);
      queue1.insert(set5, 24);

      TS_ASSERT(queue1.size() == 5);
      TS_ASSERT(queue1.empty() == false);
      TS_ASSERT(queue1.contains(set1) == true);
      TS_ASSERT(queue1.contains(set6) == false);

      const gum::Set<int> &str = queue1.top();
      TS_ASSERT(str == set3);
      TS_ASSERT(queue1.topPriority() == 2);

      gum::Set<int> str2 = queue1.pop();
      TS_ASSERT(str2 == set3);
      TS_ASSERT(queue1.top() == set1);
      TS_ASSERT(queue1.topPriority() == 8);

      queue1.setPriorityByPos(0, 4);
      TS_ASSERT(queue1.top() == set1);
      TS_ASSERT(queue1.topPriority() == 4);
      TS_ASSERT_THROWS(queue1.setPriorityByPos(10, 4), gum::NotFound);

      int nb = 2;
      queue1.setPriority(set2, nb);
      TS_ASSERT(queue1.top() == set2);
      TS_ASSERT(queue1.topPriority() == 2);
      TS_ASSERT_THROWS(queue1.setPriority(set6, 4), gum::NotFound);

      TS_ASSERT_THROWS(queue1.priority(set6), gum::NotFound);
      TS_ASSERT(queue1.priority(set2) == 2);

      queue1.resize(10);
      TS_ASSERT(queue1.capacity() == 10);

      TS_ASSERT_THROWS(queue1.insert(set1, 10), gum::DuplicateElement);

      const gum::HashTable<gum::Set<int>, gum::Size> &vals = queue1.allValues();
      TS_ASSERT(vals.size() == 4);

      queue1.clear();
      queue1.insert(set1, 8);
      queue1.insert(set2, 10);
      queue1.insert(set3, 2);
      queue1.insert(set4, 23);
      queue1.insert(set5, 24);

      gum::PriorityQueue<gum::Set<int>> queue2 = queue1;
      queue2 = queue1;
      queue2.eraseTop();
      queue2 = queue1;

      TS_ASSERT(queue2.size() == 5);
      TS_ASSERT(queue2.empty() == false);
      TS_ASSERT(queue2.contains(set1) == true);
      TS_ASSERT(queue2.contains(set6) == false);
      queue2 = queue1;
      TS_ASSERT(queue2.size() == 5);
      TS_ASSERT(queue2.empty() == false);
      TS_ASSERT(queue2.contains(set1) == true);
      TS_ASSERT(queue2.contains(set6) == false);

      queue2.eraseTop();
      TS_ASSERT(queue2.topPriority() == 8);
      queue2.eraseByPos(0);
      TS_ASSERT(queue2.topPriority() == 10);
      queue2.eraseByPos(3);
      TS_ASSERT(queue2.size() == 3);
      queue2.erase(set1);
      TS_ASSERT(queue2.size() == 3);

      const gum::Set<int> &str3 = queue2[0];
      const gum::Set<int> &str4 = queue2[1];
      TS_ASSERT(str3 == set2);
      TS_ASSERT(str4 == set4);
    }

    void testMultiConstruct() {
      gum::MultiPriorityQueue<std::string> queue1;
      TS_ASSERT(queue1.size() == 0);
      TS_ASSERT(queue1.empty() == true);
      TS_ASSERT(queue1.contains("ZZZ") == false);
      TS_ASSERT_THROWS(queue1.top(), gum::NotFound);
      TS_ASSERT_THROWS(queue1.pop(), gum::NotFound);
      TS_ASSERT_THROWS(queue1.topPriority(), gum::NotFound);

      queue1.insert("AAA", 8);
      queue1.insert("BBB", 10);
      queue1.insert("CCC", 2);
      queue1.insert("DDD", 23);
      queue1.insert("EEE", 24);
      queue1.insert("AAA", 10);

      TS_ASSERT(queue1.size() == 6);
      TS_ASSERT(queue1.empty() == false);
      TS_ASSERT(queue1.contains("AAA") == true);
      TS_ASSERT(queue1.contains("ZZZ") == false);

      const std::string &str = queue1.top();
      TS_ASSERT(str == "CCC");
      TS_ASSERT(queue1.topPriority() == 2);

      std::string str2 = queue1.pop();
      TS_ASSERT(str2 == "CCC");
      TS_ASSERT(queue1.top() == "AAA");
      TS_ASSERT(queue1.topPriority() == 8);

      queue1.setPriorityByPos(0, 4);
      TS_ASSERT(queue1.top() == "AAA");
      TS_ASSERT(queue1.topPriority() == 4);
      TS_ASSERT_THROWS(queue1.setPriorityByPos(10, 4), gum::NotFound);

      queue1.setPriority("BBB", 2);
      TS_ASSERT(queue1.top() == "BBB");
      TS_ASSERT(queue1.topPriority() == 2);
      TS_ASSERT_THROWS(queue1.setPriority("zzz", 4), gum::NotFound);

      TS_ASSERT_THROWS(queue1.priority("zzz"), gum::NotFound);
      TS_ASSERT(queue1.priority("BBB") == 2);

      queue1.resize(10);
      TS_ASSERT(queue1.capacity() == 10);

      const gum::HashTable<std::string, std::vector<gum::Size>> &vals =
          queue1.allValues();
      TS_ASSERT(vals.size() == 4);
    }

    void testMultiCopy() {
      gum::MultiPriorityQueue<std::string> queue1;
      queue1.insert("AAA", 8);
      queue1.insert("BBB", 10);
      queue1.insert("CCC", 2);
      queue1.insert("DDD", 23);
      queue1.insert("EEE", 24);
      queue1.insert("AAA", 20);

      gum::MultiPriorityQueue<std::string> queue2 = queue1;
      TS_ASSERT(queue2.size() == 6);
      TS_ASSERT(queue2.empty() == false);
      TS_ASSERT(queue2.contains("AAA") == true);
      TS_ASSERT(queue2.contains("ZZZ") == false);

      queue2.clear();
      TS_ASSERT(queue2.size() == 0);
      TS_GUM_ASSERT_THROWS_NOTHING(queue2.eraseByPos(10));
      TS_GUM_ASSERT_THROWS_NOTHING(queue2.erase("RRR"));

      queue2 = queue1;
      queue2.eraseTop();
      queue2 = queue1;
      TS_ASSERT(queue2.size() == 6);
      TS_ASSERT(queue2.empty() == false);
      TS_ASSERT(queue2.contains("AAA") == true);
      TS_ASSERT(queue2.contains("ZZZ") == false);
      queue2 = queue1;
      TS_ASSERT(queue2.size() == 6);
      TS_ASSERT(queue2.empty() == false);
      TS_ASSERT(queue2.contains("AAA") == true);
      TS_ASSERT(queue2.contains("ZZZ") == false);

      queue2.eraseTop();
      TS_ASSERT(queue2.topPriority() == 8);
      queue2.eraseByPos(0);
      TS_ASSERT(queue2.topPriority() == 10);
      queue2.eraseByPos(3);
      TS_ASSERT(queue2.size() == 3);
      queue2.erase("AAA");
      TS_ASSERT(queue2.size() == 2);

      const std::string &str1 = queue2[0];
      const std::string &str2 = queue2[1];
      TS_ASSERT(str1 == "BBB");
      TS_ASSERT(str2 == "DDD");
    }

    void testMultiConstObj() {
      gum::Set<int> set1, set2, set3, set4, set5, set6;
      set1 << 1;
      set2 << 1 << 2;
      set3 << 1 << 2 << 3;
      set4 << 1 << 2 << 3 << 4;
      set5 << 1 << 2 << 3 << 4 << 5;

      gum::MultiPriorityQueue<gum::Set<int>> queue1;
      TS_ASSERT(queue1.size() == 0);
      TS_ASSERT(queue1.empty() == true);
      TS_ASSERT(queue1.contains(set1) == false);
      TS_ASSERT_THROWS(queue1.top(), gum::NotFound);
      TS_ASSERT_THROWS(queue1.pop(), gum::NotFound);
      TS_ASSERT_THROWS(queue1.topPriority(), gum::NotFound);

      queue1.insert(set1, 8);
      queue1.insert(set2, 10);
      queue1.insert(set3, 2);
      queue1.insert(set4, 23);
      queue1.insert(set5, 24);
      queue1.insert(set1, 10);

      TS_ASSERT(queue1.size() == 6);
      TS_ASSERT(queue1.empty() == false);
      TS_ASSERT(queue1.contains(set1) == true);
      TS_ASSERT(queue1.contains(set6) == false);

      const gum::Set<int> &str = queue1.top();
      TS_ASSERT(str == set3);
      TS_ASSERT(queue1.topPriority() == 2);

      gum::Set<int> str2 = queue1.pop();
      TS_ASSERT(str2 == set3);
      TS_ASSERT(queue1.top() == set1);
      TS_ASSERT(queue1.topPriority() == 8);

      queue1.setPriorityByPos(0, 4);
      TS_ASSERT(queue1.top() == set1);
      TS_ASSERT(queue1.topPriority() == 4);
      TS_ASSERT_THROWS(queue1.setPriorityByPos(10, 4), gum::NotFound);

      queue1.setPriority(set2, 2);
      TS_ASSERT(queue1.top() == set2);
      TS_ASSERT(queue1.topPriority() == 2);
      TS_ASSERT_THROWS(queue1.setPriority(set6, 4), gum::NotFound);

      TS_ASSERT_THROWS(queue1.priority(set6), gum::NotFound);
      TS_ASSERT(queue1.priority(set2) == 2);

      queue1.resize(10);
      TS_ASSERT(queue1.capacity() == 10);

      const gum::HashTable<gum::Set<int>, std::vector<gum::Size>> &vals =
          queue1.allValues();
      TS_ASSERT(vals.size() == 4);

      queue1.clear();
      queue1.insert(set1, 8);
      queue1.insert(set2, 10);
      queue1.insert(set3, 2);
      queue1.insert(set4, 23);
      queue1.insert(set5, 24);
      queue1.insert(set1, 10);

      gum::MultiPriorityQueue<gum::Set<int>> queue2 = queue1;
      queue2 = queue1;
      queue2.eraseTop();
      queue2 = queue1;

      TS_ASSERT(queue2.size() == 6);
      TS_ASSERT(queue2.empty() == false);
      TS_ASSERT(queue2.contains(set1) == true);
      TS_ASSERT(queue2.contains(set6) == false);
      queue2 = queue1;
      TS_ASSERT(queue2.size() == 6);
      TS_ASSERT(queue2.empty() == false);
      TS_ASSERT(queue2.contains(set1) == true);
      TS_ASSERT(queue2.contains(set6) == false);

      queue2.eraseTop();
      TS_ASSERT(queue2.topPriority() == 8);
      queue2.eraseByPos(0);
      TS_ASSERT(queue2.topPriority() == 10);
      queue2.eraseByPos(3);
      TS_ASSERT(queue2.size() == 3);
      queue2.erase(set1);
      TS_ASSERT(queue2.size() == 2);

      const gum::Set<int> &str3 = queue2[0];
      const gum::Set<int> &str4 = queue2[1];
      TS_ASSERT(str3 == set2);
      TS_ASSERT(str4 == set4);
    }

    void testMultiMoveScalar() {
      gum::MultiPriorityQueue<int> queue1{std::pair<int, int>(1, 3),
                                          std::pair<int, int>(2, 1),
                                          std::pair<int, int>(3, 10)};

      gum::MultiPriorityQueue<int, int, std::less<int>, MyAlloc<int>> queue3;
      queue3 = queue1;
      TS_ASSERT(queue3.top() == 2);
      TS_ASSERT(queue3.topPriority() == 1);
      TS_ASSERT(queue3.size() == 3);

      gum::MultiPriorityQueue<int, int, std::less<int>, MyAlloc<int>> queue4(queue1);
      TS_ASSERT(queue4.top() == 2);
      TS_ASSERT(queue4.topPriority() == 1);
      TS_ASSERT(queue4.size() == 3);

      gum::MultiPriorityQueue<int> queue2 = std::move(queue1);
      TS_ASSERT(queue2.top() == 2);
      TS_ASSERT(queue2.topPriority() == 1);
      TS_ASSERT(queue2.size() == 3);

      gum::MultiPriorityQueue<int, int, std::less<int>, MyAlloc<int>> queue5(
          std::move(queue4));
      TS_ASSERT(queue5.top() == 2);
      TS_ASSERT(queue5.topPriority() == 1);
      TS_ASSERT(queue5.size() == 3);

      gum::MultiPriorityQueue<int> queue6{std::pair<int, int>(2, 1)};
      queue6 = std::move(queue2);
      TS_ASSERT(queue6.top() == 2);
      TS_ASSERT(queue6.topPriority() == 1);
      TS_ASSERT(queue6.size() == 3);
    }
  };

} /* namespace gum_tests */
