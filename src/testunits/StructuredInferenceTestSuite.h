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
#include <agrum/BN/inference/VEWithBB.h>
// ============================================================================
#include <agrum/prm/groundedInference.h>
#include <agrum/prm/structuredInference.h>
// ============================================================================
#include <agrum/prm/generator/layerGenerator.h>
// ============================================================================
#include <agrum/prm/gspan/DFSTree.h>
// ============================================================================
#include <agrum/prm/skool/SkoolReader.h>
// ============================================================================

namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;
using namespace gum::prm::skool;

class StructuredInferenceTestSuite: public CxxTest::TestSuite {
  public:
    void setUp() {
      //std::cerr << std::endl;
    }

    void generateLayer(std::vector<LayerGenerator::LayerData>& v, size_t layer_count)
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

    void testStructWithLayerGeneration() {
      std::vector<LayerGenerator::LayerData> layers;
      generateLayer(layers, 5);
      LayerGenerator generator;
      generator.setLayers(layers);
      generator.setDomainSize(2);
      generator.setMaxParents(5);
      PRM* prm = generator.generate();
      System& sys = prm->getSystem((**(prm->systems().begin())).name());
      StructuredInference inf(*prm, sys, 2, 2, new gspan::StrictSearch());
      inf.setPatternMining(false);
      const Instance& i = pickInstance(sys);
      const Attribute& a = pickAttribute(i);
      PRMInference::Chain chain = std::make_pair(&i, &a);
      Potential<prm_float> m;
      TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      prm_float sum = 0.0;
      Instantiation inst(m);
      for (inst.setFirst(); not inst.end(); inst.inc())
        sum += m.get(inst);
      TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      delete prm;
    }

    void testStrictWithLayerGeneration() {
      std::vector<LayerGenerator::LayerData> layers;
      generateLayer(layers, 5);
      LayerGenerator generator;
      generator.setLayers(layers);
      generator.setDomainSize(2);
      generator.setMaxParents(5);
      PRM* prm = generator.generate();
      System& sys = prm->getSystem((**(prm->systems().begin())).name());
      StructuredInference inf(*prm, sys, 2, 2, new gspan::StrictSearch());
      inf.setPatternMining(true);
      const Instance& i = pickInstance(sys);
      const Attribute& a = pickAttribute(i);
      PRMInference::Chain chain = std::make_pair(&i, &a);
      Potential<prm_float> m;
      TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      prm_float sum = 0.0;
      Instantiation inst(m);
      for (inst.setFirst(); not inst.end(); inst.inc())
        sum += m.get(inst);
      TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      delete prm;
    }

    void testFreqWithLayerGeneration() {
      std::vector<LayerGenerator::LayerData> layers;
      generateLayer(layers, 5);
      LayerGenerator generator;
      generator.setLayers(layers);
      generator.setDomainSize(2);
      generator.setMaxParents(5);
      PRM* prm = generator.generate();
      System& sys = prm->getSystem((**(prm->systems().begin())).name());
      StructuredInference inf(*prm, sys, 2, 2, new gspan::FrequenceSearch(2));
      inf.setPatternMining(true);
      const Instance& i = pickInstance(sys);
      const Attribute& a = pickAttribute(i);
      PRMInference::Chain chain = std::make_pair(&i, &a);
      Potential<prm_float> m;
      TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      prm_float sum = 0.0;
      Instantiation inst(m);
      for (inst.setFirst(); not inst.end(); inst.inc())
        sum += m.get(inst);
      TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      delete prm;
    }

    // void testCompareSpeed() {
    //   std::cerr << std::endl;
    //   GUM_CHECKPOINT;
    //   Timer timer;
    //   std::vector<LayerGenerator::LayerData> layers;
    //   generateLayer(layers, 6, 1);
    //   GUM_CHECKPOINT;
    //   LayerGenerator generator;
    //   generator.setLayers(layers);
    //   generator.setDomainSize(6);
    //   GUM_TRACE_VAR(std::pow(6,5));
    //   generator.setMaxParents(5);
    //   GUM_CHECKPOINT;
    //   PRM* prm = 0;
    //   TS_GUM_ASSERT_THROWS_NOTHING(generator.generate());
    //   GUM_CHECKPOINT;
    //   System& sys = prm->getSystem((**(prm->systems().begin())).name());
    //   const Instance& i = pickInstance(sys);
    //   const Attribute& a = pickAttribute(i);
    //   PRMInference::Chain chain = std::make_pair(&i, &a);
    //   GUM_CHECKPOINT;
    //   {
    //     timer.reset();
    //     Potential<prm_float> m;
    //     StructuredInference inf(*prm, sys, 2, 2, new gspan::StrictSearch());
    //     inf.setPatternMining(false);
    //     TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
    //     prm_float sum = 0.0;
    //     Instantiation inst(m);
    //     for (inst.setFirst(); not inst.end(); inst.inc())
    //       sum += m.get(inst);
    //     TS_ASSERT_DELTA(sum, 1.0, 1e-6);
    //     GUM_TRACE(timer.step());
    //   }
    //   {
    //     timer.reset();
    //     Potential<prm_float> m;
    //     StructuredInference inf(*prm, sys, 2, 2, new gspan::StrictSearch());
    //     inf.setPatternMining(true);
    //     TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
    //     prm_float sum = 0.0;
    //     Instantiation inst(m);
    //     for (inst.setFirst(); not inst.end(); inst.inc())
    //       sum += m.get(inst);
    //     TS_ASSERT_DELTA(sum, 1.0, 1e-6);
    //     GUM_TRACE(timer.step());
    //   }
    //   {
    //     timer.reset();
    //     Potential<prm_float> m;
    //     StructuredInference inf(*prm, sys, 2, 2, new gspan::FrequenceSearch(2));
    //     inf.setPatternMining(true);
    //     TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
    //     prm_float sum = 0.0;
    //     Instantiation inst(m);
    //     for (inst.setFirst(); not inst.end(); inst.inc())
    //       sum += m.get(inst);
    //     TS_ASSERT_DELTA(sum, 1.0, 1e-6);
    //     GUM_TRACE(timer.step());
    //   }
    //   delete prm;
    // }

};

} // namespace tests
} // namespace gum
// ============================================================================

