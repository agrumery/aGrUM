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

#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"

#include <agrum/prm/generator/layerGenerator.h>
#include <agrum/prm/generator/clusteredLayerGenerator.h>

namespace gum_tests {

  class PRMGeneratorTestSuite: public CxxTest::TestSuite {
    private:

    public:
      void setUp() {
        //std::cerr << std::endl;
      }

      void tearDown() { }

      void testConstructors() {
        gum::prm::LayerGenerator* gen = 0;
        TS_GUM_ASSERT_THROWS_NOTHING ( gen = new gum::prm::LayerGenerator() );
        TS_GUM_ASSERT_THROWS_NOTHING ( delete gen );
      }

      void generateLayerLayer ( std::vector<gum::prm::LayerGenerator::LayerData>& v, size_t layer_count ) {
        for ( size_t lvl = 0; lvl < layer_count; ++lvl ) {
          v.push_back ( gum::prm::LayerGenerator::LayerData() );
          v[lvl].a = 30;
          v[lvl].g = 2;
          v[lvl].c = 1;
          v[lvl].o = 10;
          v[lvl].inner_density = 0.2;
          v[lvl].outter_density = 0.05;
        }
      }

      void testLayerGenerator() {
        gum::prm::LayerGenerator* gen = 0;
        TS_GUM_ASSERT_THROWS_NOTHING ( gen = new gum::prm::LayerGenerator() );
        gen->setDomainSize ( 6 );
        gen->setMaxParents ( 5 );
        std::vector<gum::prm::LayerGenerator::LayerData> v;
        generateLayerLayer ( v, 10 );
        gen->setLayers ( v );
        gum::prm::PRM* prm = 0;
        TS_GUM_ASSERT_THROWS_NOTHING ( prm = gen->generate() );
        // testing interfaces
        const gum::Set<gum::prm::Interface*>& i_set = prm->interfaces();
        TS_ASSERT_EQUALS ( i_set.size(), ( gum::Size ) 10 );

        for ( gum::Set<gum::prm::Interface*>::const_iterator iter = i_set.begin(); iter != i_set.end(); ++iter ) {
          const gum::prm::Interface& i = **iter;

          if ( i.referenceSlots().size() ) {
            TS_ASSERT_EQUALS ( i.referenceSlots().size(), ( gum::Size ) 1 );
            TS_ASSERT_EQUALS ( i.attributes().size(), ( gum::Size ) 32 );
          } else {
            TS_ASSERT_EQUALS ( i.referenceSlots().size(), ( gum::Size ) 0 );
            TS_ASSERT_EQUALS ( i.attributes().size(), ( gum::Size ) 30 );
          }

          gum::Size six = 0;
          gum::Size two = 0;
          const gum::Set<gum::prm::Attribute*>& attr = i.attributes();

          for ( gum::Set<gum::prm::Attribute*>::const_iterator a = attr.begin(); a != attr.end(); ++a ) {
            if ( ( **a ).type()->domainSize() == ( gum::Size ) 6 ) {
              ++six;
            } else if ( ( **a ).type()->domainSize() == 2 ) {
              ++two;
            } else {
              TS_ASSERT ( false );
            }
          }

          TS_ASSERT_EQUALS ( six, ( gum::Size ) 30 );

          if ( i.referenceSlots().size() ) {
            TS_ASSERT_EQUALS ( two, ( gum::Size ) 2 );
          }
        }

        // testing classes
        const gum::Set<gum::prm::Class*>& c_set = prm->classes();

        for ( gum::Set<gum::prm::Class*>::const_iterator c = c_set.begin(); c != c_set.end(); ++c ) {
          TS_ASSERT_EQUALS ( ( **c ).attributes().size(), ( gum::Size ) 30 );

          for ( gum::Set<gum::prm::Attribute*>::const_iterator a = ( **c ).attributes().begin(); a != ( **c ).attributes().end(); ++a ) {
            TS_ASSERT ( ( **c ).dag().parents ( ( **a ).id() ).size() < 6 );
          }
        }

        // testing instances
        const gum::prm::System& sys = ** ( prm->systems().begin() );
        TS_ASSERT_EQUALS ( sys.size(), ( gum::Size ) 100 );

        if ( prm )
          delete prm;

        if ( gen ) {
          TS_GUM_ASSERT_THROWS_NOTHING ( delete gen );
        }
      }

