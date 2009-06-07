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

#include <cxxtest/TestSuite.h>
#include <agrum/core/list.h>

class ListTestSuite: public CxxTest::TestSuite {
public:

  void testPrivateMethods() {
    TS_GUM_ASSERT_THROWS_NOTHING(initializeList(7));
    TS_GUM_ASSERT_THROWS_NOTHING(initializeList(-7));
  }

  void testPush() {
    gum::List<int> liste1;
    liste1.pushFront (3);
    TS_ASSERT_EQUALS(liste1.size (), (gum::Size)1);

    liste1.pushBack  (4);
    liste1.pushBack  (5);
    TS_ASSERT_EQUALS(liste1.size (), (gum::Size)3);
  }

  void testCopy() {
    gum::List<int> liste1;
    liste1.pushFront(3);
    liste1.pushFront(4);
    liste1.pushFront(5);

    gum::List<int> liste2(liste1);
    gum::List<int> liste3 = liste1;

    TS_ASSERT_EQUALS(liste2.size (), liste1.size ());
    TS_ASSERT_EQUALS(liste2.size (), liste1.size ());

    liste1.pushBack (5);
    liste1.pushBack (5);
    liste1.pushBack (6);
    liste2 = liste1;
    gum::List<int> liste4;
    liste3 = liste4;

    TS_ASSERT_EQUALS(liste2.size (), liste1.size ());
    TS_ASSERT_EQUALS(liste3.size (), (gum::Size)0);
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
    TS_ASSERT_EQUALS(list.size(), (gum::Size) 6);

    list.erase(2);
    TS_ASSERT_EQUALS(list.size(), (gum::Size) 5);
    
    list.erase(20);
    TS_ASSERT_EQUALS(list.size(), (gum::Size) 5);
  }

  void testEraseIterator() {
    gum::List<int> list;
    for (int i = 0; i < 6; i++)
      list.insert(i);
  
    gum::List<int>::iterator iter = list.begin();
    list.erase (iter);
    TS_ASSERT_EQUALS(list.size(), (gum::Size) 5);
    list.erase (iter);
    TS_ASSERT_EQUALS(list.size(), (gum::Size) 5);

    ++iter;
    list.erase (iter);
    TS_ASSERT_EQUALS(list.size(), (gum::Size) 4);

    iter = list.end();
    list.erase (iter);
    TS_ASSERT_EQUALS(list.size(), (gum::Size) 4);

    iter = list.rend();
    list.erase (iter);
    TS_ASSERT_EQUALS(list.size(), (gum::Size) 4);

    iter = list.rbegin();
    list.erase (iter);
    TS_ASSERT_EQUALS(list.size(), (gum::Size) 3);
    
    iter = list.begin();
    list.erase (++iter);
    TS_ASSERT_EQUALS(list.size(), (gum::Size) 2);

    iter = list.rbegin();
    list.erase (--iter);
    TS_ASSERT_EQUALS(list.size(), (gum::Size) 1);
    
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
    }
    else {
      for (int i = count; i <= 1; i++)
        list.pushBack(i);
      return list;
    }
  }
  
};

