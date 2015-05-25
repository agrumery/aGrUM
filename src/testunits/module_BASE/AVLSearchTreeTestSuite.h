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

#include <cstdlib>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/AVLSearchTree.h>

namespace gum_tests {

  class AVLSearchTreeTestSuite : public CxxTest::TestSuite {
    public:
    void testConstructors() {
      gum::AVLSearchTree<int> *tree = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(tree = new gum::AVLSearchTree<int>);
      TS_ASSERT(tree->size() == 0);

      tree->insert(3);
      tree->insert(4);
      tree->insert(4);
      tree->insert(4);
      tree->insert(4);
      TS_ASSERT(tree->checkAVLStructure() == true);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::AVLSearchTree<int> tree2(*tree));
      gum::AVLSearchTree<int> tree3;
      gum::AVLSearchTree<int> tree4 = *tree;
      tree3 = tree4;
      TS_ASSERT(tree->size() == 5);

      tree3 = *tree;
      TS_ASSERT(tree3.size() == 5);

      delete tree;
    }

    void testInsert() {
      gum::AVLSearchTree<int> tree;

      for (unsigned int i = 0; i < 1000; ++i) {
        tree.insert(rand());

        if (i % 50 == 0) {
          TS_ASSERT(tree.size() == i + 1);
          TS_ASSERT(tree.checkAVLStructure() == true);
        }
      }
    }

    void testValues() {
      gum::AVLSearchTree<int> tree;
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

      TS_ASSERT(tree.rootValue() == 5);
      TS_ASSERT(tree.contains(4) == true);
      TS_ASSERT(tree.empty() == false);

      tree.clear();

      TS_ASSERT_THROWS_ANYTHING(tree.rootValue(););
      TS_ASSERT_THROWS_ANYTHING(tree.minValue(););
      TS_ASSERT_THROWS_ANYTHING(tree.maxValue(););
      TS_ASSERT(tree.size() == 0);
      TS_ASSERT(tree.empty() == true);
    }

    void testUniqueness() {
      gum::AVLSearchTree<int> tree;
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
      gum::AVLSearchTree<int> tree;
      tree.insert(5);
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);

      unsigned int i = 0;

      for (gum::AVLSearchTree<int>::iterator iter = tree.begin(); iter != tree.end();
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

      for (gum::AVLSearchTree<int>::iterator iter = tree.rbegin();
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

      gum::AVLSearchTree<int>::iterator iter = tree.root();

      TS_ASSERT(*iter == 5);
    }

    void testErase() {
      gum::AVLSearchTree<int> tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(5);
      tree.insert(5);

      TS_ASSERT(tree.size() == 7);

      TS_GUM_ASSERT_THROWS_NOTHING(tree.erase(5));
      TS_GUM_ASSERT_THROWS_NOTHING(tree.erase(5));
      TS_GUM_ASSERT_THROWS_NOTHING(tree.erase(5));

      TS_ASSERT(tree.size() == 4);
      TS_ASSERT_THROWS(tree.erase(5), gum::NotFound);
      TS_ASSERT(tree.size() == 4);
      TS_ASSERT_THROWS(tree.erase(9), gum::NotFound);
      TS_ASSERT(tree.checkAVLStructure() == true);
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

    void testRandomErase() {
      gum::AVLSearchTree<int> tree;
      std::vector<unsigned int> vect(4000);

      for (unsigned int i = 0; i < 4000; ++i)
        vect[i] = rand();

      for (unsigned int i = 0; i < 4000; ++i) {
        tree.insert(i);
        TS_ASSERT(tree.checkAVLStructure() == true);
      }

      for (unsigned int j = 0; j < 4000; j += 10) {
        for (unsigned int i = 0; i < 10; ++i) {
          try {
            tree.erase(vect[i + j]);
          } catch (gum::NotFound &e) {
          }

          TS_ASSERT(tree.checkAVLStructure() == true);
        }
      }
    }

    void testEraseIterator() {
      gum::AVLSearchTree<int> tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(5);
      tree.insert(5);

      TS_ASSERT(tree.size() == 7);

      gum::AVLSearchTree<int>::iterator iter = tree.begin();

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

    void testEraseValue() {
      gum::AVLSearchTree<char> tree;
      tree.insert('A');
      tree.insert('C');
      tree.insert('A');
      tree.insert('D');
      tree.insert('F');
      tree.insert('E');
      tree.insert('E');

      TS_ASSERT_EQUALS(tree.toString(), "{ A, A, C, D, E, E, F }");
      tree.erase('C');
      TS_ASSERT_EQUALS(tree.toString(), "{ A, A, D, E, E, F }");
      tree.erase('F');
      TS_ASSERT_EQUALS(tree.toString(), "{ A, A, D, E, E }");
      tree.erase('E');
      TS_ASSERT_EQUALS(tree.toString(), "{ A, A, D, E }");
      tree.erase('A');
      TS_ASSERT_EQUALS(tree.toString(), "{ A, D, E }");
      tree.erase('A');
      TS_ASSERT_EQUALS(tree.toString(), "{ D, E }");
      tree.erase('D');
      TS_ASSERT_EQUALS(tree.toString(), "{ E }");
      tree.erase('E');
      TS_ASSERT_EQUALS(tree.toString(), "{  }");
      TS_ASSERT_THROWS(tree.erase('X'), gum::NotFound);
      TS_ASSERT_EQUALS(tree.toString(), "{  }");
    }
  };
}
