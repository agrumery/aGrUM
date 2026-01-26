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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/heap.h>
#include <agrum/base/core/set.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Heap
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct HeapTestSuite {
    public:
    static void testConstructor() {
      gum::Heap< int > heap1;

      heap1.insert(8);
      heap1.emplace(10);
      heap1.insert(2);
      heap1.insert(23);
      heap1.insert(24);
      heap1.insert(10);
      heap1.insert(10);

      CHECK((heap1.top()) == (2));
      heap1.pop();
      CHECK((heap1.top()) == (8));
      heap1.eraseTop();
      CHECK((heap1.top()) == (10));
      heap1.eraseTop();
      CHECK((heap1.top()) == (10));

      CHECK((heap1.size()) == (static_cast< gum::Size >(4)));
      CHECK((heap1.empty()) == (false));
      CHECK((heap1.contains(8)) == (false));
      CHECK((heap1.contains(23)) == (true));
      CHECK((heap1.contains(10)) == (true));

      heap1.erase(10);
      CHECK((heap1.size()) == (static_cast< gum::Size >(3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(heap1.erase(150));

      heap1.eraseByPos(0);
      CHECK((heap1.top()) == (23));
    }

    static void testConstructor2() {
      gum::Heap< int, std::greater< int > > heap1;

      heap1.insert(8);
      heap1.insert(10);
      heap1.insert(2);
      heap1.insert(23);
      heap1.insert(24);
      heap1.insert(10);
      heap1.insert(10);

      CHECK((heap1.top()) == (24));
      heap1.pop();
      CHECK((heap1.top()) == (23));
      heap1.eraseTop();
      CHECK((heap1.top()) == (10));
      heap1.eraseTop();
      CHECK((heap1.top()) == (10));

      CHECK((heap1.size()) == (static_cast< gum::Size >(4)));
      CHECK((heap1.empty()) == (false));
      CHECK((heap1.contains(23)) == (false));
      CHECK((heap1.contains(2)) == (true));
      CHECK((heap1.contains(8)) == (true));

      heap1.erase(10);
      CHECK((heap1.size()) == (static_cast< gum::Size >(3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(heap1.erase(150));

      heap1.eraseByPos(0);
      CHECK((heap1.top()) == (8));
    }

    static void testMoves() {
      gum::Heap< int, std::greater< int > > heap1{1, 5, 2, 4};
      gum::Heap< int, std::greater< int > > heap2{3, 9};
      gum::Heap< int, std::greater< int > > heap3{1, 10, 20};

      gum::Heap< int, std::greater< int > > heap4 = std::move(heap3);
      heap3                                       = std::move(heap2);
      heap2                                       = std::move(heap1);
      CHECK((heap2.size()) == (static_cast< gum::Size >(4)));
      CHECK((heap2.top()) == (5));
    }

    static void testCopy() {
      gum::Heap< int > heap1;
      heap1.insert(8);
      heap1.insert(10);
      heap1.insert(2);
      heap1.insert(23);
      heap1.insert(24);
      heap1.insert(10);
      heap1.insert(10);

      gum::Heap< int > heap2;
      heap2.insert(8);
      heap2.insert(50);
      heap2.insert(20);

      heap2 = heap1;
      CHECK((heap2.size()) == ((gum::Size)heap1.size()));

      gum::Heap< int > heap3(heap1);
      CHECK((heap3.size()) == ((gum::Size)heap1.size()));
    }

    static void testErase() {
      gum::Heap< int > heap1;

      heap1.insert(8);
      heap1.insert(10);
      heap1.insert(2);
      heap1.insert(23);
      heap1.insert(24);
      heap1.insert(10);
      heap1.insert(10);

      heap1.eraseByPos(0);
      CHECK((heap1.top()) == (8));
      GUM_CHECK_ASSERT_THROWS_NOTHING(heap1.eraseByPos(150));
      CHECK((heap1.size()) == (static_cast< gum::Size >(6)));
      heap1.eraseByPos(4);
      CHECK((heap1.size()) == (static_cast< gum::Size >(5)));
      heap1.eraseByPos(4);
      CHECK((heap1.size()) == (static_cast< gum::Size >(4)));
      CHECK((heap1.contains(24)) == (false));
    }

    class SetCmp {
      public:
      bool operator()(const gum::Set< int >& x, const gum::Set< int >& y) const {
        return x.size() < y.size();
      }
    };

    static void testObj() {
      gum::Heap< gum::Set< int >, SetCmp > heap1;

      gum::Set< int > s1, s2, s3, s4, s5;
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

      CHECK((heap1.top()) == (s1));
      heap1.pop();
      CHECK((heap1.top()) == (s2));
      heap1.eraseTop();
      CHECK((heap1.top()) == (s2));
      heap1.eraseTop();
      CHECK((heap1.top()) == (s2));

      CHECK((heap1.size()) == (static_cast< gum::Size >(4)));
      CHECK((heap1.empty()) == (false));
      CHECK((heap1.contains(s1)) == (false));
      CHECK((heap1.contains(s4)) == (true));
      CHECK((heap1.contains(s5)) == (true));

      gum::Set< int > s6;
      heap1.erase(s2);
      CHECK((heap1.size()) == (static_cast< gum::Size >(3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(heap1.erase(s6));

      heap1.eraseByPos(0);
      CHECK((heap1.top()) == (s4));
    }
  };

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(Constructor2)
  GUM_TEST_ACTIF(Moves)
  GUM_TEST_ACTIF(Copy)
  GUM_TEST_ACTIF(Erase)
  GUM_TEST_ACTIF(Obj)

} /* namespace gum_tests */
