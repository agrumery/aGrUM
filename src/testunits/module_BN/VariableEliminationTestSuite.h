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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/config.h>

#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/tools/relevantTensorsFinderType.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  VariableElimination
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {
  struct VariableEliminationTestSuite {
    public:
    gum::BayesNet< double >* bn;
    gum::NodeId              i1;
    gum::NodeId              i2;
    gum::NodeId              i3;
    gum::NodeId              i4;
    gum::NodeId              i5;
    gum::Tensor< double >*   e_i1;
    gum::Tensor< double >*   e_i4;

    [[nodiscard]] static gum::Tensor< double > joint(const gum::BayesNet< double >& bn) {
      gum::Tensor< double > pot;
      for (const auto node: bn.dag()) {
        pot *= bn.cpt(node);
      }
      return pot;
    }

    [[nodiscard]] static bool equalTensors(const gum::Tensor< double >& p1,
                                           const gum::Tensor< double >& p2) {
      gum::Instantiation ii(p1);

      for (ii.setFirst(); !ii.end(); ii.inc()) {
        if ((p1[ii] == 0) && (std::fabs(p2[ii]) > GUM_SMALL_ERROR)) return false;
        if (p1[ii] > p2[ii]) {
          if (std::fabs((p1[ii] - p2[ii]) / p1[ii]) > GUM_SMALL_ERROR) return false;
        } else {
          if (std::fabs((p1[ii] - p2[ii]) / p1[ii]) > GUM_SMALL_ERROR) return false;
        }
      }

      return true;
    }

    // Testing when there is no evidence
    VariableEliminationTestSuite() {
      bn = new gum::BayesNet< double >();
      gum::LabelizedVariable n1("1", "", 2);
      gum::LabelizedVariable n2("2", "", 2);
      gum::LabelizedVariable n3("3", "", 2);
      gum::LabelizedVariable n4("4", "", 2);
      gum::LabelizedVariable n5("5", "", 3);
      i1 = bn->add(n1);
      i2 = bn->add(n2);
      i3 = bn->add(n3);
      i4 = bn->add(n4);
      i5 = bn->add(n5);
      bn->addArc(i1, i3);
      bn->addArc(i1, i4);
      bn->addArc(i3, i5);
      bn->addArc(i4, i5);
      bn->addArc(i2, i4);
      bn->addArc(i2, i5);
      e_i1 = new gum::Tensor< double >();
      (*e_i1) << bn->variable(i1);
      e_i1->fill(static_cast< float >(0));
      gum::Instantiation inst_1(*e_i1);
      inst_1.chgVal(bn->variable(i1), 0);
      e_i1->set(inst_1, static_cast< float >(1));
      e_i4 = new gum::Tensor< double >();
      (*e_i4) << bn->variable(i4);
      e_i4->fill(static_cast< float >(0));
      gum::Instantiation inst_4(*e_i4);
      inst_4.chgVal(bn->variable(i4), 1);
      e_i4->set(inst_4, static_cast< float >(1));
    }

    ~VariableEliminationTestSuite() {
      delete bn;
      delete e_i1;
      delete e_i4;
    }

    void testMakeInference() {
      fill(*bn);
      // Testing the inference
      gum::VariableElimination< double >* inf = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf = new gum::VariableElimination< double >(bn));

      if (inf != nullptr) {
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf->makeInference());
        GUM_CHECK_ASSERT_THROWS_NOTHING(delete inf);
      }
    }

    void testVariableElimination() {
      fill(*bn);
      gum::VariableElimination< double >* inf = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf = new gum::VariableElimination< double >(bn));

      if (inf != nullptr) {
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf->makeInference());

        const auto                               bn_joint = this->joint(*bn);
        gum::Set< const gum::DiscreteVariable* > vars;

        for (const auto node: bn->dag()) {
          const gum::Tensor< double >* posterior = nullptr;
          GUM_CHECK_ASSERT_THROWS_NOTHING(posterior = &(inf->posterior(node)));
          vars.insert(&(bn->variable(node)));
          CHECK(equalTensors(*posterior, bn_joint.sumIn(vars)));
          vars.clear();
        }

        GUM_CHECK_ASSERT_THROWS_NOTHING(delete inf);
      }
    }

    void testShaferShenoyInf_3() {
      fill(*bn);
      gum::List< const gum::Tensor< double >* > e_list;
      e_list.insert(e_i1);
      e_list.insert(e_i4);

      gum::VariableElimination< double >* inf = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf = new gum::VariableElimination< double >(bn));
      auto bn_joint = this->joint(*bn);

      if (inf != nullptr) {
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf->addListOfEvidence(e_list));
        for (const auto pot: e_list)
          bn_joint *= *pot;

        gum::Set< const gum::DiscreteVariable* > vars;
        for (const auto node: bn->dag()) {
          const gum::Tensor< double >* posterior = nullptr;
          GUM_CHECK_ASSERT_THROWS_NOTHING(posterior = &(inf->posterior(node)));
          vars.insert(&(bn->variable(node)));
          CHECK(equalTensors(*posterior, bn_joint.sumIn(vars).normalize()));
          vars.clear();
        }

        GUM_CHECK_ASSERT_THROWS_NOTHING(delete inf);
      }
    }

    // testing information methods
    void testInformationMethods() {
      fill(*bn);

      gum::VariableElimination< double > inf(bn);

      GUM_CHECK_ASSERT_THROWS_NOTHING(
          gum::InformationTheory itheo(inf, gum::NodeSet({2}), gum::NodeSet({4})));
      gum::InformationTheory itheo(inf, gum::NodeSet({2}), gum::NodeSet({4}));
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf.H((gum::NodeId)2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(itheo.mutualInformationXY());
    }

    // Testing when there is no evidence
    void testJoint() {
      fill(*bn);
      // Testing the inference
      gum::VariableElimination< double > inf(bn);
      gum::NodeSet                       nodeset;
      nodeset.insert(2);
      nodeset.insert(4);
      inf.addJointTarget(nodeset);

      CHECK_NOTHROW(inf.jointPosterior(nodeset));

      const auto                               bn_joint = this->joint(*bn);
      gum::Set< const gum::DiscreteVariable* > vars;
      vars.insert(&(bn->variable(2)));
      vars.insert(&(bn->variable(4)));
      CHECK(equalTensors(inf.jointPosterior(nodeset), bn_joint.sumIn(vars)));
    }

    // Testing when there is no evidence
    void testJoint2() {
      fill(*bn);
      // Testing the inference
      gum::VariableElimination< double > inf(bn);
      gum::NodeSet                       nodeset;
      nodeset.insert(1);
      nodeset.insert(2);
      nodeset.insert(3);
      nodeset.insert(4);
      inf.eraseAllTargets();
      inf.addJointTarget(nodeset);

      gum::NodeSet nodeset2;
      nodeset2.insert(2);
      nodeset2.insert(4);
      const auto                               bn_joint = this->joint(*bn);
      gum::Set< const gum::DiscreteVariable* > vars;
      vars.insert(&(bn->variable(2)));
      vars.insert(&(bn->variable(4)));

      CHECK_NOTHROW(inf.jointPosterior(nodeset2));
      CHECK(equalTensors(inf.jointPosterior(nodeset2), bn_joint.sumIn(vars)));

      CHECK_NOTHROW(inf.posterior(3));
      vars.clear();
      vars.insert(&(bn->variable(3)));
      CHECK(equalTensors(inf.posterior(3), bn_joint.sumIn(vars)));

      vars.insert(&(bn->variable(1)));
      vars.insert(&(bn->variable(2)));
      vars.insert(&(bn->variable(4)));
      CHECK(equalTensors(inf.jointPosterior(nodeset), bn_joint.sumIn(vars)));
    }

    static void testAlarm() {
      std::string              file = GET_RESSOURCES_PATH("bif/alarm.bif");
      gum::BayesNet< double >  alarm;
      gum::BIFReader< double > reader(&alarm, file);
      reader.proceed();
      gum::VariableElimination< double >   ve(&alarm);
      gum::ShaferShenoyInference< double > shafer(&alarm);
      CHECK_NOTHROW(shafer.makeInference());
      for (const auto node: alarm.dag()) {
        const gum::Tensor< double >* pot_1 = nullptr;
        CHECK_NOTHROW(pot_1 = &(ve.posterior(node)));
        const gum::Tensor< double >* pot_2 = nullptr;
        CHECK_NOTHROW(pot_2 = &(shafer.posterior(node)));
        if ((*pot_1) != (*pot_2)) { CHECK(false); }
      }
    }

    void testSmartManagementOfJointTarget() {
      fill(*bn);

      gum::VariableElimination< double > inf(bn);
      inf.addJointTarget(gum::NodeSet{0, 1, 2});
      inf.addJointTarget(gum::NodeSet{2, 3});
      CHECK((inf.nbrJointTargets()) == (static_cast< gum::Size >(2)));

      // should not be added since {0,1,2} already exists
      inf.addJointTarget(gum::NodeSet{0, 1});
      CHECK((inf.nbrJointTargets()) == (static_cast< gum::Size >(2)));

      // should remove {2,3} since {2,3,4} includes {2,3}
      inf.addJointTarget(gum::NodeSet{2, 3, 4});
      CHECK((inf.nbrJointTargets()) == (static_cast< gum::Size >(2)));

      auto                                     bn_joint = this->joint(*bn);
      gum::Set< const gum::DiscreteVariable* > vars;
      vars.insert(&(bn->variable(0)));
      vars.insert(&(bn->variable(1)));
      vars.insert(&(bn->variable(2)));
      CHECK(equalTensors(inf.jointPosterior(gum::NodeSet{0, 1, 2}),
                         bn_joint.sumIn(vars).normalize()));

      vars.clear();
      vars.insert(&(bn->variable(2)));
      vars.insert(&(bn->variable(3)));
      CHECK(equalTensors(inf.jointPosterior(gum::NodeSet{2, 3}), bn_joint.sumIn(vars)));

      vars.clear();
      vars.insert(&(bn->variable(0)));
      vars.insert(&(bn->variable(1)));
      CHECK(equalTensors(inf.jointPosterior(gum::NodeSet{0, 1}), bn_joint.sumIn(vars)));

      vars.clear();
      vars.insert(&(bn->variable(2)));
      vars.insert(&(bn->variable(3)));
      vars.insert(&(bn->variable(4)));
      CHECK(equalTensors(inf.jointPosterior(gum::NodeSet{2, 3, 4}), bn_joint.sumIn(vars)));
    }

    void testAsia() const {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == ((gum::Size) static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));

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
          gum::VariableElimination< double >   inf1(&bn);
          gum::ShaferShenoyInference< double > inf2(&bn);
          auto                                 joint = bn_joint;
          for (auto pot: evidences) {
            CHECK_NOTHROW(inf1.addEvidence(*pot));
            CHECK_NOTHROW(inf2.addEvidence(*pot));
            joint *= *pot;
          }
          CHECK_NOTHROW(inf1.makeInference());
          CHECK_NOTHROW(inf2.makeInference());
          for (auto node: bn.dag()) {
            CHECK(equalTensors(inf1.posterior(node), inf2.posterior(node)));
            CHECK(
                equalTensors(inf1.posterior(node), joint.sumIn({&bn.variable(node)}).normalize()));
          }
          ev_pot.set(inst, static_cast< float >(0));
        }
      }
    }

    static void testAlarm2() {
      std::string              file = GET_RESSOURCES_PATH("bif/alarm.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == ((gum::Size) static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));

      gum::VariableElimination< double > inf1(&bn);
      gum::VariableElimination< double > inf2(&bn);

      CHECK_NOTHROW(inf1.makeInference());
      CHECK_NOTHROW(inf2.makeInference());

      for (auto node: bn.dag()) {
        CHECK_NOTHROW(inf1.posterior(node));
        CHECK_NOTHROW(inf2.posterior(node));
        CHECK(equalTensors(inf1.posterior(node), inf2.posterior(node)));
      }

      std::vector< gum::NodeId >                ev_nodes{2, 6, 7, 10, 12, 14, 16};
      gum::List< const gum::Tensor< double >* > evidences;
      for (const auto node: ev_nodes) {
        gum::Tensor< double >* ev_pot = new gum::Tensor< double >;

        (*ev_pot) << bn.variable(node);
        ev_pot->fill(0.0f);
        gum::Instantiation inst(*ev_pot);
        if (node <= 10) {
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

      gum::VariableElimination< double >   inf3(&bn);
      gum::ShaferShenoyInference< double > inf4(&bn);
      for (auto pot: evidences) {
        CHECK_NOTHROW(inf1.addEvidence(*pot));
        CHECK_NOTHROW(inf2.addEvidence(*pot));
        CHECK_NOTHROW(inf3.addEvidence(*pot));
        CHECK_NOTHROW(inf4.addEvidence(*pot));
      }

      CHECK_NOTHROW(inf1.makeInference());
      CHECK_NOTHROW(inf2.makeInference());
      CHECK_NOTHROW(inf3.makeInference());
      CHECK_NOTHROW(inf4.makeInference());

      for (auto node: bn.dag()) {
        CHECK_NOTHROW(inf1.posterior(node));
        CHECK_NOTHROW(inf2.posterior(node));
        CHECK_NOTHROW(inf3.posterior(node));
        CHECK_NOTHROW(inf4.posterior(node));
        CHECK(equalTensors(inf1.posterior(node), inf2.posterior(node)));
        CHECK(equalTensors(inf1.posterior(node), inf3.posterior(node)));
        CHECK(equalTensors(inf1.posterior(node), inf4.posterior(node)));
      }

      gum::VariableElimination< double > inf5(&bn);
      inf5.setRelevantTensorsFinderType(gum::RelevantTensorsFinderType::DSEP_BAYESBALL_NODES);
      for (auto pot: evidences) {
        CHECK_NOTHROW(inf5.addEvidence(*pot));
      }
      CHECK_NOTHROW(inf5.makeInference());
      for (auto node: bn.dag()) {
        CHECK_NOTHROW(inf5.posterior(node));
        CHECK(equalTensors(inf1.posterior(node), inf5.posterior(node)));
      }

      for (auto pot: evidences)
        delete pot;
    }

    void testAsia2() const {
      std::string              file = GET_RESSOURCES_PATH("bif/asia3.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == ((gum::Size) static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));

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

                gum::VariableElimination< double >   inf1(&bn);
                gum::ShaferShenoyInference< double > inf2(&bn);
                auto                                 joint = bn_joint;
                for (auto pot: evidences) {
                  CHECK_NOTHROW(inf1.addEvidence(*pot));
                  CHECK_NOTHROW(inf2.addEvidence(*pot));
                  joint *= *pot;
                }
                CHECK_NOTHROW(inf1.makeInference());
                CHECK_NOTHROW(inf2.makeInference());

                for (auto xnode: bn.dag()) {
                  CHECK(equalTensors(inf1.posterior(xnode), inf2.posterior(xnode)));
                  CHECK(equalTensors(inf1.posterior(xnode),
                                     joint.sumIn({&bn.variable(xnode)}).normalize()));
                }
                ev_pot2.set(inst2, 0.0f);
              }
            }
          }

          ev_pot.set(inst, 0.0f);
        }
      }
    }

    void testAsia3() const {
      std::string              file = GET_RESSOURCES_PATH("bif/asia3.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == ((gum::Size) static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));

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

                gum::VariableElimination< double > inf1(&bn);
                inf1.setRelevantTensorsFinderType(
                    gum::RelevantTensorsFinderType::DSEP_BAYESBALL_NODES);
                gum::ShaferShenoyInference< double > inf2(&bn);
                auto                                 joint = bn_joint;
                for (auto pot: evidences) {
                  CHECK_NOTHROW(inf1.addEvidence(*pot));
                  CHECK_NOTHROW(inf2.addEvidence(*pot));
                  joint *= *pot;
                }
                CHECK_NOTHROW(inf1.makeInference());
                CHECK_NOTHROW(inf2.makeInference());

                for (auto xnode: bn.dag()) {
                  CHECK(equalTensors(inf1.posterior(xnode), inf2.posterior(xnode)));
                  CHECK(equalTensors(inf1.posterior(xnode),
                                     joint.sumIn({&bn.variable(xnode)}).normalize()));
                }
                ev_pot2.set(inst2, 0.0f);
              }
            }
          }

          ev_pot.set(inst, 0.0f);
        }
      }
    }

    void testAsia4() const {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == ((gum::Size) static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));

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

                gum::VariableElimination< double > inf1(&bn);
                inf1.setRelevantTensorsFinderType(
                    gum::RelevantTensorsFinderType::DSEP_BAYESBALL_TENSORS);
                gum::VariableElimination< double > inf2(&bn);
                auto                               joint = bn_joint;
                for (auto pot: evidences) {
                  CHECK_NOTHROW(inf1.addEvidence(*pot));
                  CHECK_NOTHROW(inf2.addEvidence(*pot));
                  joint *= *pot;
                }
                CHECK_NOTHROW(inf1.makeInference());
                CHECK_NOTHROW(inf2.makeInference());

                for (auto xnode: bn.dag()) {
                  try {
                    const auto res = joint.sumIn({&bn.variable(xnode)});
                    if (res.sum() > GUM_SMALL_ERROR) {
                      CHECK(equalTensors(inf1.posterior(xnode),
                                         joint.sumIn({&bn.variable(xnode)}).normalize()));
                    }
                    [[maybe_unused]] auto f
                        = equalTensors(inf1.posterior(xnode), inf2.posterior(xnode));
                  } catch (gum::IncompatibleEvidence&) {
                    // check evidence incompatibility:
                    if (node2 == gum::NodeId(2)) {
                      // node2 = tuberculos_or_cancer, then node =
                      // tuberculosis
                      CHECK(((inst2_index == 1) && (inst_index == 0)));
                    } else {
                      // node2 = lung_cancer & node =
                      // tuberculos_or_cancer
                      CHECK(((inst2_index == 0) && (inst_index == 1)));
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

    void testChgEvidence() const {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == ((gum::Size) static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));

      const auto bn_joint = this->joint(bn);

      gum::VariableElimination< double > ie_0(&bn);
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

      gum::VariableElimination< double > ie_1(&bn);
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

      gum::VariableElimination< double > ie(&bn);
      ie.addTarget(0);
      ie.addEvidence(1, 0);
      ie.makeInference();
      CHECK((p_0) == (ie.posterior(0)));
      const auto& var0 = bn.variable(0);
      CHECK(equalTensors(ie.posterior(0), joint0.sumIn({&var0}).normalize()));

      ie.chgEvidence(1, 1);
      ie.makeInference();
      CHECK((p_0) != (ie.posterior(0)));
      CHECK((p_1) == (ie.posterior(0)));
      CHECK(equalTensors(ie.posterior(0), joint1.sumIn({&var0}).normalize()));
    }

    void testChgEvidence2() const {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == ((gum::Size) static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));

      const auto bn_joint = this->joint(bn);

      gum::VariableElimination< double > ie_0(&bn);
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

      gum::VariableElimination< double > ie_1(&bn);
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

      gum::VariableElimination< double > ie(&bn);
      ie.eraseAllTargets();
      ie.addTarget(0);
      ie.addEvidence(1, 0);
      ie.makeInference();
      CHECK((p_0) == (ie.posterior(0)));
      const auto& var0 = bn.variable(0);
      CHECK(equalTensors(ie.posterior(0), joint0.sumIn({&var0}).normalize()));

      ie.chgEvidence(1, 1);
      ie.makeInference();
      CHECK((p_0) != (ie.posterior(0)));
      CHECK((p_1) == (ie.posterior(0)));
      CHECK(equalTensors(ie.posterior(0), joint1.sumIn({&var0}).normalize()));
    }

    static void testStaticEvidenceImpact() {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == ((gum::Size) static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));

      gum::VariableElimination< double > ie_all(&bn);
      CHECK_THROWS_AS(ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{0, 1, 2}),
                      const gum::InvalidArgument&);

      auto res = ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{1, 2});

      CHECK((res.nbrDim()) == (static_cast< gum::Size >(2)));   // 2 indep 0 given 1

      gum::VariableElimination< double > ie_0(&bn);
      ie_0.addTarget(0);                                        // visit_to_asia
      ie_0.addEvidence(1, 0);                                   // tuberculosis
      ie_0.makeInference();
      gum::Tensor< double > p_0 = ie_0.posterior(0);

      gum::VariableElimination< double > ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Tensor< double > p_1 = ie_1.posterior(0);

      gum::Instantiation i;
      i.add(bn.variable(1));
      i.setFirst();
      CHECK((p_0) == (res.extract(i)));
      i.inc();
      CHECK((p_1) == (res.extract(i)));
    }

    static void testEvidenceImpactWithNames() {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      CHECK((nbrErr) == (static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));


      gum::VariableElimination< double > ie_all(&bn);
      CHECK_THROWS_AS(ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{0, 1, 2}),
                      const gum::InvalidArgument&);

      CHECK_THROWS_AS(ie_all.evidenceImpact("visit_to_asia", {"tuberculoisis", "toto"}),
                      const gum::NotFound&);

      auto res = ie_all.evidenceImpact("visit_to_Asia", {"tuberculosis", "tuberculos_or_cancer"});

      CHECK((res.nbrDim()) == (static_cast< gum::Size >(2)));   // 2 indep 0 given 1

      gum::VariableElimination< double > ie_0(&bn);
      ie_0.addTarget(0);                                        // visit_to_asia
      ie_0.addEvidence(1, 0);                                   // tuberculosis
      ie_0.makeInference();
      gum::Tensor< double > p_0 = ie_0.posterior(0);

      gum::VariableElimination< double > ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Tensor< double > p_1 = ie_1.posterior(0);

      gum::Instantiation i;
      i.add(bn.variable(1));
      i.setFirst();
      CHECK((p_0) == (res.extract(i)));
      i.inc();
      CHECK((p_1) == (res.extract(i)));
    }

    static void testEvidenceImpact() {
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

      gum::VariableElimination< double > ie(&bn);
      gum::Tensor< double >              res;
      GUM_CHECK_ASSERT_THROWS_NOTHING(res = ie.evidenceImpact("E", {"A", "B", "C", "D", "F"}));
      CHECK((res.nbrDim()) == (static_cast< gum::Size >(4)));   // MarkovBlanket(E)=(A,D,C)
    }

    void testJointWithHardEvidence() const {
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

      gum::VariableElimination< double > ie(&bn);
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
        CHECK(equalTensors(p, pjoint.sumIn(xjoint).normalize()));
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void testImplicitTargetAllCheck() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->Y->E->F->G;W->E<-Z;X->E");
      auto ie = gum::VariableElimination(&bn);
      ie.addJointTarget(bn.nodeset({"B", "Y", "F"}));
      auto p = gum::Tensor< double >();
      for (const auto n: bn.nodes())
        p *= bn.cpt(n);

      // target
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y", "F"})),
                                              p.sumIn(bn.variables({"B", "Y", "F"})));
      // subtargets
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y"})),
                                              p.sumIn(bn.variables({"B", "Y"})));
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"F", "Y"})),
                                              p.sumIn(bn.variables({"F", "Y"})));
      // implicit target
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"W", "Z", "X"})),
                                              p.sumIn(bn.variables({"W", "Z", "X"})));

      // impossible target in optimized inference
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"A", "E"})),
                                              p.sumIn(bn.variables({"A", "E"})));
    }

    static void testImplicitTargetAllCheckWithEvidenceOutOFTarget() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->Y->E->F->G;W->E<-Z;X->E");
      auto ie = gum::VariableElimination(&bn);
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
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y", "F"})),
                                              p.sumIn(bn.variables({"B", "Y", "F"})));
      // subtargets
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y"})),
                                              p.sumIn(bn.variables({"B", "Y"})));
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"F", "Y"})),
                                              p.sumIn(bn.variables({"F", "Y"})));
      // implicit target
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"W", "Z", "X"})),
                                              p.sumIn(bn.variables({"W", "Z", "X"})));
    }

    static void testImplicitTargetAllCheckWithEvidenceInTarget() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->Y->E->F->G;W->E<-Z;X->E");
      auto ie = gum::VariableElimination(&bn);
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
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y", "F"})),
                                              p.sumIn(bn.variables({"B", "Y", "F"})));
      // subtargets
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"B", "Y"})),
                                              p.sumIn(bn.variables({"B", "Y"})));
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"F", "Y"})),
                                              p.sumIn(bn.variables({"F", "Y"})));
      // implicit target
      GUM_CHECK_TENSOR_ALMOST_EQUALS_SAMEVARS(ie.jointPosterior(bn.nodeset({"W", "Z", "X"})),
                                              p.sumIn(bn.variables({"W", "Z", "X"})));
    }

    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet< double >& bn) const {
      // FILLING PARAMS
      bn.cpt(i1).fillWith({0.2f, 0.8f});
      bn.cpt(i2).fillWith({0.3f, 0.7f});
      bn.cpt(i3).fillWith({0.1f, 0.9f, 0.9f, 0.1f});
      bn.cpt(i4).fillWith({// clang-format off
                              0.4f, 0.6f,
                              0.5f, 0.5f,
                              0.5f, 0.5f,
                              1.0f, 0.0f}   // clang-format on
      );
      bn.cpt(i5).fillWith({        // clang-format off
                0.3f, 0.6f, 0.1f,
                0.5f, 0.5f, 0.0f,
                0.5f, 0.5f, 0.0f,
                1.0f, 0.0f, 0.0f,
                0.4f, 0.6f, 0.0f,
                0.5f, 0.5f, 0.0f,
                0.5f, 0.5f, 0.0f,
                0.0f, 0.0f, 1.0f}  //clang-format on
          );
    }

    // Uncomment this to have some outputs.
    static void printProba(const gum::Tensor< double >& p) {
      /*gum::Instantiation inst( p );
      for ( inst.setFirst(); !inst.end(); ++inst ) {
        std::cerr << inst << " : " << p[inst] << std::endl;
      }
      std::cerr << std::endl;*/
    }
  };

