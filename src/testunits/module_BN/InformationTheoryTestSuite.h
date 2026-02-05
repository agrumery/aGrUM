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
#include <vector>

#include <agrum/base/graphicalModels/algorithms/informationTheory.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/MRF/inference/ShaferShenoyMRFInference.h>
#include <agrum/MRF/MarkovRandomField.h>

#include "agrum/base/multidim/tensor.h"
#include "testunits/gumtest/AgrumTestSuite.h"
#include "testunits/gumtest/utils.h"

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  InformationTheory
#define GUM_CURRENT_MODULE BN

namespace gum_tests {
  struct InformationTheoryTestSuite {
    public:
    static void testConstructor1() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");

      gum::LazyPropagation ie(&bn);

      gum::NodeSet X   = bn.nodeset({"A", "C"});
      gum::NodeSet Y   = bn.nodeset({"B"});
      auto         it  = gum::InformationTheory(ie, X, Y);
      auto         it2 = gum::InformationTheory(ie, {"A", "C"}, {"B"});
      CHECK((it.entropyX()) == (it2.entropyX()));
      CHECK((it.entropyY()) == (it2.entropyY()));
    }   // namespace gum_tests

    static void testCheckSimpleBN() {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B");
      bn.cpt("A").fillWith({0.8, 0.2});
      bn.cpt("B").fillWith({0.1, 0.9, 0.3, 0.7});

      gum::LazyPropagation ie(&bn);

      auto it = gum::InformationTheory(ie, {"A"}, {"B"});

      CHECK(fabs((it.entropyXY()) - (1.27338275)) < 1e-5);
      CHECK(fabs((it.entropyX()) - (0.72192809)) < 1e-5);
      CHECK(fabs((it.entropyY()) - (0.58423881)) < 1e-5);
      CHECK(fabs((it.entropyXgivenY()) - (0.68914394)) < 1e-5);
      CHECK(fabs((it.mutualInformationXY()) - (0.03278416)) < 1e-5);
    }

    static void testCheckSimpleBNwithEvidence() {
      const auto bn = gum::BayesNet< double >::fastPrototype("C->A->B");
      bn.cpt("C").fillWith({0.5, 0.5});
      bn.cpt("A").fillWith(
          {0.8, 0.2, 0.2, 0.8});   // so that bn|ev={"C"=0} is the same as the last test
      bn.cpt("B").fillWith({0.1, 0.9, 0.3, 0.7});

      gum::LazyPropagation ie(&bn);
      ie.addEvidence("C", 0);
      {
        auto it = gum::InformationTheory(ie, {"A"}, {"B"});

        CHECK(fabs((it.entropyXY()) - (1.27338275)) < 1e-5);
        CHECK(fabs((it.entropyX()) - (0.72192809)) < 1e-5);
        CHECK(fabs((it.entropyY()) - (0.58423881)) < 1e-5);
        CHECK(fabs((it.entropyXgivenY()) - (0.68914394)) < 1e-5);
        CHECK(fabs((it.entropyYgivenX()) - (0.551455)) < 1e-5);
        CHECK(fabs((it.mutualInformationXY()) - (0.03278416)) < 1e-5);
      }
      ie.chgEvidence("C", 1);
      {
        auto it = gum::InformationTheory(ie, {"A"}, {"B"});

        CHECK(fabs((it.entropyXY()) - (1.52075993298977)) < 1e-5);
        CHECK(fabs((it.entropyX()) - (0.72192809)) < 1e-5);
        CHECK(fabs((it.entropyY()) - (0.826746372492618)) < 1e-5);
        CHECK(fabs((it.entropyXgivenY()) - (0.694013560497155)) < 1e-5);
        CHECK(fabs((it.entropyYgivenX()) - (0.798832)) < 1e-5);
        CHECK(fabs((it.mutualInformationXY()) - (0.0279145343902076)) < 1e-5);
      }
    }

