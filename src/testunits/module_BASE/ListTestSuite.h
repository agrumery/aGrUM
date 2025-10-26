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
#pragma once


#include <algorithm>
#include <iterator>
#include <list>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/list.h>

namespace gum_tests {

  class GUM_TEST_SUITE(List) {
    public:
    GUM_ACTIVE_TEST(PrivateMethods){TS_GUM_ASSERT_THROWS_NOTHING(initializeList(7))
                                        TS_GUM_ASSERT_THROWS_NOTHING(initializeList(-7))}

    GUM_ACTIVE_TEST(Constructor) {
      gum::List< int > liste1{1, 2, 3, 4};
      TS_ASSERT_EQUALS(liste1.size(), static_cast< gum::Size >(4))
      TS_ASSERT_EQUALS(liste1.front(), 1)
      TS_ASSERT_EQUALS(liste1.back(), 4)
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(Moves) {
      gum::List< int > liste1{1, 2, 3, 4};
      gum::List< int > liste2{7, 8};
      gum::List< int > liste3{1, 9, 10, 20, 30, 40};

      gum::List< int > liste4 = std::move(liste3);
      liste3                  = std::move(liste2);
      liste2                  = std::move(liste1);

      TS_ASSERT_EQUALS(liste2.size(), static_cast< gum::Size >(4))
      TS_ASSERT_EQUALS(liste2.front(), 1)
      TS_ASSERT_EQUALS(liste2.back(), 4)
    }

    GUM_ACTIVE_TEST(Push) {
      gum::List< int > liste1;
      liste1.pushFront(3);
      TS_ASSERT_EQUALS(liste1.size(), static_cast< gum::Size >(1))

      liste1.pushBack(4);
      liste1.pushBack(5);
      TS_ASSERT_EQUALS(liste1.size(), static_cast< gum::Size >(3))

      int x = 4;
      liste1.pushFront(x);
      liste1.push_front(x);
      liste1.push_front(5);
      TS_ASSERT_EQUALS(liste1.size(), static_cast< gum::Size >(6))
      TS_ASSERT_EQUALS(liste1.front(), 5)

      liste1.pushBack(x);
      liste1.push_back(x);
      liste1.push_back(5);
      TS_ASSERT_EQUALS(liste1.size(), static_cast< gum::Size >(9))
      TS_ASSERT_EQUALS(liste1.back(), 5)

      liste1.emplaceFront(4);
      liste1.emplaceBack(4);
      liste1.emplaceFront(x);
      liste1.emplaceBack(x);
      TS_ASSERT_EQUALS(liste1.size(), static_cast< gum::Size >(13))
    }

    GUM_ACTIVE_TEST(Copy) {
      gum::List< int > liste1;
      liste1.pushFront(3);
      liste1.pushFront(4);
      liste1.pushFront(5);

      gum::List< int > liste2(liste1);
      gum::List< int > liste3 = liste1;

      TS_ASSERT_EQUALS(liste2.size(), liste1.size())
      TS_ASSERT_EQUALS(liste2.size(), liste1.size())

      liste1.pushBack(5);
      liste1.pushBack(5);
      liste1.pushBack(6);
      liste2 = liste1;
      gum::List< int > liste4;
      liste3 = liste4;

      TS_ASSERT_EQUALS(liste2.size(), liste1.size())
      TS_ASSERT_EQUALS(liste3.size(), static_cast< gum::Size >(0))
    }

    GUM_ACTIVE_TEST(Move) {
      gum::List< int > liste1;
      liste1.pushFront(3);
      liste1.pushFront(4);
      liste1.pushFront(5);

      gum::List< int > liste2(std::move(liste1));
      gum::List< int > liste3;
      TS_ASSERT_EQUALS(liste2.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(liste1.size(), static_cast< gum::Size >(0))
      liste3 = std::move(liste2);
      TS_ASSERT_EQUALS(liste3.size(), static_cast< gum::Size >(3))
      TS_ASSERT_EQUALS(liste2.size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(liste3.front(), 5)
    }

    GUM_ACTIVE_TEST(Insert) {
      gum::List< int > list = initializeList(6);
      list.insert(7);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(7))
    }

    GUM_ACTIVE_TEST(Front) {
      gum::List< int > list = initializeList(7);
      TS_ASSERT_EQUALS(list.front(), 1)
      list.front() = 34;
      TS_ASSERT_EQUALS(list.front(), 34)
    }

    GUM_ACTIVE_TEST(Back) {
      gum::List< int > list = initializeList(7);
      TS_ASSERT_EQUALS(list.back(), 7)
      list.back() = 42;
      TS_ASSERT_EQUALS(list.back(), 42)
    }

    GUM_ACTIVE_TEST(size) {
      gum::List< int > list = initializeList(7);
      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(7))
    }

    GUM_ACTIVE_TEST(Exists) {
      gum::List< int > list = initializeList(7);
      TS_ASSERT(list.exists(3))
      TS_ASSERT(!list.exists(42))
    }

    GUM_ACTIVE_TEST(Insert2) {
      gum::List< int > list1{1, 2, 3, 4};
      gum::List< int > list2{2, 4};
      list2.insert(0, 1);
      list2.insert(2, 3);
      TS_ASSERT_EQUALS(list1, list2)

      gum::List< int > list3{2, 4};
      int              x = 1, y = 3;
      list3.insert(0, x);
      list3.insert(2, y);
      TS_ASSERT_EQUALS(list3, list1)

      list1.pushBack(5);
      gum::List< int >::iterator iter1 = list2.rbegin();
      list2.insert(iter1, 5, gum::List< int >::location::AFTER);
      TS_ASSERT_EQUALS(list2, list1)

      gum::List< int >           list4{1, 2, 3, 5};
      gum::List< int >::iterator iter2 = list4.rbegin();
      list4.insert(iter2, 4);
      TS_ASSERT_EQUALS(list4, list1)

      list1.pushBack(6);
      gum::List< int >::iterator iter3 = list2.rbegin();
      list2.insert(iter3, 6, gum::List< int >::location::AFTER);
      TS_ASSERT_EQUALS(list2, list1)

      gum::List< int >           list5{1, 2, 3, 4, 6};
      gum::List< int >::iterator iter4 = list5.rbegin();
      list5.insert(iter4, 5);
      TS_ASSERT_EQUALS(list5, list1)

      list1.pushBack(7);
      list5.emplaceBack(7);
      TS_ASSERT_EQUALS(list5, list1)
      list1.pushBack(8);
      gum::List< int > list6{1, 2, 3, 4, 5, 6, 8};
      iter4 = list6.rbegin();
      list6.emplace(iter4, 7);
      TS_ASSERT_EQUALS(list6, list1)
    }

    GUM_ACTIVE_TEST(EraseByVal) {
      gum::List< int > list = initializeList(7);
      TS_GUM_ASSERT_THROWS_NOTHING(list.eraseByVal(20))
      list.pushBack(3);
      list.eraseByVal(3);
      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(7))
    }

