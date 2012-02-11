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
#include <agrum/prm/generator/clusteredLayerGenerator.h>
// ============================================================================
#include <agrum/prm/gspan/DFSTree.h>
// ============================================================================
#include <agrum/prm/skool/SkoolReader.h>
// ============================================================================

namespace gum_tests {

// using namespace gum;
// using namespace gum::prm;
// using namespace gum::prm::skool;

  class StructuredInferenceTestSuite: public CxxTest::TestSuite {
    public:
      void setUp() {
        //std::cerr << std::endl;
      }

      void generateLayer1( size_t nb_class, size_t depth, std::vector<LayerGenerator::LayerData>& v ) {
        for( size_t lvl = 0; lvl < depth; ++lvl ) {
          v.push_back( LayerGenerator::LayerData() );
          v[lvl].a = 10;
          v[lvl].g = 2;
          v[lvl].c = nb_class;
          v[lvl].o = 20;
          v[lvl].inner_density = 0.2;
          v[lvl].outter_density = 0.05;
        }
      }

      void generateLayer2( size_t nb_class, size_t depth, std::vector<LayerGenerator::LayerData>& v ) {
        for( size_t lvl = 0; lvl < depth; ++lvl ) {
          v.push_back( LayerGenerator::LayerData() );
          v[lvl].a = 10;
          v[lvl].g = 2;
          v[lvl].c = nb_class;
          v[lvl].o = 1 + lvl;
          v[lvl].inner_density = 0.2;
          v[lvl].outter_density = 0.05;
        }
      }

      void generateLayer3( size_t nb_class, size_t depth, std::vector<LayerGenerator::LayerData>& v ) {
        for( size_t lvl = 0; lvl < depth; ++lvl ) {
          v.push_back( LayerGenerator::LayerData() );
          v[lvl].a = 10;
          v[lvl].g = 2;
          v[lvl].c = nb_class;
          v[lvl].o = depth - lvl;
          v[lvl].inner_density = 0.2;
          v[lvl].outter_density = 0.05;
        }
      }

      const Instance& pickInstance( const System& sys ) {
        Sequence<const Instance*> seq;

        for( System::const_iterator iter = sys.begin(); iter != sys.end(); ++iter )
          seq.insert( *iter );

        return *( seq.atPos( std::rand() % seq.size() ) );
      }

      const Attribute& pickAttribute( const Instance& i ) {
        Sequence<const Attribute*> seq;

        for( Instance::const_iterator iter = i.begin(); iter != i.end(); ++iter )
          seq.insert( *iter );

        return *( seq.atPos( std::rand() % seq.size() ) );
      }

      void testStructuredInference_gen1() {
        std::vector<LayerGenerator::LayerData> layers;
        generateLayer1( 5, 2, layers );
        ClusteredLayerGenerator generator;
        generator.setClusterRatio( 1.0 );
        generator.setLayers( layers );
        generator.setDomainSize( 2 );
        generator.setMaxParents( 5 );
        PRM* prm = generator.generate();
        System& sys = prm->system( ( **( prm->systems().begin() ) ).name() );
        StructuredInference inf( *prm, sys );
        inf.setPatternMining( false );
        const Instance& i = pickInstance( sys );
        const Attribute& a = pickAttribute( i );
        PRMInference::Chain chain = std::make_pair( &i, &a );
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING( inf.marginal( chain, m ) );
        prm_float sum = 0.0;
        Instantiation inst( m );

        for( inst.setFirst(); not inst.end(); inst.inc() )
          sum += m.get( inst );

        TS_ASSERT_DELTA( sum, 1.0, 1e-6 );
        delete prm;
      }