    static void testShafShenCheckSimpleBNwithEvidence() {
      const auto bn = gum::BayesNet< double >::fastPrototype("C->A->B");
      bn.cpt("C").fillWith({0.5, 0.5});
      bn.cpt("A").fillWith(
          {0.8, 0.2, 0.2, 0.8});   // so that bn|ev={"C"=0} is the same as the last test
      bn.cpt("B").fillWith({0.1, 0.9, 0.3, 0.7});

      gum::ShaferShenoyInference ie(&bn);
      ie.addEvidence("C", 0);
      {
        auto it = gum::InformationTheory(ie, {"A"}, {"B"});

        CHECK(fabs((it.entropyXY()) - (1.27338275)) < 1e-5);
        CHECK(fabs((it.entropyX()) - (0.72192809)) < 1e-5);
        CHECK(fabs((it.entropyY()) - (0.58423881)) < 1e-5);
        CHECK(fabs((it.entropyXgivenY()) - (0.68914394)) < 1e-5);
        CHECK(fabs((it.entropyYgivenX()) - (0.551455)) < 1e-5);
        CHECK(fabs((it.mutualInformationXY()) - (0.03278416)) < 1e-5);
      }
      ie.chgEvidence("C", 1);
      {
        auto it = gum::InformationTheory(ie, {"A"}, {"B"});

        CHECK(fabs((it.entropyXY()) - (1.52075993298977)) < 1e-5);
        CHECK(fabs((it.entropyX()) - (0.72192809)) < 1e-5);
        CHECK(fabs((it.entropyY()) - (0.826746372492618)) < 1e-5);
        CHECK(fabs((it.entropyXgivenY()) - (0.694013560497155)) < 1e-5);
        CHECK(fabs((it.entropyYgivenX()) - (0.798832)) < 1e-5);
        CHECK(fabs((it.mutualInformationXY()) - (0.0279145343902076)) < 1e-5);
      }
    }

    static void testCheckConsistency() {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");

      gum::LazyPropagation ie(&bn);

      auto it = gum::InformationTheory(ie, {"A", "C"}, {"B"});
      check_this_information_theoryXY(it);
    }

    static void testShafShenCheckConsistency() {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");

      gum::ShaferShenoyInference ie(&bn);

      auto it = gum::InformationTheory(ie, {"A", "C"}, {"B"});
      check_this_information_theoryXY(it);
    }

    static void testMRFCheckConsistency() {
      const auto mrf = gum::MarkovRandomField< double >::fastPrototype("A--B--C;A--B");

      gum::ShaferShenoyMRFInference ie(&mrf);

      auto it = gum::InformationTheory(ie, {"A", "C"}, {"B"});
      check_this_information_theoryXY(it);
    }

    static void testCheckConsistency3points() {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D->E");

      gum::LazyPropagation ie(&bn);

      auto it = gum::InformationTheory(ie, {"A", "E"}, {"B"}, {"C", "D"});
      check_this_information_theoryXY(it);
      check_this_information_theoryXYZ(it);
    }

    static void testShafShenCheckConsistency3points() {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D->E");

      gum::ShaferShenoyInference ie(&bn);

      auto it = gum::InformationTheory(ie, {"A", "E"}, {"B"}, {"C", "D"});
      check_this_information_theoryXY(it);
      check_this_information_theoryXYZ(it);
    }

    static void testMRFCheckConsistency3points() {
      const auto mrf = gum::MarkovRandomField< double >::fastPrototype("A--B--C;A--D--E--B");

      gum::ShaferShenoyMRFInference ie(&mrf);

      auto it = gum::InformationTheory(ie, {"A", "E"}, {"B"}, {"C", "D"});
      check_this_information_theoryXY(it);
      check_this_information_theoryXYZ(it);
    }