GUM_TEST_ACTIF(MakeInference)
GUM_TEST_ACTIF(VariableElimination)
GUM_TEST_ACTIF(ShaferShenoyInf_3)
GUM_TEST_ACTIF(InformationMethods)
GUM_TEST_ACTIF(Joint)
GUM_TEST_ACTIF(Joint2)
GUM_TEST_ACTIF(Alarm)
GUM_TEST_ACTIF(SmartManagementOfJointTarget)
GUM_TEST_ACTIF(Asia)
GUM_TEST_ACTIF(Alarm2)
GUM_TEST_ACTIF(Asia2)
GUM_TEST_ACTIF(Asia3)
GUM_TEST_ACTIF(Asia4)
GUM_TEST_ACTIF(ChgEvidence)
GUM_TEST_ACTIF(ChgEvidence2)
GUM_TEST_ACTIF(StaticEvidenceImpact)
GUM_TEST_ACTIF(EvidenceImpactWithNames)
GUM_TEST_ACTIF(EvidenceImpact)
GUM_TEST_ACTIF(JointWithHardEvidence)
GUM_TEST_ACTIF(ImplicitTargetAllCheck)
GUM_TEST_ACTIF(ImplicitTargetAllCheckWithEvidenceOutOFTarget)
GUM_TEST_ACTIF(ImplicitTargetAllCheckWithEvidenceInTarget)
}
