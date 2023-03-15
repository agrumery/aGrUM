/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <agrum/tools/core/AVLTree.h>
#include <agrum/tools/core/heap.h>
#include <string>
#include <algorithm>

namespace gum_tests {

  class [[maybe_unused]] AVLTreeTestSuite: public CxxTest::TestSuite {
    public:
    GUM_TEST(_int) {
      gum::AVLTree< int > tree1;
      TS_GUM_ASSERT_EQUALS(tree1.empty(), true)
      TS_GUM_ASSERT_EQUALS(tree1.size(), gum::Size(0))
      TS_ASSERT_THROWS(tree1.lowestValue(), const gum::NotFound&)
      TS_ASSERT_THROWS(tree1.highestValue(), const gum::NotFound&)

      gum::AVLTree< int > tree2 = tree1;

      std::vector< int > vect = {3, 1, 7, -5, 2, 4, 3, 6, 7, 7};

      for (const auto x: vect)
        tree1.insert(x);

      tree2 = tree1;

      std::sort(vect.begin(), vect.end());
      gum::AVLTreeIterator< int > tree1_beg(tree1, true), tree1_end = tree1.end();
      for (int i = 0; tree1_beg != tree1_end; ++tree1_beg, ++i)
        TS_GUM_ASSERT_EQUALS(*tree1_beg, vect[i]);
      int i = 0;
      for (auto iter = tree1.begin(); iter != tree1.end(); ++iter) {
        TS_GUM_ASSERT_EQUALS(*iter, vect[i++])
      }
      i = 0;
      for (const auto val: tree2) {
        TS_GUM_ASSERT_EQUALS(val, vect[i++]);
      }
      gum::AVLTree< int > tree3(std::move(tree2));
      i = 0;
      for (const auto val: tree3) {
        TS_GUM_ASSERT_EQUALS(val, vect[i++]);
      }

      {
        const std::string sss = tree3.toString();
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
        str2 << tree3;
        TS_GUM_ASSERT_EQUALS(str.str(), str2.str())
      }

      gum::AVLTree< int > tree4;
      for (int i = 0; i < 100; ++i)
        tree4.insert(i);
      i = (int)tree4.size() - 1;
      for (auto iter = tree4.rbegin(); iter != tree4.rend(); ++iter, --i) {
        TS_GUM_ASSERT_EQUALS(*iter, i)
      }
      i = 99;
      for (auto iter = tree4.rbegin(); iter != tree4.rend(); iter += 2, i -= 2) {
        TS_GUM_ASSERT_EQUALS(*iter, i)
      }
      i = 0;
      for (auto iter = tree4.begin(); iter != tree4.end(); iter += 2, i += 2) {
        TS_GUM_ASSERT_EQUALS(*iter, i)
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

      tree2 = tree4;

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
          TS_GUM_ASSERT_EQUALS(*iter2, 5)
        } else if (i >= 2) TS_GUM_ASSERT_EQUALS(*iter3, (i - 2) * 4 + 9)
      }

      TS_GUM_ASSERT_EQUALS(tree2.size(), 100)
      i          = 0;
      auto iter4 = tree2.rbeginSafe();
      iter4 += 3;
      for (auto iter = tree2.rbeginSafe(); iter != tree2.rendSafe(); iter4 += 3, ++i) {
        iter += 2;
        tree2.erase(iter);
        TS_ASSERT_THROWS(*iter, const gum::NotFound&)
        iter += 2;

        if (i == 1) {
          TS_ASSERT_THROWS(*iter4, const gum::NotFound&)
          auto iter2 = iter4;
          --iter2;
          TS_GUM_ASSERT_EQUALS(*iter2, 95)
        } else if (i >= 2) TS_GUM_ASSERT_EQUALS(*iter4, 99 - (i - 2) * 4 - 9)
      }

      auto iter5 = gum::AVLTreeIteratorSafe(tree2, false);   // equiv rbegin
      TS_GUM_ASSERT_EQUALS(++iter5, tree2.endSafe())
      TS_ASSERT_THROWS_NOTHING(tree2.erase(iter5));

