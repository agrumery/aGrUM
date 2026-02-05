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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/tensor.h>
#include <agrum/BN/BayesNet.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Evidence
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct EvidenceTestSuite {
    public:
    static void testIsEvidence() {
      const auto bn = gum::BayesNet< double >::fastPrototype("A[10]->B[3]");
      CHECK(bn.cpt("A").isEvidence());
      CHECK(!((bn.cpt("B").isEvidence())));
      CHECK(!((bn.cpt("A").scale(100)).isEvidence()));
      CHECK(!((bn.cpt("A").translate(1)).isEvidence()));

      auto p = gum::Tensor(bn.cpt("A"));
      p.fillWith(0);
      CHECK(!(p.isEvidence()));
      p.fillWith(1);
      CHECK((p.isEvidence()));
      p.random();
      CHECK((p.isEvidence()));
    }   // namespace gum_tests

    static void testCombineEvidence() {
      const auto bn = gum::BayesNet< double >::fastPrototype("A[10]->C;B[10]");
      auto       p1 = gum::Tensor(bn.cpt("A"));
      auto       p2 = gum::Tensor(bn.cpt("A"));
      p2.randomCPT();
      auto q = gum::Tensor(bn.cpt("B"));

      CHECK_THROWS_AS(p1 | q, const gum::InvalidArgument&);
      CHECK_THROWS_AS(p1 & q, const gum::InvalidArgument&);
      CHECK_THROWS_AS(p1 & bn.cpt("C"), const gum::InvalidArgument&);

      auto Mp1 = p1.max();
      auto Mp2 = p2.max();
      auto mp1 = p1.min();
      auto mp2 = p2.min();

      CHECK((Mp1) <= ((p1 | p2).max()));
      CHECK((Mp2) <= ((p1 | p2).max()));
      CHECK((std::max(Mp1, Mp2)) == ((p1 | p2).max()));
      CHECK(((p1 & p2).min()) <= (mp1));
      CHECK(((p1 & p2).min()) <= (mp2));
      CHECK((std::min(mp1, mp2)) == ((p1 & p2).min()));

      CHECK((1) <= ((p1 | p2).sum()));
      CHECK(((p1 & p2).sum()) <= (1));

      CHECK((1 - p1.max()) == ((~p1).min()));
      CHECK((1 - p1.min()) == ((~p1).max()));
      CHECK((10 - 1) == doctest::Approx((~p1).sum()).epsilon(GUM_SMALL_ERROR));
    }

    static void testLikelihood() {
      const auto bn
          = gum::BayesNet< double >::fastPrototype("A[10];B[1,10];C{1.0:20.0:10};D{1:100:10};E[1.0:"
                                                   "20.0:10];X{A1|A2|A3|A4|A5|A6|A7|A8|A9|A10}");
      for (auto i: bn.nodes()) {
        CHECK((bn.variable(i).domainSize()) == (10u));
      }


      CHECK((toBoolString(gum::Tensor< double >::evEq(bn.variable("A"), 4.0))) == ("....1....."));
      CHECK((toBoolString(gum::Tensor< double >::evEq(bn.variable("B"), 4.0))) == ("...1......"));
      CHECK((toBoolString(gum::Tensor< double >::evEq(bn.variable("C"), 4.0))) == (".1........"));
      CHECK((toBoolString(gum::Tensor< double >::evEq(bn.variable("D"), 4.0))) == ("1........."));
      CHECK((toBoolString(gum::Tensor< double >::evEq(bn.variable("E"), 4.0))) == (".1........"));

      CHECK((toBoolString(gum::Tensor< double >::evEq(bn.variable("A"), 4.0)
                          | gum::Tensor< double >::evEq(bn.variable("A"), 8.0)))
            == ("....1...1."));
      CHECK((toBoolString(gum::Tensor< double >::evIn(bn.variable("A"), 4.0, 8.0)))
            == ("....11111."));
      CHECK((toBoolString(~gum::Tensor< double >::evIn(bn.variable("A"), 4.0, 8.0)))
            == ("1111.....1"));

      CHECK((toBoolString(gum::Tensor< double >::evLt(bn.variable("A"), 4.0))) == ("1111......"));
      CHECK((toBoolString(gum::Tensor< double >::evLt(bn.variable("A"), 0.0))) == ("1........."));

      CHECK((toBoolString(gum::Tensor< double >::evGt(bn.variable("A"), 4.0))) == (".....11111"));
      CHECK((toBoolString(gum::Tensor< double >::evGt(bn.variable("A"), 8.0))) == (".........1"));

      CHECK((toBoolString(gum::Tensor< double >::evLt(bn.variable("C"), 7.0))) == ("111......."));
      CHECK((toBoolString(gum::Tensor< double >::evLt(bn.variable("C"), 1.0))) == ("1........."));

      CHECK((toBoolString(gum::Tensor< double >::evGt(bn.variable("C"), 7.0))) == ("....111111"));
      CHECK((toBoolString(gum::Tensor< double >::evGt(bn.variable("C"), 20.0))) == (".........1"));
    }

    private:
    static std::string toBoolString(const gum::Tensor< double >& p) {
      std::string        s = "";
      gum::Instantiation I(p);
      for (I.setFirst(); !I.end(); I.inc())
        s += p.get(I) == 0 ? "." : "1";
      return s;
    }
  };

  GUM_TEST_ACTIF(IsEvidence)
  GUM_TEST_ACTIF(CombineEvidence)
  GUM_TEST_ACTIF(Likelihood)
}   // namespace gum_tests
