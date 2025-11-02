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
/**
 * @file
 * @brief d-separation test suite.
 */
#pragma once

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/agrum.h>

#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/set.h>
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/undiGraph.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/CM/tools/separation.h>

namespace gum_tests {

  class GUM_TEST_SUITE(DSeparation) {
    private:
    using BN      = gum::BayesNet< double >;
    using NodeId  = gum::NodeId;
    using NodeSet = gum::NodeSet;

    // Helper to make a NodeSet from names
    static NodeSet ns(const BN& bn, std::initializer_list< const char* > names) {
      NodeSet s;
      for (auto n: names)
        s.insert(bn.idFromName(n));
      return s;
    }

    public:
    /* Graph used in many Pyagrum tests:

         A   B
          \ / \
        H  C   D
         \ |   |
           E   |
            \ /
             F
             |
             G
    */

    static BN makeRefBN() { return BN::fastPrototype("A->C->E->F->G;F<-D<-B->C;H->E"); }

    // -------------------------------------------------------------------
    // General d-separation checks
    // -------------------------------------------------------------------
    GUM_ACTIVE_TEST(test_general_dsep_basic_paths) {
      auto  bn = makeRefBN();
      auto& dg = bn.dag();

      auto A = ns(bn, {"A"});
      auto B = ns(bn, {"B"});
      auto C = ns(bn, {"C"});
      auto E = ns(bn, {"E"});
      auto F = ns(bn, {"F"});
      auto G = ns(bn, {"G"});

      // A -> C -> E -> F -> G  (unblocked)
      TS_ASSERT(!gum::Separation::isDSeparated(dg, A, G, NodeSet{}));

      // Conditioning on F blocks all paths A->...->G
      TS_ASSERT(gum::Separation::isDSeparated(dg, A, G, F));

      // A ⟂ B (collider at C) without conditioning
      TS_ASSERT(gum::Separation::isDSeparated(dg, A, B, NodeSet{}));

      // Conditioning on the collider C opens A—C—B
      TS_ASSERT(!gum::Separation::isDSeparated(dg, A, B, C));

      // Conditioning downstream of collider (E) also opens via descendant
      TS_ASSERT(!gum::Separation::isDSeparated(dg, A, B, E));

      // Conditioning on F breaks path to G again (C is already in Z)
      TS_ASSERT(gum::Separation::isDSeparated(dg, A, G, ns(bn, {"C", "F"})));
    }

    // -------------------------------------------------------------------
    // Restricted variants: backdoor / forward
    // -------------------------------------------------------------------
    GUM_ACTIVE_TEST(test_backdoor_forward_simple_chain) {
      // A -> B -> C
      auto  bn = BN::fastPrototype("A->B->C");
      auto& dg = bn.dag();

      auto A = ns(bn, {"A"});
      auto C = ns(bn, {"C"});

      // From A (cause) to C (effect): there are NO backdoor paths into A
      TS_ASSERT(gum::Separation::isBackdoorSeparated(dg, A, C, NodeSet{}));

      // From C (cause) to A (effect): backdoor path C<-B<-A exists (unblocked)
      TS_ASSERT(!gum::Separation::isBackdoorSeparated(dg, C, A, NodeSet{}));

      // Forward restriction: from A to C, forward paths exist unless we condition on B
      TS_ASSERT(!gum::Separation::isForwardSeparated(dg, A, C, NodeSet{}));
      TS_ASSERT(gum::Separation::isForwardSeparated(dg, A, C, ns(bn, {"B"})));
    }

    GUM_ACTIVE_TEST(test_backdoor_examples_from_pyagrum_suite) {
      using BN      = gum::BayesNet< double >;
      using NodeSet = gum::NodeSet;

      auto mkNS = [](const BN& bn, std::initializer_list< const char* > names) {
        NodeSet s;
        for (auto n: names)
          s.insert(bn.idFromName(n));
        return s;
      };

      // Helpers that reproduce the Python semantics:
      // hasNoBackDoor(graph, X, Y)  -> backdoor separated with Z = ∅
      auto hasNoBackDoor = [&](const std::string& spec, const char* X, const char* Y) {
        BN          bn  = BN::fastPrototype(spec);
        const auto& dag = bn.dag();
        auto        Xs  = mkNS(bn, {X});
        auto        Ys  = mkNS(bn, {Y});
        TS_ASSERT(gum::Separation::isBackdoorSeparated(dag, Xs, Ys, NodeSet{}));
      };

      // hasBackDoor(graph, X, Y)    -> NOT backdoor separated with Z = ∅
      auto hasBackDoor = [&](const std::string& spec, const char* X, const char* Y) {
        BN          bn  = BN::fastPrototype(spec);
        const auto& dag = bn.dag();
        auto        Xs  = mkNS(bn, {X});
        auto        Ys  = mkNS(bn, {Y});
        TS_ASSERT(!gum::Separation::isBackdoorSeparated(dag, Xs, Ys, NodeSet{}));
      };

      // hasAllBackDoors(expectedSets, graph, X, Y) -> every Z in expectedSets backdoor-separates X
      // and Y
      auto hasAllBackDoors = [&](std::initializer_list< std::initializer_list< const char* > > sets,
                                 const std::string&                                            spec,
                                 const char*                                                   X,
                                 const char*                                                   Y) {
        BN          bn  = BN::fastPrototype(spec);
        const auto& dag = bn.dag();
        auto        Xs  = mkNS(bn, {X});
        auto        Ys  = mkNS(bn, {Y});
        for (const auto& znames: sets) {
          NodeSet Z;
          for (auto n: znames)
            Z.insert(bn.idFromName(n));
          TS_ASSERT(gum::Separation::isBackdoorSeparated(dag, Xs, Ys, Z));
        }
      };

      // ---- Tests copied from the Python suite ----
      hasNoBackDoor("A->B->C", "A", "C");
      hasBackDoor("A->B->C", "C", "A");

      hasNoBackDoor("N0<-N1->N2;N0<-N3->N2;N0<-N4->N2;N2->N0;N1->N4", "N1", "N0");

      hasAllBackDoors(
          {{"N5"}, {"N6"}},
          "N0[1,3]<-N1->N2[1,4];N0<-N3[0,3]->N2;N0<-N4[1,4]->N2;N2->N0;N1->N4;N1<-N5->N6->N0",
          "N1",
          "N0");

      hasNoBackDoor(
          "N0[1,3]<-N1->N2[1,4];N0<-N3[0,3]->N2;N0<-N4[1,4]->N2;N2->N0;N1->N4;N1<-N5->N6<-N0",
          "N1",
          "N0");

      hasBackDoor("Xi<-X3<-X1->X4<-X2->X5->Xj<-X6<-Xi<-X4->Xj", "Xi", "Xj");

      hasAllBackDoors({{"X3", "X4"}, {"X1", "X4"}, {"X4", "X2"}, {"X4", "X5"}},
                      "Xi<-X3<-X1->X4<-X2->X5->Xj<-X6<-Xi<-X4->Xj",
                      "Xi",
                      "Xj");
    }

