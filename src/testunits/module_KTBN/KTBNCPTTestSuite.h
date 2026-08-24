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

#include <cmath>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/base/variables/rangeVariable.h>
#include <agrum/KTBN/KTBN.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>


namespace gum_tests {

  struct KTBNCPTTestSuite {
    static constexpr int AT = gum::KTBN< double >::ATEMPORAL;

    // canonical k=2 model (no CPTs): temporal X, atemporal C; arcs X[0]->X[1], C->X[1]
    static gum::KTBN< double > buildK2Model() {
      gum::KTBN< double > m(2);
      m.add(gum::LabelizedVariable("X", "", 2), true);
      m.add(gum::LabelizedVariable("C", "", 2), false);
      m.addArc("X", 0, "X", 1);
      m.addArc("C", AT, "X", 1);
      return m;
    }

    // A node whose parents are the SAME process at two different slices — the
    // case that makes (base, slice) addressing necessary.

    // A non-binary node (3 modalities) with a binary parent.

    // "listed argument exists but is not a parent of the node" branch.

    // generateCPTs() must leave every CPT normalized.

    // bracket-notation overloads: cpt(string) and generateCPT(string) must
    // resolve the same way as the (base, slice) forms.
  };

  GUM_TEST(CPTShapes) {
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.addArc("X", 0, "X", 1);
    m.generateCPTs();

    CHECK_EQ(m.cpt("X", 0).nbrDim(), gum::Idx(1));   // P(X@0): no parents
    CHECK_EQ(m.cpt("X", 1).nbrDim(), gum::Idx(2));   // P(X@1 | X@0)
  }

  GUM_TEST(FillCPT) {
    gum::KTBN< double > m = buildK2Model();   // parent order of X[1]: X[0] then C

    // no-parent nodes
    m.fillCPT("X", 0, {}, {0.7, 0.3});
    m.fillCPT("C", AT, {}, {0.4, 0.6});

    // X[1] | X[0], C — parents specified in REVERSE of arc-insertion order
    m.fillCPT("X", 1, {{{"C", AT}, 0}, {{"X", 0}, 0}}, {0.9, 0.1});
    m.fillCPT("X", 1, {{{"C", AT}, 0}, {{"X", 0}, 1}}, {0.2, 0.8});
    m.fillCPT("X", 1, {{{"C", AT}, 1}, {{"X", 0}, 0}}, {0.6, 0.4});
    m.fillCPT("X", 1, {{{"C", AT}, 1}, {{"X", 0}, 1}}, {0.3, 0.7});

    // reference filled positionally (layout: X[1] fastest, then X[0], C)
    gum::KTBN< double > ref = buildK2Model();
    ref.cpt("X", 0).fillWith({0.7, 0.3});
    ref.cpt("C").fillWith({0.4, 0.6});
    ref.cpt("X", 1).fillWith({0.9, 0.1, 0.2, 0.8, 0.6, 0.4, 0.3, 0.7});

    GUM_CHECK_TENSOR_ALMOST_EQUALS(m.cpt("X", 0), ref.cpt("X", 0));
    GUM_CHECK_TENSOR_ALMOST_EQUALS(m.cpt("C"), ref.cpt("C"));
    GUM_CHECK_TENSOR_ALMOST_EQUALS(m.cpt("X", 1), ref.cpt("X", 1));

    CHECK_THROWS_AS(m.fillCPT("X", 0, {}, {0.5}), const gum::SizeError&);
    CHECK_THROWS_AS(m.fillCPT("X", 1, {{{"X", 0}, 0}}, {0.5, 0.5}), const gum::SizeError&);
    // a dictionary key is unique by construction: {"X",0} spelled twice collapses
    // to a single entry before fillCPT ever runs, so the observable symptom is
    // "too few parents", not "listed more than once" (unreachable via this overload).
    CHECK_THROWS_AS(m.fillCPT("X", 1, {{{"X", 0}, 0}, {{"X", 0}, 1}}, {0.5, 0.5}),
                    const gum::SizeError&);
    CHECK_THROWS_AS(m.fillCPT("X", 1, {{{"X", 0}, 9}, {{"C", AT}, 0}}, {0.5, 0.5}),
                    const gum::OutOfBounds&);
  }

