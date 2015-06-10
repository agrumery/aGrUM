/***************************************************************************
 *   Copyright (C) 2007 by Christophe GONZALES                             *
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

#include <agrum/core/binSearchTree.h>

namespace gum_tests {

  class binSearchTreeTestSuite : public CxxTest::TestSuite {
    public:
    void testConstructors() {
      gum::BinSearchTree<int> *tree = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(tree = new gum::BinSearchTree<int>);
      TS_ASSERT(tree->size() == 0);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::BinSearchTree<int> tree2(*tree));
      gum::BinSearchTree<int> tree3;
      gum::BinSearchTree<int> tree4 = *tree;
      tree3 = tree4;

      tree->insert(3);
      tree->insert(4);

      TS_ASSERT(tree->size() == 2);
      tree3 = *tree;
      TS_ASSERT(tree3.size() == 2);

      tree->insert(1);
      tree->insert(4);

      tree->erase(3);

      delete tree;
    }

    void testValues() {
      gum::BinSearchTree<int> tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(6);
      tree.insert(8);

      TS_ASSERT(tree.minValue() == 1);
      TS_ASSERT(tree.maxValue() == 8);
      TS_ASSERT(tree.rootValue() == 5);

      tree.erase(5);

      TS_ASSERT(tree.rootValue() == 6);
      TS_ASSERT(tree.contains(4) == true);
      TS_ASSERT(tree.empty() == false);

      tree.clear();

      TS_ASSERT_THROWS_ANYTHING(tree.rootValue(););
      TS_ASSERT_THROWS_ANYTHING(tree.minValue(););
      TS_ASSERT_THROWS_ANYTHING(tree.maxValue(););
      TS_ASSERT(tree.size() == 0);
      TS_ASSERT(tree.empty() == true);
    }

    void testErase() {
      gum::BinSearchTree<int> tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(5);
      tree.insert(5);

      TS_ASSERT(tree.size() == 7);

      tree.erase(5);
      tree.erase(5);
      tree.erase(5);

      TS_ASSERT(tree.size() == 4);
      TS_ASSERT_THROWS(tree.erase(5), gum::NotFound);
      TS_ASSERT(tree.size() == 4);
      TS_ASSERT_THROWS(tree.erase(9), gum::NotFound);
      TS_ASSERT(tree.size() == 4);
      tree.erase(1);
      TS_ASSERT(tree.size() == 3);
      tree.erase(7);
      TS_ASSERT(tree.size() == 2);
      tree.erase(4);
      TS_ASSERT(tree.size() == 1);
      tree.erase(3);
      TS_ASSERT(tree.size() == 0);
      TS_ASSERT(tree.empty() == true);
    }

    void testUniqueness() {
      gum::BinSearchTree<int> tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);

      TS_ASSERT(tree.uniquenessPolicy() == false);

      tree.insert(5);
      TS_ASSERT(tree.size() == 6);

      tree.setUniquenessPolicy(true);
      TS_ASSERT_THROWS_ANYTHING(tree.insert(5););
    }

    void testIterators() {
      gum::BinSearchTree<int> tree;
      tree.insert(5);
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);

      unsigned int i = 0;

      for (gum::BinSearchTree<int>::iterator iter = tree.begin(); iter != tree.end();
           ++iter, ++i) {
        switch (i) {
          case 0:
            TS_ASSERT(*iter == 1);
            break;

          case 1:
            TS_ASSERT(*iter == 3);
            break;

          case 2:
            TS_ASSERT(*iter == 4);
            break;

          case 3:
            TS_ASSERT(*iter == 5);
            break;

          case 4:
            TS_ASSERT(*iter == 5);
            break;

          case 5:
            TS_ASSERT(*iter == 7);
            break;
        }
      }

      i = 5;

      for (gum::BinSearchTree<int>::iterator iter = tree.rbegin();
           iter != tree.rend(); --iter, --i) {
        switch (i) {
          case 0:
            TS_ASSERT(*iter == 1);
            break;

          case 1:
            TS_ASSERT(*iter == 3);
            break;

          case 2:
            TS_ASSERT(*iter == 4);
            break;

          case 3:
            TS_ASSERT(*iter == 5);
            break;

          case 4:
            TS_ASSERT(*iter == 5);
            break;

          case 5:
            TS_ASSERT(*iter == 7);
            break;
        }
      }

      gum::BinSearchTree<int>::iterator iter = tree.root();

      TS_ASSERT(*iter == 5);
    }

    void testEraseIterator() {
      gum::BinSearchTree<int> tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(5);
      tree.insert(5);

      TS_ASSERT(tree.size() == 7);

      gum::BinSearchTree<int>::iterator iter = tree.begin();

      tree.erase(iter);
      TS_ASSERT(tree.size() == 6);

      iter = tree.rbegin();
      tree.erase(iter);
      TS_ASSERT(tree.size() == 5);

      iter = tree.begin();
      ++iter;
      ++iter;
      tree.erase(iter);
      TS_ASSERT(tree.size() == 4);

      iter = tree.end();
      tree.erase(iter);
      TS_ASSERT(tree.size() == 4);

      iter = tree.rend();
      tree.erase(iter);
      TS_ASSERT(tree.size() == 4);
    }
  };
}
