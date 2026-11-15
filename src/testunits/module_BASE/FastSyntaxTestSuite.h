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

#include <string>
#include <string_view>
#include <vector>

#include <agrum/base/core/fastSyntax.h>
#include <agrum/base/graphs/fastGraph.h>
#include <agrum/base/graphs/fastMarkedGraph.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/variables/allDiscreteVariables.h>

#include <testunits/gumtest/AgrumTestSuite.h>

namespace gum_tests {

  // the lexical rules shared by the fast syntaxes, through the public fast functions

  struct FastSyntaxTestSuite {};

  namespace {
    std::string nameOf(std::string_view desc) {
      return gum::fastVariable< double >(std::string(desc))->name();
    }
  }   // namespace

  GUM_TEST(QuoteOpensAtTheBeginningOfAName) {
    // after an operator, a ';', a blank or an ID prefix
    const auto g = gum::fastGraph< gum::MixedGraph >("'a b'->'c d'<-'e f';'g h'-'i j'");
    for (const std::string name: {"a b", "c d", "e f", "g h", "i j"})
      CHECK_MESSAGE(g.idFromName(name).has_value(), name);
    // a quote inside a word or inside brackets is an ordinary character
    CHECK_EQ(nameOf("O'Brien[3]"), "O'Brien");
    CHECK_EQ(gum::fastVariable< double >("A{l'eau|x}")->label(0), "l'eau");
    // "-o'" opens a quoted name only in a marked graph (a circle mark)
    CHECK(gum::fastGraph< gum::MixedGraph >("A-o'Brien").idFromName("o'Brien").has_value());
    const auto m = gum::fastMarkedGraph("A -o'B;C'");
    CHECK(m.isCircle(m.idFromName("A").value(), m.idFromName("B;C").value()));
  }

  GUM_TEST(QuotedNamesHideTheOperators) {
    const auto g = gum::fastGraph< gum::MixedGraph >("'a-b'->'c;d';'x\ny'->'[z]'");
    CHECK(g.existsArc(g.idFromName("a-b").value(), g.idFromName("c;d").value()));
    CHECK(g.existsArc(g.idFromName("x\ny").value(), g.idFromName("[z]").value()));
    // inside brackets, a line break is removed (a domain written on several lines)
    CHECK_EQ(gum::fastVariable< double >("A[1,\n3]")->domainSize(), static_cast< gum::Size >(3));
  }

  GUM_TEST(DoubledQuotes) {
    CHECK_EQ(nameOf("'l''eau'"), "l'eau");
    CHECK_EQ(nameOf("' x '"), " x ");   // a quoted name is not trimmed
  }

  GUM_TEST(MalformedQuotedNames) {
    for (const std::string_view desc: {"'A", "''", "'A'x", "'A' 'B'"}) {
      CHECK_THROWS_AS(gum::fastVariable< double >(std::string(desc)), const gum::InvalidArgument&);
      CHECK_THROWS_AS(gum::fastGraph< gum::MixedGraph >(desc), const gum::InvalidArgument&);
    }
  }

  GUM_TEST(SplitTopLevel) {
    CHECK((gum::fast::detail::splitTopLevel("A--'B--C'--D[0,1]", "--")
           == std::vector< std::string >{"A", "'B--C'", "D[0,1]"}));
    // an empty separator would never advance
    CHECK_THROWS_AS(gum::fast::detail::splitTopLevel("A--B", ""), const gum::InvalidArgument&);
  }

  GUM_TEST(QuoteOnlyWhenNeeded) {
    for (const std::string_view name: {"A", "Visit Asia", "O'Brien", "x.y", "a|b", "12"}) {
      CHECK_MESSAGE(!gum::fastNameNeedsQuotes(name), name);
      CHECK_EQ(gum::quoteFastName(name), std::string(name));
    }
    for (const std::string_view name: {" a",
                                       "a ",
                                       "'a",
                                       "*a",
                                       "$a",
                                       "X-ray",
                                       "X[0]",
                                       "a{b",
                                       "a;b",
                                       "a<b",
                                       "a>b",
                                       "X+",
                                       "rock 'n roll",
                                       "a\nb"}) {
      CHECK_MESSAGE(gum::fastNameNeedsQuotes(name), name);
      // a quoted name is read back as is
      CHECK_EQ(nameOf(gum::quoteFastName(name) + "[2]"), std::string(name));
    }
    CHECK_EQ(gum::quoteFastName("X-ray"), "'X-ray'");
    CHECK_EQ(gum::quoteFastName("rock 'n roll"), "'rock ''n roll'");
  }

}   // namespace gum_tests
