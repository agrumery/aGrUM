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


#include <iostream>
#include <string>
#include <vector>

#include <agrum/base/graphicalModels/algorithms/informationTheory.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include "agrum/base/multidim/tensor.h"
#include "testunits/gumtest/AgrumTestSuite.h"
#include "testunits/gumtest/utils.h"

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BNInformationTheory
#define GUM_CURRENT_MODULE BN

namespace gum_tests {
  struct BNInformationTheoryTestSuite {
    public:
    static void testConstructor1() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");

      gum::LazyPropagation ie(&bn);

      gum::NodeSet X   = bn.nodeset({"A", "C"});
      gum::NodeSet Y   = bn.nodeset({"B"});
      auto         it  = gum::InformationTheory(ie, X, Y);
      auto         it2 = gum::InformationTheory(ie, {"A", "C"}, {"B"});
      CHECK_EQ(it.entropyX(), it2.entropyX());
      CHECK_EQ(it.entropyY(), it2.entropyY());
    }

    static void testCheckSimpleBN() {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B");
      bn.cpt("A").fillWith({0.8, 0.2});
      bn.cpt("B").fillWith({0.1, 0.9, 0.3, 0.7});

      gum::LazyPropagation ie(&bn);

      auto it = gum::InformationTheory(ie, {"A"}, {"B"});

      CHECK_LT(fabs((it.entropyXY()) - (1.27338275)), GUM_SMALL_ERROR);
      CHECK_LT(fabs((it.entropyX()) - (0.72192809)), GUM_SMALL_ERROR);
      CHECK_LT(fabs((it.entropyY()) - (0.58423881)), GUM_SMALL_ERROR);
      CHECK_LT(fabs((it.entropyXgivenY()) - (0.68914394)), GUM_SMALL_ERROR);
      CHECK_LT(fabs((it.mutualInformationXY()) - (0.03278416)), GUM_SMALL_ERROR);
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

        CHECK_LT(fabs((it.entropyXY()) - (1.27338275)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyX()) - (0.72192809)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyY()) - (0.58423881)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyXgivenY()) - (0.68914394)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyYgivenX()) - (0.551455)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.mutualInformationXY()) - (0.03278416)), GUM_SMALL_ERROR);
      }
      ie.chgEvidence("C", 1);
      {
        auto it = gum::InformationTheory(ie, {"A"}, {"B"});

        CHECK_LT(fabs((it.entropyXY()) - (1.52075993298977)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyX()) - (0.72192809)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyY()) - (0.826746372492618)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyXgivenY()) - (0.694013560497155)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyYgivenX()) - (0.798832)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.mutualInformationXY()) - (0.0279145343902076)), GUM_SMALL_ERROR);
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

        CHECK_LT(fabs((it.entropyXY()) - (1.27338275)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyX()) - (0.72192809)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyY()) - (0.58423881)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyXgivenY()) - (0.68914394)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyYgivenX()) - (0.551455)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.mutualInformationXY()) - (0.03278416)), GUM_SMALL_ERROR);
      }
      ie.chgEvidence("C", 1);
      {
        auto it = gum::InformationTheory(ie, {"A"}, {"B"});

        CHECK_LT(fabs((it.entropyXY()) - (1.52075993298977)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyX()) - (0.72192809)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyY()) - (0.826746372492618)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyXgivenY()) - (0.694013560497155)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.entropyYgivenX()) - (0.798832)), GUM_SMALL_ERROR);
        CHECK_LT(fabs((it.mutualInformationXY()) - (0.0279145343902076)), GUM_SMALL_ERROR);
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

    static void testCheckCrossInferenceConsistency() {
      std::string             file = GET_RESSOURCES_PATH("bif/alarm.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);

      auto lazy = gum::LazyPropagation(&bn);
      auto shaf = gum::ShaferShenoyInference(&bn);
      for (const auto& arc: bn.arcs()) {
        gum::InformationTheory itlazy(lazy, gum::NodeSet{arc.first()}, gum::NodeSet{arc.second()});
        gum::InformationTheory itshaf(shaf, gum::NodeSet{arc.first()}, gum::NodeSet{arc.second()});
        CHECK_LT(fabs((itlazy.mutualInformationXY()) - (itshaf.mutualInformationXY())),
                 GUM_VERY_SMALL_ERROR);
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
        CHECK_LT(fabs((mi(arc.first(), arc.second())) - (it.mutualInformationXY())),
                 GUM_VERY_SMALL_ERROR);
      }
      for (const auto& node: bn.nodes()) {
        gum::InformationTheory< gum::LazyPropagation, double > it(lazy,
                                                                  gum::NodeSet{node},
                                                                  gum::NodeSet{});
        CHECK_LT(fabs((h(node)) - (it.entropyX())), GUM_VERY_SMALL_ERROR);
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
        CHECK_LT(fabs((h(node)) - (it.entropyX())), GUM_VERY_SMALL_ERROR);
      }
      for (const auto& arc: bn.arcs()) {
        gum::InformationTheory< gum::LazyPropagation, double > it(lazy,
                                                                  gum::NodeSet{arc.first()},
                                                                  gum::NodeSet{arc.second()});
        CHECK_LT(fabs((mi(arc.first(), arc.second())) - (it.mutualInformationXY())),
                 GUM_VERY_SMALL_ERROR);
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
        CHECK_LT(fabs((h(node)) - (it.entropyX())), GUM_VERY_SMALL_ERROR);
      }
      for (const auto& arc: bn.arcs()) {
        gum::InformationTheory< gum::LazyPropagation, double > it(lazy,
                                                                  gum::NodeSet{arc.first()},
                                                                  gum::NodeSet{arc.second()});
        CHECK_LT(fabs((mi(arc.first(), arc.second())) - (it.mutualInformationXY())),
                 GUM_VERY_SMALL_ERROR);
      }
    }

    private:
    template < class IT >
    static void check_this_information_theoryXY(IT& it) {
      // H(X|Y)=H(X,Y)-H(Y)
      CHECK_LT(fabs((it.entropyXgivenY()) - (it.entropyXY() - it.entropyY())),
               GUM_VERY_SMALL_ERROR);

      // H(Y|X)=H(X|Y)-H(X)+H(Y)
      CHECK_LT(fabs((it.entropyYgivenX()) - (it.entropyXgivenY() - it.entropyX() + it.entropyY())),
               GUM_VERY_SMALL_ERROR);

      // I(X,Y)=H(X)-H(X|Y)=H(Y)-H(Y|X)
      CHECK_LT(fabs((it.mutualInformationXY()) - (it.entropyX() - it.entropyXgivenY())),
               GUM_VERY_SMALL_ERROR);
      CHECK_LT(fabs((it.mutualInformationXY()) - (it.entropyY() - it.entropyYgivenX())),
               GUM_VERY_SMALL_ERROR);

      // I(X,Y)=H(X)+H(Y)-H(X,Y)
      CHECK_LT(fabs((it.mutualInformationXY()) - (it.entropyX() + it.entropyY() - it.entropyXY())),
               GUM_VERY_SMALL_ERROR);

      // I(X,Y)=H(X,Y)-H(X|Y)-H(Y|X)
      CHECK_LT(fabs((it.mutualInformationXY())
                    - (it.entropyXY() - it.entropyXgivenY() - it.entropyYgivenX())),
               GUM_VERY_SMALL_ERROR);

      // V(X,Y)=2H(X,Y)-H(X)-H(Y)
      CHECK_LT(fabs((it.variationOfInformationXY())
                    - (2 * it.entropyXY() - it.entropyXgivenY() - it.entropyYgivenX())),
               GUM_VERY_SMALL_ERROR);
    }

    template < class IT >
    static void check_this_information_theoryXYZ(IT& it) {
      // H(X|Y);Z=H(X,Y|Z)-H(Y|Z)
      CHECK_LT(fabs((it.entropyXgivenYZ()) - (it.entropyXYgivenZ() - it.entropyYgivenZ())),
               GUM_VERY_SMALL_ERROR);

      // I(X,Y|Z)=H(X|Z)+H(Y|Z)-H(X,Y|Z)
      CHECK_LT(fabs((it.mutualInformationXYgivenZ())
                    - (it.entropyXgivenZ() + it.entropyYgivenZ() - it.entropyXYgivenZ())),
               GUM_VERY_SMALL_ERROR);
    }
  };

  GUM_TEST_ACTIF(Constructor1)
  GUM_TEST_ACTIF(CheckSimpleBN)
  GUM_TEST_ACTIF(CheckSimpleBNwithEvidence)
  GUM_TEST_ACTIF(ShafShenCheckSimpleBNwithEvidence)
  GUM_TEST_ACTIF(CheckConsistency)
  GUM_TEST_ACTIF(ShafShenCheckConsistency)
  GUM_TEST_ACTIF(CheckConsistency3points)
  GUM_TEST_ACTIF(ShafShenCheckConsistency3points)
  GUM_TEST_ACTIF(CheckCrossInferenceConsistency)
  GUM_TEST_ACTIF(checkMutalInformationOnArc)
  GUM_TEST_ACTIF(checkMutalInformationOnArcWithEvidence)
  GUM_TEST_ACTIF(checkMutalInformationOnArcWithSoftEvidence)
}   // namespace gum_tests
