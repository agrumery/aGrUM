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
#pragma once


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/tensor.h>
#include <agrum/BN/BayesNet.h>

namespace gum_tests {

  class GUM_TEST_SUITE(Evidence) {
    public:
    GUM_ACTIVE_TEST(IsEvidence) {
      const auto bn = gum::BayesNet< double >::fastPrototype("A[10]->B[3]");
      TS_ASSERT(bn.cpt("A").isEvidence())
      TS_ASSERT(!((bn.cpt("B").isEvidence())))
      TS_ASSERT(!((bn.cpt("A").scale(100)).isEvidence()))
      TS_ASSERT(!((bn.cpt("A").translate(1)).isEvidence()))

      auto p = gum::Tensor(bn.cpt("A"));
      p.fillWith(0);
      TS_ASSERT(!(p.isEvidence()))
      p.fillWith(1);
      TS_ASSERT((p.isEvidence()))
      p.random();
      TS_ASSERT((p.isEvidence()))
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(CombineEvidence) {
      const auto bn = gum::BayesNet< double >::fastPrototype("A[10]->C;B[10]");
      auto       p1 = gum::Tensor(bn.cpt("A"));
      auto       p2 = gum::Tensor(bn.cpt("A"));
      p2.randomCPT();
      auto q = gum::Tensor(bn.cpt("B"));

      TS_ASSERT_THROWS(p1 | q, const gum::InvalidArgument&)
      TS_ASSERT_THROWS(p1 & q, const gum::InvalidArgument&)
      TS_ASSERT_THROWS(p1 & bn.cpt("C"), const gum::InvalidArgument&)

      auto Mp1 = p1.max();
      auto Mp2 = p2.max();
      auto mp1 = p1.min();
      auto mp2 = p2.min();

      TS_ASSERT_LESS_THAN_EQUALS(Mp1, (p1 | p2).max())
      TS_ASSERT_LESS_THAN_EQUALS(Mp2, (p1 | p2).max())
      TS_ASSERT_EQUALS(std::max(Mp1, Mp2), (p1 | p2).max())
      TS_ASSERT_LESS_THAN_EQUALS((p1 & p2).min(), mp1)
      TS_ASSERT_LESS_THAN_EQUALS((p1 & p2).min(), mp2)
      TS_ASSERT_EQUALS(std::min(mp1, mp2), (p1 & p2).min())

      TS_ASSERT_LESS_THAN_EQUALS(1, (p1 | p2).sum())
      TS_ASSERT_LESS_THAN_EQUALS((p1 & p2).sum(), 1)

      TS_ASSERT_EQUALS(1 - p1.max(), (~p1).min())
      TS_ASSERT_EQUALS(1 - p1.min(), (~p1).max())
      TS_ASSERT_DELTA(10 - 1, (~p1).sum(), TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(Likelihood) {
      const auto bn
          = gum::BayesNet< double >::fastPrototype("A[10];B[1,10];C{1.0:20.0:10};D{1:100:10};E[1.0:"
                                                   "20.0:10];X{A1|A2|A3|A4|A5|A6|A7|A8|A9|A10}");
      for (auto i: bn.nodes()) {
        TS_ASSERT_EQUALS(bn.variable(i).domainSize(), 10u)
      }


      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evEq(bn.variable("A"), 4.0)),
                       "....1.....")
      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evEq(bn.variable("B"), 4.0)),
                       "...1......")
      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evEq(bn.variable("C"), 4.0)),
                       ".1........")
      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evEq(bn.variable("D"), 4.0)),
                       "1.........")
      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evEq(bn.variable("E"), 4.0)),
                       ".1........")

      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evEq(bn.variable("A"), 4.0)
                                    | gum::Tensor< double >::evEq(bn.variable("A"), 8.0)),
                       "....1...1.")
      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evIn(bn.variable("A"), 4.0, 8.0)),
                       "....11111.")
      TS_ASSERT_EQUALS(toBoolString(~gum::Tensor< double >::evIn(bn.variable("A"), 4.0, 8.0)),
                       "1111.....1")

      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evLt(bn.variable("A"), 4.0)),
                       "1111......")
      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evLt(bn.variable("A"), 0.0)),
                       "1.........")

      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evGt(bn.variable("A"), 4.0)),
                       ".....11111")
      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evGt(bn.variable("A"), 8.0)),
                       ".........1")

      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evLt(bn.variable("C"), 7.0)),
                       "111.......")
      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evLt(bn.variable("C"), 1.0)),
                       "1.........")

      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evGt(bn.variable("C"), 7.0)),
                       "....111111")
      TS_ASSERT_EQUALS(toBoolString(gum::Tensor< double >::evGt(bn.variable("C"), 20.0)),
                       ".........1")
    }

    private:
    std::string toBoolString(const gum::Tensor< double >& p) {
      std::string        s = "";
      gum::Instantiation I(p);
      for (I.setFirst(); !I.end(); I.inc())
        s += p.get(I) == 0 ? "." : "1";
      return s;
    }
  };
}   // namespace gum_tests
