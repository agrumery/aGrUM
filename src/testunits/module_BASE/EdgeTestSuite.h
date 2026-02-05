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
#define GUM_CURRENT_SUITE  Edge
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct EdgeTestSuite {
    public:
    static void testConstructor1() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::Edge edge1(1, 2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::Edge edge1(1, 1));
    }

    static void testConstructor2() {
      gum::Edge edge1(1, 2);
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::Edge copy(edge1));
    }   // namespace gum_tests

    static void testEquality() {
      gum::Edge edge1(1, 2);
      gum::Edge edge2(2, 1);
      gum::Edge copy(edge1);
      gum::Edge edge3(6, 7);

      CHECK((edge1) == (edge1));
      CHECK((edge1) == (edge2));
      CHECK((edge2) == (edge1));
      CHECK((copy) == (edge1));
      CHECK((edge2) == (copy));

      CHECK((edge1) != (edge3));
      CHECK((edge3) != (edge1));
      CHECK((copy) != (edge3));
      CHECK((edge2) != (edge3));
    }

    static void testGetters() {
      const gum::Edge edge1((gum::NodeId)1, (gum::NodeId)2);
      const gum::Edge edge2((gum::NodeId)2, (gum::NodeId)1);

      CHECK((edge1) == (edge1));
      CHECK((edge1) == (edge2));
      CHECK((edge2) == (edge1));

      const gum::Edge edge3((gum::NodeId)3, (gum::NodeId)4);
      const gum::Edge edge4((gum::NodeId)1, (gum::NodeId)2);

      gum::NodeId n;

      GUM_CHECK_ASSERT_THROWS_NOTHING(n = edge1.first());
      GUM_CHECK_ASSERT_THROWS_NOTHING(n = edge1.second());

      GUM_CHECK_ASSERT_THROWS_NOTHING(n = edge4.first());
      GUM_CHECK_ASSERT_THROWS_NOTHING(n = edge4.second());

      CHECK((n = edge2.first()) == ((gum::NodeId)1));
      CHECK((n = edge2.second()) == ((gum::NodeId)2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(n = edge3.other((gum::NodeId)3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(n = edge3.other((gum::NodeId)4));

      CHECK((n = edge3.other((gum::NodeId)3)) == ((gum::NodeId)4));
      CHECK((n = edge3.other((gum::NodeId)4)) == ((gum::NodeId)3));

      CHECK_THROWS(n = edge4.other((gum::NodeId)42));
    }

    static void testHash() {
      gum::HashTable< gum::Edge, gum::NodeId > hash;

      for (gum::NodeId x = 0; x < 1000; ++x) {
        for (gum::NodeId y = x; y < 1000; ++y) {
          gum::Edge edge(x, y);
          hash.insert(edge, x + y);
        }
      }

      for (gum::NodeId x = 0; x < 1000; ++x) {
        for (gum::NodeId y = x; y < 1000; ++y) {
          gum::Edge edge(x, y);
          CHECK((hash[edge]) == ((x + y)));
        }
      }
    }
  };

  GUM_TEST_ACTIF(Constructor1)
  GUM_TEST_ACTIF(Constructor2)
  GUM_TEST_ACTIF(Equality)
  GUM_TEST_ACTIF(Getters)
  GUM_TEST_ACTIF(Hash)
}   // namespace gum_tests
