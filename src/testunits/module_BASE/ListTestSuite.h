/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
#include "ressources/myalloc.h"

#include <list>
#include <algorithm>
#include <vector>

#include <agrum/core/list.h>

namespace gum_tests {

  class ListTestSuite : public CxxTest::TestSuite {
    public:
    void testPrivateMethods() {
      TS_GUM_ASSERT_THROWS_NOTHING(initializeList(7));
      TS_GUM_ASSERT_THROWS_NOTHING(initializeList(-7));
    }

    void testConstructor() {
      gum::List<int> liste1{1, 2, 3, 4};
      TS_ASSERT_EQUALS(liste1.size(), (gum::Size)4);
      TS_ASSERT_EQUALS(liste1.front(), 1);
      TS_ASSERT_EQUALS(liste1.back(), 4);
    }

    void testMoves() {
      gum::List<int> liste1{1, 2, 3, 4};
      gum::List<int> liste2{7, 8};
      gum::List<int> liste3{1, 9, 10, 20, 30, 40};

      gum::List<int> liste4 = std::move(liste3);
      liste3 = std::move(liste2);
      liste2 = std::move(liste1);

      TS_ASSERT(liste2.size() == 4);
      TS_ASSERT(liste2.front() == 1);
      TS_ASSERT(liste2.back() == 4);
    }

    void testPush() {
      gum::List<int> liste1;
      liste1.pushFront(3);
      TS_ASSERT_EQUALS(liste1.size(), (gum::Size)1);

      liste1.pushBack(4);
      liste1.pushBack(5);
      TS_ASSERT_EQUALS(liste1.size(), (gum::Size)3);

      int x = 4;
      liste1.pushFront(x);
      liste1.push_front(x);
      liste1.push_front(5);
      TS_ASSERT_EQUALS(liste1.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(liste1.front(), 5);

      liste1.pushBack(x);
      liste1.push_back(x);
      liste1.push_back(5);
      TS_ASSERT_EQUALS(liste1.size(), (gum::Size)9);
      TS_ASSERT_EQUALS(liste1.back(), 5);

      liste1.emplaceFront(4);
      liste1.emplaceBack(4);
      liste1.emplaceFront(x);
      liste1.emplaceBack(x);
      TS_ASSERT_EQUALS(liste1.size(), (gum::Size)13);
    }

    void testCopy() {
      gum::List<int> liste1;
      liste1.pushFront(3);
      liste1.pushFront(4);
      liste1.pushFront(5);

      gum::List<int> liste2(liste1);
      gum::List<int> liste3 = liste1;

      TS_ASSERT_EQUALS(liste2.size(), liste1.size());
      TS_ASSERT_EQUALS(liste2.size(), liste1.size());

      liste1.pushBack(5);
      liste1.pushBack(5);
      liste1.pushBack(6);
      liste2 = liste1;
      gum::List<int> liste4;
      liste3 = liste4;

      TS_ASSERT_EQUALS(liste2.size(), liste1.size());
      TS_ASSERT_EQUALS(liste3.size(), (gum::Size)0);
    }

    void testMove() {
      gum::List<int> liste1;
      liste1.pushFront(3);
      liste1.pushFront(4);
      liste1.pushFront(5);

      gum::List<int> liste2(std::move(liste1));
      gum::List<int> liste3;
      TS_ASSERT_EQUALS(liste2.size(), gum::Size(3));
      TS_ASSERT_EQUALS(liste1.size(), gum::Size(0));
      liste3 = std::move(liste2);
      TS_ASSERT_EQUALS(liste3.size(), gum::Size(3));
      TS_ASSERT_EQUALS(liste2.size(), gum::Size(0));
      TS_ASSERT_EQUALS(liste3.front(), 5);
    }

    void testInsert() {
      gum::List<int> list = initializeList(6);
      list.insert(7);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)7);
    }

    void testFront() {
      gum::List<int> list = initializeList(7);
      TS_ASSERT_EQUALS(list.front(), 1);
      list.front() = 34;
      TS_ASSERT_EQUALS(list.front(), 34);
    }

    void testBack() {
      gum::List<int> list = initializeList(7);
      TS_ASSERT_EQUALS(list.back(), 7);
      list.back() = 42;
      TS_ASSERT_EQUALS(list.back(), 42);
    }

    void testsize() {
      gum::List<int> list = initializeList(7);
      TS_ASSERT_EQUALS(list.size(), (gum::Size)7);
    }

    void testExists() {
      gum::List<int> list = initializeList(7);
      TS_ASSERT(list.exists(3));
      TS_ASSERT(!list.exists(42));
    }

