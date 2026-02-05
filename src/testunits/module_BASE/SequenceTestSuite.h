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


#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/sequence.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Sequence
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {
  struct SequenceTestSuite {
    private:
    static void fill(gum::Sequence< int >& seq) { seq << 1 << 3 << 5 << 2 << 4; }

    public:
    static void testConstructor1() {
      gum::Sequence< int >* p_seq = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p_seq = new gum::Sequence< int >());

      GUM_CHECK_ASSERT_THROWS_NOTHING(if (p_seq) delete (p_seq));

      gum::Sequence< int > seq;
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << 1);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << 3);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << 5);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << 2);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << 4);

      CHECK((seq.size()) == (static_cast< gum::Size >(5)));

      CHECK(!seq.empty());

      gum::Sequence< int > seq2{1, 2, 3};
      CHECK((seq2.size()) == (static_cast< gum::Size >(3)));

      gum::Sequence< int > seq3(seq2);
      CHECK((seq3) == (seq2));

      gum::Sequence< int > seq4(std::move(seq2));
      CHECK((seq4.size()) == (static_cast< gum::Size >(3)));

      gum::Sequence< int > seq5{2, 4};
      gum::Sequence< int > seq6(seq5);
      CHECK((seq5) == (seq6));

      seq5 = seq;
      CHECK((seq5.size()) == (static_cast< gum::Size >(5)));
      seq6 = seq;
      CHECK((seq6.size()) == (static_cast< gum::Size >(5)));
      seq6 = std::move(seq3);
      CHECK((seq6.size()) == (static_cast< gum::Size >(3)));
    }

    static void testConstructor2() {
      gum::Sequence< std::string >* p_seq = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(p_seq = new gum::Sequence< std::string >());

      GUM_CHECK_ASSERT_THROWS_NOTHING(if (p_seq) delete (p_seq));

      std::string p[5];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";

      gum::Sequence< std::string > seq;
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << p[0]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << p[2]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << p[4]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << p[1]);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << p[3]);

      CHECK((seq.size()) == (static_cast< gum::Size >(5)));

      CHECK(!seq.empty());

      gum::Sequence< std::string > seq2{p[0], p[1], p[2]};
      CHECK((seq2.size()) == (static_cast< gum::Size >(3)));

      gum::Sequence< std::string > seq3(seq2);
      CHECK((seq3) == (seq2));

      gum::Sequence< std::string > seq4(std::move(seq2));
      CHECK((seq4.size()) == (static_cast< gum::Size >(3)));

      gum::Sequence< std::string > seq5{p[1], p[3]};
      gum::Sequence< std::string > seq6(seq5);
      CHECK((seq5) == (seq6));

      seq5 = seq;
      CHECK((seq5.size()) == (static_cast< gum::Size >(5)));
      seq6 = seq;
      CHECK((seq6.size()) == (static_cast< gum::Size >(5)));
      seq6 = std::move(seq3);
      CHECK((seq6.size()) == (static_cast< gum::Size >(3)));
    }

    static void testMoves() {
      gum::Sequence< int > seq1{1, 2, 3};
      gum::Sequence< int > seq2{4, 5};
      gum::Sequence< int > seq3{7, 8, 9, 6};

      gum::Sequence< int > seq4 = std::move(seq3);
      seq3                      = std::move(seq2);
      seq2                      = std::move(seq1);

      CHECK((seq2.size()) == (static_cast< gum::Size >(3)));
      CHECK((seq2.atPos(1)) == (2));
    }

    static void testCopy() {
      gum::Sequence< int > seq;
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << 1);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << 3);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << 5);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << 2);
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << 4);

      gum::Sequence< int >* p_seq2 = nullptr;
      CHECK((seq.toString()) == ("[0:1 - 1:3 - 2:5 - 3:2 - 4:4]"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(p_seq2 = new gum::Sequence< int >(seq));
      CHECK((p_seq2->toString()) == ("[0:1 - 1:3 - 2:5 - 3:2 - 4:4]"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(if (p_seq2) delete (p_seq2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gum::Sequence< int > seq2(seq);
          if (seq2.toString() != "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]") throw(gum::Exception("oups")););

      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gum::Sequence< int > seq3 = seq;
          if (seq3.toString() != "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]") throw(gum::Exception("oups")););
    }

    static void testEquality() {
      gum::Sequence< int > seq1, seq2;
      fill(seq1);
      fill(seq2);
      CHECK((seq1) == (seq2));

      gum::Sequence< int > seq3;
      CHECK((seq1) != (seq3));
      seq3 << 1;
      CHECK((seq1) != (seq3));
      seq3 << 2 << 3 << 4 << 5;
      CHECK((seq1) != (seq3));

      gum::Sequence< int > seq4;
      CHECK((seq1) != (seq4));
      seq4 << 1;
      CHECK((seq1) != (seq4));
      seq4 << 2 << 3 << 4 << 5;
      CHECK((seq1) != (seq4));
    }

    static void testEquality2() {
      std::vector< std::string > p(5);
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";

      gum::Sequence< std::string > seq1, seq2;
      seq1 << p[0] << p[1];
      seq2 << p[0] << p[1];
      CHECK((seq1) == (seq2));

      gum::Sequence< std::string > seq3;
      CHECK((seq1) != (seq3));
      seq3 << p[0];
      CHECK((seq1) != (seq3));
      seq3 << p[1] << p[2] << p[3] << p[4];
      CHECK((seq1) != (seq3));

      gum::Sequence< std::string > seq4;
      CHECK((seq1) != (seq4));
      seq4 << p[0] << p[1];
      CHECK((seq1) == (seq4));
      seq4 << p[2] << p[3] << p[4];
      CHECK((seq1) != (seq4));
    }

    static void testGettersAndSetters() {
      gum::Sequence< std::string > seq;

      GUM_CHECK_ASSERT_THROWS_NOTHING(seq << "aaaa"
                                          << "b"
                                          << "cc");
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq.insert("ddd"));

      CHECK_THROWS_AS(seq.insert("ddd"), const gum::DuplicateElement&);
      CHECK_THROWS_AS(seq << "ddd", const gum::DuplicateElement&);

      CHECK((seq.toString()) == ("[0:aaaa - 1:b - 2:cc - 3:ddd]"));

      CHECK((seq[1]) == ("b"));
      CHECK((seq.pos("ddd")) == (static_cast< gum::Idx >(3)));

      CHECK(seq.exists("b"));
      CHECK(!seq.exists("bb"));

      CHECK_THROWS_AS(seq[15], const gum::OutOfBounds&);
      CHECK_THROWS_AS(seq.pos("aaaaaaa"), const gum::NotFound&);

      CHECK((seq.toString()) == ("[0:aaaa - 1:b - 2:cc - 3:ddd]"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq.setAtPos(1, "boom"));
      CHECK((seq.toString()) == ("[0:aaaa - 1:boom - 2:cc - 3:ddd]"));
      CHECK_THROWS_AS(seq.setAtPos(15, "crach"), const gum::NotFound&);
      CHECK_THROWS_AS(seq.setAtPos(1, "aaaa"), const gum::DuplicateElement&);

      CHECK((seq.toString()) == ("[0:aaaa - 1:boom - 2:cc - 3:ddd]"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(seq.swap(0, 2));
      CHECK((seq.toString()) == ("[0:cc - 1:boom - 2:aaaa - 3:ddd]"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq.swap(0, 3));
      CHECK((seq.toString()) == ("[0:ddd - 1:boom - 2:aaaa - 3:cc]"));
      CHECK_THROWS_AS(seq.swap(1, 10), const gum::OutOfBounds&);
      CHECK_THROWS_AS(seq.swap(10, 1), const gum::OutOfBounds&);

      CHECK((seq.toString()) == ("[0:ddd - 1:boom - 2:aaaa - 3:cc]"));
    }

    static void testRemove() {
      gum::Sequence< int > seq;
      fill(seq);

      CHECK((seq.toString()) == ("[0:1 - 1:3 - 2:5 - 3:2 - 4:4]"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq >> 2);
      CHECK((seq.toString()) == ("[0:1 - 1:3 - 2:5 - 3:4]"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq.erase(1));
      CHECK((seq.toString()) == ("[0:3 - 1:5 - 2:4]"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq >> 4);
      CHECK((seq.toString()) == ("[0:3 - 1:5]"));
      GUM_CHECK_ASSERT_THROWS_NOTHING(seq >> 4);
      CHECK((seq.toString()) == ("[0:3 - 1:5]"));

      seq.clear();
      CHECK(seq.empty());
      CHECK((seq.size()) == (static_cast< gum::Size >(0)));
      CHECK((seq.toString()) == ("[]"));
    }

    static void testIterators() {
      gum::Sequence< int > seq;

      int n;

      n = 0;

      for (auto it = seq.begin(); it != seq.end(); ++it)
        n++;

      CHECK((n) == (0));

      n = 0;

      for (auto it = seq.rbegin(); it != seq.rend(); --it)
        n++;

      CHECK((n) == (0));

      n = 0;

      for (const auto i: seq) {
        GUM_UNUSED(i);
        n++;
      }

      CHECK((n) == (0));

      seq << 1 << 3 << 5 << 2 << 4;

      n = 0;

      for (auto it = seq.begin(); it != seq.end(); ++it) {
        n *= 10;
        n += *it;
      }

      CHECK((n) == (13524));

      n = 0;

      for (const auto i: seq) {
        n *= 10;
        n += i;
      }

      CHECK((n) == (13524));

      n = 0;

      for (gum::Sequence< int >::iterator it = seq.rbegin(); it != seq.rend(); --it) {
        n *= 10;
        n += *it;
      }

      CHECK((n) == (42531));
    }

    static void testIterators2() {
      gum::Sequence< int > seq;

      int n;
      n = 0;

      for (gum::Sequence< int >::iterator it = seq.begin(); it != seq.end(); ++it)
        n++;

      CHECK((n) == (0));

      n = 0;

      for (gum::Sequence< int >::iterator it = seq.rbegin(); it != seq.rend(); --it)
        n++;

      CHECK((n) == (0));

      seq << 1 << 3 << 5 << 2 << 4;
      n = 0;

      for (gum::Sequence< int >::iterator it = seq.begin(); it != seq.end(); ++it) {
        n *= 10;
        n += *it;
      }

      CHECK((n) == (13524));

      n = 0;

      for (gum::Sequence< int >::iterator it = seq.rbegin(); it != seq.rend(); --it) {
        n *= 10;
        n += *it;
      }

      CHECK((n) == (42531));
    }

    static void testIterators3() {
      std::string p[6];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";
      p[5] = "ff";

      gum::Sequence< std::string > seq;

      int n = 0;

      for (const auto& str: seq) {
        GUM_UNUSED(str.size());
        n++;
      }

      CHECK((n) == (0));

      n = 0;

      for (gum::Sequence< std::string >::iterator_safe it
           = seq.rbeginSafe();   // safe iterator needed here
           it != seq.rendSafe();
           --it)
        n++;

      CHECK((n) == (0));
      n = 0;

      for (gum::Sequence< std::string >::iterator_safe it = seq.rbegin(); it != seq.rend(); --it)
        n++;

      CHECK((n) == (0));
      n = 0;

      for (gum::Sequence< std::string >::iterator_safe it
           = seq.beginSafe();   // safe iterator needed here
           it != seq.endSafe();
           ++it)
        n++;

      CHECK((n) == (0));
      n = 0;

      for (gum::Sequence< std::string >::iterator_safe it = seq.begin(); it != seq.end(); ++it)
        n++;

      CHECK((n) == (0));
      n = 0;

      for (const auto& i: seq) {
        GUM_UNUSED(i.size());
        n++;
      }

      CHECK((n) == (0));

      seq << p[1] << p[3] << p[5] << p[2] << p[4];

      n = 0;
      std::string str;

      for (const auto& s: seq)
        str += s;

      CHECK((str) == ("bbddffccee"));

      str = "";

      for (gum::Sequence< std::string >::iterator_safe it = seq.rbegin(); it != seq.rend(); --it) {
        str += *it;
      }

      CHECK((str) == ("eeccffddbb"));
    }

    static void testIterators4() {
      std::string p[6];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";
      p[5] = "ff";

      gum::Sequence< std::string > seq;

      int n = 0;

      for (gum::Sequence< std::string >::iterator it = seq.begin(); it != seq.end(); ++it)
        n++;

      CHECK((n) == (0));
      n = 0;

      for (gum::Sequence< std::string >::iterator it = seq.rbegin(); it != seq.rend(); --it)
        n++;

      CHECK((n) == (0));

      seq << p[1] << p[3] << p[5] << p[2] << p[4];

      n = 0;
      std::string str;

      for (gum::Sequence< std::string >::iterator it = seq.begin(); it != seq.end(); ++it) {
        str += *it;
      }

      CHECK((str) == ("bbddffccee"));
      str = "";

      for (gum::Sequence< std::string >::iterator it = seq.rbegin(); it != seq.rend(); --it) {
        str += *it;
      }

      CHECK((str) == ("eeccffddbb"));
    }

    static void testInsert1() {
      gum::Sequence< int > seq;

      seq.insert(1);
      CHECK((seq.size()) == (static_cast< gum::Size >(1)));
      int x = 2;
      seq.insert(x);
      CHECK((seq.size()) == (static_cast< gum::Size >(2)));

      seq.clear();
      seq << 1;
      CHECK((seq.size()) == (static_cast< gum::Size >(1)));
      seq << x;
      CHECK((seq.size()) == (static_cast< gum::Size >(2)));

      seq.erase(1);
      CHECK((seq.size()) == (static_cast< gum::Size >(1)));
      seq.erase(seq.beginSafe());   // safe iterator needed here
      CHECK((seq.size()) == (static_cast< gum::Size >(0)));
    }

    static void testInsert2() {
      gum::Sequence< std::string > seq;
      std::string                  x = "1";

      seq.insert(x);
      CHECK((seq.size()) == (static_cast< gum::Size >(1)));
      seq.insert("2");
      CHECK((seq.size()) == (static_cast< gum::Size >(2)));

      seq.clear();
      seq << x;
      CHECK((seq.size()) == (static_cast< gum::Size >(1)));
      seq << "2";
      CHECK((seq.size()) == (static_cast< gum::Size >(2)));

      seq.erase(x);
      CHECK((seq.size()) == (static_cast< gum::Size >(1)));
      seq.erase(seq.beginSafe());   // safe iterator needed here
      CHECK((seq.size()) == (static_cast< gum::Size >(0)));
    }

    static void testEmplace1() {
      gum::Sequence< std::pair< int, int > > seq;
      std::pair< int, int >                  x(2, 3);

      seq.emplace(2, 3);
      CHECK((seq.size()) == (static_cast< gum::Size >(1)));
      seq.emplace(5, 7);
      CHECK((seq.size()) == (static_cast< gum::Size >(2)));

      seq.erase(x);
      CHECK((seq.size()) == (static_cast< gum::Size >(1)));
      seq.erase(seq.beginSafe());   // safe iterator needed here
      CHECK((seq.size()) == (static_cast< gum::Size >(0)));
    }

    static void testEmplace2() {
      gum::Sequence< int > seq;
      int                  x = 2;

      seq.emplace(2);
      CHECK((seq.size()) == (static_cast< gum::Size >(1)));
      seq.emplace(5);
      CHECK((seq.size()) == (static_cast< gum::Size >(2)));

      seq.erase(x);
      CHECK((seq.size()) == (static_cast< gum::Size >(1)));
      seq.erase(seq.beginSafe());   // safe iterator needed here
      CHECK((seq.size()) == (static_cast< gum::Size >(0)));
    }

    static void testNewIterOp1() {
      gum::Sequence< int > seq;
      seq << 1 << 2 << 3 << 4 << 5;
      gum::SequenceIteratorSafe< int > iter = seq.begin();
      CHECK((*iter) == (1));
      ++iter;
      CHECK((*iter) == (2));
      iter += 2;
      CHECK((*iter) == (4));
      iter -= 3;
      CHECK((*iter) == (1));
      iter += 3;
      CHECK((*(iter - 2)) == (2));
      CHECK((*(iter + 1)) == (5));
    }

    static void testNewIterOp2() {
      std::string p[6];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";
      p[5] = "ff";

      gum::Sequence< std::string > seq;
      seq << p[1] << p[2] << p[3] << p[4] << p[5];
      gum::SequenceIteratorSafe< std::string > iter = seq.begin();
      CHECK((*iter) == (p[1]));
      ++iter;
      CHECK((*iter) == (p[2]));
      iter += 2;
      CHECK((*iter) == (p[4]));
      iter -= 3;
      CHECK((*iter) == (p[1]));
      iter += 3;
      CHECK((*(iter - 2)) == (p[2]));
      CHECK((*(iter + 1)) == (p[5]));
    }

    static void testNewIterOp3() {
      gum::Sequence< int > seq;
      seq << 1 << 2 << 3 << 4 << 5;
      gum::SequenceIterator< int > iter = seq.begin();
      CHECK((*iter) == (1));
      ++iter;
      CHECK((*iter) == (2));
      iter += 2;
      CHECK((*iter) == (4));
      iter -= 3;
      CHECK((*iter) == (1));
      iter += 3;
      CHECK((*(iter - 2)) == (2));
      CHECK((*(iter + 1)) == (5));
    }

    static void testNewIterOp4() {
      std::string p[6];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";
      p[5] = "ff";

      gum::Sequence< std::string > seq;
      seq << p[1] << p[2] << p[3] << p[4] << p[5];
      gum::SequenceIterator< std::string > iter = seq.begin();
      CHECK((*iter) == (p[1]));
      ++iter;
      CHECK((*iter) == (p[2]));
      iter += 2;
      CHECK((*iter) == (p[4]));
      iter -= 3;
      CHECK((*iter) == (p[1]));
      iter += 3;
      CHECK((*(iter - 2)) == (p[2]));
      CHECK((*(iter + 1)) == (p[5]));
    }

    static void testIdxSeq() {
      gum::Sequence< gum::Idx > seq;

      seq << 1 << 3 << 5 << 2 << 4;
      gum::Idx n = 0;

      for (auto it = seq.rbeginSafe(); it != seq.rendSafe(); --it) {   // safe iterator needed here
        n *= 10;
        n += *it;
      }

      CHECK((n) == (static_cast< gum::Size >(42531)));
      n = 0;

      for (auto it = seq.rbegin(); it != seq.rend(); --it) {
        n *= 10;
        n += *it;
      }

      CHECK((n) == (static_cast< gum::Size >(42531)));
      n = 0;

      for (auto it = seq.beginSafe(); it != seq.endSafe(); ++it) {   // safe iterator needed here
        n *= 10;
        n += *it;
      }

      CHECK((n) == (static_cast< gum::Size >(13524)));
      n = 0;

      for (auto it = seq.begin(); it != seq.end(); ++it) {
        n *= 10;
        n += *it;
      }

      CHECK((n) == (static_cast< gum::Size >(13524)));
      n = 0;

      for (const auto i: seq) {
        n *= 10;
        n += i;
      }

      CHECK((n) == (static_cast< gum::Size >(13524)));

      gum::Sequence< gum::Idx > seq2;
      CHECK_THROWS_AS(seq2.front(), const gum::NotFound&);
    }
  };

  GUM_TEST_ACTIF(Constructor1)
  GUM_TEST_ACTIF(Constructor2)
  GUM_TEST_ACTIF(Moves)
  GUM_TEST_ACTIF(Copy)
  GUM_TEST_ACTIF(Equality)
  GUM_TEST_ACTIF(Equality2)
  GUM_TEST_ACTIF(GettersAndSetters)
  GUM_TEST_ACTIF(Remove)
  GUM_TEST_ACTIF(Iterators)
  GUM_TEST_ACTIF(Iterators2)
  GUM_TEST_ACTIF(Iterators3)
  GUM_TEST_ACTIF(Iterators4)
  GUM_TEST_ACTIF(Insert1)
  GUM_TEST_ACTIF(Insert2)
  GUM_TEST_ACTIF(Emplace1)
  GUM_TEST_ACTIF(Emplace2)
  GUM_TEST_ACTIF(NewIterOp1)
  GUM_TEST_ACTIF(NewIterOp2)
  GUM_TEST_ACTIF(NewIterOp3)
  GUM_TEST_ACTIF(NewIterOp4)
  GUM_TEST_ACTIF(IdxSeq)
}   // namespace gum_tests
