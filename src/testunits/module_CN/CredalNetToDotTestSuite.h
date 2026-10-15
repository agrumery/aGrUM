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
#include <agrum/BN/BayesNet.h>
#include <agrum/CN/credalNet.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The credal networks are drawn as pyAgrum drew them : heptagons, with the colors of the section
  // credalnet.
  struct CredalNetToDotTestSuite {};

  namespace {
    bool cnDotHas(const std::string& dot, const std::string& text) {
      return dot.find(text) != std::string::npos;
    }

    gum::credal::CredalNet< double > cnForDot() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B");
      for (const auto node: bn.nodes())
        bn.cpt(node).translate(1).scale(0.25).normalizeAsCPT();

      auto bmin = gum::BayesNet< double >(bn);
      auto bmax = gum::BayesNet< double >(bn);
      for (const auto node: bn.nodes()) {
        bmin.cpt(node).scale(0.9);
        bmax.cpt(node).scale(1.1);
      }
      gum::credal::CredalNet< double > cn(bmin, bmax);
      cn.intervalToCredal();
      return cn;
    }
  }   // namespace

  GUM_TEST(CredalNetLook) {
    ConfigurationGuard guard;
    const auto         cn  = cnForDot();
    const auto         dot = cn.toDot();

    CHECK(cnDotHas(dot, "digraph {\n"));
    CHECK(
        cnDotHas(dot,
                 "graph [bgcolor=\"transparent\", size=\"5\", rankdir=\"TB\", layout=\"dot\"];\n"));
    CHECK(cnDotHas(dot,
                   "node [style=\"filled\", fillcolor=\"#404040\", fontcolor=\"white\", "
                   "color=\"#4A4A4A\"];\n"));
    CHECK(cnDotHas(
        dot,
        "  \"A\" [shape=\"polygon\", sides=\"7\", peripheries=\"1\", tooltip=\"(0) A\"];\n"));
    CHECK(cnDotHas(
        dot,
        "  \"B\" [shape=\"polygon\", sides=\"7\", peripheries=\"1\", tooltip=\"(1) B\"];\n"));
    CHECK(cnDotHas(dot, "  \"A\" -> \"B\" [fontsize=\"10\", tooltip=\"0&nbsp;&rarr;&nbsp;1\"];\n"));
    CHECK_EQ(cn.toDot(gum::dot::DotOptions()), dot);
  }

  GUM_TEST(CredalNetColorsAreTheOnesOfItsSection) {
    ConfigurationGuard guard;
    const auto         cn = cnForDot();

    gum::config().set("credalnet", "nodeBgColor", "burlywood");
    gum::config().set("credalnet", "nodeFgColor", "adaptive");
    gum::config().set("notebook", "nodeBgColor", "yellow");   // not the one of the credal networks
    const auto dot = cn.toDot();
    CHECK(cnDotHas(dot, "node [style=\"filled\", fillcolor=\"burlywood\", fontcolor=\"black\""));
    CHECK_FALSE(cnDotHas(dot, "yellow"));
  }

  GUM_TEST(CredalNetOptions) {
    ConfigurationGuard guard;
    const auto         cn = cnForDot();

    gum::dot::DotOptions options;
    options.setSize("8");
    options.setNodeBgColor(0, "red");
    options.setArcWidth(0, 1, 2);
    options.setArcLabel(0, 1, "weight");

    const auto dot = cn.toDot(options);
    CHECK(cnDotHas(dot, "size=\"8\""));
    CHECK(cnDotHas(dot,
                   "\"A\" [shape=\"polygon\", sides=\"7\", peripheries=\"1\", tooltip=\"(0) A\", "
                   "fillcolor=\"red\""));
    CHECK(cnDotHas(dot,
                   "\"A\" -> \"B\" [fontsize=\"10\", tooltip=\"0&nbsp;&rarr;&nbsp;1\", "
                   "penwidth=\"2\", label=\"weight\"]"));
  }

}   // namespace gum_tests
