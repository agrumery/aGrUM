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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/config.h>

#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNetFragment.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/inference/lazyPropagation.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BayesNetFragment
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//       / \ / /            4 -> 5
//      6   5_/             2 -> 4
//                          2 -> 5
//                          3 -> 6
// all variables are binary except 5 and 6 (ternary)

namespace gum_tests {

  struct BayesNetFragmentTestSuite {
    private:
    static void fill(gum::BayesNet< double >& bn) {
      try {
        auto var1 = gum::LabelizedVariable("v1", "1", 2);
        auto var2 = gum::LabelizedVariable("v2", "2", 2);
        auto var3 = gum::LabelizedVariable("v3", "3", 2);
        auto var4 = gum::LabelizedVariable("v4", "4", 2);
        auto var5 = gum::LabelizedVariable("v5", "(gum::Size) 3", 3);
        auto var6 = gum::LabelizedVariable("v6", "(gum::Size) 3", 3);

        bn.add(var1);
        bn.add(var2);
        bn.add(var3);
        bn.add(var4);
        bn.add(var5);
        bn.add(var6);

        bn.addArc(bn.idFromName("v1"), bn.idFromName("v3"));
        bn.addArc(bn.idFromName("v1"), bn.idFromName("v4"));
        bn.addArc(bn.idFromName("v2"), bn.idFromName("v4"));
        bn.addArc(bn.idFromName("v2"), bn.idFromName("v5"));
        bn.addArc(bn.idFromName("v3"), bn.idFromName("v5"));
        bn.addArc(bn.idFromName("v3"), bn.idFromName("v6"));
        bn.addArc(bn.idFromName("v4"), bn.idFromName("v5"));

      }   // namespace gum_tests

      catch (gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }

      bn.generateCPTs();
    }

