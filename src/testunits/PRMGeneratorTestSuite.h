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
#include <agrum/prm/generator/layerGenerator.h>
// ============================================================================

namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;

class PRMInferenceTestSuite: public CxxTest::TestSuite {
  private:

  public:
    void setUp() {
      std::cerr << std::endl;
    }

    void tearDown() { }

    void testConstructors() {
      LayerGenerator* gen = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(gen = new LayerGenerator());
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void testPyramidalGen() {
      std::vector<LayerGenerator::LayerData> v;
      size_t interface_size = 3;
      size_t agg_size = 0;
      float agg_ratio = 1;
      size_t c_count = 1;
      size_t a_count = 10;
      float density = 0.2;
      size_t i_count = 1;
      v.push_back(LayerGenerator::LayerData());
      v.back().interface_size = interface_size;
      v.back().agg_size = agg_size;
      v.back().agg_ratio = agg_ratio;
      v.back().c_count = c_count;
      v.back().a_count = a_count;
      v.back().density = density;
      v.back().i_count = i_count;
      agg_size = 2;
      for (size_t lvl = 1; lvl < 10; ++lvl, ++i_count, ++c_count) {
        v.push_back(LayerGenerator::LayerData());
        v.back().interface_size = interface_size;
        v.back().agg_size = agg_size;
        v.back().agg_ratio = agg_ratio;
        v.back().c_count = c_count;
        v.back().a_count = a_count;
        v.back().density = density;
        v.back().i_count = i_count;
      }
      for (size_t i = 0; i < 10000; ++i) {
        LayerGenerator* gen = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(gen = new LayerGenerator());
        TS_GUM_ASSERT_THROWS_NOTHING(gen->setLayers(v));
        PRM* prm = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(prm = gen->generate());
        TS_GUM_ASSERT_THROWS_NOTHING(delete prm);
        TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
      }
    }

    // void testInference() {
    //   //GUM_TRACE_VAR(UINT_MAX);
    //   GroundedInference* g_ve = 0;
    //   GroundedInference* g_ss = 0;
    //   GroundedInference* g_vebb = 0;
    //   VariableElimination<prm_float>* ve = 0;
    //   ShaferShenoyInference<prm_float>* ss = 0;
    //   VEWithBB<prm_float>* vebb = 0;
    //   BayesNet<prm_float> bn;
    //   BayesNetFactory<prm_float> bn_factory(&bn);
    //   TS_GUM_ASSERT_THROWS_NOTHING(prm->getSystem("aSys").groundedBN(bn_factory));
    //   TS_GUM_ASSERT_THROWS_NOTHING(ve = new VariableElimination<prm_float>(bn));
    //   TS_GUM_ASSERT_THROWS_NOTHING(ss = new ShaferShenoyInference<prm_float>(bn));
    //   TS_GUM_ASSERT_THROWS_NOTHING(vebb = new VEWithBB<prm_float>(bn));
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_ve = new GroundedInference(*prm, prm->getSystem("aSys")));
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_ve->setBNInference(ve));
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_ss = new GroundedInference(*prm, prm->getSystem("aSys")));
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_ss->setBNInference(ss));
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_vebb = new GroundedInference(*prm, prm->getSystem("aSys")));
    //   TS_GUM_ASSERT_THROWS_NOTHING(g_vebb->setBNInference(vebb));
    //   for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
    //     Potential<prm_float> m_ve, m_ss, m_sve, m_sved, m_vebb, m_struct;
    //     try {
    //       size_t pos = bn.variableNodeMap().name(*node).find_first_of('.');
    //       const Instance& instance = prm->getSystem("aSys").get(bn.variableNodeMap().name(*node).substr(0, pos));
    //       const Attribute& attribute = instance.get(bn.variableNodeMap().name(*node).substr(pos+1));
    //       PRMInference::Chain chain = std::make_pair(&instance, &attribute);
    //       std::string dot = ".";
    //       g_ve->marginal(chain, m_ve);
    //       // GUM_TRACE("VE done");
    //       g_ss->marginal(chain, m_ss);
    //       // GUM_TRACE("SS done");
    //       g_vebb->marginal(chain, m_vebb);
    //       // GUM_TRACE("VEWithBB done");
    //       SVE sve(*prm, prm->getSystem("aSys"));
    //       sve.marginal(chain, m_sve);
    //       // GUM_TRACE("SVE done");
    //       SVED sved(*prm, prm->getSystem("aSys"));
    //       sved.marginal(chain, m_sved);
    //       // GUM_TRACE("SVED done");
    //       StructuredInference structinf(*prm, prm->getSystem("aSys"));
    //       structinf.marginal(chain, m_struct);
    //       // GUM_TRACE("StructInf done");
    //       // We need two instantiations, one for the grounded potentials and one
    //       // for the PRM-level ones
    //       Instantiation inst(m_ve), jnst(m_sve);
    //       std::string foo = instance.name() + dot + attribute.safeName();
    //       for (inst.setFirst(), jnst.setFirst(); not (inst.end() or jnst.end()); inst.inc(), jnst.inc())
    //       {
    //         TS_ASSERT_EQUALS(m_ve.nbrDim(), m_ss.nbrDim());
    //         TS_ASSERT_EQUALS(m_ve.nbrDim(), m_sve.nbrDim());
    //         TS_ASSERT_EQUALS(m_ve.nbrDim(), m_sved.nbrDim());
    //         TS_ASSERT_EQUALS(m_ve.nbrDim(), m_vebb.nbrDim());
    //         TS_ASSERT_EQUALS(m_ve.nbrDim(), m_struct.nbrDim());
    //         TS_ASSERT_EQUALS(m_ve.domainSize(), m_ss.domainSize());
    //         TS_ASSERT_EQUALS(m_ve.domainSize(), m_sve.domainSize());
    //         TS_ASSERT_EQUALS(m_ve.domainSize(), m_sved.domainSize());
    //         TS_ASSERT_EQUALS(m_ve.domainSize(), m_vebb.domainSize());
    //         TS_ASSERT_EQUALS(m_ve.domainSize(), m_struct.domainSize());
    //         TS_ASSERT_DELTA(m_ve.get(inst), m_ss.get(inst), 1.0e-3);
    //         TS_ASSERT_DELTA(m_sve.get(jnst), m_ss.get(inst), 1.0e-3);
    //         TS_ASSERT_DELTA(m_vebb.get(inst), m_ss.get(inst), 1.0e-3);
    //         TS_ASSERT_DELTA(m_sved.get(jnst), m_sve.get(jnst), 1.0e-3);
    //         TS_ASSERT_DELTA(m_sved.get(jnst), m_struct.get(jnst), 1.0e-3);
    //       }
    //       bool zero = true;
    //       for (jnst.setFirst(); not jnst.end(); jnst.inc()) {
    //         if (m_sved.get(jnst) != (prm_float) 0.0) {
    //           zero = false;
    //           break;
    //         }
    //       }
    //       if (zero) {
    //         GUM_TRACE(foo);
    //       }
    //     } catch (Exception& e) {
    //       GUM_TRACE_VAR(bn.variableNodeMap().name(*node));
    //       TS_GUM_ASSERT_THROWS_NOTHING(throw e);
    //       break;
    //     }
    //   }
    //   delete g_ve;
    //   delete g_ss;
    //   delete g_vebb;
    // }
};

} // namespace tests
} // namespace gum
// ============================================================================