    void testInsert2() {
      gum::List<int> list1{1, 2, 3, 4};
      gum::List<int> list2{2, 4};
      list2.insert(0, 1);
      list2.insert(2, 3);
      TS_ASSERT_EQUALS(list1, list2);

      gum::List<int> list3{2, 4};
      int x = 1, y = 3;
      list3.insert(0, x);
      list3.insert(2, y);
      TS_ASSERT_EQUALS(list3, list1);

      list1.pushBack(5);
      gum::List<int>::iterator iter1 = list2.rbegin();
      list2.insert(iter1, 5, gum::List<int>::location::AFTER);
      TS_ASSERT_EQUALS(list2, list1);

      gum::List<int> list4{1, 2, 3, 5};
      gum::List<int>::iterator iter2 = list4.rbegin();
      list4.insert(iter2, 4);
      TS_ASSERT_EQUALS(list4, list1);

      list1.pushBack(6);
      gum::List<int>::iterator iter3 = list2.rbegin();
      list2.insert(iter3, 6, gum::List<int>::location::AFTER);
      TS_ASSERT_EQUALS(list2, list1);

      gum::List<int> list5{1, 2, 3, 4, 6};
      gum::List<int>::iterator iter4 = list5.rbegin();
      list5.insert(iter4, 5);
      TS_ASSERT_EQUALS(list5, list1);

      list1.pushBack(7);
      list5.emplaceBack(7);
      TS_ASSERT_EQUALS(list5, list1);
      list1.pushBack(8);
      gum::List<int> list6{1, 2, 3, 4, 5, 6, 8};
      iter4 = list6.rbegin();
      list6.emplace(iter4, 7);
      TS_ASSERT_EQUALS(list6, list1);
    }

    void testEraseByVal() {
      gum::List<int> list = initializeList(7);
      TS_GUM_ASSERT_THROWS_NOTHING(list.eraseByVal(20));
      list.pushBack(3);
      list.eraseByVal(3);
      TS_ASSERT_EQUALS(list.size(), (gum::Size)7);
    }