    static void testCheckCrossInferenceConsistency() {
      std::string             file = GET_RESSOURCES_PATH("bif/alarm.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);

      auto lazy = gum::LazyPropagation(&bn);
      auto shaf = gum::ShaferShenoyInference(&bn);
      for (const auto& arc: bn.arcs()) {
        gum::InformationTheory itlazy(lazy, gum::NodeSet{arc.first()}, gum::NodeSet{arc.second()});
        gum::InformationTheory itshaf(shaf, gum::NodeSet{arc.first()}, gum::NodeSet{arc.second()});
        CHECK(fabs((itlazy.mutualInformationXY()) - (itshaf.mutualInformationXY())) < 1e-10);
      }
    }

    static void testcheckMutalInformationOnArc() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D;C->E;D->F;C<-G->H->E", 4);
      gum::Tensor< double > joint;
      for (auto n: bn.nodes())
        joint *= bn.cpt(n);

      auto mi = [&](gum::NodeId x, gum::NodeId y) -> double {
        using VarSet   = gum::VariableSet;
        const auto vx  = &bn.variable(x);
        const auto vy  = &bn.variable(y);
        const auto hxy = joint.sumIn(VarSet{vx, vy}).entropy();
        const auto hx  = joint.sumIn(VarSet{vx}).entropy();
        const auto hy  = joint.sumIn(VarSet{vy}).entropy();
        return hx + hy - hxy;
      };
      auto h = [&](gum::NodeId x) -> double {
        using VarSet  = gum::VariableSet;
        const auto vx = &bn.variable(x);
        return joint.sumIn(VarSet{vx}).entropy();
      };

      gum::LazyPropagation lazy(&bn);

      for (const auto& node: bn.nodes()) {
        GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(lazy.posterior(node),
                                                joint.sumIn({&bn.variable(node)}));
      }
      for (const auto& arc: bn.arcs()) {
        gum::InformationTheory< gum::LazyPropagation, double > it(lazy,
                                                                  gum::NodeSet{arc.first()},
                                                                  gum::NodeSet{arc.second()});
        CHECK(fabs((mi(arc.first(), arc.second())) - (it.mutualInformationXY())) < 1e-10);
      }
      for (const auto& node: bn.nodes()) {
        gum::InformationTheory< gum::LazyPropagation, double > it(lazy,
                                                                  gum::NodeSet{node},
                                                                  gum::NodeSet{});
        CHECK(fabs((h(node)) - (it.entropyX())) < 1e-10);
      }
    }

    static void testcheckMutalInformationOnArcWithEvidence() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D;C->E;D->F;C<-G->H->E", 4);
      gum::Tensor< double > joint;
      for (auto n: bn.nodes())
        joint *= bn.cpt(n);
      gum::Tensor< double > evc;
      evc.add(bn.variable("C"));
      evc.fillWith({0, 1, 0, 0});
      joint *= evc;
      joint.normalize();

      auto mi = [&](gum::NodeId x, gum::NodeId y) -> double {
        using VarSet   = gum::VariableSet;
        const auto vx  = &bn.variable(x);
        const auto vy  = &bn.variable(y);
        const auto hxy = joint.sumIn(VarSet{vx, vy}).entropy();
        const auto hx  = joint.sumIn(VarSet{vx}).entropy();
        const auto hy  = joint.sumIn(VarSet{vy}).entropy();
        return hx + hy - hxy;
      };
      auto h = [&](gum::NodeId x) -> double {
        using VarSet  = gum::VariableSet;
        const auto vx = &bn.variable(x);
        return joint.sumIn(VarSet{vx}).entropy();
      };

      gum::LazyPropagation lazy(&bn);
      lazy.addEvidence(evc);

