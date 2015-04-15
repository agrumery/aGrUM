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

#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
#include <ressources/myalloc.h>

#include <agrum/core/list.h>
#include <agrum/core/hashTable.h>

namespace gum_tests {

  class HashTableTestSuite : public CxxTest::TestSuite {
    public:
    void testConstructor() {
      gum::HashTable<int, int> *table = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING((table = new gum::HashTable<int, int>()));
      TS_GUM_ASSERT_THROWS_NOTHING(delete table);

      gum::HashTable<int, int> t2{std::pair<int, int>(3, 4)};
      table = new gum::HashTable<int, int>(t2);
      TS_ASSERT(*table == t2);

      gum::HashTable<int, int, MyAlloc<std::pair<int, int>>> t3(t2);
      TS_ASSERT(t3 == t2);

      gum::HashTable<int, int> t4(t3);
      TS_ASSERT(t4 == t3);

      gum::HashTable<int, int> t5(std::move(*table));
      delete table;

      t5 = t3;
      TS_ASSERT(t5 == t3);
      t5 = t2;
      TS_ASSERT(t5 == t2);
      t5.clear();
      t5 = std::move(t2);
      TS_ASSERT(t5.size() == 1);
    }

    void testMoves() {
      gum::HashTable<int, int> t1{std::pair<int, int>(3, 4),
                                  std::pair<int, int>(5, 6)};

      gum::HashTable<int, int> t2{std::pair<int, int>(1, 4),
                                  std::pair<int, int>(3, 6),
                                  std::pair<int, int>(2, 7)};

      gum::HashTable<int, int> t3{std::pair<int, int>(2, 7)};

      gum::HashTable<int, int> t4 = std::move(t3);
      t3 = std::move(t2);
      t2 = std::move(t1);

      TS_ASSERT(t2.size() == 2);
      TS_ASSERT(t2.exists(5));
    }

