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

#include <agrum/base/graphs/graphElements.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Arc
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct ArcTestSuite {
    public:
    static void testConstructor1() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::Arc arc1(1, 2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::Arc arc1(1, 1));
    }

    static void testConstructor2() {
      gum::Arc arc1(1, 2);
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::Arc copy(arc1));
    }   // namespace gum_tests

    static void testEquality() {
      gum::Arc arc1(1, 2);
      gum::Arc arc2(2, 1);

      CHECK((arc1) == (arc1));
      CHECK((arc1) != (arc2));
      CHECK((arc2) != (arc1));

      gum::Arc copy(arc1);
      gum::Arc arc3(6, 7);

      CHECK((arc1) == (arc1));
      CHECK((copy) == (arc1));
      CHECK((arc1) == (copy));

      CHECK((arc1) != (arc3));
      CHECK((arc3) != (arc1));
      CHECK((copy) != (arc3));
      CHECK((arc2) != (arc3));
      CHECK((arc1) != (arc2));
    }

    static void testGetters1() {
      gum::Arc arc1((gum::NodeId)1, (gum::NodeId)2);
      gum::Arc arc2((gum::NodeId)2, (gum::NodeId)1);
      gum::Arc arc3((gum::NodeId)3, (gum::NodeId)4);

      gum::Arc copy(arc1);

      gum::NodeId n;

      GUM_CHECK_ASSERT_THROWS_NOTHING(n = arc1.first());
      GUM_CHECK_ASSERT_THROWS_NOTHING(n = arc1.second());

      GUM_CHECK_ASSERT_THROWS_NOTHING(n = copy.first());
      GUM_CHECK_ASSERT_THROWS_NOTHING(n = copy.second());

      CHECK((n = arc2.first()) == (static_cast< gum::NodeId >(2)));
      CHECK((n = arc2.second()) == (static_cast< gum::NodeId >(1)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(n = arc3.other(3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(n = arc3.other(4));

      CHECK((n = arc3.other((gum::NodeId)3)) == (static_cast< gum::NodeId >(4)));
      CHECK((n = arc3.other((gum::NodeId)4)) == (static_cast< gum::NodeId >(3)));

      CHECK_THROWS(n = copy.other((gum::NodeId)42));
    }

    static void testGetters2() {
      gum::Arc arc1(1, 2);

      CHECK((arc1.tail()) == (static_cast< gum::NodeId >(1)));
      CHECK((arc1.head()) == (static_cast< gum::NodeId >(2)));
    }

    static void testHash() {
      gum::HashTable< gum::Arc, gum::NodeId > hash;

      for (gum::NodeId x = 0; x < 1000; ++x) {
        for (gum::NodeId y = x; y < 1000; ++y) {
          gum::Arc arc(x, y);
          hash.insert(arc, x + y);
        }
      }

      for (gum::NodeId x = 0; x < 1000; ++x) {
        for (gum::NodeId y = x; y < 1000; ++y) {
          gum::Arc arc(x, y);
          CHECK((hash[arc]) == ((x + y)));
        }
      }
    }
  };

  GUM_TEST_ACTIF(Constructor1)
  GUM_TEST_ACTIF(Constructor2)
  GUM_TEST_ACTIF(Equality)
  GUM_TEST_ACTIF(Getters1)
  GUM_TEST_ACTIF(Getters2)
  GUM_TEST_ACTIF(Hash)
}   // namespace gum_tests
