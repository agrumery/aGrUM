/**
 *
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * (_at_AMU) info_at_agrum_dot_org
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


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/discretizedVariable.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/variables/rangeVariable.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/MRF/inference/ShaferShenoyMRFInference.h>
#include <agrum/MRF/MarkovRandomField.h>

namespace gum_tests {
  class [[maybe_unused]] ShaferShenoyMRFTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Constructor) {
      auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B--C;C--D;C--E--F");
      gum::ShaferShenoyMRFInference< double > ie(&mn);
    }

    GUM_ACTIVE_TEST(SimpleInference) {
      auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B--C;C--D;C--E--F");
      gum::ShaferShenoyMRFInference< double > ie(&mn);
      ie.makeInference();
    }

    GUM_ACTIVE_TEST(CompareInferenceTree) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D<-E;B->F;D->G;");
      gum::LazyPropagation< double > iebn(&bn);
      iebn.makeInference();

      auto                                    mn = gum::MarkovRandomField< double >::fromBN(bn);
      gum::ShaferShenoyMRFInference< double > iemn(&mn);
      iemn.makeInference();

      for (const auto n: bn.nodes()) {
        const std::string&              name   = bn.variable(n).name();
        const gum::Potential< double >& postbn = iebn.posterior(name);

        gum::Potential< double > postmn;
        postmn.add(bn.variable(n));
        postmn.fillWith(iemn.posterior(name));   // postmn using bn variable

        TS_ASSERT_LESS_THAN((postbn - postmn).abs().max(), 1e-7)
      }
    }

    GUM_ACTIVE_TEST(CompareInferenceDAG) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D<-E;B->F;D->F;");
      gum::LazyPropagation< double > iebn(&bn);
      iebn.makeInference();

      auto                                    mn = gum::MarkovRandomField< double >::fromBN(bn);
      gum::ShaferShenoyMRFInference< double > iemn(&mn);
      iemn.makeInference();

      for (const auto n: bn.nodes()) {
        const std::string&              name   = bn.variable(n).name();
        const gum::Potential< double >& postbn = iebn.posterior(name);

        gum::Potential< double > postmn;
        postmn.add(bn.variable(n));
        postmn.fillWith(iemn.posterior(name));   // postmn using bn variable

        TS_ASSERT_LESS_THAN((postbn - postmn).abs().max(), 1e-7)
      }
    }

    GUM_ACTIVE_TEST(CompareInferenceTreeWithEvidence) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D<-E;B->F;D->G;");
      gum::LazyPropagation< double > iebn(&bn);
      iebn.addEvidence("B", 1);
      iebn.makeInference();

      auto                                    mn = gum::MarkovRandomField< double >::fromBN(bn);
      gum::ShaferShenoyMRFInference< double > iemn(&mn);
      iemn.addEvidence("B", 1);
      iemn.makeInference();

      for (const auto n: bn.nodes()) {
        const std::string&              name   = bn.variable(n).name();
        const gum::Potential< double >& postbn = iebn.posterior(name);

        gum::Potential< double > postmn;
        postmn.add(bn.variable(n));
        postmn.fillWith(iemn.posterior(name));   // postmn using bn variable

        TS_ASSERT_LESS_THAN((postbn - postmn).abs().max(), 1e-7)
      }
    }

    GUM_ACTIVE_TEST(CompareInferenceDAGWithEvidence) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D<-E;B->F;D->F;");
      gum::LazyPropagation< double > iebn(&bn);
      iebn.addEvidence("B", 1);
      iebn.makeInference();

      auto                                    mn = gum::MarkovRandomField< double >::fromBN(bn);
      gum::ShaferShenoyMRFInference< double > iemn(&mn);
      iemn.addEvidence("B", 1);
      iemn.makeInference();

      for (const auto n: bn.nodes()) {
        const std::string&              name   = bn.variable(n).name();
        const gum::Potential< double >& postbn = iebn.posterior(name);

        gum::Potential< double > postmn;
        postmn.add(bn.variable(n));
        postmn.fillWith(iemn.posterior(name));   // postmn using bn variable

        TS_ASSERT_LESS_THAN((postbn - postmn).abs().max(), 1e-8)
      }
    }

    GUM_ACTIVE_TEST(CompareInferenceDAGWithSoftEvidence) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D<-E;B->F;D->F;");
      gum::LazyPropagation< double > iebn(&bn);
      iebn.addEvidence("B", std::vector< double >{0.8, 0.5});
      iebn.makeInference();

      auto                                    mn = gum::MarkovRandomField< double >::fromBN(bn);
      gum::ShaferShenoyMRFInference< double > iemn(&mn);
      iemn.addEvidence("B", std::vector< double >{0.8, 0.5});
      iemn.makeInference();

      for (const auto n: bn.nodes()) {
        const std::string&              name   = bn.variable(n).name();
        const gum::Potential< double >& postbn = iebn.posterior(name);

        gum::Potential< double > postmn;
        postmn.add(bn.variable(n));
        postmn.fillWith(iemn.posterior(name));   // postmn using bn variable

        TS_ASSERT_LESS_THAN((postbn - postmn).abs().max(), 1e-8)
      }
    }

    GUM_ACTIVE_TEST(ClassicalInference) {
      auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B--C;C--D;D--E--F;F--A");
      gum::ShaferShenoyMRFInference< double > iemn(&mn);
      iemn.addEvidence("B", 1);
      iemn.makeInference();

      for (const auto n: mn.nodes()) {
        TS_ASSERT_DELTA(iemn.posterior(n).sum(), 1.0, 1e-8)
      }
    }

    GUM_ACTIVE_TEST(SeparationInInference) {
      auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B--C;C--D;D--E--F;F--A");
      gum::ShaferShenoyMRFInference< double > iemn(&mn);
      iemn.addEvidence("A", 0);
      iemn.addEvidence("D", 1);
      iemn.makeInference();
      gum::Potential< double > Cwithout_evB(iemn.posterior("C"));
      gum::Potential< double > Ewithout_evB(iemn.posterior("E"));

      iemn.eraseAllEvidence();
      iemn.addEvidence("A", 0);
      iemn.addEvidence("D", 1);
      iemn.addEvidence("B", 0);
      iemn.makeInference();
      gum::Potential< double > Cwith_evB0(iemn.posterior("C"));
      gum::Potential< double > Ewith_evB0(iemn.posterior("E"));

      iemn.eraseAllEvidence();
      iemn.addEvidence("A", 0);
      iemn.addEvidence("D", 1);
      iemn.addEvidence("B", 1);
      iemn.makeInference();
      gum::Potential< double > Cwith_evB1(iemn.posterior("C"));
      gum::Potential< double > Ewith_evB1(iemn.posterior("E"));

      TS_ASSERT_LESS_THAN(-(Cwithout_evB - Cwith_evB0).abs().max(), -1e-8)
      TS_ASSERT_LESS_THAN(-(Cwithout_evB - Cwith_evB1).abs().max(), -1e-8)
      TS_ASSERT_LESS_THAN(-(Cwith_evB1 - Cwith_evB0).abs().max(), -1e-8)

      TS_ASSERT_LESS_THAN((Ewithout_evB - Ewith_evB0).abs().max(), 1e-8)
      TS_ASSERT_LESS_THAN((Ewithout_evB - Ewith_evB1).abs().max(), 1e-8)
      TS_ASSERT_LESS_THAN((Ewith_evB1 - Ewith_evB0).abs().max(), 1e-8)
    }

    GUM_ACTIVE_TEST(IndependencyInference) {
      auto mn = gum::MarkovRandomField< double >::fastPrototype("A;B;C;D;E;F");
      gum::ShaferShenoyMRFInference< double > iemn(&mn);
      iemn.addEvidence("B", 1);
      iemn.makeInference();

      for (const auto n: mn.nodes()) {
        TS_ASSERT_DELTA(iemn.posterior(n).sum(), 1.0, 1e-8)
      }
    }

    GUM_ACTIVE_TEST(IncrementalInference) {
      auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B--C;C--D;D--E--F;F--A");

      {
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.addEvidence("A", 0);
        ie.addEvidence("D", 1);
        ie.addEvidence("B", 0);
        ie.makeInference();

        gum::ShaferShenoyMRFInference< double > ie2(&mn);
        ie2.addEvidence("A", 1);
        ie2.addEvidence("D", 1);
        ie2.addEvidence("B", 1);
        ie2.makeInference();

        ie2.chgEvidence("A", 0);
        ie2.chgEvidence("B", 0);
        ie2.makeInference();

        for (const auto n: mn.nodes()) {
          TS_ASSERT_LESS_THAN((ie2.posterior(n) - ie.posterior(n)).abs().max(), 1e-8)
        }
      }

      {
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.addEvidence("A", 0);
        ie.addEvidence("D", 1);
        ie.addEvidence("B", 0);
        ie.makeInference();

        gum::ShaferShenoyMRFInference< double > ie2(&mn);
        ie2.addEvidence("A", 0);
        ie2.addEvidence("D", 0);
        ie2.addEvidence("B", 0);
        ie2.makeInference();

        ie2.chgEvidence("D", 1);
        ie2.makeInference();

        for (const auto n: mn.nodes()) {
          TS_ASSERT_LESS_THAN((ie2.posterior(n) - ie.posterior(n)).abs().max(), 1e-8)
        }
      }
    }

    GUM_ACTIVE_TEST(IncrementalInferenceWithSoftEvidence) {
      auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B--C;C--D;D--E--F;F--A");

      {
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.addEvidence("A", std::vector< double >{0.3, 0.9});
        ie.addEvidence("D", std::vector< double >{0.5, 0.2});
        ie.addEvidence("B", 0);
        ie.makeInference();

        gum::ShaferShenoyMRFInference< double > ie2(&mn);
        ie2.addEvidence("A", std::vector< double >{0.5, 0.2});
        ie2.addEvidence("D", std::vector< double >{0.5, 0.2});
        ie2.addEvidence("B", std::vector< double >{0.5, 0.2});
        ie2.makeInference();

        ie2.chgEvidence("A", std::vector< double >{0.3, 0.9});
        ie2.chgEvidence("B", 0);
        ie2.makeInference();

        for (const auto n: mn.nodes()) {
          TS_ASSERT_LESS_THAN((ie2.posterior(n) - ie.posterior(n)).abs().max(), 1e-8)
        }
      }
    }

    const gum::Potential< double > pAC(const gum::MarkovRandomField< double >& mn) {
      gum::Potential< double > joint;
      for (auto& [nods, factor]: mn.factors()) {
        joint *= *factor;
      }
      joint.normalize();
      return joint.sumOut({&mn.variable("B")});
    }

    const gum::Potential< double > pAB(const gum::MarkovRandomField< double >& mn) {
      gum::Potential< double > joint;
      for (auto& [nods, factor]: mn.factors()) {
        joint *= *factor;
      }
      joint.normalize();
      return joint.sumOut({&mn.variable("C")});
    }

    GUM_ACTIVE_TEST(JointTargetFromExistingJoint) {
      // explicit jointtarget
      // gum::Potential< double > p;
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C");

        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.makeInference();
        TS_ASSERT_THROWS(auto p = ie.jointPosterior({0, 2}), const gum::UndefinedElement&)
      }
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C");
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.addJointTarget({0, 2});
        ie.makeInference();
        auto p = ie.jointPosterior({0, 2});
        TS_GUM_POTENTIAL_DELTA(p, pAC(mn), TS_GUM_SMALL_ERROR)
      }
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C");
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.addJointTarget({0, 2});

        try {
          auto p = ie.jointPosterior({0, 2});
          TS_GUM_POTENTIAL_DELTA(p, pAC(mn), TS_GUM_SMALL_ERROR)
        } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
      }
      // implicit jointtarget as factor
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C");
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.makeInference();
        auto p = ie.jointPosterior({0, 1});
        TS_GUM_POTENTIAL_DELTA(p, pAB(mn), TS_GUM_SMALL_ERROR)
      }
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C");
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.addJointTarget({0, 1});
        ie.makeInference();
        auto p = ie.jointPosterior({0, 1});
        TS_GUM_POTENTIAL_DELTA(p, pAB(mn), TS_GUM_SMALL_ERROR)
      }
      // implicit jointtarget as subset of clique in junction tree
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C;A--C");
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.makeInference();
        auto p = ie.jointPosterior({0, 2});
        TS_GUM_POTENTIAL_DELTA(p, pAC(mn), TS_GUM_SMALL_ERROR)
      }
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C;A--C");
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.addJointTarget({0, 2});
        ie.makeInference();
        auto p = ie.jointPosterior({0, 2});
        TS_GUM_POTENTIAL_DELTA(p, pAC(mn), TS_GUM_SMALL_ERROR)
      }
    }

    GUM_ACTIVE_TEST(IrrelevantSoftEvidence) {
      auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B--C;D--C--E");
      gum::Potential< double > psoft;
      gum::Potential< double > phard;

      auto hardevC = gum::Potential< double >();
      hardevC.add(mn.variable("C"));
      hardevC.fillWith({0, 1});

      {
        auto hardev = gum::Potential< double >();
        hardev.add(mn.variable("A"));
        hardev.fillWith({0, 1});

        gum::ShaferShenoyMRFInference ie(&mn);
        ie.addEvidence(hardevC);
        ie.addEvidence(hardev);
        ie.addTarget("E");
        TS_GUM_ASSERT_THROWS_NOTHING(ie.makeInference());
        phard = gum::Potential(ie.posterior("E"));
      }
      {
        auto softev = gum::Potential< double >();
        softev.add(mn.variable("A"));
        softev.fillWith({0.5, 1});

        gum::ShaferShenoyMRFInference ie(&mn);
        ie.addEvidence(hardevC);
        ie.addEvidence(softev);
        ie.addTarget("E");
        TS_GUM_ASSERT_THROWS_NOTHING(ie.makeInference());
        psoft = gum::Potential(ie.posterior("E"));
      }
      TS_GUM_POTENTIAL_DELTA(phard, psoft, TS_GUM_VERY_SMALL_ERROR)
    }
  };
}   // namespace gum_tests