  GUM_TEST(FillCPTByName) {
    gum::KTBN< double > m = buildK2Model();

    // fill using internal engine names — fillCPT does not care about parent order
    m.fillCPT("X[0]", {}, {0.7, 0.3});
    m.fillCPT("C", {}, {0.4, 0.6});
    m.fillCPT("X[1]", {{"C", 0}, {"X[0]", 1}}, {0.9, 0.1});
    m.fillCPT("X[1]", {{"C", 0}, {"X[0]", 0}}, {0.2, 0.8});
    m.fillCPT("X[1]", {{"C", 1}, {"X[0]", 1}}, {0.6, 0.4});
    m.fillCPT("X[1]", {{"C", 1}, {"X[0]", 0}}, {0.3, 0.7});

    // reference built with the (base, slice) overload in forward parent order
    gum::KTBN< double > ref = buildK2Model();
    ref.fillCPT("X", 0, {}, {0.7, 0.3});
    ref.fillCPT("C", AT, {}, {0.4, 0.6});
    ref.fillCPT("X", 1, {{{"X", 0}, 1}, {{"C", AT}, 0}}, {0.9, 0.1});
    ref.fillCPT("X", 1, {{{"X", 0}, 0}, {{"C", AT}, 0}}, {0.2, 0.8});
    ref.fillCPT("X", 1, {{{"X", 0}, 1}, {{"C", AT}, 1}}, {0.6, 0.4});
    ref.fillCPT("X", 1, {{{"X", 0}, 0}, {{"C", AT}, 1}}, {0.3, 0.7});

    GUM_CHECK_TENSOR_ALMOST_EQUALS(m.cpt("X", 0), ref.cpt("X", 0));
    GUM_CHECK_TENSOR_ALMOST_EQUALS(m.cpt("C"), ref.cpt("C"));
    GUM_CHECK_TENSOR_ALMOST_EQUALS(m.cpt("X", 1), ref.cpt("X", 1));

    CHECK_THROWS_AS(m.fillCPT("X[0]", {}, {0.5}), const gum::SizeError&);
    CHECK_THROWS_AS(m.fillCPT("NONODE", {}, {0.5, 0.5}), const gum::NotFound&);
    CHECK_THROWS_AS(m.fillCPT("X[1]", {{"X[0]", 0}}, {0.5, 0.5}), const gum::SizeError&);
    CHECK_THROWS_AS(m.fillCPT("X[1]", {{"X[0]", 9}, {"C", 0}}, {0.5, 0.5}),
                    const gum::OutOfBounds&);
    // a plain std::map would silently collapse "X[0]" spelled twice into one
    // entry before fillCPT ever runs; mixing the engine-name spelling with the
    // (base,slice) spelling of the SAME node keeps them as two distinct
    // dictionary keys, so this still reaches the runtime seen-parent check.
    CHECK_THROWS_AS(m.fillCPT("X[1]", {{"X[0]", 0}, {std::pair{"X", 0}, 1}}, {0.5, 0.5}),
                    const gum::InvalidArgument&);
  }

