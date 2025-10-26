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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/binSearchTree.h>

namespace gum_tests {

  class GUM_TEST_SUITE(binSearchTree) {
    public:
    GUM_ACTIVE_TEST(Constructors) {
      gum::BinSearchTree< int >* tree = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(tree = new gum::BinSearchTree< int >)
      TS_ASSERT_EQUALS(tree->size(), static_cast< gum::Size >(0))

      TS_GUM_ASSERT_THROWS_NOTHING(gum::BinSearchTree< int > tree2(*tree))
      gum::BinSearchTree< int > tree3;
      gum::BinSearchTree< int > tree4 = *tree;
      tree3                           = tree4;

      tree->insert(3);
      tree->insert(4);

      TS_ASSERT_EQUALS(tree->size(), static_cast< gum::Size >(2))
      tree3 = *tree;
      TS_ASSERT_EQUALS(tree3.size(), static_cast< gum::Size >(2))

      tree->insert(1);
      tree->insert(4);

      tree->erase(3);

      delete tree;
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(Values) {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(6);
      tree.insert(8);

      TS_ASSERT_EQUALS(tree.minValue(), 1)
      TS_ASSERT_EQUALS(tree.maxValue(), 8)
      TS_ASSERT_EQUALS(tree.rootValue(), 5)

      tree.erase(5);

      TS_ASSERT_EQUALS(tree.rootValue(), 6)
      TS_ASSERT_EQUALS(tree.contains(4), true)
      TS_ASSERT_EQUALS(tree.empty(), false)

      tree.clear();

      TS_ASSERT_THROWS_ANYTHING(tree.rootValue();)
      TS_ASSERT_THROWS_ANYTHING(tree.minValue();)
      TS_ASSERT_THROWS_ANYTHING(tree.maxValue();)
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(tree.empty(), true)
    }

    GUM_ACTIVE_TEST(Erase) {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(5);
      tree.insert(5);

      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(7))

      tree.erase(5);
      tree.erase(5);
      tree.erase(5);

      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(4))
      TS_ASSERT_THROWS(tree.erase(5), const gum::NotFound&)
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(4))
      TS_ASSERT_THROWS(tree.erase(9), const gum::NotFound&)
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(4))
      tree.erase(1);
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(3))
      tree.erase(7);
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(2))
      tree.erase(4);
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(1))
      tree.erase(3);
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(tree.empty(), true)
    }

    GUM_ACTIVE_TEST(Uniqueness) {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);

      TS_ASSERT_EQUALS(tree.uniquenessPolicy(), false)

      tree.insert(5);
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(6))

      tree.setUniquenessPolicy(true);
      TS_ASSERT_THROWS_ANYTHING(tree.insert(5);)
    }

    GUM_ACTIVE_TEST(Iterators) {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);

      unsigned int i = 0;

      for (gum::BinSearchTree< int >::iterator iter = tree.begin(); iter != tree.end();
           ++iter, ++i) {
        switch (i) {
          case 0 : TS_ASSERT_EQUALS(*iter, 1) break;

          case 1 : TS_ASSERT_EQUALS(*iter, 3) break;

          case 2 : TS_ASSERT_EQUALS(*iter, 4) break;

          case 3 : TS_ASSERT_EQUALS(*iter, 5) break;

          case 4 : TS_ASSERT_EQUALS(*iter, 5) break;

          case 5 : TS_ASSERT_EQUALS(*iter, 7) break;
        }
      }

      i = 5;

      for (gum::BinSearchTree< int >::iterator iter = tree.rbegin(); iter != tree.rend();
           --iter, --i) {
        switch (i) {
          case 0 : TS_ASSERT_EQUALS(*iter, 1) break;

          case 1 : TS_ASSERT_EQUALS(*iter, 3) break;

          case 2 : TS_ASSERT_EQUALS(*iter, 4) break;

          case 3 : TS_ASSERT_EQUALS(*iter, 5) break;

          case 4 : TS_ASSERT_EQUALS(*iter, 5) break;

          case 5 : TS_ASSERT_EQUALS(*iter, 7) break;
        }
      }

      gum::BinSearchTree< int >::iterator iter = tree.root();

      TS_ASSERT_EQUALS(*iter, 5)
    }

    GUM_ACTIVE_TEST(EraseIterator) {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(5);
      tree.insert(5);

      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(7))

      gum::BinSearchTree< int >::iterator iter = tree.begin();

      tree.erase(iter);
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(6))

      iter = tree.rbegin();
      tree.erase(iter);
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(5))

      iter = tree.begin();
      ++iter;
      ++iter;
      tree.erase(iter);
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(4))

      iter = tree.end();
      tree.erase(iter);
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(4))

      iter = tree.rend();
      tree.erase(iter);
      TS_ASSERT_EQUALS(tree.size(), static_cast< gum::Size >(4))
    }
  };
}   // namespace gum_tests