      for (const auto& node: bn.nodes()) {
        GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(lazy.posterior(node),
                                                joint.sumIn({&bn.variable(node)}));
      }
      for (const auto& node: bn.nodes()) {
        gum::InformationTheory< gum::LazyPropagation, double > it(lazy,
                                                                  gum::NodeSet{node},
                                                                  gum::NodeSet{});
        CHECK(fabs((h(node)) - (it.entropyX())) < 1e-10);
      }
      for (const auto& arc: bn.arcs()) {
        gum::InformationTheory< gum::LazyPropagation, double > it(lazy,
                                                                  gum::NodeSet{arc.first()},
                                                                  gum::NodeSet{arc.second()});
        CHECK(fabs((mi(arc.first(), arc.second())) - (it.mutualInformationXY())) < 1e-10);
      }
    }

    static void testcheckMutalInformationOnArcWithSoftEvidence() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D;C->E;D->F;C<-G->H->E", 4);
      gum::Tensor< double > joint;
      for (auto n: bn.nodes())
        joint *= bn.cpt(n);
      gum::Tensor< double > evc;
      evc.add(bn.variable("D"));
      evc.fillWith({0, 1, 1, 0});
      joint *= evc;
      joint.normalize();

      auto mi = [&](gum::NodeId x, gum::NodeId y) -> double {
        using VarSet   = gum::VariableSet;
        const auto vx  = &bn.variable(x);
        const auto vy  = &bn.variable(y);
        const auto hxy = joint.sumIn(VarSet{vx, vy}).entropy();
        const auto hx  = joint.sumIn(VarSet{vx}).entropy();
        const auto hy  = joint.sumIn(VarSet{vy}).entropy();
        return hx + hy - hxy;
      };
      auto h = [&](gum::NodeId x) -> double {
        using VarSet  = gum::VariableSet;
        const auto vx = &bn.variable(x);
        return joint.sumIn(VarSet{vx}).entropy();
      };

      gum::LazyPropagation lazy(&bn);
      lazy.addEvidence(evc);

      for (const auto& node: bn.nodes()) {
        GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(lazy.posterior(node),
                                                joint.sumIn({&bn.variable(node)}));
      }
      for (const auto& node: bn.nodes()) {
        gum::InformationTheory< gum::LazyPropagation, double > it(lazy,
                                                                  gum::NodeSet{node},
                                                                  gum::NodeSet{});
        CHECK(fabs((h(node)) - (it.entropyX())) < 1e-10);
      }
      for (const auto& arc: bn.arcs()) {
        gum::InformationTheory< gum::LazyPropagation, double > it(lazy,
                                                                  gum::NodeSet{arc.first()},
                                                                  gum::NodeSet{arc.second()});
        CHECK(fabs((mi(arc.first(), arc.second())) - (it.mutualInformationXY())) < 1e-10);
      }
    }

    private:
    template < class IT >
    static void check_this_information_theoryXY(IT& it) {
      // H(X|Y)=H(X,Y)-H(Y)
      CHECK(fabs((it.entropyXgivenY()) - (it.entropyXY() - it.entropyY())) < 1e-10);

      // H(Y|X)=H(X|Y)-H(X)+H(Y)
      CHECK(fabs((it.entropyYgivenX()) - (it.entropyXgivenY() - it.entropyX() + it.entropyY()))
            < 1e-10);

      // I(X,Y)=H(X)-H(X|Y)=H(Y)-H(Y|X)
      CHECK(fabs((it.mutualInformationXY()) - (it.entropyX() - it.entropyXgivenY())) < 1e-10);
      CHECK(fabs((it.mutualInformationXY()) - (it.entropyY() - it.entropyYgivenX())) < 1e-10);

      // I(X,Y)=H(X)+H(Y)-H(X,Y)
      CHECK(fabs((it.mutualInformationXY()) - (it.entropyX() + it.entropyY() - it.entropyXY()))
            < 1e-10);

      // I(X,Y)=H(X,Y)-H(X|Y)-H(Y|X)
      CHECK(fabs((it.mutualInformationXY())
                 - (it.entropyXY() - it.entropyXgivenY() - it.entropyYgivenX()))
            < 1e-10);

      // V(X,Y)=2H(X,Y)-H(X)-H(Y)
      CHECK(fabs((it.variationOfInformationXY())
                 - (2 * it.entropyXY() - it.entropyXgivenY() - it.entropyYgivenX()))
            < 1e-10);
    }

    template < class IT >
    static void check_this_information_theoryXYZ(IT& it) {
      // H(X|Y);Z=H(X,Y|Z)-H(Y|Z)
      CHECK(fabs((it.entropyXgivenYZ()) - (it.entropyXYgivenZ() - it.entropyYgivenZ())) < 1e-10);

      // I(X,Y|Z)=H(X|Z)+H(Y|Z)-H(X,Y|Z)
      CHECK(fabs((it.mutualInformationXYgivenZ())
                 - (it.entropyXgivenZ() + it.entropyYgivenZ() - it.entropyXYgivenZ()))
            < 1e-10);
    }
  };

  GUM_TEST_ACTIF(Constructor1)
  GUM_TEST_ACTIF(CheckSimpleBN)
  GUM_TEST_ACTIF(CheckSimpleBNwithEvidence)
  GUM_TEST_ACTIF(ShafShenCheckSimpleBNwithEvidence)
  GUM_TEST_ACTIF(CheckConsistency)
  GUM_TEST_ACTIF(ShafShenCheckConsistency)
  GUM_TEST_ACTIF(MRFCheckConsistency)
  GUM_TEST_ACTIF(CheckConsistency3points)
  GUM_TEST_ACTIF(ShafShenCheckConsistency3points)
  GUM_TEST_ACTIF(MRFCheckConsistency3points)
  GUM_TEST_ACTIF(CheckCrossInferenceConsistency)
  GUM_TEST_ACTIF(checkMutalInformationOnArc)
  GUM_TEST_ACTIF(checkMutalInformationOnArcWithEvidence)
  GUM_TEST_ACTIF(checkMutalInformationOnArcWithSoftEvidence)
}   // namespace gum_tests
