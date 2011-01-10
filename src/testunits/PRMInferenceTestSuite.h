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
// ============================================================================
#include <cxxtest/AgrumTestSuite.h>
// ============================================================================
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/VEWithBB.h>
// ============================================================================
#include <agrum/prm/instanceBayesNet.h>
#include <agrum/prm/classBayesNet.h>
#include <agrum/prm/groundedInference.h>
#include <agrum/prm/SVE.h>
#include <agrum/prm/SVED.h>
#include <agrum/prm/structuredBayesBall.h>
#include <agrum/prm/structuredInference.h>
// ============================================================================
#include <agrum/prm/skool/SkoolReader.h>
// ============================================================================

namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;
using namespace gum::prm::skool;

class PRMInferenceTestSuite: public CxxTest::TestSuite {
  private:
    PRM* prm;
    PRM* small;

  public:
    void setUp() {
      {
        SkoolReader reader;
        reader.readFile("../../../src/testunits/ressources/skool/inference.skool");
        prm = reader.prm();
      }
      {
        SkoolReader reader;
        reader.readFile("../../../src/testunits/ressources/skool/printers_systems.skool");
        small = reader.prm();
      }
    }

    void tearDown() {
      delete prm;
      delete small;
    }

    void testCreation() {
      ClassBayesNet* c = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(c = new ClassBayesNet(prm->getClass("SafeComputer")));
      TS_GUM_ASSERT_THROWS_NOTHING(delete c);
      InstanceBayesNet* inst = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inst = new InstanceBayesNet(prm->getSystem("aSys").get("c1")));
      TS_GUM_ASSERT_THROWS_NOTHING(delete inst);
    }

    void testClassAccess() {
      Class& c = prm->getClass("SafeComputer");
      ClassBayesNet* bn = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(bn = new ClassBayesNet(prm->getClass("SafeComputer")));
      Size elts = c.attributes().size() + c.aggregates().size();
      TS_ASSERT_EQUALS(bn->size(), elts);
      for (Set<Attribute*>::iterator attr = c.attributes().begin(); attr != c.attributes().end(); ++attr) {
        NodeId id = 0;
        TS_GUM_ASSERT_THROWS_NOTHING((**attr).cpf());
        TS_GUM_ASSERT_THROWS_NOTHING(id = bn->idFromName((**attr).safeName()));
        TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id));
        TS_ASSERT_EQUALS((**attr).cpf().nbrDim(), bn->cpt(id).nbrDim());
        TS_ASSERT_EQUALS((**attr).cpf().domainSize(), bn->cpt(id).domainSize());
      }
      TS_ASSERT(bn->modalities().size() > 0);
      TS_GUM_ASSERT_THROWS_NOTHING(delete bn);
    }

    void testInstanceAccess() {
      InstanceBayesNet* bn = 0;
      Instance& i = prm->getSystem("aSys").get("c1");
      TS_GUM_ASSERT_THROWS_NOTHING(bn = new InstanceBayesNet(i));
      TS_ASSERT_EQUALS(bn->size(), i.size());
      for (Instance::iterator attr = i.begin(); attr != i.end(); ++attr) {
        NodeId id = 0;
        TS_GUM_ASSERT_THROWS_NOTHING((**attr).cpf());
        TS_GUM_ASSERT_THROWS_NOTHING(id = bn->idFromName((**attr).safeName()));
        TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id));
        TS_ASSERT_EQUALS((**attr).cpf().nbrDim(), bn->cpt(id).nbrDim());
        TS_ASSERT_EQUALS((**attr).cpf().domainSize(), bn->cpt(id).domainSize());
      }
      TS_ASSERT(bn->modalities().size() > 0);
      TS_GUM_ASSERT_THROWS_NOTHING(delete bn);
    }

    void testGroundedBN() {
      System& sys = prm->getSystem("aSys");
      BayesNet<prm_float> bn;
      BayesNetFactory<prm_float> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(sys.groundedBN(bn_factory));
      int count = 0;
      for (System::iterator iter = sys.begin(); iter != sys.end(); ++iter) {
        for (Instance::iterator jter = (**iter).begin(); jter != (**iter).end(); ++jter) {
          ++count;
        }
      }
      int wount = 0;
      for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
        wount++;
        std::string var = bn.variable(*node).name();
        size_t pos = var.find_first_of('.');
        Instance& instance = sys.get(var.substr(0, pos));
        Attribute& attr = instance.get(var.substr(pos +1));
        TS_ASSERT_DIFFERS(bn.cpt(*node).nbrDim(), (Size)0);
        if (ClassElement::isAggregate(instance.type().get(attr.id()))) {
          TS_ASSERT_DIFFERS(attr.cpf().nbrDim(), (Size)1);
        }
      }
      TS_ASSERT_EQUALS(count, wount);
      for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
        const DiscreteVariable* var = &(bn.variable(*node));
        for (DAG::NodeIterator mode = bn.dag().beginNodes(); mode != bn.dag().endNodes(); ++mode) {
          if ((*node) != (*mode)) {
            TS_ASSERT_DIFFERS(var, &(bn.variable(*mode)));
          }
        }
      }
    }

    void testCPF() {
      System& sys = prm->getSystem("aSys");
      for (System::iterator iter = sys.begin(); iter != sys.end(); ++iter) {
        for (Instance::iterator jter = (**iter).begin(); jter != (**iter).end(); ++jter) {
          Instantiation i((**jter).cpf()), var;
          var.add((**jter).type().variable());
          for (i.setFirstOut(var); not i.end(); i.incOut(var)) {
            prm_float f = 0.0;
            for (i.setFirstIn(var); not i.end(); i.incIn(var)) {
              f += (**jter).cpf().get(i);
            }
            TS_ASSERT_DELTA(f, 1.0, 1e-9);
            i.unsetOverflow();
          }
        }
      }
    }

    void testNormalisedCPT() {
      System& sys = prm->getSystem("aSys");
      BayesNet<prm_float> bn;
      BayesNetFactory<prm_float> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(sys.groundedBN(bn_factory));
      for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
        const Potential<prm_float>& cpt = bn.cpt(*node);
        Instantiation i(cpt), j;
        j.add(bn.variable(*node));
        for (i.setFirstOut(j); not i.end(); i.incOut(j)) {
          prm_float sum = 0.0;
          for (i.setFirstIn(j); not i.end(); i.incIn(j))
            sum += cpt.get(i);
          TS_ASSERT_DELTA(sum, 1.0, 1e-9);
          i.unsetOverflow();
        }
      }
    }

    void testEvidence() {
      GroundedInference* g_ve = 0;
      VariableElimination<prm_float>* ve = 0;
      BayesNet<prm_float> bn;
      BayesNetFactory<prm_float> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getSystem("aSys").groundedBN(bn_factory));
      TS_GUM_ASSERT_THROWS_NOTHING(ve = new VariableElimination<prm_float>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new GroundedInference(*prm, prm->getSystem("aSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      const Instance& instance = prm->getSystem("aSys").get("c1");
      const Attribute& attribute = instance.get("can_print");
      Potential<prm_float> e;
      e.add(attribute.type().variable());
      e.fill( (prm_float) 0.0);
      Instantiation inst(e);
      inst.setFirst();
      e.set(inst, (prm_float) 1.0);
      PRMInference::Chain chain = std::make_pair(&instance, &attribute);
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
      TS_ASSERT(g_ve->hasEvidence(chain));
      delete g_ve;
    }

    void testSmallGrdInference() {
      // Creating the inference engine
      GroundedInference* g_ve = 0;
      VariableElimination<prm_float>* ve = 0;
      BayesNet<prm_float> bn;
      BayesNetFactory<prm_float> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(small->getSystem("microSys").groundedBN(bn_factory));
      TS_GUM_ASSERT_THROWS_NOTHING(ve = new VariableElimination<prm_float>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new GroundedInference(*small, small->getSystem("microSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      // Building query
      {
        const Instance& instance = small->getSystem("microSys").get("c");
        const Attribute& attribute = instance.get("can_print");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.600832, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.399168, 1e-6);
      }
      {
        const Instance& instance = small->getSystem("microSys").get("p");
        const Attribute& attribute = instance.get("equipState");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.49896, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.50104, 1e-6);
      }
      {
        const Instance& instance = small->getSystem("microSys").get("pow");
        const Attribute& attribute = instance.get("powState");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.99, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.01, 1e-6);
      }
      delete g_ve;
    }

    void testSmallSVEInference() {
      SVE* sve = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(sve = new SVE(*small, small->getSystem("microSys")));
      {
        const Instance& instance = small->getSystem("microSys").get("c");
        const Attribute& attribute = instance.get("can_print");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sve->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.600832, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.399168, 1e-6);
      }
      {
        const Instance& instance = small->getSystem("microSys").get("p");
        const Attribute& attribute = instance.get("equipState");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sve->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.49896, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.50104, 1e-6);
      }
      {
        const Instance& instance = small->getSystem("microSys").get("pow");
        const Attribute& attribute = instance.get("powState");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sve->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.99, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.01, 1e-6);
      }
      delete sve;
    }

    void testSmallSVEDInference() {
      SVED* sved = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(sved = new SVED(*small, small->getSystem("microSys")));
      {
        const Instance& instance = small->getSystem("microSys").get("c");
        const Attribute& attribute = instance.get("can_print");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sved->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.600832, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.399168, 1e-6);
      }
      {
        const Instance& instance = small->getSystem("microSys").get("p");
        const Attribute& attribute = instance.get("equipState");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sved->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.49896, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.50104, 1e-6);
      }
      {
        const Instance& instance = small->getSystem("microSys").get("pow");
        const Attribute& attribute = instance.get("powState");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(sved->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.99, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.01, 1e-6);
      }
      delete sved;
    }

    void testSmallStructInference() {
      StructuredInference* inf = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(inf = new StructuredInference(*small, small->getSystem("microSys")));
      {
        const Instance& instance = small->getSystem("microSys").get("c");
        const Attribute& attribute = instance.get("can_print");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(inf->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.600832, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.399168, 1e-6);
      }
      {
        const Instance& instance = small->getSystem("microSys").get("p");
        const Attribute& attribute = instance.get("equipState");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(inf->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.49896, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.50104, 1e-6);
      }
      {
        const Instance& instance = small->getSystem("microSys").get("pow");
        const Attribute& attribute = instance.get("powState");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(inf->marginal(chain, m));
        Instantiation i(m);
        i.setFirst();
        TS_ASSERT_DELTA(m.get(i), 0.99, 1e-6);
        i.inc();
        TS_ASSERT_DELTA(m.get(i), 0.01, 1e-6);
      }
      delete inf;
    }

    void testInference() {
      //GUM_TRACE_VAR(UINT_MAX);
      GroundedInference* g_ve = 0;
      GroundedInference* g_ss = 0;
      GroundedInference* g_vebb = 0;
      VariableElimination<prm_float>* ve = 0;
      ShaferShenoyInference<prm_float>* ss = 0;
      VEWithBB<prm_float>* vebb = 0;
      BayesNet<prm_float> bn;
      BayesNetFactory<prm_float> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getSystem("aSys").groundedBN(bn_factory));
      TS_GUM_ASSERT_THROWS_NOTHING(ve = new VariableElimination<prm_float>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(ss = new ShaferShenoyInference<prm_float>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(vebb = new VEWithBB<prm_float>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new GroundedInference(*prm, prm->getSystem("aSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ss = new GroundedInference(*prm, prm->getSystem("aSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ss->setBNInference(ss));
      TS_GUM_ASSERT_THROWS_NOTHING(g_vebb = new GroundedInference(*prm, prm->getSystem("aSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_vebb->setBNInference(vebb));
      for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
        Potential<prm_float> m_ve, m_ss, m_sve, m_sved, m_vebb, m_struct;
        try {
          size_t pos = bn.variableNodeMap().name(*node).find_first_of('.');
          const Instance& instance = prm->getSystem("aSys").get(bn.variableNodeMap().name(*node).substr(0, pos));
          const Attribute& attribute = instance.get(bn.variableNodeMap().name(*node).substr(pos+1));
          PRMInference::Chain chain = std::make_pair(&instance, &attribute);
          std::string dot = ".";
          g_ve->marginal(chain, m_ve);
          // GUM_TRACE("VE done");
          g_ss->marginal(chain, m_ss);
          // GUM_TRACE("SS done");
          g_vebb->marginal(chain, m_vebb);
          // GUM_TRACE("VEWithBB done");
          SVE sve(*prm, prm->getSystem("aSys"));
          sve.marginal(chain, m_sve);
          // GUM_TRACE("SVE done");
          SVED sved(*prm, prm->getSystem("aSys"));
          sved.marginal(chain, m_sved);
          // GUM_TRACE("SVED done");
          StructuredInference structinf(*prm, prm->getSystem("aSys"));
          structinf.marginal(chain, m_struct);
          // GUM_TRACE("StructInf done");
          // We need two instantiations, one for the grounded potentials and one
          // for the PRM-level ones
          Instantiation inst(m_ve), jnst(m_sve);
          std::string foo = instance.name() + dot + attribute.safeName();
          for (inst.setFirst(), jnst.setFirst(); not (inst.end() or jnst.end()); inst.inc(), jnst.inc())
          {
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
            if (m_sved.get(jnst) != (prm_float) 0.0) {
              zero = false;
              break;
            }
          }
          if (zero) {
            GUM_TRACE(foo);
          }
        } catch (Exception& e) {
          GUM_TRACE_VAR(bn.variableNodeMap().name(*node));
          TS_GUM_ASSERT_THROWS_NOTHING(throw e);
          break;
        }
      }
      delete g_ve;
      delete g_ss;
      delete g_vebb;
    }

    void testStructuredBB() {
      SVE sve(*prm, prm->getSystem("aSys"));
      StructuredBayesBall* bb = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(bb = new StructuredBayesBall(sve));
      Instance& i = prm->getSystem("aSys").get("pow");
      TS_GUM_ASSERT_THROWS_NOTHING(bb->compute(i, i.get("(t_state)state").id()));
      TS_GUM_ASSERT_THROWS_NOTHING(delete bb);
    }

    void testEvidenceSioux() {
      GroundedInference* g_ve = 0;
      VariableElimination<prm_float>* ve = 0;
      BayesNet<prm_float> bn;
      BayesNetFactory<prm_float> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getSystem("aSys").groundedBN(bn_factory));
      TS_GUM_ASSERT_THROWS_NOTHING(ve = new VariableElimination<prm_float>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new GroundedInference(*prm, prm->getSystem("aSys")));
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
      // Adding evidence
      {
        const Instance& instance = prm->getSystem("aSys").get("bw_printers[0]");
        const Attribute& attribute = instance.get("(t_degraded)equipState");
        Potential<prm_float> e;
        e.add(attribute.type().variable());
        e.fill( (prm_float) 0.0);
        Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (prm_float) 1.0);
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      {
        const Instance& instance = prm->getSystem("aSys").get("c1");
        const Attribute& attribute = instance.get("can_print");
        Potential<prm_float> e;
        e.add(attribute.type().variable());
        e.fill( (prm_float) 0.0);
        Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (prm_float) 1.0);
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      // making a first inference with evidence
      {
        const Instance& q_i = prm->getSystem("aSys").get("pow");
        const Attribute& q_a = q_i.get("state");
        PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
        Potential<prm_float> m;
        g_ve->marginal(q_chain, m);
        Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 1.0, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.0, 1e-6);
      }
      // Removing evidences
      {
        const Instance& instance = prm->getSystem("aSys").get("c1");
        const Attribute& attribute = instance.get("can_print");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      {
        const Instance& instance = prm->getSystem("aSys").get("bw_printers[0]");
        const Attribute& attribute = instance.get("(t_degraded)equipState");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      // Inference without evidences
      {
        const Instance& q_i = prm->getSystem("aSys").get("pow");
        const Attribute& q_a = q_i.get("state");
        PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
        Potential<prm_float> m;
        g_ve->marginal(q_chain, m);
        Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
      }
      delete g_ve;
    }

    void testEvidenceSioux2() {
      SVE* g_ve = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new SVE(*prm, prm->getSystem("aSys")));
      // Adding evidence
      {
        const Instance& instance = prm->getSystem("aSys").get("bw_printers[0]");
        const Attribute& attribute = instance.get("(t_degraded)equipState");
        Potential<prm_float> e;
        e.add(attribute.type().variable());
        e.fill( (prm_float) 0.0);
        Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (prm_float) 1.0);
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      {
        const Instance& instance = prm->getSystem("aSys").get("c1");
        const Attribute& attribute = instance.get("can_print");
        Potential<prm_float> e;
        e.add(attribute.type().variable());
        e.fill( (prm_float) 0.0);
        Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (prm_float) 1.0);
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      // making a first inference with evidence
      {
        const Instance& q_i = prm->getSystem("aSys").get("pow");
        const Attribute& q_a = q_i.get("state");
        PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
        Potential<prm_float> m;
        g_ve->marginal(q_chain, m);
        Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 1.0, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.0, 1e-6);
      }
      // Removing evidences
      {
        const Instance& instance = prm->getSystem("aSys").get("c1");
        const Attribute& attribute = instance.get("can_print");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      {
        const Instance& instance = prm->getSystem("aSys").get("bw_printers[0]");
        const Attribute& attribute = instance.get("(t_degraded)equipState");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      // Inference without evidences
      {
        const Instance& q_i = prm->getSystem("aSys").get("pow");
        const Attribute& q_a = q_i.get("state");
        PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
        Potential<prm_float> m;
        g_ve->marginal(q_chain, m);
        Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
      }
      delete g_ve;
    }

    void testEvidenceSioux3() {
      SVED* g_ve = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new SVED(*prm, prm->getSystem("aSys")));
      // Adding evidence
      {
        const Instance& instance = prm->getSystem("aSys").get("bw_printers[0]");
        const Attribute& attribute = instance.get("(t_degraded)equipState");
        Potential<prm_float> e;
        e.add(attribute.type().variable());
        e.fill( (prm_float) 0.0);
        Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (prm_float) 1.0);
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      {
        const Instance& instance = prm->getSystem("aSys").get("c1");
        const Attribute& attribute = instance.get("can_print");
        Potential<prm_float> e;
        e.add(attribute.type().variable());
        e.fill( (prm_float) 0.0);
        Instantiation inst(e);
        inst.setFirst();
        e.set(inst, (prm_float) 1.0);
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
        TS_ASSERT(g_ve->hasEvidence(chain));
        TS_ASSERT(g_ve->hasEvidence(chain.first));
      }
      // making a first inference with evidence
      {
        const Instance& q_i = prm->getSystem("aSys").get("pow");
        const Attribute& q_a = q_i.get("state");
        PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->marginal(q_chain, m));
        Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 1.0, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.0, 1e-6);
      }
      // Removing evidences
      {
        const Instance& instance = prm->getSystem("aSys").get("c1");
        const Attribute& attribute = instance.get("can_print");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      {
        const Instance& instance = prm->getSystem("aSys").get("bw_printers[0]");
        const Attribute& attribute = instance.get("(t_degraded)equipState");
        PRMInference::Chain chain = std::make_pair(&instance, &attribute);
        TS_GUM_ASSERT_THROWS_NOTHING(g_ve->removeEvidence(chain));
      }
      // Inference without evidences
      {
        const Instance& q_i = prm->getSystem("aSys").get("pow");
        const Attribute& q_a = q_i.get("state");
        PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
        Potential<prm_float> m;
        g_ve->marginal(q_chain, m);
        Instantiation inst(m);
        inst.setFirst();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
        inst.inc();
        TS_ASSERT_DELTA(m.get(inst), 0.5, 1e-6);
      }
      delete g_ve;
    }

};

} // namespace tests
} // namespace gum
// ============================================================================
