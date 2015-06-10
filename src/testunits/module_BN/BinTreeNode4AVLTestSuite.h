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

#include <agrum/core/AVLSearchTree.h>

namespace gum_tests {

  class binTreeNode4AVLTestSuite : public CxxTest::TestSuite {
    public:
    void testConstructors() {
      gum::BinTreeNode4AVL<int> *node = 0;

      TS_GUM_ASSERT_THROWS_NOTHING((node = new gum::BinTreeNode4AVL<int>(33)));
      TS_ASSERT(node->value() == 33);
      TS_ASSERT(**node == 33);
      TS_GUM_ASSERT_THROWS_NOTHING((delete node));

      gum::BinTreeNode4AVL<int> node2(33);
      gum::BinTreeNode4AVL<int> node3 = node2;
      TS_ASSERT(*node2 == *node3);

      gum::BinTreeNode4AVL<int> node4(1);
      node4 = node2;
      TS_ASSERT(*node2 == *node4);
    }

    void testInsertChildren() {
      gum::BinTreeNode4AVL<int> node(0);

      TS_ASSERT(node.parent() == 0);
      TS_ASSERT(node.leftChild() == 0);
      TS_ASSERT(node.rightChild() == 0);
      TS_ASSERT(node.child(gum::BinTreeDir::LEFT_CHILD) == 0);
      TS_ASSERT(node.child(gum::BinTreeDir::RIGHT_CHILD) == 0);

      gum::BinTreeNode4AVL<int> node2(1);
      gum::BinTreeNode4AVL<int> node3(2);

      node.insertLeftChild(node2);
      TS_ASSERT_THROWS_ANYTHING(node.insertLeftChild(node3));
      node.insertRightChild(node3);
      TS_ASSERT(node.leftChild() == &node2);
      TS_ASSERT(node.rightChild() == &node3);
      TS_ASSERT(node2.parent() == &node);
      TS_ASSERT(node3.parent() == &node);
      TS_ASSERT(node3.parentDir() == gum::BinTreeDir::RIGHT_CHILD);

      node2.insertLeftChild(4);
      node2.insertRightChild(6);
      TS_ASSERT(node.leftChild()->leftChild()->value() == 4);
      TS_ASSERT(node.leftChild()->rightChild()->value() == 6);

      delete node2.leftChild();
      delete node2.rightChild();

      TS_ASSERT(node.leftChild()->leftChild() == 0);
      TS_ASSERT(node.leftChild()->rightChild() == 0);

      gum::BinTreeNode4AVL<int> *node4 = node2.insertLeftChild(3);
      gum::BinTreeNode4AVL<int> *node5 = node2.insertRightChild(5);
      TS_ASSERT(node2.leftChild() == node4);
      TS_ASSERT(node2.rightChild() == node5);
      TS_ASSERT(node4->parent() == &node2);
      TS_ASSERT(node5->parent() == &node2);

      gum::BinTreeNode4AVL<int> *node6 =
          node4->insertChild(6, gum::BinTreeDir::LEFT_CHILD);
      gum::BinTreeNode4AVL<int> *node7 = new gum::BinTreeNode4AVL<int>(8);
      node4->insertChild(*node7, gum::BinTreeDir::RIGHT_CHILD);
      delete (node4);
      delete (node5);
      delete (node6);
      delete (node7);
    }

    void testEraseLink() {
      gum::BinTreeNode4AVL<int> node1(0);
      gum::BinTreeNode4AVL<int> node2(1);
      gum::BinTreeNode4AVL<int> node3(2);
      node1.insertLeftChild(node2);
      node1.insertRightChild(node3);

      TS_ASSERT(node1.leftChild() == &node2);
      TS_ASSERT(node1.rightChild() == &node3);

      node1.eraseLeftLink();
      node1.eraseRightLink();

      TS_ASSERT(node1.leftChild() == 0);
      TS_ASSERT(node1.rightChild() == 0);
      TS_ASSERT(node2.parent() == 0);
      TS_ASSERT(node3.parent() == 0);

      node1.insertLeftChild(node2);
      node1.insertRightChild(node3);
      node1.eraseLink(gum::BinTreeDir::LEFT_CHILD);
      node1.eraseLink(gum::BinTreeDir::RIGHT_CHILD);
    }

    void testTopBottom() {
      gum::BinTreeNode4AVL<int> node1(0);
      gum::BinTreeNode4AVL<int> node2(1);
      gum::BinTreeNode4AVL<int> node3(2);
      node1.insertLeftChild(node2);
      node1.insertRightChild(node3);

      TS_ASSERT(node1.leftmostNode() == &node2);
      TS_ASSERT(node1.rightmostNode() == &node3);
      TS_ASSERT(node1.root() == &node1);
    }
  };
}
