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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





#include <algorithm>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/heap.h>
#include <agrum/base/core/sharedAVLTree.h>

namespace gum_tests {

  class [[maybe_unused]] SharedAVLTreeTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_int) {
      gum::SharedAVLTree< int > tree1;
      TS_GUM_ASSERT_EQUALS(tree1.empty(), true)
      TS_GUM_ASSERT_EQUALS(tree1.size(), gum::Size(0))
      TS_ASSERT_THROWS(tree1.lowestValue(), const gum::NotFound&)
      TS_ASSERT_THROWS(tree1.highestValue(), const gum::NotFound&)

      std::vector< int >                                vect = {3, 1, 7, -5, 2, 4, 3, 6, 7, 7};
      std::vector< gum::SharedAVLTree< int >::AVLNode > nodevect1;
      for (auto x: vect)
        nodevect1.emplace_back(x);
      for (auto& x: nodevect1)
        tree1.insert(&x);

      gum::SharedAVLTree< int >                         tree2;
      std::vector< gum::SharedAVLTree< int >::AVLNode > nodevect2;
      for (auto x: vect)
        nodevect2.emplace_back(x);
      for (auto& x: nodevect2)
        tree2.insert(&x);

      std::sort(vect.begin(), vect.end());
      gum::SharedAVLTreeIterator< int > tree1_beg(tree1, true), tree1_end = tree1.end();
      for (int i = 0; tree1_beg != tree1_end; ++tree1_beg, ++i)
        TS_GUM_ASSERT_EQUALS((*tree1_beg).value, vect[i]);
      int i = 0;
      for (auto iter = tree1.begin(); iter != tree1.end(); ++iter) {
        TS_GUM_ASSERT_EQUALS(iter->value, vect[i++])
      }
      i = 0;
      for (const auto& node: tree2) {
        TS_GUM_ASSERT_EQUALS(node.value, vect[i++]);
      }

      {
        const std::string sss = tree2.toString();
        std::stringstream str;
        str << '{';
        bool first = true;
        for (const auto val: vect) {
          if (!first) str << " , ";
          else first = false;
          str << val;
        }
        str << '}';
        TS_GUM_ASSERT_EQUALS(sss, str.str())
        std::stringstream str2;
        str2 << tree2;
        TS_GUM_ASSERT_EQUALS(str.str(), str2.str())
      }

      gum::SharedAVLTree< int >                         tree4;
      std::vector< gum::SharedAVLTree< int >::AVLNode > nodevect4;
      for (int i = 0; i < 100; ++i)
        nodevect4.emplace_back(i);
      for (int i = 0; i < 100; ++i)
        tree4.insert(&nodevect4[i]);
      i = (int)tree4.size() - 1;
      for (auto iter = tree4.rbegin(); iter != tree4.rend(); ++iter, --i)
        TS_GUM_ASSERT_EQUALS((*iter).value, i)
      i = 99;
      for (auto iter = tree4.rbegin(); iter != tree4.rend(); iter += 2, i -= 2) {
        TS_GUM_ASSERT_EQUALS(iter->value, i)
      }
      i = 0;
      for (auto iter = tree4.begin(); iter != tree4.end(); iter += 2, i += 2) {
        TS_GUM_ASSERT_EQUALS(iter->value, i)
      }
      TS_GUM_ASSERT_EQUALS(tree4.size(), gum::Size(100))

      {
        auto xiter1 = tree4.begin();
        xiter1      = tree4.end();
        xiter1      = tree4.begin();

        auto xiter2 = tree4.rbegin();
        xiter2      = tree4.rend();
        xiter2      = tree4.rbegin();

        auto xiter3 = tree4.beginSafe();
        xiter3      = tree4.endSafe();
        xiter3      = tree4.beginSafe();

        auto xiter4 = tree4.rbeginSafe();
        xiter4      = tree4.rendSafe();
        xiter4      = tree4.rbeginSafe();
      }

      i          = 0;
      auto iter3 = tree4.beginSafe();
      iter3 += 3;
      for (auto iter = tree4.beginSafe(); iter != tree4.endSafe(); i++, iter3 += 3) {
        iter += 2;
        tree4.erase(iter);
        TS_ASSERT_THROWS(*iter, const gum::NotFound&)
        iter += 2;

        if (i == 1) {
          TS_ASSERT_THROWS(*iter3, const gum::NotFound&)
          auto iter2 = iter3;
          --iter2;
          TS_GUM_ASSERT_EQUALS(iter2->value, 5)
        } else if (i >= 2) TS_GUM_ASSERT_EQUALS(iter3->value, (i - 2) * 4 + 9)
      }

      gum::SharedAVLTree< int >                         tree5;
      std::vector< gum::SharedAVLTree< int >::AVLNode > nodevect5;
      for (int i = 0; i < 100; ++i)
        nodevect5.emplace_back(i);
      for (int i = 0; i < 100; ++i)
        tree5.insert(&nodevect5[i]);
      TS_GUM_ASSERT_EQUALS(tree5.size(), 100)
      i          = 0;
      auto iter5 = tree5.rbeginSafe();
      iter5 += 3;
      for (auto iter = tree5.rbeginSafe(); iter != tree5.rendSafe(); iter5 += 3, ++i) {
        iter += 2;
        tree5.erase(iter);
        TS_ASSERT_THROWS(*iter, const gum::NotFound&)
        iter += 2;

        if (i == 1) {
          TS_ASSERT_THROWS(*iter5, const gum::NotFound&)
          auto iter2 = iter5;
          --iter2;
          TS_GUM_ASSERT_EQUALS(iter2->value, 95)
        } else if (i >= 2) TS_GUM_ASSERT_EQUALS(iter5->value, 99 - (i - 2) * 4 - 9)
      }

      auto iter6 = gum::SharedAVLTreeIteratorSafe(tree5, false);   // equiv rbegin
      TS_GUM_ASSERT_EQUALS(++iter6, tree5.endSafe())
      TS_ASSERT_THROWS_NOTHING(tree5.erase(iter6));

      iter3 = tree2.beginSafe();
      iter5 = tree2.rbeginSafe();
      tree2.clear();
      TS_GUM_ASSERT_EQUALS(iter3, tree2.endSafe())
      TS_GUM_ASSERT_EQUALS(iter5, tree2.rendSafe())
    }

    GUM_ACTIVE_TEST(_safe) {
      gum::SharedAVLTree< int >                         tree1;
      std::vector< int >                                vect1 = {3, 1, 7, -5, 2, 4, 3, 6, 7, 7};
      std::vector< gum::SharedAVLTree< int >::AVLNode > nodevect1;
      for (auto x: vect1)
        nodevect1.emplace_back(x);

      auto iter1 = tree1.beginSafe();
      auto iter2 = new gum::SharedAVLTreeIteratorSafe< int >(tree1);
      auto iter3 = tree1.rbeginSafe();
      auto iter4 = new gum::SharedAVLTreeReverseIteratorSafe< int >(tree1);

      TS_GUM_ASSERT_EQUALS(iter1, *iter2)
      TS_GUM_ASSERT_EQUALS(iter3, *iter4)
      delete iter2;
      delete iter4;
    }

    struct Mycmp {
      constexpr bool operator()(const std::pair< int, int >& x,
                                const std::pair< int, int >& y) const {
        return x.first < y.first;
      }
    };

    GUM_ACTIVE_TEST(_pairs) {
      gum::SharedAVLTree< std::pair< int, int >, Mycmp > tree1;

      TS_GUM_ASSERT_EQUALS(tree1.empty(), true)
      TS_GUM_ASSERT_EQUALS(tree1.size(), gum::Size(0))
      TS_GUM_ASSERT_EQUALS(tree1.lowestNode(), nullptr)
      TS_GUM_ASSERT_EQUALS(tree1.highestNode(), nullptr)

      std::vector< int >                   vect = {3, 1, 7, -5, 2, 4, 3, 6, 7, 7};
      std::vector< std::pair< int, int > > vectp;
      for (auto x: vect)
        vectp.emplace_back(x, x + 5);

      std::vector< gum::SharedAVLTree< std::pair< int, int >, Mycmp >::AVLNode > nodevect1;
      for (const auto& x: vectp)
        nodevect1.emplace_back(x);
      for (auto& node: nodevect1)
        tree1.insert(&node);

      gum::SharedAVLTree< std::pair< int, int >, Mycmp >                         tree2;
      std::vector< gum::SharedAVLTree< std::pair< int, int >, Mycmp >::AVLNode > nodevect2;
      for (const auto& x: vectp)
        nodevect2.emplace_back(x);
      for (auto& node: nodevect2)
        tree2.insert(&node);

      std::sort(vectp.begin(), vectp.end(), Mycmp());
      gum::SharedAVLTreeIterator< std::pair< int, int >, Mycmp > tree1_beg(tree1, true),
          tree1_end = tree1.end();
      for (int i = 0; tree1_beg != tree1_end; ++tree1_beg, ++i)
        TS_GUM_ASSERT_EQUALS((*tree1_beg).value, vectp[i]);

      int i = 0;
      for (auto iter = tree1.begin(); iter != tree1.end(); ++iter) {
        TS_GUM_ASSERT_EQUALS(iter->value, vectp[i++])
      }
      i = 0;
      for (const auto& node: tree2) {
        TS_GUM_ASSERT_EQUALS(node.value, vectp[i++]);
      }

      std::vector< std::pair< int, int > > vectp2;
      for (int i = 0; i < 100; ++i)
        vectp2.emplace_back(i, i + 5);
      std::vector< gum::SharedAVLTree< std::pair< int, int >, Mycmp >::AVLNode > nodevect4;
      for (const auto& x: vectp2)
        nodevect4.emplace_back(x);
      gum::SharedAVLTree< std::pair< int, int >, Mycmp > tree4;
      for (auto& node: nodevect4)
        tree4.insert(&node);

      i = (int)tree4.size() - 1;
      for (auto iter = tree4.rbegin(); iter != tree4.rend(); ++iter, --i)
        TS_GUM_ASSERT_EQUALS((*iter).value, vectp2[i])
      i = 99;
      for (auto iter = tree4.rbegin(); iter != tree4.rend(); iter += 2, i -= 2) {
        TS_GUM_ASSERT_EQUALS(iter->value, vectp2[i])
      }
      i = 0;
      for (auto iter = tree4.begin(); iter != tree4.end(); iter += 2, i += 2) {
        TS_GUM_ASSERT_EQUALS(iter->value, vectp2[i])
      }
      TS_GUM_ASSERT_EQUALS(tree4.size(), gum::Size(100))

      i          = 0;
      auto iter3 = tree4.beginSafe();
      iter3 += 3;
      for (auto iter = tree4.beginSafe(); iter != tree4.endSafe(); i++, iter3 += 3) {
        iter += 2;
        tree4.erase(iter);
        TS_ASSERT_THROWS(*iter, const gum::NotFound&)
        iter += 2;

        if (i == 1) {
          TS_ASSERT_THROWS(*iter3, const gum::NotFound&)
          auto iter2 = iter3;
          --iter2;
          TS_GUM_ASSERT_EQUALS(iter2->value, vectp2[5])
        } else if (i >= 2) TS_GUM_ASSERT_EQUALS(iter3->value, vectp2[(i - 2) * 4 + 9])
      }

      std::vector< gum::SharedAVLTree< std::pair< int, int >, Mycmp >::AVLNode > nodevect5;
      for (const auto& x: vectp2)
        nodevect5.emplace_back(x);
      gum::SharedAVLTree< std::pair< int, int >, Mycmp > tree5;
      for (auto& node: nodevect5)
        tree5.insert(&node);
      TS_GUM_ASSERT_EQUALS(tree5.size(), 100)
      i          = 0;
      auto iter5 = tree5.rbeginSafe();
      iter5 += 3;
      for (auto iter = tree5.rbeginSafe(); iter != tree5.rendSafe(); iter5 += 3, ++i) {
        iter += 2;
        tree5.erase(iter);
        TS_ASSERT_THROWS(*iter, const gum::NotFound&)
        iter += 2;

        if (i == 1) {
          TS_ASSERT_THROWS(*iter5, const gum::NotFound&)
          auto iter2 = iter5;
          --iter2;
          TS_GUM_ASSERT_EQUALS(iter2->value, vectp2[95])
        } else if (i >= 2) TS_GUM_ASSERT_EQUALS(iter5->value, vectp2[99 - (i - 2) * 4 - 9])
      }

      auto iter6 = gum::SharedAVLTreeIteratorSafe(tree5, false);   // equiv rbegin
      TS_GUM_ASSERT_EQUALS(++iter6, tree5.endSafe())
      TS_ASSERT_THROWS_NOTHING(tree5.erase(iter6));

      iter3 = tree2.beginSafe();
      iter5 = tree2.rbeginSafe();
      tree2.clear();
      TS_GUM_ASSERT_EQUALS(iter3, tree2.endSafe())
      TS_GUM_ASSERT_EQUALS(iter5, tree2.rendSafe())
    }

    GUM_ACTIVE_TEST(_shuffle) {
      gum::SharedAVLTree< std::pair< int, int >, Mycmp > tree;
      TS_GUM_ASSERT_EQUALS(tree.empty(), true)
      TS_GUM_ASSERT_EQUALS(tree.size(), gum::Size(0))
      TS_ASSERT_THROWS(tree.lowestValue(), const gum::NotFound&)
      TS_ASSERT_THROWS(tree.highestValue(), const gum::NotFound&)

      std::vector< gum::AVLTreeNode< std::pair< int, int > >* > vect;
      for (int i = 0; i < 100; ++i) {
        const std::pair< int, int > x(i, i + 10);
        auto                        node = new gum::AVLTreeNode< std::pair< int, int > >(x);
        vect.push_back(node);
      }
      auto vect2 = vect;
      std::shuffle(std::begin(vect2), std::end(vect2), gum::randomGenerator());
      for (auto& elt: vect2) {
        tree.insert(elt);
      }
      TS_GUM_ASSERT_EQUALS(vect2string(vect), tree.toString())

      std::shuffle(std::begin(vect2), std::end(vect2), gum::randomGenerator());
      for (const auto elt: vect2) {
        const auto pos = pos2vect(vect, elt);
        tree.erase(elt);
        vect.erase(vect.begin() + pos);
        delete elt;
        TS_GUM_ASSERT_EQUALS(vect2string(vect), tree.toString())
      }
    }

    private:
    std::size_t pos2vect(const std::vector< gum::AVLTreeNode< std::pair< int, int > >* >& vect,
                         const gum::AVLTreeNode< std::pair< int, int > >*                 elt) {
      for (std::size_t i = 0, size = vect.size(); i < size; ++i) {
        if (vect[i] == elt) return i;
      }
      throw(gum::NotFound(0));
    }

    std::string vect2string(const std::vector< gum::AVLTreeNode< std::pair< int, int > >* >& vect) {
      bool              deja = false;
      std::stringstream stream;
      stream << "{";
      for (const auto elt: vect) {
        if (deja) stream << " , ";
        else deja = true;
        stream << elt->value;
      }
      stream << "}";
      return stream.str();
    }
  };

}   // namespace gum_tests