    void testInsert() {
      gum::HashTable<int, std::string> table;
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(1, "a"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(2, "b"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(3, "c"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(4, "d"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(5, "e"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(6, "f"));

      int nb = 7;
      std::string str = "kkk";
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(nb, str));
      TS_ASSERT(table.exists(1));
      TS_ASSERT(table.exists(7));

      std::pair<int, std::string> p(8, "toto");
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(p));
      TS_ASSERT(table.exists(8));

      TS_GUM_ASSERT_THROWS_NOTHING(
          table.insert(std::pair<int, std::string>(9, "l")));
      TS_ASSERT(table.exists(9));

      TS_GUM_ASSERT_THROWS_NOTHING(table.emplace(10, "m"));
      TS_ASSERT(table.exists(10));
      TS_ASSERT(!table.exists(11));
    }

    void testEquality() {
      gum::HashTable<int, std::string> t1, t2, t3;
      fill(t1);
      fill(t2);
      fill(t3);

      TS_ASSERT(t1 == t1);
      TS_ASSERT(t2 == t2);
      TS_ASSERT(t3 == t3);

      TS_ASSERT(t1 == t2);
      TS_ASSERT(t2 == t1);
      TS_ASSERT(t1 == t3);
      TS_ASSERT(t3 == t1);
      TS_ASSERT(t2 == t3);
      TS_ASSERT(t3 == t2);

      t2.erase(1);
      t2.erase(3);
      t2.erase(5);

      t3.erase(2);
      t3.erase(4);
      t3.erase(6);

      TS_ASSERT(t1 != t2);
      TS_ASSERT(t2 != t1);
      TS_ASSERT(t1 != t3);
      TS_ASSERT(t3 != t1);
      TS_ASSERT(t2 != t3);
      TS_ASSERT(t3 != t2);
    }

    void testsize() {
      gum::HashTable<int, std::string> table;

      TS_ASSERT_EQUALS(table.size(), (gum::Size)0);
      fill(table);
      TS_ASSERT_EQUALS(table.size(), (gum::Size)6);
    }

    void testErase() {
      gum::HashTable<int, std::string> table;
      fill(table);

      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(4));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)5);
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(6));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)4);
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(1));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)3);
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(3));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)2);
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(5));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)1);

      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(4));
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(-23));
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(10000));
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(42));

      TS_ASSERT_EQUALS(table.size(), (gum::Size)1);
    }

    void testEraseIterator() {
      gum::HashTable<int, std::string> table(4);
      fill(table);
      TS_ASSERT_EQUALS(table.size(), (gum::Size)6);

      gum::HashTable<int, std::string>::iterator_safe iter =
          table.beginSafe(); // safe iterator needed here

      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(iter));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)5);

      ++iter;
      ++iter;

      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(iter));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)4);

      iter = table.beginSafe(); // safe iterator needed here
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(iter));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)3);

      iter = table.endSafe();
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(iter));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)3);

      iter = table.endSafe();
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(iter));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)3);
    }

    void testEraseByVal() {
      gum::HashTable<int, std::string> table;
      fill(table);

      TS_GUM_ASSERT_THROWS_NOTHING(table.eraseByVal("a"));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)5);
      TS_GUM_ASSERT_THROWS_NOTHING(table.eraseByVal("d"));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)4);
      TS_GUM_ASSERT_THROWS_NOTHING(table.eraseByVal("e"));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)3);
      TS_GUM_ASSERT_THROWS_NOTHING(table.eraseByVal("b"));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)2);
      TS_GUM_ASSERT_THROWS_NOTHING(table.eraseByVal("f"));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)1);

      TS_GUM_ASSERT_THROWS_NOTHING(table.eraseByVal("f"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.eraseByVal("foo"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.eraseByVal("bar"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.eraseByVal("42"));

      TS_ASSERT_EQUALS(table.size(), (gum::Size)1);
    }

    void testEraseAllVal() {
      gum::HashTable<int, std::string> table;

      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(1, "Space, the final frontiere"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(2, "Space, the final frontiere"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(3, "Space, the final frontiere"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(4, "Space, the final frontiere"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(5, "Space, the final frontiere"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(6, "Space, the final frontiere"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(7, "Space, the final frontiere"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(8, "Space, the final frontiere"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(9, "Space, the final frontiere"));

      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(10, "foo"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.insert(11, "bar"));

      TS_ASSERT_EQUALS(table.size(), (gum::Size)11);

      TS_GUM_ASSERT_THROWS_NOTHING(table.eraseAllVal("Space, the final frontiere"));

      TS_ASSERT_EQUALS(table.size(), (gum::Size)2);

      TS_GUM_ASSERT_THROWS_NOTHING(table.eraseAllVal("Space, the final frontiere"));
      TS_ASSERT_EQUALS(table.size(), (gum::Size)2);
    }

    void testClear() {
      gum::HashTable<int, std::string> table;

      TS_ASSERT_EQUALS(table.size(), (gum::Size)0);
      TS_GUM_ASSERT_THROWS_NOTHING(table.clear());
      TS_ASSERT_EQUALS(table.size(), (gum::Size)0);

      fill(table);
      TS_ASSERT_EQUALS(table.size(), (gum::Size)6);
      TS_GUM_ASSERT_THROWS_NOTHING(table.clear());
      TS_ASSERT_EQUALS(table.size(), (gum::Size)0);
    }

    void testIsEmpty() {
      gum::HashTable<int, std::string> table;

      TS_ASSERT(table.empty());
      fill(table);
      TS_ASSERT(!table.empty());
      TS_GUM_ASSERT_THROWS_NOTHING(table.clear());
      TS_ASSERT(table.empty());
    }

    void testExist() {
      gum::HashTable<int, std::string> table;

      TS_ASSERT(!table.exists(1));
      TS_ASSERT(!table.exists(42));

      fill(table);

      TS_ASSERT(table.exists(1));
      TS_ASSERT(table.exists(2));
      TS_ASSERT(table.exists(3));
      TS_ASSERT(table.exists(4));
      TS_ASSERT(table.exists(5));
      TS_ASSERT(table.exists(6));

      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(4));

      TS_ASSERT(table.exists(1));
      TS_ASSERT(table.exists(2));
      TS_ASSERT(table.exists(3));
      TS_ASSERT(!table.exists(4));
      TS_ASSERT(table.exists(5));
      TS_ASSERT(table.exists(6));
    }

    void testkey() {
      gum::HashTable<int, std::string> table;
      fill(table);
      int val;

      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(1));
      TS_ASSERT_EQUALS(val, 1);
      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(2));
      TS_ASSERT_EQUALS(val, 2);
      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(3));
      TS_ASSERT_EQUALS(val, 3);
      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(4));
      TS_ASSERT_EQUALS(val, 4);
      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(5));
      TS_ASSERT_EQUALS(val, 5);
      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(6));
      TS_ASSERT_EQUALS(val, 6);

      TS_ASSERT_THROWS_ANYTHING(table.key(42));
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(6));
      TS_ASSERT_THROWS_ANYTHING(table.key(6));
    }

    void testkeyByVal() {
      gum::HashTable<int, std::string> table;
      fill(table);

      int key = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(key = table.keyByVal("a"));
      TS_ASSERT_EQUALS(key, 1);
      TS_GUM_ASSERT_THROWS_NOTHING(key = table.keyByVal("b"));
      TS_ASSERT_EQUALS(key, 2);
      TS_GUM_ASSERT_THROWS_NOTHING(key = table.keyByVal("c"));
      TS_ASSERT_EQUALS(key, 3);
      TS_GUM_ASSERT_THROWS_NOTHING(key = table.keyByVal("d"));
      TS_ASSERT_EQUALS(key, 4);
      TS_GUM_ASSERT_THROWS_NOTHING(key = table.keyByVal("e"));
      TS_ASSERT_EQUALS(key, 5);
      TS_GUM_ASSERT_THROWS_NOTHING(key = table.keyByVal("f"));
      TS_ASSERT_EQUALS(key, 6);

      TS_ASSERT_THROWS_ANYTHING(table.keyByVal("foo"));
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(6));
      TS_ASSERT_THROWS_ANYTHING(table.keyByVal("f"));
    }

    void testCopyOperator() {
      gum::HashTable<int, std::string> t1, t2, t3;
      fill(t1);

      TS_GUM_ASSERT_THROWS_NOTHING(t2 = t1);
      TS_GUM_ASSERT_THROWS_NOTHING(t3 = t2);

      TS_ASSERT_EQUALS(t1, t2);
      TS_ASSERT_EQUALS(t3, t2);
      TS_ASSERT_EQUALS(t1, t3);

      TS_GUM_ASSERT_THROWS_NOTHING(t2.clear());

      TS_ASSERT_DIFFERS(t1, t2);
      TS_ASSERT_DIFFERS(t2, t3);
      TS_ASSERT_EQUALS(t1, t3);
    }

    void testGenCopyOperator() {
      gum::HashTable<int, std::string, MyAlloc<std::pair<int, std::string>>> t2;
      gum::HashTable<int, std::string> t1, t3;
      fill(t1);

      TS_GUM_ASSERT_THROWS_NOTHING(t2 = t1);
      TS_GUM_ASSERT_THROWS_NOTHING(t3 = t2);

      TS_ASSERT_EQUALS(t1, t2);
      TS_ASSERT_EQUALS(t3, t2);
      TS_ASSERT_EQUALS(t1, t3);

      TS_GUM_ASSERT_THROWS_NOTHING(t2.clear());

      TS_ASSERT_DIFFERS(t1, t2);
      TS_ASSERT_DIFFERS(t2, t3);
      TS_ASSERT_EQUALS(t1, t3);
    }

    void testGetOperator() {
      gum::HashTable<int, std::string> t1;
      fill(t1);

      TS_ASSERT_EQUALS(t1[1], "a");
      TS_ASSERT_EQUALS(t1[2], "b");
      TS_ASSERT_EQUALS(t1[3], "c");
      TS_ASSERT_EQUALS(t1[4], "d");
      TS_ASSERT_EQUALS(t1[5], "e");
      TS_ASSERT_EQUALS(t1[6], "f");

      TS_ASSERT_THROWS_ANYTHING(t1[42]);
      TS_ASSERT_THROWS_ANYTHING(t1[-1]);
    }

    void testGetCopyOperator() {
      gum::HashTable<int, std::string> t1, t2;
      fill(t1);

      TS_GUM_ASSERT_THROWS_NOTHING(t2.insert(1, t1[1]));
      TS_GUM_ASSERT_THROWS_NOTHING(t2.insert(2, t1[2]));
      TS_GUM_ASSERT_THROWS_NOTHING(t2.insert(3, t1[3]));
      TS_GUM_ASSERT_THROWS_NOTHING(t2.insert(4, t1[4]));
      TS_GUM_ASSERT_THROWS_NOTHING(t2.insert(5, t1[5]));
      TS_GUM_ASSERT_THROWS_NOTHING(t2.insert(6, t1[6]));

      TS_ASSERT_EQUALS(t1, t2);

      TS_ASSERT_THROWS_ANYTHING(t2.insert(4, t1[42]));
      TS_ASSERT_EQUALS(t2[4], t1[4]);
    }

    void testKeyUniqueness() {
      gum::HashTable<int, std::string> t1, t2;
      fill(t1);
      fill(t2);

      TS_ASSERT(t1.keyUniquenessPolicy());
      TS_ASSERT(t2.keyUniquenessPolicy());

      TS_GUM_ASSERT_THROWS_NOTHING(t2.setKeyUniquenessPolicy(false));
      TS_ASSERT(!t2.keyUniquenessPolicy());
      TS_ASSERT_EQUALS(t1, t2);
    }

    void testResizePolicy() {
      gum::HashTable<int, std::string> t1, t2;
      fill(t1);
      fill(t2);

      TS_ASSERT(t1.resizePolicy());
      TS_ASSERT(t2.resizePolicy());

      TS_GUM_ASSERT_THROWS_NOTHING(t2.setResizePolicy(false));
      TS_ASSERT(!t2.resizePolicy());
      TS_ASSERT_EQUALS(t1, t2);
    }

    void testSize() {
      gum::HashTable<int, int> t1, t2;

      TS_GUM_ASSERT_THROWS_NOTHING(t2.setResizePolicy(false));

      TS_ASSERT_EQUALS(t1.capacity(), (gum::Size)4);
      TS_ASSERT_EQUALS(t2.capacity(), (gum::Size)4);

      for (int i = 0; i < 10000; i++) {
        TS_GUM_ASSERT_THROWS_NOTHING(t1.insert(i, i));
        TS_GUM_ASSERT_THROWS_NOTHING(t2.insert(i, i));
      }

      TS_ASSERT_DIFFERS(t1.capacity(), (gum::Size)4);

      TS_ASSERT_EQUALS(t2.capacity(), (gum::Size)4);
    }

    void testMap() {
      gum::HashTable<int, std::string> t1, map1, map2, map3, map4;
      fill(t1);

      TS_GUM_ASSERT_THROWS_NOTHING(map1 = t1.map(&mappingTestFunc_1));
      TS_GUM_ASSERT_THROWS_NOTHING(map2 = t1.map(&mappingTestFunc_2));
      TS_GUM_ASSERT_THROWS_NOTHING(map3 = t1.map(&mappingTestFunc_3));
      std::string str = "Space, the final frontiere.";
      TS_GUM_ASSERT_THROWS_NOTHING(map4 = t1.map(str));

      for (int i = 1; i < 7; i++) {
        TS_ASSERT_EQUALS(map1[i], t1[i] + ".foo");
        TS_ASSERT_EQUALS(map2[i], t1[i] + ".bar");
        TS_ASSERT_EQUALS(map3[i], t1[i] + ".42");
        TS_ASSERT_EQUALS(map4[i], str);
      }
    }

    void testMapAlloc() {
      gum::HashTable<int, std::string> t1, map1, map2, map4;
      gum::HashTable<int, int> map3, map5;
      fill(t1);

      TS_GUM_ASSERT_THROWS_NOTHING(map1 = t1.map(&mappingTestFunc_1));
      TS_GUM_ASSERT_THROWS_NOTHING(map2 = t1.map(&mappingTestFunc_2));
      TS_GUM_ASSERT_THROWS_NOTHING(map3 = t1.map(&mappingTestFunc_4));
      std::string str = "Space, the final frontiere.";
      TS_GUM_ASSERT_THROWS_NOTHING(map4 = t1.map(str));
      TS_GUM_ASSERT_THROWS_NOTHING(map5 = t1.map(12));

      for (int i = 1; i < 7; i++) {
        TS_ASSERT_EQUALS(map1[i], t1[i] + ".foo");
        TS_ASSERT_EQUALS(map2[i], t1[i] + ".bar");
        TS_ASSERT_EQUALS(map3[i], 2);
        TS_ASSERT_EQUALS(map4[i], str);
      }
    }

    void testIterator_1() {
      gum::HashTable<int, std::string> t1;
      fill(t1);

      gum::List<std::string> expected, obtained;

      for (int i = 1; i < 7; i++) {
        expected.insert(t1[i]);
      }

      for (const auto &elt : t1)
        obtained.insert(elt.second);

      TS_ASSERT_EQUALS(expected.size(), obtained.size());

      for (gum::Size i = 0; i < obtained.size(); i++) {
        TS_ASSERT(expected.exists(obtained[i]));
      }
    }

    void testIterator_2() {
      gum::HashTable<int, std::string> t1;
      fill(t1);

      gum::List<std::string> expected, obtained;

      for (int i = 1; i < 7; i++) {
        expected.insert(t1[i]);
      }

      for (const auto &elt : t1)
        obtained.insert(elt.second);

      TS_ASSERT_EQUALS(expected.size(), obtained.size());

      for (gum::Size i = 0; i < obtained.size(); i++) {
        TS_ASSERT(expected.exists(obtained[i]));
      }
    }

    void test_float_hash() {
      unsigned int size = 20;
      gum::HashTable<float, unsigned int> t1;

      for (unsigned int i = 0; i < size; ++i) {
        float nb = i + i * 0.01;
        t1.insert(nb, i * 1000 + i);
      }

      std::vector<float> vect(size + 1);

      for (unsigned int i = 0; i < vect.size(); ++i) {
        vect[i] = i + i * 0.01;
      }

      for (unsigned int i = 0; i < size; ++i) {
        TS_ASSERT(t1.exists(vect[i]));
        TS_ASSERT(t1[vect[i]] == i * 1000 + i);
      }
    }

    void test_float_pair_hash() {
      unsigned int size = 20;
      gum::HashTable<std::pair<float, float>, unsigned int> t1;

      for (unsigned int i = 0; i < size; ++i) {
        float nb1 = i + i * 0.01;
        float nb2 = i * 2;
        t1.insert(std::pair<float, float>(nb1, nb2), i * 1000 + i);
      }

      std::vector<float> vect(size + 1);

      for (unsigned int i = 0; i < vect.size(); ++i) {
        vect[i] = i + i * 0.01;
      }

      for (unsigned int i = 0; i < size; ++i) {
        std::pair<float, float> thepair(vect[i], 2.0 * i);
        TS_ASSERT(t1.exists(thepair));
        TS_ASSERT(t1[thepair] == i * 1000 + i);
      }
    }

    void test_double_hash() {
      unsigned int size = 20;
      gum::HashTable<double, unsigned int> t1;

      for (unsigned int i = 0; i < size; ++i) {
        double nb = i + i * 0.01;
        t1.insert(nb, i * 1000 + i);
      }

      std::vector<double> vect(size + 1);

      for (unsigned int i = 0; i < vect.size(); ++i) {
        vect[i] = i + i * 0.01;
      }

      for (unsigned int i = 0; i < size; ++i) {
        TS_ASSERT(t1.exists(vect[i]));
        TS_ASSERT(t1[vect[i]] == i * 1000 + i);
      }
    }

    void test_double_pair_hash() {
      unsigned int size = 20;
      gum::HashTable<std::pair<double, double>, unsigned int> t1;

      for (unsigned int i = 0; i < size; ++i) {
        double nb1 = i + i * 0.01;
        double nb2 = i * 2;
        t1.insert(std::pair<double, double>(nb1, nb2), i * 1000 + i);
      }

      std::vector<double> vect(size + 1);

      for (unsigned int i = 0; i < vect.size(); ++i) {
        vect[i] = i + i * 0.01;
      }

      for (unsigned int i = 0; i < size; ++i) {
        std::pair<double, double> thepair(vect[i], 2.0 * i);
        TS_ASSERT(t1.exists(thepair));
        TS_ASSERT(t1[thepair] == i * 1000 + i);
      }
    }

    void testAllocator() {
      {
        gum::HashTable<int, std::string, MyAlloc<std::string>> table;
        table.insert(1, "a");
        table.insert(2, "b");
        table.insert(3, "c");
        table.insert(4, "d");
        table.insert(5, "e");
        table.insert(6, "f");
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);

      {
        gum::HashTable<int, std::string, MyAlloc<std::string>> table;
        table.insert(1, "a");
        table.insert(2, "b");
        table.insert(3, "c");
        table.insert(4, "d");
        table.insert(5, "e");
        table.insert(6, "f");
        table.erase(2);
        table.eraseByVal("a");
        table.erase(4);
        table.eraseAllVal("c");
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);

      {
        gum::HashTable<int, std::string, MyAlloc<std::string>> table;
        table.insert(1, "a");
        table.insert(2, "b");
        table.insert(3, "c");
        table.insert(4, "d");
        table.insert(5, "e");
        table.insert(6, "f");

        std::string s;
        int k = 0;

        for (auto iter = table.cbeginSafe(); iter != table.cendSafe();
             ++iter) { // safe iterator needed here
          s = iter.val();
          k += iter.key();
        }

        TS_ASSERT(k == 21);

        k = 0;

        for (auto iter = table.beginSafe(); iter != table.endSafe();
             ++iter) { // safe iterator needed here
          s = iter.val();
          k += iter.key();
        }

        TS_ASSERT(k == 21);

        k = 0;

        for (auto iter = table.begin(); iter != table.end(); ++iter) {
          s = iter.val();
          k += iter.key();
        }

        TS_ASSERT(k == 21);

        k = 0;

        for (auto iter = table.cbegin(); iter != table.cend(); ++iter) {
          s = iter.val();
          k += iter.key();
        }

        TS_ASSERT(k == 21);

        k = 0;

        for (const auto &elt : table) {
          s = elt.second;
          k += elt.first;
        }

        TS_ASSERT(k == 21);

        gum::HashTableConstIterator<int, std::string> iter1 = table.cbegin();
        gum::HashTableIterator<int, std::string> iter2 = table.begin();
        gum::HashTableConstIteratorSafe<int, std::string> iter3 =
            table.cbeginSafe(); // safe iterator needed here
        gum::HashTableIteratorSafe<int, std::string> iter4 =
            table.beginSafe(); // safe iterator needed here

        // TS_ASSERT ( *iter1 == *iter2 );
        // TS_ASSERT ( *iter3 == *iter4 );
      }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);

      // gum::HashTable<int, int> t2 { 10, false, false };
      // gum::HashTable<int, int> t3 ( t2 );
      // gum::HashTable<int, int> t4 ( gum::HashTable<int, int> { 10 } );
    }

    void testInitializer_list() {
      gum::HashTable<unsigned int, std::string, MyAlloc<std::string>> table{
          std::make_pair(3U, "a"), std::make_pair(2U, "b")};

      TS_ASSERT(table.size() == gum::Size(2));
      TS_ASSERT(table.exists(3U) == true);
      TS_ASSERT(table.exists(2U) == true);
    }

    private:
    void fill(gum::HashTable<int, std::string> &table) {
      table.insert(1, "a");
      table.insert(2, "b");
      table.insert(3, "c");
      table.insert(4, "d");
      table.insert(5, "e");
      table.insert(6, "f");
    }

    void fillEven(gum::HashTable<int, std::string> &table) {
      table.insert(2, "b");
      table.insert(4, "d");
      table.insert(6, "f");
    }

    void fillOdd(gum::HashTable<int, std::string> &table) {
      table.insert(1, "a");
      table.insert(3, "c");
      table.insert(5, "e");
    }

    gum::HashTable<int, std::string>
    getIntersection(gum::HashTable<int, bool> table) {
      gum::HashTable<int, std::string> full;
      fill(full);
      gum::HashTable<int, std::string> inter;

      for (int i = 1; i < 7; i++) {
        if (table.exists(i) && table[i]) {
          inter.insert(i, full[i]);
        }
      }

      return inter;
    }

    gum::HashTable<int, std::string> getUnion(gum::HashTable<int, bool> table) {
      gum::HashTable<int, std::string> full;
      fill(full);
      gum::HashTable<int, std::string> unionTable;

      for (int i = 1; i < 7; i++) {
        if (table.exists(i) && table[i]) {
          unionTable.insert(i, full[i]);
        }
      }

      return unionTable;
    }

    static std::string mappingTestFunc_1(const std::string &s) { return s + ".foo"; }

    static std::string mappingTestFunc_2(std::string &s) { return s + ".bar"; }

    static std::string mappingTestFunc_3(std::string s) { return s + ".42"; }

    static int mappingTestFunc_4(std::string s) { return 2; }
  };
}