      iter3 = tree2.beginSafe();
      iter4 = tree2.rbeginSafe();
      tree2.clear();
      TS_GUM_ASSERT_EQUALS(iter3, tree2.endSafe())
      TS_GUM_ASSERT_EQUALS(iter4, tree2.rendSafe())

      tree2.insert(std::move(4));
      tree2.emplace(5);
      TS_GUM_ASSERT_EQUALS(tree2.size(), gum::Size(2))
      tree2.erase(4);
      tree2.erase(3);
      TS_GUM_ASSERT_EQUALS(tree2.size(), gum::Size(1))
      TS_GUM_ASSERT_EQUALS(*(tree2.begin()), 5)
      tree2.erase(5);
      TS_GUM_ASSERT_EQUALS(tree2.size(), gum::Size(0))
    }

    GUM_TEST(_safe) {
      gum::AVLTree< int > tree1;
      std::vector< int >  vect  = {3, 1, 7, -5, 2, 4, 3, 6, 7, 7};
      auto                iter1 = tree1.beginSafe();
      auto                iter2 = new gum::AVLTreeIteratorSafe< int >(tree1);
      auto                iter3 = tree1.rbeginSafe();
      auto                iter4 = new gum::AVLTreeReverseIteratorSafe< int >(tree1);

      TS_GUM_ASSERT_EQUALS(iter1, *iter2)
      TS_GUM_ASSERT_EQUALS(iter3, *iter4)
      gum::AVLTree< int > tree2(std::move(tree1));
      auto                iter5 = tree1.beginSafe();
      auto                iter6 = tree1.rbeginSafe();
      TS_GUM_ASSERT_EQUALS(iter5, *iter2)
      TS_GUM_ASSERT_EQUALS(iter6, *iter4)
      delete iter2;
      delete iter4;
      gum::AVLTree< int > tree3(std::move(tree2));
      TS_GUM_ASSERT_EQUALS(iter5, iter1)
      TS_GUM_ASSERT_EQUALS(iter6, iter3)
    }

    struct Mycmp {
      constexpr bool operator()(const std::pair< int, int >& x,
                                const std::pair< int, int >& y) const {
        return x.first < y.first;
      }
    };

