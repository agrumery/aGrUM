/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
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
#include <ressources/myalloc.h>

#include <string>
#include <agrum/core/sequence.h>

namespace gum_tests {
  class SequenceTestSuite : public CxxTest::TestSuite {
    private:
    void fill(gum::Sequence<int> &seq) { seq << 1 << 3 << 5 << 2 << 4; }

    public:
    void testConstructor1() {
      gum::Sequence<int> *p_seq = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(p_seq = new gum::Sequence<int>());

      TS_GUM_ASSERT_THROWS_NOTHING(if (p_seq) delete (p_seq));

      gum::Sequence<int> seq;
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 1);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 3);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 5);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 2);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 4);

      TS_ASSERT_EQUALS(seq.size(), (gum::Size)5);

      TS_ASSERT(!seq.empty());

      gum::Sequence<int> seq2{1, 2, 3};
      TS_ASSERT(seq2.size() == 3);

      gum::Sequence<int> seq3(seq2);
      TS_ASSERT(seq3 == seq2);

      gum::Sequence<int> seq4(std::move(seq2));
      TS_ASSERT(seq4.size() == 3);

      gum::Sequence<int, MyAlloc<int>> seq5{2, 4};
      gum::Sequence<int> seq6(seq5);
      TS_ASSERT(seq5 == seq6);

      seq5 = seq;
      TS_ASSERT(seq5.size() == 5);
      seq6 = seq;
      TS_ASSERT(seq6.size() == 5);
      seq6 = std::move(seq3);
      TS_ASSERT(seq6.size() == 3);
    }

    void testConstructor2() {
      gum::Sequence<std::string> *p_seq = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(p_seq = new gum::Sequence<std::string>());

      TS_GUM_ASSERT_THROWS_NOTHING(if (p_seq) delete (p_seq));

      std::string p[5];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";

      gum::Sequence<std::string> seq;
      TS_GUM_ASSERT_THROWS_NOTHING(seq << p[0]);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << p[2]);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << p[4]);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << p[1]);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << p[3]);

      TS_ASSERT_EQUALS(seq.size(), (gum::Size)5);

      TS_ASSERT(!seq.empty());

      gum::Sequence<std::string> seq2{p[0], p[1], p[2]};
      TS_ASSERT(seq2.size() == 3);

      gum::Sequence<std::string> seq3(seq2);
      TS_ASSERT(seq3 == seq2);

      gum::Sequence<std::string> seq4(std::move(seq2));
      TS_ASSERT(seq4.size() == 3);

      gum::Sequence<std::string, MyAlloc<std::string>> seq5{p[1], p[3]};
      gum::Sequence<std::string> seq6(seq5);
      TS_ASSERT(seq5 == seq6);

      seq5 = seq;
      TS_ASSERT(seq5.size() == 5);
      seq6 = seq;
      TS_ASSERT(seq6.size() == 5);
      seq6 = std::move(seq3);
      TS_ASSERT(seq6.size() == 3);
    }

    void testMoves() {
      gum::Sequence<int> seq1{1, 2, 3};
      gum::Sequence<int> seq2{4, 5};
      gum::Sequence<int> seq3{7, 8, 9, 6};

      gum::Sequence<int> seq4 = std::move(seq3);
      seq3 = std::move(seq2);
      seq2 = std::move(seq1);

      TS_ASSERT(seq2.size() == 3);
      TS_ASSERT(seq2.atPos(1) == 2);
    }

    void testCopy() {
      gum::Sequence<int> seq;
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 1);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 3);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 5);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 2);
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 4);

      gum::Sequence<int> *p_seq2 = nullptr;
      TS_ASSERT_EQUALS(seq.toString(), "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]");
      TS_GUM_ASSERT_THROWS_NOTHING(p_seq2 = new gum::Sequence<int>(seq));
      TS_ASSERT_EQUALS(p_seq2->toString(), "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]");

      TS_GUM_ASSERT_THROWS_NOTHING(if (p_seq2) delete (p_seq2));

      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::Sequence<int> seq2(seq);
          if (seq2.toString() !=
              "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]") throw(gum::Exception("oups")););

      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::Sequence<int> seq3 = seq;
          if (seq3.toString() !=
              "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]") throw(gum::Exception("oups")););
    }

    void testEquality() {
      gum::Sequence<int> seq1, seq2;
      fill(seq1);
      fill(seq2);
      TS_ASSERT_EQUALS(seq1, seq2);

      gum::Sequence<int> seq3;
      TS_ASSERT(seq1 != seq3);
      seq3 << 1;
      TS_ASSERT(seq1 != seq3);
      seq3 << 2 << 3 << 4 << 5;
      TS_ASSERT(seq1 != seq3);

      gum::Sequence<int, MyAlloc<int>> seq4;
      TS_ASSERT(seq1 != seq4);
      seq4 << 1;
      TS_ASSERT(seq1 != seq4);
      seq4 << 2 << 3 << 4 << 5;
      TS_ASSERT(seq1 != seq4);
    }

    void testEquality2() {
      std::string p[5];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";

      gum::Sequence<std::string> seq1, seq2;
      seq1 << p[0] << p[1];
      seq2 << p[0] << p[1];
      TS_ASSERT_EQUALS(seq1, seq2);

      gum::Sequence<std::string> seq3;
      TS_ASSERT(seq1 != seq3);
      seq3 << p[0];
      TS_ASSERT(seq1 != seq3);
      seq3 << p[1] << p[2] << p[3] << p[4];
      TS_ASSERT(seq1 != seq3);

      gum::Sequence<std::string, MyAlloc<std::string>> seq4;
      TS_ASSERT(seq1 != seq4);
      seq4 << p[0] << p[1];
      TS_ASSERT(seq1 == seq4);
      seq4 << p[2] << p[3] << p[4];
      TS_ASSERT(seq1 != seq4);
    }

    void testGettersAndSetters() {
      gum::Sequence<std::string> seq;

      TS_GUM_ASSERT_THROWS_NOTHING(seq << "aaaa"
                                       << "b"
                                       << "cc");
      TS_GUM_ASSERT_THROWS_NOTHING(seq.insert("ddd"));

      TS_ASSERT_THROWS(seq.insert("ddd"), gum::DuplicateElement);
      TS_ASSERT_THROWS(seq << "ddd", gum::DuplicateElement);

      TS_ASSERT_EQUALS(seq.toString(), "[0:aaaa - 1:b - 2:cc - 3:ddd]");

      TS_ASSERT_EQUALS(seq[1], "b");
      TS_ASSERT_EQUALS(seq.pos("ddd"), (gum::Idx)3);

      TS_ASSERT(seq.exists("b"));
      TS_ASSERT(!seq.exists("bb"));

      TS_ASSERT_THROWS(seq[15], gum::OutOfBounds);
      TS_ASSERT_THROWS(seq.pos("aaaaaaa"), gum::NotFound);

      TS_ASSERT_EQUALS(seq.toString(), "[0:aaaa - 1:b - 2:cc - 3:ddd]");
      TS_GUM_ASSERT_THROWS_NOTHING(seq.setAtPos(1, "boom"));
      TS_ASSERT_EQUALS(seq.toString(), "[0:aaaa - 1:boom - 2:cc - 3:ddd]");
      TS_ASSERT_THROWS(seq.setAtPos(15, "crach"), gum::NotFound);
      TS_ASSERT_THROWS(seq.setAtPos(1, "aaaa"), gum::DuplicateElement);

      TS_ASSERT_EQUALS(seq.toString(), "[0:aaaa - 1:boom - 2:cc - 3:ddd]");

      TS_GUM_ASSERT_THROWS_NOTHING(seq.swap(0, 2));
      TS_ASSERT_EQUALS(seq.toString(), "[0:cc - 1:boom - 2:aaaa - 3:ddd]");
      TS_GUM_ASSERT_THROWS_NOTHING(seq.swap(0, 3));
      TS_ASSERT_EQUALS(seq.toString(), "[0:ddd - 1:boom - 2:aaaa - 3:cc]");
      TS_ASSERT_THROWS(seq.swap(1, 10), gum::OutOfBounds);
      TS_ASSERT_THROWS(seq.swap(10, 1), gum::OutOfBounds);

      TS_ASSERT_EQUALS(seq.toString(), "[0:ddd - 1:boom - 2:aaaa - 3:cc]");
    }

    void testRemove() {
      gum::Sequence<int> seq;
      fill(seq);

      TS_ASSERT_EQUALS(seq.toString(), "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]");
      TS_GUM_ASSERT_THROWS_NOTHING(seq >> 2);
      TS_ASSERT_EQUALS(seq.toString(), "[0:1 - 1:3 - 2:5 - 3:4]");
      TS_GUM_ASSERT_THROWS_NOTHING(seq.erase(1));
      TS_ASSERT_EQUALS(seq.toString(), "[0:3 - 1:5 - 2:4]");
      TS_GUM_ASSERT_THROWS_NOTHING(seq >> 4);
      TS_ASSERT_EQUALS(seq.toString(), "[0:3 - 1:5]");
      TS_GUM_ASSERT_THROWS_NOTHING(seq >> 4);
      TS_ASSERT_EQUALS(seq.toString(), "[0:3 - 1:5]");

      seq.clear();
      TS_ASSERT(seq.empty());
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)0);
      TS_ASSERT_EQUALS(seq.toString(), "[]");
    }

    void testIterators() {
      gum::Sequence<int> seq;

      int n;

      n = 0;

      for (auto it = seq.begin(); it != seq.end(); ++it)
        n++;

      TS_ASSERT_EQUALS(n, 0);

      n = 0;

      for (auto it = seq.rbegin(); it != seq.rend(); --it)
        n++;

      TS_ASSERT_EQUALS(n, 0);

      n = 0;

      for (const auto i : seq) {
        GUM_UNUSED(i);
        n++;
      }

      TS_ASSERT_EQUALS(n, 0);

      seq << 1 << 3 << 5 << 2 << 4;

      n = 0;

      for (auto it = seq.begin(); it != seq.end(); ++it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 13524);

      n = 0;

      for (const auto i : seq) {
        n *= 10;
        n += i;
      }

      TS_ASSERT_EQUALS(n, 13524);

      n = 0;

      for (gum::Sequence<int>::iterator it = seq.rbegin(); it != seq.rend(); --it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 42531);
    }

    void testIterators2() {
      gum::Sequence<int> seq;

      int n;
      n = 0;

      for (gum::Sequence<int>::iterator it = seq.begin(); it != seq.end(); ++it)
        n++;

      TS_ASSERT_EQUALS(n, 0);

      n = 0;

      for (gum::Sequence<int>::iterator it = seq.rbegin(); it != seq.rend(); --it)
        n++;

      TS_ASSERT_EQUALS(n, 0);

      seq << 1 << 3 << 5 << 2 << 4;
      n = 0;

      for (gum::Sequence<int>::iterator it = seq.begin(); it != seq.end(); ++it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 13524);

      n = 0;

      for (gum::Sequence<int>::iterator it = seq.rbegin(); it != seq.rend(); --it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 42531);
    }

    void testIterators3() {
      std::string p[6];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";
      p[5] = "ff";

      gum::Sequence<std::string, MyAlloc<std::string>> seq;

      int n = 0;

      for (const auto str : seq)
        n++;

      TS_ASSERT_EQUALS(n, 0);

      n = 0;

      for (gum::Sequence<std::string>::iterator_safe it =
               seq.rbeginSafe(); // safe iterator needed here
           it != seq.rendSafe();
           --it)
        n++;

      TS_ASSERT_EQUALS(n, 0);
      n = 0;

      for (gum::Sequence<std::string>::iterator_safe it = seq.rbegin();
           it != seq.rend(); --it)
        n++;

      TS_ASSERT_EQUALS(n, 0);
      n = 0;

      for (gum::Sequence<std::string>::iterator_safe it =
               seq.beginSafe(); // safe iterator needed here
           it != seq.endSafe();
           ++it)
        n++;

      TS_ASSERT_EQUALS(n, 0);
      n = 0;

      for (gum::Sequence<std::string>::iterator_safe it = seq.begin();
           it != seq.end(); ++it)
        n++;

      TS_ASSERT_EQUALS(n, 0);
      n = 0;

      for (const auto i : seq)
        n++;

      TS_ASSERT_EQUALS(n, 0);

      seq << p[1] << p[3] << p[5] << p[2] << p[4];

      n = 0;
      std::string str;

      for (const auto &s : seq)
        str += s;

      TS_ASSERT(str == "bbddffccee");

      str = "";

      for (gum::Sequence<std::string>::iterator_safe it = seq.rbegin();
           it != seq.rend(); --it) {
        str += *it;
      }

      TS_ASSERT(str == "eeccffddbb");
    }

    void testIterators4() {
      std::string p[6];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";
      p[5] = "ff";

      gum::Sequence<std::string, MyAlloc<std::string>> seq;

      int n = 0;

      for (gum::Sequence<std::string>::iterator it = seq.begin(); it != seq.end();
           ++it)
        n++;

      TS_ASSERT_EQUALS(n, 0);
      n = 0;

      for (gum::Sequence<std::string>::iterator it = seq.rbegin(); it != seq.rend();
           --it)
        n++;

      TS_ASSERT_EQUALS(n, 0);

      seq << p[1] << p[3] << p[5] << p[2] << p[4];

      n = 0;
      std::string str;

      for (gum::Sequence<std::string>::iterator it = seq.begin(); it != seq.end();
           ++it) {
        str += *it;
      }

      TS_ASSERT(str == "bbddffccee");
      str = "";

      for (gum::Sequence<std::string>::iterator it = seq.rbegin(); it != seq.rend();
           --it) {
        str += *it;
      }

      TS_ASSERT(str == "eeccffddbb");
    }

    void testInsert1() {
      gum::Sequence<int, MyAlloc<int>> seq;

      seq.insert(1);
      TS_ASSERT(seq.size() == 1);
      int x = 2;
      seq.insert(x);
      TS_ASSERT(seq.size() == 2);

      seq.clear();
      seq << 1;
      TS_ASSERT(seq.size() == 1);
      seq << x;
      TS_ASSERT(seq.size() == 2);

      seq.erase(1);
      TS_ASSERT(seq.size() == 1);
      seq.erase(seq.beginSafe()); // safe iterator needed here
      TS_ASSERT(seq.size() == 0);
    }

    void testInsert2() {
      gum::Sequence<std::string, MyAlloc<std::string>> seq;
      std::string x = "1";

      seq.insert(x);
      TS_ASSERT(seq.size() == 1);
      seq.insert("2");
      TS_ASSERT(seq.size() == 2);

      seq.clear();
      seq << x;
      TS_ASSERT(seq.size() == 1);
      seq << "2";
      TS_ASSERT(seq.size() == 2);

      seq.erase(x);
      TS_ASSERT(seq.size() == 1);
      seq.erase(seq.beginSafe()); // safe iterator needed here
      TS_ASSERT(seq.size() == 0);
    }

    void testEmplace1() {
      gum::Sequence<std::pair<int, int>, MyAlloc<std::pair<int, int>>> seq;
      std::pair<int, int> x(2, 3);

      seq.emplace(2, 3);
      TS_ASSERT(seq.size() == 1);
      seq.emplace(5, 7);
      TS_ASSERT(seq.size() == 2);

      seq.erase(x);
      TS_ASSERT(seq.size() == 1);
      seq.erase(seq.beginSafe()); // safe iterator needed here
      TS_ASSERT(seq.size() == 0);
    }

    void testEmplace2() {
      gum::Sequence<int, MyAlloc<int>> seq;
      int x = 2;

      seq.emplace(2);
      TS_ASSERT(seq.size() == 1);
      seq.emplace(5);
      TS_ASSERT(seq.size() == 2);

      seq.erase(x);
      TS_ASSERT(seq.size() == 1);
      seq.erase(seq.beginSafe()); // safe iterator needed here
      TS_ASSERT(seq.size() == 0);
    }

    void testNewIterOp1() {
      gum::Sequence<int, MyAlloc<int>> seq;
      seq << 1 << 2 << 3 << 4 << 5;
      gum::SequenceIteratorSafe<int> iter = seq.begin();
      TS_ASSERT(*iter == 1);
      ++iter;
      TS_ASSERT(*iter == 2);
      iter += 2;
      TS_ASSERT(*iter == 4);
      iter -= 3;
      TS_ASSERT(*iter == 1);
      iter += 3;
      TS_ASSERT(*(iter - 2) == 2);
      TS_ASSERT(*(iter + 1) == 5);
    }

    void testNewIterOp2() {
      std::string p[6];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";
      p[5] = "ff";

      gum::Sequence<std::string, MyAlloc<std::string>> seq;
      seq << p[1] << p[2] << p[3] << p[4] << p[5];
      gum::SequenceIteratorSafe<std::string> iter = seq.begin();
      TS_ASSERT(*iter == p[1]);
      ++iter;
      TS_ASSERT(*iter == p[2]);
      iter += 2;
      TS_ASSERT(*iter == p[4]);
      iter -= 3;
      TS_ASSERT(*iter == p[1]);
      iter += 3;
      TS_ASSERT(*(iter - 2) == p[2]);
      TS_ASSERT(*(iter + 1) == p[5]);
    }

    void testNewIterOp3() {
      gum::Sequence<int, MyAlloc<int>> seq;
      seq << 1 << 2 << 3 << 4 << 5;
      gum::SequenceIterator<int> iter = seq.begin();
      TS_ASSERT(*iter == 1);
      ++iter;
      TS_ASSERT(*iter == 2);
      iter += 2;
      TS_ASSERT(*iter == 4);
      iter -= 3;
      TS_ASSERT(*iter == 1);
      iter += 3;
      TS_ASSERT(*(iter - 2) == 2);
      TS_ASSERT(*(iter + 1) == 5);
    }

    void testNewIterOp4() {
      std::string p[6];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";
      p[5] = "ff";

      gum::Sequence<std::string, MyAlloc<std::string>> seq;
      seq << p[1] << p[2] << p[3] << p[4] << p[5];
      gum::SequenceIterator<std::string> iter = seq.begin();
      TS_ASSERT(*iter == p[1]);
      ++iter;
      TS_ASSERT(*iter == p[2]);
      iter += 2;
      TS_ASSERT(*iter == p[4]);
      iter -= 3;
      TS_ASSERT(*iter == p[1]);
      iter += 3;
      TS_ASSERT(*(iter - 2) == p[2]);
      TS_ASSERT(*(iter + 1) == p[5]);
    }

    void testIdxSeq() {
      gum::Sequence<gum::Idx> seq;

      seq << 1 << 3 << 5 << 2 << 4;
      int n = 0;

      for (auto it = seq.rbeginSafe(); it != seq.rendSafe();
           --it) { // safe iterator needed here
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 42531);
      n = 0;

      for (auto it = seq.rbegin(); it != seq.rend(); --it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 42531);
      n = 0;

      for (auto it = seq.beginSafe(); it != seq.endSafe();
           ++it) { // safe iterator needed here
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 13524);
      n = 0;

      for (auto it = seq.begin(); it != seq.end(); ++it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 13524);
      n = 0;

      for (const auto i : seq) {
        n *= 10;
        n += i;
      }

      TS_ASSERT_EQUALS(n, 13524);

      gum::Sequence<gum::Idx> seq2;
      TS_ASSERT_THROWS(seq2.front(), gum::NotFound);
    }
  };
}
