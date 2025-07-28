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


#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/sequence.h>

namespace gum_tests {
  class [[maybe_unused]] SequenceTestSuite: public CxxTest::TestSuite {
    private:
    void fill(gum::Sequence< int >& seq) { seq << 1 << 3 << 5 << 2 << 4; }

    public:
    GUM_ACTIVE_TEST(Constructor1) {
      gum::Sequence< int >* p_seq = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(p_seq = new gum::Sequence< int >())

      TS_GUM_ASSERT_THROWS_NOTHING(if (p_seq) delete (p_seq))

      gum::Sequence< int > seq;
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 1)
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 3)
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 5)
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 2)
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 4)

      TS_ASSERT_EQUALS(seq.size(), (gum::Size)5)

      TS_ASSERT(!seq.empty())

      gum::Sequence< int > seq2{1, 2, 3};
      TS_ASSERT_EQUALS(seq2.size(), (gum::Size)3)

      gum::Sequence< int > seq3(seq2);
      TS_ASSERT_EQUALS(seq3, seq2)

      gum::Sequence< int > seq4(std::move(seq2));
      TS_ASSERT_EQUALS(seq4.size(), (gum::Size)3)

      gum::Sequence< int > seq5{2, 4};
      gum::Sequence< int > seq6(seq5);
      TS_ASSERT_EQUALS(seq5, seq6)

      seq5 = seq;
      TS_ASSERT_EQUALS(seq5.size(), (gum::Size)5)
      seq6 = seq;
      TS_ASSERT_EQUALS(seq6.size(), (gum::Size)5)
      seq6 = std::move(seq3);
      TS_ASSERT_EQUALS(seq6.size(), (gum::Size)3)
    }

    GUM_ACTIVE_TEST(Constructor2) {
      gum::Sequence< std::string >* p_seq = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(p_seq = new gum::Sequence< std::string >())

      TS_GUM_ASSERT_THROWS_NOTHING(if (p_seq) delete (p_seq))

      std::string p[5];
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";

      gum::Sequence< std::string > seq;
      TS_GUM_ASSERT_THROWS_NOTHING(seq << p[0])
      TS_GUM_ASSERT_THROWS_NOTHING(seq << p[2])
      TS_GUM_ASSERT_THROWS_NOTHING(seq << p[4])
      TS_GUM_ASSERT_THROWS_NOTHING(seq << p[1])
      TS_GUM_ASSERT_THROWS_NOTHING(seq << p[3])

      TS_ASSERT_EQUALS(seq.size(), (gum::Size)5)

      TS_ASSERT(!seq.empty())

      gum::Sequence< std::string > seq2{p[0], p[1], p[2]};
      TS_ASSERT_EQUALS(seq2.size(), (gum::Size)3)

      gum::Sequence< std::string > seq3(seq2);
      TS_ASSERT_EQUALS(seq3, seq2)

      gum::Sequence< std::string > seq4(std::move(seq2));
      TS_ASSERT_EQUALS(seq4.size(), (gum::Size)3)

      gum::Sequence< std::string > seq5{p[1], p[3]};
      gum::Sequence< std::string > seq6(seq5);
      TS_ASSERT_EQUALS(seq5, seq6)

      seq5 = seq;
      TS_ASSERT_EQUALS(seq5.size(), (gum::Size)5)
      seq6 = seq;
      TS_ASSERT_EQUALS(seq6.size(), (gum::Size)5)
      seq6 = std::move(seq3);
      TS_ASSERT_EQUALS(seq6.size(), (gum::Size)3)
    }

    GUM_ACTIVE_TEST(Moves) {
      gum::Sequence< int > seq1{1, 2, 3};
      gum::Sequence< int > seq2{4, 5};
      gum::Sequence< int > seq3{7, 8, 9, 6};

      gum::Sequence< int > seq4 = std::move(seq3);
      seq3                      = std::move(seq2);
      seq2                      = std::move(seq1);

      TS_ASSERT_EQUALS(seq2.size(), (gum::Size)3)
      TS_ASSERT_EQUALS(seq2.atPos(1), 2)
    }

    GUM_ACTIVE_TEST(Copy) {
      gum::Sequence< int > seq;
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 1)
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 3)
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 5)
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 2)
      TS_GUM_ASSERT_THROWS_NOTHING(seq << 4)

      gum::Sequence< int >* p_seq2 = nullptr;
      TS_ASSERT_EQUALS(seq.toString(), "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]")
      TS_GUM_ASSERT_THROWS_NOTHING(p_seq2 = new gum::Sequence< int >(seq))
      TS_ASSERT_EQUALS(p_seq2->toString(), "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]")

      TS_GUM_ASSERT_THROWS_NOTHING(if (p_seq2) delete (p_seq2))

      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::Sequence< int > seq2(seq);
          if (seq2.toString() != "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]") throw(gum::Exception("oups")););

      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::Sequence< int > seq3 = seq;
          if (seq3.toString() != "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]") throw(gum::Exception("oups")););
    }

    GUM_ACTIVE_TEST(Equality) {
      gum::Sequence< int > seq1, seq2;
      fill(seq1);
      fill(seq2);
      TS_ASSERT_EQUALS(seq1, seq2)

      gum::Sequence< int > seq3;
      TS_ASSERT_DIFFERS(seq1, seq3)
      seq3 << 1;
      TS_ASSERT_DIFFERS(seq1, seq3)
      seq3 << 2 << 3 << 4 << 5;
      TS_ASSERT_DIFFERS(seq1, seq3)

      gum::Sequence< int > seq4;
      TS_ASSERT_DIFFERS(seq1, seq4)
      seq4 << 1;
      TS_ASSERT_DIFFERS(seq1, seq4)
      seq4 << 2 << 3 << 4 << 5;
      TS_ASSERT_DIFFERS(seq1, seq4)
    }

    GUM_ACTIVE_TEST(Equality2) {
      std::vector< std::string > p(5);
      p[0] = "aa";
      p[1] = "bb";
      p[2] = "cc";
      p[3] = "dd";
      p[4] = "ee";

      gum::Sequence< std::string > seq1, seq2;
      seq1 << p[0] << p[1];
      seq2 << p[0] << p[1];
      TS_ASSERT_EQUALS(seq1, seq2)

      gum::Sequence< std::string > seq3;
      TS_ASSERT_DIFFERS(seq1, seq3)
      seq3 << p[0];
      TS_ASSERT_DIFFERS(seq1, seq3)
      seq3 << p[1] << p[2] << p[3] << p[4];
      TS_ASSERT_DIFFERS(seq1, seq3)

      gum::Sequence< std::string > seq4;
      TS_ASSERT_DIFFERS(seq1, seq4)
      seq4 << p[0] << p[1];
      TS_ASSERT_EQUALS(seq1, seq4)
      seq4 << p[2] << p[3] << p[4];
      TS_ASSERT_DIFFERS(seq1, seq4)
    }

    GUM_ACTIVE_TEST(GettersAndSetters) {
      gum::Sequence< std::string > seq;

      TS_GUM_ASSERT_THROWS_NOTHING(seq << "aaaa"
                                       << "b"
                                       << "cc");
      TS_GUM_ASSERT_THROWS_NOTHING(seq.insert("ddd"))

      TS_ASSERT_THROWS(seq.insert("ddd"), const gum::DuplicateElement&)
      TS_ASSERT_THROWS(seq << "ddd", const gum::DuplicateElement&)

      TS_ASSERT_EQUALS(seq.toString(), "[0:aaaa - 1:b - 2:cc - 3:ddd]")

      TS_ASSERT_EQUALS(seq[1], "b")
      TS_ASSERT_EQUALS(seq.pos("ddd"), (gum::Idx)3)

      TS_ASSERT(seq.exists("b"))
      TS_ASSERT(!seq.exists("bb"))

      TS_ASSERT_THROWS(seq[15], const gum::OutOfBounds&)
      TS_ASSERT_THROWS(seq.pos("aaaaaaa"), const gum::NotFound&)

      TS_ASSERT_EQUALS(seq.toString(), "[0:aaaa - 1:b - 2:cc - 3:ddd]")
      TS_GUM_ASSERT_THROWS_NOTHING(seq.setAtPos(1, "boom"))
      TS_ASSERT_EQUALS(seq.toString(), "[0:aaaa - 1:boom - 2:cc - 3:ddd]")
      TS_ASSERT_THROWS(seq.setAtPos(15, "crach"), const gum::NotFound&)
      TS_ASSERT_THROWS(seq.setAtPos(1, "aaaa"), const gum::DuplicateElement&)

      TS_ASSERT_EQUALS(seq.toString(), "[0:aaaa - 1:boom - 2:cc - 3:ddd]")

      TS_GUM_ASSERT_THROWS_NOTHING(seq.swap(0, 2))
      TS_ASSERT_EQUALS(seq.toString(), "[0:cc - 1:boom - 2:aaaa - 3:ddd]")
      TS_GUM_ASSERT_THROWS_NOTHING(seq.swap(0, 3))
      TS_ASSERT_EQUALS(seq.toString(), "[0:ddd - 1:boom - 2:aaaa - 3:cc]")
      TS_ASSERT_THROWS(seq.swap(1, 10), const gum::OutOfBounds&)
      TS_ASSERT_THROWS(seq.swap(10, 1), const gum::OutOfBounds&)

      TS_ASSERT_EQUALS(seq.toString(), "[0:ddd - 1:boom - 2:aaaa - 3:cc]")
    }

    GUM_ACTIVE_TEST(Remove) {
      gum::Sequence< int > seq;
      fill(seq);

      TS_ASSERT_EQUALS(seq.toString(), "[0:1 - 1:3 - 2:5 - 3:2 - 4:4]")
      TS_GUM_ASSERT_THROWS_NOTHING(seq >> 2)
      TS_ASSERT_EQUALS(seq.toString(), "[0:1 - 1:3 - 2:5 - 3:4]")
      TS_GUM_ASSERT_THROWS_NOTHING(seq.erase(1))
      TS_ASSERT_EQUALS(seq.toString(), "[0:3 - 1:5 - 2:4]")
      TS_GUM_ASSERT_THROWS_NOTHING(seq >> 4)
      TS_ASSERT_EQUALS(seq.toString(), "[0:3 - 1:5]")
      TS_GUM_ASSERT_THROWS_NOTHING(seq >> 4)
      TS_ASSERT_EQUALS(seq.toString(), "[0:3 - 1:5]")

      seq.clear();
      TS_ASSERT(seq.empty())
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)0)
      TS_ASSERT_EQUALS(seq.toString(), "[]")
    }

    GUM_ACTIVE_TEST(Iterators) {
      gum::Sequence< int > seq;

      int n;

      n = 0;

      for (auto it = seq.begin(); it != seq.end(); ++it)
        n++;

      TS_ASSERT_EQUALS(n, 0)

      n = 0;

      for (auto it = seq.rbegin(); it != seq.rend(); --it)
        n++;

      TS_ASSERT_EQUALS(n, 0)

      n = 0;

      for (const auto i: seq) {
        GUM_UNUSED(i);
        n++;
      }

      TS_ASSERT_EQUALS(n, 0)

      seq << 1 << 3 << 5 << 2 << 4;

      n = 0;

      for (auto it = seq.begin(); it != seq.end(); ++it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 13524)

      n = 0;

      for (const auto i: seq) {
        n *= 10;
        n += i;
      }

      TS_ASSERT_EQUALS(n, 13524)

      n = 0;

      for (gum::Sequence< int >::iterator it = seq.rbegin(); it != seq.rend(); --it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 42531)
    }

    GUM_ACTIVE_TEST(Iterators2) {
      gum::Sequence< int > seq;

      int n;
      n = 0;

      for (gum::Sequence< int >::iterator it = seq.begin(); it != seq.end(); ++it)
        n++;

      TS_ASSERT_EQUALS(n, 0)

      n = 0;

      for (gum::Sequence< int >::iterator it = seq.rbegin(); it != seq.rend(); --it)
        n++;

      TS_ASSERT_EQUALS(n, 0)

      seq << 1 << 3 << 5 << 2 << 4;
      n = 0;

      for (gum::Sequence< int >::iterator it = seq.begin(); it != seq.end(); ++it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 13524)

      n = 0;

      for (gum::Sequence< int >::iterator it = seq.rbegin(); it != seq.rend(); --it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, 42531)
    }

    GUM_ACTIVE_TEST(Iterators3) {
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

      TS_ASSERT_EQUALS(n, 0)

      n = 0;

      for (gum::Sequence< std::string >::iterator_safe it
           = seq.rbeginSafe();   // safe iterator needed here
           it != seq.rendSafe();
           --it)
        n++;

      TS_ASSERT_EQUALS(n, 0)
      n = 0;

      for (gum::Sequence< std::string >::iterator_safe it = seq.rbegin(); it != seq.rend(); --it)
        n++;

      TS_ASSERT_EQUALS(n, 0)
      n = 0;

      for (gum::Sequence< std::string >::iterator_safe it
           = seq.beginSafe();   // safe iterator needed here
           it != seq.endSafe();
           ++it)
        n++;

      TS_ASSERT_EQUALS(n, 0)
      n = 0;

      for (gum::Sequence< std::string >::iterator_safe it = seq.begin(); it != seq.end(); ++it)
        n++;

      TS_ASSERT_EQUALS(n, 0)
      n = 0;

      for (const auto& i: seq) {
        GUM_UNUSED(i.size());
        n++;
      }

      TS_ASSERT_EQUALS(n, 0)

      seq << p[1] << p[3] << p[5] << p[2] << p[4];

      n = 0;
      std::string str;

      for (const auto& s: seq)
        str += s;

      TS_ASSERT_EQUALS(str, "bbddffccee")

      str = "";

      for (gum::Sequence< std::string >::iterator_safe it = seq.rbegin(); it != seq.rend(); --it) {
        str += *it;
      }

      TS_ASSERT_EQUALS(str, "eeccffddbb")
    }

    GUM_ACTIVE_TEST(Iterators4) {
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

      TS_ASSERT_EQUALS(n, 0)
      n = 0;

      for (gum::Sequence< std::string >::iterator it = seq.rbegin(); it != seq.rend(); --it)
        n++;

      TS_ASSERT_EQUALS(n, 0)

      seq << p[1] << p[3] << p[5] << p[2] << p[4];

      n = 0;
      std::string str;

      for (gum::Sequence< std::string >::iterator it = seq.begin(); it != seq.end(); ++it) {
        str += *it;
      }

      TS_ASSERT_EQUALS(str, "bbddffccee")
      str = "";

      for (gum::Sequence< std::string >::iterator it = seq.rbegin(); it != seq.rend(); --it) {
        str += *it;
      }

      TS_ASSERT_EQUALS(str, "eeccffddbb")
    }

    GUM_ACTIVE_TEST(Insert1) {
      gum::Sequence< int > seq;

      seq.insert(1);
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)1)
      int x = 2;
      seq.insert(x);
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)2)

      seq.clear();
      seq << 1;
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)1)
      seq << x;
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)2)

      seq.erase(1);
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)1)
      seq.erase(seq.beginSafe());   // safe iterator needed here
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(Insert2) {
      gum::Sequence< std::string > seq;
      std::string                  x = "1";

      seq.insert(x);
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)1)
      seq.insert("2");
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)2)

      seq.clear();
      seq << x;
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)1)
      seq << "2";
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)2)

      seq.erase(x);
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)1)
      seq.erase(seq.beginSafe());   // safe iterator needed here
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(Emplace1) {
      gum::Sequence< std::pair< int, int > > seq;
      std::pair< int, int >                  x(2, 3);

      seq.emplace(2, 3);
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)1)
      seq.emplace(5, 7);
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)2)

      seq.erase(x);
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)1)
      seq.erase(seq.beginSafe());   // safe iterator needed here
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(Emplace2) {
      gum::Sequence< int > seq;
      int                  x = 2;

      seq.emplace(2);
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)1)
      seq.emplace(5);
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)2)

      seq.erase(x);
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)1)
      seq.erase(seq.beginSafe());   // safe iterator needed here
      TS_ASSERT_EQUALS(seq.size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(NewIterOp1) {
      gum::Sequence< int > seq;
      seq << 1 << 2 << 3 << 4 << 5;
      gum::SequenceIteratorSafe< int > iter = seq.begin();
      TS_ASSERT_EQUALS(*iter, 1)
      ++iter;
      TS_ASSERT_EQUALS(*iter, 2)
      iter += 2;
      TS_ASSERT_EQUALS(*iter, 4)
      iter -= 3;
      TS_ASSERT_EQUALS(*iter, 1)
      iter += 3;
      TS_ASSERT_EQUALS(*(iter - 2), 2)
      TS_ASSERT_EQUALS(*(iter + 1), 5)
    }

    GUM_ACTIVE_TEST(NewIterOp2) {
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
      TS_ASSERT_EQUALS(*iter, p[1])
      ++iter;
      TS_ASSERT_EQUALS(*iter, p[2])
      iter += 2;
      TS_ASSERT_EQUALS(*iter, p[4])
      iter -= 3;
      TS_ASSERT_EQUALS(*iter, p[1])
      iter += 3;
      TS_ASSERT_EQUALS(*(iter - 2), p[2])
      TS_ASSERT_EQUALS(*(iter + 1), p[5])
    }

    GUM_ACTIVE_TEST(NewIterOp3) {
      gum::Sequence< int > seq;
      seq << 1 << 2 << 3 << 4 << 5;
      gum::SequenceIterator< int > iter = seq.begin();
      TS_ASSERT_EQUALS(*iter, 1)
      ++iter;
      TS_ASSERT_EQUALS(*iter, 2)
      iter += 2;
      TS_ASSERT_EQUALS(*iter, 4)
      iter -= 3;
      TS_ASSERT_EQUALS(*iter, 1)
      iter += 3;
      TS_ASSERT_EQUALS(*(iter - 2), 2)
      TS_ASSERT_EQUALS(*(iter + 1), 5)
    }

    GUM_ACTIVE_TEST(NewIterOp4) {
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
      TS_ASSERT_EQUALS(*iter, p[1])
      ++iter;
      TS_ASSERT_EQUALS(*iter, p[2])
      iter += 2;
      TS_ASSERT_EQUALS(*iter, p[4])
      iter -= 3;
      TS_ASSERT_EQUALS(*iter, p[1])
      iter += 3;
      TS_ASSERT_EQUALS(*(iter - 2), p[2])
      TS_ASSERT_EQUALS(*(iter + 1), p[5])
    }

    GUM_ACTIVE_TEST(IdxSeq) {
      gum::Sequence< gum::Idx > seq;

      seq << 1 << 3 << 5 << 2 << 4;
      gum::Idx n = 0;

      for (auto it = seq.rbeginSafe(); it != seq.rendSafe(); --it) {   // safe iterator needed here
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, (gum::Size)42531)
      n = 0;

      for (auto it = seq.rbegin(); it != seq.rend(); --it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, (gum::Size)42531)
      n = 0;

      for (auto it = seq.beginSafe(); it != seq.endSafe(); ++it) {   // safe iterator needed here
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, (gum::Size)13524)
      n = 0;

      for (auto it = seq.begin(); it != seq.end(); ++it) {
        n *= 10;
        n += *it;
      }

      TS_ASSERT_EQUALS(n, (gum::Size)13524)
      n = 0;

      for (const auto i: seq) {
        n *= 10;
        n += i;
      }

      TS_ASSERT_EQUALS(n, (gum::Size)13524)

      gum::Sequence< gum::Idx > seq2;
      TS_ASSERT_THROWS(seq2.front(), const gum::NotFound&)
    }
  };
}   // namespace gum_tests
