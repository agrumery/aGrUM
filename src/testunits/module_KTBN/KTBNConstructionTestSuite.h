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
 *   useful, but WITHOUT ANY KIND, EXPRESS OR IMPLIED,                      *
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

#include <sstream>
#include <string>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/KTBN/KTBN.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>


namespace gum_tests {

  struct KTBNConstructionTestSuite {
    static constexpr int AT = gum::KTBN< double >::ATEMPORAL;

    // canonical k=2 model (no CPTs): temporal X{2}, atemporal C{2}, X@0->X@1, C->X@1
    static gum::KTBN< double > buildK2Model() {
      gum::KTBN< double > m(2);
      m.add(gum::LabelizedVariable("X", "", 2), true);
      m.add(gum::LabelizedVariable("C", "", 2), false);
      m.addArc("X", 0, "X", 1);
      m.addArc("C", AT, "X", 1);
      return m;
    }
  };

  GUM_TEST(ConstructorAndKOrder) {
    gum::KTBN< double > m(3);
    CHECK_EQ(m.k(), gum::Size(3));
    CHECK(m.empty());
    CHECK_EQ(m.size(), gum::Size(0));
    CHECK_THROWS_AS(gum::KTBN< double >(0), const gum::InvalidArgument&);

    // default constructor: k defaults to 2
    CHECK_EQ(gum::KTBN< double >().k(), gum::Size(2));
  }

  GUM_TEST(CopyAndMove) {
    gum::KTBN< double > a = buildK2Model();
    a.generateCPTs();

    gum::KTBN< double > b(a);   // (deep) copy
    CHECK_EQ(b.k(), gum::Size(2));
    CHECK(b.existsArc("X", 0, "X", 1));

    // copy preserves temporal/atemporal classification
    CHECK_EQ(b.nbTemporalVars(), a.nbTemporalVars());
    CHECK_EQ(b.nbAtemporalVars(), a.nbAtemporalVars());

    // copy preserves CPT content
    GUM_CHECK_TENSOR_ALMOST_EQUALS(b.cpt("X", 1), a.cpt("X", 1));

    // deep-copy independence: mutating the copy must not touch the original
    b.add(gum::LabelizedVariable("Z", "", 2), true);
    CHECK(b.exists("Z"));
    CHECK_FALSE(a.exists("Z"));
    b.eraseArc("X", 0, "X", 1);
    CHECK_FALSE(b.existsArc("X", 0, "X", 1));
    CHECK(a.existsArc("X", 0, "X", 1));

    gum::KTBN< double > c(std::move(a));
    CHECK(c.existsArc("X", 0, "X", 1));

    gum::KTBN< double > d(1);
    d = c;
    CHECK(d.existsArc("X", 0, "X", 1));

    // self copy-assignment is a no-op, not a corruption
    gum::KTBN< double >& dref = d;
    d                         = dref;
    CHECK_EQ(d.k(), gum::Size(2));
    CHECK(d.existsArc("X", 0, "X", 1));

    gum::KTBN< double > e(1);
    e = std::move(c);
    CHECK(e.existsArc("X", 0, "X", 1));
  }

  GUM_TEST(FromBN) {
    // k=2 round-trip: fromBN must recover temporal/atemporal classification and k
    {
      gum::KTBN< double > orig = buildK2Model();
      orig.generateCPTs();

      const gum::BayesNet< double > bn = orig.toBN();
      CHECK_EQ(bn.size(), gum::Size(3));   // X[0], X[1], C

      const auto restored = gum::KTBN< double >::fromBN(bn);
      CHECK_EQ(restored.k(), gum::Size(2));
      CHECK(restored.exists("X"));
      CHECK(restored.exists("C"));
      CHECK(restored.existsArc("X", 0, "X", 1));
      CHECK(restored.existsArc("C", AT, "X", 1));
    }

    // k=1 (order-0): X[0] alone is a valid temporal process, must not throw
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X[0]", "", 2));
      bn.add(gum::LabelizedVariable("C", "", 2));
      bn.addArc(bn.idFromName("C"), bn.idFromName("X[0]"));
      bn.generateCPTs();

      gum::KTBN< double > m1;
      CHECK_NOTHROW(m1 = gum::KTBN< double >::fromBN(bn));
      CHECK_EQ(m1.k(), gum::Size(1));
      CHECK(m1.exists("X"));
      CHECK(m1.exists("C"));
    }

    // orphan bracket with k >= 2: Y holds a single slice, so it is reclassified
    // as an atemporal variable (bracket name kept) with a warning; X stays a
    // process. Declaring it says the same thing explicitly, without the warning.
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X[0]", "", 2));
      bn.add(gum::LabelizedVariable("X[1]", "", 2));
      bn.add(gum::LabelizedVariable("Y[0]", "", 2));   // Y has only one slice → orphan
      bn.generateCPTs();