    GUM_ACTIVE_TEST(EraseAllVal) {
      gum::List< int > list;

      for (int i = 0; i < 7; i++)
        list.insert(42);

      list.pushFront(1);

      list.pushBack(2);

      list.eraseAllVal(42);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(2))
    }

    GUM_ACTIVE_TEST(Erase) {
      gum::List< int > list;

      for (int i = 0; i < 7; i++)
        list.insert(i);

      list.erase(2);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(6))

      list.erase(2);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(5))

      list.erase(20);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(5))
    }

    GUM_ACTIVE_TEST(EraseIterator) {
      gum::List< int > list;

      for (int i = 0; i < 6; i++)
        list.insert(i);

      auto iter = list.beginSafe();   // safe iterator needed here

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(5))

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(5))

      ++iter;

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(4))

      iter = list.endSafe();   // safe iterator needed here

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(4))

      iter = list.rendSafe();   // safe iterator needed here

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(4))

      iter = list.rbeginSafe();   // safe iterator needed here

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(3))

      iter = list.beginSafe();   // safe iterator needed here

      list.erase(++iter);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(2))

      iter = list.rbeginSafe();   // safe iterator needed here

      list.erase(--iter);

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(1))
    }

    GUM_ACTIVE_TEST(IteratorStep) {
      gum::List< int > list{1, 2, 3, 4, 5, 6};
      auto             iter = list.beginSafe();   // safe iterator needed here
      list.erase(iter);
      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(5))

      iter = list.rbeginSafe();   // safe iterator needed here
      list.erase(iter);
      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(4))

      auto iter2 = list.cbeginSafe();   // safe iterator needed here
      list.erase(iter2);
      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(3))

      iter2 = list.crbeginSafe();   // safe iterator needed here
      list.erase(iter2);
      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(2))

      iter2 = list.beginSafe();   // safe iterator needed here
      list.erase(iter2);
      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(1))

      iter2 = list.crbeginSafe();   // safe iterator needed here
      list.erase(iter2);
      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(0))
    }

    GUM_ACTIVE_TEST(IteratorStep2) {
      gum::List< int >           xlist{1, 2, 3, 4, 5, 6, 7, 8};
      gum::List< int >::iterator iter = xlist.begin();
      iter += 4;
      TS_ASSERT_EQUALS(*iter, 5)
      iter += 1;
      TS_ASSERT_EQUALS(*iter, 6)
      iter -= 1;
      TS_ASSERT_EQUALS(*iter, 5)
      iter -= 10;
      TS_ASSERT_EQUALS(iter, xlist.end())

      gum::List< int >::iterator iter2 = xlist.begin();
      iter2 += 4;
      TS_ASSERT_EQUALS(*iter2, 5)
      iter2 += 1;
      TS_ASSERT_EQUALS(*iter2, 6)
      iter2 -= 1;
      TS_ASSERT_EQUALS(*iter2, 5)
      iter2 -= 10;
      TS_ASSERT_EQUALS(iter2, xlist.end())

      {
        gum::List< int >::const_iterator iter3 = xlist.cbegin() + 4;
        TS_ASSERT_EQUALS(*iter3, 5)
        gum::List< int >::const_iterator iter4 = xlist.begin() + 4;
        TS_ASSERT_EQUALS(*iter4, 5)
        gum::List< int >::iterator iter5 = xlist.begin() + 4;
        TS_ASSERT_EQUALS(*iter5, 5)

        gum::List< int >::const_iterator iter13 = xlist.cbegin() + 4;
        TS_ASSERT_EQUALS(*iter13, 5)
        gum::List< int >::const_iterator iter14 = xlist.begin() + 4;
        TS_ASSERT_EQUALS(*iter14, 5)
        gum::List< int >::iterator iter15 = xlist.begin() + 4;
        TS_ASSERT_EQUALS(*iter15, 5)
      }

      {
        gum::List< int >::const_iterator iter3 = xlist.crbegin() - 3;
        TS_ASSERT_EQUALS(*iter3, 5)
        gum::List< int >::const_iterator iter4 = xlist.rbegin() - 3;
        TS_ASSERT_EQUALS(*iter4, 5)
        gum::List< int >::iterator iter5 = xlist.rbegin() - 3;
        TS_ASSERT_EQUALS(*iter5, 5)

        gum::List< int >::const_iterator iter13 = xlist.crbegin() - 3;
        TS_ASSERT_EQUALS(*iter13, 5)
        gum::List< int >::const_iterator iter14 = xlist.rbegin() - 3;
        TS_ASSERT_EQUALS(*iter14, 5)
        gum::List< int >::iterator iter15 = xlist.rbegin() - 3;
        TS_ASSERT_EQUALS(*iter15, 5)
      }
    }

    GUM_ACTIVE_TEST(PopBack) {
      gum::List< int > list = initializeList(7);
      int              last = list.back();
      list.popBack();

      TS_ASSERT_DIFFERS(list.back(), last)
    }

    GUM_ACTIVE_TEST(PopFront) {
      gum::List< int > list  = initializeList(7);
      int              first = list.front();
      list.popFront();

      TS_ASSERT_DIFFERS(list.front(), first)
    }

    GUM_ACTIVE_TEST(STL) {
      gum::List< int > xlist{1, 2, 3, 4, 5, 6, 7, 8};
      int              xx = 0;
      std::for_each(xlist.begin(), xlist.end(), [&xx](int x) { xx += x; });
      TS_ASSERT_EQUALS(xx, 36)

      std::vector< int > v;
      std::copy(xlist.begin(), xlist.end(), std::back_inserter(v));
      TS_ASSERT_EQUALS(v.size(), size_t(8))

      gum::List< int > list2;
      std::copy(v.begin(), v.end(), std::back_inserter(list2));
      TS_ASSERT_EQUALS(list2, xlist)

      gum::List< int >                 list3{2, 1, 8, 5, 3, 6, 4, 7};
      gum::List< int >::const_iterator iter1 = list3.cbegin();
      gum::List< int >::const_iterator iter2 = list3.cbegin() + 4;
      int                              d1    = int(iter2 - iter1);
      TS_ASSERT_EQUALS(d1, 4)

      gum::List< int >::iterator iter3 = list3.begin();
      gum::List< int >::iterator iter4 = list3.begin() + 4;
      int                        d2    = int(iter4 - iter3);
      TS_ASSERT_EQUALS(d2, 4)

      gum::List< int >::const_iterator iter11 = list3.cbegin();
      gum::List< int >::const_iterator iter12 = list3.cbegin() + 4;
      int                              d11    = int(iter12 - iter11);
      TS_ASSERT_EQUALS(d11, 4)

      gum::List< int >::iterator iter13 = list3.begin();
      gum::List< int >::iterator iter14 = list3.begin() + 4;
      int                        d12    = int(iter14 - iter13);
      TS_ASSERT_EQUALS(d12, 4)
    }

    private:
    /**
     * Initialize a list with integers from 1 to count.
     * If count < 1, then initialize from count to 1.
     */
    gum::List< int > initializeList(int count) {
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
}   // namespace gum_tests
