/***************************************************************************
 *   Copyright (C) 2007 by Jean-Philippe Dubus                             *
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

#include <cxxtest/TestSuite.h>
#include <agrum/core/avlTree.h>


class AvlTestSuite : public CxxTest::TestSuite {

private:
  
  typedef gum::AVLTree<std::string> tree_t;

  tree_t* create() {
    tree_t* result = new tree_t;
    result->insert(1, "one");
    result->insert(2, "two");
    result->insert(3, "three");
    result->insert(4, "four");
    result->insert(5, "five");
    result->insert(10, "ten");
    result->insert(25, "twenty-five");
    result->insert(50, "fifteen");
    result->insert(100, "hundred");
    result->insert(200, "2 hundred");
    result->insert(300, "3 hundred");
    result->insert(400, "4 hundred");
    result->insert(500, "5 hundred");
    return result;
  }

public:

  void testConstructor() {
    tree_t* tree = 0;
    TS_GUM_ASSERT_THROWS_NOTHING((tree = new tree_t));
    TS_GUM_ASSERT_THROWS_NOTHING((delete tree));
  }

  void testInsert() {
    tree_t* tree = 0;
    TS_GUM_ASSERT_THROWS_NOTHING((tree = create()));
    TS_ASSERT(tree->getNbrElts() == 13);
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((delete tree));
  }

  void testSearch() {
    tree_t* tree = create();
    TS_ASSERT((tree->search(1).compare("one") == 0));
    TS_ASSERT((tree->search(2).compare("two") == 0));
    TS_ASSERT((tree->search(3).compare("three") == 0));
    TS_ASSERT((tree->search(4).compare("four") == 0));
    TS_ASSERT((tree->search(5).compare("five") == 0));
    TS_ASSERT((tree->search(10).compare("ten") == 0));
    TS_ASSERT((tree->search(25).compare("twenty-five") == 0));
    TS_ASSERT((tree->search(50).compare("fifteen") == 0));
    TS_ASSERT((tree->search(100).compare("hundred") == 0));
    TS_ASSERT((tree->search(200).compare("2 hundred") == 0));
    TS_ASSERT((tree->search(300).compare("3 hundred") == 0));
    TS_ASSERT((tree->search(400).compare("4 hundred") == 0));
    TS_ASSERT((tree->search(500).compare("5 hundred") == 0));
    delete tree;
  }

  void test_delete() {
    tree_t* tree = create();
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(50)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(1)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(2)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(3)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(4)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(5)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(10)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(25)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(100)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(200)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(300)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(400)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteByPriority(500)));
    TS_ASSERT(tree->hasAVLProperty());
    TS_ASSERT(tree->getNbrElts() == 0);
    delete tree;
  }

  void test_maximal() {
    tree_t* tree = create();
    TS_ASSERT((tree->getMaximalPriority() == 500));
    TS_ASSERT((tree->getMaximalElement().compare("5 hundred") == 0));
    TS_GUM_ASSERT_THROWS_NOTHING((tree->deleteMaximalElement()));
    TS_ASSERT((tree->getMaximalPriority() == 400));
    TS_ASSERT((tree->getMaximalElement().compare("4 hundred") == 0));
    delete tree;
  }
};
