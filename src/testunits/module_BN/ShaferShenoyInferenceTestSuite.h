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


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/config.h>

#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/variables/labelizedVariable.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/tools/relevantPotentialsFinderType.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/io/BIF/BIFReader.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class [[maybe_unused]] ShaferShenoyInferenceBNTestSuite: public CxxTest::TestSuite {
    public:
    gum::BayesNet< double >* bn;
    gum::NodeId              i1, i2, i3, i4, i5;

    void setUp() {
      bn = new gum::BayesNet< double >();

      gum::LabelizedVariable n1("1", "", 2), n2("2", "", 2), n3("3", "", 2);
      gum::LabelizedVariable n4("4", "", 2), n5("5", "", 3);

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
    }

    void tearDown() { delete bn; }

    [[nodiscard]] gum::Potential< double > joint(const gum::BayesNet< double >& bn) {
      gum::Potential< double > pot;
      for (const auto node: bn.dag()) {
        pot *= bn.cpt(node);
      }
      return pot;
    }

    [[nodiscard]] bool equalPotentials(const gum::Potential< double >& p1,
                                       const gum::Potential< double >& p2) const {
      gum::Instantiation ii(p1);

      for (ii.setFirst(); !ii.end(); ii.inc()) {
        if ((p1[ii] == 0) && (std::fabs(p2[ii]) > TS_GUM_SMALL_ERROR)) return false;
        if (p1[ii] > p2[ii]) {
          if (std::fabs((p1[ii] - p2[ii]) / p1[ii]) > TS_GUM_SMALL_ERROR) return false;
        } else {
          if (std::fabs((p1[ii] - p2[ii]) / p1[ii]) > TS_GUM_SMALL_ERROR) return false;
        }
      }

      return true;
    }

    // Testing when there is no evidence
    GUM_ACTIVE_TEST(CreationAndInference) {
      try {
        fill(*bn);
        // Testing the inference
        gum::ShaferShenoyInference< double >* inf = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(inf = new gum::ShaferShenoyInference< double >(bn))
        TS_GUM_ASSERT_THROWS_NOTHING(inf->makeInference())

        if (inf != 0) { TS_GUM_ASSERT_THROWS_NOTHING(delete inf); }
      } catch (gum::Exception& e) {
        TS_ASSERT(false)
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    GUM_ACTIVE_TEST(Marginal) {
      fill(*bn);
      gum::ShaferShenoyInference< double > inf(bn);

      TS_GUM_ASSERT_THROWS_NOTHING(inf.makeInference())
      TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(i1))
      TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(i2))
      TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(i3))
      TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(i4))
      TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(i5))

      const auto bn_joint = this->joint(*bn);
      gum::Set< const gum::DiscreteVariable* > vars;

      for (const auto node: bn->dag()) {
        vars.insert(&(bn->variable(node)));
        TS_ASSERT(equalPotentials(inf.posterior(node), bn_joint.margSumIn(vars)))
        vars.clear();
      }
    }

    GUM_ACTIVE_TEST(MarginalWithEvidence) {
      fill(*bn);
      gum::List< const gum::Potential< double >* > e_list;
      e_list.insert(const_cast< gum::Potential< double >* >(&(bn->cpt(i1))));
      e_list.insert(const_cast< gum::Potential< double >* >(&(bn->cpt(i2))));

      gum::ShaferShenoyInference< double > inf(bn);
      auto bn_joint = this->joint(*bn);
      for (auto pot: e_list) {
        bn_joint *= *pot;
      }

      TS_GUM_ASSERT_THROWS_NOTHING(inf.addListOfEvidence(e_list))
      TS_GUM_ASSERT_THROWS_NOTHING(inf.makeInference())

      gum::Set< const gum::DiscreteVariable* > vars;
      for (const auto node: bn->dag()) {
        vars.insert(&(bn->variable(node)));
        TS_ASSERT(equalPotentials(inf.posterior(node), bn_joint.margSumIn(vars).normalize()))
        vars.clear();
      }
    }

    // Testing when there is no evidence
    GUM_ACTIVE_TEST(Joint) {
      fill(*bn);
      // Testing the inference
      gum::ShaferShenoyInference< double > inf(bn);
      gum::NodeSet                         nodeset;
      nodeset.insert(2);
      nodeset.insert(4);
      inf.addJointTarget(nodeset);

      TS_ASSERT_THROWS_NOTHING(inf.jointPosterior(nodeset))

      const auto bn_joint = this->joint(*bn);
      gum::Set< const gum::DiscreteVariable* > vars;
      vars.insert(&(bn->variable(2)));
      vars.insert(&(bn->variable(4)));
      TS_ASSERT(equalPotentials(inf.jointPosterior(nodeset), bn_joint.margSumIn(vars)))
    }

    // Testing when there is no evidence
    GUM_ACTIVE_TEST(Joint2) {
      fill(*bn);
      // Testing the inference
      gum::ShaferShenoyInference< double > inf(bn);
      gum::NodeSet                         nodeset;
      nodeset.insert(1);
      nodeset.insert(2);
      nodeset.insert(3);
      nodeset.insert(4);
      inf.eraseAllTargets();
      inf.addJointTarget(nodeset);

      gum::NodeSet nodeset2;
      nodeset2.insert(2);
      nodeset2.insert(4);
      const auto bn_joint = this->joint(*bn);
      gum::Set< const gum::DiscreteVariable* > vars;
      vars.insert(&(bn->variable(2)));
      vars.insert(&(bn->variable(4)));

      TS_ASSERT_THROWS_NOTHING(inf.jointPosterior(nodeset2))
      TS_ASSERT(equalPotentials(inf.jointPosterior(nodeset2), bn_joint.margSumIn(vars)))

      TS_ASSERT_THROWS_NOTHING(inf.posterior(3))
      vars.clear();
      vars.insert(&(bn->variable(3)));
      TS_ASSERT(equalPotentials(inf.posterior(3), bn_joint.margSumIn(vars)))

      vars.insert(&(bn->variable(1)));
      vars.insert(&(bn->variable(2)));
      vars.insert(&(bn->variable(4)));
      TS_ASSERT(equalPotentials(inf.jointPosterior(nodeset), bn_joint.margSumIn(vars)))
    }

    // testing information methods
    GUM_ACTIVE_TEST(InformationMethods) {
      fill(*bn);

      gum::ShaferShenoyInference< double > inf(bn);

      TS_GUM_ASSERT_THROWS_NOTHING(inf.H((gum::NodeId)2))

      //@TODO : test computations and not only good behaviour
    }

    GUM_ACTIVE_TEST(WithGenerator) {
      gum::Size density[] = {9, 18, 27, 36, 45};
      int       trial_nb  = 5;

      for (int i = 0; i < trial_nb; ++i) {
        gum::SimpleBayesNetGenerator< double > bnGen(10, density[i]);

        auto bayesNet = new gum::BayesNet< double >();
        bnGen.generateBN(*bayesNet);

        gum::ShaferShenoyInference< double >* inf = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(inf = new gum::ShaferShenoyInference< double >(bayesNet))

        TS_GUM_ASSERT_THROWS_NOTHING(if (inf) inf->makeInference())

        TS_GUM_ASSERT_THROWS_NOTHING(if (inf) delete inf)

        delete bayesNet;
      }
    }

    GUM_ACTIVE_TEST(SmartManagementOfJointTarget) {
      fill(*bn);

      gum::ShaferShenoyInference< double > inf(bn);
      inf.addJointTarget(gum::NodeSet{0, 1, 2});
      inf.addJointTarget(gum::NodeSet{2, 3});
      TS_ASSERT_EQUALS(inf.nbrJointTargets(), (gum::Size)2)

      // should not be added since {0,1,2} already exists
      inf.addJointTarget(gum::NodeSet{0, 1});
      TS_ASSERT_EQUALS(inf.nbrJointTargets(), (gum::Size)2)

      // should remove {2,3} since {2,3,4} includes {2,3}
      inf.addJointTarget(gum::NodeSet{2, 3, 4});
      TS_ASSERT_EQUALS(inf.nbrJointTargets(), (gum::Size)2)

      auto bn_joint = this->joint(*bn);
      gum::Set< const gum::DiscreteVariable* > vars;
      vars.insert(&(bn->variable(0)));
      vars.insert(&(bn->variable(1)));
      vars.insert(&(bn->variable(2)));
      TS_ASSERT(equalPotentials(inf.jointPosterior(gum::NodeSet{0, 1, 2}), bn_joint.margSumIn(vars).normalize()))

      vars.clear();
      vars.insert(&(bn->variable(2)));
      vars.insert(&(bn->variable(3)));
      TS_ASSERT(equalPotentials(inf.jointPosterior(gum::NodeSet{2, 3}), bn_joint.margSumIn(vars)))

      vars.clear();
      vars.insert(&(bn->variable(0)));
      vars.insert(&(bn->variable(1)));
      TS_ASSERT(equalPotentials(inf.jointPosterior(gum::NodeSet{0, 1}), bn_joint.margSumIn(vars)))

      vars.clear();
      vars.insert(&(bn->variable(2)));
      vars.insert(&(bn->variable(3)));
      vars.insert(&(bn->variable(4)));
      TS_ASSERT(equalPotentials(inf.jointPosterior(gum::NodeSet{2, 3, 4}), bn_joint.margSumIn(vars)))
    }

    GUM_ACTIVE_TEST(EvidenceProbability) {
      fill(*bn);
      gum::ShaferShenoyInference< double > inf(bn);
      inf.makeInference();
      auto p     = inf.posterior(0);
      auto I     = gum::Instantiation(p);
      auto proba = p.get(I);

      inf.addEvidence(0, 0);
      inf.makeInference();
      auto proba2 = inf.evidenceProbability();

      TS_ASSERT_DELTA(proba, proba2, TS_GUM_SMALL_ERROR)

      auto bn_joint = this->joint(*bn);
      gum::Potential< double > ev0;
      const auto& var0 = bn->variable(0);
      ev0 << var0;
      ev0.fillWith(0.0);
      gum::Instantiation inst0(ev0);
      inst0.chgVal(var0, 0);
      ev0.set(inst0, 1);
      bn_joint *= ev0;
      const auto proba3 = bn_joint.margSumIn({&var0}).sum();

      TS_ASSERT_DELTA(proba, proba3, TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(EvidenceProbability2) {
      fill(*bn);
      gum::ShaferShenoyInference< double > inf(bn);
      inf.makeInference();
      auto p     = inf.posterior(0);
      auto I     = gum::Instantiation(p);
      auto proba = p.get(I);

      inf.addEvidence(0, 0);
      inf.makeInference();
      auto proba2 = inf.evidenceProbability();

      TS_ASSERT_DELTA(proba, proba2, TS_GUM_SMALL_ERROR)

      auto bn_joint = this->joint(*bn);
      gum::Potential< double > ev0;
      const auto& var0 = bn->variable(0);
      ev0 << var0;
      ev0.fillWith(0.0);
      gum::Instantiation inst0(ev0);
      inst0.chgVal(var0, 0);
      ev0.set(inst0, 1);
      bn_joint *= ev0;
      const auto proba3 = bn_joint.margSumIn({&var0}).sum();

      TS_ASSERT_DELTA(proba, proba3, TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(EvidenceProbabilityAsia) {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)(gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)

      auto id = bn.idFromName("lung_cancer");

      gum::ShaferShenoyInference< double > inf(&bn);
      inf.makeInference();
      auto p     = inf.posterior(id);
      auto I     = gum::Instantiation(p);
      auto proba = p.get(I);

      inf.addEvidence(id, 0);
      inf.makeInference();
      auto proba2 = inf.evidenceProbability();

      TS_ASSERT_DELTA(proba, proba2, TS_GUM_SMALL_ERROR)

      auto bn_joint = this->joint(bn);
      gum::Potential< double > ev0;
      const auto& var0 = bn.variable(id);
      ev0 << var0;
      ev0.fillWith(0.0);
      gum::Instantiation inst0(ev0);
      inst0.chgVal(var0, 0);
      ev0.set(inst0, 1);
      bn_joint *= ev0;
      const auto proba3 = bn_joint.margSumIn({&var0}).sum();

      TS_ASSERT_DELTA(proba, proba3, TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(Asia) {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)(gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)

      const auto bn_joint = this->joint(bn);

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
          gum::LazyPropagation< double >       inf1(&bn);
          gum::ShaferShenoyInference< double > inf2(&bn);
          auto joint = bn_joint;
          for (auto pot: evidences) {
            TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot))
            TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot))
            joint *= *pot;
          }
          TS_ASSERT_THROWS_NOTHING(inf1.makeInference())
          TS_ASSERT_THROWS_NOTHING(inf2.makeInference())
          for (auto node: bn.dag()) {
            TS_ASSERT(equalPotentials(inf1.posterior(node), inf2.posterior(node)))
            TS_ASSERT(equalPotentials(inf2.posterior(node), joint.margSumIn({&bn.variable(node)}).normalize()))
          }
          ev_pot.set(inst, (float)0);
        }
      }
    }

    GUM_ACTIVE_TEST(Alarm) {
      std::string              file = GET_RESSOURCES_PATH("bif/alarm.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)(gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)

      gum::ShaferShenoyInference< double > inf1(&bn);
      gum::VariableElimination< double >   inf2(&bn);

      TS_ASSERT_THROWS_NOTHING(inf1.makeInference())
      TS_ASSERT_THROWS_NOTHING(inf2.makeInference())

      for (auto node: bn.dag()) {
        TS_ASSERT_THROWS_NOTHING(inf1.posterior(node))
        TS_ASSERT_THROWS_NOTHING(inf2.posterior(node))
        TS_ASSERT(equalPotentials(inf1.posterior(node), inf2.posterior(node)))
      }

      std::vector< gum::NodeId >                   ev_nodes{2, 6, 7, 10, 12, 14, 16};
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

      gum::ShaferShenoyInference< double > inf3(&bn);
      gum::LazyPropagation< double >       inf4(&bn);
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
        TS_ASSERT(equalPotentials(inf1.posterior(node), inf2.posterior(node)))
        TS_ASSERT(equalPotentials(inf1.posterior(node), inf3.posterior(node)))
        TS_ASSERT(equalPotentials(inf1.posterior(node), inf4.posterior(node)))
      }

      gum::ShaferShenoyInference< double > inf5(&bn);
      for (auto pot: evidences) {
        TS_ASSERT_THROWS_NOTHING(inf5.addEvidence(*pot))
      }
      TS_ASSERT_THROWS_NOTHING(inf5.makeInference())
      for (auto node: bn.dag()) {
        TS_ASSERT_THROWS_NOTHING(inf5.posterior(node))
        TS_ASSERT(equalPotentials(inf1.posterior(node), inf5.posterior(node)))
      }

      for (auto pot: evidences)
        delete pot;
    }

    GUM_ACTIVE_TEST(Asia2) {
      std::string              file = GET_RESSOURCES_PATH("bif/asia3.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)(gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)

      const auto bn_joint = this->joint(bn);

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

                gum::ShaferShenoyInference< double > inf1(&bn);
                gum::LazyPropagation< double >       inf2(&bn);
                auto joint = bn_joint;
                for (auto pot: evidences) {
                  TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot))
                  TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot))
                  joint *= *pot;
                }
                TS_ASSERT_THROWS_NOTHING(inf1.makeInference())
                TS_ASSERT_THROWS_NOTHING(inf2.makeInference())

                for (auto xnode: bn.dag()) {
                  TS_ASSERT(equalPotentials(inf1.posterior(xnode), inf2.posterior(xnode)))
                  TS_ASSERT(equalPotentials(inf1.posterior(xnode), joint.margSumIn({&bn.variable(xnode)}).normalize()))

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
      std::string              file = GET_RESSOURCES_PATH("bif/asia3.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)(gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)

      const auto bn_joint = this->joint(bn);

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

                gum::ShaferShenoyInference< double > inf1(&bn);
                gum::LazyPropagation< double >       inf2(&bn);
                auto joint = bn_joint;
                for (auto pot: evidences) {
                  TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot))
                  TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot))
                  joint *= *pot;
                }
                TS_ASSERT_THROWS_NOTHING(inf1.makeInference())
                TS_ASSERT_THROWS_NOTHING(inf2.makeInference())

                for (auto xnode: bn.dag()) {
                  TS_ASSERT(equalPotentials(inf1.posterior(xnode), joint.margSumIn({&bn.variable(xnode)}).normalize()))
                  TS_ASSERT(equalPotentials(inf1.posterior(xnode), inf2.posterior(xnode)))
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
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)(gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)

      const auto bn_joint = this->joint(bn);

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

                gum::ShaferShenoyInference< double > inf1(&bn);
                gum::LazyPropagation< double >       inf2(&bn);
                auto joint = bn_joint;
                for (auto pot: evidences) {
                  TS_ASSERT_THROWS_NOTHING(inf1.addEvidence(*pot))
                  TS_ASSERT_THROWS_NOTHING(inf2.addEvidence(*pot))
                  joint *= *pot;
                }
                TS_ASSERT_THROWS_NOTHING(inf1.makeInference())
                TS_ASSERT_THROWS_NOTHING(inf2.makeInference())

                for (auto xnode: bn.dag()) {
                  try {
                    const auto res = joint.margSumIn({&bn.variable(xnode)});
                    if (res.sum() > TS_GUM_SMALL_ERROR) {
                      TS_ASSERT(equalPotentials(inf1.posterior(xnode),
                                                joint.margSumIn({&bn.variable(xnode)}).normalize()))
                    }
                    [[maybe_unused]] auto f =
                       equalPotentials(inf1.posterior(xnode), inf2.posterior(xnode));
                  } catch (gum::IncompatibleEvidence&) {
                    // check evidence incompatibility:
                    if (node2 == gum::NodeId(2)) {
                      // node2 = tuberculos_or_cancer, then node =
                      // tuberculosis
                      TS_ASSERT((inst2_index == 1) && (inst_index == 0))
                    } else {   // node2 = lung_cancer & node =
                      // tuberculos_or_cancer
                      TS_ASSERT((inst2_index == 0) && (inst_index == 1))
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
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)(gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)

      const auto bn_joint = this->joint(bn);

      gum::ShaferShenoyInference< double > ie_0(&bn);
      ie_0.addTarget(0);        // visit_to_asia
      ie_0.addEvidence(1, 0);   // tuberculosis
      ie_0.makeInference();
      gum::Potential< double > p_0 = ie_0.posterior(0);
      gum::Potential< double > ev_pot0;
      const auto& var1 = bn.variable(1);
      ev_pot0 << var1;
      ev_pot0.fillWith(0.0);
      gum::Instantiation inst0(ev_pot0);
      inst0.chgVal(var1, 0);
      ev_pot0.set(inst0, 1);
      auto joint0 = bn_joint * ev_pot0;

      gum::ShaferShenoyInference< double > ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Potential< double > p_1 = ie_1.posterior(0);
      gum::Potential< double > ev_pot1;
      ev_pot1 << var1;
      ev_pot1.fillWith(0.0);
      gum::Instantiation inst1(ev_pot1);
      inst1.chgVal(var1, 1);
      ev_pot1.set(inst1, 1);
      auto joint1 = bn_joint * ev_pot1;

      gum::ShaferShenoyInference< double > ie(&bn);
      ie.addTarget(0);
      ie.addEvidence(1, 0);
      ie.makeInference();
      TS_ASSERT_EQUALS(p_0, ie.posterior(0))
      const auto& var0 = bn.variable(0);
      TS_ASSERT(equalPotentials(ie.posterior(0), joint0.margSumIn({&var0}).normalize()))

      ie.chgEvidence(1, 1);
      ie.makeInference();
      TS_ASSERT_DIFFERS(p_0, ie.posterior(0))
      TS_ASSERT_EQUALS(p_1, ie.posterior(0))
      TS_ASSERT(equalPotentials(ie.posterior(0), joint1.margSumIn({&var0}).normalize()))
    }

    GUM_ACTIVE_TEST(ChgEvidence2) {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)(gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)

      const auto bn_joint = this->joint(bn);

      gum::ShaferShenoyInference< double > ie_0(&bn);
      ie_0.addTarget(0);        // visit_to_asia
      ie_0.addEvidence(1, 0);   // tuberculosis
      ie_0.makeInference();
      gum::Potential< double > p_0 = ie_0.posterior(0);
      gum::Potential< double > ev_pot0;
      const auto& var1 = bn.variable(1);
      ev_pot0 << var1;
      ev_pot0.fillWith(0.0);
      gum::Instantiation inst0(ev_pot0);
      inst0.chgVal(var1, 0);
      ev_pot0.set(inst0, 1);
      auto joint0 = bn_joint * ev_pot0;

      gum::ShaferShenoyInference< double > ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Potential< double > p_1 = ie_1.posterior(0);
      gum::Potential< double > ev_pot1;
      ev_pot1 << var1;
      ev_pot1.fillWith(0.0);
      gum::Instantiation inst1(ev_pot1);
      inst1.chgVal(var1, 1);
      ev_pot1.set(inst1, 1);
      auto joint1 = bn_joint * ev_pot1;

      gum::ShaferShenoyInference< double > ie(&bn);
      ie.eraseAllTargets();
      ie.addTarget(0);
      ie.addEvidence(1, 0);
      ie.makeInference();
      TS_ASSERT_EQUALS(p_0, ie.posterior(0))
      const auto& var0 = bn.variable(0);
      TS_ASSERT(equalPotentials(ie.posterior(0), joint0.margSumIn({&var0}).normalize()))

      ie.chgEvidence(1, 1);
      ie.makeInference();
      TS_ASSERT_DIFFERS(p_0, ie.posterior(0))
      TS_ASSERT_EQUALS(p_1, ie.posterior(0))
      TS_ASSERT(equalPotentials(ie.posterior(0), joint1.margSumIn({&var0}).normalize()))
    }

    GUM_ACTIVE_TEST(StaticEvidenceImpact) {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)

      gum::ShaferShenoyInference< double > ie_all(&bn);
      TS_ASSERT_THROWS(ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{0, 1, 2}),
                       const gum::InvalidArgument&)

      auto res = ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{1, 2});

      TS_ASSERT_EQUALS(res.nbrDim(), (gum::Size)2);   // 2 indep 0 given 1

      gum::ShaferShenoyInference< double > ie_0(&bn);
      ie_0.addTarget(0);                              // visit_to_asia
      ie_0.addEvidence(1, 0);                         // tuberculosis
      ie_0.makeInference();
      gum::Potential< double > p_0 = ie_0.posterior(0);

      gum::ShaferShenoyInference< double > ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Potential< double > p_1 = ie_1.posterior(0);

      gum::Instantiation i;
      i.add(bn.variable(1));
      i.setFirst();
      TS_ASSERT_EQUALS(p_0, res.extract(i))
      i.inc();
      TS_ASSERT_EQUALS(p_1, res.extract(i))
    }

    GUM_ACTIVE_TEST(EvidenceImpactWithNames) {
      std::string              file = GET_RESSOURCES_PATH("bif/asia.bif");
      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);

      gum::Size nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())
      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)


      gum::ShaferShenoyInference< double > ie_all(&bn);
      TS_ASSERT_THROWS(ie_all.evidenceImpact(gum::NodeId(0), gum::NodeSet{0, 1, 2}),
                       const gum::InvalidArgument&)

      TS_ASSERT_THROWS(ie_all.evidenceImpact("visit_to_asia", {"tuberculoisis", "toto"}),
                       const gum::NotFound&)

      auto res = ie_all.evidenceImpact("visit_to_Asia", {"tuberculosis", "tuberculos_or_cancer"});

      TS_ASSERT_EQUALS(res.nbrDim(), (gum::Size)2);   // 2 indep 0 given 1

      gum::ShaferShenoyInference< double > ie_0(&bn);
      ie_0.addTarget(0);                              // visit_to_asia
      ie_0.addEvidence(1, 0);                         // tuberculosis
      ie_0.makeInference();
      gum::Potential< double > p_0 = ie_0.posterior(0);

      gum::ShaferShenoyInference< double > ie_1(&bn);
      ie_1.addTarget(0);
      ie_1.addEvidence(1, 1);
      ie_1.makeInference();
      gum::Potential< double > p_1 = ie_1.posterior(0);

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

      gum::ShaferShenoyInference< double > ie(&bn);
      gum::Potential< double >             res;
      TS_GUM_ASSERT_THROWS_NOTHING(res = ie.evidenceImpact("E", {"A", "B", "C", "D", "F"}))
      TS_ASSERT_EQUALS(res.nbrDim(), (gum::Size)4);   // MarkovBlanket(E)=(A,D,C)
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

      gum::ShaferShenoyInference< double > ie(&bn);
      ie.addEvidence("B", 0);
      gum::NodeSet joint{bn.idFromName("A"), bn.idFromName("B"), bn.idFromName("D")};

      gum::Potential< double > ev_potB;
      const auto& varB = bn.variableFromName("B");
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
        gum::Set< const gum::DiscreteVariable* >
          xjoint{&bn.variableFromName("A"),&bn.variableFromName("B"),&bn.variableFromName("D")};
        TS_ASSERT(equalPotentials(p, pjoint.margSumIn(xjoint).normalize()))
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false)
      }
    }

    GUM_ACTIVE_TEST(AdaptiveNbThreads) {
      fill(*bn);
      // Testing the inference
      gum::ShaferShenoyInference< double > inf(bn);

      TS_ASSERT(inf.getNumberOfThreads() == gum::getNumberOfThreads());
      inf.setNumberOfThreads(10);
      TS_ASSERT(inf.getNumberOfThreads() == 10);
      gum::setNumberOfThreads(25);
      TS_ASSERT(inf.getNumberOfThreads() == 10);
      inf.setNumberOfThreads(0);
      TS_ASSERT(inf.getNumberOfThreads() == gum::getNumberOfThreads());
      gum::setNumberOfThreads(32);
      TS_ASSERT(inf.getNumberOfThreads() == gum::getNumberOfThreads());
    }

    GUM_ACTIVE_TEST(ProbaEvidence) {
      std::vector< gum::LabelizedVariable* > vars(10);

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

      gum::Potential< double > ev0;
      ev0 << x0;
      ev0.fillWith({0.3, 0.2});

      gum::Potential< double > ev0b;
      ev0b << x0;
      ev0b.fillWith({1.0, 0.0});

      {
        gum::ShaferShenoyInference< double > inf(&bn);
        inf.addEvidence(ev0);
        double xpe0 = inf.evidenceProbability();
        double pe0  = (p0 * ev0).sum();
        TS_ASSERT_DELTA(pe0, 0.27, 0.0001)
        TS_ASSERT_DELTA(xpe0, pe0, 0.0001)
      }

      {
        gum::ShaferShenoyInference< double > inf(&bn);
        inf.addEvidence(ev0b);
        double xpe0b = inf.evidenceProbability();
        double pe0b  = (p0 * ev0b).sum();
        TS_ASSERT_DELTA(pe0b, 0.7, 0.0001)
        TS_ASSERT_DELTA(xpe0b, pe0b, 0.0001)
      }

      gum::NodeId i1 = bn.add(*vars[1]);
      auto&       x1 = bn.variable(i1);
      auto&       p1 = bn.cpt(i1);
      p1.fillWith({0.4, 0.6});

      gum::Potential< double > ev1;
      ev1 << x1;
      ev1.fillWith({0.4, 0.6});

      gum::Potential< double > ev1b;
      ev1b << x1;
      ev1b.fillWith({0.0, 1.0});

      {
        gum::ShaferShenoyInference< double > inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        double xpe01 = inf.evidenceProbability();
        double pe01  = (p0 * ev0 * p1 * ev1).sum();
        TS_ASSERT_DELTA(pe01, 0.27 * 0.52, 0.0001)
        TS_ASSERT_DELTA(xpe01, pe01, 0.0001)
      }

      {
        gum::ShaferShenoyInference< double > inf(&bn);
        inf.addEvidence(ev0b);
        inf.addEvidence(ev1);
        double xpe01 = inf.evidenceProbability();
        double pe01  = (p0 * ev0b * p1 * ev1).sum();
        TS_ASSERT_DELTA(pe01, 0.7 * 0.52, 0.0001)
        TS_ASSERT_DELTA(xpe01, pe01, 0.0001)
      }

      {
        gum::ShaferShenoyInference< double > inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1b);
        double xpe01 = inf.evidenceProbability();
        double pe01  = (p0 * ev0 * p1 * ev1b).sum();
        TS_ASSERT_DELTA(pe01, 0.27 * 0.6, 0.0001)
        TS_ASSERT_DELTA(xpe01, pe01, 0.0001)
      }

      {
        gum::ShaferShenoyInference< double > inf(&bn);
        inf.addEvidence(ev0b);
        inf.addEvidence(ev1b);
        double xpe01 = inf.evidenceProbability();
        double pe01  = (p0 * ev0b * p1 * ev1b).sum();
        TS_ASSERT_DELTA(pe01, 0.7 * 0.6, 0.0001)
        TS_ASSERT_DELTA(xpe01, pe01, 0.0001)
      }

      gum::NodeId i2 = bn.add(*vars[2]);
      bn.addArc(i0, i2);
      auto& x2 = bn.variable(i2);
      auto& p2 = bn.cpt(i2);
      p2.fillWith({0.4, 0.6, 0.8, 0.2});

      {
        gum::ShaferShenoyInference< double > inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        double xpe02 = inf.evidenceProbability();
        double pe02  = (p0 * ev0 * p1 * ev1 * p2).sum();
        TS_ASSERT_DELTA(pe02, 0.27 * 0.52, 0.0001)
        TS_ASSERT_DELTA(xpe02, pe02, 0.0001)
      }

      gum::NodeId i3 = bn.add(*vars[3]);
      bn.addArc(i2, i3);
      auto& x3 = bn.variable(i3);
      auto& p3 = bn.cpt(i3);
      p3.fillWith({0.1, 0.9, 0.7, 0.3});

      {
        gum::ShaferShenoyInference< double > inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        double xpe03 = inf.evidenceProbability();
        double pe03  = (p0 * ev0 * p1 * ev1 * p2 * p3).sum();
        TS_ASSERT_DELTA(pe03, 0.27 * 0.52, 0.0001)
        TS_ASSERT_DELTA(xpe03, pe03, 0.0001)
      }

      gum::Potential< double > ev3;
      ev3 << x3;
      ev3.fillWith({0.4, 0.3});

      gum::Potential< double > ev3b;
      ev3b << x3;
      ev3b.fillWith({1.0, 0.0});

      {
        gum::ShaferShenoyInference< double > inf(&bn);
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
        gum::ShaferShenoyInference< double > inf(&bn);
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

      gum::NodeId i4 = bn.add(*vars[4]);
      bn.addArc(i1, i4);
      bn.addArc(i3, i4);
      auto& x4 = bn.variable(i4);
      auto& p4 = bn.cpt(i4);
      p4.fillWith({0.2, 0.8, 0.3, 0.7, 0.4, 0.6, 0.5, 0.5});

      gum::NodeId i5 = bn.add(*vars[5]);
      bn.addArc(i3, i5);
      auto& x5 = bn.variable(i5);
      auto& p5 = bn.cpt(i5);
      p5.fillWith({0.7, 0.3, 0.8, 0.2});

      gum::Potential< double > ev5;
      ev5 << x5;
      ev5.fillWith({0.7, 0.6});

      gum::Potential< double > pot0_5;
      {
        gum::ShaferShenoyInference< double > inf(&bn);
        inf.addJointTarget({i0, i1, i5});
        pot0_5 = inf.jointPosterior({i0, i1, i5});
      }

      {
        gum::ShaferShenoyInference< double > inf(&bn);
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

      gum::Potential< double > ev6;
      ev6 << x6;
      ev6.fillWith({0.7, 0.6});

      {
        gum::ShaferShenoyInference< double > inf(&bn);
        inf.addEvidence(ev0);
        inf.addEvidence(ev1);
        inf.addEvidence(ev5);
        inf.addEvidence(ev6);

        double xpe0_6 = inf.evidenceProbability();
        double pe0_6  = (pot0_5 * ev0 * ev1 * ev5 * p6 * ev6).sum();
        TS_ASSERT_DELTA(xpe0_6, pe0_6, 0.0001)
      }

      gum::NodeId i7 = bn.add(*vars[7]);
      bn.addArc(i4, i7);
      auto& x7 = bn.variable(i7);
      auto& p7 = bn.cpt(i7);
      p7.fillWith({0.4, 0.6, 0.6, 0.4});

      gum::Potential< double > ev7;
      ev7 << x7;
      ev7.fillWith({0.2, 0.5});

      gum::Potential< double > pot0_7;
      {
        gum::ShaferShenoyInference< double > inf(&bn);
        inf.addJointTarget({i0, i1, i5, i7});
        pot0_7 = inf.jointPosterior({i0, i1, i5, i7});
      }

      {
        gum::ShaferShenoyInference< double > inf(&bn);
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

      gum::NodeId i0 = xbn.add(*vars[0]);
      gum::NodeId i1 = xbn.add(*vars[1]);
      gum::NodeId i2 = xbn.add(*vars[2]);
      gum::NodeId i3 = xbn.add(*vars[3]);
      gum::NodeId i4 = xbn.add(*vars[4]);

      xbn.addArc(i0, i2);
      xbn.addArc(i1, i2);
      xbn.addArc(i2, i3);
      xbn.addArc(i3, i4);

      auto& x0 = xbn.variable(i0);
      auto& x1 = xbn.variable(i1);
      auto& x2 = xbn.variable(i2);
      auto& x3 = xbn.variable(i3);
      auto& x4 = xbn.variable(i4);

      auto& p0 = xbn.cpt(i0);
      auto& p1 = xbn.cpt(i1);
      auto& p2 = xbn.cpt(i2);
      auto& p3 = xbn.cpt(i3);
      auto& p4 = xbn.cpt(i4);

      p0.fillWith({0.7, 0.3});
      p1.fillWith({0.4, 0.6});
      p2.fillWith({0.2, 0.8, 0.3, 0.7, 0.4, 0.6, 0.5, 0.5});
      p3.fillWith({0.8, 0.2, 0.2, 0.8});
      p4.fillWith({0.4, 0.6, 0.3, 0.7});

      {
        gum::ShaferShenoyInference< double > inf(&xbn);
        inf.addTarget("x0");
        TS_ASSERT(inf.targets().size() == 1)
        inf.makeInference();
        TS_ASSERT_THROWS(inf.posterior(i3), const gum::UndefinedElement&)
        inf.addTarget("x3");
        const auto& pot3 = inf.posterior(i3);
        TS_ASSERT(inf.junctionTree()->sizeNodes() == 2)

        gum::VariableSet               set3{&x0, &x1, &x2, &x4};
        const gum::Potential< double > pot3bis
           = (p0 * p1 * p2 * p3 * p4).margSumOut(set3).normalize();
        TS_ASSERT(equalPotentials(pot3, pot3bis))
      }

      {
        gum::ShaferShenoyInference< double > inf(&xbn);
        inf.addTarget("x0");
        inf.addTarget("x2");
        TS_ASSERT(inf.targets().size() == 2)
        inf.makeInference();
        TS_ASSERT(inf.junctionTree()->sizeNodes() == 1)

        inf.addEvidence(i4, 1);
        const auto& pot2 = inf.posterior(i2);
        TS_ASSERT(inf.junctionTree()->sizeNodes() == 2)

        gum::VariableSet         set2{&x0, &x1, &x3, &x4};
        gum::Potential< double > ev4;
        ev4 << x4;
        ev4.fillWith({0.0, 1.0});
        const gum::Potential< double > pot2bis
           = (p0 * p1 * p2 * p3 * p4 * ev4).margSumOut(set2).normalize();
        TS_ASSERT(equalPotentials(pot2, pot2bis))
      }

      {
        gum::ShaferShenoyInference< double > inf(&xbn);
        inf.addTarget("x0");
        TS_ASSERT(inf.targets().size() == 1)
        inf.makeInference();
        TS_ASSERT_THROWS(inf.posterior(i2), const gum::UndefinedElement&)
        TS_ASSERT(inf.junctionTree()->sizeNodes() == 1)

        inf.addTarget("x2");
        inf.addEvidence(i4, 1);
        const auto& pot2 = inf.posterior(i2);
        TS_ASSERT(inf.junctionTree()->sizeNodes() == 2)

        gum::VariableSet         set2{&x0, &x1, &x3, &x4};
        gum::Potential< double > ev4;
        ev4 << x4;
        ev4.fillWith({0.0, 1.0});
        const gum::Potential< double > pot2bis
           = (p0 * p1 * p2 * p3 * p4 * ev4).margSumOut(set2).normalize();
        TS_ASSERT(equalPotentials(pot2, pot2bis))
      }

      {
        gum::ShaferShenoyInference< double > inf(&xbn);
        inf.addTarget("x0");
        TS_ASSERT(inf.targets().size() == 1)
        inf.makeInference();
        TS_ASSERT_THROWS(inf.posterior(i2), const gum::UndefinedElement&)
        TS_ASSERT(inf.junctionTree()->sizeNodes() == 1)

        inf.addTarget("x2");
        inf.addEvidence(i4, 1);
        inf.makeInference();
        const auto& pot2 = inf.posterior(i2);
        TS_ASSERT(inf.junctionTree()->sizeNodes() == 2)

        gum::VariableSet         set2{&x0, &x1, &x3, &x4};
        gum::Potential< double > ev4;
        ev4 << x4;
        ev4.fillWith({0.0, 1.0});
        const gum::Potential< double > pot2bis
           = (p0 * p1 * p2 * p3 * p4 * ev4).margSumOut(set2).normalize();
        TS_ASSERT(equalPotentials(pot2, pot2bis))
      }

      for (const auto var: vars)
        delete var;
    }

    GUM_ACTIVE_TEST(ImplicitTargetAllCheck) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->Y->E->F->G;W->E<-Z;X->E");
      auto ie = gum::ShaferShenoyInference(&bn);
      ie.addJointTarget(bn.nodeset({"B", "Y", "F"}));
      auto p = gum::Potential< double >();
      for (const auto n: bn.nodes())
        p *= bn.cpt(n);

      // target
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"B", "Y", "F"})),
                             p.margSumIn(bn.variables({"B", "Y", "F"})),
                             TS_GUM_SMALL_ERROR)
      // subtargets
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"B", "Y"})),
                             p.margSumIn(bn.variables({"B", "Y"})),
                             TS_GUM_SMALL_ERROR)
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"F", "Y"})),
                             p.margSumIn(bn.variables({"F", "Y"})),
                             TS_GUM_SMALL_ERROR)
      // implicit target
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"W", "Z", "X"})),
                             p.margSumIn(bn.variables({"W", "Z", "X"})),
                             TS_GUM_SMALL_ERROR)

      // impossible target in optimized inference
      TS_ASSERT_THROWS(ie.jointPosterior(bn.nodeset({"A", "E"})), gum::UndefinedElement&)
    }

    GUM_ACTIVE_TEST(ImplicitTargetAllCheckWithEvidenceOutOFTarget) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->Y->E->F->G;W->E<-Z;X->E");
      auto ie = gum::ShaferShenoyInference(&bn);
      ie.addEvidence("E", 1);
      ie.addJointTarget(bn.nodeset({"B", "Y", "F"}));

      auto p = gum::Potential< double >();
      for (const auto n: bn.nodes())
        p *= bn.cpt(n);
      gum::Potential evY1 = gum::Potential< double >();
      evY1.add(bn.variableFromName("E"));
      evY1.fillWith({0, 1});
      p *= evY1;
      p.normalize();

      // target
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"B", "Y", "F"})),
                             p.margSumIn(bn.variables({"B", "Y", "F"})),
                             TS_GUM_SMALL_ERROR)
      // subtargets
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"B", "Y"})),
                             p.margSumIn(bn.variables({"B", "Y"})),
                             TS_GUM_SMALL_ERROR)
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"F", "Y"})),
                             p.margSumIn(bn.variables({"F", "Y"})),
                             TS_GUM_SMALL_ERROR)
      // implicit target
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"W", "Z", "X"})),
                             p.margSumIn(bn.variables({"W", "Z", "X"})),
                             TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(ImplicitTargetAllCheckWithEvidenceInTarget) {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C->Y->E->F->G;W->E<-Z;X->E");
      auto ie = gum::ShaferShenoyInference(&bn);
      ie.addEvidence("Y", 1);
      ie.addJointTarget(bn.nodeset({"B", "Y", "F"}));

      auto p = gum::Potential< double >();
      for (const auto n: bn.nodes())
        p *= bn.cpt(n);
      gum::Potential evY1 = gum::Potential< double >();
      evY1.add(bn.variableFromName("Y"));
      evY1.fillWith({0, 1});
      p *= evY1;
      p.normalize();

      // target
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"B", "Y", "F"})),
                             p.margSumIn(bn.variables({"B", "Y", "F"})),
                             TS_GUM_SMALL_ERROR)
      // subtargets
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"B", "Y"})),
                             p.margSumIn(bn.variables({"B", "Y"})),
                             TS_GUM_SMALL_ERROR)
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"F", "Y"})),
                             p.margSumIn(bn.variables({"F", "Y"})),
                             TS_GUM_SMALL_ERROR)
      // implicit target
      TS_GUM_POTENTIAL_DELTA(ie.jointPosterior(bn.nodeset({"W", "Z", "X"})),
                             p.margSumIn(bn.variables({"W", "Z", "X"})),
                             TS_GUM_SMALL_ERROR)
    }

    GUM_ACTIVE_TEST(IrrelevantSoftEvidence) {
      auto                     bn = gum::BayesNet< double >::fastPrototype("A->B<-C");
      gum::Potential< double > psoft;
      gum::Potential< double > phard;
      const auto bn_joint = this->joint(bn);
      gum::Potential< double > jointhard, jointsoft;
      {
        auto hardev = gum::Potential< double >();
        hardev.add(bn.variable("A"));
        hardev.fillWith({0, 1});
        jointhard = bn_joint * hardev;

        gum::ShaferShenoyInference ie(&bn);
        ie.addEvidence(hardev);
        ie.addTarget("C");
        TS_GUM_ASSERT_THROWS_NOTHING(ie.makeInference());
        phard = gum::Potential(ie.posterior("C"));
      }
      {
        auto softev = gum::Potential< double >();
        softev.add(bn.variable("A"));
        softev.fillWith({0.5, 1});
        jointsoft = bn_joint * softev;

        gum::ShaferShenoyInference ie(&bn);
        ie.addEvidence(softev);
        ie.addTarget("C");
        TS_GUM_ASSERT_THROWS_NOTHING(ie.makeInference());
        psoft = gum::Potential(ie.posterior("C"));
      }
      TS_GUM_POTENTIAL_DELTA(phard, psoft, TS_GUM_VERY_SMALL_ERROR)
      TS_ASSERT(equalPotentials(phard, jointhard.margSumIn({&bn.variableFromName("C")}).normalize()))
      TS_ASSERT(equalPotentials(psoft, jointsoft.margSumIn({&bn.variableFromName("C")}).normalize()))
    }

    GUM_ACTIVE_TEST(NonRegressionBugJoinTarget) {
      auto bn = gum::BayesNet< double >::fastPrototype(
         "X02->X03->X11<-X08->X09->X10<-X07<-X05->X09<-X04<-X00;X03->X04;X01->X10;X05->X06");
      auto ie=gum::ShaferShenoyInference(&bn);
      ie.addJointTarget(bn.nodeset({"X04", "X11"}));
      ie.addJointTarget(bn.nodeset({"X00", "X01", "X06"}));
      const auto bn_joint = this->joint(bn);

      try {
        ie.makeInference();
        gum::Set< const gum::DiscreteVariable* > set{&bn.variableFromName("X04"), &bn.variable("X11")};
        TS_ASSERT(equalPotentials(ie.jointPosterior(bn.nodeset({"X04", "X11"})), bn_joint.margSumIn(set).normalize()))
        set = {&bn.variableFromName("X00"), &bn.variable("X01"), &bn.variable("X06")};
        TS_ASSERT(equalPotentials(ie.jointPosterior(bn.nodeset({"X00", "X01", "X06"})), bn_joint.margSumIn(set).normalize()))
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
    }
    
    private:
    void randomInitP(const gum::Potential< double >& tt) {
      auto&              t = const_cast< gum::Potential< double >& >(tt);
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, 1.0f + rand() * 50.0f / float(RAND_MAX));
    }

    // Builds a BN to tst the inference
    void fill(gum::BayesNet< double >& bn) {
      bn.cpt(i1).fillWith({0.2f, 0.8f});
      bn.cpt(i2).fillWith({0.3f, 0.7f});
      bn.cpt(i3).fillWith({0.1f, 0.9f, 0.9f, 0.1f});
      bn.cpt(i4).fillWith(   // clang-format off
              {0.4f, 0.6f,
               0.5f, 0.5f,
               0.5f, 0.5f,
               1.0f, 0.0f} );  // clang-format
                                                               // on
      bn.cpt( i5 ).fillWith(  // clang-format off
              {0.3f, 0.6f, 0.1f,
               0.5f, 0.5f, 0.0f,
               0.5f, 0.5f, 0.0f,
               1.0f, 0.0f, 0.0f,
               0.4f, 0.6f, 0.0f,
               0.5f, 0.5f, 0.0f,
               0.5f, 0.5f, 0.0f,
               0.0f, 0.0f, 1.0f} );   // clang-format on
    }

    // Uncomment this to have some outputs.
    void printProba(const gum::Potential< double >&) {
      // for (inst.setFirst(); !inst.end(); ++inst)
      //{
      //  std::cerr << inst<<" : " <<p[inst] << std::endl;
      //}
      // std::cerr << std::endl;
    }
  };
}   // namespace gum_tests