      void testStructuredInference_gen2() {
        std::vector<LayerGenerator::LayerData> layers;
        generateLayer2( 5, 2, layers );
        ClusteredLayerGenerator generator;
        generator.setClusterRatio( 1.0 );
        generator.setLayers( layers );
        generator.setDomainSize( 2 );
        generator.setMaxParents( 5 );
        PRM* prm = generator.generate();
        System& sys = prm->system( ( **( prm->systems().begin() ) ).name() );
        StructuredInference inf( *prm, sys );
        inf.setPatternMining( false );
        const Instance& i = pickInstance( sys );
        const Attribute& a = pickAttribute( i );
        PRMInference::Chain chain = std::make_pair( &i, &a );
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING( inf.marginal( chain, m ) );
        prm_float sum = 0.0;
        Instantiation inst( m );

        for( inst.setFirst(); not inst.end(); inst.inc() )
          sum += m.get( inst );

        TS_ASSERT_DELTA( sum, 1.0, 1e-6 );
        delete prm;
      }

      void testStructuredInference_gen3() {
        std::vector<LayerGenerator::LayerData> layers;
        generateLayer3( 5, 2, layers );
        ClusteredLayerGenerator generator;
        generator.setClusterRatio( 1.0 );
        generator.setLayers( layers );
        generator.setDomainSize( 2 );
        generator.setMaxParents( 5 );
        PRM* prm = generator.generate();
        System& sys = prm->system( ( **( prm->systems().begin() ) ).name() );
        StructuredInference inf( *prm, sys );
        inf.setPatternMining( false );
        const Instance& i = pickInstance( sys );
        const Attribute& a = pickAttribute( i );
        PRMInference::Chain chain = std::make_pair( &i, &a );
        Potential<prm_float> m;
        TS_GUM_ASSERT_THROWS_NOTHING( inf.marginal( chain, m ) );
        prm_float sum = 0.0;
        Instantiation inst( m );

        for( inst.setFirst(); not inst.end(); inst.inc() )
          sum += m.get( inst );

        TS_ASSERT_DELTA( sum, 1.0, 1e-6 );
        delete prm;
      }

      // void testFrequenceSearch_gen1() {
      //   std::vector<LayerGenerator::LayerData> layers;
      //   generateLayer1(5, 2, layers);
      //   ClusteredLayerGenerator generator;
      //   generator.setClusterRatio(1.0);
      //   generator.setLayers(layers);
      //   generator.setDomainSize(2);
      //   generator.setMaxParents(5);
      //   PRM* prm = generator.generate();
      //   System& sys = prm->system((**(prm->systems().begin())).name());
      //   StructuredInference inf(*prm, sys, new gspan::FrequenceSearch(2));
      //   inf.setPatternMining(true);
      //   const Instance& i = pickInstance(sys);
      //   const Attribute& a = pickAttribute(i);
      //   PRMInference::Chain chain = std::make_pair(&i, &a);
      //   Potential<prm_float> m;
      //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      //   prm_float sum = 0.0;
      //   Instantiation inst(m);
      //   for (inst.setFirst(); not inst.end(); inst.inc())
      //     sum += m.get(inst);
      //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      //   delete prm;
      // }

      // void testFrequenceSearch_gen2() {
      //   std::vector<LayerGenerator::LayerData> layers;
      //   generateLayer2(5, 2, layers);
      //   ClusteredLayerGenerator generator;
      //   generator.setClusterRatio(1.0);
      //   generator.setLayers(layers);
      //   generator.setDomainSize(2);
      //   generator.setMaxParents(5);
      //   PRM* prm = generator.generate();
      //   System& sys = prm->system((**(prm->systems().begin())).name());
      //   StructuredInference inf(*prm, sys, new gspan::FrequenceSearch(2));
      //   inf.setPatternMining(true);
      //   const Instance& i = pickInstance(sys);
      //   const Attribute& a = pickAttribute(i);
      //   PRMInference::Chain chain = std::make_pair(&i, &a);
      //   Potential<prm_float> m;
      //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      //   prm_float sum = 0.0;
      //   Instantiation inst(m);
      //   for (inst.setFirst(); not inst.end(); inst.inc())
      //     sum += m.get(inst);
      //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      //   delete prm;
      // }

