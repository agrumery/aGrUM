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


#include <algorithm>
#include <iterator>
#include <list>
#include <vector>

#include <agrum/base/core/list.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct ListTestSuite {
    public:
    // namespace gum_tests


    protected:
    /**
     * Initialize a list with integers from 1 to count.
     * If count < 1, then initialize from count to 1.
     */
    static gum::List< int > initializeList(int count) {
      gum::List< int > list;

      if (count >= 1) {
        for (int i = 1; i <= count; i++)
          list.pushBack(i);

        return list;
      } else {
        for (int i = count; i <= 1; i++)
          list.pushBack(i);

        return list;
      }
    }
  };

  GUM_TEST(PrivateMethods) {
    GUM_CHECK_ASSERT_THROWS_NOTHING(initializeList(7));
    GUM_CHECK_ASSERT_THROWS_NOTHING(initializeList(-7));
  }

  GUM_TEST(Constructor) {
    gum::List< int > liste1{1, 2, 3, 4};
    CHECK_EQ(liste1.size(), static_cast< gum::Size >(4));
    CHECK_EQ(liste1.front(), 1);
    CHECK_EQ(liste1.back(), 4);
  }

  GUM_TEST(Moves) {
    gum::List< int > liste1{1, 2, 3, 4};
    gum::List< int > liste2{7, 8};
    gum::List< int > liste3{1, 9, 10, 20, 30, 40};

    gum::List< int > liste4 = std::move(liste3);
    liste3                  = std::move(liste2);
    liste2                  = std::move(liste1);

    CHECK_EQ(liste2.size(), static_cast< gum::Size >(4));
    CHECK_EQ(liste2.front(), 1);
    CHECK_EQ(liste2.back(), 4);
  }

  GUM_TEST(Push) {
    gum::List< int > liste1;
    liste1.pushFront(3);
    CHECK_EQ(liste1.size(), static_cast< gum::Size >(1));

    liste1.pushBack(4);
    liste1.pushBack(5);
    CHECK_EQ(liste1.size(), static_cast< gum::Size >(3));

    int x = 4;
    liste1.pushFront(x);
    liste1.push_front(x);
    liste1.push_front(5);
    CHECK_EQ(liste1.size(), static_cast< gum::Size >(6));
    CHECK_EQ(liste1.front(), 5);

    liste1.pushBack(x);
    liste1.push_back(x);
    liste1.push_back(5);
    CHECK_EQ(liste1.size(), static_cast< gum::Size >(9));
    CHECK_EQ(liste1.back(), 5);

    liste1.emplaceFront(4);
    liste1.emplaceBack(4);
    liste1.emplaceFront(x);
    liste1.emplaceBack(x);
    CHECK_EQ(liste1.size(), static_cast< gum::Size >(13));
  }

  GUM_TEST(Copy) {
    gum::List< int > liste1;
    liste1.pushFront(3);
    liste1.pushFront(4);
    liste1.pushFront(5);

    gum::List< int > liste2(liste1);
    gum::List< int > liste3 = liste1;

    CHECK_EQ(liste2.size(), liste1.size());
    CHECK_EQ(liste2.size(), liste1.size());

    liste1.pushBack(5);
    liste1.pushBack(5);
    liste1.pushBack(6);
    liste2 = liste1;
    gum::List< int > liste4;
    liste3 = liste4;

    CHECK_EQ(liste2.size(), liste1.size());
    CHECK_EQ(liste3.size(), static_cast< gum::Size >(0));
  }

  GUM_TEST(Move) {
    gum::List< int > liste1;
    liste1.pushFront(3);
    liste1.pushFront(4);
    liste1.pushFront(5);

    gum::List< int > liste2(std::move(liste1));
    gum::List< int > liste3;
    CHECK_EQ(liste2.size(), static_cast< gum::Size >(3));
    CHECK_EQ(liste1.size(), static_cast< gum::Size >(0));
    liste3 = std::move(liste2);
    CHECK_EQ(liste3.size(), static_cast< gum::Size >(3));
    CHECK_EQ(liste2.size(), static_cast< gum::Size >(0));
    CHECK_EQ(liste3.front(), 5);
  }

  GUM_TEST(Insert) {
    gum::List< int > list = initializeList(6);
    list.insert(7);

    CHECK_EQ(list.size(), static_cast< gum::Size >(7));
  }

  GUM_TEST(Front) {
    gum::List< int > list = initializeList(7);
    CHECK_EQ(list.front(), 1);
    list.front() = 34;
    CHECK_EQ(list.front(), 34);
  }

  GUM_TEST(Back) {
    gum::List< int > list = initializeList(7);
    CHECK_EQ(list.back(), 7);
    list.back() = 42;
    CHECK_EQ(list.back(), 42);
  }

  GUM_TEST(size) {
    gum::List< int > list = initializeList(7);
    CHECK_EQ(list.size(), static_cast< gum::Size >(7));
  }

  GUM_TEST(Exists) {
    gum::List< int > list = initializeList(7);
    CHECK(list.exists(3));
    CHECK(!list.exists(42));
  }

  GUM_TEST(Insert2) {
    gum::List< int > list1{1, 2, 3, 4};
    gum::List< int > list2{2, 4};
    list2.insert(0, 1);
    list2.insert(2, 3);
    CHECK_EQ(list1, list2);

    gum::List< int > list3{2, 4};
    int              x = 1, y = 3;
    list3.insert(0, x);
    list3.insert(2, y);
    CHECK_EQ(list3, list1);

    list1.pushBack(5);
    gum::List< int >::iterator iter1 = list2.rbegin();
    list2.insert(iter1, 5, gum::List< int >::location::AFTER);
    CHECK_EQ(list2, list1);

    gum::List< int >           list4{1, 2, 3, 5};
    gum::List< int >::iterator iter2 = list4.rbegin();
    list4.insert(iter2, 4);
    CHECK_EQ(list4, list1);

    list1.pushBack(6);
    gum::List< int >::iterator iter3 = list2.rbegin();
    list2.insert(iter3, 6, gum::List< int >::location::AFTER);
    CHECK_EQ(list2, list1);

    gum::List< int >           list5{1, 2, 3, 4, 6};
    gum::List< int >::iterator iter4 = list5.rbegin();
    list5.insert(iter4, 5);
    CHECK_EQ(list5, list1);

    list1.pushBack(7);
    list5.emplaceBack(7);
    CHECK_EQ(list5, list1);
    list1.pushBack(8);
    gum::List< int > list6{1, 2, 3, 4, 5, 6, 8};
    iter4 = list6.rbegin();
    list6.emplace(iter4, 7);
    CHECK_EQ(list6, list1);
  }

  GUM_TEST(EraseByVal) {
    gum::List< int > list = initializeList(7);
    GUM_CHECK_ASSERT_THROWS_NOTHING(list.eraseByVal(20));
    list.pushBack(3);
    list.eraseByVal(3);
    CHECK_EQ(list.size(), static_cast< gum::Size >(7));
  }

  GUM_TEST(EraseAllVal) {
    gum::List< int > list;

    for (int i = 0; i < 7; i++)
      list.insert(42);

    list.pushFront(1);

    list.pushBack(2);

    list.eraseAllVal(42);

    CHECK_EQ(list.size(), static_cast< gum::Size >(2));
  }

  GUM_TEST(Erase) {
    gum::List< int > list;

    for (int i = 0; i < 7; i++)
      list.insert(i);

    list.erase(2);

    CHECK_EQ(list.size(), static_cast< gum::Size >(6));

    list.erase(2);

    CHECK_EQ(list.size(), static_cast< gum::Size >(5));

    list.erase(20);

    CHECK_EQ(list.size(), static_cast< gum::Size >(5));
  }

  GUM_TEST(EraseIterator) {
    gum::List< int > list;

    for (int i = 0; i < 6; i++)
      list.insert(i);

    auto iter = list.beginSafe();   // safe iterator needed here

    list.erase(iter);

    CHECK_EQ(list.size(), static_cast< gum::Size >(5));

    list.erase(iter);

    CHECK_EQ(list.size(), static_cast< gum::Size >(5));

    ++iter;

    list.erase(iter);

    CHECK_EQ(list.size(), static_cast< gum::Size >(4));

    iter = list.endSafe();   // safe iterator needed here

    list.erase(iter);

    CHECK_EQ(list.size(), static_cast< gum::Size >(4));

    iter = list.rendSafe();   // safe iterator needed here

    list.erase(iter);

    CHECK_EQ(list.size(), static_cast< gum::Size >(4));

    iter = list.rbeginSafe();   // safe iterator needed here

    list.erase(iter);

    CHECK_EQ(list.size(), static_cast< gum::Size >(3));

    iter = list.beginSafe();   // safe iterator needed here

    list.erase(++iter);

    CHECK_EQ(list.size(), static_cast< gum::Size >(2));

    iter = list.rbeginSafe();   // safe iterator needed here

    list.erase(--iter);

    CHECK_EQ(list.size(), static_cast< gum::Size >(1));
  }

  GUM_TEST(IteratorStep) {
    gum::List< int > list{1, 2, 3, 4, 5, 6};
    auto             iter = list.beginSafe();   // safe iterator needed here
    list.erase(iter);
    CHECK_EQ(list.size(), static_cast< gum::Size >(5));

    iter = list.rbeginSafe();   // safe iterator needed here
    list.erase(iter);
    CHECK_EQ(list.size(), static_cast< gum::Size >(4));

    auto iter2 = list.cbeginSafe();   // safe iterator needed here
    list.erase(iter2);
    CHECK_EQ(list.size(), static_cast< gum::Size >(3));

    iter2 = list.crbeginSafe();   // safe iterator needed here
    list.erase(iter2);
    CHECK_EQ(list.size(), static_cast< gum::Size >(2));

    iter2 = list.beginSafe();   // safe iterator needed here
    list.erase(iter2);
    CHECK_EQ(list.size(), static_cast< gum::Size >(1));

    iter2 = list.crbeginSafe();   // safe iterator needed here
    list.erase(iter2);
    CHECK_EQ(list.size(), static_cast< gum::Size >(0));
  }

  GUM_TEST(IteratorStep2) {
    gum::List< int >           xlist{1, 2, 3, 4, 5, 6, 7, 8};
    gum::List< int >::iterator iter = xlist.begin();
    iter += 4;
    CHECK_EQ(*iter, 5);
    iter += 1;
    CHECK_EQ(*iter, 6);
    iter -= 1;
    CHECK_EQ(*iter, 5);
    iter -= 10;
    CHECK_EQ(iter, xlist.end());

    gum::List< int >::iterator iter2 = xlist.begin();
    iter2 += 4;
    CHECK_EQ(*iter2, 5);
    iter2 += 1;
    CHECK_EQ(*iter2, 6);
    iter2 -= 1;
    CHECK_EQ(*iter2, 5);
    iter2 -= 10;
    CHECK_EQ(iter2, xlist.end());

    {
      gum::List< int >::const_iterator iter3 = xlist.cbegin() + 4;
      CHECK_EQ(*iter3, 5);
      gum::List< int >::const_iterator iter4 = xlist.begin() + 4;
      CHECK_EQ(*iter4, 5);
      gum::List< int >::iterator iter5 = xlist.begin() + 4;
      CHECK_EQ(*iter5, 5);

      gum::List< int >::const_iterator iter13 = xlist.cbegin() + 4;
      CHECK_EQ(*iter13, 5);
      gum::List< int >::const_iterator iter14 = xlist.begin() + 4;
      CHECK_EQ(*iter14, 5);
      gum::List< int >::iterator iter15 = xlist.begin() + 4;
      CHECK_EQ(*iter15, 5);
    }

    {
      gum::List< int >::const_iterator iter3 = xlist.crbegin() - 3;
      CHECK_EQ(*iter3, 5);
      gum::List< int >::const_iterator iter4 = xlist.rbegin() - 3;
      CHECK_EQ(*iter4, 5);
      gum::List< int >::iterator iter5 = xlist.rbegin() - 3;
      CHECK_EQ(*iter5, 5);

      gum::List< int >::const_iterator iter13 = xlist.crbegin() - 3;
      CHECK_EQ(*iter13, 5);
      gum::List< int >::const_iterator iter14 = xlist.rbegin() - 3;
      CHECK_EQ(*iter14, 5);
      gum::List< int >::iterator iter15 = xlist.rbegin() - 3;
      CHECK_EQ(*iter15, 5);
    }
  }

  GUM_TEST(PopBack) {
    gum::List< int > list = initializeList(7);
    int              last = list.back();
    list.popBack();

    CHECK_NE(list.back(), last);
  }

  GUM_TEST(PopFront) {
    gum::List< int > list  = initializeList(7);
    int              first = list.front();
    list.popFront();

    CHECK_NE(list.front(), first);
  }

  GUM_TEST(STL) {
    gum::List< int > xlist{1, 2, 3, 4, 5, 6, 7, 8};
    int              xx = 0;
    std::for_each(xlist.begin(), xlist.end(), [&xx](int x) { xx += x; });
    CHECK_EQ(xx, 36);

    std::vector< int > v;
    std::copy(xlist.begin(), xlist.end(), std::back_inserter(v));
    CHECK_EQ(v.size(), size_t(8));

    gum::List< int > list2;
    std::copy(v.begin(), v.end(), std::back_inserter(list2));
    CHECK_EQ(list2, xlist);

    gum::List< int >                 list3{2, 1, 8, 5, 3, 6, 4, 7};
    gum::List< int >::const_iterator iter1 = list3.cbegin();
    gum::List< int >::const_iterator iter2 = list3.cbegin() + 4;
    int                              d1    = int(iter2 - iter1);
    CHECK_EQ(d1, 4);

    gum::List< int >::iterator iter3 = list3.begin();
    gum::List< int >::iterator iter4 = list3.begin() + 4;
    int                        d2    = int(iter4 - iter3);
    CHECK_EQ(d2, 4);

    gum::List< int >::const_iterator iter11 = list3.cbegin();
    gum::List< int >::const_iterator iter12 = list3.cbegin() + 4;
    int                              d11    = int(iter12 - iter11);
    CHECK_EQ(d11, 4);

    gum::List< int >::iterator iter13 = list3.begin();
    gum::List< int >::iterator iter14 = list3.begin() + 4;
    int                        d12    = int(iter14 - iter13);
    CHECK_EQ(d12, 4);
  }
}   // namespace gum_tests