    void testEraseAllVal() {
      gum::List<int> list;

      for (int i = 0; i < 7; i++)
        list.insert(42);

      list.pushFront(1);

      list.pushBack(2);

      list.eraseAllVal(42);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)2);
    }

    void testErase() {
      gum::List<int> list;

      for (int i = 0; i < 7; i++)
        list.insert(i);

      list.erase(2);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)6);

      list.erase(2);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)5);

      list.erase(20);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)5);
    }

    void testEraseIterator() {
      gum::List<int> list;

      for (int i = 0; i < 6; i++)
        list.insert(i);

      auto iter = list.beginSafe(); // safe iterator needed here

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)5);

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)5);

      ++iter;

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)4);

      iter = list.endSafe(); // safe iterator needed here

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)4);

      iter = list.rendSafe(); // safe iterator needed here

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)4);

      iter = list.rbeginSafe(); // safe iterator needed here

      list.erase(iter);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)3);

      iter = list.beginSafe(); // safe iterator needed here

      list.erase(++iter);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)2);

      iter = list.rbeginSafe(); // safe iterator needed here

      list.erase(--iter);

      TS_ASSERT_EQUALS(list.size(), (gum::Size)1);
    }

    void testIteratorStep() {
      gum::List<int> list{1, 2, 3, 4, 5, 6};
      auto iter = list.beginSafe(); // safe iterator needed here
      list.erase(iter);
      TS_ASSERT_EQUALS(list.size(), (gum::Size)5);

      iter = list.rbeginSafe(); // safe iterator needed here
      list.erase(iter);
      TS_ASSERT_EQUALS(list.size(), (gum::Size)4);

      auto iter2 = list.cbeginSafe(); // safe iterator needed here
      list.erase(iter2);
      TS_ASSERT_EQUALS(list.size(), (gum::Size)3);

      iter2 = list.crbeginSafe(); // safe iterator needed here
      list.erase(iter2);
      TS_ASSERT_EQUALS(list.size(), (gum::Size)2);

      iter2 = list.beginSafe(); // safe iterator needed here
      list.erase(iter2);
      TS_ASSERT_EQUALS(list.size(), (gum::Size)1);

      iter2 = list.crbeginSafe(); // safe iterator needed here
      list.erase(iter2);
      TS_ASSERT_EQUALS(list.size(), (gum::Size)0);
    }

    void testIteratorStep2() {
      gum::List<int> xlist{1, 2, 3, 4, 5, 6, 7, 8};
      gum::List<int>::iterator iter = xlist.begin();
      iter += 4;
      TS_ASSERT_EQUALS(*iter, 5);
      iter += 1;
      TS_ASSERT_EQUALS(*iter, 6);
      iter -= 1;
      TS_ASSERT_EQUALS(*iter, 5);
      iter -= 10;
      TS_ASSERT_EQUALS(iter, xlist.end());

      gum::List<int>::iterator iter2 = xlist.begin();
      iter2 += 4;
      TS_ASSERT_EQUALS(*iter2, 5);
      iter2 += 1;
      TS_ASSERT_EQUALS(*iter2, 6);
      iter2 -= 1;
      TS_ASSERT_EQUALS(*iter2, 5);
      iter2 -= 10;
      TS_ASSERT_EQUALS(iter2, xlist.end());

      {
        gum::List<int>::const_iterator iter3 = xlist.cbegin() + 4;
        TS_ASSERT_EQUALS(*iter3, 5);
        gum::List<int>::const_iterator iter4 = xlist.begin() + 4;
        TS_ASSERT_EQUALS(*iter4, 5);
        gum::List<int>::iterator iter5 = xlist.begin() + 4;
        TS_ASSERT_EQUALS(*iter5, 5);

        gum::List<int>::const_iterator iter13 = xlist.cbegin() + 4;
        TS_ASSERT_EQUALS(*iter13, 5);
        gum::List<int>::const_iterator iter14 = xlist.begin() + 4;
        TS_ASSERT_EQUALS(*iter14, 5);
        gum::List<int>::iterator iter15 = xlist.begin() + 4;
        TS_ASSERT_EQUALS(*iter15, 5);
      }

      {
        gum::List<int>::const_iterator iter3 = xlist.crbegin() - 3;
        TS_ASSERT_EQUALS(*iter3, 5);
        gum::List<int>::const_iterator iter4 = xlist.rbegin() - 3;
        TS_ASSERT_EQUALS(*iter4, 5);
        gum::List<int>::iterator iter5 = xlist.rbegin() - 3;
        TS_ASSERT_EQUALS(*iter5, 5);

        gum::List<int>::const_iterator iter13 = xlist.crbegin() - 3;
        TS_ASSERT_EQUALS(*iter13, 5);
        gum::List<int>::const_iterator iter14 = xlist.rbegin() - 3;
        TS_ASSERT_EQUALS(*iter14, 5);
        gum::List<int>::iterator iter15 = xlist.rbegin() - 3;
        TS_ASSERT_EQUALS(*iter15, 5);
      }
    }

    void testPopBack() {
      gum::List<int> list = initializeList(7);
      int last = list.back();
      list.popBack();

      TS_ASSERT_DIFFERS(list.back(), last);
    }

    void testPopFront() {
      gum::List<int> list = initializeList(7);
      int first = list.front();
      list.popFront();

      TS_ASSERT_DIFFERS(list.front(), first);
    }

    void testSTL() {
      gum::List<int> xlist{1, 2, 3, 4, 5, 6, 7, 8};
      int xx = 0;
      std::for_each(xlist.begin(), xlist.end(), [&xx](int x) { xx += x; });
      TS_ASSERT_EQUALS(xx, 36);

      std::vector<int> v;
      std::copy(xlist.begin(), xlist.end(), back_inserter(v));
      TS_ASSERT_EQUALS(v.size(), size_t(8));

      gum::List<int> list2;
      std::copy(v.begin(), v.end(), back_inserter(list2));
      TS_ASSERT_EQUALS(list2, xlist);

      gum::List<int> list3{2, 1, 8, 5, 3, 6, 4, 7};
      gum::List<int>::const_iterator iter1 = list3.cbegin();
      gum::List<int>::const_iterator iter2 = list3.cbegin() + 4;
      int d1 = iter2 - iter1;
      TS_ASSERT_EQUALS(d1, 4);

      gum::List<int>::iterator iter3 = list3.begin();
      gum::List<int>::iterator iter4 = list3.begin() + 4;
      int d2 = iter4 - iter3;
      TS_ASSERT_EQUALS(d2, 4);

      gum::List<int>::const_iterator iter11 = list3.cbegin();
      gum::List<int>::const_iterator iter12 = list3.cbegin() + 4;
      int d11 = iter12 - iter11;
      TS_ASSERT_EQUALS(d11, 4);

      gum::List<int>::iterator iter13 = list3.begin();
      gum::List<int>::iterator iter14 = list3.begin() + 4;
      int d12 = iter14 - iter13;
      TS_ASSERT_EQUALS(d12, 4);
    }

    void testAllocator() {
      { gum::List<int, MyAlloc<int>> xlist{1, 2, 3, 4, 5, 6, 7, 8}; }

      TS_ASSERT(MyAllocCount::hasMeroryLeak() == false);

      gum::List<int, MyAlloc<int>> xlist{1, 2, 3, 4, 5, 6, 7, 8};
      gum::ListIterator<int> iter = xlist.begin();
      gum::ListIterator<int> iter1(xlist);
      gum::ListConstIterator<int> iter2(xlist);
      gum::ListIterator<int> iter3(xlist);
      gum::ListConstIterator<int> iter4(xlist);

      unsigned int i;

      for (i = 0; iter1 != xlist.end(); ++iter1, ++i) {
      }

      TS_ASSERT(i == xlist.size());

      for (i = 0; iter2 != xlist.cend(); ++iter2, ++i) {
      }

      TS_ASSERT(i == xlist.size());

      for (i = 0; iter3 != xlist.end(); ++iter3, ++i) {
      }

      TS_ASSERT(i == xlist.size());

      for (i = 0; iter4 != xlist.cend(); ++iter4, ++i) {
      }

      TS_ASSERT(i == xlist.size());
    }

    private:
    /**
     * Initialize a list with integers from 1 to count.
     * If count < 1, then initialize from count to 1.
     */
    gum::List<int> initializeList(int count) {
      gum::List<int> list;

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
}
