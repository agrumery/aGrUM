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
