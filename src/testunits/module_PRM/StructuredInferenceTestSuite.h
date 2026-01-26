/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/PRM/generator/clusteredLayerGenerator.h>
#include <agrum/PRM/generator/layerGenerator.h>
#include <agrum/PRM/gspan/DFSTree.h>
#include <agrum/PRM/inference/groundedInference.h>
#include <agrum/PRM/inference/structuredInference.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  StructuredInference
#define GUM_CURRENT_MODULE PRM

namespace gum_tests {

  struct StructuredInferenceTestSuite {
    public:
    void generateLayer1(gum::Size                                                     nb_class,
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
        }   // namespace gum_tests
      }

      catch (gum::Exception&) {
        CHECK(false);
      }
    }

    void generateLayer2(gum::Size                                                     nb_class,
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
      } catch (gum::Exception&) { CHECK(false); }
    }

    void generateLayer3(gum::Size                                                     nb_class,
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
      } catch (gum::Exception&) { CHECK(false); }
    }

    const gum::prm::PRMInstance< double >& pickInstance(const gum::prm::PRMSystem< double >& sys) {
      gum::Sequence< const gum::prm::PRMInstance< double >* > seq;

      for (gum::prm::PRMSystem< double >::const_iterator iter = sys.begin(); iter != sys.end();
           ++iter)
        seq.insert(iter.val());

      return *(seq.atPos(std::rand() % seq.size()));
    }

    const gum::prm::PRMAttribute< double >&
        pickAttribute(const gum::prm::PRMInstance< double >& i) {
      gum::Sequence< const gum::prm::PRMAttribute< double >* > seq;

      for (gum::prm::PRMInstance< double >::const_iterator iter = i.begin(); iter != i.end();
           ++iter)
        seq.insert(iter.val());

      return *(seq.atPos(std::rand() % seq.size()));
    }

    static void testStructuredInference_gen1() {
      try {
        std::vector< gum::prm::LayerGenerator< double >::LayerData > layers;
        generateLayer1(5, 2, layers);
        gum::prm::ClusteredLayerGenerator< double > generator;
        generator.setClusterRatio(1.0);
        generator.setLayers(layers);
        generator.setDomainSize(2);
        generator.setMaxParents(5);
        gum::prm::PRM< double >*       prm = generator.generate();
        gum::prm::PRMSystem< double >& sys = prm->getSystem((**(prm->systems().begin())).name());
        gum::prm::StructuredInference< double > inf(*prm, sys);
        inf.setPatternMining(false);
        const gum::prm::PRMInstance< double >&  i     = pickInstance(sys);
        const gum::prm::PRMAttribute< double >& a     = pickAttribute(i);
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&i, &a);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf.posterior(chain, m));
        double             sum = 0.0;
        gum::Instantiation inst(m);

        for (inst.setFirst(); !inst.end(); inst.inc())
          sum += m.get(inst);

        CHECK((sum) == doctest::Approx(1.0).epsilon(1e-6));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testStructuredInference_gen2() {
      try {
        std::vector< gum::prm::LayerGenerator< double >::LayerData > layers;
        generateLayer2(5, 2, layers);
        gum::prm::ClusteredLayerGenerator< double > generator;
        generator.setClusterRatio(1.0);
        generator.setLayers(layers);
        generator.setDomainSize(2);
        generator.setMaxParents(5);
        gum::prm::PRM< double >*       prm = generator.generate();
        gum::prm::PRMSystem< double >& sys = prm->getSystem((**(prm->systems().begin())).name());
        gum::prm::StructuredInference< double > inf(*prm, sys);
        inf.setPatternMining(false);
        const gum::prm::PRMInstance< double >&  i     = pickInstance(sys);
        const gum::prm::PRMAttribute< double >& a     = pickAttribute(i);
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&i, &a);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf.posterior(chain, m));
        double             sum = 0.0;
        gum::Instantiation inst(m);

        for (inst.setFirst(); !inst.end(); inst.inc())
          sum += m.get(inst);

        CHECK((sum) == doctest::Approx(1.0).epsilon(1e-6));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    static void testStructuredInference_gen3() {
      try {
        std::vector< gum::prm::LayerGenerator< double >::LayerData > layers;
        generateLayer3(5, 2, layers);
        gum::prm::ClusteredLayerGenerator< double > generator;
        generator.setClusterRatio(1.0);
        generator.setLayers(layers);
        generator.setDomainSize(2);
        generator.setMaxParents(5);
        gum::prm::PRM< double >*       prm = generator.generate();
        gum::prm::PRMSystem< double >& sys = prm->getSystem((**(prm->systems().begin())).name());
        gum::prm::StructuredInference< double > inf(*prm, sys);
        inf.setPatternMining(false);
        const gum::prm::PRMInstance< double >&  i     = pickInstance(sys);
        const gum::prm::PRMAttribute< double >& a     = pickAttribute(i);
        gum::prm::PRMInference< double >::Chain chain = std::make_pair(&i, &a);
        gum::Tensor< double >                   m;
        GUM_CHECK_ASSERT_THROWS_NOTHING(inf.posterior(chain, m));
        double             sum = 0.0;
        gum::Instantiation inst(m);

        for (inst.setFirst(); !inst.end(); inst.inc())
          sum += m.get(inst);

        CHECK((sum) == doctest::Approx(1.0).epsilon(1e-6));
        delete prm;
      } catch (gum::Exception&) { CHECK(false); }
    }

    // static void testFrequenceSearch_gen1() {
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
    //    gum::Tensor<double> m;
    //    GUM_CHECK_ASSERT_THROWS_NOTHING( inf.posterior( chain, m ) );
    //    double sum = 0.0;
    //    gum::Instantiation inst( m );

    //    for( inst.setFirst(); not inst.end(); inst.inc() )
    //      sum += m.get( inst );

    //    CHECK((sum) == doctest::Approx(1.0).epsilon(1e-6));
    //    delete prm;
    //  } catch (gum::Exception&) {
    //    CHECK(false);
    //  }
    //}
  };

  GUM_TEST_ACTIF(StructuredInference_gen1)
  GUM_TEST_ACTIF(StructuredInference_gen2)
  GUM_TEST_ACTIF(StructuredInference_gen3)
  GUM_TEST_ACTIF(FrequenceSearch_gen1)

}   // namespace gum_tests
