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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/graphElements.h>

namespace gum_tests {

  class [[maybe_unused]] ArcTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Constructor1){TS_GUM_ASSERT_THROWS_NOTHING(gum::Arc arc1(1, 2))
                                      TS_GUM_ASSERT_THROWS_NOTHING(gum::Arc arc1(1, 1))}

    GUM_ACTIVE_TEST(Constructor2) {
      gum::Arc arc1(1, 2);
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Arc copy(arc1))
    }

    GUM_ACTIVE_TEST(Equality) {
      gum::Arc arc1(1, 2);
      gum::Arc arc2(2, 1);

      TS_ASSERT_EQUALS(arc1, arc1)
      TS_ASSERT_DIFFERS(arc1, arc2)
      TS_ASSERT_DIFFERS(arc2, arc1)

      gum::Arc copy(arc1);
      gum::Arc arc3(6, 7);

      TS_ASSERT_EQUALS(arc1, arc1)
      TS_ASSERT_EQUALS(copy, arc1)
      TS_ASSERT_EQUALS(arc1, copy)

      TS_ASSERT_DIFFERS(arc1, arc3)
      TS_ASSERT_DIFFERS(arc3, arc1)
      TS_ASSERT_DIFFERS(copy, arc3)
      TS_ASSERT_DIFFERS(arc2, arc3)
      TS_ASSERT_DIFFERS(arc1, arc2)
    }

    GUM_ACTIVE_TEST(Getters1) {
      gum::Arc arc1((gum::NodeId)1, (gum::NodeId)2);
      gum::Arc arc2((gum::NodeId)2, (gum::NodeId)1);
      gum::Arc arc3((gum::NodeId)3, (gum::NodeId)4);

      gum::Arc copy(arc1);

      gum::NodeId n;

      TS_GUM_ASSERT_THROWS_NOTHING(n = arc1.first())
      TS_GUM_ASSERT_THROWS_NOTHING(n = arc1.second())

      TS_GUM_ASSERT_THROWS_NOTHING(n = copy.first())
      TS_GUM_ASSERT_THROWS_NOTHING(n = copy.second())

      TS_ASSERT_EQUALS(n = arc2.first(), (gum::NodeId)2)
      TS_ASSERT_EQUALS(n = arc2.second(), (gum::NodeId)1)

      TS_GUM_ASSERT_THROWS_NOTHING(n = arc3.other((gum::NodeId)3))
      TS_GUM_ASSERT_THROWS_NOTHING(n = arc3.other((gum::NodeId)4))

      TS_ASSERT_EQUALS(n = arc3.other((gum::NodeId)3), (gum::NodeId)4)
      TS_ASSERT_EQUALS(n = arc3.other((gum::NodeId)4), (gum::NodeId)3)

      TS_ASSERT_THROWS_ANYTHING(n = copy.other((gum::NodeId)42))
    }

    GUM_ACTIVE_TEST(Getters2) {
      gum::Arc arc1(1, 2);

      TS_ASSERT_EQUALS(arc1.tail(), (gum::NodeId)1)
      TS_ASSERT_EQUALS(arc1.head(), (gum::NodeId)2)
    }

    GUM_ACTIVE_TEST(Hash) {
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
          TS_ASSERT_EQUALS(hash[arc], (x + y))
        }
      }
    }
  };
}   // namespace gum_tests
