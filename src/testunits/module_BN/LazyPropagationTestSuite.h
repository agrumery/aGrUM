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
#pragma once


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/tools/relevantTensorsFinderType.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <agrum/base/core/math/math_utils.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {
  class GUM_TEST_SUITE(LazyPropagation) {
    public:
    gum::BayesNet< double >* _bn;
    gum::NodeId              i1;
    gum::NodeId              i2;
    gum::NodeId              i3;
    gum::NodeId              i4;
    gum::NodeId              i5;
    gum::Tensor< double >*   e_i1;
    gum::Tensor< double >*   e_i4;

    void setUp() final {
      _bn = new gum::BayesNet< double >();

      gum::LabelizedVariable n1("1", "", 2), n2("2", "", 2), n3("3", "", 2);
      gum::LabelizedVariable n4("4", "", 2), n5("5", "", 3);

      i1 = _bn->add(n1);
      i2 = _bn->add(n2);
      i3 = _bn->add(n3);
      i4 = _bn->add(n4);
      i5 = _bn->add(n5);

      _bn->addArc(i1, i3);
      _bn->addArc(i1, i4);
      _bn->addArc(i3, i5);
      _bn->addArc(i4, i5);
      _bn->addArc(i2, i4);
      _bn->addArc(i2, i5);

      e_i1 = new gum::Tensor< double >();
      (*e_i1) << _bn->variable(i1);
      e_i1->fill(0.0f);
      gum::Instantiation inst_1(*e_i1);
      inst_1.chgVal(_bn->variable(i1), 0);
      e_i1->set(inst_1, 1.0f);

      e_i4 = new gum::Tensor< double >();
      (*e_i4) << _bn->variable(i4);
      e_i4->fill(0.0f);
      gum::Instantiation inst_4(*e_i4);
      inst_4.chgVal(_bn->variable(i4), 1);
      e_i4->set(inst_4, 1.0f);
    }

    void tearDown() override {
      delete _bn;
      delete e_i1;
      delete e_i4;
    }

    [[nodiscard]] gum::Tensor< double > joint(const gum::BayesNet< double >& bn) {
      gum::Tensor< double > pot;
      for (const auto node: bn.dag()) {
        pot *= bn.cpt(node);
      }
      return pot;
    }

    // Testing when  no evidence
    GUM_ACTIVE_TEST(CreationAndInference) {
      fill(*_bn);
      // Testing the inference
      TS_ASSERT_THROWS_NOTHING({
        auto inf = new gum::LazyPropagation(_bn);
        inf->makeInference();
        delete inf;
      })
    }

    // Testing when no evidence
    GUM_ACTIVE_TEST(SequentialInference) {
      fill(*_bn);
      // Testing the inference
      gum::LazyPropagation inf(_bn);
      inf.setNumberOfThreads(1);
      TS_ASSERT(inf.getNumberOfThreads() == gum::Size(1))
      TS_ASSERT_THROWS_NOTHING(inf.makeInference())
    }

    GUM_ACTIVE_TEST(BigInference) {
      std::string             file = GET_RESSOURCES_PATH("bif/Munin1.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);
      auto                    nbrErr = static_cast< gum::Size >(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, static_cast< gum::Size >(0))

      gum::LazyPropagation inf(&bn);

      // const bool seq = true;
      const bool seq = false;
      if (seq) {
        inf.setNumberOfThreads(1);
      } else {
        inf.setNumberOfThreads(4);
      }

      inf.makeInference();
    }

    GUM_ACTIVE_TEST(Marginal1) {
      fill(*_bn);
      gum::LazyPropagation       inf(_bn);
      gum::ShaferShenoyInference inf2(_bn);
      inf.setNumberOfThreads(1);

      TS_ASSERT_THROWS_NOTHING(inf.makeInference())
      for (const auto node: _bn->dag()) {
        TS_ASSERT_THROWS_NOTHING(inf.posterior(node))
      }

      inf.makeInference();
      inf2.makeInference();

      const auto                               bn_joint = this->joint(*_bn);
      gum::Set< const gum::DiscreteVariable* > vars;

      for (const auto node: _bn->dag()) {
        vars.insert(&(_bn->variable(node)));
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.posterior(node), bn_joint.sumIn(vars))
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.posterior(node), inf2.posterior(node))
        vars.clear();
      }
    }

    GUM_ACTIVE_TEST(MarginalThreaded) {
      fill(*_bn);
      gum::LazyPropagation                 inf(_bn);
      gum::ShaferShenoyInference< double > inf2(_bn);

      TS_ASSERT_THROWS_NOTHING(inf.makeInference())
      for (const auto node: _bn->dag()) {
        TS_ASSERT_THROWS_NOTHING(inf.posterior(node))
      }

      inf.makeInference();
      inf2.makeInference();

      const auto                               bn_joint = this->joint(*_bn);
      gum::Set< const gum::DiscreteVariable* > vars;

      for (const auto node: _bn->dag()) {
        vars.insert(&(_bn->variable(node)));
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.posterior(node), bn_joint.sumIn(vars))
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.posterior(node), inf2.posterior(node))
        vars.clear();
      }
    }

    GUM_ACTIVE_TEST(MarginalWithEvidence) {
      fill(*_bn);
      gum::List< const gum::Tensor< double >* > e_list;
      e_list.insert(e_i1);
      e_list.insert(e_i4);

      gum::LazyPropagation       inf(_bn);
      gum::ShaferShenoyInference infX(_bn);
      auto                       bn_joint = this->joint(*_bn);

      for (auto pot: e_list) {
        TS_ASSERT_THROWS_NOTHING(inf.addEvidence(*pot))
        TS_ASSERT_THROWS_NOTHING(infX.addEvidence(*pot))
        bn_joint *= *pot;
      }

      TS_ASSERT_THROWS_NOTHING(inf.makeInference())
      for (const auto node: _bn->dag()) {
        TS_ASSERT_THROWS_NOTHING(inf.posterior(node))
      }

      TS_ASSERT_THROWS_NOTHING(infX.makeInference())

      gum::Set< const gum::DiscreteVariable* > vars;
      for (const auto node: _bn->dag()) {
        vars.insert(&(_bn->variable(node)));
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.posterior(node), bn_joint.sumIn(vars).normalize())
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.posterior(node), infX.posterior(node))
        vars.clear();
      }

      gum::LazyPropagation       inf2(_bn);
      gum::ShaferShenoyInference inf2X(_bn);
      bn_joint = this->joint(*_bn);

      gum::Tensor< double > ev1;
      const auto&           var1 = _bn->variable(i1);
      ev1 << var1;
      ev1.fillWith(0.0);
      gum::Instantiation inst1(ev1);
      inst1.chgVal(var1, 0);
      ev1.set(inst1, 1);
      TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(i1, 0))
      TS_ASSERT_THROWS_NOTHING(inf2X.addEvidence(i1, 0))
      bn_joint *= ev1;

      gum::Tensor< double > ev4;
      const auto&           var4 = _bn->variable(i4);
      ev4 << var4;
      ev4.fillWith(0.0);
      gum::Instantiation inst4(ev4);
      inst4.chgVal(var4, 1);
      ev4.set(inst4, 1);
      TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(i4, 1))
      TS_ASSERT_THROWS_NOTHING(inf2X.addEvidence(i4, 1))
      bn_joint *= ev4;

      TS_ASSERT_THROWS_NOTHING(inf2.makeInference())
      TS_ASSERT_THROWS_NOTHING(inf2X.makeInference())

      for (const auto node: _bn->dag()) {
        vars.insert(&(_bn->variable(node)));
        TS_ASSERT_THROWS_NOTHING(inf2.posterior(node))
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf2.posterior(node), bn_joint.sumIn(vars).normalize())
        TS_ASSERT_EQUALS(inf.posterior(node), inf2.posterior(node))
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf2.posterior(node), inf2X.posterior(node))
        vars.clear();
      }
    }

    // Testing when no evidence
    GUM_ACTIVE_TEST(Joint) {
      fill(*_bn);
      // Testing the inference
      gum::LazyPropagation inf(_bn);
      gum::NodeSet         nodeset;
      nodeset.insert(2);
      nodeset.insert(4);
      inf.addJointTarget(nodeset);

      TS_ASSERT_THROWS_NOTHING(inf.jointPosterior(nodeset))

      const auto                               bn_joint = this->joint(*_bn);
      gum::Set< const gum::DiscreteVariable* > vars;
      vars.insert(&(_bn->variable(2)));
      vars.insert(&(_bn->variable(4)));
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.jointPosterior(nodeset), bn_joint.sumIn(vars))
    }

    // Testing when no evidence
    GUM_ACTIVE_TEST(Joint2) {
      fill(*_bn);
      // Testing the inference
      gum::LazyPropagation inf(_bn);
      gum::NodeSet         nodeset;
      nodeset.insert(1);
      nodeset.insert(2);
      nodeset.insert(3);
      nodeset.insert(4);
      inf.eraseAllTargets();
      inf.addJointTarget(nodeset);

      gum::NodeSet nodeset2;
      nodeset2.insert(2);
      nodeset2.insert(4);

      const auto                               bn_joint = this->joint(*_bn);
      gum::Set< const gum::DiscreteVariable* > vars;
      vars.insert(&(_bn->variable(2)));
      vars.insert(&(_bn->variable(4)));
      TS_ASSERT_THROWS_NOTHING(inf.jointPosterior(nodeset2))
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.jointPosterior(nodeset2), bn_joint.sumIn(vars))

      TS_ASSERT_THROWS_NOTHING(inf.posterior(3))
      vars.clear();
      vars.insert(&(_bn->variable(3)));
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.posterior(3), bn_joint.sumIn(vars))

      vars.insert(&(_bn->variable(1)));
      vars.insert(&(_bn->variable(2)));
      vars.insert(&(_bn->variable(4)));
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.jointPosterior(nodeset), bn_joint.sumIn(vars))
    }

    // Testing information methods
    GUM_ACTIVE_TEST(InformationMethods) {
      fill(*_bn);

      gum::LazyPropagation inf(_bn);

      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::InformationTheory itheo(inf, gum::NodeSet({2}), gum::NodeSet({4})))
      gum::InformationTheory itheo(inf, gum::NodeSet({2}), gum::NodeSet({4}));
      TS_GUM_ASSERT_THROWS_NOTHING(inf.H((gum::NodeId)2))
      TS_GUM_ASSERT_THROWS_NOTHING(itheo.mutualInformationXY())
    }

    GUM_ACTIVE_TEST(InformationMethodsWithNames) {
      fill(*_bn);

      gum::LazyPropagation inf(_bn);
      TS_GUM_ASSERT_THROWS_NOTHING(inf.H(_bn->variable(2).name()))
    }

    GUM_ACTIVE_TEST(SmartManagementOfJointTarget) {
      fill(*_bn);

      gum::LazyPropagation inf(_bn);
      inf.addJointTarget(gum::NodeSet{0, 1, 2});
      inf.addJointTarget(gum::NodeSet{2, 3});
      TS_ASSERT_EQUALS(inf.nbrJointTargets(), static_cast< gum::Size >(2))

      // should not be added since {0,1,2} already exists
      inf.addJointTarget(gum::NodeSet{0, 1});
      TS_ASSERT_EQUALS(inf.nbrJointTargets(), static_cast< gum::Size >(2))

      // should remove {2,3} since {2,3,4} includes {2,3}
      inf.addJointTarget(gum::NodeSet{2, 3, 4});
      TS_ASSERT_EQUALS(inf.nbrJointTargets(), static_cast< gum::Size >(2))

      auto                                     bn_joint = this->joint(*_bn);
      gum::Set< const gum::DiscreteVariable* > vars;
      vars.insert(&(_bn->variable(0)));
      vars.insert(&(_bn->variable(1)));
      vars.insert(&(_bn->variable(2)));
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.jointPosterior(gum::NodeSet{0, 1, 2}),
                                           bn_joint.sumIn(vars).normalize())

      vars.clear();
      vars.insert(&(_bn->variable(2)));
      vars.insert(&(_bn->variable(3)));
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.jointPosterior(gum::NodeSet{2, 3}),
                                           bn_joint.sumIn(vars))

      vars.clear();
      vars.insert(&(_bn->variable(0)));
      vars.insert(&(_bn->variable(1)));
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.jointPosterior(gum::NodeSet{0, 1}),
                                           bn_joint.sumIn(vars))

      vars.clear();
      vars.insert(&(_bn->variable(2)));
      vars.insert(&(_bn->variable(3)));
      vars.insert(&(_bn->variable(4)));
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf.jointPosterior(gum::NodeSet{2, 3, 4}),
                                           bn_joint.sumIn(vars))
    }

    GUM_ACTIVE_TEST(EvidenceProbability) {
      fill(*_bn);
      gum::LazyPropagation inf(_bn);
      inf.setRelevantTensorsFinderType(gum::RelevantTensorsFinderType::FIND_ALL);
      inf.makeInference();
      auto p     = inf.posterior(0);
      auto I     = gum::Instantiation(p);
      auto proba = p.get(I);

      inf.addEvidence(0, 0);
      inf.makeInference();
      auto proba2 = inf.evidenceProbability();

      TS_ASSERT_DELTA(proba, proba2, TS_GUM_SMALL_ERROR)

      auto                  bn_joint = this->joint(*_bn);
      gum::Tensor< double > ev0;
      const auto&           var0 = _bn->variable(0);
      ev0 << var0;
      ev0.fillWith(0.0);
      gum::Instantiation inst0(ev0);
      inst0.chgVal(var0, 0);
      ev0.set(inst0, 1);
      bn_joint *= ev0;
      const auto proba3 = bn_joint.sumIn({&var0}).sum();

      TS_ASSERT_DELTA(proba, proba3, TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(EvidenceProbability2) {
      fill(*_bn);
      gum::LazyPropagation inf(_bn);
      inf.makeInference();
      auto p     = inf.posterior(0);
      auto I     = gum::Instantiation(p);
      auto proba = p.get(I);

      inf.addEvidence(0, 0);
      inf.makeInference();
      auto proba2 = inf.evidenceProbability();

      TS_ASSERT_DELTA(proba, proba2, TS_GUM_SMALL_ERROR)

      auto                  bn_joint = this->joint(*_bn);
      gum::Tensor< double > ev0;
      const auto&           var0 = _bn->variable(0);
      ev0 << var0;
      ev0.fillWith(0.0);
      gum::Instantiation inst0(ev0);
      inst0.chgVal(var0, 0);
      ev0.set(inst0, 1);
      bn_joint *= ev0;
      const auto proba3 = bn_joint.sumIn({&var0}).sum();

      TS_ASSERT_DELTA(proba, proba3, TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(EvidenceProbabilityAsia) {
      std::string             file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);
      auto                    nbrErr = static_cast< gum::Size >(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(0))

      auto id = bn.idFromName("lung_cancer");

      gum::LazyPropagation inf(&bn);
      inf.setRelevantTensorsFinderType(gum::RelevantTensorsFinderType::FIND_ALL);
      inf.makeInference();
      auto p     = inf.posterior(id);
      auto I     = gum::Instantiation(p);
      auto proba = p.get(I);

      inf.addEvidence(id, 0);
      inf.makeInference();
      auto proba2 = inf.evidenceProbability();

      TS_ASSERT_DELTA(proba, proba2, TS_GUM_SMALL_ERROR)

      auto                  bn_joint = this->joint(bn);
      gum::Tensor< double > ev0;
      const auto&           var0 = bn.variable(id);
      ev0 << var0;
      ev0.fillWith(0.0);
      gum::Instantiation inst0(ev0);
      inst0.chgVal(var0, 0);
      ev0.set(inst0, 1);
      bn_joint *= ev0;
      const auto proba3 = bn_joint.sumIn({&var0}).sum();

      TS_ASSERT_DELTA(proba, proba3, TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(Asia) {
      std::string             file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);
      auto                    nbrErr = static_cast< gum::Size >(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(0))

      const auto bn_joint = this->joint(bn);

      for (auto node: bn.dag()) {
        const auto&           variable = bn.variable(node);
        gum::Tensor< double > ev_pot;
        ev_pot << variable;
        ev_pot.fill(0.0f);
        gum::List< const gum::Tensor< double >* > evidences;
        evidences.insert(&ev_pot);

        gum::Instantiation inst(ev_pot);
        for (inst.setFirst(); !inst.end(); ++inst) {
          ev_pot.set(inst, 1.0f);
          gum::LazyPropagation                 inf1(&bn);
          gum::ShaferShenoyInference< double > inf2(&bn);
          auto                                 joint = bn_joint;

          for (auto pot: evidences) {
            TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot))
            TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot))
            joint *= *pot;
          }
          TS_ASSERT_THROWS_NOTHING(inf1.makeInference())
          TS_ASSERT_THROWS_NOTHING(inf2.makeInference())
          for (auto node2: bn.dag()) {
            TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf1.posterior(node2),
                                                 joint.sumIn({&bn.variable(node2)}).normalize())
            TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf1.posterior(node2), inf2.posterior(node2))
          }
          ev_pot.set(inst, static_cast< float >(0));
        }
      }
    }

    GUM_ACTIVE_TEST(Alarm) {
      std::string             file = GET_RESSOURCES_PATH("bif/alarm.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);
      auto                    nbrErr = static_cast< gum::Size >(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(0))

      gum::LazyPropagation               inf1(&bn);
      gum::VariableElimination< double > inf2(&bn);

      TS_ASSERT_THROWS_NOTHING(inf1.makeInference())
      TS_ASSERT_THROWS_NOTHING(inf2.makeInference())

      for (auto node: bn.dag()) {
        TS_ASSERT_THROWS_NOTHING(inf1.posterior(node))
        TS_ASSERT_THROWS_NOTHING(inf2.posterior(node))
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf1.posterior(node), inf2.posterior(node))
      }

      std::vector< gum::NodeId >                ev_nodes{2, 6, 7, 10, 12, 14, 16};
      gum::List< const gum::Tensor< double >* > evidences;
      for (const auto node: ev_nodes) {
        auto ev_pot = new gum::Tensor< double >;

        (*ev_pot) << bn.variable(node);
        ev_pot->fill(0.0f);
        gum::Instantiation inst(*ev_pot);
        if (node <= gum::Size(10)) {
          inst.chgVal(bn.variable(node), 0);
          ev_pot->set(inst, 1.0f);
        } else {
          inst.chgVal(bn.variable(node), 0);
          ev_pot->set(inst, 0.4f);
          inst.chgVal(bn.variable(node), 1);
          ev_pot->set(inst, 0.6f);
        }
        evidences.insert(ev_pot);
      }

      gum::LazyPropagation                 inf3(&bn);
      gum::ShaferShenoyInference< double > inf4(&bn);
      for (auto pot: evidences) {
        TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot))
        TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot))
        TS_ASSERT_THROWS_NOTHING(inf3.addEvidence(*pot))
        TS_ASSERT_THROWS_NOTHING(inf4.addEvidence(*pot))
      }

      TS_ASSERT_THROWS_NOTHING(inf1.makeInference())
      TS_ASSERT_THROWS_NOTHING(inf2.makeInference())
      TS_ASSERT_THROWS_NOTHING(inf3.makeInference())
      TS_ASSERT_THROWS_NOTHING(inf4.makeInference())

      for (auto node: bn.dag()) {
        TS_ASSERT_THROWS_NOTHING(inf1.posterior(node))
        TS_ASSERT_THROWS_NOTHING(inf2.posterior(node))
        TS_ASSERT_THROWS_NOTHING(inf3.posterior(node))
        TS_ASSERT_THROWS_NOTHING(inf4.posterior(node))
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf1.posterior(node), inf2.posterior(node))
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf1.posterior(node), inf3.posterior(node))
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf1.posterior(node), inf4.posterior(node))
      }

      gum::LazyPropagation inf5(&bn);
      inf5.setRelevantTensorsFinderType(gum::RelevantTensorsFinderType::DSEP_BAYESBALL_NODES);
      for (auto pot: evidences) {
        TS_ASSERT_THROWS_NOTHING(inf5.addEvidence(*pot))
      }
      TS_ASSERT_THROWS_NOTHING(inf5.makeInference())
      for (auto node: bn.dag()) {
        TS_ASSERT_THROWS_NOTHING(inf5.posterior(node))
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf1.posterior(node), inf5.posterior(node))
      }

      for (auto pot: evidences)
        delete pot;
    }

    GUM_ACTIVE_TEST(Asia2) {
      std::string             file = GET_RESSOURCES_PATH("bif/asia3.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);
      auto                    nbrErr = static_cast< gum::Size >(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(0))

      const auto bn_joint = this->joint(bn);

      for (auto node: bn.dag()) {
        const auto&           variable = bn.variable(node);
        gum::Tensor< double > ev_pot;
        ev_pot << variable;
        ev_pot.fill(0.0f);

        gum::Instantiation inst(ev_pot);
        for (inst.setFirst(); !inst.end(); ++inst) {
          ev_pot.set(inst, 1.0f);

          for (auto node2: bn.dag()) {
            if (node2 > node) {
              const auto&           variable2 = bn.variable(node2);
              gum::Tensor< double > ev_pot2;
              ev_pot2 << variable2;
              ev_pot2.fill(0.0f);

              gum::List< const gum::Tensor< double >* > evidences;
              evidences.insert(&ev_pot);
              evidences.insert(&ev_pot2);

              gum::Instantiation inst2(ev_pot2);
              for (inst2.setFirst(); !inst2.end(); ++inst2) {
                ev_pot2.set(inst2, 1.0f);

                gum::LazyPropagation                 inf1(&bn);
                gum::ShaferShenoyInference< double > inf2(&bn);
                auto                                 joint = bn_joint;
                for (auto pot: evidences) {
                  TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot))
                  TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot))
                  joint *= *pot;
                }
                TS_ASSERT_THROWS_NOTHING(inf1.makeInference())
                TS_ASSERT_THROWS_NOTHING(inf2.makeInference())

                for (auto xnode: bn.dag()) {
                  TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(
                      inf1.posterior(xnode),
                      joint.sumIn({&bn.variable(xnode)}).normalize())
                  TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf1.posterior(xnode), inf2.posterior(xnode))
                }
                ev_pot2.set(inst2, 0.0f);
              }
            }
          }

          ev_pot.set(inst, 0.0f);
        }
      }
    }

    GUM_ACTIVE_TEST(Asia3) {
      std::string             file = GET_RESSOURCES_PATH("bif/asia3.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);
      auto                    nbrErr = static_cast< gum::Size >(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(0))

      const auto bn_joint = this->joint(bn);

      for (auto node: bn.dag()) {
        const auto&           variable = bn.variable(node);
        gum::Tensor< double > ev_pot;
        ev_pot << variable;
        ev_pot.fill(0.0f);

        gum::Instantiation inst(ev_pot);
        for (inst.setFirst(); !inst.end(); ++inst) {
          ev_pot.set(inst, 1.0f);

          for (auto node2: bn.dag()) {
            if (node2 > node) {
              const auto&           variable2 = bn.variable(node2);
              gum::Tensor< double > ev_pot2;
              ev_pot2 << variable2;
              ev_pot2.fill(0.0f);

              gum::List< const gum::Tensor< double >* > evidences;
              evidences.insert(&ev_pot);
              evidences.insert(&ev_pot2);

              gum::Instantiation inst2(ev_pot2);
              for (inst2.setFirst(); !inst2.end(); ++inst2) {
                ev_pot2.set(inst2, 1.0f);

                gum::LazyPropagation inf1(&bn);
                inf1.setRelevantTensorsFinderType(
                    gum::RelevantTensorsFinderType::DSEP_BAYESBALL_NODES);
                gum::ShaferShenoyInference< double > inf2(&bn);
                auto                                 joint = bn_joint;
                for (auto pot: evidences) {
                  TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot))
                  TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot))
                  joint *= *pot;
                }
                TS_ASSERT_THROWS_NOTHING(inf1.makeInference())
                TS_ASSERT_THROWS_NOTHING(inf2.makeInference())

                for (auto xnode: bn.dag()) {
                  TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(
                      inf1.posterior(xnode),
                      joint.sumIn({&bn.variable(xnode)}).normalize())
                  TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(inf1.posterior(xnode), inf2.posterior(xnode))
                }
                ev_pot2.set(inst2, 0.0f);
              }
            }
          }

          ev_pot.set(inst, 0.0f);
        }
      }
    }

    GUM_ACTIVE_TEST(Asia4) {
      std::string             file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);
      auto                    nbrErr = static_cast< gum::Size >(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(0))

      const auto bn_joint = this->joint(bn);

      for (auto node: bn.dag()) {
        const auto&           variable = bn.variable(node);
        gum::Tensor< double > ev_pot;
        ev_pot << variable;
        ev_pot.fill(0.0f);

        gum::Instantiation inst(ev_pot);
        int                inst_index = 0;
        for (inst.setFirst(); !inst.end(); ++inst, ++inst_index) {
          ev_pot.set(inst, 1.0f);

          for (auto node2: bn.dag()) {
            if (node2 > node) {
              const auto&           variable2 = bn.variable(node2);
              gum::Tensor< double > ev_pot2;
              ev_pot2 << variable2;
              ev_pot2.fill(0.0f);

              gum::List< const gum::Tensor< double >* > evidences;
              evidences.insert(&ev_pot);
              evidences.insert(&ev_pot2);

              gum::Instantiation inst2(ev_pot2);
              int                inst2_index = 0;
              for (inst2.setFirst(); !inst2.end(); ++inst2, ++inst2_index) {
                ev_pot2.set(inst2, 1.0f);
                gum::LazyPropagation inf1(&bn);
                inf1.setRelevantTensorsFinderType(
                    gum::RelevantTensorsFinderType::DSEP_BAYESBALL_TENSORS);
                gum::LazyPropagation inf2(&bn);
                auto                 joint = bn_joint;

                for (auto pot: evidences) {
                  TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot))
                  TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot))
                  joint *= *pot;
                }
                TS_ASSERT_THROWS_NOTHING(inf1.makeInference())
                TS_ASSERT_THROWS_NOTHING(inf2.makeInference())

                for (auto xnode: bn.dag()) {
                  try {
                    const auto res = joint.sumIn({&bn.variable(xnode)});
                    if (res.sum() > TS_GUM_SMALL_ERROR) {
                      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(
                          inf1.posterior(xnode),
                          joint.sumIn({&bn.variable(xnode)}).normalize())
                    }
                  } catch (gum::IncompatibleEvidence&) {
                    // check evidence incompatibility:
                    if (node2 == gum::NodeId(2)) {
                      // node2 = tuberculos_or_cancer, then node =
                      // tuberculosis
                      TS_ASSERT((inst2_index == gum::Size(1)) && (inst_index == 0))
                    } else {
                      // node2 = lung_cancer & node =
                      // tuberculos_or_cancer
                      TS_ASSERT((inst2_index == 0) && (inst_index == gum::Size(1)))
                    }
                  }
                }
                ev_pot2.set(inst2, 0.0f);
              }
            }
          }

          ev_pot.set(inst, 0.0f);
        }
      }
    }

    GUM_ACTIVE_TEST(ChgEvidence) {
      std::string             file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);
      auto                    nbrErr = static_cast< gum::Size >(0);

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(0))

      const auto bn_joint = this->joint(bn);

      gum::LazyPropagation ie_0(&bn);
      ie_0.addTarget(0);        // visit_to_asia
      ie_0.addEvidence(1, 0);   // tuberculosis
      ie_0.makeInference();
      gum::Tensor< double > p_0 = ie_0.posterior(0);
      gum::Tensor< double > ev_pot0;
      const auto&           var1 = bn.variable(1);
      ev_pot0 << var1;
      ev_pot0.fillWith(0.0);
      gum::Instantiation inst0(ev_pot0);
      inst0.chgVal(var1, 0);
      ev_pot0.set(inst0, 1);
      auto joint0 = bn_joint * ev_pot0;

      gum::LazyPropagation ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Tensor< double > p_1 = ie_1.posterior(0);
      gum::Tensor< double > ev_pot1;
      ev_pot1 << var1;
      ev_pot1.fillWith(0.0);
      gum::Instantiation inst1(ev_pot1);
      inst1.chgVal(var1, 1);
      ev_pot1.set(inst1, 1);
      auto joint1 = bn_joint * ev_pot1;

      gum::LazyPropagation ie(&bn);
      ie.addTarget(0);
      ie.addEvidence(1, 0);
      ie.makeInference();
      TS_ASSERT_EQUALS(p_0, ie.posterior(0))
      const auto& var0 = bn.variable(0);
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.posterior(0), joint0.sumIn({&var0}).normalize())

      ie.chgEvidence(1, 1);
      ie.makeInference();
      TS_ASSERT_DIFFERS(p_0, ie.posterior(0))
      TS_ASSERT_EQUALS(p_1, ie.posterior(0))
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.posterior(0), joint1.sumIn({&var0}).normalize())
    }

    GUM_ACTIVE_TEST(ChgEvidence2) {
      std::string             file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);
      auto                    nbrErr = static_cast< gum::Size >(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(0))

      const auto bn_joint = this->joint(bn);

      gum::LazyPropagation ie_0(&bn);
      ie_0.addTarget(0);        // visit_to_asia
      ie_0.addEvidence(1, 0);   // tuberculosis
      ie_0.makeInference();
      gum::Tensor< double > p_0 = ie_0.posterior(0);
      gum::Tensor< double > ev_pot0;
      const auto&           var1 = bn.variable(1);
      ev_pot0 << var1;
      ev_pot0.fillWith(0.0);
      gum::Instantiation inst0(ev_pot0);
      inst0.chgVal(var1, 0);
      ev_pot0.set(inst0, 1);
      auto joint0 = bn_joint * ev_pot0;

      gum::LazyPropagation ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Tensor< double > p_1 = ie_1.posterior(0);
      gum::Tensor< double > ev_pot1;
      ev_pot1 << var1;
      ev_pot1.fillWith(0.0);
      gum::Instantiation inst1(ev_pot1);
      inst1.chgVal(var1, 1);
      ev_pot1.set(inst1, 1);
      auto joint1 = bn_joint * ev_pot1;

      gum::LazyPropagation ie(&bn);
      ie.eraseAllTargets();
      ie.addTarget(0);
      ie.addEvidence(1, 0);
      ie.makeInference();
      TS_ASSERT_EQUALS(p_0, ie.posterior(0))
      const auto& var0 = bn.variable(0);
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.posterior(0), joint0.sumIn({&var0}).normalize())

      ie.chgEvidence(1, 1);
      ie.makeInference();
      TS_ASSERT_DIFFERS(p_0, ie.posterior(0))
      TS_ASSERT_EQUALS(p_1, ie.posterior(0))
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.posterior(0), joint1.sumIn({&var0}).normalize())
    }

    GUM_ACTIVE_TEST(StaticEvidenceImpact) {
      std::string             file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);
      auto                    nbrErr = static_cast< gum::Size >(0);

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(0))

      gum::LazyPropagation ie_all(&bn);
      TS_ASSERT_THROWS(ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{0, 1, 2}),
                       const gum::InvalidArgument&)

      auto res = ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{1, 2});

      TS_ASSERT_EQUALS(res.nbrDim(), static_cast< gum::Size >(2))   // 2 indep 0 given 1

      gum::LazyPropagation ie_0(&bn);
      ie_0.addTarget(0);                                            // visit_to_asia
      ie_0.addEvidence(1, 0);                                       // tuberculosis
      ie_0.makeInference();
      gum::Tensor< double > p_0 = ie_0.posterior(0);

      gum::LazyPropagation ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Tensor< double > p_1 = ie_1.posterior(0);

      gum::Instantiation i;
      i.add(bn.variable(1));
      i.setFirst();
      TS_ASSERT_EQUALS(p_0, res.extract(i))
      i.inc();
      TS_ASSERT_EQUALS(p_1, res.extract(i))
    }

    GUM_ACTIVE_TEST(EvidenceImpactWithNames) {
      std::string             file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double > bn;
      gum::BIFReader          reader(&bn, file);
      auto                    nbrErr = static_cast< gum::Size >(0);

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(reader.warnings(), static_cast< gum::Size >(0))


      gum::LazyPropagation ie_all(&bn);
      TS_ASSERT_THROWS(ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{0, 1, 2}),
                       const gum::InvalidArgument&)

      TS_ASSERT_THROWS(ie_all.evidenceImpact("visit_to_asia", {"tuberculosis", "toto"}),
                       const gum::NotFound&)

      auto res = ie_all.evidenceImpact("visit_to_Asia", {"tuberculosis", "tuberculos_or_cancer"});

      TS_ASSERT_EQUALS(res.nbrDim(), static_cast< gum::Size >(2))   // 2 indep 0 given 1

      gum::LazyPropagation ie_0(&bn);
      ie_0.addTarget(0);                                            // visit_to_asia
      ie_0.addEvidence(1, 0);                                       // tuberculosis
      ie_0.makeInference();
      gum::Tensor< double > p_0 = ie_0.posterior(0);

      gum::LazyPropagation ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Tensor< double > p_1 = ie_1.posterior(0);

      gum::Instantiation i;
      i.add(bn.variable(1));
      i.setFirst();
      TS_ASSERT_EQUALS(p_0, res.extract(i))
      i.inc();
      TS_ASSERT_EQUALS(p_1, res.extract(i))
    }

    GUM_ACTIVE_TEST(EvidenceImpact) {
      /*
      F  A
      \ / \
       B   |
       |   E
       C   |
      / \ /
      H  D
      */
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D;A->E->D;F->B;C->H;");

      gum::LazyPropagation  ie(&bn);
      gum::Tensor< double > res;
      TS_GUM_ASSERT_THROWS_NOTHING(res = ie.evidenceImpact("E", {"A", "B", "C", "D", "F"}))
      TS_ASSERT_EQUALS(res.nbrDim(), static_cast< gum::Size >(4))   // MarkovBlanket(E)=(A,D,C)
      try {
        auto joint = bn.cpt("A") * bn.cpt("B") * bn.cpt("C") * bn.cpt("D") * bn.cpt("E")
                   * bn.cpt("F") * bn.cpt("H");
        auto pADCE = joint.sumIn({&bn.variableFromName("A"),
                                  &bn.variableFromName("C"),
                                  &bn.variableFromName("D"),
                                  &bn.variableFromName("E")});
        auto pADC  = pADCE.sumOut({&bn.variableFromName("E")});
        TS_ASSERT_EQUALS(res, pADCE / pADC)
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(JointWithHardEvidence) {
      /*
      F  A
      \ / \
       B   |
       |   E
       C   |
      / \ /
      H  D
      */
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D;A->E->D;F->B;C->H;");

      gum::LazyPropagation ie(&bn);
      ie.addEvidence("B", 0);
      gum::NodeSet joint{bn.idFromName("A"), bn.idFromName("B"), bn.idFromName("D")};
      gum::Set< const gum::DiscreteVariable* > xjoint{&bn.variableFromName("A"),
                                                      &bn.variableFromName("B"),
                                                      &bn.variableFromName("D")};

      gum::Tensor< double > ev_potB;
      const auto&           varB = bn.variableFromName("B");
      ev_potB << varB;
      ev_potB.fillWith(0.0);
      gum::Instantiation instB(ev_potB);
      instB.chgVal(varB, 0);
      ev_potB.set(instB, 1);
      const auto pjoint = this->joint(bn) * ev_potB;

      ie.addJointTarget(joint);
      ie.makeInference();
      try {
        auto p = ie.jointPosterior(joint);
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(p, pjoint.sumIn(xjoint).normalize())
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e)
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(JointEvidenceImpact) {
      /*
      F  A
      \ / \
       B   |
       |   E
       C   |
      / \ /
      H  D
      */
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D;A->E->D;F->B;C->H;");

      gum::LazyPropagation  ie(&bn);
      gum::Tensor< double > res;
      TS_GUM_ASSERT_THROWS_NOTHING(res = ie.evidenceJointImpact({"D", "E"}, {"A", "B", "C", "F"}))
      TS_ASSERT_EQUALS(res.nbrDim(), static_cast< gum::Size >(4))   // MarkovBlanket(E)=(A,D,C)
      try {
        auto joint = bn.cpt("A") * bn.cpt("B") * bn.cpt("C") * bn.cpt("D") * bn.cpt("E")
                   * bn.cpt("F") * bn.cpt("H");
        auto pADCE = joint.sumIn({&bn.variableFromName("A"),
                                  &bn.variableFromName("C"),
                                  &bn.variableFromName("D"),
                                  &bn.variableFromName("E")});
        auto pAC   = pADCE.sumOut({&bn.variableFromName("D"), &bn.variableFromName("E")});
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(res, pADCE / pAC)
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    GUM_ACTIVE_TEST(JointMutualInformation) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D;A->E->D;F->B;C->H;");

      gum::LazyPropagation ie(&bn);
      ie.makeInference();

      TS_ASSERT_THROWS(ie.jointMutualInformation(gum::NodeSet{0}), const gum::InvalidArgument&)
      gum::InformationTheory itheo(ie, gum::NodeSet{0}, gum::NodeSet{1});
      TS_ASSERT_DELTA(itheo.mutualInformationXY(),
                      ie.jointMutualInformation(gum::NodeSet{0, 1}),
                      1e-7)

      ie.addJointTarget({1, 4, 3});
      ie.makeInference();
      double byHandJMI;
      double JMI;

      byHandJMI = -ie.jointPosterior({1, 3, 4}).entropy();
      byHandJMI += ie.jointPosterior({1, 4}).entropy() + ie.jointPosterior({1, 3}).entropy()
                 + ie.jointPosterior({4, 3}).entropy();
      byHandJMI
          -= ie.posterior(1).entropy() + ie.posterior(4).entropy() + ie.posterior(3).entropy();

      //@todo why do I need to create a new LazyPropagation
      gum::LazyPropagation ie2(&bn);
      JMI = ie2.jointMutualInformation({1, 3, 4});

      TS_ASSERT_DELTA(JMI, byHandJMI, 1e-7)

      ie.clear();
      ie.addJointTarget({0, 1, 2, 3});
      ie.makeInference();
      byHandJMI = -ie.jointPosterior({0, 1, 2, 3}).entropy();
      byHandJMI += ie.jointPosterior({0, 1, 2}).entropy() + ie.jointPosterior({0, 1, 3}).entropy()
                 + ie.jointPosterior({0, 2, 3}).entropy() + ie.jointPosterior({1, 2, 3}).entropy();
      byHandJMI -= ie.jointPosterior({0, 1}).entropy() + ie.jointPosterior({0, 2}).entropy()
                 + ie.jointPosterior({0, 3}).entropy() + ie.jointPosterior({1, 2}).entropy()
                 + ie.jointPosterior({1, 3}).entropy() + ie.jointPosterior({2, 3}).entropy();
      byHandJMI += ie.posterior(0).entropy() + ie.posterior(1).entropy() + ie.posterior(2).entropy()
                 + ie.posterior(3).entropy();

      //@todo why do I need to create a new LazyPropagation
      gum::LazyPropagation ie3(&bn);
      JMI = ie3.jointMutualInformation({0, 1, 2, 3});

      TS_ASSERT_DELTA(JMI, byHandJMI, 1e-7)
    }

    GUM_ACTIVE_TEST(LogitInBN) {
      gum::BayesNet< double > bn;

      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);
      gum::LabelizedVariable oneMore("OneMore", "", 2);
      gum::LabelizedVariable oneMoreParent1("OneMoreParent1", "", 2);
      gum::LabelizedVariable oneMoreParent2("OneMoreParent2", "", 2);

      gum::NodeId idCold    = bn.add(cold);
      gum::NodeId idFlu     = bn.add(flu);
      gum::NodeId idMalaria = bn.add(malaria);
      gum::NodeId idFever   = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(idFever = bn.addLogit(fever, 0.3f))
      gum::NodeId idOneMore        = bn.add(oneMore);
      gum::NodeId idOneMoreParent1 = bn.add(oneMoreParent1);
      gum::NodeId idOneMoreParent2 = bn.add(oneMoreParent2);

      bn.addWeightedArc(idMalaria, idFever, 0.9f);
      bn.addWeightedArc(idFlu, idFever, 0.8f);
      bn.addWeightedArc(idCold, idFever, 0.4f);

      TS_ASSERT_THROWS(bn.addWeightedArc(idMalaria, idCold, 0.8f), const gum::InvalidArc&)

      const gum::Tensor< double >& pOneMoreParent1 = bn.cpt(idOneMoreParent1);
      // FILLING PARAMS
      pOneMoreParent1.fillWith({0.2f, 0.8f});

      const gum::Tensor< double >& pOneMoreParent2 = bn.cpt(idOneMoreParent2);
      // FILLING PARAMS
      pOneMoreParent2.fillWith({0.3f, 0.7f});

      bn.addArc(idOneMoreParent1, idOneMore);
      bn.addArc(idFever, idOneMore);
      bn.addArc(idOneMoreParent2, idOneMore);
      const gum::Tensor< double >& pOneMore = bn.cpt(idOneMore);
      const gum::Tensor< double >& p        = bn.cpt(idFever);

      gum::Instantiation i(p);

      // FILLING PARAMS
      // clang-format off
      pOneMore.fillWith(
                        {0.1f,0.9f,
                         0.8f,0.2f,
                         0.1f,0.9f,
                         0.8f,0.2f,
                         0.1f,0.9f,
                         0.8f,0.2f,
                         0.1f,0.9f,
                         0.8f,0.2f});

      float witness[] =
                        {0.42555748318834097f,0.574442516811659f,
                         0.23147521650098246f,0.7685247834990175f,
                         0.2497398944048823f,0.7502601055951177f,
                         0.11920292202211769f,0.8807970779778823f,
                         0.33181222783183384f,0.6681877721681662f,
                         0.16798161486607555f,0.8320183851339245f,
                         0.18242552380635635f,0.8175744761936437f,
                         0.08317269649392234f,0.9168273035060777f};
      // clang-format on

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6)
      }

      gum::LazyPropagation inf_LazyProp(&bn);

      inf_LazyProp.makeInference();
    }

    GUM_ACTIVE_TEST(MultipleJointMutualInformation) {
      auto                 bn = gum::BayesNet< double >::fastPrototype("A->B;A->C->D");
      gum::LazyPropagation ie(&bn);

      const auto bn_joint = this->joint(bn);

      try {
        for (const auto& a: bn.arcs()) {
          ie.eraseAllTargets();
          ie.eraseAllEvidence();
          // GUM_TRACE_VAR(a);
          ie.addJointTarget({a.first(), a.second()});
          // GUM_TRACE_VAR(ie.jointTargets());
          ie.makeInference();
          const auto po = ie.jointPosterior({a.first(), a.second()});

          gum::Set< const gum::DiscreteVariable* > set{&bn.variable(a.first()),
                                                       &bn.variable(a.second())};
          TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(po, bn_joint.sumIn(set).normalize())
        }
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
      /*
      try {
        for (const auto& a : _bn.arcs()) {
          std::cout << a << " : " << std::flush
                    << ie.jointMutualInformation({a.first(), a.second()})
                    << std::endl;
        }
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
       */
    }

    GUM_ACTIVE_TEST(AdaptiveNbThreads) {
      fill(*_bn);
      // Testing the inference
      gum::LazyPropagation inf(_bn);

      TS_ASSERT_EQUALS(inf.getNumberOfThreads(), gum::getNumberOfThreads())
      inf.setNumberOfThreads(gum::Size(10));
      TS_ASSERT_EQUALS(inf.getNumberOfThreads(), gum::Size(10))
      gum::setNumberOfThreads(25);
      TS_ASSERT_EQUALS(inf.getNumberOfThreads(), gum::Size(10))
      inf.setNumberOfThreads(0);
      TS_ASSERT_EQUALS(inf.getNumberOfThreads(), gum::getNumberOfThreads())
      gum::setNumberOfThreads(32);
      TS_ASSERT_EQUALS(inf.getNumberOfThreads(), gum::getNumberOfThreads())
    }

    GUM_ACTIVE_TEST(ProbaEvidence) {
      std::vector< gum::LabelizedVariable* > vars(gum::Size(10));

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 2);
      }

      gum::BayesNet< double > bn;
      gum::NodeId             i0 = bn.add(*vars[0]);
      auto&                   x0 = bn.variable(i0);
      auto&                   p0 = bn.cpt(i0);
      p0.fillWith({0.7, 0.3});

      gum::Tensor< double > ev0;
      ev0 << x0;
      ev0.fillWith({0.3, 0.2});

      gum::Tensor< double > ev0b;
      ev0b << x0;
      ev0b.fillWith({1.0, 0.0});

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0);
        double xpe0 = inf.evidenceProbability();
        double pe0  = (p0 * ev0).sum();
        TS_ASSERT_DELTA(pe0, 0.27, 0.0001)
        TS_ASSERT_DELTA(xpe0, pe0, 0.0001)
      }

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0b);
        double xpe0b = inf.evidenceProbability();
        double pe0b  = (p0 * ev0b).sum();
        TS_ASSERT_DELTA(pe0b, 0.7, 0.0001)
        TS_ASSERT_DELTA(xpe0b, pe0b, 0.0001)
      }

      gum::NodeId ii1 = bn.add(*vars[1]);
      auto&       x1  = bn.variable(ii1);
      auto&       p1  = bn.cpt(ii1);
      p1.fillWith({0.4, 0.6});

      gum::Tensor< double > ev1;
      ev1 << x1;
      ev1.fillWith({0.4, 0.6});

      gum::Tensor< double > ev1b;
      ev1b << x1;
      ev1b.fillWith({0.0, 1.0});

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        double xpe01 = inf.evidenceProbability();
        double pe01  = (p0 * ev0 * p1 * ev1).sum();
        TS_ASSERT_DELTA(pe01, 0.27 * 0.52, 0.0001)
        TS_ASSERT_DELTA(xpe01, pe01, 0.0001)
      }

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0b);
        inf.addEvidence(ev1);
        double xpe01 = inf.evidenceProbability();
        double pe01  = (p0 * ev0b * p1 * ev1).sum();
        TS_ASSERT_DELTA(pe01, 0.7 * 0.52, 0.0001)
        TS_ASSERT_DELTA(xpe01, pe01, 0.0001)
      }

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1b);
        double xpe01 = inf.evidenceProbability();
        double pe01  = (p0 * ev0 * p1 * ev1b).sum();
        TS_ASSERT_DELTA(pe01, 0.27 * 0.6, 0.0001)
        TS_ASSERT_DELTA(xpe01, pe01, 0.0001)
      }

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0b);
        inf.addEvidence(ev1b);
        double xpe01 = inf.evidenceProbability();
        double pe01  = (p0 * ev0b * p1 * ev1b).sum();
        TS_ASSERT_DELTA(pe01, 0.7 * 0.6, 0.0001)
        TS_ASSERT_DELTA(xpe01, pe01, 0.0001)
      }

      gum::NodeId ii2 = bn.add(*vars[2]);
      bn.addArc(i0, ii2);
      auto& x2 = bn.variable(ii2);
      auto& p2 = bn.cpt(ii2);
      p2.fillWith({0.4, 0.6, 0.8, 0.2});

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        double xpe02 = inf.evidenceProbability();
        double pe02  = (p0 * ev0 * p1 * ev1 * p2).sum();
        TS_ASSERT_DELTA(pe02, 0.27 * 0.52, 0.0001)
        TS_ASSERT_DELTA(xpe02, pe02, 0.0001)
      }

      gum::NodeId ii3 = bn.add(*vars[3]);
      bn.addArc(ii2, ii3);
      auto& x3 = bn.variable(ii3);
      auto& p3 = bn.cpt(ii3);
      p3.fillWith({0.1, 0.9, 0.7, 0.3});

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        double xpe03 = inf.evidenceProbability();
        double pe03  = (p0 * ev0 * p1 * ev1 * p2 * p3).sum();
        TS_ASSERT_DELTA(pe03, 0.27 * 0.52, 0.0001)
        TS_ASSERT_DELTA(xpe03, pe03, 0.0001)
      }

      gum::Tensor< double > ev3;
      ev3 << x3;
      ev3.fillWith({0.4, 0.3});

      gum::Tensor< double > ev3b;
      ev3b << x3;
      ev3b.fillWith({1.0, 0.0});

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        inf.addEvidence(ev3);
        double xpe0_3 = inf.evidenceProbability();
        double pe0_3  = (p0 * ev0 * p1 * ev1 * p2 * p3 * ev3).sum();

        // P(x0,x3) = [X00: [0.3220, 0.3780], x01: [0.0660, 0.2340]]
        double p03 = 0.322 * 0.12 + 0.378 * 0.09 + 0.066 * 0.08 + 0.234 * .06;
        TS_ASSERT_DELTA(pe0_3, p03 * 0.52, 0.0001)
        TS_ASSERT_DELTA(xpe0_3, pe0_3, 0.0001)
      }

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        inf.addEvidence(ev3b);
        double xpe0_3 = inf.evidenceProbability();
        double pe0_3  = (p0 * ev0 * p1 * ev1 * p2 * p3 * ev3b).sum();

        // P(x0,x3) = [X00: [0.3220, 0.3780], x01: [0.0660, 0.2340]]
        double p03 = 0.322 * 0.3 + 0.066 * 0.2;
        TS_ASSERT_DELTA(pe0_3, p03 * 0.52, 0.0001)
        TS_ASSERT_DELTA(xpe0_3, pe0_3, 0.0001)
      }

      gum::NodeId ii4 = bn.add(*vars[4]);
      bn.addArc(ii1, ii4);
      bn.addArc(ii3, ii4);
      auto& x4 = bn.variable(ii4);
      auto& p4 = bn.cpt(ii4);
      p4.fillWith({0.2, 0.8, 0.3, 0.7, 0.4, 0.6, 0.5, 0.5});

      gum::NodeId ii5 = bn.add(*vars[5]);
      bn.addArc(ii3, ii5);
      auto& x5 = bn.variable(ii5);
      auto& p5 = bn.cpt(ii5);
      p5.fillWith({0.7, 0.3, 0.8, 0.2});

      gum::Tensor< double > ev5;
      ev5 << x5;
      ev5.fillWith({0.7, 0.6});

      gum::Tensor< double > pot0_5;
      {
        gum::LazyPropagation inf(&bn);
        inf.addJointTarget({i0, ii1, ii5});
        pot0_5 = inf.jointPosterior({i0, ii1, ii5});
      }

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        inf.addEvidence(ev5);

        double xpe0_5 = inf.evidenceProbability();
        double pe0_5  = (pot0_5 * ev0 * ev1 * ev5).sum();
        TS_ASSERT_DELTA(xpe0_5, pe0_5, 0.0001)
      }

      gum::NodeId i6 = bn.add(*vars[6]);
      auto&       x6 = bn.variable(i6);
      auto&       p6 = bn.cpt(i6);
      p6.fillWith({0.8, 0.2});

      gum::Tensor< double > ev6;
      ev6 << x6;
      ev6.fillWith({0.7, 0.6});

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        inf.addEvidence(ev5);
        inf.addEvidence(ev6);

        double xpe0_6 = inf.evidenceProbability();
        double pe0_6  = (pot0_5 * ev0 * ev1 * ev5 * p6 * ev6).sum();
        TS_ASSERT_DELTA(xpe0_6, pe0_6, 0.0001)
      }

      gum::NodeId i7 = bn.add(*vars[7]);
      bn.addArc(ii4, i7);
      auto& x7 = bn.variable(i7);
      auto& p7 = bn.cpt(i7);
      p7.fillWith({0.4, 0.6, 0.6, 0.4});

      gum::Tensor< double > ev7;
      ev7 << x7;
      ev7.fillWith({0.2, 0.5});

      gum::Tensor< double > pot0_7;
      {
        gum::LazyPropagation inf(&bn);
        inf.addJointTarget({i0, ii1, ii5, i7});
        pot0_7 = inf.jointPosterior({i0, ii1, ii5, i7});
      }

      {
        gum::LazyPropagation inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        inf.addEvidence(ev5);
        inf.addEvidence(ev6);
        inf.addEvidence(ev7);

        double xpe0_7 = inf.evidenceProbability();
        double pe0_7  = (pot0_7 * ev0 * ev1 * ev5 * ev7 * p6 * ev6).sum();
        TS_ASSERT_DELTA(xpe0_7, pe0_7, 0.0001)
      }

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(OnChanged) {
      std::vector< gum::LabelizedVariable* > vars(5);

      for (gum::Idx i = 0; i < 5; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 2);
      }

      gum::BayesNet< double > xbn;

      gum::NodeId n0 = xbn.add(*vars[0]);
      gum::NodeId n1 = xbn.add(*vars[1]);
      gum::NodeId n2 = xbn.add(*vars[2]);
      gum::NodeId n3 = xbn.add(*vars[3]);
      gum::NodeId n4 = xbn.add(*vars[4]);

      xbn.addArc(n0, n2);
      xbn.addArc(n1, n2);
      xbn.addArc(n2, n3);
      xbn.addArc(n3, n4);

      auto& x0 = xbn.variable(n0);
      auto& x1 = xbn.variable(n1);
      auto& x2 = xbn.variable(n2);
      auto& x3 = xbn.variable(n3);
      auto& x4 = xbn.variable(n4);

      auto& p0 = xbn.cpt(n0);
      auto& p1 = xbn.cpt(n1);
      auto& p2 = xbn.cpt(n2);
      auto& p3 = xbn.cpt(n3);
      auto& p4 = xbn.cpt(n4);

      p0.fillWith({0.7, 0.3});
      p1.fillWith({0.4, 0.6});
      p2.fillWith({0.2, 0.8, 0.3, 0.7, 0.4, 0.6, 0.5, 0.5});
      p3.fillWith({0.8, 0.2, 0.2, 0.8});
      p4.fillWith({0.4, 0.6, 0.3, 0.7});

      {
        gum::LazyPropagation inf(&xbn);
        inf.addTarget("x0");
        TS_ASSERT(inf.targets().size() == gum::Size(1))
        inf.makeInference();
        TS_ASSERT_THROWS(inf.posterior(n3), const gum::UndefinedElement&)
        inf.addTarget("x3");
        const auto& pot3 = inf.posterior(n3);
        TS_ASSERT(inf.junctionTree()->sizeNodes() == gum::Size(2))

        gum::VariableSet            set3{&x0, &x1, &x2, &x4};
        const gum::Tensor< double > pot3bis = (p0 * p1 * p2 * p3 * p4).sumOut(set3).normalize();
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(pot3, pot3bis)
      }

      {
        gum::LazyPropagation inf(&xbn);
        inf.addTarget("x0");
        inf.addTarget("x2");

        TS_ASSERT(inf.targets().size() == gum::Size(2))
        inf.makeInference();

        TS_ASSERT(inf.junctionTree()->sizeNodes() == gum::Size(1))

        inf.addEvidence(n4, 1);
        inf.makeInference();

        const auto& pot2 = inf.posterior(n2);
        TS_ASSERT(inf.junctionTree()->sizeNodes() == gum::Size(2))

        gum::VariableSet      set2{&x0, &x1, &x3, &x4};
        gum::Tensor< double > ev4;
        ev4 << x4;
        ev4.fillWith({0.0, 1.0});

        const gum::Tensor< double > pot2bis
            = (p0 * p1 * p2 * p3 * p4 * ev4).sumOut(set2).normalize();

        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(pot2, pot2bis)
      }

      {
        gum::LazyPropagation inf(&xbn);
        inf.addTarget("x0");
        TS_ASSERT(inf.targets().size() == gum::Size(1))
        inf.makeInference();
        TS_ASSERT_THROWS(inf.posterior(n2), const gum::UndefinedElement&)
        TS_ASSERT(inf.junctionTree()->sizeNodes() == gum::Size(1))

        inf.addTarget("x2");
        inf.addEvidence(n4, 1);
        const auto& pot2 = inf.posterior(n2);
        TS_ASSERT(inf.junctionTree()->sizeNodes() == gum::Size(2))

        gum::VariableSet      set2{&x0, &x1, &x3, &x4};
        gum::Tensor< double > ev4;
        ev4 << x4;
        ev4.fillWith({0.0, 1.0});
        const gum::Tensor< double > pot2bis
            = (p0 * p1 * p2 * p3 * p4 * ev4).sumOut(set2).normalize();
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(pot2, pot2bis)
      }

      {
        gum::LazyPropagation inf(&xbn);
        inf.addTarget("x0");
        TS_ASSERT(inf.targets().size() == gum::Size(1))
        inf.makeInference();
        TS_ASSERT_THROWS(inf.posterior(n2), const gum::UndefinedElement&)
        TS_ASSERT(inf.junctionTree()->sizeNodes() == gum::Size(1))

        inf.addTarget("x2");
        inf.addEvidence(n4, 1);
        inf.makeInference();
        const auto& pot2 = inf.posterior(n2);
        TS_ASSERT(inf.junctionTree()->sizeNodes() == gum::Size(2))

        gum::VariableSet      set2{&x0, &x1, &x3, &x4};
        gum::Tensor< double > ev4;
        ev4 << x4;
        ev4.fillWith({0.0, 1.0});
        const gum::Tensor< double > pot2bis
            = (p0 * p1 * p2 * p3 * p4 * ev4).sumOut(set2).normalize();
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(pot2, pot2bis)
      }

      for (const auto var: vars)
        delete var;
    }

    GUM_ACTIVE_TEST(ImplicitTarget) {
      // there always is a clique containing a node and its parent
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->D");

      // {0,1} is an implicit target
      gum::LazyPropagation ie(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(ie.jointPosterior(gum::NodeSet{0, 1}))
      TS_ASSERT_THROWS(ie.jointPosterior(gum::NodeSet{0, 3}), gum::UndefinedElement&)

      auto p1 = ie.jointPosterior(gum::NodeSet{0, 1});

      // expliciting target {0,1}
      gum::LazyPropagation ie2(&bn);
      ie2.addJointTarget((gum::NodeSet{0, 1}));
      auto p2 = ie2.jointPosterior(gum::NodeSet{0, 1});

      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(p1, p2)   // no diff !
    }

    GUM_ACTIVE_TEST(ImplicitTarget2) {
      // there always is a clique containing the parents of the same node
      auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D");

      // {0,2} is an implicit target
      gum::LazyPropagation ie(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(ie.jointPosterior(gum::NodeSet{0, 2}))
      TS_ASSERT_THROWS(ie.jointPosterior(gum::NodeSet{0, 3}), gum::UndefinedElement&)

      auto p1 = ie.jointPosterior(gum::NodeSet{0, 2});

      // expliciting target {0,2}
      gum::LazyPropagation ie2(&bn);
      ie2.addJointTarget((gum::NodeSet{0, 2}));
      auto p2 = ie2.jointPosterior(gum::NodeSet{0, 2});

      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(p1, p2)   // no diff !
    }

    GUM_ACTIVE_TEST(ImplicitTargetAllCheck) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->Y->E->F->G;W->E<-Z;X->E");
      auto ie = gum::LazyPropagation(&bn);
      ie.addJointTarget(bn.nodeset({"B", "Y", "F"}));
      auto p = gum::Tensor< double >();
      for (const auto n: bn.nodes())
        p *= bn.cpt(n);

      // target
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y", "F"})),
                                           p.sumIn(bn.variables({"B", "Y", "F"})))
      // subtargets
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y"})),
                                           p.sumIn(bn.variables({"B", "Y"})))
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"F", "Y"})),
                                           p.sumIn(bn.variables({"F", "Y"})))
      // implicit target
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"W", "Z", "X"})),
                                           p.sumIn(bn.variables({"W", "Z", "X"})))

      // impossible target in optimized inference
      TS_ASSERT_THROWS(ie.jointPosterior(bn.nodeset({"A", "E"})), gum::UndefinedElement&)
    }

    GUM_ACTIVE_TEST(ImplicitTargetAllCheckWithEvidenceOutOFTarget) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->Y->E->F->G;W->E<-Z;X->E");
      auto ie = gum::LazyPropagation(&bn);
      ie.addEvidence("E", 1);
      ie.addJointTarget(bn.nodeset({"B", "Y", "F"}));

      auto p = gum::Tensor< double >();
      for (const auto n: bn.nodes())
        p *= bn.cpt(n);
      gum::Tensor evY1 = gum::Tensor< double >();
      evY1.add(bn.variableFromName("E"));
      evY1.fillWith({0, 1});
      p *= evY1;
      p.normalize();

      // target
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y", "F"})),
                                           p.sumIn(bn.variables({"B", "Y", "F"})))
      // subtargets
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y"})),
                                           p.sumIn(bn.variables({"B", "Y"})))
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"F", "Y"})),
                                           p.sumIn(bn.variables({"F", "Y"})))
      // implicit target
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"W", "Z", "X"})),
                                           p.sumIn(bn.variables({"W", "Z", "X"})))
    }

    GUM_ACTIVE_TEST(ImplicitTargetAllCheckWithEvidenceInTarget) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->Y->E->F->G;W->E<-Z;X->E");
      auto ie = gum::LazyPropagation(&bn);
      ie.addEvidence("Y", 1);
      ie.addJointTarget(bn.nodeset({"B", "Y", "F"}));

      auto p = gum::Tensor< double >();
      for (const auto n: bn.nodes())
        p *= bn.cpt(n);
      gum::Tensor evY1 = gum::Tensor< double >();
      evY1.add(bn.variableFromName("Y"));
      evY1.fillWith({0, 1});
      p *= evY1;
      p.normalize();

      // target
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y", "F"})),
                                           p.sumIn(bn.variables({"B", "Y", "F"})))
      // subtargets
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y"})),
                                           p.sumIn(bn.variables({"B", "Y"})))
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"F", "Y"})),
                                           p.sumIn(bn.variables({"F", "Y"})))
      // implicit target
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"W", "Z", "X"})),
                                           p.sumIn(bn.variables({"W", "Z", "X"})))
    }

    GUM_ACTIVE_TEST(IrrelevantSoftEvidence) {
      auto                  bn = gum::BayesNet< double >::fastPrototype("A->B<-C");
      gum::Tensor< double > psoft;
      gum::Tensor< double > phard;
      const auto            bn_joint = this->joint(bn);
      gum::Tensor< double > jointhard, jointsoft;
      {
        auto hardev = gum::Tensor< double >();
        hardev.add(bn.variable("A"));
        hardev.fillWith({0, 1});
        jointhard = bn_joint * hardev;

        gum::LazyPropagation ie(&bn);
        ie.addEvidence(hardev);
        ie.addTarget("C");
        TS_GUM_ASSERT_THROWS_NOTHING(ie.makeInference());
        phard = gum::Tensor(ie.posterior("C"));
      }
      {
        auto softev = gum::Tensor< double >();
        softev.add(bn.variable("A"));
        softev.fillWith({0.5, 1});
        jointsoft = bn_joint * softev;

        gum::LazyPropagation ie(&bn);
        ie.addEvidence(softev);
        ie.addTarget("C");
        TS_GUM_ASSERT_THROWS_NOTHING(ie.makeInference());
        psoft = gum::Tensor(ie.posterior("C"));
      }
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(phard, psoft)
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(phard,
                                           jointhard.sumIn({&bn.variableFromName("C")}).normalize())
      TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(psoft,
                                           jointsoft.sumIn({&bn.variableFromName("C")}).normalize())
    }

    GUM_ACTIVE_TEST(MostProbableExplanation) {
      try {
        gum::BayesNet< double > bn;

        gum::LabelizedVariable n1("1", "", 2), n2("2", "", 2), n3("3", "", 3);
        gum::LabelizedVariable n4("4", "", 4), n5("5", "", 3);

        auto i1 = bn.add(n1);
        auto i2 = bn.add(n2);
        auto i3 = bn.add(n3);
        auto i4 = bn.add(n4);
        auto i5 = bn.add(n5);

        bn.addArc(i1, i3);
        bn.addArc(i1, i4);
        bn.addArc(i3, i5);
        bn.addArc(i4, i5);
        bn.addArc(i2, i4);
        bn.addArc(i2, i5);

        for (const auto node: bn.dag()) {
          bn.cpt(node).random().normalizeAsCPT();
        }

        // compute the joint
        gum::Tensor< double > joint(bn.cpt(i1));
        for (const auto node: bn.dag()) {
          if (node != i1) joint *= bn.cpt(node);
        }
        auto marg5 = joint.sumIn({&bn.variable(i5)}).normalize();

        auto joint_argmax = joint.normalize().argmax();

        // check singly connected component
        gum::LazyPropagation< double > ie(&bn);
        auto                           mpe = ie.mpeLog2Posterior();
        TS_ASSERT_EQUALS(mpe.first, *(joint_argmax.first.begin()))
        TS_ASSERT_DELTA(mpe.second, std::log2(joint_argmax.second), TS_GUM_SMALL_ERROR)

        auto p5 = ie.posterior(i5);
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(marg5, p5)

        // add hard evidence on n3
        gum::Tensor< double > e_i3;
        e_i3 << bn.variable(i3);
        e_i3.fill(0.0f);
        gum::Instantiation inst_3(e_i3);
        inst_3.chgVal(bn.variable(i3), 0);
        e_i3.set(inst_3, 1.0f);
        joint *= e_i3;
        joint_argmax = joint.normalize().argmax();

        ie.addEvidence(i3, 0);
        mpe = ie.mpeLog2Posterior();
        TS_ASSERT_EQUALS(mpe.first, *(joint_argmax.first.begin()))
        TS_ASSERT_DELTA(mpe.second, std::log2(joint_argmax.second), TS_GUM_SMALL_ERROR)

        marg5 = joint.sumIn({&bn.variable(i5)}).normalize();
        p5    = ie.posterior(i5);
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(marg5, p5)

        gum::Tensor< double > e_i4;
        e_i4 << bn.variable(i4);
        e_i4.fillWith({0.0, 1.0, 0.5, 0.0});
        joint *= e_i4;
        joint_argmax = joint.normalize().argmax();

        ie.addEvidence(e_i4);
        mpe = ie.mpeLog2Posterior();
        TS_ASSERT_EQUALS(mpe.first, *(joint_argmax.first.begin()))
        TS_ASSERT_DELTA(mpe.second, std::log2(joint_argmax.second), TS_GUM_SMALL_ERROR)

        marg5 = joint.sumIn({&bn.variable(i5)}).normalize();
        p5    = ie.posterior(i5);
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(marg5, p5)

        // checking multiply connected components
        gum::LabelizedVariable n6("6", "", 2), n7("7", "", 3), n8("8", "", 3);
        gum::LabelizedVariable n9("9", "", 4), n10("10", "", 3);

        auto i6  = bn.add(n6);
        auto i7  = bn.add(n7);
        auto i8  = bn.add(n8);
        auto i9  = bn.add(n9);
        auto i10 = bn.add(n10);

        bn.addArc(i6, i8);
        bn.addArc(i6, i9);
        bn.addArc(i7, i9);
        bn.addArc(i9, i10);

        for (auto node = i6; node <= i10; ++node) {
          bn.cpt(node).random().normalizeAsCPT();
          joint *= bn.cpt(node);
        }
        joint_argmax = joint.normalize().argmax();


        gum::LazyPropagation< double > ie2(&bn);

        // enter the previous evidence
        ie2.addEvidence(i3, 0);
        ie2.addEvidence(e_i4);

        mpe = ie2.mpeLog2Posterior();
        TS_ASSERT_EQUALS(mpe.first, *(joint_argmax.first.begin()))
        TS_ASSERT_DELTA(mpe.second, std::log2(joint_argmax.second), TS_GUM_SMALL_ERROR)

        marg5 = joint.sumIn({&bn.variable(i5)}).normalize();
        p5    = ie2.posterior(i5);
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(marg5, p5)

        gum::Tensor< double > e_i7;
        e_i7 << bn.variable(i7);
        e_i7.fillWith({0.2, 0.5, 0.1});
        joint *= e_i7;
        joint_argmax = joint.normalize().argmax();

        ie2.addEvidence(e_i7);
        mpe = ie2.mpeLog2Posterior();
        TS_ASSERT_EQUALS(mpe.first, *(joint_argmax.first.begin()))
        TS_ASSERT_DELTA(mpe.second, std::log2(joint_argmax.second), TS_GUM_SMALL_ERROR)

        marg5      = joint.sumIn({&bn.variable(i5)}).normalize();
        p5         = ie2.posterior(i5);
        auto marg9 = joint.sumIn({&bn.variable(i9)}).normalize();
        auto p9    = ie2.posterior(i9);
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(marg5, p5)

        gum::Tensor< double > e_i10;
        e_i10 << bn.variable(i10);
        e_i10.fillWith({0.0, 1.0, 0.0});
        joint *= e_i10;
        joint_argmax = joint.normalize().argmax();

        ie2.addEvidence(e_i10);
        mpe = ie2.mpeLog2Posterior();
        TS_ASSERT_EQUALS(mpe.first, *(joint_argmax.first.begin()))
        TS_ASSERT_DELTA(mpe.second, std::log2(joint_argmax.second), TS_GUM_SMALL_ERROR)

        marg9 = joint.sumIn({&bn.variable(i9)}).normalize();
        p9    = ie2.posterior(i9);
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(marg9, p9)


        auto marg10 = joint.sumIn({&bn.variable(i10)}).normalize();
        auto p10    = ie2.posterior(i10);
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(marg10, p10)

        // what if we have a component in which all the nodes have received hard evidence
        gum::LabelizedVariable n11("11", "", 2), n12("12", "", 3), n13("13", "", 3);

        auto i11 = bn.add(n11);
        auto i12 = bn.add(n12);
        auto i13 = bn.add(n13);

        for (auto node = i11; node <= i13; ++node) {
          bn.cpt(node).random().normalizeAsCPT();
          joint *= bn.cpt(node);
        }
        gum::Tensor< double > e_i11;
        e_i11 << bn.variable(i11);
        e_i11.fillWith({0.0, 1.0});
        joint *= e_i11;
        gum::Tensor< double > e_i12;
        e_i12 << bn.variable(i12);
        e_i12.fillWith({0.0, 0.0, 1.0});
        joint *= e_i12;
        gum::Tensor< double > e_i13;
        e_i13 << bn.variable(i13);
        e_i13.fillWith({1.0, 0.0, 0.0});
        joint *= e_i13;

        joint_argmax = joint.normalize().argmax();

        gum::LazyPropagation< double > ie3(&bn);

        // enter the previous evidence
        ie3.addEvidence(i3, 0);
        ie3.addEvidence(e_i4);
        ie3.addEvidence(e_i7);
        ie3.addEvidence(e_i10);
        ie3.addEvidence(e_i11);
        ie3.addEvidence(e_i12);
        ie3.addEvidence(e_i13);

        mpe = ie3.mpeLog2Posterior();
        TS_ASSERT_EQUALS(mpe.first, *(joint_argmax.first.begin()))
        TS_ASSERT_DELTA(mpe.second, std::log2(joint_argmax.second), TS_GUM_SMALL_ERROR)

        marg9 = joint.sumIn({&bn.variable(i9)}).normalize();
        p9    = ie3.posterior(i9);
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(marg9, p9)
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    GUM_ACTIVE_TEST(NonRegressionBugJoinTarget) {
      auto bn = gum::BayesNet< double >::fastPrototype(
          "X02->X03->X11<-X08->X09->X10<-X07<-X05->X09<-X04<-X00;X03->X04;X01->X10;X05->X06");
      auto ie = gum::LazyPropagation(&bn);
      ie.addJointTarget(bn.nodeset({"X04", "X11"}));
      ie.addJointTarget(bn.nodeset({"X00", "X01", "X06"}));
      const auto bn_joint = this->joint(bn);

      try {
        ie.makeInference();
        gum::Set< const gum::DiscreteVariable* > set{&bn.variableFromName("X04"),
                                                     &bn.variable("X11")};
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"X04", "X11"})),
                                             bn_joint.sumIn(set).normalize())
        set = {&bn.variableFromName("X00"), &bn.variable("X01"), &bn.variable("X06")};
        TS_GUM_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"X00", "X01", "X06"})),
                                             bn_joint.sumIn(set).normalize())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    private:
    void checkInference(gum::LazyPropagation< double >& ie,
                        const gum::Tensor< double >&    joint,
                        const gum::NodeSet&             nodes) {
      // p=ie.jointPosterior(ie.BN().nam)
    }

    // Builds a _bn to test the inference
    void fill(gum::BayesNet< double >& bn) const {
      bn.cpt(i1).fillWith({0.2f, 0.8f});
      bn.cpt(i2).fillWith({0.3f, 0.7f});
      bn.cpt(i3).fillWith({0.1f, 0.9f, 0.9f, 0.1f});
      bn.cpt(i4).fillWith(   // clang-format off
              {0.4f, 0.6f,
               0.5f, 0.5f,
               0.5f, 0.5f,
               1.0f, 0.0f});                  // clang-format on
      bn.cpt(i5).fillWith(   // clang-format off
              {0.3f, 0.6f, 0.1f,
               0.5f, 0.5f, 0.0f,
               0.5f, 0.5f, 0.0f,
               1.0f, 0.0f, 0.0f,
               0.4f, 0.6f, 0.0f,
               0.5f, 0.5f, 0.0f,
               0.5f, 0.5f, 0.0f,
               0.0f, 0.0f, 1.0f});   // clang-format on
    }
  };
}   // namespace gum_tests