      // void testFrequenceSearch_gen3() {
      //   std::vector<LayerGenerator::LayerData> layers;
      //   generateLayer3(5, 2, layers);
      //   ClusteredLayerGenerator generator;
      //   generator.setClusterRatio(1.0);
      //   generator.setLayers(layers);
      //   generator.setDomainSize(2);
      //   generator.setMaxParents(5);
      //   PRM* prm = generator.generate();
      //   System& sys = prm->system((**(prm->systems().begin())).name());
      //   StructuredInference inf(*prm, sys, new gspan::FrequenceSearch(2));
      //   inf.setPatternMining(true);
      //   const Instance& i = pickInstance(sys);
      //   const Attribute& a = pickAttribute(i);
      //   PRMInference::Chain chain = std::make_pair(&i, &a);
      //   Potential<prm_float> m;
      //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      //   prm_float sum = 0.0;
      //   Instantiation inst(m);
      //   for (inst.setFirst(); not inst.end(); inst.inc())
      //     sum += m.get(inst);
      //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      //   delete prm;
      // }

      // void testStrictSearch_gen1() {
      //   std::vector<LayerGenerator::LayerData> layers;
      //   generateLayer1(5, 2, layers);
      //   ClusteredLayerGenerator generator;
      //   generator.setClusterRatio(1.0);
      //   generator.setLayers(layers);
      //   generator.setDomainSize(2);
      //   generator.setMaxParents(5);
      //   PRM* prm = generator.generate();
      //   System& sys = prm->system((**(prm->systems().begin())).name());
      //   StructuredInference inf(*prm, sys, new gspan::StrictSearch(2));
      //   inf.setPatternMining(true);
      //   const Instance& i = pickInstance(sys);
      //   const Attribute& a = pickAttribute(i);
      //   PRMInference::Chain chain = std::make_pair(&i, &a);
      //   Potential<prm_float> m;
      //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      //   prm_float sum = 0.0;
      //   Instantiation inst(m);
      //   for (inst.setFirst(); not inst.end(); inst.inc())
      //     sum += m.get(inst);
      //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      //   delete prm;
      // }

      // void testStrictSearch_gen2() {
      //   std::vector<LayerGenerator::LayerData> layers;
      //   generateLayer2(5, 2, layers);
      //   ClusteredLayerGenerator generator;
      //   generator.setClusterRatio(1.0);
      //   generator.setLayers(layers);
      //   generator.setDomainSize(2);
      //   generator.setMaxParents(5);
      //   PRM* prm = generator.generate();
      //   System& sys = prm->system((**(prm->systems().begin())).name());
      //   StructuredInference inf(*prm, sys, new gspan::StrictSearch(2));
      //   inf.setPatternMining(true);
      //   const Instance& i = pickInstance(sys);
      //   const Attribute& a = pickAttribute(i);
      //   PRMInference::Chain chain = std::make_pair(&i, &a);
      //   Potential<prm_float> m;
      //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      //   prm_float sum = 0.0;
      //   Instantiation inst(m);
      //   for (inst.setFirst(); not inst.end(); inst.inc())
      //     sum += m.get(inst);
      //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      //   delete prm;
      // }

      // void testStrictStrategy_gen3() {
      //   std::vector<LayerGenerator::LayerData> layers;
      //   generateLayer3(5, 2, layers);
      //   ClusteredLayerGenerator generator;
      //   generator.setClusterRatio(1.0);
      //   generator.setLayers(layers);
      //   generator.setDomainSize(2);
      //   generator.setMaxParents(5);
      //   PRM* prm = generator.generate();
      //   System& sys = prm->system((**(prm->systems().begin())).name());
      //   StructuredInference inf(*prm, sys, new gspan::StrictSearch(2));
      //   inf.setPatternMining(true);
      //   const Instance& i = pickInstance(sys);
      //   const Attribute& a = pickAttribute(i);
      //   PRMInference::Chain chain = std::make_pair(&i, &a);
      //   Potential<prm_float> m;
      //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      //   prm_float sum = 0.0;
      //   Instantiation inst(m);
      //   for (inst.setFirst(); not inst.end(); inst.inc())
      //     sum += m.get(inst);
      //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      //   delete prm;
      // }