      void testClusterGenerator() {
        gum::prm::ClusteredLayerGenerator* gen = 0;
        TS_GUM_ASSERT_THROWS_NOTHING ( gen = new gum::prm::ClusteredLayerGenerator() );
        gen->setDomainSize ( 6 );
        gen->setMaxParents ( 5 );
        gen->setClusterRatio ( 1.0 );
        std::vector<gum::prm::LayerGenerator::LayerData> v;
        generateLayerLayer ( v, 10 );
        gen->setLayers ( v );
        gum::prm::PRM* prm = 0;
        TS_GUM_ASSERT_THROWS_NOTHING ( prm = gen->generate() );
        // testing interfaces
        const gum::Set<gum::prm::Interface*>& i_set = prm->interfaces();
        TS_ASSERT_EQUALS ( i_set.size(), ( gum::Size ) 10 );

        for ( gum::Set<gum::prm::Interface*>::const_iterator iter = i_set.begin(); iter != i_set.end(); ++iter ) {
          const gum::prm::Interface& i = **iter;

          if ( i.referenceSlots().size() ) {
            TS_ASSERT_EQUALS ( i.referenceSlots().size(), ( gum::Size ) 1 );
            TS_ASSERT_EQUALS ( i.attributes().size(), ( gum::Size ) 32 );
          } else {
            TS_ASSERT_EQUALS ( i.referenceSlots().size(), ( gum::Size ) 0 );
            TS_ASSERT_EQUALS ( i.attributes().size(), ( gum::Size ) 30 );
          }

          gum::Size six = 0;
          gum::Size two = 0;
          const gum::Set<gum::prm::Attribute*>& attr = i.attributes();

          for ( gum::Set<gum::prm::Attribute*>::const_iterator a = attr.begin(); a != attr.end(); ++a ) {
            if ( ( **a ).type()->domainSize() == ( gum::Size ) 6 ) {
              ++six;
            } else if ( ( **a ).type()->domainSize() == 2 ) {
              ++two;
            } else {
              TS_ASSERT ( false );
            }
          }

          TS_ASSERT_EQUALS ( six, ( gum::Size ) 30 );

          if ( i.referenceSlots().size() ) {
            TS_ASSERT_EQUALS ( two, ( gum::Size ) 2 );
          }
        }

        // testing classes
        const gum::Set<gum::prm::Class*>& c_set = prm->classes();

        for ( gum::Set<gum::prm::Class*>::const_iterator c = c_set.begin(); c != c_set.end(); ++c ) {
          TS_ASSERT_EQUALS ( ( **c ).attributes().size(), ( gum::Size ) 30 );

          for ( gum::Set<gum::prm::Attribute*>::const_iterator a = ( **c ).attributes().begin(); a != ( **c ).attributes().end(); ++a ) {
            TS_ASSERT ( ( **c ).dag().parents ( ( **a ).id() ).size() < 6 );
          }
        }

        // testing instances
        const gum::prm::System& sys = ** ( prm->systems().begin() );
        TS_ASSERT ( sys.size() > ( gum::Size ) 100 );

        if ( prm )
          delete prm;

        if ( gen ) {
          TS_GUM_ASSERT_THROWS_NOTHING ( delete gen );
        }
      }

      void __generateLayer1 ( std::vector<gum::prm::LayerGenerator::LayerData>& v ) {
        for ( size_t lvl = 0; lvl < 5; ++lvl ) {
          v.push_back ( gum::prm::LayerGenerator::LayerData() );
          v[lvl].a = 10;
          v[lvl].g = 2;
          v[lvl].c = 1;
          v[lvl].o = 10;
          v[lvl].inner_density = 0.2;
          v[lvl].outter_density = 0.05;
        }
      }

  };

} // namespace gum_tests
