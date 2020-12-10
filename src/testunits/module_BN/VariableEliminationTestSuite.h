/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/config.h>

#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/variables/labelizedVariable.h>

#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/tools/relevantPotentialsFinderType.h>
#include <agrum/BN/inference/variableElimination.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class VariableElimination: public CxxTest::TestSuite {
    public:
    gum::BayesNet< double >* bn;
    gum::NodeId              i1, i2, i3, i4, i5;
    gum::Potential< double >*e_i1, *e_i4;

    float __epsilon{1e-6f};

    void setUp() {
      bn = new gum::BayesNet< double >();

      gum::LabelizedVariable n1("1", "", 2), n2("2", "", 2), n3("3", "", 2),
         n4("4", "", 2), n5("5", "", 3);

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

      e_i1 = new gum::Potential< double >();
      (*e_i1) << bn->variable(i1);
      e_i1->fill((float)0);
      gum::Instantiation inst_1(*e_i1);
      inst_1.chgVal(bn->variable(i1), 0);
      e_i1->set(inst_1, (float)1);

      e_i4 = new gum::Potential< double >();
      (*e_i4) << bn->variable(i4);
      e_i4->fill((float)0);
      gum::Instantiation inst_4(*e_i4);
      inst_4.chgVal(bn->variable(i4), 1);
      e_i4->set(inst_4, (float)1);
    }

    void tearDown() {
      delete bn;
      delete e_i1;
      delete e_i4;
    }

    bool equalPotentials(const gum::Potential< double >& p1,
                         const gum::Potential< double >& p2) {
      gum::Instantiation i1(p1);
      gum::Instantiation i2(p2);

      for (i1.setFirst(), i2.setFirst(); !i1.end(); i1.inc(), i2.inc()) {
        if ((p1[i1] == 0) && (std::fabs(p2[i2]) > __epsilon)) return false;
        if (p1[i1] > p2[i2]) {
          if (std::fabs((p1[i1] - p2[i2]) / p1[i1]) > __epsilon) return false;
        } else {
          if (std::fabs((p1[i1] - p2[i2]) / p1[i2]) > __epsilon) return false;
        }
      }

      return true;
    }

    // Testing when there is no evidence
    void testMakeInference() {
      fill(*bn);
      // Testing the inference
      gum::VariableElimination< double >* inf = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inf =
                                      new gum::VariableElimination< double >(bn));

      if (inf != 0) {
        TS_GUM_ASSERT_THROWS_NOTHING(inf->makeInference());
        TS_GUM_ASSERT_THROWS_NOTHING(delete inf);
      }
    }

    void testVariableElimination() {
      fill(*bn);
      gum::VariableElimination< double >* inf = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(inf =
                                      new gum::VariableElimination< double >(bn));

      if (inf != nullptr) {
        TS_GUM_ASSERT_THROWS_NOTHING(inf->makeInference());

        const gum::Potential< double >* posterior_1 = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(posterior_1 = &(inf->posterior(i1)));

        if (posterior_1 != 0) printProba(*posterior_1);

        const gum::Potential< double >* posterior_2 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_2 = &(inf->posterior(i2)));

        if (posterior_2 != 0) printProba(*posterior_2);

        const gum::Potential< double >* posterior_3 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_3 = &(inf->posterior(i3)));

        if (posterior_3 != 0) printProba(*posterior_3);

        const gum::Potential< double >* posterior_4 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_4 = &(inf->posterior(i4)));

        if (posterior_4 != 0) printProba(*posterior_4);

        const gum::Potential< double >* posterior_5 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_5 = &(inf->posterior(i5)));

        if (posterior_5 != 0) printProba(*posterior_5);

        TS_GUM_ASSERT_THROWS_NOTHING(delete inf);
      }
    }

    void testShaferShenoyInf_3() {
      fill(*bn);
      gum::List< const gum::Potential< double >* > e_list;
      e_list.insert(e_i1);
      e_list.insert(e_i4);

      gum::VariableElimination< double >* inf = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inf =
                                      new gum::VariableElimination< double >(bn));

      if (inf != 0) {
        TS_GUM_ASSERT_THROWS_NOTHING(inf->addListOfEvidence(e_list));

        const gum::Potential< double >* posterior_1 = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(posterior_1 = &(inf->posterior(i1)));

        if (posterior_1 != 0) printProba(*posterior_1);

        const gum::Potential< double >* posterior_2 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_2 = &(inf->posterior(i2)));

        if (posterior_2 != 0) printProba(*posterior_2);

        const gum::Potential< double >* posterior_3 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_3 = &(inf->posterior(i3)));

        if (posterior_3 != 0) printProba(*posterior_3);

        const gum::Potential< double >* posterior_4 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_4 = &(inf->posterior(i4)));

        if (posterior_4 != 0) printProba(*posterior_4);

        const gum::Potential< double >* posterior_5 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING(posterior_5 = &(inf->posterior(i5)));

        if (posterior_5 != 0) printProba(*posterior_5);

        TS_GUM_ASSERT_THROWS_NOTHING(delete inf);
      }
    }


    // testing information methods
    void testInformationMethods() {
      fill(*bn);

      gum::VariableElimination< double > inf(bn);
      gum::NodeSet                       nodeset;
      nodeset.insert(0);
      nodeset.insert(4);
      inf.addJointTarget(nodeset);
      nodeset.clear();
      nodeset.insert(2);
      nodeset.insert(4);
      inf.addJointTarget(nodeset);
      inf.makeInference();

      TS_GUM_ASSERT_THROWS_NOTHING(inf.H((gum::NodeId)2));
      TS_GUM_ASSERT_THROWS_NOTHING(inf.I((gum::NodeId)2, (gum::NodeId)4));
      TS_ASSERT_THROWS(inf.I((gum::NodeId)2, (gum::NodeId)2),
                       gum::OperationNotAllowed);
      TS_GUM_ASSERT_THROWS_NOTHING(inf.VI((gum::NodeId)2, (gum::NodeId)4));
      TS_GUM_ASSERT_THROWS_NOTHING(inf.I((gum::NodeId)0, (gum::NodeId)4));

      for (const auto node: bn->dag()) {
        for (const auto par: bn->dag().parents(node)) {
          TS_GUM_ASSERT_THROWS_NOTHING(inf.I(node, par));
          TS_GUM_ASSERT_THROWS_NOTHING(inf.I(par, node));
        }
      }

      //@TODO : test computations and not only good behaviour
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

      TS_ASSERT_THROWS_NOTHING(inf.jointPosterior(nodeset));
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

      TS_ASSERT_THROWS_NOTHING(inf.jointPosterior(nodeset2));
      TS_ASSERT_THROWS_NOTHING(inf.posterior(3));
    }

    void testAlarm() {
      std::string              file = GET_RESSOURCES_PATH("bif/alarm.bif");
      gum::BayesNet< double >  alarm;
      gum::BIFReader< double > reader(&alarm, file);
      reader.proceed();
      gum::VariableElimination< double >   ve(&alarm);
      gum::ShaferShenoyInference< double > shafer(&alarm);
      TS_ASSERT_THROWS_NOTHING(shafer.makeInference());
      for (const auto node: alarm.dag()) {
        const gum::Potential< double >* pot_1 = nullptr;
        TS_ASSERT_THROWS_NOTHING(pot_1 = &(ve.posterior(node)));
        const gum::Potential< double >* pot_2 = nullptr;
        TS_ASSERT_THROWS_NOTHING(pot_2 = &(shafer.posterior(node)));
        if ((*pot_1) != (*pot_2)) { TS_ASSERT(false); }
      }
    }


    void testSmartManagementOfJointTarget() {
      fill(*bn);

      gum::VariableElimination< double > inf(bn);
      inf.addJointTarget(gum::NodeSet{0, 1, 2});
      inf.addJointTarget(gum::NodeSet{2, 3});
      TS_ASSERT_EQUALS(inf.nbrJointTargets(), (gum::Size)2);

      // should not be added since {0,1,2} already exists
      inf.addJointTarget(gum::NodeSet{0, 1});
      TS_ASSERT_EQUALS(inf.nbrJointTargets(), (gum::Size)2);

      // should remove {2,3} since {2,3,4} includes {2,3}
      inf.addJointTarget(gum::NodeSet{2, 3, 4});
      TS_ASSERT_EQUALS(inf.nbrJointTargets(), (gum::Size)2);
    }


    void testAsia() {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);

      for (auto node: bn.dag()) {
        const auto&              variable = bn.variable(node);
        gum::Potential< double > ev_pot;
        ev_pot << variable;
        ev_pot.fill(0.0f);
        gum::List< const gum::Potential< double >* > evidences;
        evidences.insert(&ev_pot);

        gum::Instantiation inst(ev_pot);
        for (inst.setFirst(); !inst.end(); ++inst) {
          ev_pot.set(inst, 1.0f);
          gum::VariableElimination< double >   inf1(&bn);
          gum::ShaferShenoyInference< double > inf2(&bn);
          for (auto pot: evidences) {
            TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot));
            TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot));
          }
          TS_ASSERT_THROWS_NOTHING(inf1.makeInference());
          TS_ASSERT_THROWS_NOTHING(inf2.makeInference());
          for (auto node: bn.dag()) {
            TS_ASSERT(equalPotentials(inf1.posterior(node), inf2.posterior(node)));
          }
          ev_pot.set(inst, (float)0);
        }
      }
    }

    void testAlarm2() {
      std::string              file = GET_RESSOURCES_PATH("bif/alarm.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);

      gum::VariableElimination< double > inf1(&bn);
      gum::VariableElimination< double > inf2(&bn);

      TS_ASSERT_THROWS_NOTHING(inf1.makeInference());
      TS_ASSERT_THROWS_NOTHING(inf2.makeInference());

      for (auto node: bn.dag()) {
        TS_ASSERT_THROWS_NOTHING(inf1.posterior(node));
        TS_ASSERT_THROWS_NOTHING(inf2.posterior(node));
        TS_ASSERT(equalPotentials(inf1.posterior(node), inf2.posterior(node)));
      }

      std::vector< gum::NodeId > ev_nodes{2, 6, 7, 10, 12, 14, 16};
      gum::List< const gum::Potential< double >* > evidences;
      for (const auto node: ev_nodes) {
        gum::Potential< double >* ev_pot = new gum::Potential< double >;

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
        TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot));
        TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot));
        TS_ASSERT_THROWS_NOTHING(inf3.addEvidence(*pot));
        TS_ASSERT_THROWS_NOTHING(inf4.addEvidence(*pot));
      }

      TS_ASSERT_THROWS_NOTHING(inf1.makeInference());
      TS_ASSERT_THROWS_NOTHING(inf2.makeInference());
      TS_ASSERT_THROWS_NOTHING(inf3.makeInference());
      TS_ASSERT_THROWS_NOTHING(inf4.makeInference());

      for (auto node: bn.dag()) {
        TS_ASSERT_THROWS_NOTHING(inf1.posterior(node));
        TS_ASSERT_THROWS_NOTHING(inf2.posterior(node));
        TS_ASSERT_THROWS_NOTHING(inf3.posterior(node));
        TS_ASSERT_THROWS_NOTHING(inf4.posterior(node));
        TS_ASSERT(equalPotentials(inf1.posterior(node), inf2.posterior(node)));
        TS_ASSERT(equalPotentials(inf1.posterior(node), inf3.posterior(node)));
        TS_ASSERT(equalPotentials(inf1.posterior(node), inf4.posterior(node)));
      }

      gum::VariableElimination< double > inf5(&bn);
      inf5.setRelevantPotentialsFinderType(
         gum::RelevantPotentialsFinderType::DSEP_BAYESBALL_NODES);
      for (auto pot: evidences) {
        TS_ASSERT_THROWS_NOTHING(inf5.addEvidence(*pot));
      }
      TS_ASSERT_THROWS_NOTHING(inf5.makeInference());
      for (auto node: bn.dag()) {
        TS_ASSERT_THROWS_NOTHING(inf5.posterior(node));
        TS_ASSERT(equalPotentials(inf1.posterior(node), inf5.posterior(node)));
      }

      for (auto pot: evidences)
        delete pot;
    }

    void testAsia2() {
      std::string              file = GET_RESSOURCES_PATH("bif/asia3.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);

      for (auto node: bn.dag()) {
        const auto&              variable = bn.variable(node);
        gum::Potential< double > ev_pot;
        ev_pot << variable;
        ev_pot.fill(0.0f);

        gum::Instantiation inst(ev_pot);
        for (inst.setFirst(); !inst.end(); ++inst) {
          ev_pot.set(inst, 1.0f);

          for (auto node2: bn.dag()) {
            if (node2 > node) {
              const auto&              variable2 = bn.variable(node2);
              gum::Potential< double > ev_pot2;
              ev_pot2 << variable2;
              ev_pot2.fill(0.0f);

              gum::List< const gum::Potential< double >* > evidences;
              evidences.insert(&ev_pot);
              evidences.insert(&ev_pot2);

              gum::Instantiation inst2(ev_pot2);
              for (inst2.setFirst(); !inst2.end(); ++inst2) {
                ev_pot2.set(inst2, 1.0f);

                gum::VariableElimination< double >   inf1(&bn);
                gum::ShaferShenoyInference< double > inf2(&bn);
                for (auto pot: evidences) {
                  TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot));
                  TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot));
                }
                TS_ASSERT_THROWS_NOTHING(inf1.makeInference());
                TS_ASSERT_THROWS_NOTHING(inf2.makeInference());

                for (auto xnode: bn.dag()) {
                  TS_ASSERT(equalPotentials(inf1.posterior(xnode),
                                            inf2.posterior(xnode)));
                }
                ev_pot2.set(inst2, 0.0f);
              }
            }
          }

          ev_pot.set(inst, 0.0f);
        }
      }
    }

    void testAsia3() {
      std::string              file = GET_RESSOURCES_PATH("bif/asia3.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);

      for (auto node: bn.dag()) {
        const auto&              variable = bn.variable(node);
        gum::Potential< double > ev_pot;
        ev_pot << variable;
        ev_pot.fill(0.0f);

        gum::Instantiation inst(ev_pot);
        for (inst.setFirst(); !inst.end(); ++inst) {
          ev_pot.set(inst, 1.0f);

          for (auto node2: bn.dag()) {
            if (node2 > node) {
              const auto&              variable2 = bn.variable(node2);
              gum::Potential< double > ev_pot2;
              ev_pot2 << variable2;
              ev_pot2.fill(0.0f);

              gum::List< const gum::Potential< double >* > evidences;
              evidences.insert(&ev_pot);
              evidences.insert(&ev_pot2);

              gum::Instantiation inst2(ev_pot2);
              for (inst2.setFirst(); !inst2.end(); ++inst2) {
                ev_pot2.set(inst2, 1.0f);

                gum::VariableElimination< double > inf1(&bn);
                inf1.setRelevantPotentialsFinderType(
                   gum::RelevantPotentialsFinderType::DSEP_BAYESBALL_NODES);
                gum::ShaferShenoyInference< double > inf2(&bn);
                for (auto pot: evidences) {
                  TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot));
                  TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot));
                }
                TS_ASSERT_THROWS_NOTHING(inf1.makeInference());
                TS_ASSERT_THROWS_NOTHING(inf2.makeInference());

                for (auto xnode: bn.dag()) {
                  TS_ASSERT(equalPotentials(inf1.posterior(xnode),
                                            inf2.posterior(xnode)));
                }
                ev_pot2.set(inst2, 0.0f);
              }
            }
          }

          ev_pot.set(inst, 0.0f);
        }
      }
    }

    void testAsia4() {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);

      for (auto node: bn.dag()) {
        const auto&              variable = bn.variable(node);
        gum::Potential< double > ev_pot;
        ev_pot << variable;
        ev_pot.fill(0.0f);

        gum::Instantiation inst(ev_pot);
        int                inst_index = 0;
        for (inst.setFirst(); !inst.end(); ++inst, ++inst_index) {
          ev_pot.set(inst, 1.0f);

          for (auto node2: bn.dag()) {
            if (node2 > node) {
              const auto&              variable2 = bn.variable(node2);
              gum::Potential< double > ev_pot2;
              ev_pot2 << variable2;
              ev_pot2.fill(0.0f);

              gum::List< const gum::Potential< double >* > evidences;
              evidences.insert(&ev_pot);
              evidences.insert(&ev_pot2);

              gum::Instantiation inst2(ev_pot2);
              int                inst2_index = 0;
              for (inst2.setFirst(); !inst2.end(); ++inst2, ++inst2_index) {
                ev_pot2.set(inst2, 1.0f);

                gum::VariableElimination< double > inf1(&bn);
                inf1.setRelevantPotentialsFinderType(
                   gum::RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS);
                gum::VariableElimination< double > inf2(&bn);
                for (auto pot: evidences) {
                  TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot));
                  TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot));
                }
                TS_ASSERT_THROWS_NOTHING(inf1.makeInference());
                TS_ASSERT_THROWS_NOTHING(inf2.makeInference());

                for (auto xnode: bn.dag()) {
                  try {
                    equalPotentials(inf1.posterior(xnode), inf2.posterior(xnode));
                  } catch (gum::IncompatibleEvidence&) {
                    // check evidence incompatibility:
                    if (node2 == gum::NodeId(2)) {
                      // node2 = tuberculos_or_cancer, then node =
                      // tuberculosis
                      TS_ASSERT((inst2_index == 1) && (inst_index == 0));
                    } else {   // node2 = lung_cancer & node =
                      // tuberculos_or_cancer
                      TS_ASSERT((inst2_index == 0) && (inst_index == 1));
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

    void testChgEvidence() {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);


      gum::VariableElimination< double > ie_0(&bn);
      ie_0.addTarget(0);        // visit_to_asia
      ie_0.addEvidence(1, 0);   // tuberculosis
      ie_0.makeInference();
      gum::Potential< double > p_0 = ie_0.posterior(0);

      gum::VariableElimination< double > ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Potential< double > p_1 = ie_1.posterior(0);

      gum::VariableElimination< double > ie(&bn);
      ie.addTarget(0);
      ie.addEvidence(1, 0);
      ie.makeInference();
      TS_ASSERT_EQUALS(p_0, ie.posterior(0));

      ie.chgEvidence(1, 1);
      ie.makeInference();
      TS_ASSERT_DIFFERS(p_0, ie.posterior(0));
      TS_ASSERT_EQUALS(p_1, ie.posterior(0));
    }

    void testChgEvidence2() {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);


      gum::VariableElimination< double > ie_0(&bn);
      ie_0.addTarget(0);        // visit_to_asia
      ie_0.addEvidence(1, 0);   // tuberculosis
      ie_0.makeInference();
      gum::Potential< double > p_0 = ie_0.posterior(0);

      gum::VariableElimination< double > ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Potential< double > p_1 = ie_1.posterior(0);

      gum::VariableElimination< double > ie(&bn);
      ie.eraseAllTargets();
      ie.addTarget(0);
      ie.addEvidence(1, 0);
      ie.makeInference();
      TS_ASSERT_EQUALS(p_0, ie.posterior(0));

      ie.chgEvidence(1, 1);
      ie.makeInference();
      TS_ASSERT_DIFFERS(p_0, ie.posterior(0));
      TS_ASSERT_EQUALS(p_1, ie.posterior(0));
    }

    void testStaticEvidenceImpact() {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == gum::Size(0));
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);

      gum::VariableElimination< double > ie_all(&bn);
      TS_ASSERT_THROWS(
         ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{0, 1, 2}),
         gum::InvalidArgument);

      auto res = ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{1, 2});

      TS_ASSERT_EQUALS(res.nbrDim(), gum::Size(2));   // 2 indep 0 given 1

      gum::VariableElimination< double > ie_0(&bn);
      ie_0.addTarget(0);        // visit_to_asia
      ie_0.addEvidence(1, 0);   // tuberculosis
      ie_0.makeInference();
      gum::Potential< double > p_0 = ie_0.posterior(0);

      gum::VariableElimination< double > ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Potential< double > p_1 = ie_1.posterior(0);

      gum::Instantiation i;
      i.add(bn.variable(1));
      i.setFirst();
      TS_ASSERT_EQUALS(p_0, res.extract(i));
      i.inc();
      TS_ASSERT_EQUALS(p_1, res.extract(i));
    }

    void testEvidenceImpactWithNames() {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());
      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);


      gum::VariableElimination< double > ie_all(&bn);
      TS_ASSERT_THROWS(
         ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{0, 1, 2}),
         gum::InvalidArgument);

      TS_ASSERT_THROWS(
         ie_all.evidenceImpact("visit_to_asia", {"tuberculoisis", "toto"}),
         gum::NotFound);

      auto res = ie_all.evidenceImpact("visit_to_Asia",
                                       {"tuberculosis", "tuberculos_or_cancer"});

      TS_ASSERT_EQUALS(res.nbrDim(), gum::Size(2));   // 2 indep 0 given 1

      gum::VariableElimination< double > ie_0(&bn);
      ie_0.addTarget(0);        // visit_to_asia
      ie_0.addEvidence(1, 0);   // tuberculosis
      ie_0.makeInference();
      gum::Potential< double > p_0 = ie_0.posterior(0);

      gum::VariableElimination< double > ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Potential< double > p_1 = ie_1.posterior(0);

      gum::Instantiation i;
      i.add(bn.variable(1));
      i.setFirst();
      TS_ASSERT_EQUALS(p_0, res.extract(i));
      i.inc();
      TS_ASSERT_EQUALS(p_1, res.extract(i));
    }

    void testEvidenceImpact() {
      /*
      F  A
      \ / \
       B   |
       |   E
       C   |
      / \ /
      H  D
      */
      auto bn =
         gum::BayesNet< double >::fastPrototype("A->B->C->D;A->E->D;F->B;C->H;");

      gum::VariableElimination< double > ie(&bn);
      gum::Potential< double >           res;
      TS_GUM_ASSERT_THROWS_NOTHING(
         res = ie.evidenceImpact("E", {"A", "B", "C", "D", "F"}));
      TS_ASSERT_EQUALS(res.nbrDim(), gum::Size(4));   // MarkovBlanket(E)=(A,D,C)
    }
    void testJointWithHardEvidence() {
      /*
      F  A
      \ / \
       B   |
       |   E
       C   |
      / \ /
      H  D
      */
      auto bn =
         gum::BayesNet< double >::fastPrototype("A->B->C->D;A->E->D;F->B;C->H;");

      gum::VariableElimination< double > ie(&bn);
      ie.addEvidence("B", 0);
      gum::NodeSet joint{
         bn.idFromName("A"), bn.idFromName("B"), bn.idFromName("D")};

      ie.addJointTarget(joint);
      ie.makeInference();
      try {
        auto p = ie.jointPosterior(joint);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }


    private:
    // Builds a BN to test the inference
    void fill(gum::BayesNet< double >& bn) {
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
      bn.cpt(i5).fillWith(
         {// clang-format off
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
    void printProba( const gum::Potential<double>& p ) {
      /*gum::Instantiation inst( p );
      for ( inst.setFirst(); !inst.end(); ++inst ) {
        std::cerr << inst << " : " << p[inst] << std::endl;
      }
      std::cerr << std::endl;*/
    }
  };
}
