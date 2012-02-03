/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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
#include <iostream>
// ============================================================================
#include <cxxtest/AgrumTestSuite.h>
// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/prm/gspan/interfaceGraph.h>
// ============================================================================
#include <agrum/prm/skool/SkoolReader.h>
#include <agrum/prm/generator/layerGenerator.h>
// ============================================================================

namespace gum {

  namespace tests {

    using namespace gum;

    using namespace gum::prm;

    using namespace gum::prm::skool;

    using namespace gum::prm::gspan;

    class InterfaceGraphTestSuite: public CxxTest::TestSuite {

      private:
        PRM*  __prm;
        std::string source_dir;
        std::string ressource_dir;
        std::string printers;

      public:
        void setUp() {
          SkoolReader reader;
          reader.readFile( "../../../src/testunits/ressources/skool/printers_systems.skool" );
          __prm = reader.prm();
        }

        void tearDown() {
          delete __prm;
        }

        void testConstructorDestructor() {
          InterfaceGraph* ig = 0;
          TS_GUM_ASSERT_THROWS_NOTHING( ig = new InterfaceGraph( __prm->system( "microSys" ) ) );
          TS_GUM_ASSERT_THROWS_NOTHING( delete ig );
        }

        void testMicroTopologie() {
          InterfaceGraph* ig = 0;
          System& m = __prm->system( "microSys" );
          TS_GUM_ASSERT_THROWS_NOTHING( ig = new InterfaceGraph( m ) );
          // Checking existing nodes
          TS_ASSERT( ig->graph().existsNode( ig->id( m.get( "pow" ) ) ) );
          TS_ASSERT( ig->graph().existsNode( ig->id( m.get( "r" ) ) ) );
          TS_ASSERT( ig->graph().existsNode( ig->id( m.get( "p" ) ) ) );
          TS_ASSERT( ig->graph().existsNode( ig->id( m.get( "c" ) ) ) );
          TS_ASSERT( ig->graph().existsNode( ig->id( m.get( "e" ) ) ) );
          TS_ASSERT_EQUALS( ig->graph().size(), ( Size ) 5 );
          // Checking existing edges
          TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( "pow" ) ), ig->id( m.get( "p" ) ) ) );
          TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( "pow" ) ), ig->id( m.get( "c" ) ) ) );
          TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( "pow" ) ), ig->id( m.get( "e" ) ) ) );
          TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( "p" ) ), ig->id( m.get( "c" ) ) ) );
          TS_ASSERT_EQUALS( ig->graph().sizeEdges(), ( Size ) 4 );
          TS_GUM_ASSERT_THROWS_NOTHING( delete ig );
        }

        void testMicroLabelsOnNodes() {
          InterfaceGraph* ig = 0;
          System& m = __prm->system( "microSys" );
          TS_GUM_ASSERT_THROWS_NOTHING( ig = new InterfaceGraph( m ) );
          // Testing power supply
          TS_ASSERT_EQUALS( ig->size( ig->node( ig->id( m.get( "pow" ) ) ).l ), ( Size ) 1 );
          TS_ASSERT_DIFFERS( ig->node( ig->id( m.get( "pow" ) ) ).l->l,
                             ig->node( ig->id( m.get( "r" ) ) ).l->l );
          TS_ASSERT_DIFFERS( ig->node( ig->id( m.get( "pow" ) ) ).l->l,
                             ig->node( ig->id( m.get( "p" ) ) ).l->l );
          TS_ASSERT_DIFFERS( ig->node( ig->id( m.get( "pow" ) ) ).l->l,
                             ig->node( ig->id( m.get( "c" ) ) ).l->l );
          // Testing rooms
          TS_ASSERT_EQUALS( ig->size( ig->node( ig->id( m.get( "r" ) ) ).l ), ( Size ) 1 );
          TS_ASSERT_DIFFERS( ig->node( ig->id( m.get( "r" ) ) ).l->l,
                             ig->node( ig->id( m.get( "p" ) ) ).l->l );
          TS_ASSERT_DIFFERS( ig->node( ig->id( m.get( "r" ) ) ).l->l,
                             ig->node( ig->id( m.get( "c" ) ) ).l->l );
          // Testing printers
          TS_ASSERT_EQUALS( ig->size( ig->node( ig->id( m.get( "p" ) ) ).l ), ( Size ) 1 );
          TS_ASSERT_DIFFERS( ig->node( ig->id( m.get( "p" ) ) ).l->l,
                             ig->node( ig->id( m.get( "c" ) ) ).l->l );
          TS_GUM_ASSERT_THROWS_NOTHING( delete ig );
        }

        void testMicroLabelsOnEdges() {
          InterfaceGraph* ig = 0;
          System& m = __prm->system( "microSys" );
          TS_GUM_ASSERT_THROWS_NOTHING( ig = new InterfaceGraph( m ) );
          // Test difference
          TS_ASSERT_DIFFERS( ig->edge( ig->id( m.get( "pow" ) ), ig->id( m.get( "p" ) ) ).l,
                             ig->edge( ig->id( m.get( "pow" ) ), ig->id( m.get( "c" ) ) ).l );
          TS_ASSERT_DIFFERS( ig->edge( ig->id( m.get( "pow" ) ), ig->id( m.get( "p" ) ) ).l,
                             ig->edge( ig->id( m.get( "p" ) ),   ig->id( m.get( "c" ) ) ).l );
          TS_ASSERT_DIFFERS( ig->edge( ig->id( m.get( "pow" ) ), ig->id( m.get( "c" ) ) ).l,
                             ig->edge( ig->id( m.get( "p" ) ),   ig->id( m.get( "c" ) ) ).l );
          TS_GUM_ASSERT_THROWS_NOTHING( delete ig );
        }

        std::string getPrinter( int i ) const {
          std::stringstream sBuff;
          sBuff << "printers[" << i << "]";
          return sBuff.str();
        }

        std::string getComputer( int i ) const {
          std::stringstream sBuff;
          sBuff << "computers[" << i << "]";
          return sBuff.str();
        }

        void testSmallTopologie() {
          InterfaceGraph* ig = 0;
          System& m = __prm->system( "smallSys" );
          TS_GUM_ASSERT_THROWS_NOTHING( ig = new InterfaceGraph( m ) );
          // Checking existing nodes
          int node_count = 0;
          TS_ASSERT( ig->graph().existsNode( ig->id( m.get( "pow" ) ) ) );
          TS_ASSERT( ig->graph().existsNode( ig->id( m.get( "r" ) ) ) );
          node_count += 2;

          for ( int i = 0; i < 2; ++i, ++node_count ) {
            TS_ASSERT( ig->graph().existsNode( ig->id( m.get( getPrinter( i ) ) ) ) );
          }

          TS_ASSERT( ig->graph().existsNode( ig->id( m.get( "another_printer" ) ) ) );

          ++node_count;

          for ( int i = 0; i < 4; ++i, ++node_count ) {
            TS_ASSERT( ig->graph().existsNode( ig->id( m.get( getComputer( i ) ) ) ) );
          }

          TS_ASSERT( ig->graph().existsNode( ig->id( m.get( "another_computer" ) ) ) );

          ++node_count;
          TS_ASSERT_EQUALS( ig->graph().size(), ( Size ) node_count );
          // Checking existing edges from pow
          int edge_count = 0;

          for ( int i = 0; i < 2; ++i, ++edge_count ) {
            TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( "pow" ) ), ig->id( m.get( getPrinter( i ) ) ) ) );
          }

          TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( "pow" ) ), ig->id( m.get( "another_printer" ) ) ) );

          ++edge_count;

          for ( int i = 0; i < 4; ++i, ++edge_count ) {
            TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( "pow" ) ), ig->id( m.get( getComputer( i ) ) ) ) );
          }

          TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( "pow" ) ), ig->id( m.get( "another_computer" ) ) ) );

          ++edge_count;
          // Checking existing edges from printers

          for ( int i = 0; i < 2; ++i, ++edge_count ) {
            for ( int j = 0; j < 4; ++j, ++edge_count ) {
              TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( getPrinter( i ) ) ), ig->id( m.get( getComputer( j ) ) ) ) );
            }

            TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( getPrinter( i ) ) ), ig->id( m.get( "another_computer" ) ) ) );
          }

          for ( int i = 0; i < 4; ++i, ++edge_count ) {
            TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( "another_printer" ) ), ig->id( m.get( getComputer( i ) ) ) ) );
          }

          TS_ASSERT( ig->graph().existsEdge( ig->id( m.get( "another_printer" ) ), ig->id( m.get( "another_computer" ) ) ) );

          ++edge_count;
          TS_ASSERT_EQUALS( ig->graph().sizeEdges(), ( Size ) edge_count );
          TS_GUM_ASSERT_THROWS_NOTHING( delete ig );
        }

        void testSmallLabelsOnNodes() {
          InterfaceGraph* ig = 0;
          System& m = __prm->system( "smallSys" );
          TS_GUM_ASSERT_THROWS_NOTHING( ig = new InterfaceGraph( m ) );
          // Testing each labels size (the number of nodes with the given label)
          TS_ASSERT_EQUALS( ig->size( ig->node( ig->id( m.get( "pow" ) ) ).l ), ( Size ) 1 );
          TS_ASSERT_EQUALS( ig->size( ig->node( ig->id( m.get( "r" ) ) ).l ), ( Size ) 1 );
          TS_ASSERT_EQUALS( ig->size( ig->node( ig->id( m.get( "another_printer" ) ) ).l ), ( Size ) 3 );
          TS_ASSERT_EQUALS( ig->size( ig->node( ig->id( m.get( "another_computer" ) ) ).l ), ( Size ) 5 );
          TS_GUM_ASSERT_THROWS_NOTHING( delete ig );
        }

        void testSmallLabelsOnEdges() {
          InterfaceGraph* ig = 0;
          System& m = __prm->system( "smallSys" );
          TS_GUM_ASSERT_THROWS_NOTHING( ig = new InterfaceGraph( m ) );
          // Test difference
          int edge_count = 0;
          TS_ASSERT_EQUALS( ig->size( ig->edge( ig->id( m.get( "pow" ) ), ig->id( m.get( "another_printer" ) ) ).l ), ( Size ) 3 );
          edge_count += 3;
          TS_ASSERT_EQUALS( ig->size( ig->edge( ig->id( m.get( "pow" ) ), ig->id( m.get( "another_computer" ) ) ).l ), ( Size ) 5 );
          edge_count += 5;
          TS_ASSERT_EQUALS( ig->size( ig->edge( ig->id( m.get( "another_printer" ) ), ig->id( m.get( "another_computer" ) ) ).l ), ( Size ) 15 );
          edge_count += 15;
          TS_ASSERT_EQUALS( ig->graph().sizeEdges(), ( Size ) edge_count );
          TS_GUM_ASSERT_THROWS_NOTHING( delete ig );
        }

        void generateLayer( std::vector<LayerGenerator::LayerData>& v, size_t layer_count ) {
          for ( size_t lvl = 0; lvl < layer_count; ++lvl ) {
            v.push_back( LayerGenerator::LayerData() );
            v[lvl].a = 30;
            v[lvl].g = 2;
            v[lvl].c = 1;
            v[lvl].o = 10;
            v[lvl].inner_density = 0.2;
            v[lvl].outter_density = 0.05;
          }
        }

        void testWithLayerGeneration() {
          std::vector<LayerGenerator::LayerData> layers;
          generateLayer( layers, 10 );
          LayerGenerator generator;
          generator.setLayers( layers );
          generator.setDomainSize( 2 );
          generator.setMaxParents( 5 );
          PRM* prm = generator.generate();
          System& sys = prm->system(( **( prm->systems().begin() ) ).name() );
          InterfaceGraph* g = 0;
          TS_GUM_ASSERT_THROWS_NOTHING( g = new InterfaceGraph( sys ) );

          if ( g != 0 )
            delete g;

          delete prm;
        }

    };

  } /* namespace test */
} /* namespace gum */
// ============================================================================
