/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/PRM/inference/groundedInference.h>
#include <agrum/PRM/inference/structuredInference.h>

#include <agrum/PRM/generator/clusteredLayerGenerator.h>
#include <agrum/PRM/generator/layerGenerator.h>

#include <agrum/PRM/gspan/DFSTree.h>

#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum_tests {

  class StructuredInferenceTestSuite: public CxxTest::TestSuite {
    public:
    void setUp() {}

    void generateLayer1(
       gum::Size                                                     nb_class,
       gum::Size                                                     depth,
       std::vector< gum::prm::LayerGenerator< double >::LayerData >& v) {
      try {
        for (gum::Size lvl = 0; lvl < depth; ++lvl) {
          v.push_back(gum::prm::LayerGenerator< double >::LayerData());
          v[lvl].a              = 10;
          v[lvl].g              = 2;
          v[lvl].c              = nb_class;
          v[lvl].o              = 20;
          v[lvl].inner_density  = 0.2f;
          v[lvl].outter_density = 0.05f;
        }
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void generateLayer2(
       gum::Size                                                     nb_class,
       gum::Size                                                     depth,
       std::vector< gum::prm::LayerGenerator< double >::LayerData >& v) {
      try {
        for (gum::Size lvl = 0; lvl < depth; ++lvl) {
          v.push_back(gum::prm::LayerGenerator< double >::LayerData());
          v[lvl].a              = 10;
          v[lvl].g              = 2;
          v[lvl].c              = nb_class;
          v[lvl].o              = 1 + lvl;
          v[lvl].inner_density  = 0.2f;
          v[lvl].outter_density = 0.05f;
        }
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void generateLayer3(
       gum::Size                                                     nb_class,
       gum::Size                                                     depth,
       std::vector< gum::prm::LayerGenerator< double >::LayerData >& v) {
      try {
        for (gum::Size lvl = 0; lvl < depth; ++lvl) {
          v.push_back(gum::prm::LayerGenerator< double >::LayerData());
          v[lvl].a              = 10;
          v[lvl].g              = 2;
          v[lvl].c              = nb_class;
          v[lvl].o              = depth - lvl;
          v[lvl].inner_density  = 0.2f;
          v[lvl].outter_density = 0.05f;
        }
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    const gum::prm::PRMInstance< double >&
       pickInstance(const gum::prm::PRMSystem< double >& sys) {
      gum::Sequence< const gum::prm::PRMInstance< double >* > seq;

      for (gum::prm::PRMSystem< double >::const_iterator iter = sys.begin();
           iter != sys.end();
           ++iter)
        seq.insert(iter.val());

      return *(seq.atPos(std::rand() % seq.size()));
    }

    const gum::prm::PRMAttribute< double >&
       pickAttribute(const gum::prm::PRMInstance< double >& i) {
      gum::Sequence< const gum::prm::PRMAttribute< double >* > seq;

      for (gum::prm::PRMInstance< double >::const_iterator iter = i.begin();
           iter != i.end();
           ++iter)
        seq.insert(iter.val());

      return *(seq.atPos(std::rand() % seq.size()));
    }

    void testStructuredInference_gen1() {
      try {
        std::vector< gum::prm::LayerGenerator< double >::LayerData > layers;
        generateLayer1(5, 2, layers);
        gum::prm::ClusteredLayerGenerator< double > generator;
        generator.setClusterRatio(1.0);
        generator.setLayers(layers);
        generator.setDomainSize(2);
        generator.setMaxParents(5);
        gum::prm::PRM< double >*       prm = generator.generate();
        gum::prm::PRMSystem< double >& sys
           = prm->getSystem((**(prm->systems().begin())).name());
        gum::prm::StructuredInference< double > inf(*prm, sys);
        inf.setPatternMining(false);
        const gum::prm::PRMInstance< double >&  i     = pickInstance(sys);
        const gum::prm::PRMAttribute< double >& a     = pickAttribute(i);
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&i, &a);
        gum::Potential< double >                m;
        TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(chain, m));
        double             sum = 0.0;
        gum::Instantiation inst(m);

        for (inst.setFirst(); !inst.end(); inst.inc())
          sum += m.get(inst);

        TS_ASSERT_DELTA(sum, 1.0, 1e-6);
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testStructuredInference_gen2() {
      try {
        std::vector< gum::prm::LayerGenerator< double >::LayerData > layers;
        generateLayer2(5, 2, layers);
        gum::prm::ClusteredLayerGenerator< double > generator;
        generator.setClusterRatio(1.0);
        generator.setLayers(layers);
        generator.setDomainSize(2);
        generator.setMaxParents(5);
        gum::prm::PRM< double >*       prm = generator.generate();
        gum::prm::PRMSystem< double >& sys
           = prm->getSystem((**(prm->systems().begin())).name());
        gum::prm::StructuredInference< double > inf(*prm, sys);
        inf.setPatternMining(false);
        const gum::prm::PRMInstance< double >&  i     = pickInstance(sys);
        const gum::prm::PRMAttribute< double >& a     = pickAttribute(i);
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&i, &a);
        gum::Potential< double >                m;
        TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(chain, m));
        double             sum = 0.0;
        gum::Instantiation inst(m);

        for (inst.setFirst(); !inst.end(); inst.inc())
          sum += m.get(inst);

        TS_ASSERT_DELTA(sum, 1.0, 1e-6);
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    void testStructuredInference_gen3() {
      try {
        std::vector< gum::prm::LayerGenerator< double >::LayerData > layers;
        generateLayer3(5, 2, layers);
        gum::prm::ClusteredLayerGenerator< double > generator;
        generator.setClusterRatio(1.0);
        generator.setLayers(layers);
        generator.setDomainSize(2);
        generator.setMaxParents(5);
        gum::prm::PRM< double >*       prm = generator.generate();
        gum::prm::PRMSystem< double >& sys
           = prm->getSystem((**(prm->systems().begin())).name());
        gum::prm::StructuredInference< double > inf(*prm, sys);
        inf.setPatternMining(false);
        const gum::prm::PRMInstance< double >&  i     = pickInstance(sys);
        const gum::prm::PRMAttribute< double >& a     = pickAttribute(i);
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&i, &a);
        gum::Potential< double >                m;
        TS_GUM_ASSERT_THROWS_NOTHING(inf.posterior(chain, m));
        double             sum = 0.0;
        gum::Instantiation inst(m);

        for (inst.setFirst(); !inst.end(); inst.inc())
          sum += m.get(inst);

        TS_ASSERT_DELTA(sum, 1.0, 1e-6);
        delete prm;
      } catch (gum::Exception&) { TS_ASSERT(false); }
    }

    // void testFrequenceSearch_gen1() {
    //  try {
    //    std::vector<gum::prm::LayerGenerator<double>::LayerData> layers;
    //    generateLayer1( 5, 2, layers );
    //    gum::prm::ClusteredLayerGenerator<double> generator;
    //    generator.setClusterRatio( 1.0 );
    //    generator.setLayers( layers );
    //    generator.setDomainSize( 2 );
    //    generator.setMaxParents( 5 );

    //    gum::prm::PRM<double>* prm = generator.generate();
    //    gum::prm::PRMSystem<double>& sys = prm->getSystem( ( **(
    //    prm->systems().begin() ) ).name() );

    //    gum::prm::gspan::FrequenceSearch<double>* search = new
    //    gum::prm::gspan::FrequenceSearch<double>( ( gum::Size ) 2 );
    //    gum::prm::StructuredInference<double> inf( *prm, sys, search );

    //    inf.setPatternMining( true );
    //    const gum::prm::PRMInstance<double>& i = pickInstance( sys );
    //    const gum::prm::PRMAttribute<double>& a = pickAttribute( i );
    //    gum::prm::PRMInference<double>::Chain chain = std::make_pair( &i, &a
    //    );
    //    gum::Potential<double> m;
    //    TS_GUM_ASSERT_THROWS_NOTHING( inf.posterior( chain, m ) );
    //    double sum = 0.0;
    //    gum::Instantiation inst( m );

    //    for( inst.setFirst(); not inst.end(); inst.inc() )
    //      sum += m.get( inst );

    //    TS_ASSERT_DELTA( sum, 1.0, 1e-6 );
    //    delete prm;
    //  } catch (gum::Exception&) {
    //    TS_ASSERT(false);
    //  }
    //}
  };

}   // namespace gum_tests
