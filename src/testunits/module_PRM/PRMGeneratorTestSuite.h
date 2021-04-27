/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <fstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/PRM/generator/clusteredLayerGenerator.h>
#include <agrum/PRM/generator/layerGenerator.h>

namespace gum_tests {

  class PRMGeneratorTestSuite: public CxxTest::TestSuite {
    private:
    public:
    void setUp() {
      // std::cerr << std::endl;
    }

    void tearDown() {}

    void testConstructors() {
      gum::prm::LayerGenerator< double >* gen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(gen = new gum::prm::LayerGenerator< double >());
      TS_GUM_ASSERT_THROWS_NOTHING(delete gen);
    }

    void generateLayerLayer(
       std::vector< gum::prm::LayerGenerator< double >::LayerData >& v,
       size_t                                                        layer_count) {
      for (size_t lvl = 0; lvl < layer_count; ++lvl) {
        v.push_back(gum::prm::LayerGenerator< double >::LayerData());
        v[lvl].a              = 30;
        v[lvl].g              = 2;
        v[lvl].c              = 1;
        v[lvl].o              = 10;
        v[lvl].inner_density  = 0.2f;
        v[lvl].outter_density = 0.05f;
      }
    }

    void testLayerGenerator() {
      gum::prm::LayerGenerator< double >* gen = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(gen = new gum::prm::LayerGenerator< double >());
      gen->setDomainSize(6);
      gen->setMaxParents(5);
      std::vector< gum::prm::LayerGenerator< double >::LayerData > v;
      generateLayerLayer(v, 10);
      gen->setLayers(v);
      gum::prm::PRM< double >* prm = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = gen->generate());
      // testing interfaces
      const gum::Set< gum::prm::PRMInterface< double >* >& i_set
         = prm->interfaces();
      TS_ASSERT_EQUALS(i_set.size(), (gum::Size)10);

      for (auto iter = i_set.begin(); iter != i_set.end(); ++iter) {
        const gum::prm::PRMInterface< double >& i = **iter;

        if (i.referenceSlots().size()) {
          TS_ASSERT_EQUALS(i.referenceSlots().size(), (gum::Size)1);
          TS_ASSERT_EQUALS(i.attributes().size(), (gum::Size)32);
        } else {
          TS_ASSERT_EQUALS(i.referenceSlots().size(), (gum::Size)0);
          TS_ASSERT_EQUALS(i.attributes().size(), (gum::Size)30);
        }

        gum::Size                                            six  = 0;
        gum::Size                                            two  = 0;
        const gum::Set< gum::prm::PRMAttribute< double >* >& attr = i.attributes();

        for (gum::Set< gum::prm::PRMAttribute< double >* >::const_iterator a
             = attr.begin();
             a != attr.end();
             ++a) {
          if ((**a).type()->domainSize() == (gum::Size)6) {
            ++six;
          } else if ((**a).type()->domainSize() == 2) {
            ++two;
          } else {
            TS_ASSERT(false);
          }
        }

        TS_ASSERT_EQUALS(six, (gum::Size)30);

        if (i.referenceSlots().size()) { TS_ASSERT_EQUALS(two, (gum::Size)2); }
      }

      // testing classes
      const gum::Set< gum::prm::PRMClass< double >* >& c_set = prm->classes();

      for (gum::Set< gum::prm::PRMClass< double >* >::const_iterator c
           = c_set.begin();
           c != c_set.end();
           ++c) {
        TS_ASSERT_EQUALS((**c).attributes().size(), (gum::Size)30);

        for (gum::Set< gum::prm::PRMAttribute< double >* >::const_iterator a
             = (**c).attributes().begin();
             a != (**c).attributes().end();
             ++a) {
          TS_ASSERT((**c).containerDag().parents((**a).id()).size() < 6);
        }
      }

      // testing instances
      const gum::prm::PRMSystem< double >& sys = **(prm->systems().begin());
      TS_ASSERT_EQUALS(sys.size(), (gum::Size)100);

      if (prm) delete prm;

      if (gen) { TS_GUM_ASSERT_THROWS_NOTHING(delete gen); }
    }

    void testClusterGenerator() {
      gum::prm::ClusteredLayerGenerator< double >* gen = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
         gen = new gum::prm::ClusteredLayerGenerator< double >());
      gen->setDomainSize(6);
      gen->setMaxParents(5);
      gen->setClusterRatio(1.0);
      std::vector< gum::prm::LayerGenerator< double >::LayerData > v;
      generateLayerLayer(v, 10);
      gen->setLayers(v);
      gum::prm::PRM< double >* prm = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(prm = gen->generate());
      // testing interfaces
      const gum::Set< gum::prm::PRMInterface< double >* >& i_set
         = prm->interfaces();
      TS_ASSERT_EQUALS(i_set.size(), (gum::Size)10);

      for (gum::Set< gum::prm::PRMInterface< double >* >::const_iterator iter
           = i_set.begin();
           iter != i_set.end();
           ++iter) {
        const gum::prm::PRMInterface< double >& i = **iter;

        if (i.referenceSlots().size()) {
          TS_ASSERT_EQUALS(i.referenceSlots().size(), (gum::Size)1);
          TS_ASSERT_EQUALS(i.attributes().size(), (gum::Size)32);
        } else {
          TS_ASSERT_EQUALS(i.referenceSlots().size(), (gum::Size)0);
          TS_ASSERT_EQUALS(i.attributes().size(), (gum::Size)30);
        }

        gum::Size                                            six  = 0;
        gum::Size                                            two  = 0;
        const gum::Set< gum::prm::PRMAttribute< double >* >& attr = i.attributes();

        for (gum::Set< gum::prm::PRMAttribute< double >* >::const_iterator a
             = attr.begin();
             a != attr.end();
             ++a) {
          if ((**a).type()->domainSize() == (gum::Size)6) {
            ++six;
          } else if ((**a).type()->domainSize() == 2) {
            ++two;
          } else {
            TS_ASSERT(false);
          }
        }

        TS_ASSERT_EQUALS(six, (gum::Size)30);

        if (i.referenceSlots().size()) { TS_ASSERT_EQUALS(two, (gum::Size)2); }
      }

      // testing classes
      const gum::Set< gum::prm::PRMClass< double >* >& c_set = prm->classes();

      for (gum::Set< gum::prm::PRMClass< double >* >::const_iterator c
           = c_set.begin();
           c != c_set.end();
           ++c) {
        TS_ASSERT_EQUALS((**c).attributes().size(), (gum::Size)30);

        for (gum::Set< gum::prm::PRMAttribute< double >* >::const_iterator a
             = (**c).attributes().begin();
             a != (**c).attributes().end();
             ++a) {
          TS_ASSERT((**c).containerDag().parents((**a).id()).size() < 6);
        }
      }

      // testing instances
      const gum::prm::PRMSystem< double >& sys = **(prm->systems().begin());
      TS_ASSERT(sys.size() > (gum::Size)100);

      if (prm) delete prm;

      if (gen) { TS_GUM_ASSERT_THROWS_NOTHING(delete gen); }
    }
  };

}   // namespace gum_tests
