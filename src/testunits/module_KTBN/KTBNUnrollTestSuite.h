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

#include <cmath>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/KTBN/KTBN.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct KTBNUnrollTestSuite {
    static constexpr int AT = gum::KTBN< double >::ATEMPORAL;

    // max |a - b| over the two CPTs iterated in lockstep (same structure assumed)
    static double maxAbsCptDiff(const gum::Tensor< double >& a, const gum::Tensor< double >& b) {
      gum::Instantiation ia(a), ib(b);
      double             d = 0.0;
      for (ia.setFirst(), ib.setFirst(); !ia.end(); ia.inc(), ib.inc())
        d = std::max(d, std::fabs(a.get(ia) - b.get(ib)));
      return d;
    }

    // k=3 model: X, Y temporal; C atemporal; arcs of mixed lag into the kernel slice.
    static gum::KTBN< double > buildK3Model() {
      gum::KTBN< double > m(3);
      m.add(gum::LabelizedVariable("X", "", 2), true);
      m.add(gum::LabelizedVariable("Y", "", 2), true);
      m.add(gum::LabelizedVariable("C", "", 2), false);
      m.addArc("X", 0, "X", 2);
      m.addArc("X", 1, "X", 2);
      m.addArc("X", 2, "Y", 2);
      m.addArc("C", AT, "X", 2);
      m.generateCPTs();
      return m;
    }

    // horizon exactly equal to k: unroll reproduces just the template slices,
    // with no extension slices created (the Phase-2 extension loop never runs).

    // k=1: kernel slice is k-1 == 0, so every slice t>=1 replicates slice 0:
    // X[t] keeps C as its only parent and the same CPT, no X[t-1]->X[t] arc.

    // Degenerate case: no temporal variable at all. The k-DBN is a plain BN, so
    // unroll() must reproduce it once (atemporal nodes only), for any T >= k.
  };

  GUM_TEST(UnrollStructureArbitraryLag) {
    const auto              m  = buildK3Model();
    gum::BayesNet< double > bn = m.unroll(5);

    CHECK_EQ(bn.size(), gum::Size(5 + 5 + 1));   // X:5, Y:5, C:1

    // template arcs (slices 0..2) preserved verbatim
    CHECK(bn.existsArc("X[0]", "X[2]"));
    CHECK(bn.existsArc("X[1]", "X[2]"));
    CHECK(bn.existsArc("X[2]", "Y[2]"));
    CHECK(bn.existsArc("C", "X[2]"));

    // transition kernel repeated at t=3 (shift +1)
    CHECK(bn.existsArc("X[1]", "X[3]"));
    CHECK(bn.existsArc("X[2]", "X[3]"));
    CHECK(bn.existsArc("C", "X[3]"));
    CHECK(bn.existsArc("X[3]", "Y[3]"));

    // transition kernel repeated at t=4 (shift +2)
    CHECK(bn.existsArc("X[2]", "X[4]"));
    CHECK(bn.existsArc("X[3]", "X[4]"));
    CHECK(bn.existsArc("C", "X[4]"));
    CHECK(bn.existsArc("X[4]", "Y[4]"));

    // repeated kernel must carry the SAME CPT values as the template slice
    // (k-1 == 2). X has two lagged parents, so a plain lockstep compares
    // mismatched cells; map explicitly: rep X[3]<-tmpl X[2], X[2]<-X[1] (lag 1),
    // X[1]<-X[0] (lag 2), C<-C.
    {
      const gum::Tensor< double >& tmpl = bn.cpt("X[2]");
      const gum::Tensor< double >& rep  = bn.cpt("X[3]");
      const gum::DiscreteVariable& vX0  = bn.variableFromName("X[0]");
      const gum::DiscreteVariable& vX1  = bn.variableFromName("X[1]");
      const gum::DiscreteVariable& vX2  = bn.variableFromName("X[2]");
      const gum::DiscreteVariable& vX3  = bn.variableFromName("X[3]");
      const gum::DiscreteVariable& vC   = bn.variableFromName("C");
      double                       diff = 0.0;
      for (gum::Instantiation ir(rep); !ir.end(); ir.inc()) {
        gum::Instantiation it(tmpl);
        it.chgVal(vX2, ir.val(vX3));   // node
        it.chgVal(vX1, ir.val(vX2));   // lag-1 parent
        it.chgVal(vX0, ir.val(vX1));   // lag-2 parent
        it.chgVal(vC, ir.val(vC));     // atemporal parent
        diff = std::max(diff, std::fabs(rep.get(ir) - tmpl.get(it)));
      }
      CHECK_LT(diff, 1e-12);
    }
    // Y has a single parent X[t], so its kernel values can be compared directly
    CHECK_LT(maxAbsCptDiff(bn.cpt("Y[3]"), bn.cpt("Y[2]")), 1e-12);

    CHECK_THROWS_AS(m.unroll(2), const gum::OperationNotAllowed&);
  }

  GUM_TEST(UnrollHorizonEqualsK) {
    const auto              m  = buildK3Model();   // k = 3
    gum::BayesNet< double > bn = m.unroll(3);

    CHECK_EQ(bn.size(), gum::Size(3 + 3 + 1));     // X:3, Y:3, C:1
    CHECK(bn.exists("X[2]"));
    CHECK_FALSE(bn.exists("X[3]"));                // no extension slice
    CHECK(bn.existsArc("X[0]", "X[2]"));
    CHECK(bn.existsArc("C", "X[2]"));
  }

  GUM_TEST(UnrollInitialArcsDoNotPropagate) {
    gum::KTBN< double > m(3);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.addArc("X", 0, "X", 1);   // initial arc (into slice 1, not the k-1 kernel)
    m.generateCPTs();

    gum::BayesNet< double > bn = m.unroll(5);
    CHECK(bn.existsArc("X[0]", "X[1]"));
    CHECK_FALSE(bn.existsArc("X[1]", "X[2]"));   // initial arc must not be repeated
    CHECK_FALSE(bn.existsArc("X[2]", "X[3]"));
  }

  GUM_TEST(UnrollReusesHomogeneousKernel) {
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.addArc("X", 0, "X", 1);
    m.generateCPTs();

    gum::BayesNet< double > bn = m.unroll(3);
    // homogeneous kernel: P(X[2]|X[1]) must carry the same values as P(X[1]|X[0])
    CHECK_LT(maxAbsCptDiff(bn.cpt("X[1]"), bn.cpt("X[2]")), 1e-12);
  }

  GUM_TEST(UnrollK1Memoryless) {
    gum::KTBN< double > m(1);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("C", "", 2), false);
    m.addArc("C", AT, "X", 0);
    m.generateCPTs();

    gum::BayesNet< double > bn = m.unroll(4);
    CHECK_EQ(bn.size(), gum::Size(4 + 1));   // X:4, C:1

    for (gum::Size t = 0; t < 4; ++t) {
      const std::string xt = "X[" + std::to_string(t) + "]";
      CHECK(bn.exists(xt));
      CHECK(bn.existsArc("C", xt));
      CHECK_EQ(bn.parents(bn.idFromName(xt)).size(), gum::Size(1));   // C only, no X[t-1]
    }

    // every slice shares the same i.i.d. kernel P(X[t]|C)
    const gum::Tensor< double >& c0 = bn.cpt("X[0]");
    for (gum::Size t = 1; t < 4; ++t)
      CHECK_LT(maxAbsCptDiff(c0, bn.cpt("X[" + std::to_string(t) + "]")), 1e-12);

    CHECK_THROWS_AS(m.unroll(0), const gum::OperationNotAllowed&);
  }

  GUM_TEST(UnrollNoTemporalVars) {
    gum::KTBN< double > m(1);
    m.add(gum::LabelizedVariable("A", "", 2), false);
    m.add(gum::LabelizedVariable("B", "", 2), false);
    m.addArc("A", AT, "B", AT);
    m.generateCPTs();

    gum::BayesNet< double > bn = m.unroll(3);
    CHECK_EQ(bn.size(), gum::Size(2));   // A, B kept as-is, no replication
    CHECK(bn.exists("A"));
    CHECK(bn.exists("B"));
    CHECK(bn.existsArc("A", "B"));
    CHECK_FALSE(bn.exists("A[0]"));   // atemporal names are never bracketed
  }
}   // namespace gum_tests
