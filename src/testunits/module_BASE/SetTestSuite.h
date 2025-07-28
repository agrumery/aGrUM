/****************************************************************************
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/list.h>
#include <agrum/base/core/set.h>

namespace gum_tests {

  class [[maybe_unused]] SetTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Constructor) {
      gum::Set< int >* set = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(set = new gum::Set< int >())
      TS_GUM_ASSERT_THROWS_NOTHING(delete set)

      gum::Set< int > set2{1, 2, 3};
      set = new gum::Set< int >(set2);
      TS_ASSERT_EQUALS(*set, set2)
      delete set;

      gum::Set< int > set3{1, 2, 3};
      set = new gum::Set< int >(set3);
      TS_ASSERT_EQUALS(*set, set3)
      delete set;

      set = new gum::Set< int >{1, 2, 3};
      gum::Set< int > set4(std::move(*set));
      delete set;
      TS_ASSERT_EQUALS(set4.size(), (gum::Size)3)

      set4.clear();
      set4 = set2;
      TS_ASSERT_EQUALS(set4.size(), (gum::Size)3)
      set4.clear();
      TS_ASSERT_EQUALS(set4.size(), (gum::Size)0)
      set4 = set3;
      TS_ASSERT_EQUALS(set4, set3)
      set4.clear();
      set4 = std::move(set2);
      TS_ASSERT_EQUALS(set4.size(), (gum::Size)3)
    }

    GUM_ACTIVE_TEST(Moves) {
      gum::Set< int > set1{1, 2, 3};
      gum::Set< int > set2{4, 5, 7};
      gum::Set< int > set3{8, 10};
      gum::Set< int > set4{20, 50};

      gum::Set< int > set5 = std::move(set4);
      set4                 = std::move(set3);
      set3                 = std::move(set2);
      set2                 = std::move(set1);

      TS_ASSERT_EQUALS(set2.size(), (gum::Size)3)
    }

    GUM_ACTIVE_TEST(Insert) {
      gum::Set< std::string > set;
      TS_GUM_ASSERT_THROWS_NOTHING(set.insert("a"))
      TS_GUM_ASSERT_THROWS_NOTHING(set.insert("b"))
      TS_GUM_ASSERT_THROWS_NOTHING(set.insert("c"))
      TS_GUM_ASSERT_THROWS_NOTHING(set.insert("d"))
      TS_GUM_ASSERT_THROWS_NOTHING(set.insert("e"))
      TS_GUM_ASSERT_THROWS_NOTHING(set.insert("f"))
      TS_ASSERT(set.exists("f"))

      std::string x = "aaa";
      TS_GUM_ASSERT_THROWS_NOTHING(set.insert(x))
      TS_ASSERT(set.exists(x))
      TS_ASSERT(set.exists("aaa"))

      TS_GUM_ASSERT_THROWS_NOTHING(set.emplace("xxx"))
      TS_ASSERT(set.exists("xxx"))

      TS_GUM_ASSERT_THROWS_NOTHING(set << "ab")
      x = "ac";
      TS_GUM_ASSERT_THROWS_NOTHING(set << x)
    }

    GUM_ACTIVE_TEST(Equality) {
      gum::Set< int > t1, t2, t3;
      fill(t1);
      fill(t2);
      fill(t3);

      TS_ASSERT_EQUALS(t1, t1)
      TS_ASSERT_EQUALS(t2, t2)
      TS_ASSERT_EQUALS(t3, t3)

      TS_ASSERT_EQUALS(t1, t2)
      TS_ASSERT_EQUALS(t2, t1)
      TS_ASSERT_EQUALS(t1, t3)
      TS_ASSERT_EQUALS(t3, t1)
      TS_ASSERT_EQUALS(t2, t3)
      TS_ASSERT_EQUALS(t3, t2)

      t2.erase(1);
      t2.erase(3);
      t2.erase(5);

      t3.erase(2);
      t3.erase(4);
      t3.erase(6);

      TS_ASSERT_DIFFERS(t1, t2)
      TS_ASSERT_DIFFERS(t2, t1)
      TS_ASSERT_DIFFERS(t1, t3)
      TS_ASSERT_DIFFERS(t3, t1)
      TS_ASSERT_DIFFERS(t2, t3)
      TS_ASSERT_DIFFERS(t3, t2)

      gum::Set< int > t4, t5;
      fill(t4);
      TS_ASSERT_EQUALS(t1, t4)
      TS_ASSERT_DIFFERS(t2, t4)
      TS_ASSERT_EQUALS(t4, t1)
      TS_ASSERT_DIFFERS(t5, t4)
    }

    GUM_ACTIVE_TEST(Size) {
      gum::Set< int > set;

      TS_ASSERT_EQUALS(set.size(), (gum::Size)0)
      fill(set);
      TS_ASSERT_EQUALS(set.size(), (gum::Size)6)
    }

    GUM_ACTIVE_TEST(Erase) {
      gum::Set< int > set;
      fill(set);

      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(4))
      TS_ASSERT_EQUALS(set.size(), (gum::Size)5)
      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(6))
      TS_ASSERT_EQUALS(set.size(), (gum::Size)4)
      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(1))
      TS_ASSERT_EQUALS(set.size(), (gum::Size)3)
      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(3))
      TS_ASSERT_EQUALS(set.size(), (gum::Size)2)
      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(5))
      TS_ASSERT_EQUALS(set.size(), (gum::Size)1)

      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(4))
      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(-23))
      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(10000))
      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(42))

      TS_ASSERT_EQUALS(set.size(), (gum::Size)1)

      gum::Set< int > set2;
      fill(set2);
      TS_GUM_ASSERT_THROWS_NOTHING(set2 >> 4)
    }

    GUM_ACTIVE_TEST(Iterator) {
      gum::Set< int > set;
      fill(set);

      gum::Set< int >::iterator iter1 = set.begin();
      gum::SetIterator< int >   iter2 = set.begin();
      TS_ASSERT_EQUALS(iter1, iter2)
      TS_ASSERT_EQUALS(*iter1, 3)
      TS_ASSERT_DIFFERS(iter1, set.end())
      TS_ASSERT_DIFFERS(iter2, set.end())

      gum::Set< int >::iterator iter3 = set.begin();
      gum::SetIterator< int >   iter4 = set.begin();
      TS_ASSERT_EQUALS(iter3, iter4)
      TS_ASSERT_EQUALS(*iter3, 3)
      TS_ASSERT_DIFFERS(iter3, set.end())
      TS_ASSERT_DIFFERS(iter4, set.end())

      gum::Set< int >::const_iterator iter5 = set.cbegin();
      gum::SetConstIterator< int >    iter6 = set.cbegin();
      TS_ASSERT_EQUALS(iter5, iter6)
      TS_ASSERT_EQUALS(*iter5, 3)
      TS_ASSERT_DIFFERS(iter5, set.end())
      TS_ASSERT_DIFFERS(iter6, set.end())

      gum::Set< int >::const_iterator iter7 = set.cbegin();
      gum::SetConstIterator< int >    iter8 = set.cbegin();
      TS_ASSERT_EQUALS(iter7, iter8)
      TS_ASSERT_EQUALS(*iter8, 3)
      TS_ASSERT_DIFFERS(iter7, set.cend())
      TS_ASSERT_DIFFERS(iter8, set.cend())
    }

    GUM_ACTIVE_TEST(EraseIterator) {
      gum::Set< int > set;
      fill(set);
      TS_ASSERT_EQUALS(set.size(), (gum::Size)6)

      auto iter = set.beginSafe();   // safe iterator needed here
      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(iter))
      TS_ASSERT_EQUALS(set.size(), (gum::Size)5)

      ++iter;
      ++iter;
      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(iter))
      TS_ASSERT_EQUALS(set.size(), (gum::Size)4)

      iter = set.begin();
      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(iter))
      TS_ASSERT_EQUALS(set.size(), (gum::Size)3)

      iter = set.end();
      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(iter))
      TS_ASSERT_EQUALS(set.size(), (gum::Size)3)

      gum::Set< int > set2;
      fill(set2);
      TS_ASSERT_EQUALS(set2.size(), (gum::Size)6)
      iter = set2.beginSafe();   // safe iterator needed here
      TS_GUM_ASSERT_THROWS_NOTHING(set2.erase(iter))
      TS_ASSERT_EQUALS(set2.size(), (gum::Size)5)
    }

    GUM_ACTIVE_TEST(Clear) {
      gum::Set< int > set;

      TS_ASSERT_EQUALS(set.size(), (gum::Size)0)
      TS_GUM_ASSERT_THROWS_NOTHING(set.clear())
      TS_ASSERT_EQUALS(set.size(), (gum::Size)0)

      fill(set);
      TS_ASSERT_EQUALS(set.size(), (gum::Size)6)
      TS_GUM_ASSERT_THROWS_NOTHING(set.clear())
      TS_ASSERT_EQUALS(set.size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(IsEmpty) {
      gum::Set< int > set;

      TS_ASSERT(set.empty())
      fill(set);
      TS_ASSERT(!set.empty())
      TS_GUM_ASSERT_THROWS_NOTHING(set.clear())
      TS_ASSERT(set.empty())
    }

    GUM_ACTIVE_TEST(Exist) {
      gum::Set< int > set;

      TS_ASSERT(!set.contains(1))
      TS_ASSERT(!set.contains(42))

      fill(set);

      TS_ASSERT(set.contains(1))
      TS_ASSERT(set.contains(2))
      TS_ASSERT(set.contains(3))
      TS_ASSERT(set.contains(4))
      TS_ASSERT(set.contains(5))
      TS_ASSERT(set.contains(6))

      TS_GUM_ASSERT_THROWS_NOTHING(set.erase(4))

      TS_ASSERT(set.contains(1))
      TS_ASSERT(set.contains(2))
      TS_ASSERT(set.contains(3))
      TS_ASSERT(!set.contains(4))
      TS_ASSERT(set.contains(5))
      TS_ASSERT(set.contains(6))
    }

    GUM_ACTIVE_TEST(CopyOperator) {
      gum::Set< int > t1, t2, t3;
      fill(t1);

      TS_GUM_ASSERT_THROWS_NOTHING(t2 = t1)
      TS_GUM_ASSERT_THROWS_NOTHING(t3 = t2)

      TS_ASSERT_EQUALS(t1, t2)
      TS_ASSERT_EQUALS(t3, t2)
      TS_ASSERT_EQUALS(t1, t3)

      TS_GUM_ASSERT_THROWS_NOTHING(t2.clear())

      TS_ASSERT_DIFFERS(t1, t2)
      TS_ASSERT_DIFFERS(t2, t3)
      TS_ASSERT_EQUALS(t1, t3)
    }

    GUM_ACTIVE_TEST(IntersectionOperator) {
      gum::Set< int > t1, t2, t3, empty, inter;
      gum::Set< int > fakeInter;
      fill(t1);
      fillEven(t2);
      fillOdd(t3);

      TS_GUM_ASSERT_THROWS_NOTHING(fakeInter = t1 * t2)
      inter = getIntersection(fakeInter);
      TS_ASSERT_EQUALS(inter, t2)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeInter = t1 * t3)
      inter = getIntersection(fakeInter);
      TS_ASSERT_EQUALS(inter, t3)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeInter = t1 * t1)
      inter = getIntersection(fakeInter);
      TS_ASSERT_EQUALS(inter, t1)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeInter = t2 * t2)
      inter = getIntersection(fakeInter);
      TS_ASSERT_EQUALS(inter, t2)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeInter = t3 * t3)
      inter = getIntersection(fakeInter);
      TS_ASSERT_EQUALS(inter, t3)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeInter = t2 * t3)
      inter = getIntersection(fakeInter);
      TS_ASSERT_EQUALS(inter, empty)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeInter = t3 * t2)
      inter = getIntersection(fakeInter);
      TS_ASSERT_EQUALS(inter, empty)
    }

    GUM_ACTIVE_TEST(UnionOperator) {
      gum::Set< int > t1, t2, t3, empty, unionSet;
      gum::Set< int > fakeUnion;
      fill(t1);
      fillEven(t2);
      fillOdd(t3);

      TS_GUM_ASSERT_THROWS_NOTHING(fakeUnion = empty + t2)
      unionSet = getUnion(fakeUnion);
      TS_ASSERT_EQUALS(unionSet, t2)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeUnion = empty + t3)
      unionSet = getUnion(fakeUnion);
      TS_ASSERT_EQUALS(unionSet, t3)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeUnion = t1 + t1)
      unionSet = getUnion(fakeUnion);
      TS_ASSERT_EQUALS(unionSet, t1)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeUnion = t2 + t2)
      unionSet = getUnion(fakeUnion);
      TS_ASSERT_EQUALS(unionSet, t2)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeUnion = t3 + t3)
      unionSet = getUnion(fakeUnion);
      TS_ASSERT_EQUALS(unionSet, t3)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeUnion = t2 + t3)
      unionSet = getUnion(fakeUnion);
      TS_ASSERT_EQUALS(unionSet, t1)

      TS_GUM_ASSERT_THROWS_NOTHING(fakeUnion = t3 + t2)
      unionSet = getUnion(fakeUnion);
      TS_ASSERT_EQUALS(unionSet, t1)
    }

    GUM_ACTIVE_TEST(DisjunctionOperator) {
      gum::Set< int > t1, t2, t3, empty, disjunction;
      fill(t1);
      fillEven(t2);
      fillOdd(t3);

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = t1 - t2)
      TS_ASSERT_EQUALS(disjunction, t3)

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = t1 - t3)
      TS_ASSERT_EQUALS(disjunction, t2)

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = t1 - t1)
      TS_ASSERT_EQUALS(disjunction, empty)

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = t2 - t2)
      TS_ASSERT_EQUALS(disjunction, empty)

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = t3 - t3)
      TS_ASSERT_EQUALS(disjunction, empty)

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = t2 - t3)
      TS_ASSERT_EQUALS(disjunction, t2)

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = t2 - empty)
      TS_ASSERT_EQUALS(disjunction, t2)

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = t3 - t2)
      TS_ASSERT_EQUALS(disjunction, t3)

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = t3 - empty)
      TS_ASSERT_EQUALS(disjunction, t3)

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = empty - t1)
      TS_ASSERT_EQUALS(disjunction, empty)

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = empty - t2)
      TS_ASSERT_EQUALS(disjunction, empty)

      TS_GUM_ASSERT_THROWS_NOTHING(disjunction = empty - t3)
      TS_ASSERT_EQUALS(disjunction, empty)
    }

    GUM_ACTIVE_TEST(Map1) {
      gum::Set< std::string > t1;
      t1 << "a"
         << "b"
         << "c"
         << "d";
      gum::HashTable< std::string, std::string > map1, map4;

      TS_GUM_ASSERT_THROWS_NOTHING(map1 = t1.hashMap(&mappingTestFunc_1))
      std::string str = "Space, the final frontiere.";
      TS_GUM_ASSERT_THROWS_NOTHING(map4 = t1.hashMap(str))

      gum::List< std::string > map2;
      TS_GUM_ASSERT_THROWS_NOTHING(map2 = t1.listMap(&mappingTestFunc_1))
    }

    GUM_ACTIVE_TEST(Iterator_1) {
      gum::Set< int > t1;
      fill(t1);

      gum::Set< int > obtained;

      for (gum::Set< int >::iterator iter = t1.begin(); iter != t1.end(); ++iter) {
        obtained.insert(*iter);
      }

      TS_ASSERT_EQUALS(t1, obtained)

      gum::Set< int > obtained2;

      for (gum::Set< int >::iterator iter = t1.begin(); iter != t1.end(); ++iter) {
        obtained2.insert(*iter);
      }

      TS_ASSERT_EQUALS(t1, obtained2)

      obtained.clear();
      obtained2.clear();

      for (gum::Set< int >::const_iterator iter = t1.cbegin(); iter != t1.cend(); ++iter) {
        obtained.insert(*iter);
      }

      TS_ASSERT_EQUALS(t1, obtained)

      for (gum::Set< int >::const_iterator iter = t1.cbegin(); iter != t1.cend(); ++iter) {
        obtained2.insert(*iter);
      }

      TS_ASSERT_EQUALS(t1, obtained2)

      gum::Set< int >::iterator       iter1;
      gum::Set< int >::iterator       iter2(t1);
      gum::Set< int >::const_iterator iter3(iter1);
      gum::Set< int >::const_iterator iter4(iter2);
      TS_ASSERT_EQUALS(iter4, iter2)
      gum::Set< int >::const_iterator iter5 = t1.begin();
      gum::Set< int >::const_iterator iter6(iter5);
      TS_ASSERT_EQUALS(*iter5, *iter6)
      gum::Set< int >::const_iterator iter7(std::move(iter6));
      TS_ASSERT_EQUALS(*iter7, *(t1.begin()))

      iter7 = iter5;
      TS_ASSERT_EQUALS(*iter7, *iter5)
      iter7 = std::move(iter4);
      TS_ASSERT_EQUALS(*iter7, *iter2)
      iter7 = iter3;
      TS_ASSERT_EQUALS(iter7, iter3)

      iter7 = t1.begin();
      iter1 = t1.begin();
      iter7 += 3;
      ++iter1;
      ++iter1;
      ++iter1;
      TS_ASSERT_EQUALS(*iter7, *iter1)
      TS_ASSERT_EQUALS(iter7, t1.begin() + 3)
    }

    GUM_ACTIVE_TEST(Iterator_2) {
      gum::Set< int > t1;
      fill(t1);

      gum::Set< int > obtained;

      for (const auto i: t1)
        obtained.insert(i);

      TS_ASSERT_EQUALS(t1, obtained)
      obtained.clear();

      for (gum::Set< int >::iterator iter = t1.begin(); iter != t1.end(); ++iter) {
        obtained.insert(*iter);
      }

      TS_ASSERT_EQUALS(t1, obtained)

      obtained.clear();

      for (gum::Set< int >::const_iterator iter = t1.cbegin(); iter != t1.cend(); ++iter) {
        obtained.insert(*iter);
      }

      TS_ASSERT_EQUALS(t1, obtained)

      gum::Set< int >::iterator       iter1;
      gum::Set< int >::iterator       iter2(t1);
      gum::Set< int >::const_iterator iter3(iter1);
      gum::Set< int >::const_iterator iter4(iter2);
      TS_ASSERT_EQUALS(iter4, iter2)
      gum::Set< int >::const_iterator iter5 = t1.begin();
      gum::Set< int >::const_iterator iter6(iter5);
      TS_ASSERT_EQUALS(*iter5, *iter6)
      gum::Set< int >::const_iterator iter7(std::move(iter6));
      TS_ASSERT_EQUALS(*iter7, *(t1.begin()))

      iter7 = iter5;
      TS_ASSERT_EQUALS(*iter7, *iter5)
      iter7 = std::move(iter4);
      TS_ASSERT_EQUALS(*iter7, *iter2)
      iter7 = iter3;
      TS_ASSERT_EQUALS(iter7, iter3)

      iter7 = t1.begin();
      iter1 = t1.begin();
      iter7 += 3;
      ++iter1;
      ++iter1;
      ++iter1;
      TS_ASSERT_EQUALS(*iter7, *iter1)
      TS_ASSERT_EQUALS(iter7, t1.begin() + 3)
    }

    GUM_ACTIVE_TEST(InitializerList) {
      gum::Set< int > t{1};
      TS_ASSERT_EQUALS(t.size(), 1u)
      TS_ASSERT_EQUALS(t.toString(), "{1}")
    }

    GUM_ACTIVE_TEST(SubsetSuperset) {
      gum::Set< int > t1{1, 2, 3};
      gum::Set< int > t2{1, 3};
      gum::Set< int > t3{4};

      TS_ASSERT(!t1.isStrictSubsetOf(t1))
      TS_ASSERT(!t1.isStrictSubsetOf(t2))
      TS_ASSERT(!t1.isStrictSubsetOf(t3))

      TS_ASSERT(t2.isStrictSubsetOf(t1))
      TS_ASSERT(!t2.isStrictSubsetOf(t2))
      TS_ASSERT(!t2.isStrictSubsetOf(t3))

      TS_ASSERT(!t3.isStrictSubsetOf(t1))
      TS_ASSERT(!t3.isStrictSubsetOf(t2))
      TS_ASSERT(!t3.isStrictSubsetOf(t3))

      TS_ASSERT(!t1.isStrictSupersetOf(t1))
      TS_ASSERT(t1.isStrictSupersetOf(t2))
      TS_ASSERT(!t1.isStrictSupersetOf(t3))

      TS_ASSERT(!t2.isStrictSupersetOf(t1))
      TS_ASSERT(!t2.isStrictSupersetOf(t2))
      TS_ASSERT(!t2.isStrictSupersetOf(t3))

      TS_ASSERT(!t3.isStrictSupersetOf(t1))
      TS_ASSERT(!t3.isStrictSupersetOf(t2))
      TS_ASSERT(!t3.isStrictSupersetOf(t3))
    }

    private:
    void fill(gum::Set< int >& set) {
      set.insert(1);
      set.insert(2);
      set.insert(3);
      set.insert(4);
      set.insert(5);
      set.insert(6);
    }

    void fillEven(gum::Set< int >& set) {
      set.insert(2);
      set.insert(4);
      set.insert(6);
    }

    void fillOdd(gum::Set< int >& set) {
      set.insert(1);
      set.insert(3);
      set.insert(5);
    }

    gum::Set< int > getIntersection(gum::Set< int >& set) {
      gum::Set< int > full;
      fill(full);
      gum::Set< int > inter;

      for (int i = 1; i < 7; i++) {
        if (set.contains(i)) { inter.insert(i); }
      }

      return inter;
    }

    gum::Set< int > getUnion(gum::Set< int >& set) {
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
}   // namespace gum_tests
