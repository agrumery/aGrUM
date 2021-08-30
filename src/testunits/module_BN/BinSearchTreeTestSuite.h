/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/core/binSearchTree.h>

namespace gum_tests {

  class binSearchTreeTestSuite: public CxxTest::TestSuite {
    public:
    void testConstructors() {
      gum::BinSearchTree< int >* tree = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(tree = new gum::BinSearchTree< int >);
      TS_ASSERT_EQUALS(tree->size(), (gum::Size)0)

      TS_GUM_ASSERT_THROWS_NOTHING(gum::BinSearchTree< int > tree2(*tree));
      gum::BinSearchTree< int > tree3;
      gum::BinSearchTree< int > tree4 = *tree;
      tree3                           = tree4;

      tree->insert(3);
      tree->insert(4);

      TS_ASSERT_EQUALS(tree->size(), (gum::Size)2)
      tree3 = *tree;
      TS_ASSERT_EQUALS(tree3.size(), (gum::Size)2)

      tree->insert(1);
      tree->insert(4);

      tree->erase(3);

      delete tree;
    }

    void testValues() {
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
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)0)
      TS_ASSERT_EQUALS(tree.empty(), true)
    }

    void testErase() {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(5);
      tree.insert(5);

      TS_ASSERT_EQUALS(tree.size(), (gum::Size)7)

      tree.erase(5);
      tree.erase(5);
      tree.erase(5);

      TS_ASSERT_EQUALS(tree.size(), (gum::Size)4)
      TS_ASSERT_THROWS(tree.erase(5), gum::NotFound)
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)4)
      TS_ASSERT_THROWS(tree.erase(9), gum::NotFound)
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)4)
      tree.erase(1);
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)3)
      tree.erase(7);
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)2)
      tree.erase(4);
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)1)
      tree.erase(3);
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)0)
      TS_ASSERT_EQUALS(tree.empty(), true)
    }

    void testUniqueness() {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);

      TS_ASSERT_EQUALS(tree.uniquenessPolicy(), false)

      tree.insert(5);
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)6)

      tree.setUniquenessPolicy(true);
      TS_ASSERT_THROWS_ANYTHING(tree.insert(5);)
    }

    void testIterators() {
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
          case 0:
            TS_ASSERT_EQUALS(*iter, 1)
            break;

          case 1:
            TS_ASSERT_EQUALS(*iter, 3)
            break;

          case 2:
            TS_ASSERT_EQUALS(*iter, 4)
            break;

          case 3:
            TS_ASSERT_EQUALS(*iter, 5)
            break;

          case 4:
            TS_ASSERT_EQUALS(*iter, 5)
            break;

          case 5:
            TS_ASSERT_EQUALS(*iter, 7)
            break;
        }
      }

      i = 5;

      for (gum::BinSearchTree< int >::iterator iter = tree.rbegin(); iter != tree.rend();
           --iter, --i) {
        switch (i) {
          case 0:
            TS_ASSERT_EQUALS(*iter, 1)
            break;

          case 1:
            TS_ASSERT_EQUALS(*iter, 3)
            break;

          case 2:
            TS_ASSERT_EQUALS(*iter, 4)
            break;

          case 3:
            TS_ASSERT_EQUALS(*iter, 5)
            break;

          case 4:
            TS_ASSERT_EQUALS(*iter, 5)
            break;

          case 5:
            TS_ASSERT_EQUALS(*iter, 7)
            break;
        }
      }

      gum::BinSearchTree< int >::iterator iter = tree.root();

      TS_ASSERT_EQUALS(*iter, 5)
    }

    void testEraseIterator() {
      gum::BinSearchTree< int > tree;
      tree.insert(5);
      tree.insert(3);
      tree.insert(1);
      tree.insert(4);
      tree.insert(7);
      tree.insert(5);
      tree.insert(5);

      TS_ASSERT_EQUALS(tree.size(), (gum::Size)7)

      gum::BinSearchTree< int >::iterator iter = tree.begin();

      tree.erase(iter);
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)6)

      iter = tree.rbegin();
      tree.erase(iter);
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)5)

      iter = tree.begin();
      ++iter;
      ++iter;
      tree.erase(iter);
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)4)

      iter = tree.end();
      tree.erase(iter);
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)4)

      iter = tree.rend();
      tree.erase(iter);
      TS_ASSERT_EQUALS(tree.size(), (gum::Size)4)
    }
  };
}   // namespace gum_tests
