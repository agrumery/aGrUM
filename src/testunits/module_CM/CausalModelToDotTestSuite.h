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
#include <agrum/CM/causalModel.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The causal models are drawn with the keys of the section causal : the colors of the observed
  // and of the latent variables, and the display of the names of the latent variables.
  struct CausalModelToDotTestSuite {};

  namespace {
    bool cmDotHas(const std::string& dot, const std::string& text) {
      return dot.find(text) != std::string::npos;
    }

    gum::CausalModel< double > cmWithLatent() {
      auto                        bn = gum::BayesNet< double >::fastPrototype("A->B->C;A->C");
      gum::LatentDescriptorVector descs;
      descs.push_back(gum::LatentDescriptorIds("L", {bn.idFromName("A"), bn.idFromName("C")}));
      return gum::CausalModel< double >(bn, descs);
    }
  }   // namespace

  GUM_TEST(CausalModelLook) {
    ConfigurationGuard guard;
    const auto         cm  = cmWithLatent();
    const auto         dot = cm.toDot();

    CHECK(cmDotHas(
        dot,
        "graph [bgcolor=\"transparent\", size=\"2.5\", rankdir=\"TB\", layout=\"dot\"];\n"));
    // an observed variable
    CHECK(cmDotHas(dot, "  \"B\" [tooltip=\"(1) B\"];\n"));
    // a latent variable : a point (the names of the latent variables are not shown), with the
    // colors of the latent
    CHECK(
        cmDotHas(dot,
                 "\"L\" [fillcolor=\"#A08080\", fontcolor=\"white\", shape=\"point\", tooltip=\""));
    // the arcs of the observed variables are drawn with two lines, the arcs of a latent variable
    // are dashed
    CHECK(cmDotHas(dot, "\"A\" -> \"B\" [color=\"#4A4A4A:#4A4A4A\"];\n"));
    CHECK(cmDotHas(dot, "\"L\" -> \"A\" [style=\"dashed\"];\n"));
    CHECK(cmDotHas(dot, "\"L\" -> \"C\" [style=\"dashed\"];\n"));
    CHECK_EQ(cm.toDot(gum::dot::DotOptions()), dot);
  }

  GUM_TEST(KeysOfTheCausalSectionAreRead) {
    ConfigurationGuard guard;
    const auto         cm = cmWithLatent();

    gum::config().set("causal", "graphSize", "1.9");
    gum::config().set("causal", "nodeBgColor", "#101010");
    gum::config().set("causal", "nodeFgColor", "adaptive");
    gum::config().set("causal", "latentBgColor", "yellow");
    gum::config().set("causal", "latentFgColor", "red");
    gum::config().set("causal", "showLatentNames", "true");
    gum::config().set("notebook", "arcColor", "#AAAAAA");

    const auto dot = cm.toDot();
    CHECK(cmDotHas(dot, "size=\"1.9\""));
    CHECK(cmDotHas(dot,
                   "node [style=\"filled\", fillcolor=\"#101010\", fontcolor=\"white\", "
                   "color=\"#AAAAAA\"];\n"));
    CHECK(cmDotHas(dot, "\"L\" [fillcolor=\"yellow\", fontcolor=\"red\", shape=\"ellipse\""));
    CHECK(cmDotHas(dot, "\"A\" -> \"B\" [color=\"#AAAAAA:#AAAAAA\"];\n"));
  }

  GUM_TEST(CausalModelOptions) {
    ConfigurationGuard guard;
    const auto         cm = cmWithLatent();

    gum::dot::DotOptions options;
    options.setNodeBgColor(1, "burlywood");
    options.setArcColor(1, 2, "green");
    const auto dot = cm.toDot(options);
    CHECK(cmDotHas(dot,
                   "\"B\" [tooltip=\"(1) B\", fillcolor=\"burlywood\", fontcolor=\"black\"];\n"));
    CHECK(cmDotHas(dot, "\"B\" -> \"C\" [color=\"green\"];\n"));
    // the other arcs keep their style
    CHECK(cmDotHas(dot, "\"A\" -> \"B\" [color=\"#4A4A4A:#4A4A4A\"];\n"));
  }

  GUM_TEST(ValuesGivenByPositionStillWork) {
    ConfigurationGuard guard;
    const auto         cm = cmWithLatent();

    // the old call, with its defaults : the names of the latent variables are not shown
    CHECK(cmDotHas(cm.toDot(false), "shape=\"point\""));
    CHECK(cmDotHas(cm.toDot(true), "shape=\"ellipse\""));

    // the values given by position are used for all the nodes
    const auto dot = cm.toDot(true, "#123456", "pink", "#654321");
    CHECK(cmDotHas(dot, "\"L\" [fillcolor=\"#123456\", fontcolor=\"pink\", shape=\"ellipse\""));
    CHECK(cmDotHas(dot, "\"B\" [tooltip=\"(1) B\"];\n"));
    CHECK(cmDotHas(dot, "fillcolor=\"#123456\", fontcolor=\"pink\", color=\"#654321\""));
    CHECK(cmDotHas(dot, "\"A\" -> \"B\" [color=\"#654321:#654321\"];\n"));
  }

}   // namespace gum_tests
