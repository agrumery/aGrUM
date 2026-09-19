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
#include <string>
#include <vector>

#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/KTBN/KTBN.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct KTBNVariableTestSuite {
    static constexpr int AT = gum::KTBN< double >::ATEMPORAL;

    // CPT values in natural Instantiation order, as a flat vector.
    static std::vector< double > flatCPT(const gum::Tensor< double >& cpt) {
      std::vector< double > v;
      v.reserve(cpt.domainSize());
      for (gum::Instantiation inst(cpt); !inst.end(); inst.inc())
        v.push_back(cpt[inst]);
      return v;
    }

    // The DiscreteVariable overloads of addTemporal()/addAtemporal() (the string
    // "fast" overloads are covered by testFastDescription).

    // unlike the atemporal case (testAddVariableErrors), a temporal "X[0]" base
    // name is NOT rejected: slice nodes get a second bracket ("X[0][0]",
    // "X[0][1]"), and (base, slice) round-trips via the LAST bracket.

    // bracket-notation overload: "X[1]" -> (X,1), "C" -> (C,ATEMPORAL),
    // bracket-named atemporal "Y[0]" -> (Y[0], ATEMPORAL).
  };

  GUM_TEST(AddVariables) {
    gum::KTBN< double > m(3);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("Y", "", 3), true);
    m.add(gum::LabelizedVariable("C", "", 2), false);

    CHECK_EQ(m.nbTemporalVars(), gum::Size(2));
    CHECK_EQ(m.nbAtemporalVars(), gum::Size(1));
    CHECK_EQ(m.size(), gum::Size(2 * 3 + 1));

    CHECK(m.temporalVarNames().contains("X"));
    CHECK(m.atemporalVarNames().contains("C"));
    CHECK(m.exists("Y"));
    CHECK_FALSE(m.exists("Z"));

    CHECK_EQ(m.timeSlice(m.variable("X", 1)), 1);
    CHECK_EQ(m.baseName(m.variable("X", 1)), std::string("X"));
    CHECK_EQ(m.timeSlice(m.variable("C")), AT);

    CHECK_EQ(m.variable("X", 0).domainSize(), gum::Size(2));
    CHECK_EQ(m.variable("Y", 2).domainSize(), gum::Size(3));

    // timeSlice()/baseName() of a variable that is not a node must throw
    const gum::LabelizedVariable foreign("FOREIGN", "", 2);
    CHECK_THROWS_AS(m.timeSlice(foreign), const gum::NotFound&);
    CHECK_THROWS_AS(m.baseName(foreign), const gum::NotFound&);
  }

  GUM_TEST(AddVariableOverloads) {
    gum::KTBN< double > m(2);
    m.addTemporal(gum::LabelizedVariable("X", "", 3));    // -> add(var, true)
    m.addAtemporal(gum::LabelizedVariable("C", "", 2));   // -> add(var, false)

    CHECK(m.temporalVarNames().contains("X"));
    CHECK(m.atemporalVarNames().contains("C"));
    CHECK_EQ(m.nbTemporalVars(), gum::Size(1));
    CHECK_EQ(m.nbAtemporalVars(), gum::Size(1));
    CHECK_EQ(m.size(), gum::Size(2 * 1 + 1));   // X[0], X[1], C
    CHECK_EQ(m.variable("X", 0).domainSize(), gum::Size(3));
    CHECK_EQ(m.variable("C").domainSize(), gum::Size(2));
  }

  GUM_TEST(AddVariableErrors) {
    gum::KTBN< double > m(3);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("C", "", 2), false);

    CHECK_THROWS_AS(m.add(gum::LabelizedVariable("X", "", 2), true), const gum::DuplicateLabel&);
    // names not matching the "[number]" slice pattern decode as plain bases,
    // so they're unambiguous (bracket-number temporal base: testTemporalBracketBaseName)
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.add(gum::LabelizedVariable("W2", "", 2), true));
    CHECK(m.temporalVarNames().contains("W2"));
    CHECK_EQ(m.baseName(m.variable("W2", 1)), std::string("W2"));
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.add(gum::LabelizedVariable("A[B]", "", 2), true));
    CHECK(m.temporalVarNames().contains("A[B]"));
    // atemporal "X[0]" conflicts with temporal "X" at slice 0
    CHECK_THROWS_AS(m.add(gum::LabelizedVariable("X[0]", "", 2), false),
                    const gum::InvalidArgument&);
    // a bracket-named atemporal variable is allowed when no temporal process
    // shares its decoded base (there is no temporal "Z" here)
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.add(gum::LabelizedVariable("Z[1]", "", 2), false));
    CHECK(m.atemporalVarNames().contains("Z[1]"));
    // ... but adding temporal "Z" now collides with the existing "Z[1]" node
    CHECK_THROWS_AS(m.add(gum::LabelizedVariable("Z", "", 2), true), const gum::InvalidArgument&);

    // temporal and atemporal may NOT share the same base name
    CHECK_THROWS_AS(m.add(gum::LabelizedVariable("X", "", 2), false), const gum::DuplicateLabel&);
    CHECK_FALSE(m.atemporalVarNames().contains("X"));
    CHECK(m.temporalVarNames().contains("X"));

    CHECK_THROWS_AS(m.variable("X", 5), const gum::OutOfBounds&);
    CHECK_THROWS_AS(m.variable("X", AT), const gum::OperationNotAllowed&);
    CHECK_THROWS_AS(m.variable("C", 0), const gum::OperationNotAllowed&);
    CHECK_THROWS_AS(m.variable("Z", 0), const gum::NotFound&);
  }

  GUM_TEST(TemporalBracketBaseName) {
    gum::KTBN< double > m(2);
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.add(gum::LabelizedVariable("X[0]", "", 2), true));

    CHECK(m.temporalVarNames().contains("X[0]"));
    CHECK_EQ(m.nbTemporalVars(), gum::Size(1));
    CHECK_EQ(m.size(), gum::Size(2));   // slice nodes "X[0][0]" and "X[0][1]"

    // both slices are addressable under the bracketed base name
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.variable("X[0]", 0));
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.variable("X[0]", 1));

    // (base, slice) round-trips: the LAST bracket is the slice index
    CHECK_EQ(m.baseName(m.variable("X[0]", 1)), std::string("X[0]"));
    CHECK_EQ(m.timeSlice(m.variable("X[0]", 1)), 1);
  }

  GUM_TEST(FastDescription) {
    gum::KTBN< double > m(2);
    m.add("Z[3]", true);    // temporal Z, 3 modalities
    m.add("C[4]", false);   // atemporal C, 4 modalities

    CHECK(m.temporalVarNames().contains("Z"));
    CHECK(m.atemporalVarNames().contains("C"));
    CHECK_EQ(m.variable("Z", 0).domainSize(), gum::Size(3));
    CHECK_EQ(m.variable("Z", 1).domainSize(), gum::Size(3));
    CHECK_EQ(m.variable("C").domainSize(), gum::Size(4));

    gum::KTBN< double > m2(2);
    m2.addTemporal("X[3]");         // temporal X, 3 modalities
    m2.addAtemporal("C{yes|no}");   // atemporal C, labels yes/no
    m2.addTemporal("R");            // temporal R, default 2 modalities

    CHECK(m2.temporalVarNames().contains("X"));
    CHECK(m2.atemporalVarNames().contains("C"));
    CHECK(m2.temporalVarNames().contains("R"));
    CHECK_EQ(m2.variable("X", 0).domainSize(), gum::Size(3));
    CHECK_EQ(m2.variable("C").domainSize(), gum::Size(2));
    CHECK_EQ(m2.variable("R", 1).domainSize(), gum::Size(2));
  }

  GUM_TEST(Erase) {
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("Y", "", 2), true);
    m.add(gum::LabelizedVariable("C", "", 2), false);
    m.addArc("X", 0, "X", 1);
    m.addArc("C", AT, "X", 1);

    m.erase("Y");
    CHECK_FALSE(m.temporalVarNames().contains("Y"));
    CHECK_EQ(m.nbTemporalVars(), gum::Size(1));
    CHECK_EQ(m.size(), gum::Size(3));   // X[0], X[1], C

    m.add(gum::LabelizedVariable("Z", "", 2), true);
    m.erase("Z");
    CHECK_FALSE(m.temporalVarNames().contains("Z"));

    m.erase("C");
    CHECK_FALSE(m.atemporalVarNames().contains("C"));
    CHECK_EQ(m.nbAtemporalVars(), gum::Size(0));
    CHECK_EQ(m.sizeArcs(), gum::Size(1));   // C->X[1] arc removed with C; X[0]->X[1] remains

    CHECK_THROWS_AS(m.erase("W"), const gum::NotFound&);
  }

  GUM_TEST(ChangeVariableName) {
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("C", "", 2), false);
    m.addArc("X", 0, "X", 1);
    m.addArc("C", AT, "X", 1);

    // known CPT values, to check they survive the re-encode of the slice names
    const std::vector< double > x1values = {0.9, 0.1, 0.2, 0.8, 0.6, 0.4, 0.3, 0.7};
    m.cpt("X", 1).fillWith(x1values);

    // rename temporal process
    m.changeVariableName("X", "W");

    // the renamed process keeps its CPT contents intact
    CHECK_EQ(flatCPT(m.cpt("W", 1)), x1values);
    CHECK(m.temporalVarNames().contains("W"));
    CHECK_FALSE(m.temporalVarNames().contains("X"));
    CHECK_FALSE(m.exists("X"));
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.variable("W", 0));
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.variable("W", 1));
    CHECK(m.existsArc("W", 0, "W", 1));
    CHECK(m.existsArc("C", AT, "W", 1));

    m.changeVariableName("W", "V");
    CHECK(m.temporalVarNames().contains("V"));
    CHECK_FALSE(m.temporalVarNames().contains("W"));

    // rename atemporal variable
    m.changeVariableName("C", "D");
    CHECK(m.atemporalVarNames().contains("D"));
    CHECK_FALSE(m.atemporalVarNames().contains("C"));
    CHECK(m.existsArc("D", AT, "V", 1));

    // no-op: rename to same name
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.changeVariableName("D", "D"));

    // renaming atemporal to bracket notation is forbidden
    CHECK_THROWS_AS(m.changeVariableName("D", "V[0]"), const gum::InvalidArgument&);

    // temporal and atemporal may NOT share a name
    CHECK_THROWS_AS(m.changeVariableName("V", "D"), const gum::DuplicateLabel&);

    m.add(gum::LabelizedVariable("P", "", 2), true);
    CHECK_THROWS_AS(m.changeVariableName("P", "D"), const gum::DuplicateLabel&);
    CHECK_THROWS_AS(m.changeVariableName("P", ""), const gum::InvalidArgument&);
    CHECK_THROWS_AS(m.changeVariableName("Z", "W"), const gum::NotFound&);
  }

  GUM_TEST(ParentsChildren) {
    gum::KTBN< double > m(3);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("Y", "", 2), true);
    m.add(gum::LabelizedVariable("C", "", 2), false);
    m.addArc("X", 0, "X", 2);
    m.addArc("X", 1, "X", 2);
    m.addArc("C", AT, "X", 2);
    m.addArc("X", 2, "Y", 2);

    const auto pX2 = m.parents("X", 2);
    CHECK_EQ(pX2.size(), std::size_t(3));   // X@0, X@1, C

    CHECK(m.parents("X", 0).empty());

    const auto cX2 = m.children("X", 2);
    CHECK_EQ(cX2.size(), std::size_t(1));
    CHECK_EQ(cX2[0].first, std::string("Y"));
    CHECK_EQ(cX2[0].second, 2);

    const auto cC = m.children("C");
    CHECK_EQ(cC.size(), std::size_t(1));
    CHECK_EQ(cC[0].first, std::string("X"));
  }

  GUM_TEST(BracketNotationVariable) {
    gum::KTBN< double > m(3);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("C", "", 4), false);
    m.add(gum::LabelizedVariable("Y[0]", "", 3), false);   // bracket-named atemporal

    // pointer identity: bracket overload returns the same object as 2-arg form
    CHECK_EQ(&m.variable("X[0]"), &m.variable("X", 0));
    CHECK_EQ(&m.variable("X[2]"), &m.variable("X", 2));
    CHECK_EQ(&m.variable("C"), &m.variable("C", AT));

    // atemporal bracket-named variable: "Y[0]" is in _atemporal_,
    // so _determineNode_ resolves it to ("Y[0]", ATEMPORAL), not ("Y", 0)
    CHECK_EQ(&m.variable("Y[0]"), &m.variable("Y[0]", AT));
    CHECK_EQ(m.variable("Y[0]").domainSize(), gum::Size(3));

    // domain sizes are correct
    CHECK_EQ(m.variable("X[1]").domainSize(), gum::Size(2));
    CHECK_EQ(m.variable("C").domainSize(), gum::Size(4));

    // unknown name throws NotFound
    CHECK_THROWS_AS(m.variable("Z[0]"), const gum::NotFound&);
  }
}   // namespace gum_tests
