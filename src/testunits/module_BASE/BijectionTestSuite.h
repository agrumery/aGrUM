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

#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
#include <ressources/myalloc.h>

#include <agrum/core/bijection.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/graphs/graphElements.h>

namespace gum_tests {

  class BijectionTestSuite : public CxxTest::TestSuite {
    public:
    void test_constructors1() {
      gum::Bijection<int, int> bijection;

      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(1, 2));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(3, 4));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(5, 6));

      TS_ASSERT_THROWS(bijection.insert(5, 6), gum::DuplicateElement);
      TS_ASSERT_THROWS(bijection.insert(5, 7), gum::DuplicateElement);
      TS_ASSERT_THROWS(bijection.insert(7, 6), gum::DuplicateElement);

      gum::Bijection<int, int> bijection2(bijection);
      TS_ASSERT(bijection2.size() == 3);

      gum::Bijection<int, int, MyAlloc<int>> bij_bis(bijection);
      bij_bis.insert(8, 10);
      TS_ASSERT(bij_bis.size() == 4);

      gum::Bijection<int, int> *bijection3 = new gum::Bijection<int, int>;
      bijection3->insert(1, 2);
      bijection3->insert(3, 3);

      gum::Bijection<int, int> bijection4 = bijection;
      bijection4 = *bijection3;
      delete bijection3;
      TS_ASSERT(bijection4.first(2) == 1);
      TS_ASSERT(bijection4.first(3) == 3);
      TS_ASSERT(bijection4.second(1) == 2);
      TS_ASSERT(bijection4.second(3) == 3);

      gum::Bijection<int, int, MyAlloc<int>> bij_ter(std::move(bij_bis));
      TS_ASSERT(bij_ter.size() == 4);

      gum::Bijection<int, int> bij5{std::pair<int, int>(3, 4),
                                    std::pair<int, int>(5, 6)};
      TS_ASSERT(bij5.size() == 2);

      bij5 = bijection;
      TS_ASSERT(bij5.size() == 3);
      TS_ASSERT(bij5.existsFirst(5));
      TS_ASSERT(bij5.existsFirst(3));

      bij5 = bij_ter;
      TS_ASSERT(bij5.size() == 4);
      TS_ASSERT(bij5.existsFirst(8));

      bij5 = std::move(bijection);
      TS_ASSERT(bij5.size() == 3);
    }

    void test_constructors2() {
      gum::Bijection<std::string, std::string> bijection;

      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("a", "ba"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("b", "bb"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("c", "bc"));

      TS_ASSERT_THROWS(bijection.insert("b", "d"), gum::DuplicateElement);
      TS_ASSERT_THROWS(bijection.insert("b", "bb"), gum::DuplicateElement);
      TS_ASSERT_THROWS(bijection.insert("e", "bc"), gum::DuplicateElement);

      gum::Bijection<std::string, std::string> bijection2(bijection);
      TS_ASSERT(bijection2.size() == 3);

      gum::Bijection<std::string, std::string, MyAlloc<int>> bij_bis(bijection);
      bij_bis.insert("d", "bd");
      TS_ASSERT(bij_bis.size() == 4);

      gum::Bijection<std::string, std::string> *bijection3 =
          new gum::Bijection<std::string, std::string>;
      bijection3->insert("a", "b");
      bijection3->insert("b", "a");

      gum::Bijection<std::string, std::string> bijection4 = bijection;
      bijection4 = *bijection3;
      delete bijection3;
      TS_ASSERT(bijection4.first("a") == "b");
      TS_ASSERT(bijection4.first("b") == "a");
      TS_ASSERT(bijection4.second("a") == "b");
      TS_ASSERT(bijection4.second("b") == "a");

      gum::Bijection<std::string, std::string, MyAlloc<int>> bij_ter(
          std::move(bij_bis));
      TS_ASSERT(bij_ter.size() == 4);

      gum::Bijection<std::string, std::string> bij5{
          std::pair<std::string, std::string>("3", "4"),
          std::pair<std::string, std::string>("5", "6")};
      TS_ASSERT(bij5.size() == 2);

      bij5 = bijection;
      TS_ASSERT(bij5.size() == 3);
      TS_ASSERT(bij5.existsFirst("a"));
      TS_ASSERT(bij5.existsFirst("c"));

      bij5 = bij_ter;
      TS_ASSERT(bij5.size() == 4);
      TS_ASSERT(bij5.existsFirst("d"));

      bij5 = std::move(bijection);
      TS_ASSERT(bij5.size() == 3);
    }

    void testMoves() {
      gum::Bijection<int, int> bij1;
      gum::Bijection<int, int> bij2;
      gum::Bijection<int, int> bij3;
      gum::Bijection<int, int> bij4;

      TS_GUM_ASSERT_THROWS_NOTHING(bij1.insert(1, 2));
      TS_GUM_ASSERT_THROWS_NOTHING(bij1.insert(3, 4));
      TS_GUM_ASSERT_THROWS_NOTHING(bij1.insert(5, 6));

      TS_GUM_ASSERT_THROWS_NOTHING(bij2.insert(7, 8));
      TS_GUM_ASSERT_THROWS_NOTHING(bij2.insert(9, 10));

      TS_GUM_ASSERT_THROWS_NOTHING(bij3.insert(1, 2));

      bij4 = std::move(bij3);
      bij3 = std::move(bij2);
      bij2 = std::move(bij1);

      TS_ASSERT(bij2.size() == 3);
      TS_ASSERT(bij2.second(3) == 4);
    }

    void testAccess1() {
      gum::Bijection<int, int> bijection;

      int x1 = 1, x2 = 2;
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(x1, x2));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(3, 4));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(5, 6));

      TS_ASSERT(bijection.first(2) == 1);
      TS_ASSERT(bijection.first(4) == 3);
      TS_ASSERT(bijection.first(6) == 5);
      TS_ASSERT(bijection.second(1) == 2);
      TS_ASSERT(bijection.second(3) == 4);
      TS_ASSERT(bijection.second(5) == 6);
      TS_ASSERT(bijection.existsFirst(1));
      TS_ASSERT(bijection.existsFirst(3));
      TS_ASSERT(bijection.existsFirst(5));
      TS_ASSERT(bijection.existsSecond(2));
      TS_ASSERT(bijection.existsSecond(4));
      TS_ASSERT(bijection.existsSecond(6));
      TS_ASSERT(bijection.size() == 3);
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseFirst(1));
      TS_ASSERT(!bijection.existsSecond(2));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseFirst(3));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseSecond(6));
      TS_ASSERT(bijection.empty());

      bijection.insert(1, 2);
      bijection.insert(3, 4);
      bijection.insert(5, 6);

      TS_ASSERT(!bijection.empty());

      bijection.clear();
      TS_ASSERT(bijection.empty());
    }

    void testAccess2() {
      gum::Bijection<std::string, std::string> bijection;

      std::string x1 = "1", x2 = "2";
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(x1, x2));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("3", "4"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("5", "6"));

      TS_ASSERT(bijection.first("2") == "1");
      TS_ASSERT(bijection.first("4") == "3");
      TS_ASSERT(bijection.first("6") == "5");
      TS_ASSERT(bijection.second("1") == "2");
      TS_ASSERT(bijection.second("3") == "4");
      TS_ASSERT(bijection.second("5") == "6");
      TS_ASSERT(bijection.existsFirst("1"));
      TS_ASSERT(bijection.existsFirst("3"));
      TS_ASSERT(bijection.existsFirst("5"));
      TS_ASSERT(bijection.existsSecond("2"));
      TS_ASSERT(bijection.existsSecond("4"));
      TS_ASSERT(bijection.existsSecond("6"));
      TS_ASSERT(bijection.size() == 3);
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseFirst("1"));
      TS_ASSERT(!bijection.existsSecond("2"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseFirst("3"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseSecond("6"));
      TS_ASSERT(bijection.empty());

      bijection.insert("1", "2");
      bijection.insert("3", "4");
      bijection.insert("5", "6");

      TS_ASSERT(!bijection.empty());

      bijection.clear();
      TS_ASSERT(bijection.empty());
    }

    void testEmplace1() {
      gum::Bijection<std::string, std::string> bijection;

      TS_GUM_ASSERT_THROWS_NOTHING(bijection.emplace("3", "4"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.emplace("5", "6"));
      TS_ASSERT(bijection.existsFirst("3"));
    }

    void testEmplace2() {
      gum::Bijection<int, int> bijection;

      TS_GUM_ASSERT_THROWS_NOTHING(bijection.emplace(3, 4));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.emplace(5, 6));
      TS_ASSERT(bijection.existsFirst(3));
    }

    void test_ph() {
      gum::Bijection<int, int> carre;
      carre.insert(1, 1);
      carre.insert(2, 4);
      carre.insert(3, 9);

      try {
        carre.insert(4, 1);
      } catch (gum::DuplicateElement &) {
      }

      TS_ASSERT(!carre.existsFirst(4));
    }

    void testResize() {
      gum::Bijection<std::string, std::string> bijection(2);

      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("1", "2"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("3", "4"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("5", "6"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("6", "3"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("7", "5"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("8", "9"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("9", "7"));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert("2", "1"));

      bijection.resize(2);

      TS_ASSERT(bijection.capacity() != 2);
    }

    void testResize2() {
      gum::Bijection<int, int> bijection(2);

      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(1, 2));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(3, 4));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(5, 6));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(6, 3));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(7, 5));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(8, 9));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(9, 7));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(2, 1));

      bijection.resize(2);

      TS_ASSERT(bijection.capacity() != 2);
    }

    void testIterators1() {
      gum::Bijection<int, int> bijection;

      bijection.insert(1, 2);
      bijection.insert(3, 4);
      bijection.insert(5, 6);
      bijection.insert(6, 3);
      bijection.insert(7, 5);
      bijection.insert(8, 9);
      bijection.insert(9, 7);
      bijection.insert(2, 1);

      auto iter1 = bijection.begin();
      auto iter2 = bijection.end();

      unsigned int nb = 0;

      for (auto iter = bijection.begin(); iter != bijection.end(); ++iter, ++nb) {
      }

      TS_ASSERT(nb == 8);

      bijection.eraseFirst(1);
      bijection.eraseFirst(5);
      bijection.eraseFirst(6);
      bijection.eraseFirst(9);

      nb = 0;

      for (; iter1 != iter2; ++iter1, ++nb) {
      }

      TS_ASSERT(nb == 4);

      nb = 0;
      auto iter = iter2;

      for (iter = bijection.begin(); iter != iter2; ++iter, ++nb) {
      }

      TS_ASSERT(nb == 4);

      iter = bijection.begin();
      nb = iter.first();
      nb = iter.second();
    }

    void testIterators2() {
      gum::Bijection<int, int> bijection;

      bijection.insert(1, 2);
      bijection.insert(3, 4);
      bijection.insert(5, 6);
      bijection.insert(6, 3);
      bijection.insert(7, 5);
      bijection.insert(8, 9);
      bijection.insert(9, 7);
      bijection.insert(2, 1);

      gum::Bijection<int, int>::iterator iter1 = bijection.begin();
      gum::Bijection<int, int>::iterator iter2 = bijection.end();

      unsigned int nb = 0;

      for (gum::Bijection<int, int>::iterator iter = bijection.begin();
           iter != bijection.end(); ++iter, ++nb) {
      }

      TS_ASSERT(nb == 8);

      bijection.eraseFirst(1);
      bijection.eraseFirst(5);
      bijection.eraseFirst(6);
      bijection.eraseFirst(9);

      nb = 0;

      for (; iter1 != iter2; ++iter1, ++nb) {
      }

      TS_ASSERT(nb == 4);

      nb = 0;
      gum::Bijection<int, int>::iterator iter = iter2;

      for (iter = bijection.begin(); iter != iter2; ++iter, ++nb) {
      }

      TS_ASSERT(nb == 4);

      iter = bijection.begin();
      nb = iter.first();
      nb = iter.second();
    }

    void testIterators3() {
      gum::Bijection<std::string, std::string> bijection;

      bijection.insert("1", "2");
      bijection.insert("3", "4");
      bijection.insert("5", "6");
      bijection.insert("6", "3");
      bijection.insert("7", "5");
      bijection.insert("8", "9");
      bijection.insert("9", "7");
      bijection.insert("2", "1");

      auto iter1 = bijection.begin();
      auto iter2 = bijection.end();

      unsigned int nb = 0;

      for (auto iter = bijection.begin(); iter != bijection.end(); ++iter, ++nb) {
      }

      TS_ASSERT(nb == 8);

      bijection.eraseFirst("1");
      bijection.eraseFirst("5");
      bijection.eraseFirst("6");
      bijection.eraseFirst("9");

      nb = 0;

      for (; iter1 != iter2; ++iter1, ++nb) {
      }

      TS_ASSERT(nb == 4);

      nb = 0;
      auto iter = iter2;

      for (iter = bijection.begin(); iter != iter2; ++iter, ++nb) {
      }

      TS_ASSERT(nb == 4);

      iter = bijection.begin();
      std::string str = iter.first();
      str = iter.second();
    }

    void testIterators4() {
      gum::Bijection<std::string, std::string> bijection;

      bijection.insert("1", "2");
      bijection.insert("3", "4");
      bijection.insert("5", "6");
      bijection.insert("6", "3");
      bijection.insert("7", "5");
      bijection.insert("8", "9");
      bijection.insert("9", "7");
      bijection.insert("2", "1");

      gum::Bijection<std::string, std::string>::iterator iter1 = bijection.begin();
      gum::Bijection<std::string, std::string>::iterator iter2 = bijection.end();

      unsigned int nb = 0;

      for (gum::Bijection<std::string, std::string>::iterator
               iter = bijection.begin();
           iter != bijection.end(); ++iter, ++nb) {
      }

      TS_ASSERT(nb == 8);

      bijection.eraseFirst("1");
      bijection.eraseFirst("5");
      bijection.eraseFirst("6");
      bijection.eraseFirst("9");

      nb = 0;

      for (; iter1 != iter2; ++iter1, ++nb) {
      }

      TS_ASSERT(nb == 4);

      nb = 0;
      gum::Bijection<std::string, std::string>::iterator iter = iter2;

      for (iter = bijection.begin(); iter != iter2; ++iter, ++nb) {
      }

      TS_ASSERT(nb == 4);

      iter = bijection.begin();
      std::string str = iter.first();
      str = iter.second();
    }

    void test_constructorsStar() {
      gum::Bijection<int *, int *> bijection;

      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)1, (int *)2));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)3, (int *)4));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)5, (int *)6));

      TS_ASSERT_THROWS(bijection.insert((int *)5, (int *)6), gum::DuplicateElement);
      TS_ASSERT_THROWS(bijection.insert((int *)5, (int *)7), gum::DuplicateElement);
      TS_ASSERT_THROWS(bijection.insert((int *)7, (int *)6), gum::DuplicateElement);

      gum::Bijection<int *, int *> bijection2 = bijection;
      TS_ASSERT(bijection2.size() == 3);

      gum::Bijection<int *, int *> *bijection3 = new gum::Bijection<int *, int *>;
      bijection3->insert((int *)1, (int *)2);
      bijection3->insert((int *)3, (int *)3);

      gum::Bijection<int *, int *> bijection4 = bijection;
      bijection4 = *bijection3;
      delete bijection3;
      TS_ASSERT(bijection4.first((int *)2) == (int *)1);
      TS_ASSERT(bijection4.first((int *)3) == (int *)3);
      TS_ASSERT(bijection4.second((int *)1) == (int *)2);
      TS_ASSERT(bijection4.second((int *)3) == (int *)3);
    }

    void testAccessStar() {
      gum::Bijection<int *, int *> bijection;

      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)1, (int *)2));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)3, (int *)4));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)5, (int *)6));

      TS_ASSERT(bijection.first((int *)2) == (int *)1);
      TS_ASSERT(bijection.first((int *)4) == (int *)3);
      TS_ASSERT(bijection.first((int *)6) == (int *)5);
      TS_ASSERT(bijection.second((int *)1) == (int *)2);
      TS_ASSERT(bijection.second((int *)3) == (int *)4);
      TS_ASSERT(bijection.second((int *)5) == (int *)6);
      TS_ASSERT(bijection.existsFirst((int *)1));
      TS_ASSERT(bijection.existsFirst((int *)3));
      TS_ASSERT(bijection.existsFirst((int *)5));
      TS_ASSERT(bijection.existsSecond((int *)2));
      TS_ASSERT(bijection.existsSecond((int *)4));
      TS_ASSERT(bijection.existsSecond((int *)6));
      TS_ASSERT(bijection.size() == 3);
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseFirst((int *)1));
      TS_ASSERT(!bijection.existsSecond((int *)2));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseFirst((int *)3));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseSecond((int *)6));
      TS_ASSERT(bijection.empty());

      bijection.insert((int *)1, (int *)2);
      bijection.insert((int *)3, (int *)4);
      bijection.insert((int *)5, (int *)6);

      TS_ASSERT(!bijection.empty());

      bijection.clear();
      TS_ASSERT(bijection.empty());
    }

    void test_phStar() {
      gum::Bijection<int *, int *> carre;
      carre.insert((int *)1, (int *)1);
      carre.insert((int *)2, (int *)4);
      carre.insert((int *)3, (int *)9);

      try {
        carre.insert((int *)4, (int *)1);
      } catch (gum::DuplicateElement &) {
      }

      TS_ASSERT(!carre.existsFirst((int *)4));
    }

    void testResizeStar() {
      gum::Bijection<int *, int *> bijection(2);

      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)1, (int *)2));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)3, (int *)4));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)5, (int *)6));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)6, (int *)3));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)7, (int *)5));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)8, (int *)9));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)9, (int *)7));
      TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert((int *)2, (int *)1));

      bijection.resize(2);

      TS_ASSERT(bijection.capacity() != 2);
    }

    void testIteratorsStar() {
      gum::Bijection<int *, int *> bijection;

      bijection.insert((int *)1, (int *)2);
      bijection.insert((int *)3, (int *)4);
      bijection.insert((int *)5, (int *)6);
      bijection.insert((int *)6, (int *)3);
      bijection.insert((int *)7, (int *)5);
      bijection.insert((int *)8, (int *)9);
      bijection.insert((int *)9, (int *)7);
      bijection.insert((int *)2, (int *)1);

      auto iter1 = bijection.begin();
      auto iter2 = bijection.end();

      unsigned int nb = 0;

      for (auto iter = bijection.begin(); iter != bijection.end(); ++iter, ++nb) {
      }

      TS_ASSERT(nb == 8);

      bijection.eraseFirst((int *)1);
      bijection.eraseFirst((int *)5);
      bijection.eraseFirst((int *)6);
      bijection.eraseFirst((int *)9);

      nb = 0;

      for (; iter1 != iter2; ++iter1, ++nb) {
      }

      TS_ASSERT(nb == 4);

      nb = 0;
      auto iter = iter2;

      for (iter = bijection.begin(); iter != iter2; ++iter, ++nb) {
      }

      TS_ASSERT(nb == 4);

      iter = bijection.begin();
      iter.first();
      iter.second();
    }

    void testcopyConstructor() {
      gum::Bijection<gum::NodeId, const gum::LabelizedVariable *> b;
      b.insert(1, new gum::LabelizedVariable("A"));
      b.insert(2, new gum::LabelizedVariable("B"));
      b.insert(3, new gum::LabelizedVariable("C"));
      b.insert(4, new gum::LabelizedVariable("D"));
      b.insert(5, new gum::LabelizedVariable("E"));

      gum::Bijection<gum::NodeId, const gum::LabelizedVariable *> copy(b);

      for (auto iter = b.begin(); iter != b.end(); ++iter) {
        delete iter.second();
      }
    }
  };
}
