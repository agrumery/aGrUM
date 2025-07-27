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


#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/base/variables/rangeVariable.h>
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
        const std::string&           name   = bn.variable(n).name();
        const gum::Tensor< double >& postbn = iebn.posterior(name);

        gum::Tensor< double > postmn;
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
        const std::string&           name   = bn.variable(n).name();
        const gum::Tensor< double >& postbn = iebn.posterior(name);

        gum::Tensor< double > postmn;
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
        const std::string&           name   = bn.variable(n).name();
        const gum::Tensor< double >& postbn = iebn.posterior(name);

        gum::Tensor< double > postmn;
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
        const std::string&           name   = bn.variable(n).name();
        const gum::Tensor< double >& postbn = iebn.posterior(name);

        gum::Tensor< double > postmn;
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
        const std::string&           name   = bn.variable(n).name();
        const gum::Tensor< double >& postbn = iebn.posterior(name);

        gum::Tensor< double > postmn;
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
      gum::Tensor< double > Cwithout_evB(iemn.posterior("C"));
      gum::Tensor< double > Ewithout_evB(iemn.posterior("E"));

      iemn.eraseAllEvidence();
      iemn.addEvidence("A", 0);
      iemn.addEvidence("D", 1);
      iemn.addEvidence("B", 0);
      iemn.makeInference();
      gum::Tensor< double > Cwith_evB0(iemn.posterior("C"));
      gum::Tensor< double > Ewith_evB0(iemn.posterior("E"));

      iemn.eraseAllEvidence();
      iemn.addEvidence("A", 0);
      iemn.addEvidence("D", 1);
      iemn.addEvidence("B", 1);
      iemn.makeInference();
      gum::Tensor< double > Cwith_evB1(iemn.posterior("C"));
      gum::Tensor< double > Ewith_evB1(iemn.posterior("E"));

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

    const gum::Tensor< double > pAC(const gum::MarkovRandomField< double >& mn) {
      gum::Tensor< double > joint;
      for (auto& [nods, factor]: mn.factors()) {
        joint *= *factor;
      }
      joint.normalize();
      return joint.sumOut({&mn.variable("B")});
    }

    const gum::Tensor< double > pAB(const gum::MarkovRandomField< double >& mn) {
      gum::Tensor< double > joint;
      for (auto& [nods, factor]: mn.factors()) {
        joint *= *factor;
      }
      joint.normalize();
      return joint.sumOut({&mn.variable("C")});
    }

    GUM_ACTIVE_TEST(JointTargetFromExistingJoint) {
      // explicit jointtarget
      // gum::Tensor< double > p;
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
        TS_GUM_TENSOR_ALMOST_EQUALS(p, pAC(mn))
      }
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C");
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.addJointTarget({0, 2});

        try {
          auto p = ie.jointPosterior({0, 2});
          TS_GUM_TENSOR_ALMOST_EQUALS(p, pAC(mn))
        } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
      }
      // implicit jointtarget as factor
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C");
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.makeInference();
        auto p = ie.jointPosterior({0, 1});
        TS_GUM_TENSOR_ALMOST_EQUALS(p, pAB(mn))
      }
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C");
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.addJointTarget({0, 1});
        ie.makeInference();
        auto p = ie.jointPosterior({0, 1});
        TS_GUM_TENSOR_ALMOST_EQUALS(p, pAB(mn))
      }
      // implicit jointtarget as subset of clique in junction tree
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C;A--C");
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.makeInference();
        auto p = ie.jointPosterior({0, 2});
        TS_GUM_TENSOR_ALMOST_EQUALS(p, pAC(mn))
      }
      {
        auto mn = gum::MarkovRandomField< double >::fastPrototype("A--B;B--C;A--C");
        gum::ShaferShenoyMRFInference< double > ie(&mn);
        ie.addJointTarget({0, 2});
        ie.makeInference();
        auto p = ie.jointPosterior({0, 2});
        TS_GUM_TENSOR_ALMOST_EQUALS(p, pAC(mn))
      }
    }

    GUM_ACTIVE_TEST(IrrelevantSoftEvidence) {
      auto                  mn = gum::MarkovRandomField< double >::fastPrototype("A--B--C;D--C--E");
      gum::Tensor< double > psoft;
      gum::Tensor< double > phard;

      auto hardevC = gum::Tensor< double >();
      hardevC.add(mn.variable("C"));
      hardevC.fillWith({0, 1});

      {
        auto hardev = gum::Tensor< double >();
        hardev.add(mn.variable("A"));
        hardev.fillWith({0, 1});

        gum::ShaferShenoyMRFInference ie(&mn);
        ie.addEvidence(hardevC);
        ie.addEvidence(hardev);
        ie.addTarget("E");
        TS_GUM_ASSERT_THROWS_NOTHING(ie.makeInference());
        phard = gum::Tensor(ie.posterior("E"));
      }
      {
        auto softev = gum::Tensor< double >();
        softev.add(mn.variable("A"));
        softev.fillWith({0.5, 1});

        gum::ShaferShenoyMRFInference ie(&mn);
        ie.addEvidence(hardevC);
        ie.addEvidence(softev);
        ie.addTarget("E");
        TS_GUM_ASSERT_THROWS_NOTHING(ie.makeInference());
        psoft = gum::Tensor(ie.posterior("E"));
      }
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(phard, psoft)
    }
  };
}   // namespace gum_tests
