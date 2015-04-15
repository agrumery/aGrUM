/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/VEWithBB.h>

#include <agrum/PRM/instanceBayesNet.h>
#include <agrum/PRM/classBayesNet.h>
#include <agrum/PRM/inference/groundedInference.h>
#include <agrum/PRM/inference/SVE.h>
#include <agrum/PRM/inference/SVED.h>
#include <agrum/PRM/inference/structuredBayesBall.h>
#include <agrum/PRM/inference/structuredInference.h>

#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {

  class PRMInferenceTestSuite : public CxxTest::TestSuite {
    private:
    gum::prm::PRM<double> *prm;
    gum::prm::PRM<double> *small;

    public:
    void setUp() {
      {
        gum::prm::o3prm::O3prmReader<double> reader;
        reader.readFile(GET_PATH_STR("o3prm/inference.o3prm"));
        prm = reader.prm();
      }
      {
        gum::prm::o3prm::O3prmReader<double> reader;
        reader.readFile(GET_PATH_STR("o3prm/printers_systems.o3prm"));
        small = reader.prm();
      }
    }

    void tearDown() {
      delete prm;
      delete small;
    }

    void testEvidence() {
      gum::prm::GroundedInference<double> *g_ve = nullptr;
      gum::VariableElimination<double> *ve = nullptr;
      gum::BayesNet<double> bn;
      gum::BayesNetFactory<double> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->system("aSys").groundedBN(bn_factory));
      TS_GUM_ASSERT_THROWS_NOTHING(ve = new gum::VariableElimination<double>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(
          g_ve = new gum::prm::GroundedInference<double>(*prm, prm->system("aSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      const gum::prm::Instance<double> &instance = prm->system("aSys").get("c1");
      const gum::prm::Attribute<double> &attribute = instance.get("can_print");
      gum::Potential<double> e;
      e.add(attribute.type().variable());
      e.fill((double)0.0);
      gum::Instantiation inst(e);
      inst.setFirst();
      e.set(inst, (double)1.0);
      gum::prm::PRMInference<double>::Chain chain =
          std::make_pair(&instance, &attribute);
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
      TS_ASSERT(g_ve->hasEvidence(chain));
      delete g_ve;
    }

    void testSmallGrdInference() {
      // Creating the inference engine
      gum::prm::GroundedInference<double> *g_ve = 0;
      gum::VariableElimination<double> *ve = 0;
      gum::BayesNet<double> bn;
      gum::BayesNetFactory<double> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(small->system("microSys").groundedBN(bn_factory));
      TS_GUM_ASSERT_THROWS_NOTHING(ve = new gum::VariableElimination<double>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new gum::prm::GroundedInference<double>(
                                       *small, small->system("microSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      // Building query
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("c");
        const gum::prm::Attribute<double> &attribute = instance.get("can_print");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.600832, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.399168, 1e-6);
      }
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("p");
        const gum::prm::Attribute<double> &attribute = instance.get("equipState");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.49896, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.50104, 1e-6);
      }
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("pow");
        const gum::prm::Attribute<double> &attribute = instance.get("powState");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.99, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.01, 1e-6);
      }
      delete g_ve;
    }

    void testSmallSVEInference() {
      gum::prm::SVE<double> *sve = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
          sve = new gum::prm::SVE<double>(*small, small->system("microSys")));
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("c");
        const gum::prm::Attribute<double> &attribute = instance.get("can_print");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sve->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.600832, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.399168, 1e-6);
      }
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("p");
        const gum::prm::Attribute<double> &attribute = instance.get("equipState");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sve->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.49896, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.50104, 1e-6);
      }
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("pow");
        const gum::prm::Attribute<double> &attribute = instance.get("powState");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sve->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.99, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.01, 1e-6);
      }
      delete sve;
    }

    void testSmallSVEDInference() {
      gum::prm::SVED<double> *sved = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
          sved = new gum::prm::SVED<double>(*small, small->system("microSys")));
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("c");
        const gum::prm::Attribute<double> &attribute = instance.get("can_print");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sved->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.600832, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.399168, 1e-6);
      }
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("p");
        const gum::prm::Attribute<double> &attribute = instance.get("equipState");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sved->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.49896, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.50104, 1e-6);
      }
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("pow");
        const gum::prm::Attribute<double> &attribute = instance.get("powState");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sved->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.99, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.01, 1e-6);
      }
      delete sved;
    }

    void testSmallStructInference() {
      gum::prm::StructuredInference<double> *inf = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inf = new gum::prm::StructuredInference<double>(
                                       *small, small->system("microSys")));
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("c");
        const gum::prm::Attribute<double> &attribute = instance.get("can_print");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(inf->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.600832, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.399168, 1e-6);
      }
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("p");
        const gum::prm::Attribute<double> &attribute = instance.get("equipState");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(inf->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.49896, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.50104, 1e-6);
      }
      {
        const gum::prm::Instance<double> &instance =
            small->system("microSys").get("pow");
        const gum::prm::Attribute<double> &attribute = instance.get("powState");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(inf->marginal(chain, m));
        gum::Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.99, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.01, 1e-6);
      }
      delete inf;
    }

    void testInference() {
      // GUM_TRACE_VAR(UINT_MAX);
      gum::prm::GroundedInference<double> *g_ve = 0;
      gum::prm::GroundedInference<double> *g_ss = 0;
      gum::prm::GroundedInference<double> *g_vebb = 0;
      gum::VariableElimination<double> *ve = 0;
      gum::ShaferShenoyInference<double> *ss = 0;
      gum::VEWithBB<double> *vebb = 0;
      gum::BayesNet<double> bn;
      gum::BayesNetFactory<double> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->system("aSys").groundedBN(bn_factory));
      TS_GUM_ASSERT_THROWS_NOTHING(ve = new gum::VariableElimination<double>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(ss = new gum::ShaferShenoyInference<double>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(vebb = new gum::VEWithBB<double>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(
          g_ve = new gum::prm::GroundedInference<double>(*prm, prm->system("aSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      TS_GUM_ASSERT_THROWS_NOTHING(
          g_ss = new gum::prm::GroundedInference<double>(*prm, prm->system("aSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ss->setBNInference(ss));
      TS_GUM_ASSERT_THROWS_NOTHING(g_vebb = new gum::prm::GroundedInference<double>(
                                       *prm, prm->system("aSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_vebb->setBNInference(vebb));

      for (const auto node : bn.nodes()) {
        gum::Potential<double> m_ve, m_ss, m_sve, m_sved, m_vebb, m_struct;

        try {
          size_t pos = bn.variableNodeMap().name(node).find_first_of('.');
          const gum::prm::Instance<double> &instance = prm->system("aSys").get(
              bn.variableNodeMap().name(node).substr(0, pos));
          const gum::prm::Attribute<double> &attribute =
              instance.get(bn.variableNodeMap().name(node).substr(pos + 1));
          gum::prm::PRMInference<double>::Chain chain =
              std::make_pair(&instance, &attribute);
          std::string dot = ".";
          g_ve->marginal(chain, m_ve);
          // GUM_TRACE("VE done");
          g_ss->marginal(chain, m_ss);
          // GUM_TRACE("SS done");
          g_vebb->marginal(chain, m_vebb);
          // GUM_TRACE("gum::VEWithBB done");
          gum::prm::SVE<double> sve(*prm, prm->system("aSys"));
          sve.marginal(chain, m_sve);
          // GUM_TRACE("SVE<double> done");
          gum::prm::SVED<double> sved(*prm, prm->system("aSys"));
          sved.marginal(chain, m_sved);
          // GUM_TRACE("SVED<double> done");
          gum::prm::StructuredInference<double> structinf(*prm, prm->system("aSys"));
          structinf.marginal(chain, m_struct);
          // GUM_TRACE("StructInf done");
          // We need two instantiations, one for the grounded potentials and one
          // for the PRM<double>-level ones
          gum::Instantiation inst(m_ve), jnst(m_sve);
          std::string foo = instance.name() + dot + attribute.safeName();

          for (inst.setFirst(), jnst.setFirst(); not(inst.end() or jnst.end());
               inst.inc(), jnst.inc()) {
            TS_ASSERT_EQUALS(m_ve.nbrDim(), m_ss.nbrDim());
            TS_ASSERT_EQUALS(m_ve.nbrDim(), m_sve.nbrDim());
            TS_ASSERT_EQUALS(m_ve.nbrDim(), m_sved.nbrDim());
            TS_ASSERT_EQUALS(m_ve.nbrDim(), m_vebb.nbrDim());
            TS_ASSERT_EQUALS(m_ve.nbrDim(), m_struct.nbrDim());
            TS_ASSERT_EQUALS(m_ve.domainSize(), m_ss.domainSize());
            TS_ASSERT_EQUALS(m_ve.domainSize(), m_sve.domainSize());
            TS_ASSERT_EQUALS(m_ve.domainSize(), m_sved.domainSize());
            TS_ASSERT_EQUALS(m_ve.domainSize(), m_vebb.domainSize());
            TS_ASSERT_EQUALS(m_ve.domainSize(), m_struct.domainSize());
            TS_ASSERT_DELTA(m_ve.get(inst), m_ss.get(inst), 1.0e-3);
            TS_ASSERT_DELTA(m_sve.get(jnst), m_ss.get(inst), 1.0e-3);
            TS_ASSERT_DELTA(m_vebb.get(inst), m_ss.get(inst), 1.0e-3);
            TS_ASSERT_DELTA(m_sved.get(jnst), m_sve.get(jnst), 1.0e-3);
            TS_ASSERT_DELTA(m_sved.get(jnst), m_struct.get(jnst), 1.0e-3);
          }

          bool zero = true;

          for (jnst.setFirst(); not jnst.end(); jnst.inc()) {
            if (m_sved.get(jnst) != (double)0.0) {
              zero = false;
              break;
            }
          }

          if (zero) {
            GUM_TRACE(foo);
          }
        } catch (gum::Exception &e) {
          GUM_TRACE_VAR(bn.variableNodeMap().name(node));
          TS_GUM_ASSERT_THROWS_NOTHING(throw e);
          break;
        }

#ifndef NDEBUG
        // In debug mode we do it for just one node
        break;
#endif
      }

      delete g_ve;
      delete g_ss;
      delete g_vebb;
    }

    void testStructuredBB() {
      gum::prm::SVE<double> sve(*prm, prm->system("aSys"));
      gum::prm::StructuredBayesBall<double> *bb = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
          bb = new gum::prm::StructuredBayesBall<double>(sve));
      gum::prm::Instance<double> &i = prm->system("aSys").get("pow");
      TS_GUM_ASSERT_THROWS_NOTHING(bb->compute(i, i.get("(t_state)state").id()));
      TS_GUM_ASSERT_THROWS_NOTHING(delete bb);
    }

    void testEvidenceSioux() {
      gum::prm::GroundedInference<double> *g_ve = 0;
      gum::VariableElimination<double> *ve = 0;
      gum::BayesNet<double> bn;
      gum::BayesNetFactory<double> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->system("aSys").groundedBN(bn_factory));
      TS_GUM_ASSERT_THROWS_NOTHING(ve = new gum::VariableElimination<double>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(
          g_ve = new gum::prm::GroundedInference<double>(*prm, prm->system("aSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      // Adding evidence
      {
        const gum::prm::Instance<double> &instance =
            prm->system("aSys").get("bw_printers[0]");
        const gum::prm::Attribute<double> &attribute =
            instance.get("(t_degraded)equipState");
        gum::Potential<double> e;
        e.add(attribute.type().variable());
        e.fill((double)0.0);
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (double)1.0);
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      {
        const gum::prm::Instance<double> &instance = prm->system("aSys").get("c1");
        const gum::prm::Attribute<double> &attribute = instance.get("can_print");
        gum::Potential<double> e;
        e.add(attribute.type().variable());
        e.fill((double)0.0);
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (double)1.0);
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      // making a first inference with evidence
      {
        const gum::prm::Instance<double> &q_i = prm->system("aSys").get("pow");
        const gum::prm::Attribute<double> &q_a = q_i.get("state");
        gum::prm::PRMInference<double>::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Potential<double> m;
        g_ve->marginal(q_chain, m);
        gum::Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 1.0, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.0, 1e-6);
      }
      // Removing evidences
      {
        const gum::prm::Instance<double> &instance = prm->system("aSys").get("c1");
        const gum::prm::Attribute<double> &attribute = instance.get("can_print");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      {
        const gum::prm::Instance<double> &instance =
            prm->system("aSys").get("bw_printers[0]");
        const gum::prm::Attribute<double> &attribute =
            instance.get("(t_degraded)equipState");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      // Inference without evidences
      {
        const gum::prm::Instance<double> &q_i = prm->system("aSys").get("pow");
        const gum::prm::Attribute<double> &q_a = q_i.get("state");
        gum::prm::PRMInference<double>::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Potential<double> m;
        g_ve->marginal(q_chain, m);
        gum::Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
      }
      delete g_ve;
    }

    void testEvidenceSioux2() {
      gum::prm::SVE<double> *g_ve = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
          g_ve = new gum::prm::SVE<double>(*prm, prm->system("aSys")));
      // Adding evidence
      {
        const gum::prm::Instance<double> &instance =
            prm->system("aSys").get("bw_printers[0]");
        const gum::prm::Attribute<double> &attribute =
            instance.get("(t_degraded)equipState");
        gum::Potential<double> e;
        e.add(attribute.type().variable());
        e.fill((double)0.0);
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (double)1.0);
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      {
        const gum::prm::Instance<double> &instance = prm->system("aSys").get("c1");
        const gum::prm::Attribute<double> &attribute = instance.get("can_print");
        gum::Potential<double> e;
        e.add(attribute.type().variable());
        e.fill((double)0.0);
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (double)1.0);
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      // making a first inference with evidence
      {
        const gum::prm::Instance<double> &q_i = prm->system("aSys").get("pow");
        const gum::prm::Attribute<double> &q_a = q_i.get("state");
        gum::prm::PRMInference<double>::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Potential<double> m;
        g_ve->marginal(q_chain, m);
        gum::Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 1.0, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.0, 1e-6);
      }
      // Removing evidences
      {
        const gum::prm::Instance<double> &instance = prm->system("aSys").get("c1");
        const gum::prm::Attribute<double> &attribute = instance.get("can_print");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      {
        const gum::prm::Instance<double> &instance =
            prm->system("aSys").get("bw_printers[0]");
        const gum::prm::Attribute<double> &attribute =
            instance.get("(t_degraded)equipState");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      // Inference without evidences
      {
        const gum::prm::Instance<double> &q_i = prm->system("aSys").get("pow");
        const gum::prm::Attribute<double> &q_a = q_i.get("state");
        gum::prm::PRMInference<double>::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Potential<double> m;
        g_ve->marginal(q_chain, m);
        gum::Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
      }
      delete g_ve;
    }

    void testEvidenceSioux3withSVE() {
      gum::prm::PRMInference<double> *g_ve = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          g_ve = new gum::prm::SVE<double>(*prm, prm->system("aSys")));
      __testEvidencenSioux3(g_ve);
      delete g_ve;
    }

    void /*test*/ EvidenceSioux3withSVED() {
      gum::prm::PRMInference<double> *g_ve = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          g_ve = new gum::prm::SVED<double>(*prm, prm->system("aSys")));
      __testEvidencenSioux3(g_ve);
      delete g_ve;
    }

    private:
    void __testEvidencenSioux3(gum::prm::PRMInference<double> *g_ve) {
      // Adding evidence
      {
        const gum::prm::Instance<double> &instance =
            prm->system("aSys").get("bw_printers[0]");
        const gum::prm::Attribute<double> &attribute =
            instance.get("(t_degraded)equipState");
        gum::Potential<double> e;
        e.add(attribute.type().variable());
        e.fill((double)0.0);
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (double)1.0);
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      {
        const gum::prm::Instance<double> &instance = prm->system("aSys").get("c1");
        const gum::prm::Attribute<double> &attribute = instance.get("can_print");
        gum::Potential<double> e;
        e.add(attribute.type().variable());
        e.fill((double)0.0);
        gum::Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (double)1.0);
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      // making a first inference with evidence
      {
        const gum::prm::Instance<double> &q_i = prm->system("aSys").get("pow");
        const gum::prm::Attribute<double> &q_a = q_i.get("state");
        gum::prm::PRMInference<double>::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Potential<double> m;
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->marginal(q_chain, m));
        gum::Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 1.0, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.0, 1e-6);
      }
      // Removing evidences
      {
        const gum::prm::Instance<double> &instance = prm->system("aSys").get("c1");
        const gum::prm::Attribute<double> &attribute = instance.get("can_print");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      {
        const gum::prm::Instance<double> &instance =
            prm->system("aSys").get("bw_printers[0]");
        const gum::prm::Attribute<double> &attribute =
            instance.get("(t_degraded)equipState");
        gum::prm::PRMInference<double>::Chain chain =
            std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      // Inference without evidences
      {
        const gum::prm::Instance<double> &q_i = prm->system("aSys").get("pow");
        const gum::prm::Attribute<double> &q_a = q_i.get("state");
        gum::prm::PRMInference<double>::Chain q_chain = std::make_pair(&q_i, &q_a);
        gum::Potential<double> m;
        g_ve->marginal(q_chain, m);
        gum::Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
      }
    }
  };

} // namespace gum_tests
