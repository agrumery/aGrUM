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

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/dot/dotOptions.h>
#include <agrum/KTBN/KTBN.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The k-TBNs are drawn with one cluster per time slice, whose colors are the ones of the section
  // dynamicBN.
  struct KTBNToDotTestSuite {};

  namespace {
    bool ktbnDotHas(const std::string& dot, const std::string& text) {
      return dot.find(text) != std::string::npos;
    }

    // canonical k=2 model : temporal X, atemporal C, X[0]->X[1], C->X[1]
    gum::KTBN< double > ktbnForDot() {
      gum::KTBN< double > m(2);
      m.add(gum::LabelizedVariable("X", "", 2), true);
      m.add(gum::LabelizedVariable("C", "", 2), false);
      m.addArc("X", 0, "X", 1);
      m.addArc("C", gum::KTBN< double >::ATEMPORAL, "X", 1);
      return m;
    }
  }   // namespace

  GUM_TEST(ClustersOfTheTimeSlices) {
    ConfigurationGuard guard;
    const auto         m   = ktbnForDot();
    const auto         dot = m.toDot();

    CHECK(ktbnDotHas(dot, "digraph \"KTBN\" {\n"));
    CHECK(ktbnDotHas(
        dot,
        "graph [bgcolor=\"transparent\", size=\"6\", rankdir=\"LR\", splines=\"ortho\"];\n"));
    CHECK(ktbnDotHas(
        dot,
        "node [color=\"#000000\", fillcolor=\"white\", fontcolor=\"black\", style=\"filled\"];\n"));
    // the atemporal variables, then one cluster by slice
    CHECK(ktbnDotHas(dot,
                     "  subgraph \"cluster_atemporal\" {\n"
                     "    label=\"atemporal\";\n"
                     "    style=\"filled\";\n"
                     "    bgcolor=\"lightyellow\";\n"
                     "    \"C\" [label=\"C\"];\n"
                     "  }\n"));
    CHECK(ktbnDotHas(dot,
                     "  subgraph \"cluster_0\" {\n"
                     "    label=\"Time slice 0\";\n"
                     "    style=\"filled\";\n"
                     "    bgcolor=\"#DDDDDD\";\n"
                     "    \"X[0]\" [label=\"X\"];\n"
                     "  }\n"));
    CHECK(ktbnDotHas(dot, "  edge [color=\"black\", constraint=\"false\"];\n"));
    CHECK(ktbnDotHas(dot, "  \"X[0]\" -> \"X[1]\";\n"));
    CHECK(ktbnDotHas(dot, "  \"C\" -> \"X[1]\";\n"));
    // the invisible chain of the slices
    CHECK(ktbnDotHas(dot,
                     "  edge [style=\"invis\", constraint=\"true\"];\n  \"X[0]\" -> \"X[1]\";\n"));
    CHECK_EQ(m.toUnrolledDot(2, false, gum::dot::DotOptions()).find("cluster_1")
                 != std::string::npos,
             true);
  }

  GUM_TEST(ColorsOfTheClustersAreInTheConfiguration) {
    ConfigurationGuard guard;
    const auto         m = ktbnForDot();

    gum::config().set("dynamicBN", "graphSize", "9");
    gum::config().set("dynamicBN", "timeSliceBgColor", "#EEEEEE");
    gum::config().set("dynamicBN", "replicatedSliceBgColor", "pink");
    gum::config().set("dynamicBN", "atemporalBgColor", "ivory");

    const auto dot = m.toUnrolledDot(3, true);
    CHECK(ktbnDotHas(dot, "size=\"9\""));
    CHECK(ktbnDotHas(dot, "bgcolor=\"ivory\""));
    CHECK(ktbnDotHas(dot,
                     "\"cluster_0\" {\n    label=\"Time slice 0\";\n    style=\"filled\";\n    "
                     "bgcolor=\"#EEEEEE\""));
    CHECK(ktbnDotHas(dot,
                     "\"cluster_1\" {\n    label=\"Time slice 1\";\n    style=\"filled\";\n    "
                     "bgcolor=\"#EEEEEE\""));
    CHECK(ktbnDotHas(dot,
                     "\"cluster_2\" {\n    label=\"Time slice 2\";\n    style=\"filled\";\n    "
                     "bgcolor=\"pink\""));
    // the replicated slices are not highlighted unless it is asked
    CHECK_FALSE(ktbnDotHas(m.toUnrolledDot(3), "pink"));
  }

  GUM_TEST(OptionsOfTheKTBN) {
    ConfigurationGuard guard;
    const auto         m = ktbnForDot();

    // the nodes are the ones of the unrolled BayesNet
    const auto           bn = m.unroll(3);
    gum::dot::DotOptions options;
    options.setSize("12!");
    options.setNodeBgColor(bn.idFromName("C"), "red");
    options.setArcColor(bn.idFromName("C"), bn.idFromName("X[1]"), "blue");

    const auto dot = m.toUnrolledDot(3, false, options);
    CHECK(ktbnDotHas(dot, "size=\"12!\""));
    CHECK(ktbnDotHas(dot, "\"C\" [label=\"C\", fillcolor=\"red\", fontcolor=\"white\"];\n"));
    CHECK(ktbnDotHas(dot, "\"C\" -> \"X[1]\" [color=\"blue\"];\n"));
    CHECK_THROWS_AS(m.toUnrolledDot(1, false, options), const gum::OperationNotAllowed&);
  }

}   // namespace gum_tests