    // -------------------------------------------------------------------
    // Reduction & barren nodes
    // -------------------------------------------------------------------
    GUM_ACTIVE_TEST(test_barren_nodes_and_reduction) {
      using BN = gum::BayesNet< double >;
      auto  bn = BN::fastPrototype("A->B->C; X->Y; U->V; C->W; T");
      auto& dg = bn.dag();

      const auto idA = bn.idFromName("A");
      const auto idB = bn.idFromName("B");
      const auto idC = bn.idFromName("C");
      const auto idX = bn.idFromName("X");
      const auto idY = bn.idFromName("Y");
      const auto idU = bn.idFromName("U");
      const auto idV = bn.idFromName("V");
      const auto idW = bn.idFromName("W");
      const auto idT = bn.idFromName("T");

      // Interest (targets) = {A, C}, Evidence = ∅
      gum::NodeSet targets;
      targets.insert(idA);
      targets.insert(idC);
      gum::NodeSet evidence;   // empty

      // X->Y, U->V, T, and W (descendant of C) are barren w.r.t. {A,C}
      auto barren = gum::Separation::findBarrenNodes(dg, evidence, targets);
      TS_ASSERT(barren.exists(idX));
      TS_ASSERT(barren.exists(idY));
      TS_ASSERT(barren.exists(idU));
      TS_ASSERT(barren.exists(idV));
      TS_ASSERT(barren.exists(idT));
      TS_ASSERT(barren.exists(idW));   // descendant of C ⇒ cannot influence {A,C}

      // Reduction should keep {A,B,C} and remove {X,Y,U,V,W,T}
      gum::NodeSet Xset;
      Xset.insert(idA);
      gum::NodeSet Yset;
      Yset.insert(idC);
      auto reduced = gum::Separation::reduceForDSeparation(dg, Xset, Yset, evidence);

      TS_ASSERT(reduced.existsNode(idA));
      TS_ASSERT(reduced.existsNode(idB));
      TS_ASSERT(reduced.existsNode(idC));

      TS_ASSERT(!reduced.existsNode(idX));
      TS_ASSERT(!reduced.existsNode(idY));
      TS_ASSERT(!reduced.existsNode(idU));
      TS_ASSERT(!reduced.existsNode(idV));
      TS_ASSERT(!reduced.existsNode(idW));
      TS_ASSERT(!reduced.existsNode(idT));
    }

    // -------------------------------------------------------------------
    // Cross-check: general d-sep agrees with DAG::dSeparation
    // -------------------------------------------------------------------
    GUM_ACTIVE_TEST(test_agreement_with_DAG_API) {
      auto        bn  = makeRefBN();
      const auto& dag = bn.dag();

      auto A = ns(bn, {"A"});
      auto B = ns(bn, {"B"});
      auto C = ns(bn, {"C"});

      TS_ASSERT_EQUALS(gum::Separation::isDSeparated(dag, A, B, gum::NodeSet{}),
                       dag.dSeparation(A, B, gum::NodeSet{}));

      TS_ASSERT_EQUALS(gum::Separation::isDSeparated(dag, A, B, C), dag.dSeparation(A, B, C));
    }

    GUM_ACTIVE_TEST(test_dsep_confounder_conditioning_blocks) {
      // Confounding only (no direct X->Y path): U->X, U->Y
      auto        bn  = BN::fastPrototype("U->X;U->Y");
      const auto& dag = bn.dag();

      auto X = ns(bn, {"X"});
      auto Y = ns(bn, {"Y"});
      auto U = ns(bn, {"U"});

      // Without conditioning: NOT d-separated (path X <- U -> Y is open)
      TS_ASSERT(!gum::Separation::isDSeparated(dag, X, Y, NodeSet{}));

      // Conditioning on the confounder blocks that backdoor path
      TS_ASSERT(gum::Separation::isDSeparated(dag, X, Y, U));
    }
  };

}   // namespace gum_tests
