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
#include <fstream>
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
#include <agrum/prm/generator/clusteredLayerGenerator.h>
// ============================================================================

namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;

class PRMGeneratorTestSuite: public CxxTest::TestSuite {
  private:

  public:
    void setUp() {
      //std::cerr << std::endl;
    }

    void tearDown() { }

    void testConstructors() {
      LayerGenerator* gen = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(gen = new LayerGenerator());
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    // void testPyramidalGen() {
    //   std::vector<LayerGenerator::LayerData> v;
    //   size_t interface_size = 3;
    //   size_t agg_size = 0;
    //   float agg_ratio = 1;
    //   size_t c_count = 1;
    //   size_t a_count = 10;
    //   float density = 0.2;
    //   size_t i_count = 1;
    //   v.push_back(LayerGenerator::LayerData());
    //   v.back().interface_size = interface_size;
    //   v.back().agg_size = agg_size;
    //   v.back().agg_ratio = agg_ratio;
    //   v.back().c_count = c_count;
    //   v.back().a_count = a_count;
    //   v.back().density = density;
    //   v.back().i_count = i_count;
    //   agg_size = 2;
    //   for (size_t lvl = 1; lvl < 10; ++lvl, ++i_count, ++c_count) {
    //     v.push_back(LayerGenerator::LayerData());
    //     v.back().interface_size = interface_size;
    //     v.back().agg_size = agg_size;
    //     v.back().agg_ratio = agg_ratio;
    //     v.back().c_count = c_count;
    //     v.back().a_count = a_count;
    //     v.back().density = density;
    //     v.back().i_count = i_count;
    //   }
    //   for (size_t i = 0; i < 100; ++i) {
    //     LayerGenerator* gen = 0;
    //     TS_GUM_ASSERT_THROWS_NOTHING(gen = new LayerGenerator());
    //     TS_GUM_ASSERT_THROWS_NOTHING(gen->setLayers(v));
    //     PRM* prm = 0;
    //     TS_GUM_ASSERT_THROWS_NOTHING(prm = gen->generate());
    //     TS_GUM_ASSERT_THROWS_NOTHING(delete prm);
    //     TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    //   }
    // }

    void testStructuredInference() {
      std::vector<LayerGenerator::LayerData> v;
      __generateLayer1(v);
      LayerGenerator* gen = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(gen = new LayerGenerator());
      TS_GUM_ASSERT_THROWS_NOTHING(gen->setLayers(v));
      PRM* prm = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = gen->generate());
      Class& c = **(prm->classes().begin());
      System& s = **(prm->systems().begin());
      Instance& i = **(s.get(c).begin());
      Attribute& a = **(i.begin());
      PRMInference::Chain chain(&i, &a);
      StructuredInference structinf(*prm, prm->getSystem(s.name()), 2, 10, new gspan::StrictSearch());
      structinf.setPatternMining(false);
      Potential<prm_float> m_struct;
      TS_GUM_ASSERT_THROWS_NOTHING(structinf.marginal(chain, m_struct));
      Instantiation inst(m_struct);
      delete prm;
      delete gen;
    }

