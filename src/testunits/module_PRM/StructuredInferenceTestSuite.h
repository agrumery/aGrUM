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

#include <agrum/BN/inference/VEWithBB.h>

#include <agrum/PRM/inference/groundedInference.h>
#include <agrum/PRM/inference/structuredInference.h>

#include <agrum/PRM/generator/layerGenerator.h>
#include <agrum/PRM/generator/clusteredLayerGenerator.h>

#include <agrum/PRM/gspan/DFSTree.h>

#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {

  class StructuredInferenceTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {
      // std::cerr << std::endl;
    }

    void
    generateLayer1(gum::Size nb_class, gum::Size depth,
                   std::vector<gum::prm::LayerGenerator<double>::LayerData> &v) {
      for (gum::Size lvl = 0; lvl < depth; ++lvl) {
        v.push_back(gum::prm::LayerGenerator<double>::LayerData());
        v[lvl].a = 10;
        v[lvl].g = 2;
        v[lvl].c = nb_class;
        v[lvl].o = 20;
        v[lvl].inner_density = 0.2;
        v[lvl].outter_density = 0.05;
      }
    }

    void
    generateLayer2(gum::Size nb_class, gum::Size depth,
                   std::vector<gum::prm::LayerGenerator<double>::LayerData> &v) {
      for (gum::Size lvl = 0; lvl < depth; ++lvl) {
        v.push_back(gum::prm::LayerGenerator<double>::LayerData());
        v[lvl].a = 10;
        v[lvl].g = 2;
        v[lvl].c = nb_class;
        v[lvl].o = 1 + lvl;
        v[lvl].inner_density = 0.2;
        v[lvl].outter_density = 0.05;
      }
    }

    void
    generateLayer3(gum::Size nb_class, gum::Size depth,
                   std::vector<gum::prm::LayerGenerator<double>::LayerData> &v) {
      for (gum::Size lvl = 0; lvl < depth; ++lvl) {
        v.push_back(gum::prm::LayerGenerator<double>::LayerData());
        v[lvl].a = 10;
        v[lvl].g = 2;
        v[lvl].c = nb_class;
        v[lvl].o = depth - lvl;
        v[lvl].inner_density = 0.2;
        v[lvl].outter_density = 0.05;
      }
    }

    const gum::prm::Instance<double> &
    pickInstance(const gum::prm::System<double> &sys) {
      gum::Sequence<const gum::prm::Instance<double> *> seq;

      for (gum::prm::System<double>::const_iterator iter = sys.begin();
           iter != sys.end(); ++iter)
        seq.insert(iter.val());

      return *(seq.atPos(std::rand() % seq.size()));
    }

    const gum::prm::Attribute<double> &
    pickAttribute(const gum::prm::Instance<double> &i) {
      gum::Sequence<const gum::prm::Attribute<double> *> seq;

      for (gum::prm::Instance<double>::const_iterator iter = i.begin();
           iter != i.end(); ++iter)
        seq.insert(iter.val());

      return *(seq.atPos(std::rand() % seq.size()));
    }

    void testStructuredInference_gen1() {
      std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
      generateLayer1(5, 2, layers);
      gum::prm::ClusteredLayerGenerator<double> generator;
      generator.setClusterRatio(1.0);
      generator.setLayers(layers);
      generator.setDomainSize(2);
      generator.setMaxParents(5);
      gum::prm::PRM<double> *prm = generator.generate();
      gum::prm::System<double> &sys =
          prm->system((**(prm->systems().begin())).name());
      gum::prm::StructuredInference<double> inf(*prm, sys);
      inf.setPatternMining(false);
      const gum::prm::Instance<double> &i = pickInstance(sys);
      const gum::prm::Attribute<double> &a = pickAttribute(i);
      gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
      gum::Potential<double> m;
      TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      double sum = 0.0;
      gum::Instantiation inst(m);

      for (inst.setFirst(); not inst.end(); inst.inc())
        sum += m.get(inst);

      TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      delete prm;
    }

    void testStructuredInference_gen2() {
      std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
      generateLayer2(5, 2, layers);
      gum::prm::ClusteredLayerGenerator<double> generator;
      generator.setClusterRatio(1.0);
      generator.setLayers(layers);
      generator.setDomainSize(2);
      generator.setMaxParents(5);
      gum::prm::PRM<double> *prm = generator.generate();
      gum::prm::System<double> &sys =
          prm->system((**(prm->systems().begin())).name());
      gum::prm::StructuredInference<double> inf(*prm, sys);
      inf.setPatternMining(false);
      const gum::prm::Instance<double> &i = pickInstance(sys);
      const gum::prm::Attribute<double> &a = pickAttribute(i);
      gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
      gum::Potential<double> m;
      TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      double sum = 0.0;
      gum::Instantiation inst(m);

      for (inst.setFirst(); not inst.end(); inst.inc())
        sum += m.get(inst);

      TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      delete prm;
    }

    void testStructuredInference_gen3() {
      std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
      generateLayer3(5, 2, layers);
      gum::prm::ClusteredLayerGenerator<double> generator;
      generator.setClusterRatio(1.0);
      generator.setLayers(layers);
      generator.setDomainSize(2);
      generator.setMaxParents(5);
      gum::prm::PRM<double> *prm = generator.generate();
      gum::prm::System<double> &sys =
          prm->system((**(prm->systems().begin())).name());
      gum::prm::StructuredInference<double> inf(*prm, sys);
      inf.setPatternMining(false);
      const gum::prm::Instance<double> &i = pickInstance(sys);
      const gum::prm::Attribute<double> &a = pickAttribute(i);
      gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
      gum::Potential<double> m;
      TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      double sum = 0.0;
      gum::Instantiation inst(m);

      for (inst.setFirst(); not inst.end(); inst.inc())
        sum += m.get(inst);

      TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      delete prm;
    }

    void testFrequenceSearch_gen1() {
      std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
      generateLayer1(5, 2, layers);
      gum::prm::ClusteredLayerGenerator<double> generator;
      generator.setClusterRatio(1.0);
      generator.setLayers(layers);
      generator.setDomainSize(2);
      generator.setMaxParents(5);

      gum::prm::PRM<double> *prm = generator.generate();
      gum::prm::System<double> &sys =
          prm->system((**(prm->systems().begin())).name());

      gum::prm::gspan::FrequenceSearch<double> *search =
          new gum::prm::gspan::FrequenceSearch<double>((gum::Size)2);
      gum::prm::StructuredInference<double> inf(*prm, sys, search);

      inf.setPatternMining(true);
      const gum::prm::Instance<double> &i = pickInstance(sys);
      const gum::prm::Attribute<double> &a = pickAttribute(i);
      gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
      gum::Potential<double> m;
      TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      double sum = 0.0;
      gum::Instantiation inst(m);

      for (inst.setFirst(); not inst.end(); inst.inc())
        sum += m.get(inst);

      TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      delete prm;
    }

    // void testFrequenceSearch_gen2() {
    //   std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
    //   generateLayer2(5, 2, layers);
    //   Clusteredgum::prm::LayerGenerator<double> generator;
    //   generator.setClusterRatio(1.0);
    //   generator.setLayers(layers);
    //   generator.setDomainSize(2);
    //   generator.setMaxParents(5);
    //   gum::prm::PRM<double>* prm = generator.generate();
    //   gum::prm::System<double>& sys =
    //   prm->system((**(prm->systems().begin())).name());
    //   gum::prm::StructuredInference<double> inf(*prm, sys, new
    //   gspan::FrequenceSearch(2));
    //   inf.setPatternMining(true);
    //   const gum::prm::Instance<double>& i = pickgum::prm::Instance<double>(sys);
    //   const gum::prm::Attribute<double>& a = pickAttribute(i);
    //   gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
    //   gum::Potential<double> m;
    //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
    //   double sum = 0.0;
    //   gum::Instantiation inst(m);
    //   for (inst.setFirst(); not inst.end(); inst.inc())
    //     sum += m.get(inst);
    //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
    //   delete prm;
    // }

    // void testFrequenceSearch_gen3() {
    //   std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
    //   generateLayer3(5, 2, layers);
    //   Clusteredgum::prm::LayerGenerator<double> generator;
    //   generator.setClusterRatio(1.0);
    //   generator.setLayers(layers);
    //   generator.setDomainSize(2);
    //   generator.setMaxParents(5);
    //   gum::prm::PRM<double>* prm = generator.generate();
    //   gum::prm::System<double>& sys =
    //   prm->system((**(prm->systems().begin())).name());
    //   gum::prm::StructuredInference<double> inf(*prm, sys, new
    //   gspan::FrequenceSearch(2));
    //   inf.setPatternMining(true);
    //   const gum::prm::Instance<double>& i = pickgum::prm::Instance<double>(sys);
    //   const gum::prm::Attribute<double>& a = pickAttribute(i);
    //   gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
    //   gum::Potential<double> m;
    //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
    //   double sum = 0.0;
    //   gum::Instantiation inst(m);
    //   for (inst.setFirst(); not inst.end(); inst.inc())
    //     sum += m.get(inst);
    //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
    //   delete prm;
    // }

    // void testStrictSearch_gen1() {
    //   std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
    //   generateLayer1(5, 2, layers);
    //   Clusteredgum::prm::LayerGenerator<double> generator;
    //   generator.setClusterRatio(1.0);
    //   generator.setLayers(layers);
    //   generator.setDomainSize(2);
    //   generator.setMaxParents(5);
    //   gum::prm::PRM<double>* prm = generator.generate();
    //   gum::prm::System<double>& sys =
    //   prm->system((**(prm->systems().begin())).name());
    //   gum::prm::StructuredInference<double> inf(*prm, sys, new
    //   gspan::StrictSearch(2));
    //   inf.setPatternMining(true);
    //   const gum::prm::Instance<double>& i = pickgum::prm::Instance<double>(sys);
    //   const gum::prm::Attribute<double>& a = pickAttribute(i);
    //   gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
    //   gum::Potential<double> m;
    //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
    //   double sum = 0.0;
    //   gum::Instantiation inst(m);
    //   for (inst.setFirst(); not inst.end(); inst.inc())
    //     sum += m.get(inst);
    //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
    //   delete prm;
    // }

    // void testStrictSearch_gen2() {
    //   std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
    //   generateLayer2(5, 2, layers);
    //   Clusteredgum::prm::LayerGenerator<double> generator;
    //   generator.setClusterRatio(1.0);
    //   generator.setLayers(layers);
    //   generator.setDomainSize(2);
    //   generator.setMaxParents(5);
    //   gum::prm::PRM<double>* prm = generator.generate();
    //   gum::prm::System<double>& sys =
    //   prm->system((**(prm->systems().begin())).name());
    //   gum::prm::StructuredInference<double> inf(*prm, sys, new
    //   gspan::StrictSearch(2));
    //   inf.setPatternMining(true);
    //   const gum::prm::Instance<double>& i = pickgum::prm::Instance<double>(sys);
    //   const gum::prm::Attribute<double>& a = pickAttribute(i);
    //   gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
    //   gum::Potential<double> m;
    //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
    //   double sum = 0.0;
    //   gum::Instantiation inst(m);
    //   for (inst.setFirst(); not inst.end(); inst.inc())
    //     sum += m.get(inst);
    //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
    //   delete prm;
    // }

    // void testStrictStrategy_gen3() {
    //   std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
    //   generateLayer3(5, 2, layers);
    //   Clusteredgum::prm::LayerGenerator<double> generator;
    //   generator.setClusterRatio(1.0);
    //   generator.setLayers(layers);
    //   generator.setDomainSize(2);
    //   generator.setMaxParents(5);
    //   gum::prm::PRM<double>* prm = generator.generate();
    //   gum::prm::System<double>& sys =
    //   prm->system((**(prm->systems().begin())).name());
    //   gum::prm::StructuredInference<double> inf(*prm, sys, new
    //   gspan::StrictSearch(2));
    //   inf.setPatternMining(true);
    //   const gum::prm::Instance<double>& i = pickgum::prm::Instance<double>(sys);
    //   const gum::prm::Attribute<double>& a = pickAttribute(i);
    //   gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
    //   gum::Potential<double> m;
    //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
    //   double sum = 0.0;
    //   gum::Instantiation inst(m);
    //   for (inst.setFirst(); not inst.end(); inst.inc())
    //     sum += m.get(inst);
    //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
    //   delete prm;
    // }

    // void testTreeWidthSearch_gen1() {
    //   std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
    //   generateLayer1(5, 2, layers);
    //   Clusteredgum::prm::LayerGenerator<double> generator;
    //   generator.setClusterRatio(1.0);
    //   generator.setLayers(layers);
    //   generator.setDomainSize(2);
    //   generator.setMaxParents(5);
    //   gum::prm::PRM<double>* prm = generator.generate();
    //   gum::prm::System<double>& sys =
    //   prm->system((**(prm->systems().begin())).name());
    //   gum::prm::StructuredInference<double> inf(*prm, sys, new
    //   gspan::TreeWidthSearch());
    //   inf.setPatternMining(true);
    //   const gum::prm::Instance<double>& i = pickgum::prm::Instance<double>(sys);
    //   const gum::prm::Attribute<double>& a = pickAttribute(i);
    //   gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
    //   gum::Potential<double> m;
    //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
    //   double sum = 0.0;
    //   gum::Instantiation inst(m);
    //   for (inst.setFirst(); not inst.end(); inst.inc())
    //     sum += m.get(inst);
    //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
    //   delete prm;
    // }

    // void testTreeWidthSearch_gen2() {
    //   std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
    //   generateLayer2(5, 2, layers);
    //   Clusteredgum::prm::LayerGenerator<double> generator;
    //   generator.setClusterRatio(1.0);
    //   generator.setLayers(layers);
    //   generator.setDomainSize(2);
    //   generator.setMaxParents(5);
    //   gum::prm::PRM<double>* prm = generator.generate();
    //   gum::prm::System<double>& sys =
    //   prm->system((**(prm->systems().begin())).name());
    //   gum::prm::StructuredInference<double> inf(*prm, sys, new
    //   gspan::TreeWidthSearch());
    //   inf.setPatternMining(true);
    //   const gum::prm::Instance<double>& i = pickgum::prm::Instance<double>(sys);
    //   const gum::prm::Attribute<double>& a = pickAttribute(i);
    //   gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
    //   gum::Potential<double> m;
    //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
    //   double sum = 0.0;
    //   gum::Instantiation inst(m);
    //   for (inst.setFirst(); not inst.end(); inst.inc())
    //     sum += m.get(inst);
    //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
    //   delete prm;
    // }

    // void testTreeWidthSearch_gen3() {
    //   std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
    //   generateLayer3(5, 2, layers);
    //   Clusteredgum::prm::LayerGenerator<double> generator;
    //   generator.setClusterRatio(1.0);
    //   generator.setLayers(layers);
    //   generator.setDomainSize(2);
    //   generator.setMaxParents(5);
    //   gum::prm::PRM<double>* prm = generator.generate();
    //   gum::prm::System<double>& sys =
    //   prm->system((**(prm->systems().begin())).name());
    //   gum::prm::StructuredInference<double> inf(*prm, sys, new
    //   gspan::TreeWidthSearch());
    //   inf.setPatternMining(true);
    //   const gum::prm::Instance<double>& i = pickgum::prm::Instance<double>(sys);
    //   const gum::prm::Attribute<double>& a = pickAttribute(i);
    //   gum::prm::PRMInference<double>::Chain chain = std::make_pair(&i, &a);
    //   gum::Potential<double> m;
    //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
    //   double sum = 0.0;
    //   gum::Instantiation inst(m);
    //   for (inst.setFirst(); not inst.end(); inst.inc())
    //     sum += m.get(inst);
    //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
    //   delete prm;
    // }
  };

} // namespace gum_tests
