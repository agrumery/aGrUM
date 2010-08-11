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
// ============================================================================
#include <agrum/prm/skool/SkoolReader.h>
// ============================================================================

namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;
using namespace gum::prm::skool;

class GroundedBNTestSuite: public CxxTest::TestSuite {
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
      std::cerr << std::endl;
    }

    void tearDown() {
      delete prm;
      delete small;
    }

//     void testCreation() {
//       ClassBayesNet* c = 0;
//       TS_GUM_ASSERT_THROWS_NOTHING(c = new ClassBayesNet(prm->getClass("SafeComputer")));
//       TS_GUM_ASSERT_THROWS_NOTHING(delete c);
//       InstanceBayesNet* inst = 0;
//       TS_GUM_ASSERT_THROWS_NOTHING(inst = new InstanceBayesNet(prm->getSystem("aSys").get("c1")));
//       TS_GUM_ASSERT_THROWS_NOTHING(delete inst);
//     }
// 
//     void testClassAccess() {
//       Class& c = prm->getClass("SafeComputer");
//       ClassBayesNet* bn = 0;
//       TS_GUM_ASSERT_THROWS_NOTHING(bn = new ClassBayesNet(prm->getClass("SafeComputer")));
//       Size elts = c.attributes().size() + c.aggregates().size();
//       TS_ASSERT_EQUALS(bn->size(), elts);
//       for (Set<Attribute*>::iterator attr = c.attributes().begin(); attr != c.attributes().end(); ++attr) {
//         NodeId id = 0;
//         TS_GUM_ASSERT_THROWS_NOTHING((**attr).cpf());
//         TS_GUM_ASSERT_THROWS_NOTHING(id = bn->idFromName((**attr).safeName()));
//         TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id));
//         TS_ASSERT_EQUALS((**attr).cpf().nbrDim(), bn->cpt(id).nbrDim());
//         TS_ASSERT_EQUALS((**attr).cpf().domainSize(), bn->cpt(id).domainSize());
//       }
//       TS_ASSERT(bn->modalities().size() > 0);
//       TS_GUM_ASSERT_THROWS_NOTHING(delete bn);
//     }
// 
//     void testInstanceAccess() {
//       InstanceBayesNet* bn = 0;
//       Instance& i = prm->getSystem("aSys").get("c1");
//       TS_GUM_ASSERT_THROWS_NOTHING(bn = new InstanceBayesNet(i));
//       TS_ASSERT_EQUALS(bn->size(), i.size());
//       for (Instance::iterator attr = i.begin(); attr != i.end(); ++attr) {
//         NodeId id = 0;
//         TS_GUM_ASSERT_THROWS_NOTHING((**attr).cpf());
//         TS_GUM_ASSERT_THROWS_NOTHING(id = bn->idFromName((**attr).safeName()));
//         TS_GUM_ASSERT_THROWS_NOTHING(bn->cpt(id));
//         TS_ASSERT_EQUALS((**attr).cpf().nbrDim(), bn->cpt(id).nbrDim());
//         TS_ASSERT_EQUALS((**attr).cpf().domainSize(), bn->cpt(id).domainSize());
//       }
//       TS_ASSERT(bn->modalities().size() > 0);
//       TS_GUM_ASSERT_THROWS_NOTHING(delete bn);
//     }
// 
//     void testGroundedBN() {
//       System& sys = prm->getSystem("aSys");
//       BayesNet<prm_float> bn;
//       BayesNetFactory<prm_float> bn_factory(&bn);
//       TS_GUM_ASSERT_THROWS_NOTHING(sys.groundedBN(bn_factory));
//       int count = 0;
//       for (System::iterator iter = sys.begin(); iter != sys.end(); ++iter) {
//         for (Instance::iterator jter = (**iter).begin(); jter != (**iter).end(); ++jter) {
//           ++count;
//         }
//       }
//       int wount = 0;
//       for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
//         wount++;
//         std::string var = bn.variable(*node).name();
//         size_t pos = var.find_first_of('.');
//         Instance& instance = sys.get(var.substr(0, pos));
//         Attribute& attr = instance.get(var.substr(pos +1));
//         if (bn.cpt(*node).nbrDim() == 0) {
//           std::stringstream sBuff;
//           sBuff << instance.name() << "." << attr.safeName();
//           GUM_TRACE(sBuff.str());
//         }
//         if (ClassElement::isAggregate(instance.type().get(attr.id()))) {
//           if (attr.cpf().nbrDim() == 1) {
//             std::stringstream sBuff;
//             sBuff << instance.name() << "." << attr.safeName();
//             GUM_TRACE(sBuff.str());
//           }
//         }
//       }
//       TS_ASSERT_EQUALS(count, wount);
//       for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
//         const DiscreteVariable* var = &(bn.variable(*node));
//         for (DAG::NodeIterator mode = bn.dag().beginNodes(); mode != bn.dag().endNodes(); ++mode) {
//           if ((*node) != (*mode)) {
//             TS_ASSERT_DIFFERS(var, &(bn.variable(*mode)));
//           }
//         }
//       }
//     }
// 
//     void testEvidence() {
//       GroundedInference* g_ve = 0;
//       VariableElimination<prm_float>* ve = 0;
//       BayesNet<prm_float> bn;
//       BayesNetFactory<prm_float> bn_factory(&bn);
//       TS_GUM_ASSERT_THROWS_NOTHING(prm->getSystem("aSys").groundedBN(bn_factory));
//       TS_GUM_ASSERT_THROWS_NOTHING(ve = new VariableElimination<prm_float>(bn));
//       TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new GroundedInference(*prm, prm->getSystem("aSys")));
//       TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
//       const Instance& instance = prm->getSystem("aSys").get("c1");
//       const Attribute& attribute = instance.get("can_print");
//       Potential<prm_float> e;
//       e.add(attribute.type().variable());
//       e.fill( (prm_float) 0.0);
//       Instantiation inst(e);
//       inst.setFirst();
//       e.set(inst, (prm_float) 1.0);
//       PRMInference::Chain chain = std::make_pair(&instance, &attribute);
//       TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
//       TS_ASSERT(g_ve->hasEvidence(chain));
//       delete g_ve;
//     }
// 
//     // void testSmallGrdInference() {
//     //   GUM_TRACE("VE");
//     //   // Creating the inference engine
//     //   GroundedInference* g_ve = 0;
//     //   VariableElimination<prm_float>* ve = 0;
//     //   BayesNet<prm_float> bn;
//     //   BayesNetFactory<prm_float> bn_factory(&bn);
//     //   TS_GUM_ASSERT_THROWS_NOTHING(small->getSystem("microSys").groundedBN(bn_factory));
//     //   TS_GUM_ASSERT_THROWS_NOTHING(ve = new VariableElimination<prm_float>(bn));
//     //   TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new GroundedInference(*small, small->getSystem("microSys")));
//     //   TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
//     //   // Building query
//     //   {
//     //     const Instance& instance = small->getSystem("microSys").get("c");
//     //     const Attribute& attribute = instance.get("can_print");
//     //     PRMInference::Chain chain = std::make_pair(&instance, &attribute);
//     //     Potential<prm_float> m;
//     //     TS_GUM_ASSERT_THROWS_NOTHING(g_ve->marginal(chain, m));
//     //     Instantiation i(m);
//     //     for (i.setFirst(); not i.end(); i.inc()) {
//     //       std::stringstream sBuff;
//     //       sBuff << i << ": " << m.get(i);
//     //       GUM_TRACE(sBuff.str());
//     //     }
//     //   }
//     //   std::cerr << std::endl;
//     //   {
//     //     const Instance& instance = small->getSystem("microSys").get("p");
//     //     const Attribute& attribute = instance.get("equipState");
//     //     PRMInference::Chain chain = std::make_pair(&instance, &attribute);
//     //     Potential<prm_float> m;
//     //     TS_GUM_ASSERT_THROWS_NOTHING(g_ve->marginal(chain, m));
//     //     Instantiation i(m);
//     //     for (i.setFirst(); not i.end(); i.inc()) {
//     //       std::stringstream sBuff;
//     //       sBuff << i << ": " << m.get(i);
//     //       GUM_TRACE(sBuff.str());
//     //     }
//     //   }
//     //   std::cerr << std::endl;
//     //   {
//     //     const Instance& instance = small->getSystem("microSys").get("pow");
//     //     const Attribute& attribute = instance.get("powState");
//     //     PRMInference::Chain chain = std::make_pair(&instance, &attribute);
//     //     Potential<prm_float> m;
//     //     TS_GUM_ASSERT_THROWS_NOTHING(g_ve->marginal(chain, m));
//     //     Instantiation i(m);
//     //     for (i.setFirst(); not i.end(); i.inc()) {
//     //       std::stringstream sBuff;
//     //       sBuff << i << ": " << m.get(i);
//     //       GUM_TRACE(sBuff.str());
//     //     }
//     //   }
//     //   delete g_ve;
//     // }
// 
//     // void testSmallSVEInference() {
//     //   GUM_TRACE("SVE");
//     //   SVE* sve = 0;
//     //   TS_GUM_ASSERT_THROWS_NOTHING(sve = new SVE(*small, small->getSystem("microSys")));
//     //   // Building query
//     //   {
//     //     const Instance& instance = small->getSystem("microSys").get("c");
//     //     const Attribute& attribute = instance.get("can_print");
//     //     PRMInference::Chain chain = std::make_pair(&instance, &attribute);
//     //     Potential<prm_float> m;
//     //     TS_GUM_ASSERT_THROWS_NOTHING(sve->marginal(chain, m));
//     //     Instantiation i(m);
//     //     for (i.setFirst(); not i.end(); i.inc()) {
//     //       std::stringstream sBuff;
//     //       sBuff << i << ": " << m.get(i);
//     //       GUM_TRACE(sBuff.str());
//     //     }
//     //   }
//     //   std::cerr << std::endl;
//     //   {
//     //     const Instance& instance = small->getSystem("microSys").get("p");
//     //     const Attribute& attribute = instance.get("equipState");
//     //     PRMInference::Chain chain = std::make_pair(&instance, &attribute);
//     //     Potential<prm_float> m;
//     //     TS_GUM_ASSERT_THROWS_NOTHING(sve->marginal(chain, m));
//     //     Instantiation i(m);
//     //     for (i.setFirst(); not i.end(); i.inc()) {
//     //       std::stringstream sBuff;
//     //       sBuff << i << ": " << m.get(i);
//     //       GUM_TRACE(sBuff.str());
//     //     }
//     //   }
//     //   std::cerr << std::endl;
//     //   // Creating the inference engine
//     //   {
//     //     const Instance& instance = small->getSystem("microSys").get("pow");
//     //     const Attribute& attribute = instance.get("powState");
//     //     PRMInference::Chain chain = std::make_pair(&instance, &attribute);
//     //     Potential<prm_float> m;
//     //     TS_GUM_ASSERT_THROWS_NOTHING(sve->marginal(chain, m));
//     //     Instantiation i(m);
//     //     for (i.setFirst(); not i.end(); i.inc()) {
//     //       std::stringstream sBuff;
//     //       sBuff << i << ": " << m.get(i);
//     //       GUM_TRACE(sBuff.str());
//     //     }
//     //   }
//     //   std::cerr << std::endl;
//     //   delete sve;
//     // }
// 
//     // void testSmallSVEDInference() {
//     //   GUM_TRACE("SVED");
//     //   SVED* sved = 0;
//     //   TS_GUM_ASSERT_THROWS_NOTHING(sved = new SVED(*small, small->getSystem("microSys")));
//     //   // Building query
//     //   {
//     //     const Instance& instance = small->getSystem("microSys").get("c");
//     //     const Attribute& attribute = instance.get("can_print");
//     //     PRMInference::Chain chain = std::make_pair(&instance, &attribute);
//     //     Potential<prm_float> m;
//     //     TS_GUM_ASSERT_THROWS_NOTHING(sved->marginal(chain, m));
//     //     Instantiation i(m);
//     //     for (i.setFirst(); not i.end(); i.inc()) {
//     //       std::stringstream sBuff;
//     //       sBuff << i << ": " << m.get(i);
//     //       GUM_TRACE(sBuff.str());
//     //     }
//     //   }
//     //   std::cerr << std::endl;
//     //   {
//     //     const Instance& instance = small->getSystem("microSys").get("p");
//     //     const Attribute& attribute = instance.get("equipState");
//     //     PRMInference::Chain chain = std::make_pair(&instance, &attribute);
//     //     Potential<prm_float> m;
//     //     TS_GUM_ASSERT_THROWS_NOTHING(sved->marginal(chain, m));
//     //     Instantiation i(m);
//     //     for (i.setFirst(); not i.end(); i.inc()) {
//     //       std::stringstream sBuff;
//     //       sBuff << i << ": " << m.get(i);
//     //       GUM_TRACE(sBuff.str());
//     //     }
//     //   }
//     //   std::cerr << std::endl;
//     //   // Creating the inference engine
//     //   {
//     //     const Instance& instance = small->getSystem("microSys").get("pow");
//     //     const Attribute& attribute = instance.get("powState");
//     //     PRMInference::Chain chain = std::make_pair(&instance, &attribute);
//     //     Potential<prm_float> m;
//     //     TS_GUM_ASSERT_THROWS_NOTHING(sved->marginal(chain, m));
//     //     Instantiation i(m);
//     //     for (i.setFirst(); not i.end(); i.inc()) {
//     //       std::stringstream sBuff;
//     //       sBuff << i << ": " << m.get(i);
//     //       GUM_TRACE(sBuff.str());
//     //     }
//     //   }
//     //   std::cerr << std::endl;
//     //   delete sved;
//     // }
// 
//     void testInference() {
//       //GUM_TRACE_VAR(UINT_MAX);
//       GroundedInference* g_ve = 0;
//       GroundedInference* g_ss = 0;
//       GroundedInference* g_vebb = 0;
//       VariableElimination<prm_float>* ve = 0;
//       ShaferShenoyInference<prm_float>* ss = 0;
//       VEWithBB<prm_float>* vebb = 0;
//       BayesNet<prm_float> bn;
//       BayesNetFactory<prm_float> bn_factory(&bn);
//       TS_GUM_ASSERT_THROWS_NOTHING(prm->getSystem("aSys").groundedBN(bn_factory));
//       TS_GUM_ASSERT_THROWS_NOTHING(ve = new VariableElimination<prm_float>(bn));
//       TS_GUM_ASSERT_THROWS_NOTHING(ss = new ShaferShenoyInference<prm_float>(bn));
//       TS_GUM_ASSERT_THROWS_NOTHING(vebb = new VEWithBB<prm_float>(bn));
//       TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new GroundedInference(*prm, prm->getSystem("aSys")));
//       TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
//       TS_GUM_ASSERT_THROWS_NOTHING(g_ss = new GroundedInference(*prm, prm->getSystem("aSys")));
//       TS_GUM_ASSERT_THROWS_NOTHING(g_ss->setBNInference(ss));
//       TS_GUM_ASSERT_THROWS_NOTHING(g_vebb = new GroundedInference(*prm, prm->getSystem("aSys")));
//       TS_GUM_ASSERT_THROWS_NOTHING(g_vebb->setBNInference(vebb));
//       for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
//         Potential<prm_float> m_ve, m_ss, m_sve, m_sved, m_vebb;
//         try {
//           size_t pos = bn.variableNodeMap().name(*node).find_first_of('.');
//           const Instance& instance = prm->getSystem("aSys").get(bn.variableNodeMap().name(*node).substr(0, pos));
//           const Attribute& attribute = instance.get(bn.variableNodeMap().name(*node).substr(pos+1));
//           //if ((instance.name() == "c1") and (attribute.safeName() == "(boolean)can_print")) {
//             PRMInference::Chain chain = std::make_pair(&instance, &attribute);
//             std::string dot = ".";
//             g_ve->marginal(chain, m_ve);
//             // GUM_TRACE("VE done");
//             g_ss->marginal(chain, m_ss);
//             // GUM_TRACE("SS done");
//             g_vebb->marginal(chain, m_vebb);
//             // GUM_TRACE("VEWithBB done");
//             SVE sve(*prm, prm->getSystem("aSys"));
//             sve.marginal(chain, m_sve);
//             // GUM_TRACE("SVE done");
//             SVED sved(*prm, prm->getSystem("aSys"));
//             sved.marginal(chain, m_sved);
//             Instantiation inst(m_ve), jnst(m_sve);
//             Set<std::string> plop;
//             std::string foo = instance.name() + dot + attribute.safeName();
//             for (inst.setFirst(), jnst.setFirst(); not (inst.end() or jnst.end()); inst.inc(), jnst.inc()) {
//               TS_ASSERT_EQUALS(m_ve.nbrDim(), m_ss.nbrDim());
//               TS_ASSERT_EQUALS(m_ve.nbrDim(), m_sve.nbrDim());
//               TS_ASSERT_EQUALS(m_ve.nbrDim(), m_sved.nbrDim());
//               TS_ASSERT_EQUALS(m_ve.nbrDim(), m_vebb.nbrDim());
//               TS_ASSERT_EQUALS(m_ve.domainSize(), m_ss.domainSize());
//               TS_ASSERT_EQUALS(m_ve.domainSize(), m_sve.domainSize());
//               TS_ASSERT_EQUALS(m_ve.domainSize(), m_sved.domainSize());
//               TS_ASSERT_EQUALS(m_ve.domainSize(), m_vebb.domainSize());
//               float bar = (m_ve.get(inst) - m_sved.get(jnst));
//               bar = (bar>0?bar:-bar);
//               if (  bar > 10e-4 ) {
//                 if (not plop.exists(foo)) {
//                   plop.insert(foo);
//                   GUM_TRACE(foo);
//                 }
//               }
//               TS_ASSERT_DELTA(m_ve.get(inst), m_ss.get(inst), 1.0e-3);
//               TS_ASSERT_DELTA(m_sve.get(jnst), m_ss.get(inst), 1.0e-3);
//               TS_ASSERT_DELTA(m_vebb.get(inst), m_ss.get(inst), 1.0e-3);
//               TS_ASSERT_DELTA(m_sved.get(jnst), m_sve.get(jnst), 1.0e-3);
//             }
//             bool zero = true;
//             for (jnst.setFirst(); not jnst.end(); jnst.inc()) {
//               if (m_sved.get(jnst) != (prm_float) 0.0) {
//                 zero = false;
//                 break;
//               }
//             }
//             if (zero) {
//               GUM_TRACE(foo);
//             }
//           //}
//         } catch (Exception& e) {
//           TS_GUM_ASSERT_THROWS_NOTHING(throw e);
//           break;
//         }
//       }
//       delete g_ve;
//       delete g_ss;
//     }
// 
//     void testStructuredBB() {
//       SVE sve(*prm, prm->getSystem("aSys"));
//       StructuredBayesBall* bb = 0;
//       TS_GUM_ASSERT_THROWS_NOTHING(bb = new StructuredBayesBall(sve));
//       Instance& i = prm->getSystem("aSys").get("pow");
//       TS_GUM_ASSERT_THROWS_NOTHING(bb->compute(i, i.get("(t_state)state").id()));
//       TS_GUM_ASSERT_THROWS_NOTHING(delete bb);
//     }

    // void testEvidenceSioux() {
    //   GUM_CHECKPOINT;
    //   GroundedInference* g_ve = 0;
    //   VariableElimination<prm_float>* ve = 0;
    //   BayesNet<prm_float> bn;
    //   BayesNetFactory<prm_float> bn_factory(&bn);
    //   TS_GUM_ASSERT_THROWS_NOTHING(prm->getSystem("aSys").groundedBN(bn_factory));
    //   TS_GUM_ASSERT_THROWS_NOTHING(ve = new VariableElimination<prm_float>(bn));
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new GroundedInference(*prm, prm->getSystem("aSys")));
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
    //   GUM_CHECKPOINT;
    //   // making a first inference without evidence
    //   {
    //     const Instance& q_i = prm->getSystem("aSys").get("pow");
    //     const Attribute& q_a = q_i.get("state");
    //     PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
    //     Potential<prm_float> m;
    //     g_ve->marginal(q_chain, m);
    //     {
    //       Instantiation inst(m);
    //       for (inst.setFirst(); not inst.end(); inst.inc()) {
    //         GUM_TRACE(m.get(inst));
    //       }
    //     }
    //   }
    //   // Adding evidence
    //   const Instance& instance = prm->getSystem("aSys").get("c1");
    //   const Attribute& attribute = instance.get("can_print");
    //   Potential<prm_float> e;
    //   e.add(attribute.type().variable());
    //   e.fill( (prm_float) 0.0);
    //   Instantiation inst(e);
    //   inst.setFirst();
    //   e.set(inst, (prm_float) 1.0);
    //   PRMInference::Chain chain = std::make_pair(&instance, &attribute);
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
    //   TS_ASSERT(g_ve->hasEvidence(chain));
    //   TS_ASSERT(g_ve->hasEvidence(chain.first));
    //   GUM_CHECKPOINT;
    //   {
    //     const Instance& q_i = prm->getSystem("aSys").get("pow");
    //     const Attribute& q_a = q_i.get("state");
    //     PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
    //     Potential<prm_float> m;
    //     g_ve->marginal(q_chain, m);
    //     {
    //       Instantiation inst(m);
    //       for (inst.setFirst(); not inst.end(); inst.inc()) {
    //         GUM_TRACE(m.get(inst));
    //       }
    //     }
    //   }
    //   GUM_CHECKPOINT;
    //   delete g_ve;
    // }

    // void testEvidenceSioux2() {
    //   GUM_CHECKPOINT;
    //   SVE* g_ve = 0;
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new SVE(*prm, prm->getSystem("aSys")));
    //   GUM_CHECKPOINT;
    //   // making a first inference without evidence
    //   {
    //     const Instance& q_i = prm->getSystem("aSys").get("pow");
    //     const Attribute& q_a = q_i.get("state");
    //     PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
    //     Potential<prm_float> m;
    //     g_ve->marginal(q_chain, m);
    //     {
    //       Instantiation inst(m);
    //       for (inst.setFirst(); not inst.end(); inst.inc()) {
    //         GUM_TRACE(m.get(inst));
    //       }
    //     }
    //   }
    //   // Adding evidence
    //   const Instance& instance = prm->getSystem("aSys").get("c1");
    //   const Attribute& attribute = instance.get("can_print");
    //   Potential<prm_float> e;
    //   e.add(attribute.type().variable());
    //   e.fill( (prm_float) 0.0);
    //   Instantiation inst(e);
    //   inst.setFirst();
    //   e.set(inst, (prm_float) 1.0);
    //   PRMInference::Chain chain = std::make_pair(&instance, &attribute);
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
    //   TS_ASSERT(g_ve->hasEvidence(chain));
    //   TS_ASSERT(g_ve->hasEvidence(chain.first));
    //   GUM_CHECKPOINT;
    //   {
    //     const Instance& q_i = prm->getSystem("aSys").get("pow");
    //     const Attribute& q_a = q_i.get("state");
    //     PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
    //     Potential<prm_float> m;
    //     g_ve->marginal(q_chain, m);
    //     {
    //       Instantiation inst(m);
    //       for (inst.setFirst(); not inst.end(); inst.inc()) {
    //         GUM_TRACE(m.get(inst));
    //       }
    //     }
    //   }
    //   GUM_CHECKPOINT;
    //   delete g_ve;
    // }

    // void testEvidenceSioux3() {
    //   GUM_CHECKPOINT;
    //   SVED* g_ve = 0;
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new SVED(*prm, prm->getSystem("aSys")));
    //   GUM_CHECKPOINT;
    //   // making a first inference without evidence
    //   // {
    //   //   const Instance& q_i = prm->getSystem("aSys").get("pow");
    //   //   const Attribute& q_a = q_i.get("state");
    //   //   PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
    //   //   Potential<prm_float> m;
    //   //   g_ve->marginal(q_chain, m);
    //   //   {
    //   //     Instantiation inst(m);
    //   //     for (inst.setFirst(); not inst.end(); inst.inc()) {
    //   //       GUM_TRACE(m.get(inst));
    //   //     }
    //   //   }
    //   // }
    //   // Adding evidence
    //   const Instance& instance = prm->getSystem("aSys").get("c1");
    //   const Attribute& attribute = instance.get("can_print");
    //   Potential<prm_float> e;
    //   e.add(attribute.type().variable());
    //   e.fill( (prm_float) 0.0);
    //   Instantiation inst(e);
    //   inst.setFirst();
    //   e.set(inst, (prm_float) 1.0);
    //   PRMInference::Chain chain = std::make_pair(&instance, &attribute);
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_ve->addEvidence(chain, e));
    //   TS_ASSERT(g_ve->hasEvidence(chain));
    //   TS_ASSERT(g_ve->hasEvidence(chain.first));
    //   GUM_CHECKPOINT;
    //   {
    //     const Instance& q_i = prm->getSystem("aSys").get("pow");
    //     const Attribute& q_a = q_i.get("state");
    //     PRMInference::Chain q_chain = std::make_pair(&q_i, &q_a);
    //     Potential<prm_float> m;
    //     g_ve->marginal(q_chain, m);
    //     {
    //       Instantiation inst(m);
    //       for (inst.setFirst(); not inst.end(); inst.inc()) {
    //         GUM_TRACE(m.get(inst));
    //       }
    //     }
    //   }
    //   GUM_CHECKPOINT;
    //   delete g_ve;
    // }

};

} // namespace tests
} // namespace gum
// ============================================================================
