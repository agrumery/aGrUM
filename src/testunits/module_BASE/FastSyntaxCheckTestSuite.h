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

#include <string_view>
#include <vector>

#include <agrum/base/graphicalModels/fastSyntaxCheck.h>

#include <testunits/gumtest/AgrumTestSuite.h>

namespace gum_tests {

  struct FastSyntaxCheckTestSuite {};

  using gum::FastSyntax;

  GUM_TEST(ValidSyntaxes) {
    using V = std::vector< FastSyntax >;
    // a description may be valid for several syntaxes
    CHECK(gum::checkFastSyntax("A->B->C").validSyntaxes()
          == V{FastSyntax::DiGraph,
               FastSyntax::MixedGraph,
               FastSyntax::MarkedGraph,
               FastSyntax::BayesNet,
               FastSyntax::InfluenceDiagram});
    CHECK(gum::checkFastSyntax("A-B").validSyntaxes()
          == V{FastSyntax::UndiGraph,
               FastSyntax::MixedGraph,
               FastSyntax::MarkedGraph,
               FastSyntax::MarkovRandomField});
    CHECK_EQ(gum::checkFastSyntax("A->B;B-C").validSyntaxes(),
             V{FastSyntax::MixedGraph, FastSyntax::MarkedGraph});
    // in a model, an unquoted name may contain blanks: "A o" is a name there
    CHECK(gum::checkFastSyntax("A o-> B").validSyntaxes()
          == V{FastSyntax::MarkedGraph, FastSyntax::BayesNet, FastSyntax::InfluenceDiagram});
    CHECK(gum::checkFastSyntax("'A'o->'B'").validSyntaxes() == V{FastSyntax::MarkedGraph});
    CHECK(gum::checkFastSyntax("A--B--C").validSyntaxes() == V{FastSyntax::MarkovRandomField});
    // a domain is a part of the name of a graph node ("A{yes|no}"), a blank is not
    CHECK(gum::checkFastSyntax("A{yes|no}->B").validSyntaxes()
          == V{FastSyntax::DiGraph,
               FastSyntax::MixedGraph,
               FastSyntax::MarkedGraph,
               FastSyntax::BayesNet,
               FastSyntax::InfluenceDiagram});
    CHECK(gum::checkFastSyntax("A{yes|no}->Visit Asia").validSyntaxes()
          == V{FastSyntax::BayesNet, FastSyntax::InfluenceDiagram});
    CHECK(gum::checkFastSyntax("*D->$U").validSyntaxes()
          == V{FastSyntax::DiGraph,
               FastSyntax::MixedGraph,
               FastSyntax::MarkedGraph,
               FastSyntax::BayesNet,
               FastSyntax::InfluenceDiagram});
    CHECK(gum::checkFastSyntax("'Visit Asia'->X").validSyntaxes()
          == V{FastSyntax::DiGraph,
               FastSyntax::MixedGraph,
               FastSyntax::MarkedGraph,
               FastSyntax::BayesNet,
               FastSyntax::InfluenceDiagram});
    // acyclicity is not a matter of syntax
    CHECK(gum::checkFastSyntax("A->B->A").isValid(FastSyntax::DiGraph));
    CHECK(gum::checkFastSyntax("A->B->A").isValid(FastSyntax::BayesNet));
  }

  GUM_TEST(Errors) {
    const auto check = gum::checkFastSyntax("A->");
    CHECK(check.validSyntaxes().empty());
    CHECK_EQ(check.description(), "A->");
    for (const auto syntax: {FastSyntax::UndiGraph, FastSyntax::DiGraph, FastSyntax::BayesNet})
      CHECK_FALSE(check.error(syntax).empty());
    const auto arcs = gum::checkFastSyntax("A->B");
    CHECK(arcs.error(FastSyntax::DiGraph).empty());
    CHECK_NE(arcs.error(FastSyntax::UndiGraph).find("arc"), std::string::npos);
    CHECK(gum::checkFastSyntax("'A->B").validSyntaxes().empty());
    CHECK(gum::checkFastSyntax("A[3]x->B").validSyntaxes()
          == std::vector< FastSyntax >{FastSyntax::DiGraph,
                                       FastSyntax::MixedGraph,
                                       FastSyntax::MarkedGraph});
    CHECK_THROWS_AS(gum::checkFastSyntax("A->B", "2"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(gum::checkFastSyntax("A->B", "[x]"), const gum::InvalidArgument&);
    // a number which cannot be read: not a variable, but still a node name
    const auto number = gum::checkFastSyntax("A[x]->B");
    CHECK(number.isValid(FastSyntax::DiGraph));
    CHECK_FALSE(number.isValid(FastSyntax::BayesNet));
    CHECK_NE(number.error(FastSyntax::BayesNet).find("Invalid number"), std::string::npos);
    CHECK_THROWS_AS(number.isValid(static_cast< FastSyntax >(42)), const gum::OutOfBounds&);
    CHECK_THROWS_AS(number.error(static_cast< FastSyntax >(42)), const gum::OutOfBounds&);
    // a misplaced operator is refused in a model too
    CHECK(gum::checkFastSyntax("A<->B").validSyntaxes()
          == std::vector< FastSyntax >{FastSyntax::MarkedGraph});
  }

  GUM_TEST(WhatTheModelsRefuseIsNotValid) {
    // an arc given twice (BayesNet::addArc and InfluenceDiagram::addArc refuse it)
    const auto twice = gum::checkFastSyntax("A->B;A->B");
    CHECK(twice.isValid(FastSyntax::DiGraph));
    CHECK_FALSE(twice.isValid(FastSyntax::BayesNet));
    CHECK_FALSE(twice.isValid(FastSyntax::InfluenceDiagram));
    CHECK_NE(twice.error(FastSyntax::BayesNet).find("twice"), std::string::npos);
    CHECK_FALSE(gum::checkFastSyntax("A->B;B<-A").isValid(FastSyntax::BayesNet));

    // a factor given twice (whatever the order) or empty (MarkovRandomField::addFactor refuses it)
    for (const auto* desc: {"A--B;B--A", "A;A", "A;--", "A--B--C;C--A--B"}) {
      CHECK_FALSE(gum::checkFastSyntax(desc).isValid(FastSyntax::MarkovRandomField));
    }
    CHECK(gum::checkFastSyntax("A--B;A").isValid(FastSyntax::MarkovRandomField));
    CHECK(gum::checkFastSyntax("A--B;A--B--C").isValid(FastSyntax::MarkovRandomField));

    // a '-' left at an end of a name by a misplaced "--": "A---B" is not A and "-B"
    for (const auto* desc: {"A---B", "A---", "-A--B"})
      CHECK_FALSE(gum::checkFastSyntax(desc).isValid(FastSyntax::MarkovRandomField));
    CHECK(gum::checkFastSyntax("'-B'--A").isValid(FastSyntax::MarkovRandomField));
  }

}   // namespace gum_tests