    GUM_TEST(_pairs) {
      gum::AVLTree< std::pair< int, int >, Mycmp > tree1;
      TS_GUM_ASSERT_EQUALS(tree1.empty(), true)
      TS_GUM_ASSERT_EQUALS(tree1.size(), gum::Size(0))
      TS_ASSERT_THROWS(tree1.lowestValue(), const gum::NotFound&)
      TS_ASSERT_THROWS(tree1.highestValue(), const gum::NotFound&)

      gum::AVLTree< std::pair< int, int >, Mycmp > tree2 = tree1;

      std::vector< int >                   vect1 = {3, 1, 7, -5, 2, 4, 3, 6, 7, 7};
      std::vector< std::pair< int, int > > vect;
      for (auto x: vect1) {
        vect.push_back({x, x + 5});
      }

      for (const auto& x: vect)
        tree1.insert(x);

      tree2 = tree1;

      std::sort(vect.begin(), vect.end());
      gum::AVLTreeIterator< std::pair< int, int >, Mycmp > tree1_beg(tree1, true),
         tree1_end = tree1.end();
      for (int i = 0; tree1_beg != tree1_end; ++tree1_beg, ++i) {
        TS_GUM_ASSERT_EQUALS(*tree1_beg, vect[i]);
      }
      int i = 0;
      for (auto iter = tree1.begin(); iter != tree1.end(); ++iter) {
        TS_GUM_ASSERT_EQUALS(*iter, vect[i++])
      }
      i = 0;
      for (const auto& val: tree2) {
        TS_GUM_ASSERT_EQUALS(val, vect[i++]);
      }

      gum::AVLTree< std::pair< int, int >, Mycmp > tree3(std::move(tree2));
      i = 0;
      for (const auto& val: tree3) {
        TS_GUM_ASSERT_EQUALS(val, vect[i++]);
      }

      gum::AVLTree< std::pair< int, int >, Mycmp > tree4;
      for (int i = 0; i < 100; ++i)
        tree4.emplace(i, i + 5);
      i = (int)tree4.size() - 1;
      for (auto iter = tree4.rbegin(); iter != tree4.rend(); ++iter, --i) {
        std::pair< int, int > pair(i, i + 5);
        TS_GUM_ASSERT_EQUALS(*iter, pair)
      }
      i = 99;
      for (auto iter = tree4.rbegin(); iter != tree4.rend(); iter += 2, i -= 2) {
        std::pair< int, int > pair(i, i + 5);
        TS_GUM_ASSERT_EQUALS(*iter, pair)
      }
      i = 0;
      for (auto iter = tree4.begin(); iter != tree4.end(); iter += 2, i += 2) {
        std::pair< int, int > pair(i, i + 5);
        TS_GUM_ASSERT_EQUALS(*iter, pair)
      }
      TS_GUM_ASSERT_EQUALS(tree4.size(), gum::Size(100))

      tree2 = tree4;

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
          std::pair< int, int > pair(5, 10);
          TS_GUM_ASSERT_EQUALS(*iter2, pair)
        } else if (i >= 2) {
          const int             x = (i - 2) * 4 + 9;
          std::pair< int, int > pair(x, x + 5);
          TS_GUM_ASSERT_EQUALS(*iter3, pair)
        }
      }

      TS_GUM_ASSERT_EQUALS(tree2.size(), 100)
      i          = 0;
      auto iter4 = tree2.rbeginSafe();
      iter4 += 3;
      for (auto iter = tree2.rbeginSafe(); iter != tree2.rendSafe(); iter4 += 3, ++i) {
        iter += 2;
        tree2.erase(iter);
        TS_ASSERT_THROWS(*iter, const gum::NotFound&)
        iter += 2;

        if (i == 1) {
          TS_ASSERT_THROWS(*iter4, const gum::NotFound&)
          auto iter2 = iter4;
          --iter2;
          std::pair< int, int > pair(95, 100);
          TS_GUM_ASSERT_EQUALS(*iter2, pair)
        } else if (i >= 2) {
          const int             x = 99 - (i - 2) * 4 - 9;
          std::pair< int, int > pair(x, x + 5);
          TS_GUM_ASSERT_EQUALS(*iter4, pair)
        }
      }

      auto iter5 = gum::AVLTreeIteratorSafe(tree2, false);   // equiv rbegin
      TS_GUM_ASSERT_EQUALS(++iter5, tree2.endSafe())
      TS_ASSERT_THROWS_NOTHING(tree2.erase(iter5));

      iter3 = tree2.beginSafe();
      iter4 = tree2.rbeginSafe();
      tree2.clear();
      TS_GUM_ASSERT_EQUALS(iter3, tree2.endSafe())
      TS_GUM_ASSERT_EQUALS(iter4, tree2.rendSafe())

      std::pair< int, int > pair(4, 9);
      tree2.insert(std::move(pair));
      tree2.emplace(5, 10);
      TS_GUM_ASSERT_EQUALS(tree2.size(), gum::Size(2))
      std::pair< int, int > pair2(4, 9);
      tree2.erase(pair2);
      std::pair< int, int > pair3(3, 8);
      tree2.erase(pair3);
      TS_GUM_ASSERT_EQUALS(tree2.size(), gum::Size(1))
      std::pair< int, int > pair4(5, 10);
      TS_GUM_ASSERT_EQUALS(*(tree2.begin()), pair4)
      tree2.erase(pair4);
      TS_GUM_ASSERT_EQUALS(tree2.size(), gum::Size(0))
    }

    GUM_TEST(_erase) {
      gum::AVLTree< int >      tree{8, 10, 2, 23, 24};
      const std::vector< int > vect{2, 8, 10, 23, 24};
      {
        const std::string sss = tree.toString();
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
        str2 << tree;
        TS_GUM_ASSERT_EQUALS(str.str(), str2.str())
      }

      TS_ASSERT_EQUALS(tree.size(), (gum::Size)5)
      TS_ASSERT_EQUALS(tree.empty(), false)
      TS_ASSERT_EQUALS(tree.contains(8), true)
      TS_ASSERT_EQUALS(tree.contains(5), false)

      tree.erase(tree.highestValue());
      const int high = tree.highestValue();
      TS_ASSERT_EQUALS(high, 23)
      TS_ASSERT_EQUALS(tree.highestValue(), 23)

      tree.erase(tree.highestValue());   // remains: {2, 8, 10}
      TS_ASSERT_EQUALS(tree.highestValue(), 10)
      tree.erase(tree.highestValue());   // remains: {2, 8}
      TS_ASSERT_EQUALS(tree.highestValue(), 8)
      tree.erase(tree.highestValue());   // remains: {2}
      TS_ASSERT_EQUALS(tree.highestValue(), 2)
      tree.erase(tree.highestValue());   // remains: {}
      TS_ASSERT_THROWS(tree.highestValue(), gum::NotFound&)

      for (const auto val: vect)
        tree.insert(val);
      {
        const std::string sss = tree.toString();
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
        str2 << tree;
        TS_GUM_ASSERT_EQUALS(str.str(), str2.str())
      }

      tree.erase(tree.lowestValue());   // remains: {8, 10, 23, 24}
      const int low = tree.lowestValue();
      TS_ASSERT_EQUALS(low, 8)
      TS_ASSERT_EQUALS(tree.lowestValue(), 8)

      tree.erase(tree.lowestValue());   // remains: {10, 23, 24}
      TS_ASSERT_EQUALS(tree.lowestValue(), 10)
      tree.erase(tree.lowestValue());   // remains: {23, 24}
      TS_ASSERT_EQUALS(tree.lowestValue(), 23)
      tree.erase(tree.lowestValue());   // remains: {24}
      TS_ASSERT_EQUALS(tree.lowestValue(), 24)
      tree.erase(tree.lowestValue());   // remains: {}
      TS_ASSERT_THROWS(tree.lowestValue(), gum::NotFound&)
    }

    GUM_TEST(_shuffle) {
      gum::AVLTree< std::pair< int, int >, Mycmp > tree;
      TS_GUM_ASSERT_EQUALS(tree.empty(), true)
      TS_GUM_ASSERT_EQUALS(tree.size(), gum::Size(0))
      TS_ASSERT_THROWS(tree.lowestValue(), const gum::NotFound&)
      TS_ASSERT_THROWS(tree.highestValue(), const gum::NotFound&)

      std::vector< std::pair< int, int > > vect;
      for (int i = 0; i < 100; ++i) {
        vect.emplace_back(i, i + 10);
      }
      auto vect2 = vect;
      std::shuffle(std::begin(vect2), std::end(vect2), gum::randomGenerator());
      for (const auto& elt: vect2) {
        if (tree.size() % 2 == 0) tree.insert(elt);
        else {
          auto elt2 = elt;
          tree.insert(std::move(elt2));
        }
      }
      TS_GUM_ASSERT_EQUALS(vect2string(vect), tree.toString())

      std::shuffle(std::begin(vect2), std::end(vect2), gum::randomGenerator());
      for (const auto& elt: vect2) {
        tree.erase(elt);
        vect.erase(vect.begin() + pos2vect(vect, elt));
        TS_GUM_ASSERT_EQUALS(vect2string(vect), tree.toString())
      }
    }

    private:
    std::size_t pos2vect(const std::vector< std::pair< int, int > >& vect,
                         const std::pair< int, int >&                elt) {
      for (std::size_t i = 0, size = vect.size(); i < size; ++i) {
        if (vect[i] == elt) return i;
      }
      throw(gum::NotFound(0));
    }

    std::string vect2string(const std::vector< std::pair< int, int > >& vect) {
      bool              deja = false;
      std::stringstream stream;
      stream << "{";
      for (const auto& elt: vect) {
        if (deja) stream << " , ";
        else deja = true;
        stream << elt;
      }
      stream << "}";
      return stream.str();
    }
  };

}   // namespace gum_tests
