/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/CM/causalModel.h>

namespace gum_tests {

  class CausalModelTestSuite : public CxxTest::TestSuite {
  public:

    GUM_ACTIVE_TEST(SimpleBN) {
      auto bn = gum::BayesNet<double>::fastPrototype("A[2]->B[3]");
      gum::CausalModel<double> cm(bn);

      // BN has 2 variables
      TS_ASSERT_EQUALS(cm.observedBayesNet().size(), 2u);
      TS_ASSERT(cm.existsArc("A","B"));
      TS_ASSERT(!cm.existsArc("B","A"));
    }

    GUM_ACTIVE_TEST(LatentFromNames) {
      auto bn = gum::BayesNet<double>::fastPrototype("Smoking->Cancer");
      gum::LatentDescriptorList descs;
      descs.pushBack(gum::LatentDescriptor(
        gum::LatentDescriptorNames("Genotype", {"Smoking","Cancer"})));

      gum::CausalModel<double> cm(bn, descs);

      TS_ASSERT(cm.latentVariablesNames().contains("Genotype"));
      TS_ASSERT(cm.existsArc("Genotype","Smoking"));
      TS_ASSERT(cm.existsArc("Genotype","Cancer"));
    }

    GUM_ACTIVE_TEST(LatentFromIds) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Y");
      gum::List<gum::NodeId> childIds;
      childIds.pushBack(bn.idFromName("X"));
      childIds.pushBack(bn.idFromName("Y"));

      gum::CausalModel<double> cm(bn);
      cm.addLatentVariable("Hidden", childIds);

      TS_ASSERT(cm.latentVariablesNames().contains("Hidden"));
      TS_ASSERT(cm.existsArc("Hidden","X"));
      TS_ASSERT(cm.existsArc("Hidden","Y"));
    }

    GUM_ACTIVE_TEST(ToDotSmoke) {
      // Smoke network: Smoking -> Tar -> Cancer; Smoking -> Cancer
      auto bn = gum::BayesNet<double>::fastPrototype("Smoking->Tar->Cancer;Smoking->Cancer");
      gum::LatentDescriptorList descs;
      descs.pushBack(gum::LatentDescriptor(
        gum::LatentDescriptorNames("Genotype", {"Smoking","Cancer"})));

      gum::CausalModel<double> cm(bn, descs);

      auto dot = cm.toDot();
      GUM_TRACE_VAR(std::string("\n") + dot);

      TS_ASSERT(dot.find("Smoking")  != std::string::npos);
      TS_ASSERT(dot.find("Cancer")   != std::string::npos);
      TS_ASSERT(dot.find("Tar")      != std::string::npos);
      TS_ASSERT(dot.find("Genotype") != std::string::npos);

      // Expected arcs
      TS_ASSERT(dot.find("\"Smoking\"->\"Tar\"")    != std::string::npos);
      TS_ASSERT(dot.find("\"Tar\"->\"Cancer\"")     != std::string::npos);
      TS_ASSERT(dot.find("\"Genotype\"->\"Smoking\"") != std::string::npos);
      TS_ASSERT(dot.find("\"Genotype\"->\"Cancer\"")  != std::string::npos);

      // Because keepArcs=false by default, the direct Smoking->Cancer must be gone
      TS_ASSERT(dot.find("\"Smoking\"->\"Cancer\"") == std::string::npos);
    }

    GUM_ACTIVE_TEST(ToDotSimpson) {
      // Simpson network: Gender->Drug->Patient;Gender->Patient
      auto bn = gum::BayesNet<double>::fastPrototype("Gender->Drug->Patient;Gender->Patient");

      gum::CausalModel<double> cm(bn);
      auto dot = cm.toDot();

      GUM_TRACE_VAR(std::string("\n") + dot);
      std::cerr << "\n" << dot << std::endl;

      // Nodes
      TS_ASSERT(dot.find("Gender")  != std::string::npos);
      TS_ASSERT(dot.find("Drug")    != std::string::npos);
      TS_ASSERT(dot.find("Patient") != std::string::npos);

      // Expected arcs
      TS_ASSERT(dot.find("\"Gender\"->\"Drug\"")    != std::string::npos);
      TS_ASSERT(dot.find("\"Gender\"->\"Patient\"") != std::string::npos);
      TS_ASSERT(dot.find("\"Drug\"->\"Patient\"")   != std::string::npos);
    }

  };



}