  GUM_TEST(FillCPTByModalityLabel) {
    // a parent value may be spelled as a modality label instead of an index,
    // and the two spellings may be mixed within one parent map.
    auto build = []() {
      gum::KTBN< double > m(2);
      m.add(gum::LabelizedVariable("X", "", {"no", "yes"}), true);
      m.add(gum::LabelizedVariable("C", "", {"low", "high"}), false);
      m.addArc("X", 0, "X", 1);
      m.addArc("C", AT, "X", 1);
      return m;
    };

    gum::KTBN< double > byIndex = build(), byLabel = build(), mixed = build();

    byIndex.fillCPT("X", 1, {{{"X", 0}, 0}, {{"C", AT}, 0}}, {0.9, 0.1});
    byIndex.fillCPT("X", 1, {{{"X", 0}, 1}, {{"C", AT}, 0}}, {0.2, 0.8});
    byIndex.fillCPT("X", 1, {{{"X", 0}, 0}, {{"C", AT}, 1}}, {0.6, 0.4});
    byIndex.fillCPT("X", 1, {{{"X", 0}, 1}, {{"C", AT}, 1}}, {0.3, 0.7});

    byLabel.fillCPT("X", 1, {{{"X", 0}, "no"}, {{"C", AT}, "low"}}, {0.9, 0.1});
    byLabel.fillCPT("X", 1, {{{"X", 0}, "yes"}, {{"C", AT}, "low"}}, {0.2, 0.8});
    byLabel.fillCPT("X", 1, {{{"X", 0}, "no"}, {{"C", AT}, "high"}}, {0.6, 0.4});
    byLabel.fillCPT("X", 1, {{{"X", 0}, "yes"}, {{"C", AT}, "high"}}, {0.3, 0.7});

    mixed.fillCPT("X", 1, {{{"X", 0}, 0}, {{"C", AT}, "low"}}, {0.9, 0.1});
    mixed.fillCPT("X", 1, {{{"X", 0}, "yes"}, {{"C", AT}, 0}}, {0.2, 0.8});
    mixed.fillCPT("X[1]", {{"X[0]", "no"}, {"C", 1}}, {0.6, 0.4});   // engine-name overload
    mixed.fillCPT("X[1]", {{"X[0]", 1}, {"C", "high"}}, {0.3, 0.7});

    GUM_CHECK_TENSOR_ALMOST_EQUALS(byLabel.cpt("X", 1), byIndex.cpt("X", 1));
    GUM_CHECK_TENSOR_ALMOST_EQUALS(mixed.cpt("X", 1), byIndex.cpt("X", 1));

    // an unknown label is rejected, on both overloads
    CHECK_THROWS_AS(byLabel.fillCPT("X", 1, {{{"X", 0}, "maybe"}, {{"C", AT}, 0}}, {0.5, 0.5}),
                    const gum::OutOfBounds&);
    CHECK_THROWS_AS(byLabel.fillCPT("X[1]", {{"X[0]", "maybe"}, {"C", 0}}, {0.5, 0.5}),
                    const gum::OutOfBounds&);
  }

  GUM_TEST(FillCPTLabelIsNotAnIndex) {
    // for a variable whose modalities are numeric, the label and the index of a
    // given modality differ: on a range 2..5, index 1 is the modality "3".
    gum::KTBN< double > m(2);
    m.add(gum::RangeVariable("R", "", 2, 5), true);
    m.addArc("R", 0, "R", 1);

    gum::KTBN< double > byLabel = m;
    m.fillCPT("R", 1, {{{"R", 0}, 1}}, {0.1, 0.2, 0.3, 0.4});          // index 1
    byLabel.fillCPT("R", 1, {{{"R", 0}, "3"}}, {0.1, 0.2, 0.3, 0.4});   // label "3" == index 1
    GUM_CHECK_TENSOR_ALMOST_EQUALS(byLabel.cpt("R", 1), m.cpt("R", 1));

    // "2" is the FIRST modality, whereas the index 2 is the modality "4"
    CHECK_EQ(m.variable("R", 0).index("2"), gum::Idx(0));
    CHECK_EQ(m.variable("R", 0).label(2), std::string("4"));

    // a label outside the range is rejected
    CHECK_THROWS_AS(m.fillCPT("R", 1, {{{"R", 0}, "9"}}, {0.1, 0.2, 0.3, 0.4}),
                    const gum::NotFound&);
  }

  GUM_TEST(GenerateSingleCPT) {
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.addArc("X", 0, "X", 1);

    // hand-fill X[0], leave X[1] zero, then randomly (re)generate only X[1]
    m.cpt("X", 0).fillWith({0.7, 0.3});
    m.cpt("X", 1).fillWith(0.0);
    const gum::Tensor< double > x0before = m.cpt("X", 0);

    m.generateCPT("X", 1);

    // 2 parent configs, each a normalized distribution -> tensor sums to 2.0
    CHECK(std::abs(m.cpt("X", 1).sum() - 2.0) < 1e-9);
    // a single-node regeneration must leave X[0] untouched
    GUM_CHECK_TENSOR_ALMOST_EQUALS(m.cpt("X", 0), x0before);

    CHECK_THROWS(m.generateCPT("Z", 0));
  }