      std::vector< std::string > warnings;
      auto                       m = gum::KTBN< double >::fromBN(bn, {}, &warnings);
      CHECK_EQ(m.k(), gum::Size(2));
      CHECK(m.temporalVarNames().contains("X"));
      CHECK(m.atemporalVarNames().contains("Y[0]"));
      CHECK_EQ(warnings.size(), std::size_t(1));

      warnings.clear();
      auto m2 = gum::KTBN< double >::fromBN(bn, {"Y[0]"}, &warnings);
      CHECK(m2.atemporalVarNames().contains("Y[0]"));
      CHECK(warnings.empty());

      // a declared name that is not a node of the BN is rejected
      CHECK_THROWS_AS(gum::KTBN< double >::fromBN(bn, {"Z"}), const gum::NotFound&);
    }

    // missing middle slice: X[0] and X[2] present (so k=3) but X[1] absent, so
    // no process survives and k falls back to 1
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X[0]", "", 2));
      bn.add(gum::LabelizedVariable("X[2]", "", 2));
      bn.generateCPTs();

      std::vector< std::string > warnings;
      auto                       m = gum::KTBN< double >::fromBN(bn, {}, &warnings);
      CHECK_EQ(m.k(), gum::Size(1));
      CHECK(m.temporalVarNames().empty());
      CHECK(m.atemporalVarNames().contains("X[0]"));
      CHECK(m.atemporalVarNames().contains("X[2]"));
      CHECK_EQ(warnings.size(), std::size_t(1));
    }

    // temporal -> atemporal arc is rejected
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X[0]", "", 2));
      bn.add(gum::LabelizedVariable("X[1]", "", 2));
      bn.add(gum::LabelizedVariable("C", "", 2));
      bn.addArc(bn.idFromName("X[1]"), bn.idFromName("C"));   // temporal -> atemporal
      bn.generateCPTs();
      CHECK_THROWS_AS(gum::KTBN< double >::fromBN(bn), const gum::OperationNotAllowed&);
    }

    // future -> past arc is rejected
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X[0]", "", 2));
      bn.add(gum::LabelizedVariable("X[1]", "", 2));
      bn.addArc(bn.idFromName("X[1]"), bn.idFromName("X[0]"));   // slice 1 -> slice 0
      bn.generateCPTs();
      CHECK_THROWS_AS(gum::KTBN< double >::fromBN(bn), const gum::OperationNotAllowed&);
    }

    // toBN() returns an INDEPENDENT deep copy: mutating it must not touch the KTBN
    {
      gum::KTBN< double > m(2);
      m.add(gum::LabelizedVariable("X", "", 2), true);
      m.addArc("X", 0, "X", 1);
      m.generateCPTs();

      gum::BayesNet< double > bn = m.toBN();
      const gum::Size         before = m.size();
      bn.add(gum::LabelizedVariable("NEW", "", 2));   // mutate the returned BN
      CHECK_FALSE(m.exists("NEW"));
      CHECK_EQ(m.size(), before);
    }

    // bare "Y" (atemporal) collides with bracket "Y[0]"/"Y[1]" (temporal)
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("Y", "", 2));
      bn.add(gum::LabelizedVariable("Y[0]", "", 2));
      bn.add(gum::LabelizedVariable("Y[1]", "", 2));
      bn.generateCPTs();
      CHECK_THROWS_AS(gum::KTBN< double >::fromBN(bn), const gum::OperationNotAllowed&);
    }
  }

  GUM_TEST(FromBNBracketFreeConvention) {
    // basic k=2: X0/X1 (temporal, base "X") + C (atemporal, no trailing digit).
    // Also checks that the source names get renamed onto the engine's canonical
    // base[t] form: existsArc()/exists() go through that encoding, so they would
    // fail to find anything if the rename had not happened.
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X0", "", 2));
      bn.add(gum::LabelizedVariable("X1", "", 2));
      bn.add(gum::LabelizedVariable("C", "", 2));
      bn.addArc(bn.idFromName("X0"), bn.idFromName("X1"));
      bn.addArc(bn.idFromName("C"), bn.idFromName("X1"));
      bn.generateCPTs();

      const auto m = gum::KTBN< double >::fromBN(bn);
      CHECK_EQ(m.k(), gum::Size(2));
      CHECK(m.temporalVarNames().contains("X"));
      CHECK(m.atemporalVarNames().contains("C"));
      CHECK(m.existsArc("X", 0, "X", 1));
      CHECK(m.existsArc("C", AT, "X", 1));
    }

    // k=1 (order-0): a lone "X0" is a complete process on its own, must not throw
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X0", "", 2));
      bn.add(gum::LabelizedVariable("C", "", 2));
      bn.addArc(bn.idFromName("C"), bn.idFromName("X0"));
      bn.generateCPTs();

      gum::KTBN< double > m;
      CHECK_NOTHROW(m = gum::KTBN< double >::fromBN(bn));
      CHECK_EQ(m.k(), gum::Size(1));
      CHECK(m.exists("X"));
      CHECK(m.exists("C"));
    }

    // multi-digit base disambiguation: base is everything before the trailing run
    // of digits, so "X12" belongs to process "X" at slice 12, not process "X1"
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X0", "", 2));
      bn.add(gum::LabelizedVariable("X1", "", 2));
      bn.generateCPTs();

      const auto m = gum::KTBN< double >::fromBN(bn);
      CHECK(m.temporalVarNames().contains("X"));
      CHECK_FALSE(m.temporalVarNames().contains("X1"));
    }

    // missing slice: X0 and X12 present (so k=13) but slices 1..11 absent, so the
    // process does not survive and is reclassified as atemporal, original
    // (bracket-free) names kept, k falls back to 1
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X0", "", 2));
      bn.add(gum::LabelizedVariable("X12", "", 2));
      bn.generateCPTs();

      std::vector< std::string > warnings;
      const auto                 m = gum::KTBN< double >::fromBN(bn, {}, &warnings);
      CHECK_EQ(m.k(), gum::Size(1));
      CHECK(m.temporalVarNames().empty());
      CHECK(m.atemporalVarNames().contains("X0"));
      CHECK(m.atemporalVarNames().contains("X12"));
      CHECK_EQ(warnings.size(), std::size_t(1));
    }

    // atemporalNodes lifts the ambiguity explicitly, without a warning
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X0", "", 2));
      bn.generateCPTs();

      std::vector< std::string > warnings;
      const auto m = gum::KTBN< double >::fromBN(bn, {"X0"}, &warnings);
      CHECK(m.atemporalVarNames().contains("X0"));
      CHECK(warnings.empty());
    }

    // a bracket-shaped atemporalNodes name must not flip the whole graph onto
    // the bracket convention: X0/X1 stay a bracket-free temporal process even
    // though "Y[0]" (explicitly atemporal) looks bracket-shaped
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X0", "", 2));
      bn.add(gum::LabelizedVariable("X1", "", 2));
      bn.add(gum::LabelizedVariable("Y[0]", "", 2));
      bn.addArc(bn.idFromName("X0"), bn.idFromName("X1"));
      bn.generateCPTs();

      const auto m = gum::KTBN< double >::fromBN(bn, {"Y[0]"});
      CHECK_EQ(m.k(), gum::Size(2));
      CHECK(m.temporalVarNames().contains("X"));
      CHECK(m.existsArc("X", 0, "X", 1));
      CHECK(m.atemporalVarNames().contains("Y[0]"));
    }

    // bare "Y" (atemporal) collides with digit-suffixed "Y0"/"Y1" (temporal)
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("Y", "", 2));
      bn.add(gum::LabelizedVariable("Y0", "", 2));
      bn.add(gum::LabelizedVariable("Y1", "", 2));
      bn.generateCPTs();
      CHECK_THROWS_AS(gum::KTBN< double >::fromBN(bn), const gum::OperationNotAllowed&);
    }

    // temporal -> atemporal arc is rejected
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X0", "", 2));
      bn.add(gum::LabelizedVariable("X1", "", 2));
      bn.add(gum::LabelizedVariable("C", "", 2));
      bn.addArc(bn.idFromName("X1"), bn.idFromName("C"));   // temporal -> atemporal
      bn.generateCPTs();
      CHECK_THROWS_AS(gum::KTBN< double >::fromBN(bn), const gum::OperationNotAllowed&);
    }

    // future -> past arc is rejected
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("X0", "", 2));
      bn.add(gum::LabelizedVariable("X1", "", 2));
      bn.addArc(bn.idFromName("X1"), bn.idFromName("X0"));   // slice 1 -> slice 0
      bn.generateCPTs();
      CHECK_THROWS_AS(gum::KTBN< double >::fromBN(bn), const gum::OperationNotAllowed&);
    }

    // the two conventions never mix: a single bracket-named node anywhere in the
    // graph forces the WHOLE graph to be read under the bracket convention, so a
    // digit-suffixed name elsewhere ("Z9") is then read literally, atemporal
    {
      gum::BayesNet< double > bn;
      bn.add(gum::LabelizedVariable("Y[0]", "", 2));
      bn.add(gum::LabelizedVariable("Y[1]", "", 2));
      bn.add(gum::LabelizedVariable("Z9", "", 2));
      bn.generateCPTs();

      const auto m = gum::KTBN< double >::fromBN(bn);
      CHECK(m.temporalVarNames().contains("Y"));
      CHECK(m.atemporalVarNames().contains("Z9"));
      CHECK_FALSE(m.temporalVarNames().contains("Z"));
    }
  }

  GUM_TEST(ToString) {
    const gum::KTBN< double > m = buildK2Model();

    const std::string s = m.toString();
    CHECK(s.find("k=2") != std::string::npos);
    CHECK(s.find("temporal processes (1)") != std::string::npos);
    CHECK(s.find("atemporal variables (1)") != std::string::npos);
    CHECK(s.find("arcs (2)") != std::string::npos);
    CHECK(s.find("X[0]") != std::string::npos);
    CHECK(s.find("X[1]") != std::string::npos);
  }

  GUM_TEST(NodesArcsAndClear) {
    gum::KTBN< double > m = buildK2Model();

    CHECK_EQ(m.sizeArcs(), gum::Size(2));

    // nodes(): one atemporal C + two temporal X slices = 3 (base,slice) pairs
    const auto nodes = m.nodes();
    CHECK_EQ(nodes.size(), gum::Size(3));
    gum::Size nTemporal = 0, nAtemporal = 0;
    for (const auto& [base, slice]: nodes) {
      if (slice == AT) {
        ++nAtemporal;
        CHECK_EQ(base, std::string("C"));
      } else {
        ++nTemporal;
        CHECK_EQ(base, std::string("X"));
        CHECK((slice == 0 || slice == 1));
      }
    }
    CHECK_EQ(nAtemporal, gum::Size(1));
    CHECK_EQ(nTemporal, gum::Size(2));

    // arcs(): the two (tail,head) couples of (base,slice)
    const auto arcs = m.arcs();
    CHECK_EQ(arcs.size(), gum::Size(2));
    bool sawXX = false, sawCX = false;
    for (const auto& [tail, head]: arcs) {
      if (tail.first == "X" && tail.second == 0 && head.first == "X" && head.second == 1)
        sawXX = true;
      if (tail.first == "C" && tail.second == AT && head.first == "X" && head.second == 1)
        sawCX = true;
    }
    CHECK(sawXX);
    CHECK(sawCX);

    // clear() empties variables and arcs but keeps the order k
    m.clear();
    CHECK(m.empty());
    CHECK_EQ(m.size(), gum::Size(0));
    CHECK_EQ(m.sizeArcs(), gum::Size(0));
    CHECK_EQ(m.k(), gum::Size(2));
    CHECK(m.nodes().empty());
    CHECK(m.arcs().empty());
  }

  GUM_TEST(DotAndStreamOutput) {
    const gum::KTBN< double > m = buildK2Model();

    // toDot(): time-slice clustered view with bracket-encoded node ids
    const std::string dot = m.toDot();
    CHECK(dot.find("digraph KTBN") != std::string::npos);
    CHECK(dot.find("cluster_0") != std::string::npos);
    CHECK(dot.find("cluster_1") != std::string::npos);
    CHECK(dot.find("\"C\" [label=\"C\"]") != std::string::npos);   // atemporal: no cluster
    CHECK(dot.find("\"X[0]\" -> \"X[1]\"") != std::string::npos);

    // toUnrolledDot(): highlightReplicated is off by default (uniform gray clusters)
    const std::string undot = m.toUnrolledDot(3);
    CHECK(undot.find("cluster_2") != std::string::npos);
    CHECK(undot.find("lightcyan") == std::string::npos);

    // toUnrolledDot(T, true): slices >= k (here slice 2) are shaded lightcyan
    const std::string undotHl = m.toUnrolledDot(3, true);
    CHECK(undotHl.find("cluster_2 {\n    label=\"Time slice 2\";\n    style=filled;\n    "
                        "bgcolor=\"lightcyan\"")
          != std::string::npos);
    CHECK(undotHl.find("cluster_0 {\n    label=\"Time slice 0\";\n    style=filled;\n    "
                        "bgcolor=\"#DDDDDD\"")
          != std::string::npos);

    // bnToDot(): raw storage-BN dot, also using bracket engine names
    const std::string bndot = m.bnToDot();
    CHECK(bndot.find("digraph") != std::string::npos);
    CHECK(bndot.find("X[0]") != std::string::npos);

    // operator<< prints toString()
    std::stringstream ss;
    ss << m;
    CHECK_EQ(ss.str(), m.toString());
  }
}   // namespace gum_tests
