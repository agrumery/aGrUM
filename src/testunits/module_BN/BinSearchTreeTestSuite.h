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

#include <agrum/base/core/binSearchTree.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  binSearchTree
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct binSearchTreeTestSuite {
    public:
    static void testConstructors() {
      gum::BinSearchTree< int >* tree = 0;

      GUM_CHECK_ASSERT_THROWS_NOTHING(tree = new gum::BinSearchTree< int >);
      CHECK((tree->size()) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::BinSearchTree< int > tree2(*tree));
      gum::BinSearchTree< int > tree3;
      gum::BinSearchTree< int > tree4 = *tree;
      tree3                           = tree4;

      tree->insert(3);
      tree->insert(4);

      CHECK((tree->size()) == (static_cast< gum::Size >(2)));
      tree3 = *tree;
      CHECK((tree3.size()) == (static_cast< gum::Size >(2)));

      tree->insert(1);
      tree->insert(4);

      tree->erase(3);

      delete tree;
    }   // namespace gum_tests

    static void testValues() {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(6);
      tree.insert(8);

      CHECK((tree.minValue()) == (1));
      CHECK((tree.maxValue()) == (8));
      CHECK((tree.rootValue()) == (5));

      tree.erase(5);

      CHECK((tree.rootValue()) == (6));
      CHECK((tree.contains(4)) == (true));
      CHECK((tree.empty()) == (false));

      tree.clear();

      CHECK_THROWS(tree.rootValue());
      CHECK_THROWS(tree.minValue());
      CHECK_THROWS(tree.maxValue());
      CHECK((tree.size()) == (static_cast< gum::Size >(0)));
      CHECK((tree.empty()) == (true));
    }

    static void testErase() {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(5);
      tree.insert(5);

      CHECK((tree.size()) == (static_cast< gum::Size >(7)));

      tree.erase(5);
      tree.erase(5);
      tree.erase(5);

      CHECK((tree.size()) == (static_cast< gum::Size >(4)));
      CHECK_THROWS_AS(tree.erase(5), const gum::NotFound&);
      CHECK((tree.size()) == (static_cast< gum::Size >(4)));
      CHECK_THROWS_AS(tree.erase(9), const gum::NotFound&);
      CHECK((tree.size()) == (static_cast< gum::Size >(4)));
      tree.erase(1);
      CHECK((tree.size()) == (static_cast< gum::Size >(3)));
      tree.erase(7);
      CHECK((tree.size()) == (static_cast< gum::Size >(2)));
      tree.erase(4);
      CHECK((tree.size()) == (static_cast< gum::Size >(1)));
      tree.erase(3);
      CHECK((tree.size()) == (static_cast< gum::Size >(0)));
      CHECK((tree.empty()) == (true));
    }

    static void testUniqueness() {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);

      CHECK((tree.uniquenessPolicy()) == (false));

      tree.insert(5);
      CHECK((tree.size()) == (static_cast< gum::Size >(6)));

      tree.setUniquenessPolicy(true);
      CHECK_THROWS(tree.insert(5));
    }

    static void testIterators() {
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
          case 0 : CHECK((*iter) == (1)); break;

          case 1 : CHECK((*iter) == (3)); break;

          case 2 : CHECK((*iter) == (4)); break;

          case 3 : CHECK((*iter) == (5)); break;

          case 4 : CHECK((*iter) == (5)); break;

          case 5 : CHECK((*iter) == (7)); break;
        }
      }

      i = 5;

      for (gum::BinSearchTree< int >::iterator iter = tree.rbegin(); iter != tree.rend();
           --iter, --i) {
        switch (i) {
          case 0 : CHECK((*iter) == (1)); break;

          case 1 : CHECK((*iter) == (3)); break;

          case 2 : CHECK((*iter) == (4)); break;

          case 3 : CHECK((*iter) == (5)); break;

          case 4 : CHECK((*iter) == (5)); break;

          case 5 : CHECK((*iter) == (7)); break;
        }
      }

      gum::BinSearchTree< int >::iterator iter = tree.root();

      CHECK((*iter) == (5));
    }

    static void testEraseIterator() {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(5);
      tree.insert(5);

      CHECK((tree.size()) == (static_cast< gum::Size >(7)));

      gum::BinSearchTree< int >::iterator iter = tree.begin();

      tree.erase(iter);
      CHECK((tree.size()) == (static_cast< gum::Size >(6)));

      iter = tree.rbegin();
      tree.erase(iter);
      CHECK((tree.size()) == (static_cast< gum::Size >(5)));

      iter = tree.begin();
      ++iter;
      ++iter;
      tree.erase(iter);
      CHECK((tree.size()) == (static_cast< gum::Size >(4)));

      iter = tree.end();
      tree.erase(iter);
      CHECK((tree.size()) == (static_cast< gum::Size >(4)));

      iter = tree.rend();
      tree.erase(iter);
      CHECK((tree.size()) == (static_cast< gum::Size >(4)));
    }
  };

  GUM_TEST_ACTIF(Constructors)
  GUM_TEST_ACTIF(Values)
  GUM_TEST_ACTIF(Erase)
  GUM_TEST_ACTIF(Uniqueness)
  GUM_TEST_ACTIF(Iterators)
  GUM_TEST_ACTIF(EraseIterator)
}   // namespace gum_tests