  GUM_TEST(FillCPTMultiSliceParents) {
    gum::KTBN< double > m(3);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.addArc("X", 0, "X", 2);   // parents added in this order: X[0] then X[1]
    m.addArc("X", 1, "X", 2);

    // fill X[2] | X[0], X[1] addressing parents in REVERSE order, to prove
    // the result is independent of the argument order
    m.fillCPT("X", 2, {{{"X", 1}, 0}, {{"X", 0}, 0}}, {0.9, 0.1});
    m.fillCPT("X", 2, {{{"X", 1}, 0}, {{"X", 0}, 1}}, {0.2, 0.8});
    m.fillCPT("X", 2, {{{"X", 1}, 1}, {{"X", 0}, 0}}, {0.6, 0.4});
    m.fillCPT("X", 2, {{{"X", 1}, 1}, {{"X", 0}, 1}}, {0.3, 0.7});

    // reference filled positionally (layout: X[2] fastest, then X[0], X[1])
    gum::KTBN< double > ref(3);
    ref.add(gum::LabelizedVariable("X", "", 2), true);
    ref.addArc("X", 0, "X", 2);
    ref.addArc("X", 1, "X", 2);
    ref.cpt("X", 2).fillWith({0.9, 0.1, 0.2, 0.8, 0.6, 0.4, 0.3, 0.7});

    GUM_CHECK_TENSOR_ALMOST_EQUALS(m.cpt("X", 2), ref.cpt("X", 2));
  }

  GUM_TEST(FillCPTNonBinary) {
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("Y", "", 3), true);
    m.add(gum::LabelizedVariable("C", "", 2), false);
    m.addArc("C", AT, "Y", 0);

    m.fillCPT("Y", 0, {{{"C", AT}, 0}}, {0.2, 0.3, 0.5});
    m.fillCPT("Y", 0, {{{"C", AT}, 1}}, {0.1, 0.1, 0.8});

    // reference positional (Y fastest over its 3 modalities, then C)
    gum::KTBN< double > ref(2);
    ref.add(gum::LabelizedVariable("Y", "", 3), true);
    ref.add(gum::LabelizedVariable("C", "", 2), false);
    ref.addArc("C", AT, "Y", 0);
    ref.cpt("Y", 0).fillWith({0.2, 0.3, 0.5, 0.1, 0.1, 0.8});

    GUM_CHECK_TENSOR_ALMOST_EQUALS(m.cpt("Y", 0), ref.cpt("Y", 0));

    // a distribution must carry exactly domainSize (3) values
    CHECK_THROWS_AS(m.fillCPT("Y", 0, {{{"C", AT}, 0}}, {0.5, 0.5}), const gum::SizeError&);
  }

  GUM_TEST(FillCPTNotAParent) {
    gum::KTBN< double > m = buildK2Model();
    m.add(gum::LabelizedVariable("D", "", 2), false);   // exists, never a parent

    // (base, slice) overload: D is a real variable but not a parent of X[1]
    CHECK_THROWS_AS(m.fillCPT("X", 1, {{{"X", 0}, 0}, {{"D", AT}, 0}}, {0.5, 0.5}),
                    const gum::InvalidArgument&);

    // name overload: same not-a-parent rejection, and an unknown name is NotFound
    CHECK_THROWS_AS(m.fillCPT("X[1]", {{"X[0]", 0}, {"D", 0}}, {0.5, 0.5}),
                    const gum::InvalidArgument&);
    CHECK_THROWS_AS(m.fillCPT("X[1]", {{"X[0]", 0}, {"NOPE", 0}}, {0.5, 0.5}),
                    const gum::NotFound&);
  }

  GUM_TEST(GenerateAllCPTsNormalized) {
    gum::KTBN< double > m = buildK2Model();

    m.generateCPTs();

    CHECK(std::abs(m.cpt("X", 0).sum() - 1.0) < 1e-9);   // no parents
    CHECK(std::abs(m.cpt("C").sum() - 1.0) < 1e-9);      // no parents
    CHECK(std::abs(m.cpt("X", 1).sum() - 4.0) < 1e-9);   // 2 parents (X[0], C)
  }

  GUM_TEST(BracketNotationCPT) {
    gum::KTBN< double > m = buildK2Model();

    m.fillCPT("X[0]", {}, {0.3, 0.7});
    m.fillCPT("C",    {}, {0.4, 0.6});

    CHECK_EQ(&m.cpt("X[0]"), &m.cpt("X", 0));
    CHECK_EQ(&m.cpt("C"), &m.cpt("C", AT));
    CHECK_EQ(&m.cpt("X[1]"), &m.cpt("X", 1));

    GUM_CHECK_ASSERT_THROWS_NOTHING(m.generateCPT("X[1]"));
    CHECK(std::abs(m.cpt("X[1]").sum() - 4.0) < 1e-9);   // 2 parent configs

    CHECK_THROWS_AS(m.cpt("Z[0]"), const gum::NotFound&);
    CHECK_THROWS_AS(m.generateCPT("Z[0]"), const gum::NotFound&);
  }
}   // namespace gum_tests
