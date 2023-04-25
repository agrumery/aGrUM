/**
*
*   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
*   info_at_agrum_dot_org
*
*  This library is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public License
*  along with this library.  If not, see <http://www.gnu.org/licenses/>.
*
*/


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <memory>
#include <vector>
#include "agrum/tools/core/exceptions.h"
#include "cxxtest/TestSuite.h"

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/tools/variables/labelizedVariable.h>


// The graph used for the tests, the ids and the domain sizes:
/*
   A             0             2
    \             \             \
     B   G         1   6         3   4
      \ / \         \ / \         \ / \
       C   H         2   7         2   3
      / \           / \           / \
     D   E         3   4         3   2
    /             /             /
   F             5             3
*/

namespace gum_tests {


 class [[maybe_unused]] IncrementalTestSuite: public CxxTest::TestSuite {
   using TestPotentialSet = gum::Set< const gum::Potential< double >* >;

   static void defineVariables(gum::BayesNet< double >& bn, gum::Potential< double >& joint) {
     bn = gum::BayesNet< double >::fastPrototype(
        "A->B[3]->C->D[3];C->E[2];D->F[3];C<-G[4]->H[3]->E");

     for (const auto node: bn.nodes()) {
       joint *= bn.cpt(node);
     }
   }


   public:
   // ============================================================================
   gum::Potential< double > create_evidence(const gum::NodeId              node_id,
                                            const std::vector< double >&   ev,
                                            const gum::BayesNet< double >& bn) {
     gum::Potential< double > proba;
     proba.add(bn.variable(node_id));
     proba.fillWith(ev);

     return proba;
   }


   // ============================================================================
   gum::Potential< double > posterior_joint(const gum::Potential< double >&             joint,
                                            gum::Set< const gum::Potential< double >* > evidence) {
     gum::Potential< double > joint_pot(joint);
     for (const auto p: evidence)
       joint_pot *= (*p);
     joint_pot.normalize();

     return joint_pot;
   }


   // ============================================================================
   gum::Potential< double > get_marginal(const gum::Potential< double >& joint,
                                         const gum::NodeId               target_id,
                                         const gum::BayesNet< double >&  bn) {
     return joint.margSumIn({&bn.variable(target_id)}).normalize();
   }


   // ============================================================================
   gum::Potential< double > get_joint(const gum::Potential< double > joint,
                                      const gum::NodeSet&            target_ids,
                                      const gum::BayesNet< double >& bn) {
     // get the set of variables to erase
     gum::Set< const gum::DiscreteVariable* > kept;
     for (const auto n: target_ids)
       kept.insert(&bn.variable(n));
     return joint.margSumIn(kept).normalize();
   }


   // ============================================================================
   bool equalPotentials(const gum::Potential< double >& p1, const gum::Potential< double >& p2) {
     gum::Instantiation i1(p1);
     gum::Instantiation i2(p2);

     for (i1.setFirst(), i2.setFirst(); !i1.end(); i1.inc(), i2.inc()) {
       if ((p1[i1] == 0) && (std::fabs(p2[i2]) > TS_GUM_SMALL_ERROR)) return false;
       if (p1[i1] > p2[i2]) {
         if (std::fabs((p1[i1] - p2[i2]) / p1[i1]) > TS_GUM_SMALL_ERROR) return false;
       } else {
         if (std::fabs((p1[i1] - p2[i2]) / p1[i2]) > TS_GUM_SMALL_ERROR) return false;
       }
     }

     return true;
   }


