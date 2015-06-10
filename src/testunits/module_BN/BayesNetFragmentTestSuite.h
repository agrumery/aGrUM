/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/config.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/potential.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/BayesNetFragment.h>
#include <agrum/BN/inference/lazyPropagation.h>

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

  class BayesNetFragmentTestSuite : public CxxTest::TestSuite {

    private:
    void fill(gum::BayesNet<float> &bn) {
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

      } catch (gum::Exception &e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }

      bn.generateCPTs();
    }

    // the same BN but with no 4-->5
    void fill2(gum::BayesNet<float> &bn, const gum::BayesNet<float> &source) {
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
        for (const auto node : bn.nodes())
          if (node != bn.idFromName("v5")) {
            const gum::Potential<float> &pot = bn.cpt(node);
            const gum::Potential<float> &src =
                source.cpt(source.idFromName(bn.variable(node).name()));
            gum::Instantiation I(pot);
            gum::Instantiation J(src);

            for (I.setFirst(), J.setFirst(); !I.end(); ++I, ++J) {
              bn.cpt(node).set(I, src[J]);
            }
          }

        gum::SimpleCPTGenerator<float> generator;
        generator.generateCPT(0, bn.cpt(bn.idFromName("v5")));
      } catch (gum::Exception &e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    public:
    void testCreation() {
      gum::BayesNet<float> bn;
      fill(bn);

      gum::BayesNetFragment<float> frag(bn);
    }

    void testInstallNodes() {
      gum::BayesNet<float> bn;
      fill(bn);

      gum::BayesNetFragment<float> frag(bn);

      // install a node
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)0);
      TS_GUM_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v1")));
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)1);

      // install twice the same node
      TS_GUM_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v1")));
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)1);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)0);

      // install a non-existing node
      TS_ASSERT_EQUALS(bn.dag().exists((gum::NodeId)100), false);
      TS_ASSERT_THROWS(frag.installNode((gum::NodeId)100), gum::NotFound);
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)1);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)0);

      // install a second node (without arc)
      TS_GUM_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v6")));
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)2);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)0);

      // install a third node (and 2 arcs)
      TS_GUM_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v3")));
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)2);

      // install ascendants (nothing should happen)
      TS_GUM_ASSERT_THROWS_NOTHING(frag.installAscendants(bn.idFromName("v6")));
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)2);

      // install ascendants (nothing should happen)
      TS_GUM_ASSERT_THROWS_NOTHING(frag.installAscendants(bn.idFromName("v5")));
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)7);

      // another test for ascendants
      gum::BayesNetFragment<float> frag2(bn);
      TS_GUM_ASSERT_THROWS_NOTHING(frag2.installAscendants(bn.idFromName("v5")));
      TS_ASSERT_EQUALS(frag2.size(), (gum::Size)5);
      TS_ASSERT_EQUALS(frag2.sizeArcs(), (gum::Size)6);
    }

    void testUninstallNode() {
      gum::BayesNet<float> bn;
      fill(bn);

      gum::BayesNetFragment<float> frag(bn);

      // install ascendants (nothing should happen)
      TS_GUM_ASSERT_THROWS_NOTHING(frag.installAscendants(bn.idFromName("v6")));
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)2);

      // uninstall node 3 (in the middle)
      TS_GUM_ASSERT_THROWS_NOTHING(frag.uninstallNode(bn.idFromName("v3")));
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)2);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)0);
    }

    void testIBayetNetMethodsWithoutLocalCPTs() {
      gum::BayesNet<float> bn;
      fill(bn);

      gum::BayesNetFragment<float> frag(bn);

      TS_ASSERT(frag.empty());
      TS_GUM_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v1")));
      TS_ASSERT(!frag.empty());

      TS_GUM_ASSERT_THROWS_NOTHING(frag.installNode(bn.idFromName("v6")));

      TS_ASSERT_EQUALS(frag.dag().sizeNodes(), gum::Size(2));
      TS_ASSERT_EQUALS(frag.dag().sizeArcs(), gum::Size(0));
      TS_ASSERT_EQUALS(frag.size(), gum::Size(2));
      TS_ASSERT_EQUALS(frag.dim(), gum::Size((3 - 1) + (2 - 1)));
      TS_ASSERT_EQUALS(pow(10, frag.log10DomainSize()), 2 * 3);

      TS_GUM_ASSERT_THROWS_NOTHING(frag.installAscendants(bn.idFromName("v6")));

      TS_ASSERT_EQUALS(frag.dag().sizeNodes(), gum::Size(3));
      TS_ASSERT_EQUALS(frag.dag().sizeArcs(), gum::Size(2));
      TS_ASSERT_EQUALS(frag.size(), gum::Size(3));
      TS_ASSERT_EQUALS(frag.dim(),
                       gum::Size((2 * (3 - 1)) + (2 * (2 - 1)) + (2 - 1)));
      TS_ASSERT_DELTA(pow(10, frag.log10DomainSize()), 2 * 2 * 3, 1e-5);

      gum::Instantiation I;
      frag.completeInstantiation(I);
      I.setFirst();
      TS_ASSERT_EQUALS(I.toString(), "<v1:0|v3:0|v6:0>");

      while (!I.end()) {
        float p = bn.cpt(bn.idFromName("v1"))[I] * bn.cpt(bn.idFromName("v3"))[I] *
                  bn.cpt(bn.idFromName("v6"))[I];
        TS_ASSERT_DELTA(frag.jointProbability(I), p, 1e-5);
        TS_ASSERT_DELTA(frag.log2JointProbability(I), log2(p), 1e-5);
        ++I;
      }

      gum::Size count = 0;

      for (const auto node : frag.nodes()) {
        GUM_UNUSED(node);
        count++;
      }

      TS_ASSERT_EQUALS(count, frag.size());

      count = 0;

      for (const auto arc : frag.arcs()) {
        GUM_UNUSED(arc);
        count++;
      }

      TS_ASSERT_EQUALS(count, frag.sizeArcs());

      const auto &order = frag.topologicalOrder();
      TS_ASSERT_EQUALS(order.size(), gum::Size(3));
      TS_ASSERT_EQUALS(frag.variable(order.atPos(0)).name(), "v1");
      TS_ASSERT_EQUALS(frag.variable(order.atPos(1)).name(), "v3");
      TS_ASSERT_EQUALS(frag.variable(order.atPos(2)).name(), "v6");
    }

    void testListeners() {
      gum::BayesNet<float> bn;
      fill(bn);

      gum::BayesNetFragment<float> frag(bn);
      frag.installAscendants(bn.idFromName("v5"));

      TS_ASSERT_EQUALS(frag.size(), (gum::Size)5);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)6);

      bn.erase(bn.idFromName("v4"));

      TS_ASSERT_EQUALS(frag.size(), (gum::Size)4);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)3);

      TS_ASSERT_EQUALS(frag.dag().parents(bn.idFromName("v5")).size(), (gum::Size)2);
      TS_ASSERT(
          !frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v1")));
      TS_ASSERT(
          frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v2")));
      TS_ASSERT(
          frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v3")));

      bn.eraseArc(gum::Arc(bn.idFromName("v2"), bn.idFromName("v5")));

      TS_ASSERT_EQUALS(frag.size(), (gum::Size)4);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)2);

      TS_ASSERT_EQUALS(frag.dag().parents(bn.idFromName("v5")).size(), (gum::Size)1);
      TS_ASSERT(
          !frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v1")));
      TS_ASSERT(
          !frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v2")));
      TS_ASSERT(
          frag.dag().parents(bn.idFromName("v5")).contains(bn.idFromName("v3")));

      // nothing should change here
      gum::BayesNet<float> bn2;
      fill(bn2);

      gum::BayesNetFragment<float> frag2(bn2);
      frag2.installAscendants(bn2.idFromName("v6"));

      TS_ASSERT_EQUALS(frag2.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(frag2.sizeArcs(), (gum::Size)2);

      std::string tostr2{frag2.toString()};

      bn2.erase(bn2.idFromName("v4"));

      TS_ASSERT_EQUALS(frag2.toString(), tostr2);

      bn2.eraseArc(gum::Arc(bn.idFromName("v2"), bn2.idFromName("v5")));

      TS_ASSERT_EQUALS(frag2.toString(), tostr2);

      bn2.add(gum::LabelizedVariable("v7", "unused var"));

      TS_ASSERT_EQUALS(frag2.toString(), tostr2);

      TS_GUM_ASSERT_THROWS_NOTHING(
          bn2.addArc(bn2.idFromName("v6"), bn2.idFromName("v7")));

      TS_ASSERT_EQUALS(frag2.toString(), tostr2);
    }

    void testRelevantForRelevantReasonning() {
      // an inference for all the bn with an hard evidence and an inference for
      // the right fragment with a local CPT should be the same

      gum::BayesNet<float> bn;
      fill(bn);

      // propagation in the full BN
      gum::LazyPropagation<float> inf_complete(bn);
      gum::Potential<float> ev;
      ev << bn.variable(bn.idFromName("v3"));
      ev.fillWith(std::vector<float>({0.0, 1.0}));
      gum::List<const gum::Potential<float> *> l{&ev};
      inf_complete.insertEvidence(l);
      inf_complete.makeInference();
      const gum::Potential<float> &p1 = inf_complete.posterior(bn.idFromName("v6"));

      // propagation in the fragment
      gum::BayesNetFragment<float> frag(bn);
      frag.installAscendants(bn.idFromName("v6")); // 1->3->6
      gum::Potential<float> *newV3 = new gum::Potential<float>();
      (*newV3) << bn.variable(bn.idFromName("v3"));
      newV3->fillWith(std::vector<float>({0.0, 1.0}));
      frag.installMarginal(frag.idFromName("v3"), newV3); // 1   3->6
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)1);

      gum::LazyPropagation<float> inf_frag(frag);
      inf_frag.makeInference();
      const gum::Potential<float> &p2 = inf_frag.posterior(bn.idFromName("v6"));

      // comparison
      gum::Instantiation I(p1);
      gum::Instantiation J(p2);

      for (I.setFirst(), J.setFirst(); !I.end(); ++I, ++J)
        TS_ASSERT_DELTA(p1[I], p2[J], 1e-6);
    }

    void testInstallCPTs() {
      gum::BayesNet<float> bn;
      fill(bn);
      gum::BayesNetFragment<float> frag(bn);
      frag.installAscendants(bn.idFromName("v6")); // 1->3->6
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)3);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)2);

      for (const auto node : frag.nodes())
        TS_ASSERT(frag.checkConsistency(node));

      TS_ASSERT(frag.checkConsistency());

      frag.installNode(bn.idFromName("v5"));
      // 1->3->6 et 3->5 but 5 does not have all this parents (2,3 et 4)
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)4);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)3);

      for (const auto node : frag.nodes())
        TS_ASSERT(node == frag.idFromName("v5") || frag.checkConsistency(node));

      TS_ASSERT(!frag.checkConsistency());

      gum::Potential<float> *newV5 = new gum::Potential<float>();
      (*newV5) << bn.variable(bn.idFromName("v5"));
      newV5->fillWith(std::vector<float>({0.0, 0.0, 1.0}));
      frag.installMarginal(frag.idFromName("v5"), newV5); // 1-->3-->6 5
      TS_ASSERT(frag.checkConsistency());
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)4);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)2);

      frag.installAscendants(bn.idFromName("v4"));
      TS_ASSERT(!frag.checkConsistency()); // V5 has now 2 parents : 4 and 2
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)6);

      frag.uninstallCPT(frag.idFromName("v5"));
      // V5 potential got its 3 parents back from the referred BN
      // the fragment is the BN
      TS_ASSERT(frag.checkConsistency());
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)7);

      // removing 4 make V5 unconsistent
      frag.uninstallNode(frag.idFromName("v4"));
      TS_ASSERT(!frag.checkConsistency());
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)5);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)4);

      gum::Potential<float> *newV5bis = new gum::Potential<float>();
      (*newV5bis) << bn.variable(bn.idFromName("v5"))
                  << bn.variable(bn.idFromName("v2"))
                  << bn.variable(bn.idFromName("v3"));
      frag.installCPT(frag.idFromName("v5"), newV5bis);
      TS_ASSERT(frag.checkConsistency());
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)5);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)4);
    }

    void testInferenceWithLocalCPTs() {
      gum::BayesNet<float> bn;
      fill(bn);
      gum::BayesNet<float> bn2;
      fill2(bn2, bn);

      gum::BayesNetFragment<float> frag(bn);

      for (const auto node : bn.nodes())
        frag.installNode(node);

      TS_ASSERT(frag.checkConsistency());
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)7);

      gum::Potential<float> *newV5 = new gum::Potential<float>();
      (*newV5) << bn.variable(bn.idFromName("v5"))
               << bn.variable(bn.idFromName("v2"))
               << bn.variable(bn.idFromName("v3"));

      const gum::Potential<float> &pot2 = bn2.cpt(bn2.idFromName("v5"));
      gum::Instantiation I(pot2);
      gum::Instantiation J(*newV5);

      for (I.setFirst(), J.setFirst(); !I.end(); ++I, ++J)
        newV5->set(J, pot2[I]);

      frag.installCPT(frag.idFromName("v5"), newV5);
      TS_ASSERT(frag.checkConsistency());
      TS_ASSERT_EQUALS(frag.size(), (gum::Size)6);
      TS_ASSERT_EQUALS(frag.sizeArcs(), (gum::Size)6);
      str2file("test.dot", bn2.toDot());

      TS_ASSERT(bn2 == frag);

      gum::LazyPropagation<float> ie2(bn2);
      ie2.makeInference();
      const gum::Potential<float> &p2 = ie2.posterior(bn2.idFromName("v5"));

      gum::LazyPropagation<float> ie(frag);
      ie.makeInference();
      const gum::Potential<float> &p1 = ie.posterior(frag.idFromName("v5"));

      // comparison
      gum::Instantiation II(p1);
      gum::Instantiation JJ(p2);

      for (II.setFirst(), JJ.setFirst(); !II.end(); ++II, ++JJ)
        TS_ASSERT_DELTA(p1[II], p2[JJ], 1e-6);
    }
  };
} // gum_tests
