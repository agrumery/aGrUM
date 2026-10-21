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


#include <iostream>
#include <string>

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/variableElimination.h>

#include <agrum/base/core/math/math_utils.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  struct MarginalTargetedInferenceTestSuite {
    public:
    // namespace gum_tests
  };

  GUM_TEST(AddTarget) {
    auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D;A->E->D;F->B;C->H;");

    gum::LazyPropagation< double > lazy(&bn);
    CHECK_EQ(lazy.targets(), gum::NodeSet({0, 1, 2, 3, 4, 5, 6}));
    lazy.addTarget("A");
    CHECK_EQ(lazy.targets(), gum::NodeSet({0}));
    lazy.addTarget("B");
    CHECK_EQ(lazy.targets(), gum::NodeSet({0, 1}));

    gum::ShaferShenoyInference< double > shafer(&bn);
    CHECK_EQ(shafer.targets(), gum::NodeSet({0, 1, 2, 3, 4, 5, 6}));
    shafer.addTarget("A");
    CHECK_EQ(shafer.targets(), gum::NodeSet({0}));
    shafer.addTarget("B");
    CHECK_EQ(shafer.targets(), gum::NodeSet({0, 1}));

    gum::VariableElimination< double > ve(&bn);
    CHECK_EQ(ve.targets(), gum::NodeSet({0, 1, 2, 3, 4, 5, 6}));
    ve.addTarget("A");
    CHECK_EQ(ve.targets(), gum::NodeSet({0}));
    ve.addTarget("B");
    CHECK_EQ(ve.targets(), gum::NodeSet({0, 1}));
  }

  GUM_TEST(ImpactsKeepTheTargetsAndTheEvidence) {
    // evidenceImpact, evidenceJointImpact and jointMutualInformation run their own inferences: the
    // targets and the evidence of the user must be restored afterwards
    auto                           bn = gum::BayesNet< double >::fastPrototype("A->B->C;A->D");
    gum::LazyPropagation< double > ie(&bn);
    ie.addTarget("A");
    ie.addJointTarget(bn.nodeset({"A", "D"}));
    ie.addEvidence("C", 1);
    ie.addEvidence("D", std::vector< double >{0.2, 0.6});
    ie.makeInference();
    const auto posterior = gum::Tensor< double >(ie.posterior("A"));

    auto check = [&]() {
      CHECK_EQ(ie.targets(), gum::NodeSet{bn.idFromName("A")});
      CHECK(ie.isJointTarget(bn.nodeset({"A", "D"})));
      CHECK_EQ(ie.hardEvidenceNodes(), gum::NodeSet{bn.idFromName("C")});
      CHECK_EQ(ie.softEvidenceNodes(), gum::NodeSet{bn.idFromName("D")});
      GUM_CHECK_TENSOR_ALMOST_EQUALS(ie.posterior("A"), posterior);
    };

    ie.evidenceImpact("A", std::vector< std::string >{"B"});
    check();
    ie.evidenceJointImpact(std::vector< std::string >{"A", "D"}, std::vector< std::string >{"B"});
    check();
    ie.jointMutualInformation(bn.nodeset({"A", "B"}));
    check();

    // without any target given by the user, all the nodes remain targets
    gum::LazyPropagation< double > ie2(&bn);
    ie2.evidenceImpact("A", std::vector< std::string >{"B"});
    CHECK_EQ(ie2.targets(), bn.nodes().asNodeSet());
    CHECK(ie2.evidence().empty());
  }
}   // namespace gum_tests
