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

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/KTBN/KTBN.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>


namespace gum_tests {

  struct KTBNArcTestSuite {
    static constexpr int AT = gum::KTBN< double >::ATEMPORAL;

    // bracket-notation overloads: addArc/eraseArc/existsArc(string, string).
    // "Y[0]" is an atemporal variable name here, not temporal Y at slice 0.
  };

  GUM_TEST(AddArcAndValidityRules) {
    gum::KTBN< double > m(3);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("Y", "", 2), true);
    m.add(gum::LabelizedVariable("C", "", 2), false);

    GUM_CHECK_ASSERT_THROWS_NOTHING(m.addArc("X", 0, "X", 2));   // lag 2
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.addArc("X", 1, "X", 2));   // lag 1
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.addArc("X", 2, "Y", 2));   // intra-slice
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.addArc("C", AT, "X", 2));  // atemporal -> temporal

    CHECK(m.existsArc("X", 0, "X", 2));
    CHECK_FALSE(m.existsArc("X", 0, "X", 1));
    CHECK_THROWS(m.existsArc("Z", 0, "X", 2));   // unknown variable -> throws

    CHECK_THROWS_AS(m.addArc("X", 2, "X", 0), const gum::OperationNotAllowed&);   // future->past
    CHECK_THROWS_AS(m.addArc("X", 0, "C", AT), const gum::OperationNotAllowed&);  // temp->atemp

    m.addArc("X", 0, "Y", 0);
    CHECK_THROWS_AS(m.addArc("Y", 0, "X", 0), const gum::InvalidDirectedCycle&);  // cycle

    m.eraseArc("X", 0, "Y", 0);
    CHECK_FALSE(m.existsArc("X", 0, "Y", 0));
  }

  GUM_TEST(ArcCount) {
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("Y", "", 2), true);
    m.add(gum::LabelizedVariable("C", "", 2), false);

    CHECK_EQ(m.sizeArcs(), gum::Size(0));
    m.addArc("X", 0, "X", 1);
    CHECK_EQ(m.sizeArcs(), gum::Size(1));
    m.addArc("C", AT, "X", 1);
    CHECK_EQ(m.sizeArcs(), gum::Size(2));
    m.addArc("X", 0, "Y", 1);
    CHECK_EQ(m.sizeArcs(), gum::Size(3));
    m.eraseArc("X", 0, "Y", 1);
    CHECK_EQ(m.sizeArcs(), gum::Size(2));

    // arcs() and sizeArcs() must agree on the count
    CHECK_EQ(m.arcs().size(), std::size_t(m.sizeArcs()));
  }

  GUM_TEST(EraseArcEdgeCases) {
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("X", "", 2), true);

    // erasing an absent arc (valid endpoints) throws
    CHECK_THROWS(m.eraseArc("X", 0, "X", 1));

    // erasing with a non-existent variable throws NotFound
    CHECK_THROWS_AS(m.eraseArc("Z", 0, "X", 1), const gum::NotFound&);
  }

  GUM_TEST(DuplicateArc) {
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.addArc("X", 0, "X", 1);
    CHECK_EQ(m.sizeArcs(), gum::Size(1));

    // adding the same arc twice throws DuplicateElement
    CHECK_THROWS_AS(m.addArc("X", 0, "X", 1), const gum::DuplicateElement&);
  }

  GUM_TEST(AtemporalArc) {
    gum::KTBN< double > m(1);
    m.add(gum::LabelizedVariable("C", "", 2), false);
    m.add(gum::LabelizedVariable("D", "", 3), false);

    // atemporal -> atemporal is allowed (static causal structure)
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.addArc("C", AT, "D", AT));
    CHECK(m.existsArc("C", AT, "D", AT));
    CHECK_EQ(m.sizeArcs(), gum::Size(1));

    auto ch = m.children("C", AT);
    CHECK_EQ(ch.size(), std::size_t(1));
    CHECK_EQ(ch[0].first, std::string("D"));
  }

  GUM_TEST(BracketNotationArcs) {
    gum::KTBN< double > m(3);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("Y[0]", "", 2), false);

    // addArc via bracket names: atemporal "Y[0]" -> temporal X at slice 2
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.addArc("Y[0]", "X[2]"));

    // existsArc(string,string) agrees with the (base,slice) query
    CHECK(m.existsArc("Y[0]", "X[2]"));
    CHECK(m.existsArc("Y[0]", AT, "X", 2));

    // eraseArc removes it and the bracket query reflects that
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.eraseArc("Y[0]", "X[2]"));
    CHECK_FALSE(m.existsArc("Y[0]", "X[2]"));

    // regular temporal bracket names: X[0] -> X[2] (lag 2)
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.addArc("X[0]", "X[2]"));
    CHECK(m.existsArc("X[0]", "X[2]"));
    CHECK(m.existsArc("X", 0, "X", 2));

    // unknown bracket name is NotFound
    CHECK_THROWS_AS(m.addArc("Z[0]", "X[2]"), const gum::NotFound&);

    // causality violation still propagates from the underlying overload
    CHECK_THROWS_AS(m.addArc("X[2]", "X[0]"), const gum::OperationNotAllowed&);

    // parents/children bracket overloads resolve the same way
    m.addArc("Y[0]", "X[1]");
    auto ch = m.children("Y[0]");   // atemporal bracket name
    CHECK_EQ(ch.size(), std::size_t(1));   // X[1] only (Y[0]->X[2] was erased above)

    auto pa = m.parents("X[1]");   // temporal bracket name
    CHECK_EQ(pa.size(), std::size_t(1));
    CHECK_EQ(pa[0].first, std::string("Y[0]"));
    CHECK_EQ(pa[0].second, AT);
  }
}   // namespace gum_tests
