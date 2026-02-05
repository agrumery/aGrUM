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
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/list.h>
#include <agrum/base/core/set.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Set
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct SetTestSuite {
    public:
    static void testConstructor() {
      gum::Set< int >* set = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(set = new gum::Set< int >());
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete set);

      gum::Set< int > set2{1, 2, 3};
      set = new gum::Set< int >(set2);
      CHECK((*set) == (set2));
      delete set;

      gum::Set< int > set3{1, 2, 3};
      set = new gum::Set< int >(set3);
      CHECK((*set) == (set3));
      delete set;

      set = new gum::Set< int >{1, 2, 3};
      gum::Set< int > set4(std::move(*set));
      delete set;
      CHECK((set4.size()) == (static_cast< gum::Size >(3)));

      set4.clear();
      set4 = set2;
      CHECK((set4.size()) == (static_cast< gum::Size >(3)));
      set4.clear();
      CHECK((set4.size()) == (static_cast< gum::Size >(0)));
      set4 = set3;
      CHECK((set4) == (set3));
      set4.clear();
      set4 = std::move(set2);
      CHECK((set4.size()) == (static_cast< gum::Size >(3)));
    }   // namespace gum_tests

    static void testMoves() {
      gum::Set< int > set1{1, 2, 3};
      gum::Set< int > set2{4, 5, 7};
      gum::Set< int > set3{8, 10};
      gum::Set< int > set4{20, 50};

      gum::Set< int > set5 = std::move(set4);
      set4                 = std::move(set3);
      set3                 = std::move(set2);
      set2                 = std::move(set1);

      CHECK((set2.size()) == (static_cast< gum::Size >(3)));
    }

    static void testInsert() {
      gum::Set< std::string > set;
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.insert("a"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.insert("b"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.insert("c"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.insert("d"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.insert("e"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.insert("f"));
      CHECK(set.exists("f"));

      std::string x = "aaa";
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.insert(x));
      CHECK(set.exists(x));
      CHECK(set.exists("aaa"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(set.emplace("xxx"));
      CHECK(set.exists("xxx"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(set << "ab");
      x = "ac";
      GUM_CHECK_ASSERT_THROWS_NOTHING(set << x);
    }

    static void testEquality() {
      gum::Set< int > t1, t2, t3;
      fill(t1);
      fill(t2);
      fill(t3);

      CHECK((t1) == (t1));
      CHECK((t2) == (t2));
      CHECK((t3) == (t3));

      CHECK((t1) == (t2));
      CHECK((t2) == (t1));
      CHECK((t1) == (t3));
      CHECK((t3) == (t1));
      CHECK((t2) == (t3));
      CHECK((t3) == (t2));

      t2.erase(1);
      t2.erase(3);
      t2.erase(5);

      t3.erase(2);
      t3.erase(4);
      t3.erase(6);

      CHECK((t1) != (t2));
      CHECK((t2) != (t1));
      CHECK((t1) != (t3));
      CHECK((t3) != (t1));
      CHECK((t2) != (t3));
      CHECK((t3) != (t2));

      gum::Set< int > t4, t5;
      fill(t4);
      CHECK((t1) == (t4));
      CHECK((t2) != (t4));
      CHECK((t4) == (t1));
      CHECK((t5) != (t4));
    }

    static void testSize() {
      gum::Set< int > set;

      CHECK((set.size()) == (static_cast< gum::Size >(0)));
      fill(set);
      CHECK((set.size()) == (static_cast< gum::Size >(6)));
    }

    static void testErase() {
      gum::Set< int > set;
      fill(set);

      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(4));
      CHECK((set.size()) == (static_cast< gum::Size >(5)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(6));
      CHECK((set.size()) == (static_cast< gum::Size >(4)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(1));
      CHECK((set.size()) == (static_cast< gum::Size >(3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(3));
      CHECK((set.size()) == (static_cast< gum::Size >(2)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(5));
      CHECK((set.size()) == (static_cast< gum::Size >(1)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(-23));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(10000));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(42));

      CHECK((set.size()) == (static_cast< gum::Size >(1)));

      gum::Set< int > set2;
      fill(set2);
      GUM_CHECK_ASSERT_THROWS_NOTHING(set2 >> 4);
    }

    static void testIterator() {
      gum::Set< int > set;
      fill(set);

      gum::Set< int >::iterator iter1 = set.begin();
      gum::SetIterator< int >   iter2 = set.begin();
      CHECK((iter1) == (iter2));
      CHECK((*iter1) == (3));
      CHECK((iter1) != (set.end()));
      CHECK((iter2) != (set.end()));

      gum::Set< int >::iterator iter3 = set.begin();
      gum::SetIterator< int >   iter4 = set.begin();
      CHECK((iter3) == (iter4));
      CHECK((*iter3) == (3));
      CHECK((iter3) != (set.end()));
      CHECK((iter4) != (set.end()));

      gum::Set< int >::const_iterator iter5 = set.cbegin();
      gum::SetConstIterator< int >    iter6 = set.cbegin();
      CHECK((iter5) == (iter6));
      CHECK((*iter5) == (3));
      CHECK((iter5) != (set.end()));
      CHECK((iter6) != (set.end()));

      gum::Set< int >::const_iterator iter7 = set.cbegin();
      gum::SetConstIterator< int >    iter8 = set.cbegin();
      CHECK((iter7) == (iter8));
      CHECK((*iter8) == (3));
      CHECK((iter7) != (set.cend()));
      CHECK((iter8) != (set.cend()));
    }

    static void testEraseIterator() {
      gum::Set< int > set;
      fill(set);
      CHECK((set.size()) == (static_cast< gum::Size >(6)));

      auto iter = set.beginSafe();   // safe iterator needed here
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(iter));
      CHECK((set.size()) == (static_cast< gum::Size >(5)));

      ++iter;
      ++iter;
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(iter));
      CHECK((set.size()) == (static_cast< gum::Size >(4)));

      iter = set.begin();
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(iter));
      CHECK((set.size()) == (static_cast< gum::Size >(3)));

      iter = set.end();
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(iter));
      CHECK((set.size()) == (static_cast< gum::Size >(3)));

      gum::Set< int > set2;
      fill(set2);
      CHECK((set2.size()) == (static_cast< gum::Size >(6)));
      iter = set2.beginSafe();   // safe iterator needed here
      GUM_CHECK_ASSERT_THROWS_NOTHING(set2.erase(iter));
      CHECK((set2.size()) == (static_cast< gum::Size >(5)));
    }

    static void testClear() {
      gum::Set< int > set;

      CHECK((set.size()) == (static_cast< gum::Size >(0)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.clear());
      CHECK((set.size()) == (static_cast< gum::Size >(0)));

      fill(set);
      CHECK((set.size()) == (static_cast< gum::Size >(6)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.clear());
      CHECK((set.size()) == (static_cast< gum::Size >(0)));
    }

    static void testIsEmpty() {
      gum::Set< int > set;

      CHECK(set.empty());
      fill(set);
      CHECK(!set.empty());
      GUM_CHECK_ASSERT_THROWS_NOTHING(set.clear());
      CHECK(set.empty());
    }

    static void testExist() {
      gum::Set< int > set;

      CHECK(!set.contains(1));
      CHECK(!set.contains(42));

      fill(set);

      CHECK(set.contains(1));
      CHECK(set.contains(2));
      CHECK(set.contains(3));
      CHECK(set.contains(4));
      CHECK(set.contains(5));
      CHECK(set.contains(6));

      GUM_CHECK_ASSERT_THROWS_NOTHING(set.erase(4));

      CHECK(set.contains(1));
      CHECK(set.contains(2));
      CHECK(set.contains(3));
      CHECK(!set.contains(4));
      CHECK(set.contains(5));
      CHECK(set.contains(6));
    }

    static void testCopyOperator() {
      gum::Set< int > t1, t2, t3;
      fill(t1);

      GUM_CHECK_ASSERT_THROWS_NOTHING(t2 = t1);
      GUM_CHECK_ASSERT_THROWS_NOTHING(t3 = t2);

      CHECK((t1) == (t2));
      CHECK((t3) == (t2));
      CHECK((t1) == (t3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(t2.clear());

      CHECK((t1) != (t2));
      CHECK((t2) != (t3));
      CHECK((t1) == (t3));
    }

    static void testIntersectionOperator() {
      gum::Set< int > t1, t2, t3, empty, inter;
      gum::Set< int > fakeInter;
      fill(t1);
      fillEven(t2);
      fillOdd(t3);

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeInter = t1 * t2);
      inter = getIntersection(fakeInter);
      CHECK((inter) == (t2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeInter = t1 * t3);
      inter = getIntersection(fakeInter);
      CHECK((inter) == (t3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeInter = t1 * t1);
      inter = getIntersection(fakeInter);
      CHECK((inter) == (t1));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeInter = t2 * t2);
      inter = getIntersection(fakeInter);
      CHECK((inter) == (t2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeInter = t3 * t3);
      inter = getIntersection(fakeInter);
      CHECK((inter) == (t3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeInter = t2 * t3);
      inter = getIntersection(fakeInter);
      CHECK((inter) == (empty));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeInter = t3 * t2);
      inter = getIntersection(fakeInter);
      CHECK((inter) == (empty));
    }

    static void testUnionOperator() {
      gum::Set< int > t1, t2, t3, empty, unionSet;
      gum::Set< int > fakeUnion;
      fill(t1);
      fillEven(t2);
      fillOdd(t3);

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeUnion = empty + t2);
      unionSet = getUnion(fakeUnion);
      CHECK((unionSet) == (t2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeUnion = empty + t3);
      unionSet = getUnion(fakeUnion);
      CHECK((unionSet) == (t3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeUnion = t1 + t1);
      unionSet = getUnion(fakeUnion);
      CHECK((unionSet) == (t1));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeUnion = t2 + t2);
      unionSet = getUnion(fakeUnion);
      CHECK((unionSet) == (t2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeUnion = t3 + t3);
      unionSet = getUnion(fakeUnion);
      CHECK((unionSet) == (t3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeUnion = t2 + t3);
      unionSet = getUnion(fakeUnion);
      CHECK((unionSet) == (t1));

      GUM_CHECK_ASSERT_THROWS_NOTHING(fakeUnion = t3 + t2);
      unionSet = getUnion(fakeUnion);
      CHECK((unionSet) == (t1));
    }

    static void testDisjunctionOperator() {
      gum::Set< int > t1, t2, t3, empty, disjunction;
      fill(t1);
      fillEven(t2);
      fillOdd(t3);

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = t1 - t2);
      CHECK((disjunction) == (t3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = t1 - t3);
      CHECK((disjunction) == (t2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = t1 - t1);
      CHECK((disjunction) == (empty));

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = t2 - t2);
      CHECK((disjunction) == (empty));

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = t3 - t3);
      CHECK((disjunction) == (empty));

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = t2 - t3);
      CHECK((disjunction) == (t2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = t2 - empty);
      CHECK((disjunction) == (t2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = t3 - t2);
      CHECK((disjunction) == (t3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = t3 - empty);
      CHECK((disjunction) == (t3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = empty - t1);
      CHECK((disjunction) == (empty));

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = empty - t2);
      CHECK((disjunction) == (empty));

      GUM_CHECK_ASSERT_THROWS_NOTHING(disjunction = empty - t3);
      CHECK((disjunction) == (empty));
    }

    static void testMap1() {
      gum::Set< std::string > t1;
      t1 << "a"
         << "b"
         << "c"
         << "d";
      gum::HashTable< std::string, std::string > map1, map4;

      GUM_CHECK_ASSERT_THROWS_NOTHING(map1 = t1.hashMap(&mappingTestFunc_1));
      std::string str = "Space, the final frontiere.";
      GUM_CHECK_ASSERT_THROWS_NOTHING(map4 = t1.hashMap(str));

      gum::List< std::string > map2;
      GUM_CHECK_ASSERT_THROWS_NOTHING(map2 = t1.listMap(&mappingTestFunc_1));
    }

    static void testIterator_1() {
      gum::Set< int > t1;
      fill(t1);

      gum::Set< int > obtained;

      for (gum::Set< int >::iterator iter = t1.begin(); iter != t1.end(); ++iter) {
        obtained.insert(*iter);
      }

      CHECK((t1) == (obtained));

      gum::Set< int > obtained2;

      for (gum::Set< int >::iterator iter = t1.begin(); iter != t1.end(); ++iter) {
        obtained2.insert(*iter);
      }

      CHECK((t1) == (obtained2));

      obtained.clear();
      obtained2.clear();

      for (gum::Set< int >::const_iterator iter = t1.cbegin(); iter != t1.cend(); ++iter) {
        obtained.insert(*iter);
      }

      CHECK((t1) == (obtained));

      for (gum::Set< int >::const_iterator iter = t1.cbegin(); iter != t1.cend(); ++iter) {
        obtained2.insert(*iter);
      }

      CHECK((t1) == (obtained2));

      gum::Set< int >::iterator       iter1;
      gum::Set< int >::iterator       iter2(t1);
      gum::Set< int >::const_iterator iter3(iter1);
      gum::Set< int >::const_iterator iter4(iter2);
      CHECK((iter4) == (iter2));
      gum::Set< int >::const_iterator iter5 = t1.begin();
      gum::Set< int >::const_iterator iter6(iter5);
      CHECK((*iter5) == (*iter6));
      gum::Set< int >::const_iterator iter7(std::move(iter6));
      CHECK((*iter7) == (*(t1.begin())));

      iter7 = iter5;
      CHECK((*iter7) == (*iter5));
      iter7 = std::move(iter4);
      CHECK((*iter7) == (*iter2));
      iter7 = iter3;
      CHECK((iter7) == (iter3));

      iter7 = t1.begin();
      iter1 = t1.begin();
      iter7 += 3;
      ++iter1;
      ++iter1;
      ++iter1;
      CHECK((*iter7) == (*iter1));
      CHECK((iter7) == (t1.begin() + 3));
    }

    static void testIterator_2() {
      gum::Set< int > t1;
      fill(t1);

      gum::Set< int > obtained;

      for (const auto i: t1)
        obtained.insert(i);

      CHECK((t1) == (obtained));
      obtained.clear();

      for (gum::Set< int >::iterator iter = t1.begin(); iter != t1.end(); ++iter) {
        obtained.insert(*iter);
      }

      CHECK((t1) == (obtained));

      obtained.clear();

      for (gum::Set< int >::const_iterator iter = t1.cbegin(); iter != t1.cend(); ++iter) {
        obtained.insert(*iter);
      }

      CHECK((t1) == (obtained));

      gum::Set< int >::iterator       iter1;
      gum::Set< int >::iterator       iter2(t1);
      gum::Set< int >::const_iterator iter3(iter1);
      gum::Set< int >::const_iterator iter4(iter2);
      CHECK((iter4) == (iter2));
      gum::Set< int >::const_iterator iter5 = t1.begin();
      gum::Set< int >::const_iterator iter6(iter5);
      CHECK((*iter5) == (*iter6));
      gum::Set< int >::const_iterator iter7(std::move(iter6));
      CHECK((*iter7) == (*(t1.begin())));

      iter7 = iter5;
      CHECK((*iter7) == (*iter5));
      iter7 = std::move(iter4);
      CHECK((*iter7) == (*iter2));
      iter7 = iter3;
      CHECK((iter7) == (iter3));

      iter7 = t1.begin();
      iter1 = t1.begin();
      iter7 += 3;
      ++iter1;
      ++iter1;
      ++iter1;
      CHECK((*iter7) == (*iter1));
      CHECK((iter7) == (t1.begin() + 3));
    }

    static void testInitializerList() {
      gum::Set< int > t{1};
      CHECK((t.size()) == (1u));
      CHECK((t.toString()) == ("{1}"));
    }

    static void testSubsetSuperset() {
      gum::Set< int > t1{1, 2, 3};
      gum::Set< int > t2{1, 3};
      gum::Set< int > t3{4};

      CHECK(!t1.isStrictSubsetOf(t1));
      CHECK(!t1.isStrictSubsetOf(t2));
      CHECK(!t1.isStrictSubsetOf(t3));

      CHECK(t2.isStrictSubsetOf(t1));
      CHECK(!t2.isStrictSubsetOf(t2));
      CHECK(!t2.isStrictSubsetOf(t3));

      CHECK(!t3.isStrictSubsetOf(t1));
      CHECK(!t3.isStrictSubsetOf(t2));
      CHECK(!t3.isStrictSubsetOf(t3));

      CHECK(!t1.isStrictSupersetOf(t1));
      CHECK(t1.isStrictSupersetOf(t2));
      CHECK(!t1.isStrictSupersetOf(t3));

      CHECK(!t2.isStrictSupersetOf(t1));
      CHECK(!t2.isStrictSupersetOf(t2));
      CHECK(!t2.isStrictSupersetOf(t3));

      CHECK(!t3.isStrictSupersetOf(t1));
      CHECK(!t3.isStrictSupersetOf(t2));
      CHECK(!t3.isStrictSupersetOf(t3));
    }

    private:
    static void fill(gum::Set< int >& set) {
      set.insert(1);
      set.insert(2);
      set.insert(3);
      set.insert(4);
      set.insert(5);
      set.insert(6);
    }

    static void fillEven(gum::Set< int >& set) {
      set.insert(2);
      set.insert(4);
      set.insert(6);
    }

    static void fillOdd(gum::Set< int >& set) {
      set.insert(1);
      set.insert(3);
      set.insert(5);
    }

    static gum::Set< int > getIntersection(gum::Set< int >& set) {
      gum::Set< int > full;
      fill(full);
      gum::Set< int > inter;

      for (int i = 1; i < 7; i++) {
        if (set.contains(i)) { inter.insert(i); }
      }

      return inter;
    }

    static gum::Set< int > getUnion(gum::Set< int >& set) {
      gum::Set< int > full;
      fill(full);
      gum::Set< int > unionSet;

      for (int i = 1; i < 7; i++) {
        if (set.contains(i)) { unionSet.insert(i); }
      }

      return unionSet;
    }

    static std::string mappingTestFunc_1(const std::string& s) { return s + ".foo"; }

    static std::string mappingTestFunc_2(std::string& s) { return s + ".bar"; }

    static std::string mappingTestFunc_3(std::string s) { return s + ".42"; }
  };

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(Moves)
  GUM_TEST_ACTIF(Insert)
  GUM_TEST_ACTIF(Equality)
  GUM_TEST_ACTIF(Size)
  GUM_TEST_ACTIF(Erase)
  GUM_TEST_ACTIF(Iterator)
  GUM_TEST_ACTIF(EraseIterator)
  GUM_TEST_ACTIF(Clear)
  GUM_TEST_ACTIF(IsEmpty)
  GUM_TEST_ACTIF(Exist)
  GUM_TEST_ACTIF(CopyOperator)
  GUM_TEST_ACTIF(IntersectionOperator)
  GUM_TEST_ACTIF(UnionOperator)
  GUM_TEST_ACTIF(DisjunctionOperator)
  GUM_TEST_ACTIF(Map1)
  GUM_TEST_ACTIF(Iterator_1)
  GUM_TEST_ACTIF(Iterator_2)
  GUM_TEST_ACTIF(InitializerList)
  GUM_TEST_ACTIF(SubsetSuperset)
}   // namespace gum_tests