      // void testTreeWidthSearch_gen1() {
      //   std::vector<LayerGenerator::LayerData> layers;
      //   generateLayer1(5, 2, layers);
      //   ClusteredLayerGenerator generator;
      //   generator.setClusterRatio(1.0);
      //   generator.setLayers(layers);
      //   generator.setDomainSize(2);
      //   generator.setMaxParents(5);
      //   PRM* prm = generator.generate();
      //   System& sys = prm->system((**(prm->systems().begin())).name());
      //   StructuredInference inf(*prm, sys, new gspan::TreeWidthSearch());
      //   inf.setPatternMining(true);
      //   const Instance& i = pickInstance(sys);
      //   const Attribute& a = pickAttribute(i);
      //   PRMInference::Chain chain = std::make_pair(&i, &a);
      //   Potential<prm_float> m;
      //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      //   prm_float sum = 0.0;
      //   Instantiation inst(m);
      //   for (inst.setFirst(); not inst.end(); inst.inc())
      //     sum += m.get(inst);
      //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      //   delete prm;
      // }

      // void testTreeWidthSearch_gen2() {
      //   std::vector<LayerGenerator::LayerData> layers;
      //   generateLayer2(5, 2, layers);
      //   ClusteredLayerGenerator generator;
      //   generator.setClusterRatio(1.0);
      //   generator.setLayers(layers);
      //   generator.setDomainSize(2);
      //   generator.setMaxParents(5);
      //   PRM* prm = generator.generate();
      //   System& sys = prm->system((**(prm->systems().begin())).name());
      //   StructuredInference inf(*prm, sys, new gspan::TreeWidthSearch());
      //   inf.setPatternMining(true);
      //   const Instance& i = pickInstance(sys);
      //   const Attribute& a = pickAttribute(i);
      //   PRMInference::Chain chain = std::make_pair(&i, &a);
      //   Potential<prm_float> m;
      //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      //   prm_float sum = 0.0;
      //   Instantiation inst(m);
      //   for (inst.setFirst(); not inst.end(); inst.inc())
      //     sum += m.get(inst);
      //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      //   delete prm;
      // }

      // void testTreeWidthSearch_gen3() {
      //   std::vector<LayerGenerator::LayerData> layers;
      //   generateLayer3(5, 2, layers);
      //   ClusteredLayerGenerator generator;
      //   generator.setClusterRatio(1.0);
      //   generator.setLayers(layers);
      //   generator.setDomainSize(2);
      //   generator.setMaxParents(5);
      //   PRM* prm = generator.generate();
      //   System& sys = prm->system((**(prm->systems().begin())).name());
      //   StructuredInference inf(*prm, sys, new gspan::TreeWidthSearch());
      //   inf.setPatternMining(true);
      //   const Instance& i = pickInstance(sys);
      //   const Attribute& a = pickAttribute(i);
      //   PRMInference::Chain chain = std::make_pair(&i, &a);
      //   Potential<prm_float> m;
      //   TS_GUM_ASSERT_THROWS_NOTHING(inf.marginal(chain, m));
      //   prm_float sum = 0.0;
      //   Instantiation inst(m);
      //   for (inst.setFirst(); not inst.end(); inst.inc())
      //     sum += m.get(inst);
      //   TS_ASSERT_DELTA(sum, 1.0, 1e-6);
      //   delete prm;
      // }

  };

} // namespace gum_tests