    // the same BN but with no 4-->5
    static void fill2(gum::BayesNet< double >& bn, const gum::BayesNet< double >& source) {
      try {
        auto var1 = gum::LabelizedVariable("v1", "1", 2);
        auto var2 = gum::LabelizedVariable("v2", "2", 2);
        auto var3 = gum::LabelizedVariable("v3", "3", 2);
        auto var4 = gum::LabelizedVariable("v4", "4", 2);
        auto var5 = gum::LabelizedVariable("v5", "(gum::Size) 3", 3);
        auto var6 = gum::LabelizedVariable("v6", "(gum::Size) 3", 3);

        bn.add(var1);
        bn.add(var2);
        bn.add(var3);
        bn.add(var4);
        bn.add(var5);
        bn.add(var6);

        bn.addArc(bn.idFromName("v1"), bn.idFromName("v3"));
        bn.addArc(bn.idFromName("v1"), bn.idFromName("v4"));
        bn.addArc(bn.idFromName("v2"), bn.idFromName("v4"));
        bn.addArc(bn.idFromName("v2"), bn.idFromName("v5"));
        bn.addArc(bn.idFromName("v3"), bn.idFromName("v5"));
        bn.addArc(bn.idFromName("v3"), bn.idFromName("v6"));
        // bn.addArc ( bn.idFromName ( "v4" ), bn.idFromName ( "v5" ) );

        // copy the cpt except for var5
        for (const auto node: bn.nodes())
          if (node != bn.idFromName("v5")) {
            const gum::Tensor< double >& pot = bn.cpt(node);
            const gum::Tensor< double >& src
                = source.cpt(source.idFromName(bn.variable(node).name()));
            gum::Instantiation I(pot);
            gum::Instantiation J(src);

            for (I.setFirst(), J.setFirst(); !I.end(); ++I, ++J) {
              bn.cpt(node).set(I, src[J]);
            }
          }

        gum::SimpleCPTGenerator< double > generator;
        generator.generateCPT(0, bn.cpt(bn.idFromName("v5")));
      } catch (gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    public:
    static void testCreation() {
      gum::BayesNet< double > bn;
      fill(bn);

      gum::BayesNetFragment< double > frag(bn);
      gum::BayesNetFragment< double > frag2(static_cast< const gum::IBayesNet< double >& >(frag));
    }

    static void testInstallNodes() {
      gum::BayesNet< double > bn;
      fill(bn);

      gum::BayesNetFragment< double > frag(bn);

      // install a node
      CHECK((frag.size()) == (static_cast< gum::Size >(0)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v1")));
      CHECK((frag.size()) == (static_cast< gum::Size >(1)));

      // install twice the same node
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v1")));
      CHECK((frag.size()) == (static_cast< gum::Size >(1)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(0)));

      // install a non-existing node
      CHECK((bn.dag().exists((gum::NodeId)100)) == (false));
      CHECK_THROWS_AS(frag.installNode((gum::NodeId)100), const gum::NotFound&);
      CHECK((frag.size()) == (static_cast< gum::Size >(1)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(0)));

      // install a second node (without arc)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v6")));
      CHECK((frag.size()) == (static_cast< gum::Size >(2)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(0)));

      // install a third node (and 2 arcs)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v3")));
      CHECK((frag.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(2)));

      // install ascendants (nothing should happen)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installAscendants(bn.idFromName("v6")));
      CHECK((frag.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(2)));

      // install ascendants (nothing should happen)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installAscendants(bn.idFromName("v5")));
      CHECK((frag.size()) == (static_cast< gum::Size >(6)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(7)));

      // another test for ascendants
      gum::BayesNetFragment< double > frag2(bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag2.installAscendants(bn.idFromName("v5")));
      CHECK((frag2.size()) == (static_cast< gum::Size >(5)));
      CHECK((frag2.sizeArcs()) == (static_cast< gum::Size >(6)));
    }

    static void testInstallNodesWithVar() {
      gum::BayesNet< double > bn;
      fill(bn);

      gum::BayesNetFragment< double > frag(bn);

      // install a node
      CHECK((frag.size()) == (static_cast< gum::Size >(0)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode("v1"));
      CHECK((frag.size()) == (static_cast< gum::Size >(1)));

      // install twice the same node
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode("v1"));
      CHECK((frag.size()) == (static_cast< gum::Size >(1)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(0)));

      // install a non-existing node
      CHECK_THROWS_AS(frag.installNode("v100"), const gum::NotFound&);
      CHECK((frag.size()) == (static_cast< gum::Size >(1)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(0)));

      // install a second node (without arc)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode("v6"));
      CHECK((frag.size()) == (static_cast< gum::Size >(2)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(0)));

      // install a third node (and 2 arcs)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode("v3"));
      CHECK((frag.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(2)));

      // install ascendants (nothing should happen)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installAscendants("v6"));
      CHECK((frag.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(2)));

      // install ascendants (nothing should happen)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installAscendants("v5"));
      CHECK((frag.size()) == (static_cast< gum::Size >(6)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(7)));

      // another test for ascendants
      gum::BayesNetFragment< double > frag2(bn);
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag2.installAscendants("v5"));
      CHECK((frag2.size()) == (static_cast< gum::Size >(5)));
      CHECK((frag2.sizeArcs()) == (static_cast< gum::Size >(6)));
    }

    static void testUninstallNode() {
      gum::BayesNet< double > bn;
      fill(bn);

      gum::BayesNetFragment< double > frag(bn);

      // install ascendants (nothing should happen)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installAscendants(bn.idFromName("v6")));
      CHECK((frag.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(2)));

      // uninstall node 3 (in the middle)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.uninstallNode(bn.idFromName("v3")));
      CHECK((frag.size()) == (static_cast< gum::Size >(2)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(0)));
    }

    static void testUninstallNodeWithNames() {
      gum::BayesNet< double > bn;
      fill(bn);

      gum::BayesNetFragment< double > frag(bn);

      // install ascendants (nothing should happen)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installAscendants("v6"));
      CHECK((frag.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(2)));

      // uninstall node 3 (in the middle)
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.uninstallNode("v3"));
      CHECK((frag.size()) == (static_cast< gum::Size >(2)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(0)));
    }

    static void testIBayetNetMethodsWithoutLocalCPTs() {
      gum::BayesNet< double > bn;
      fill(bn);

      gum::BayesNetFragment< double > frag(bn);

      CHECK(frag.empty());
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v1")));
      CHECK(!frag.empty());

      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v6")));

      CHECK((frag.dag().sizeNodes()) == (static_cast< gum::Size >(2)));
      CHECK((frag.dag().sizeArcs()) == (static_cast< gum::Size >(0)));
      CHECK((frag.size()) == (static_cast< gum::Size >(2)));
      CHECK((frag.dim()) == (gum::Size((3 - 1) + (2 - 1))));
      CHECK((pow(10, frag.log10DomainSize())) == (2 * 3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installAscendants(bn.idFromName("v6")));

      CHECK((frag.dag().sizeNodes()) == (static_cast< gum::Size >(3)));
      CHECK((frag.dag().sizeArcs()) == (static_cast< gum::Size >(2)));
      CHECK((frag.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag.dim()) == (gum::Size((2 * (3 - 1)) + (2 * (2 - 1)) + (2 - 1))));
      CHECK((pow(10, frag.log10DomainSize()))
            == doctest::Approx(2 * 2 * 3).epsilon(GUM_SMALL_ERROR));

      auto I = frag.completeInstantiation();
      I.setFirst();
      CHECK((I.toString()) == ("<v1:0|v3:0|v6:0>"));

      while (!I.end()) {
        double p = bn.cpt(bn.idFromName("v1"))[I] * bn.cpt(bn.idFromName("v3"))[I]
                 * bn.cpt(bn.idFromName("v6"))[I];
        CHECK((frag.jointProbability(I)) == doctest::Approx(p).epsilon(GUM_SMALL_ERROR));
        CHECK((frag.log2JointProbability(I)) == doctest::Approx(log2(p)).epsilon(GUM_SMALL_ERROR));
        ++I;
      }

      gum::Size count = 0;

      for (const auto node: frag.nodes()) {
        GUM_UNUSED(node);
        count++;
      }

      CHECK((count) == (frag.size()));

      count = 0;

      for (const auto& arc: frag.arcs()) {
        GUM_UNUSED(arc);
        count++;
      }

      CHECK((count) == (frag.sizeArcs()));

      const auto order = frag.topologicalOrder();
      CHECK((order.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag.variable(order.atPos(0)).name()) == ("v1"));
      CHECK((frag.variable(order.atPos(1)).name()) == ("v3"));
      CHECK((frag.variable(order.atPos(2)).name()) == ("v6"));
    }

    static void testIBayetNetMethodsWithoutLocalCPTsWithNames() {
      gum::BayesNet< double > bn;
      fill(bn);

      gum::BayesNetFragment< double > frag(bn);

      CHECK(frag.empty());
      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode("v1"));
      CHECK(!frag.empty());

      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installNode("v6"));

      CHECK((frag.dag().sizeNodes()) == (static_cast< gum::Size >(2)));
      CHECK((frag.dag().sizeArcs()) == (static_cast< gum::Size >(0)));
      CHECK((frag.size()) == (static_cast< gum::Size >(2)));
      CHECK((frag.dim()) == (gum::Size((3 - 1) + (2 - 1))));
      CHECK((pow(10, frag.log10DomainSize())) == (2 * 3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(frag.installAscendants("v6"));

      CHECK((frag.dag().sizeNodes()) == (static_cast< gum::Size >(3)));
      CHECK((frag.dag().sizeArcs()) == (static_cast< gum::Size >(2)));
      CHECK((frag.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag.dim()) == (gum::Size((2 * (3 - 1)) + (2 * (2 - 1)) + (2 - 1))));
      CHECK((pow(10, frag.log10DomainSize()))
            == doctest::Approx(2 * 2 * 3).epsilon(GUM_SMALL_ERROR));

      auto I = frag.completeInstantiation();
      I.setFirst();
      CHECK((I.toString()) == ("<v1:0|v3:0|v6:0>"));

      while (!I.end()) {
        double p = bn.cpt("v1")[I] * bn.cpt("v3")[I] * bn.cpt("v6")[I];
        CHECK((frag.jointProbability(I)) == doctest::Approx(p).epsilon(GUM_SMALL_ERROR));
        CHECK((frag.log2JointProbability(I)) == doctest::Approx(log2(p)).epsilon(GUM_SMALL_ERROR));
        ++I;
      }

      gum::Size count = 0;

      for (const auto node: frag.nodes()) {
        GUM_UNUSED(node);
        count++;
      }

      CHECK((count) == (frag.size()));

      count = 0;

      for (const auto& arc: frag.arcs()) {
        GUM_UNUSED(arc);
        count++;
      }

      CHECK((count) == (frag.sizeArcs()));

      const auto order = frag.topologicalOrder();
      CHECK((order.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag.variable(order.atPos(0)).name()) == ("v1"));
      CHECK((frag.variable(order.atPos(1)).name()) == ("v3"));
      CHECK((frag.variable(order.atPos(2)).name()) == ("v6"));
    }

    static void testListeners() {
      gum::BayesNet< double > bn;
      fill(bn);

      gum::BayesNetFragment< double > frag(bn);
      frag.installAscendants(bn.idFromName("v5"));

      CHECK((frag.size()) == (static_cast< gum::Size >(5)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(6)));

      bn.erase(bn.idFromName("v4"));

      CHECK((frag.size()) == (static_cast< gum::Size >(4)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(3)));

      CHECK((frag.dag().parents(bn.idFromName("v5")).size()) == (static_cast< gum::Size >(2)));
      CHECK(!frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v1")));
      CHECK(frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v2")));
      CHECK(frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v3")));

      bn.eraseArc(gum::Arc(bn.idFromName("v2"), bn.idFromName("v5")));

      CHECK((frag.size()) == (static_cast< gum::Size >(4)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(2)));

      CHECK((frag.dag().parents(bn.idFromName("v5")).size()) == (static_cast< gum::Size >(1)));
      CHECK(!frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v1")));
      CHECK(!frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v2")));
      CHECK(frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v3")));

      // nothing should change here
      gum::BayesNet< double > bn2;
      fill(bn2);

      gum::BayesNetFragment< double > frag2(bn2);
      frag2.installAscendants(bn2.idFromName("v6"));

      CHECK((frag2.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag2.sizeArcs()) == (static_cast< gum::Size >(2)));

      std::string tostr2{frag2.toString()};

      bn2.erase(bn2.idFromName("v4"));

      CHECK((frag2.toString()) == (tostr2));

      bn2.eraseArc(gum::Arc(bn.idFromName("v2"), bn2.idFromName("v5")));

      CHECK((frag2.toString()) == (tostr2));

      bn2.add(gum::LabelizedVariable("v7", "unused var"));

      CHECK((frag2.toString()) == (tostr2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(bn2.addArc(bn2.idFromName("v6"), bn2.idFromName("v7")));

      CHECK((frag2.toString()) == (tostr2));
    }

    static void testRelevantForRelevantReasonning() {
      // an inference for all the bn with an hard evidence and an inference for
      // the right fragment with a local CPT should be the same
      try {
        gum::BayesNet< double > bn;
        fill(bn);

        // propagation in the full BN
        gum::LazyPropagation< double > inf_complete(&bn);

        gum::Tensor< double > ev;
        ev << bn.variable(bn.idFromName("v3"));
        ev.fillWith({0.0, 1.0});

        gum::List< const gum::Tensor< double >* > l{&ev};
        inf_complete.addEvidence(ev);
        inf_complete.makeInference();

        auto                         p  = inf_complete.posterior(bn.idFromName("v6"));
        const gum::Tensor< double >& p1 = inf_complete.posterior(bn.idFromName("v6"));

        // propagation in the fragment
        gum::BayesNetFragment< double > frag(bn);
        frag.installAscendants(bn.idFromName("v6"));   // 1->3->6

        gum::Tensor< double > newV3;
        newV3 << bn.variable(bn.idFromName("v3"));
        newV3.fillWith({0.0, 1.0});
        frag.installMarginal(frag.idFromName("v3"), newV3);   // 1   3->6
        CHECK((frag.size()) == (static_cast< gum::Size >(3)));
        CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(1)));

        gum::LazyPropagation< double > inf_frag(&frag);
        inf_frag.makeInference();

        const gum::Tensor< double >& p2 = inf_frag.posterior(bn.idFromName("v6"));

        // comparison
        gum::Instantiation I(p1);
        gum::Instantiation J(p2);

        for (I.setFirst(), J.setFirst(); !I.end(); ++I, ++J)
          CHECK((p1[I]) == doctest::Approx(p2[J]).epsilon(1e-6));
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    static void testInstallCPTs() {
      gum::BayesNet< double > bn;
      fill(bn);
      gum::BayesNetFragment< double > frag(bn);
      frag.installAscendants(bn.idFromName("v6"));   // 1->3->6
      CHECK((frag.size()) == (static_cast< gum::Size >(3)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(2)));

      for (const auto node: frag.nodes())
        CHECK(frag.checkConsistency(node));

      CHECK(frag.checkConsistency());

      frag.installNode(bn.idFromName("v5"));
      // 1->3->6 et 3->5 but 5 does not have all this parents (2,3 et 4)
      CHECK((frag.size()) == (static_cast< gum::Size >(4)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(3)));

      for (const auto node: frag.nodes())
        CHECK((node == frag.idFromName("v5") || frag.checkConsistency(node)));

      CHECK(!frag.checkConsistency());

      gum::Tensor< double > newV5;
      newV5 << bn.variable(bn.idFromName("v5"));
      newV5.fillWith({0.0, 0.0, 1.0});
      frag.installMarginal(frag.idFromName("v5"), newV5);   // 1-->3-->6 5
      CHECK(frag.checkConsistency());
      CHECK((frag.size()) == (static_cast< gum::Size >(4)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(2)));

      frag.installAscendants(bn.idFromName("v4"));
      CHECK(!frag.checkConsistency());   // V5 has now 2 parents : 4 and 2
      CHECK((frag.size()) == (static_cast< gum::Size >(6)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(6)));

      frag.uninstallCPT(frag.idFromName("v5"));
      // V5 tensor got its 3 parents back from the referred BN
      // the fragment is the BN
      CHECK(frag.checkConsistency());
      CHECK((frag.size()) == (static_cast< gum::Size >(6)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(7)));

      // removing 4 make V5 unconsistent
      frag.uninstallNode(frag.idFromName("v4"));
      CHECK(!frag.checkConsistency());
      CHECK((frag.size()) == (static_cast< gum::Size >(5)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(4)));

      gum::Tensor< double > newV5bis;
      newV5bis << bn.variable(bn.idFromName("v5")) << bn.variable(bn.idFromName("v2"))
               << bn.variable(bn.idFromName("v3"));
      frag.installCPT(frag.idFromName("v5"), newV5bis);
      CHECK(frag.checkConsistency());
      CHECK((frag.size()) == (static_cast< gum::Size >(5)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(4)));
    }

    static void testInferenceWithLocalCPTs() {
      gum::BayesNet< double > bn;
      fill(bn);
      gum::BayesNet< double > bn2;
      fill2(bn2, bn);

      gum::BayesNetFragment< double > frag(bn);

      for (const auto node: bn.nodes())
        frag.installNode(node);

      CHECK(frag.checkConsistency());
      CHECK((frag.size()) == (static_cast< gum::Size >(6)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(7)));

      gum::Tensor< double > newV5;
      newV5 << bn.variable(bn.idFromName("v5")) << bn.variable(bn.idFromName("v2"))
            << bn.variable(bn.idFromName("v3"));

      const gum::Tensor< double >& pot2 = bn2.cpt(bn2.idFromName("v5"));
      gum::Instantiation           I(pot2);
      gum::Instantiation           J(newV5);

      for (I.setFirst(), J.setFirst(); !I.end(); ++I, ++J)
        newV5.set(J, pot2[I]);

      frag.installCPT(frag.idFromName("v5"), newV5);
      CHECK(frag.checkConsistency());
      CHECK((frag.size()) == (static_cast< gum::Size >(6)));
      CHECK((frag.sizeArcs()) == (static_cast< gum::Size >(6)));
      str2file("outputs/test.dot", bn2.toDot());

      CHECK(bn2 == frag);

      gum::LazyPropagation< double > ie2(&bn2);
      ie2.makeInference();
      const gum::Tensor< double >& p2 = ie2.posterior(bn2.idFromName("v5"));

      gum::LazyPropagation< double > ie(&frag);
      ie.makeInference();
      const gum::Tensor< double >& p1 = ie.posterior(frag.idFromName("v5"));

      // comparison
      gum::Instantiation II(p1);
      gum::Instantiation JJ(p2);

      for (II.setFirst(), JJ.setFirst(); !II.end(); ++II, ++JJ)
        CHECK((p1[II]) == doctest::Approx(p2[JJ]).epsilon(1e-6));
    }

    static void testCopyToBN() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D;E<-C<-F;");
      CHECK((&bn.cpt("B").variable(1)) == (&bn.variable("A")));

      gum::BayesNetFragment< double > frag(bn);

      frag.installNode("B");
      CHECK(!frag.checkConsistency());
      CHECK_THROWS_AS(frag.toBN(), const gum::OperationNotAllowed&);

      // checking if the nodes are well copied and referenced in frag and then in
      // minibn checking if the tensor are well copied
      frag.installNode("A");
      CHECK(frag.checkConsistency());
      CHECK((&bn.variable("A")) == (&frag.variable("A")));
      CHECK((&bn.variable("B")) == (&frag.variable("B")));
      CHECK((bn.cpt("A").toString()) == (frag.cpt("A").toString()));
      CHECK((bn.cpt("B").toString()) == (frag.cpt("B").toString()));
      CHECK((&frag.cpt("B").variable(1)) == (&bn.variable("A")));
      CHECK((&frag.cpt("B").variable(1)) == (&frag.variable("A")));

      const auto& minibn = frag.toBN();
      CHECK((minibn.size()) == (2u));
      CHECK((minibn.sizeArcs()) == (1u));
      CHECK((&bn.variable("A")) != (&minibn.variable("A")));
      CHECK((&bn.variable("B")) != (&minibn.variable("B")));
      CHECK((bn.cpt("A").toString()) == (minibn.cpt("A").toString()));
      CHECK((bn.cpt("B").toString()) == (minibn.cpt("B").toString()));
      CHECK((&minibn.cpt("B").variable(1)) == (&minibn.variable("A")));
      CHECK((&minibn.cpt("B").variable(1)) != (&frag.variable("A")));
    }

    static void testGraphicalMethods() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D;E<-C<-F;");
      CHECK((&bn.cpt("B").variable(1)) == (&bn.variable("A")));

      gum::BayesNetFragment< double > frag(bn);
      frag.installNode("A");
      frag.installNode("B");
      frag.installNode("C");

      CHECK((frag.children("B")) == (gum::NodeSet({2})));
      CHECK((frag.variableNodeMap().size())
            == (static_cast< gum::Size >(6)));   // the size of bn.size() ...

      CHECK((frag.nodes().asNodeSet()) == (gum::NodeSet({0, 1, 2})));
    }
  };

  GUM_TEST_ACTIF(Creation)
  GUM_TEST_ACTIF(InstallNodes)
  GUM_TEST_ACTIF(InstallNodesWithVar)
  GUM_TEST_ACTIF(UninstallNode)
  GUM_TEST_ACTIF(UninstallNodeWithNames)
  GUM_TEST_ACTIF(IBayetNetMethodsWithoutLocalCPTs)
  GUM_TEST_ACTIF(IBayetNetMethodsWithoutLocalCPTsWithNames)
  GUM_TEST_ACTIF(Listeners)
  GUM_TEST_ACTIF(RelevantForRelevantReasonning)
  GUM_TEST_ACTIF(InstallCPTs)
  GUM_TEST_ACTIF(InferenceWithLocalCPTs)
  GUM_TEST_ACTIF(CopyToBN)
  GUM_TEST_ACTIF(GraphicalMethods)
}   // namespace gum_tests
