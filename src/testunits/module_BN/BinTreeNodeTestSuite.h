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

#include <agrum/base/core/binTreeNode.h>

namespace gum_tests {

  class [[maybe_unused]] binTreeNodeTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Constructors) {
      gum::BinTreeNode< int >* node = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(node = new gum::BinTreeNode< int >(33))
      TS_ASSERT_EQUALS(node->value(), 33)
      TS_ASSERT_EQUALS(**node, 33)
      TS_GUM_ASSERT_THROWS_NOTHING(delete node)

      gum::BinTreeNode< int > node2(33);
      gum::BinTreeNode< int > node3 = node2;
      TS_ASSERT_EQUALS(*node2, *node3)

      gum::BinTreeNode< int > node4(1);
      node4 = node2;
      TS_ASSERT_EQUALS(*node2, *node4)
    }

    GUM_ACTIVE_TEST(InsertChildren) {
      gum::BinTreeNode< int > node(0);

      TS_ASSERT_EQUALS(node.parent(), nullptr)
      TS_ASSERT_EQUALS(node.leftChild(), nullptr)
      TS_ASSERT_EQUALS(node.rightChild(), nullptr)
      TS_ASSERT_EQUALS(node.child(gum::BinTreeDir::LEFT_CHILD), nullptr)
      TS_ASSERT_EQUALS(node.child(gum::BinTreeDir::RIGHT_CHILD), nullptr)

      gum::BinTreeNode< int > node2(1);
      gum::BinTreeNode< int > node3(2);

      node.insertLeftChild(node2);
      TS_ASSERT_THROWS_ANYTHING(node.insertLeftChild(node3))
      node.insertRightChild(node3);
      TS_ASSERT_EQUALS(node.leftChild(), &node2)
      TS_ASSERT_EQUALS(node.rightChild(), &node3)
      TS_ASSERT_EQUALS(node2.parent(), &node)
      TS_ASSERT_EQUALS(node3.parent(), &node)
      TS_ASSERT_EQUALS(node3.parentDir(), gum::BinTreeDir::RIGHT_CHILD)

      node2.insertLeftChild(4);
      node2.insertRightChild(6);
      TS_ASSERT_EQUALS(node.leftChild()->leftChild()->value(), 4)
      TS_ASSERT_EQUALS(node.leftChild()->rightChild()->value(), 6)

      delete node2.leftChild();
      delete node2.rightChild();

      TS_ASSERT_EQUALS(node.leftChild()->leftChild(), nullptr)
      TS_ASSERT_EQUALS(node.leftChild()->rightChild(), nullptr)

      gum::BinTreeNode< int >* node4 = node2.insertLeftChild(3);
      gum::BinTreeNode< int >* node5 = node2.insertRightChild(5);
      TS_ASSERT_EQUALS(node2.leftChild(), node4)
      TS_ASSERT_EQUALS(node2.rightChild(), node5)
      TS_ASSERT_EQUALS(node4->parent(), &node2)
      TS_ASSERT_EQUALS(node5->parent(), &node2)

      gum::BinTreeNode< int >* node6 = node4->insertChild(6, gum::BinTreeDir::LEFT_CHILD);
      gum::BinTreeNode< int >* node7 = new gum::BinTreeNode< int >(8);
      node4->insertChild(*node7, gum::BinTreeDir::RIGHT_CHILD);

      delete (node4);
      delete (node5);
      delete (node6);
      delete (node7);
    }

    GUM_ACTIVE_TEST(EraseLink) {
      gum::BinTreeNode< int > node1(0);
      gum::BinTreeNode< int > node2(1);
      gum::BinTreeNode< int > node3(2);
      node1.insertLeftChild(node2);
      node1.insertRightChild(node3);

      TS_ASSERT_EQUALS(node1.leftChild(), &node2)
      TS_ASSERT_EQUALS(node1.rightChild(), &node3)

      node1.eraseLeftLink();
      node1.eraseRightLink();

      TS_ASSERT_EQUALS(node1.leftChild(), nullptr)
      TS_ASSERT_EQUALS(node1.rightChild(), nullptr)
      TS_ASSERT_EQUALS(node2.parent(), nullptr)
      TS_ASSERT_EQUALS(node3.parent(), nullptr)

      node1.insertLeftChild(node2);
      node1.insertRightChild(node3);
      node1.eraseLink(gum::BinTreeDir::LEFT_CHILD);
      node1.eraseLink(gum::BinTreeDir::RIGHT_CHILD);
    }
  };
}   // namespace gum_tests
