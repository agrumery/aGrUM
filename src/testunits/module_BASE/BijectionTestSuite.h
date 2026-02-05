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


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/bijection.h>
#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/variables/labelizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Bijection
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct BijectionTestSuite {
    public:
    static void test_constructors1() {
      gum::Bijection< int, int > bijection;

      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(1, 2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(3, 4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(5, 6));

      CHECK_THROWS_AS(bijection.insert(5, 6), const gum::DuplicateElement&);
      CHECK_THROWS_AS(bijection.insert(5, 7), const gum::DuplicateElement&);
      CHECK_THROWS_AS(bijection.insert(7, 6), const gum::DuplicateElement&);

      gum::Bijection< int, int > bijection2(bijection);
      CHECK((bijection2.size()) == ((gum::Size) static_cast< gum::Size >(3)));

      gum::Bijection< int, int > bij_bis(bijection);
      bij_bis.insert(8, 10);
      CHECK((bij_bis.size()) == ((gum::Size) static_cast< gum::Size >(4)));

      gum::Bijection< int, int >* bijection3 = new gum::Bijection< int, int >;
      bijection3->insert(1, 2);
      bijection3->insert(3, 3);

      gum::Bijection< int, int > bijection4 = bijection;
      bijection4                            = *bijection3;
      delete bijection3;
      CHECK((bijection4.first(2)) == (1));
      CHECK((bijection4.first(3)) == (3));
      CHECK((bijection4.second(1)) == (2));
      CHECK((bijection4.second(3)) == (3));

      gum::Bijection< int, int > bij_ter(std::move(bij_bis));
      CHECK((bij_ter.size()) == ((gum::Size) static_cast< gum::Size >(4)));

      gum::Bijection< int, int > bij5{std::pair< int, int >(3, 4), std::pair< int, int >(5, 6)};
      CHECK((bij5.size()) == ((gum::Size) static_cast< gum::Size >(2)));

      bij5 = bijection;
      CHECK((bij5.size()) == ((gum::Size) static_cast< gum::Size >(3)));
      CHECK(bij5.existsFirst(5));
      CHECK(bij5.existsFirst(3));

      bij5 = bij_ter;
      CHECK((bij5.size()) == ((gum::Size) static_cast< gum::Size >(4)));
      CHECK(bij5.existsFirst(8));

      bij5 = std::move(bijection);
      CHECK((bij5.size()) == (static_cast< gum::Size >(3)));
    }   // namespace gum_tests

    static void test_constructors2() {
      gum::Bijection< std::string, std::string > bijection;

      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("a", "ba"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("b", "bb"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("c", "bc"));

      CHECK_THROWS_AS(bijection.insert("b", "d"), const gum::DuplicateElement&);
      CHECK_THROWS_AS(bijection.insert("b", "bb"), const gum::DuplicateElement&);
      CHECK_THROWS_AS(bijection.insert("e", "bc"), const gum::DuplicateElement&);

      gum::Bijection< std::string, std::string > bijection2(bijection);
      CHECK((bijection2.size()) == (static_cast< gum::Size >(3)));

      gum::Bijection< std::string, std::string > bij_bis(bijection);
      bij_bis.insert("d", "bd");
      CHECK((bij_bis.size()) == (static_cast< gum::Size >(4)));

      gum::Bijection< std::string, std::string >* bijection3
          = new gum::Bijection< std::string, std::string >;
      bijection3->insert("a", "b");
      bijection3->insert("b", "a");

      gum::Bijection< std::string, std::string > bijection4 = bijection;
      bijection4                                            = *bijection3;
      delete bijection3;
      CHECK((bijection4.first("a")) == ("b"));
      CHECK((bijection4.first("b")) == ("a"));
      CHECK((bijection4.second("a")) == ("b"));
      CHECK((bijection4.second("b")) == ("a"));

      gum::Bijection< std::string, std::string > bij_ter(std::move(bij_bis));
      CHECK((bij_ter.size()) == (static_cast< gum::Size >(4)));

      gum::Bijection< std::string, std::string > bij5{
          std::pair< std::string, std::string >("3", "4"),
          std::pair< std::string, std::string >("5", "6")};
      CHECK((bij5.size()) == (static_cast< gum::Size >(2)));

      bij5 = bijection;
      CHECK((bij5.size()) == (static_cast< gum::Size >(3)));
      CHECK(bij5.existsFirst("a"));
      CHECK(bij5.existsFirst("c"));

      bij5 = bij_ter;
      CHECK((bij5.size()) == (static_cast< gum::Size >(4)));
      CHECK(bij5.existsFirst("d"));

      bij5 = std::move(bijection);
      CHECK((bij5.size()) == (static_cast< gum::Size >(3)));
    }

    static void testMoves() {
      gum::Bijection< int, int > bij1;
      gum::Bijection< int, int > bij2;
      gum::Bijection< int, int > bij3;
      gum::Bijection< int, int > bij4;

      GUM_CHECK_ASSERT_THROWS_NOTHING(bij1.insert(1, 2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bij1.insert(3, 4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bij1.insert(5, 6));

      GUM_CHECK_ASSERT_THROWS_NOTHING(bij2.insert(7, 8));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bij2.insert(9, 10));

      GUM_CHECK_ASSERT_THROWS_NOTHING(bij3.insert(1, 2));

      bij4 = std::move(bij3);
      bij3 = std::move(bij2);
      bij2 = std::move(bij1);

      CHECK((bij2.size()) == (static_cast< gum::Size >(3)));
      CHECK((bij2.second(3)) == (4));
    }

    static void testAccess1() {
      gum::Bijection< int, int > bijection;

      int x1 = 1, x2 = 2;
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(x1, x2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(3, 4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(5, 6));

      CHECK((bijection.first(2)) == (1));
      CHECK((bijection.first(4)) == (3));
      CHECK((bijection.first(6)) == (5));
      CHECK((bijection.second(1)) == (2));
      CHECK((bijection.second(3)) == (4));
      CHECK((bijection.second(5)) == (6));
      CHECK(bijection.existsFirst(1));
      CHECK(bijection.existsFirst(3));
      CHECK(bijection.existsFirst(5));
      CHECK(bijection.existsSecond(2));
      CHECK(bijection.existsSecond(4));
      CHECK(bijection.existsSecond(6));
      CHECK((bijection.size()) == (static_cast< gum::Size >(3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.eraseFirst(1));
      CHECK(!bijection.existsSecond(2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.eraseFirst(3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.eraseSecond(6));
      CHECK(bijection.empty());

      bijection.insert(1, 2);
      bijection.insert(3, 4);
      bijection.insert(5, 6);

      CHECK(!bijection.empty());

      bijection.clear();
      CHECK(bijection.empty());
    }

    static void testAccess2() {
      gum::Bijection< std::string, std::string > bijection;

      std::string x1 = "1", x2 = "2";
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(x1, x2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("3", "4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("5", "6"));

      CHECK((bijection.first("2")) == ("1"));
      CHECK((bijection.first("4")) == ("3"));
      CHECK((bijection.first("6")) == ("5"));
      CHECK((bijection.second("1")) == ("2"));
      CHECK((bijection.second("3")) == ("4"));
      CHECK((bijection.second("5")) == ("6"));
      CHECK(bijection.existsFirst("1"));
      CHECK(bijection.existsFirst("3"));
      CHECK(bijection.existsFirst("5"));
      CHECK(bijection.existsSecond("2"));
      CHECK(bijection.existsSecond("4"));
      CHECK(bijection.existsSecond("6"));
      CHECK((bijection.size()) == (static_cast< gum::Size >(3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.eraseFirst("1"));
      CHECK(!bijection.existsSecond("2"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.eraseFirst("3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.eraseSecond("6"));
      CHECK(bijection.empty());

      bijection.insert("1", "2");
      bijection.insert("3", "4");
      bijection.insert("5", "6");

      CHECK(!bijection.empty());

      bijection.clear();
      CHECK(bijection.empty());
    }

    static void testEmplace1() {
      gum::Bijection< std::string, std::string > bijection;

      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.emplace("3", "4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.emplace("5", "6"));
      CHECK(bijection.existsFirst("3"));
    }

    static void testEmplace2() {
      gum::Bijection< int, int > bijection;

      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.emplace(3, 4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.emplace(5, 6));
      CHECK(bijection.existsFirst(3));
    }

    static void test_ph() {
      gum::Bijection< int, int > carre;
      carre.insert(1, 1);
      carre.insert(2, 4);
      carre.insert(3, 9);

      try {
        carre.insert(4, 1);
      } catch (gum::DuplicateElement&) {}

      CHECK(!carre.existsFirst(4));
    }

    static void testResize() {
      gum::Bijection< std::string, std::string > bijection(2);

      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("1", "2"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("3", "4"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("5", "6"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("6", "3"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("7", "5"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("8", "9"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("9", "7"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert("2", "1"));

      bijection.resize(2);

      CHECK((bijection.capacity()) != (static_cast< gum::Size >(2)));
    }

    static void testResize2() {
      gum::Bijection< int, int > bijection(static_cast< gum::Size >(2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(1, 2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(3, 4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(5, 6));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(6, 3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(7, 5));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(8, 9));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(9, 7));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert(2, 1));

      bijection.resize(2);

      CHECK((bijection.capacity()) != (static_cast< gum::Size >(2)));
    }

    static void testIterators1() {
      gum::Bijection< int, int > bijection;

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

      gum::Size nb = 0;

      for (auto iter = bijection.begin(); iter != bijection.end(); ++iter, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(8)));

      bijection.eraseFirst(1);
      bijection.eraseFirst(5);
      bijection.eraseFirst(6);
      bijection.eraseFirst(9);

      nb = 0;

      for (; iter1 != iter2; ++iter1, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(4)));

      nb        = 0;
      auto iter = iter2;

      for (iter = bijection.begin(); iter != iter2; ++iter, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(4)));

      iter = bijection.begin();
      nb   = iter.first();
      nb   = iter.second();
    }

    static void testIterators2() {
      gum::Bijection< int, int > bijection;

      bijection.insert(1, 2);
      bijection.insert(3, 4);
      bijection.insert(5, 6);
      bijection.insert(6, 3);
      bijection.insert(7, 5);
      bijection.insert(8, 9);
      bijection.insert(9, 7);
      bijection.insert(2, 1);

      gum::Bijection< int, int >::iterator iter1 = bijection.begin();
      gum::Bijection< int, int >::iterator iter2 = bijection.end();

      gum::Size nb = 0;

      for (gum::Bijection< int, int >::iterator iter = bijection.begin(); iter != bijection.end();
           ++iter, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(8)));

      bijection.eraseFirst(1);
      bijection.eraseFirst(5);
      bijection.eraseFirst(6);
      bijection.eraseFirst(9);

      nb = 0;

      for (; iter1 != iter2; ++iter1, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(4)));

      nb                                        = 0;
      gum::Bijection< int, int >::iterator iter = iter2;

      for (iter = bijection.begin(); iter != iter2; ++iter, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(4)));

      iter = bijection.begin();
      nb   = iter.first();
      nb   = iter.second();
    }

    static void testIterators3() {
      gum::Bijection< std::string, std::string > bijection;

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

      gum::Size nb = 0;

      for (auto iter = bijection.begin(); iter != bijection.end(); ++iter, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(8)));

      bijection.eraseFirst("1");
      bijection.eraseFirst("5");
      bijection.eraseFirst("6");
      bijection.eraseFirst("9");

      nb = 0;

      for (; iter1 != iter2; ++iter1, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(4)));

      nb        = 0;
      auto iter = iter2;

      for (iter = bijection.begin(); iter != iter2; ++iter, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(4)));

      iter            = bijection.begin();
      std::string str = iter.first();
      str             = iter.second();
    }

    static void testIterators4() {
      gum::Bijection< std::string, std::string > bijection;

      bijection.insert("1", "2");
      bijection.insert("3", "4");
      bijection.insert("5", "6");
      bijection.insert("6", "3");
      bijection.insert("7", "5");
      bijection.insert("8", "9");
      bijection.insert("9", "7");
      bijection.insert("2", "1");

      gum::Bijection< std::string, std::string >::iterator iter1 = bijection.begin();
      gum::Bijection< std::string, std::string >::iterator iter2 = bijection.end();

      gum::Size nb = 0;

      for (gum::Bijection< std::string, std::string >::iterator iter = bijection.begin();
           iter != bijection.end();
           ++iter, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(8)));

      bijection.eraseFirst("1");
      bijection.eraseFirst("5");
      bijection.eraseFirst("6");
      bijection.eraseFirst("9");

      nb = 0;

      for (; iter1 != iter2; ++iter1, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(4)));

      nb                                                        = 0;
      gum::Bijection< std::string, std::string >::iterator iter = iter2;

      for (iter = bijection.begin(); iter != iter2; ++iter, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(4)));

      iter            = bijection.begin();
      std::string str = iter.first();
      str             = iter.second();
    }

    static void test_constructorsStar() {
      gum::Bijection< int*, int* > bijection;

      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)1, (int*)2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)3, (int*)4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)5, (int*)6));

      CHECK_THROWS_AS(bijection.insert((int*)5, (int*)6), const gum::DuplicateElement&);
      CHECK_THROWS_AS(bijection.insert((int*)5, (int*)7), const gum::DuplicateElement&);
      CHECK_THROWS_AS(bijection.insert((int*)7, (int*)6), const gum::DuplicateElement&);

      gum::Bijection< int*, int* > bijection2 = bijection;
      CHECK((bijection2.size()) == (static_cast< gum::Size >(3)));

      gum::Bijection< int*, int* >* bijection3 = new gum::Bijection< int*, int* >;
      bijection3->insert((int*)1, (int*)2);
      bijection3->insert((int*)3, (int*)3);

      gum::Bijection< int*, int* > bijection4 = bijection;
      bijection4                              = *bijection3;
      delete bijection3;
      CHECK((bijection4.first((int*)2)) == ((int*)1));
      CHECK((bijection4.first((int*)3)) == ((int*)3));
      CHECK((bijection4.second((int*)1)) == ((int*)2));
      CHECK((bijection4.second((int*)3)) == ((int*)3));
    }

    static void testAccessStar() {
      gum::Bijection< int*, int* > bijection;

      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)1, (int*)2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)3, (int*)4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)5, (int*)6));

      CHECK((bijection.first((int*)2)) == ((int*)1));
      CHECK((bijection.first((int*)4)) == ((int*)3));
      CHECK((bijection.first((int*)6)) == ((int*)5));
      CHECK((bijection.second((int*)1)) == ((int*)2));
      CHECK((bijection.second((int*)3)) == ((int*)4));
      CHECK((bijection.second((int*)5)) == ((int*)6));
      CHECK(bijection.existsFirst((int*)1));
      CHECK(bijection.existsFirst((int*)3));
      CHECK(bijection.existsFirst((int*)5));
      CHECK(bijection.existsSecond((int*)2));
      CHECK(bijection.existsSecond((int*)4));
      CHECK(bijection.existsSecond((int*)6));
      CHECK((bijection.size()) == (static_cast< gum::Size >(3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.eraseFirst((int*)1));
      CHECK(!bijection.existsSecond((int*)2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.eraseFirst((int*)3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.eraseSecond((int*)6));
      CHECK(bijection.empty());

      bijection.insert((int*)1, (int*)2);
      bijection.insert((int*)3, (int*)4);
      bijection.insert((int*)5, (int*)6);

      CHECK(!bijection.empty());

      bijection.clear();
      CHECK(bijection.empty());
    }

    static void test_phStar() {
      gum::Bijection< int*, int* > carre;
      carre.insert((int*)1, (int*)1);
      carre.insert((int*)2, (int*)4);
      carre.insert((int*)3, (int*)9);

      try {
        carre.insert((int*)4, (int*)1);
      } catch (gum::DuplicateElement&) {}

      CHECK(!carre.existsFirst((int*)4));
    }

    static void testResizeStar() {
      gum::Bijection< int*, int* > bijection(2);

      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)1, (int*)2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)3, (int*)4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)5, (int*)6));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)6, (int*)3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)7, (int*)5));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)8, (int*)9));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)9, (int*)7));
      GUM_CHECK_ASSERT_THROWS_NOTHING(bijection.insert((int*)2, (int*)1));

      bijection.resize(2);

      CHECK((bijection.capacity()) != (static_cast< gum::Size >(2)));
    }

    static void testIteratorsStar() {
      gum::Bijection< int*, int* > bijection;

      bijection.insert((int*)1, (int*)2);
      bijection.insert((int*)3, (int*)4);
      bijection.insert((int*)5, (int*)6);
      bijection.insert((int*)6, (int*)3);
      bijection.insert((int*)7, (int*)5);
      bijection.insert((int*)8, (int*)9);
      bijection.insert((int*)9, (int*)7);
      bijection.insert((int*)2, (int*)1);

      auto iter1 = bijection.begin();
      auto iter2 = bijection.end();

      gum::Size nb = 0;

      for (auto iter = bijection.begin(); iter != bijection.end(); ++iter, ++nb) {}

      CHECK((nb) == ((gum::Size) static_cast< gum::Size >(8)));

      bijection.eraseFirst((int*)1);
      bijection.eraseFirst((int*)5);
      bijection.eraseFirst((int*)6);
      bijection.eraseFirst((int*)9);

      nb = 0;

      for (; iter1 != iter2; ++iter1, ++nb) {}

      CHECK((nb) == (static_cast< gum::Size >(4)));

      nb        = 0;
      auto iter = iter2;

      for (iter = bijection.begin(); iter != iter2; ++iter, ++nb) {}

      CHECK((nb) == (static_cast< gum::Size >(4)));

      iter = bijection.begin();
      iter.first();
      iter.second();
    }

    static void testcopyConstructor() {
      gum::Bijection< gum::NodeId, const gum::LabelizedVariable* > b;
      b.insert(1, new gum::LabelizedVariable("A"));
      b.insert(2, new gum::LabelizedVariable("B"));
      b.insert(3, new gum::LabelizedVariable("C"));
      b.insert(4, new gum::LabelizedVariable("D"));
      b.insert(5, new gum::LabelizedVariable("E"));

      gum::Bijection< gum::NodeId, const gum::LabelizedVariable* > copy(b);

      for (auto iter = b.begin(); iter != b.end(); ++iter) {
        delete iter.second();
      }
    }
  };

  GUM_TEST_ACTIF(_constructors1)
  GUM_TEST_ACTIF(_constructors2)
  GUM_TEST_ACTIF(Moves)
  GUM_TEST_ACTIF(Access1)
  GUM_TEST_ACTIF(Access2)
  GUM_TEST_ACTIF(Emplace1)
  GUM_TEST_ACTIF(Emplace2)
  GUM_TEST_ACTIF(_ph)
  GUM_TEST_ACTIF(Resize)
  GUM_TEST_ACTIF(Resize2)
  GUM_TEST_ACTIF(Iterators1)
  GUM_TEST_ACTIF(Iterators2)
  GUM_TEST_ACTIF(Iterators3)
  GUM_TEST_ACTIF(Iterators4)
  GUM_TEST_ACTIF(_constructorsStar)
  GUM_TEST_ACTIF(AccessStar)
  GUM_TEST_ACTIF(_phStar)
  GUM_TEST_ACTIF(ResizeStar)
  GUM_TEST_ACTIF(IteratorsStar)
  GUM_TEST_ACTIF(copyConstructor)
}   // namespace gum_tests
