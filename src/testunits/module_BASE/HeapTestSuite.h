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

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/heap.h>
#include <agrum/core/set.h>
namespace gum_tests {

  class HeapTestSuite : public CxxTest::TestSuite {
    public:
    void testConstructor() {
      gum::Heap<int> heap1;

      heap1.insert(8);
      heap1.emplace(10);
      heap1.insert(2);
      heap1.insert(23);
      heap1.insert(24);
      heap1.insert(10);
      heap1.insert(10);

      TS_ASSERT(heap1.top() == 2);
      heap1.pop();
      TS_ASSERT(heap1.top() == 8);
      heap1.eraseTop();
      TS_ASSERT(heap1.top() == 10);
      heap1.eraseTop();
      TS_ASSERT(heap1.top() == 10);

      TS_ASSERT(heap1.size() == 4);
      TS_ASSERT(heap1.empty() == false);
      TS_ASSERT(heap1.contains(8) == false);
      TS_ASSERT(heap1.contains(23) == true);
      TS_ASSERT(heap1.contains(10) == true);

      heap1.erase(10);
      TS_ASSERT(heap1.size() == 3);
      TS_GUM_ASSERT_THROWS_NOTHING(heap1.erase(150));

      heap1.eraseByPos(0);
      TS_ASSERT(heap1.top() == 23);
    }

    void testConstructor2() {
      gum::Heap<int, std::greater<int>> heap1;

      heap1.insert(8);
      heap1.insert(10);
      heap1.insert(2);
      heap1.insert(23);
      heap1.insert(24);
      heap1.insert(10);
      heap1.insert(10);

      TS_ASSERT(heap1.top() == 24);
      heap1.pop();
      TS_ASSERT(heap1.top() == 23);
      heap1.eraseTop();
      TS_ASSERT(heap1.top() == 10);
      heap1.eraseTop();
      TS_ASSERT(heap1.top() == 10);

      TS_ASSERT(heap1.size() == 4);
      TS_ASSERT(heap1.empty() == false);
      TS_ASSERT(heap1.contains(23) == false);
      TS_ASSERT(heap1.contains(2) == true);
      TS_ASSERT(heap1.contains(8) == true);

      heap1.erase(10);
      TS_ASSERT(heap1.size() == 3);
      TS_GUM_ASSERT_THROWS_NOTHING(heap1.erase(150));

      heap1.eraseByPos(0);
      TS_ASSERT(heap1.top() == 8);
    }

    void testMoves() {
      gum::Heap<int, std::greater<int>> heap1{1, 5, 2, 4};
      gum::Heap<int, std::greater<int>> heap2{3, 9};
      gum::Heap<int, std::greater<int>> heap3{1, 10, 20};

      gum::Heap<int, std::greater<int>> heap4 = std::move(heap3);
      heap3 = std::move(heap2);
      heap2 = std::move(heap1);
      TS_ASSERT(heap2.size() == 4);
      TS_ASSERT(heap2.top() == 5);
    }

    void testCopy() {
      gum::Heap<int> heap1;
      heap1.insert(8);
      heap1.insert(10);
      heap1.insert(2);
      heap1.insert(23);
      heap1.insert(24);
      heap1.insert(10);
      heap1.insert(10);

      gum::Heap<int> heap2;
      heap2.insert(8);
      heap2.insert(50);
      heap2.insert(20);

      heap2 = heap1;
      TS_ASSERT(heap2.size() == heap1.size());

      gum::Heap<int> heap3(heap1);
      TS_ASSERT(heap3.size() == heap1.size());
    }

    void testErase() {
      gum::Heap<int> heap1;

      heap1.insert(8);
      heap1.insert(10);
      heap1.insert(2);
      heap1.insert(23);
      heap1.insert(24);
      heap1.insert(10);
      heap1.insert(10);

      heap1.eraseByPos(0);
      TS_ASSERT(heap1.top() == 8);
      TS_GUM_ASSERT_THROWS_NOTHING(heap1.eraseByPos(150));
      TS_ASSERT(heap1.size() == 6);
      heap1.eraseByPos(4);
      TS_ASSERT(heap1.size() == 5);
      heap1.eraseByPos(4);
      TS_ASSERT(heap1.size() == 4);
      TS_ASSERT(heap1.contains(24) == false);
    }

    class SetCmp {
      public:
      bool operator()(const gum::Set<int> &x, const gum::Set<int> &y) const {
        return x.size() < y.size();
      }
    };

    void testObj() {
      gum::Heap<gum::Set<int>, SetCmp> heap1;

      gum::Set<int> s1, s2, s3, s4, s5;
      s1.insert(1);
      s2.insert(1);
      s2.insert(2);
      s3.insert(1);
      s3.insert(2);
      s3.insert(3);
      s4.insert(1);
      s4.insert(2);
      s4.insert(3);
      s4.insert(4);
      s5.insert(1);
      s5.insert(2);
      s5.insert(3);
      s5.insert(4);
      s5.insert(5);

      heap1.insert(s4);
      heap1.insert(s2);
      heap1.insert(s1);
      heap1.insert(s3);
      heap1.insert(s2);
      heap1.insert(s2);
      heap1.insert(s5);

      TS_ASSERT(heap1.top() == s1);
      heap1.pop();
      TS_ASSERT(heap1.top() == s2);
      heap1.eraseTop();
      TS_ASSERT(heap1.top() == s2);
      heap1.eraseTop();
      TS_ASSERT(heap1.top() == s2);

      TS_ASSERT(heap1.size() == 4);
      TS_ASSERT(heap1.empty() == false);
      TS_ASSERT(heap1.contains(s1) == false);
      TS_ASSERT(heap1.contains(s4) == true);
      TS_ASSERT(heap1.contains(s5) == true);

      gum::Set<int> s6;
      heap1.erase(s2);
      TS_ASSERT(heap1.size() == 3);
      TS_GUM_ASSERT_THROWS_NOTHING(heap1.erase(s6));

      heap1.eraseByPos(0);
      TS_ASSERT(heap1.top() == s4);
    }
  };

} /* namespace gum_tests */