   // ============================================================================
   // ============================================================================
   GUM_ACTIVE_TEST(_prior) {
     gum::BayesNet< double >  bn;
     gum::Potential< double > joint;
     defineVariables(bn, joint);

     gum::LazyPropagation< double > inf(&bn);
     TS_ASSERT_THROWS_NOTHING(inf.makeInference())

     // get the marginals of A, C, D, H
     auto pa = get_marginal(joint, bn.idFromName("A"), bn);
     auto pc = get_marginal(joint, bn.idFromName("C"), bn);
     auto pd = get_marginal(joint, bn.idFromName("D"), bn);
     auto ph = get_marginal(joint, bn.idFromName("H"), bn);

     TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
     TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("C")), pc))
     TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("H")), ph))
   }

   // ============================================================================
   // ============================================================================
   GUM_ACTIVE_TEST(_prior_with_targets) {
     gum::BayesNet< double >  bn;
     gum::Potential< double > joint;
     defineVariables(bn, joint);

     gum::LazyPropagation< double > inf(&bn);
     inf.eraseAllTargets();
     inf.addTarget(bn.idFromName("A"));
     inf.addTarget(bn.idFromName("C"));

     TS_ASSERT_THROWS_NOTHING(inf.makeInference())

     // get the marginals of A, C, D
     auto pa = get_marginal(joint, bn.idFromName("A"), bn);
     auto pc = get_marginal(joint, bn.idFromName("C"), bn);
     auto pb = get_marginal(joint, bn.idFromName("B"), bn);

     TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(bn.idFromName("B")))
     TS_ASSERT_THROWS(inf.posterior(bn.idFromName("D")), const gum::UndefinedElement&)

     TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
     TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("C")), pc))
     TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("B")), pb))
   }


   // ============================================================================
   // ============================================================================
   GUM_ACTIVE_TEST(_prior_with_targets_evidence) {
     gum::BayesNet< double >  bn;
     gum::Potential< double > joint;
     defineVariables(bn, joint);

     gum::LazyPropagation< double > inf(&bn);
     inf.eraseAllTargets();
     inf.addTarget(bn.idFromName("A"));   // A
     inf.addTarget(bn.idFromName("F"));   // F

     auto ev1 = create_evidence(bn.idFromName("B"), {0, 0, 1}, bn);
     inf.addEvidence(bn.idFromName("B"), 2);
     auto ev3 = create_evidence(bn.idFromName("D"), {0.2, 0.6, 0.6}, bn);
     inf.addEvidence(ev3);

     const TestPotentialSet evset{&ev1, &ev3};
     auto                   posterior = posterior_joint(joint, evset);

     TS_ASSERT_THROWS_NOTHING(inf.makeInference())

     // get the marginals of A, F
     auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
     auto pf = get_marginal(posterior, bn.idFromName("F"), bn);

     TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
     TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("F")), pf))
   }


   // ============================================================================
   // ============================================================================
   GUM_ACTIVE_TEST(_prior_with_targets_outside_evidence) {
     gum::BayesNet< double >  bn;
     gum::Potential< double > joint;
     defineVariables(bn, joint);

     gum::LazyPropagation< double > inf(&bn);
     inf.eraseAllTargets();
     inf.addTarget(bn.idFromName("A"));   // A
     inf.addTarget(bn.idFromName("D"));   // D

     auto ev0 = create_evidence(0, {0.3, 0.7}, bn);
     inf.addEvidence(ev0);
     auto ev1 = create_evidence(1, {0.3, 0.1, 0.8}, bn);
     inf.addEvidence(ev1);
     auto ev7 = create_evidence(7, {0.4, 0.2, 0.3}, bn);
     inf.addEvidence(ev7);

     const TestPotentialSet evset{&ev0, &ev1, &ev7};
     auto                   posterior = posterior_joint(joint, evset);

     TS_ASSERT_THROWS_NOTHING(inf.makeInference())

     // get the marginals of A, D
     auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
     auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

     TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
     TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
   }


   // ============================================================================
   // ============================================================================
   GUM_ACTIVE_TEST(_prior_with_targets_evidence_values_changed) {
     gum::BayesNet< double >  bn;
     gum::Potential< double > joint;
     defineVariables(bn, joint);

     gum::LazyPropagation< double > inf(&bn);
     inf.eraseAllTargets();
     inf.addTarget(bn.idFromName("A"));   // A
     inf.addTarget(bn.idFromName("D"));   // D

     auto ev0 = create_evidence(0, {0.3, 0.7}, bn);
     inf.addEvidence(ev0);
     auto ev1 = create_evidence(1, {0.3, 0.1, 0.8}, bn);
     inf.addEvidence(ev1);
     auto ev7 = create_evidence(7, {0.4, 0.2, 0.3}, bn);
     inf.addEvidence(ev7);


     {
       const TestPotentialSet evset{&ev0, &ev1, &ev7};
       auto                   posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }


     auto evp0 = create_evidence(0, {1, 0}, bn);
     inf.chgEvidence(0, 0);
     auto evp1 = create_evidence(1, {0.8, 0.4, 0.1}, bn);
     inf.chgEvidence(1, std::vector< double >{0.8, 0.4, 0.1});
     auto evp7 = create_evidence(7, {0.2, 0.3, 0.6}, bn);
     inf.chgEvidence(evp7);

     {
       const TestPotentialSet evset{&evp0, &evp1, &evp7};
       auto                   posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }


     auto evpp7 = create_evidence(7, {0.9, 0.1, 0.3}, bn);
     inf.chgEvidence(evpp7);

     {
       const TestPotentialSet evset{&evp0, &evp1, &evpp7};
       auto                   posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }


     inf.chgEvidence(ev0);
     inf.chgEvidence(evp7);
     {
       const TestPotentialSet evset{&ev0, &evp1, &evp7};
       auto                   posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }

     inf.eraseEvidence(0);
     {
       const TestPotentialSet evset{&evp1, &evp7};
       auto                   posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }


     inf.addEvidence(evp0);
     TS_ASSERT_THROWS_NOTHING(inf.makeInference())
     inf.eraseEvidence(0);

     {
       const TestPotentialSet evset{&evp1, &evp7};
       auto                   posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }
   }


   // ============================================================================
   // ============================================================================
   GUM_ACTIVE_TEST(_prior_with_targets_hard_evidence_values_changed) {
     gum::BayesNet< double >  bn;
     gum::Potential< double > joint;
     defineVariables(bn, joint);

     gum::LazyPropagation< double > inf(&bn);
     inf.eraseAllTargets();
     inf.addTarget(bn.idFromName("A"));   // A
     inf.addTarget(bn.idFromName("D"));   // D

     auto ev0 = create_evidence(0, {0.3, 0.7}, bn);
     inf.addEvidence(ev0);
     auto ev1 = create_evidence(1, {0, 1, 0}, bn);
     inf.addEvidence(ev1);
     auto ev7 = create_evidence(7, {0.4, 0.2, 0.3}, bn);
     inf.addEvidence(ev7);


     {
       TestPotentialSet evset{&ev0, &ev1, &ev7};
       auto             posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }


     auto evp0 = create_evidence(0, {1, 0}, bn);
     inf.chgEvidence(0, 0);
     auto evp7 = create_evidence(7, {0.2, 0.3, 0.6}, bn);
     inf.chgEvidence(evp7);

     {
       TestPotentialSet evset{&evp0, &ev1, &evp7};
       auto             posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }


     auto evpp0 = create_evidence(0, {0, 1}, bn);
     inf.chgEvidence(evpp0);
     {
       TestPotentialSet evset{&evpp0, &ev1, &evp7};
       auto             posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }


     inf.chgEvidence(ev0);
     inf.chgEvidence(evp7);
     {
       TestPotentialSet evset{&ev0, &ev1, &evp7};
       auto             posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }

     inf.eraseEvidence(0);
     {
       TestPotentialSet evset{&ev1, &evp7};
       auto             posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }


     inf.addEvidence(evp0);
     TS_ASSERT_THROWS_NOTHING(inf.makeInference())
     inf.eraseEvidence(0);

     {
       TestPotentialSet evset{&ev1, &evp7};
       auto             posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }
   }


   // ============================================================================
   // ============================================================================
   GUM_ACTIVE_TEST(_prior_with_targets_evidence_changed) {
     gum::BayesNet< double >  bn;
     gum::Potential< double > joint;
     defineVariables(bn, joint);

     gum::LazyPropagation< double > inf(&bn);
     inf.eraseAllTargets();
     inf.addTarget(bn.idFromName("A"));   // A
     inf.addTarget(bn.idFromName("D"));   // D

     auto ev0 = create_evidence(0, {0.3, 1.7}, bn);
     inf.addEvidence(ev0);
     auto ev1 = create_evidence(1, {0.3, 0.1, 0.8}, bn);
     inf.addEvidence(ev1);
     auto ev7 = create_evidence(7, {0.4, 0.2, 0.3}, bn);
     inf.addEvidence(ev7);

     {
       TestPotentialSet evset{&ev0, &ev1, &ev7};
       auto             posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }


     auto ev4  = create_evidence(4, {1, 0}, bn);
     auto evp7 = create_evidence(7, {0.2, 0.3, 0.6}, bn);
     inf.eraseEvidence(0);
     inf.addEvidence(ev4);
     inf.chgEvidence(evp7);
     {
       TestPotentialSet evset{&ev1, &ev4, &evp7};
       auto             posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())

       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }

     auto evp0  = create_evidence(0, {1, 0}, bn);
     auto evpp4 = create_evidence(4, {0.7, 0.7}, bn);
     auto evpp7 = create_evidence(7, {0, 1, 0}, bn);
     inf.addEvidence(0, 0);
     inf.chgEvidence(evpp4);
     inf.chgEvidence(evpp7);

     {
       TestPotentialSet evset{&evp0, &ev1, &evpp4, &evpp7};
       auto             posterior = posterior_joint(joint, evset);

       TS_ASSERT_THROWS_NOTHING(inf.makeInference())
       // get the marginals of A, D
       auto pa = get_marginal(posterior, bn.idFromName("A"), bn);
       auto pd = get_marginal(posterior, bn.idFromName("D"), bn);

       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("A")), pa))
       TS_ASSERT(equalPotentials(inf.posterior(bn.idFromName("D")), pd))
     }
   }

   GUM_ACTIVE_TEST(_implicit_joint_target) {
     gum::BayesNet< double >  bn;
     gum::Potential< double > joint;
     defineVariables(bn, joint);

     gum::LazyPropagation< double > inf(&bn);
     inf.eraseAllTargets();
     inf.addTarget(bn.idFromName("A"));   // A
     inf.addTarget(bn.idFromName("D"));   // D
     inf.makeInference();

     TS_GUM_ASSERT_THROWS_NOTHING(
        inf.jointPosterior(gum::NodeSet{bn.idFromName("D"), bn.idFromName("C")}))

     TS_ASSERT_THROWS(inf.jointPosterior(gum::NodeSet{bn.idFromName("F"), bn.idFromName("H")}),
                      gum::UndefinedElement&)

     TS_GUM_POTENTIAL_DELTA(
        inf.jointPosterior(gum::NodeSet{bn.idFromName("D"), bn.idFromName("C")}),
        joint.margSumIn({&bn.variable("D"), &bn.variable("C")}).normalize(),
        TS_GUM_SMALL_ERROR)
   }
 };
}   // namespace gum_tests