    void testInference() {
      std::vector<LayerGenerator::LayerData> v;
      __generateLayer1(v);
      LayerGenerator* gen = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(gen = new LayerGenerator());
      TS_GUM_ASSERT_THROWS_NOTHING(gen->setLayers(v));
      PRM* prm = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = gen->generate());
      std::string sys = (**(prm->systems().begin())).name();
      GroundedInference* g_vebb = 0;
      VEWithBB<prm_float>* vebb = 0;
      BayesNet<prm_float> bn;
      BayesNetFactory<prm_float> bn_factory(&bn);
      TS_GUM_ASSERT_THROWS_NOTHING(prm->getSystem(sys).groundedBN(bn_factory));
      TS_GUM_ASSERT_THROWS_NOTHING(vebb = new VEWithBB<prm_float>(bn));
      TS_GUM_ASSERT_THROWS_NOTHING(g_vebb = new GroundedInference(*prm, prm->getSystem(sys)));
      TS_GUM_ASSERT_THROWS_NOTHING(g_vebb->setBNInference(vebb));
      //std::cout << bn.toDot() << std::flush;
      for (DAG::NodeIterator node = bn.dag().beginNodes(); node != bn.dag().endNodes(); ++node) {
        Potential<prm_float> m_vebb, m_struct;
        try {
          size_t pos = bn.variableNodeMap().name(*node).find_first_of('.');
          const Instance& instance = prm->getSystem(sys).get(bn.variableNodeMap().name(*node).substr(0, pos));
          const Attribute& attribute = instance.get(bn.variableNodeMap().name(*node).substr(pos+1));
          PRMInference::Chain chain = std::make_pair(&instance, &attribute);
          std::string dot = ".";
          g_vebb->marginal(chain, m_vebb);
          StructuredInference structinf(*prm, prm->getSystem(sys));
          structinf.setPatternMining(false);
          structinf.marginal(chain, m_struct);
          // We need two instantiations, one for the grounded potentials and one
          // for the PRM-level ones
          Instantiation inst(m_vebb), jnst(m_struct);
          std::string foo = instance.name() + dot + attribute.safeName();
          TS_ASSERT_EQUALS(m_vebb.nbrDim(), m_struct.nbrDim());
          prm_float sum = 0.0;
          for (inst.setFirst(), jnst.setFirst(); not (inst.end() or jnst.end()); inst.inc(), jnst.inc())
          {
            sum += m_vebb.get(inst);
            TS_ASSERT_DELTA(m_vebb.get(inst), m_struct.get(jnst), 1.0e-3);
          }
          TS_ASSERT_DELTA(sum, (prm_float) 1.0, 1.0e-3);
        } catch (Exception& e) {
          GUM_TRACE_VAR(bn.variableNodeMap().name(*node));
          TS_GUM_ASSERT_THROWS_NOTHING(throw e);
          break;
        }
//#ifdef GUM_NO_INLINE
        break;
//#endif
      }
      delete g_vebb;
      delete prm;
      delete gen;
    }

    void generateLayerLayer(std::vector<LayerGenerator::LayerData>& v, size_t layer_count)
    {
      for (size_t lvl = 0; lvl < layer_count; ++lvl) {
        v.push_back(LayerGenerator::LayerData());
        v[lvl].a = 30;
        v[lvl].g = 2;
        v[lvl].c = 1;
        v[lvl].o = 10;
        v[lvl].inner_density = 0.2;
        v[lvl].outter_density = 0.05;
      }
    }

    const Instance& pickInstance(const System& sys) {
      Sequence<const Instance*> seq;
      for (System::const_iterator iter = sys.begin(); iter != sys.end(); ++iter)
        seq.insert(*iter);
      return *(seq.atPos(std::rand() % seq.size()));
    }

    const Attribute& pickAttribute(const Instance& i) {
      Sequence<const Attribute*> seq;
      for (Instance::const_iterator iter = i.begin(); iter != i.end(); ++iter)
        seq.insert(*iter);
      return *(seq.atPos(std::rand() % seq.size()));
    }

    void testLayerGenerator() {
      LayerGenerator* gen = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(gen = new LayerGenerator());
      gen->setDomainSize(6);
      gen->setMaxParents(5);
      std::vector<LayerGenerator::LayerData> v;
      generateLayerLayer(v, 10);
      gen->setLayers(v);
      PRM* prm = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = gen->generate());
      // testing interfaces
      const Set<Interface*>& i_set = prm->interfaces();
      TS_ASSERT_EQUALS(i_set.size(), (Size) 10);
      for (Set<Interface*>::const_iterator iter = i_set.begin(); iter != i_set.end(); ++iter) {
        const Interface& i = **iter;
        if (i.referenceSlots().size()) {
          TS_ASSERT_EQUALS(i.referenceSlots().size(), (Size) 1);
          TS_ASSERT_EQUALS(i.attributes().size(), (Size) 32);
        } else {
          TS_ASSERT_EQUALS(i.referenceSlots().size(), (Size) 0);
          TS_ASSERT_EQUALS(i.attributes().size(), (Size) 30);
        }
        Size six = 0;
        Size two = 0;
        const Set<Attribute*>& attr = i.attributes();
        for (Set<Attribute*>::const_iterator a = attr.begin(); a != attr.end(); ++a) {
          if ((**a).type()->domainSize() == (Size) 6) {
            ++six;
          } else if ((**a).type()->domainSize() == 2) {
            ++two;
          } else {
            TS_ASSERT(false);
          }
        }
        TS_ASSERT_EQUALS(six, (Size) 30);
        if (i.referenceSlots().size()) {
          TS_ASSERT_EQUALS(two, (Size) 2);
        }
      }
      // testing classes
      const Set<Class*>& c_set = prm->classes();
      for (Set<Class*>::const_iterator c = c_set.begin(); c != c_set.end(); ++c) {
        TS_ASSERT_EQUALS((**c).attributes().size(), (Size) 30);
        for (Set<Attribute*>::const_iterator a = (**c).attributes().begin(); a != (**c).attributes().end(); ++a) {
          TS_ASSERT((**c).dag().parents((**a).id()).size() < 6);
        }
      }
      // testing instances
      const System& sys = **(prm->systems().begin());
      TS_ASSERT_EQUALS(sys.size(), (Size) 100);
      if (prm)
        delete prm;
      if (gen) {
        TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
      }
    }

    void testClusterGenerator() {
      ClusteredLayerGenerator* gen = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(gen = new ClusteredLayerGenerator());
      gen->setDomainSize(6);
      gen->setMaxParents(5);
      gen->setClusterRatio(1.0);
      std::vector<LayerGenerator::LayerData> v;
      generateLayerLayer(v, 10);
      gen->setLayers(v);
      PRM* prm = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = gen->generate());
      // testing interfaces
      const Set<Interface*>& i_set = prm->interfaces();
      TS_ASSERT_EQUALS(i_set.size(), (Size) 10);
      for (Set<Interface*>::const_iterator iter = i_set.begin(); iter != i_set.end(); ++iter) {
        const Interface& i = **iter;
        if (i.referenceSlots().size()) {
          TS_ASSERT_EQUALS(i.referenceSlots().size(), (Size) 1);
          TS_ASSERT_EQUALS(i.attributes().size(), (Size) 32);
        } else {
          TS_ASSERT_EQUALS(i.referenceSlots().size(), (Size) 0);
          TS_ASSERT_EQUALS(i.attributes().size(), (Size) 30);
        }
        Size six = 0;
        Size two = 0;
        const Set<Attribute*>& attr = i.attributes();
        for (Set<Attribute*>::const_iterator a = attr.begin(); a != attr.end(); ++a) {
          if ((**a).type()->domainSize() == (Size) 6) {
            ++six;
          } else if ((**a).type()->domainSize() == 2) {
            ++two;
          } else {
            TS_ASSERT(false);
          }
        }
        TS_ASSERT_EQUALS(six, (Size) 30);
        if (i.referenceSlots().size()) {
          TS_ASSERT_EQUALS(two, (Size) 2);
        }
      }
      // testing classes
      const Set<Class*>& c_set = prm->classes();
      for (Set<Class*>::const_iterator c = c_set.begin(); c != c_set.end(); ++c) {
        TS_ASSERT_EQUALS((**c).attributes().size(), (Size) 30);
        for (Set<Attribute*>::const_iterator a = (**c).attributes().begin(); a != (**c).attributes().end(); ++a) {
          TS_ASSERT((**c).dag().parents((**a).id()).size() < 6);
        }
      }
      // testing instances
      const System& sys = **(prm->systems().begin());
      TS_ASSERT(sys.size() > (Size) 100);

      if (prm)
        delete prm;
      if (gen) {
        TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
      }
    }

    void __generateLayer1(std::vector<LayerGenerator::LayerData>& v) {
      for (size_t lvl = 0; lvl < 5; ++lvl) {
        v.push_back(LayerGenerator::LayerData());
        v[lvl].a = 10;
        v[lvl].g = 2;
        v[lvl].c = 1;
        v[lvl].o = 10;
        v[lvl].inner_density = 0.2;
        v[lvl].outter_density = 0.05;
      }
    }

};

} // namespace tests
} // namespace gum
// ============================================================================
