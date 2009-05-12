/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
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


#include <iostream>
#include <string>

#include <cxxtest/TestSuite.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/graphs/defaultTriangulation.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIFIO.h>
#include <agrum/BN/inference/lazyPropagation.h>

#define GET_PATH_STR(x) ("../../../src/testunits/resources/" #x)


// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

class DefaultTriangulationTestSuite: public CxxTest::TestSuite {
  public:
    gum::LabelizedVariable *var1, *var2, *var3, *var4, *var5;

    void setUp() {
      var1 = new gum::LabelizedVariable( "var1", "1" );
      var2 = new gum::LabelizedVariable( "var2", "2" );
      var3 = new gum::LabelizedVariable( "var3", "3" );
      var4 = new gum::LabelizedVariable( "var4", "4" );
      var5 = new gum::LabelizedVariable( "var5", "5" );

    };


    void tearDown() {
      delete var1;
      delete var2;
      delete var3;
      delete var4;
      delete var5;
    };

    void testtriangulatedGraph() {
      gum::BayesNet<float> topo;
      gum::List<gum::NodeId> idList;

      fill( topo, idList );
      const gum::UndiGraph &undiGraph = topo.moralGraph();
      gum::HashTable<gum::NodeId, gum::Idx> modalities;
      // Builds a hashTable where the keys are the id of the variable,
      // and the values the variable's domain size.

      for ( gum::NodeSetIterator iter = topo.beginNodes(); iter != topo.endNodes(); ++iter )
        modalities.insert(*iter, topo.variable(*iter).domainSize());

      gum::DefaultTriangulation *triangle=NULL;

      TS_GUM_ASSERT_THROWS_NOTHING( triangle = new gum::DefaultTriangulation( undiGraph, modalities ) );

      TS_GUM_ASSERT_THROWS_NOTHING( triangle->triangulatedGraph() );

      TS_GUM_ASSERT_THROWS_NOTHING( if ( triangle ) delete triangle );
    };

    void testjunctionTree() {
      gum::BayesNet<float> topo;
      gum::List<gum::NodeId> idList;

      fill( topo, idList );

      gum::UndiGraph undiGraph;

      try {
        undiGraph = topo.moralGraph();
      } catch ( gum::Exception& e ) {GUM_SHOWERROR( e );exit( 0 );};


      gum::HashTable<gum::NodeId, gum::Idx> modalities;

      // Builds a hashTable where the keys are the id of the variable,
      // and the values the variable's domain size.

      for ( gum::NodeSetIterator iter = topo.beginNodes(); iter != topo.endNodes(); ++iter )
        modalities.insert(*iter, topo.variable(*iter).domainSize());

      gum::DefaultTriangulation *triangle=NULL;

      TS_GUM_ASSERT_THROWS_NOTHING( triangle = new gum::DefaultTriangulation( undiGraph, modalities ) );

      try {
        triangle->junctionTree();
      } catch ( gum::Exception &e ) {GUM_SHOWERROR( e );};

      TS_GUM_ASSERT_THROWS_NOTHING( triangle->junctionTree() );

      TS_ASSERT( triangle->junctionTree().hasRunningIntersection() );

      TS_GUM_ASSERT_THROWS_NOTHING( if ( triangle ) delete triangle );
    };

    void testBIFtriangulation1( void ) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif( GET_PATH_STR( Barley.bif ) ));
    };
    void testBIFtriangulation1bis( void ) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif( GET_PATH_STR( alarm.bif ) ));
    };

    void testBIFtriangulation2( void ) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif( GET_PATH_STR( carpo.bif ) ));
    };

    void testBIFtriangulation3( void ) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif( GET_PATH_STR( Pigs.bif ) ));
    };

    void testBIFtriangulation4( void ) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif( GET_PATH_STR( Water.bif ) ));
    };

    void testBIFtriangulation5( void ) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif( GET_PATH_STR( Link.bif ) ));
    };

    void testBIFtriangulation6( void ) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif( GET_PATH_STR( Mildew.bif ) ));
    };

    void testBIFtriangulation7( void ) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif( GET_PATH_STR( Munin1.bif ) ));
    };

    void testBIFtriangulation8( void ) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif( GET_PATH_STR( hailfinder.bif ) ));
    };

    void testBIFtriangulation9( void ) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif( GET_PATH_STR( Diabetes.bif ) ));
    };

    void testBIFtriangulation10( void ) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif( GET_PATH_STR( insurance.bif ) ));
    };

  private:

    void __triangulate_bif( std::string n ) {
      gum::BayesNet<float>* bn = 0;
      //try {
      gum::BIFReader b;
      bn=b.read( n );

      gum::LazyPropagation<float> inf( *bn );
      //} catch (gum::Exception& e) {
      //  std::cerr << std::endl << e.getContent() << std::endl;
      //  GUM_CHECKPOINT;
      //}
      if (bn != 0) delete( bn );
      else GUM_CHECKPOINT;
    };

    void fill( gum::BayesNet<float>& topo, gum::List<gum::NodeId> &idList ) {
      idList.clear();
      idList.insert( topo.addVariable( *var1 ) );
      idList.insert( topo.addVariable( *var2 ) );
      idList.insert( topo.addVariable( *var3 ) );
      idList.insert( topo.addVariable( *var4 ) );
      idList.insert( topo.addVariable( *var5 ) );

      topo.insertArc( idList[0], idList[2] );
      topo.insertArc( idList[2], idList[4] );
      topo.insertArc( idList[1], idList[3] );
      topo.insertArc( idList[0], idList[3] );
      topo.insertArc( idList[3], idList[4] );
      topo.insertArc( idList[1], idList[4] );
    };

    gum::UndiGraph getRealMoralGraph( const gum::List<gum::NodeId> &idList ) {
      gum::UndiGraph graph;
      /*
            idList.clear();
            idList.insert(graph.insertNode());
            idList.insert(graph.insertNode());
            idList.insert(graph.insertNode());
            idList.insert(graph.insertNode());
            idList.insert(graph.insertNode());*/

      graph.insertEdge( gum::Edge( idList[0], idList[2] ) );
      graph.insertEdge( gum::Edge( idList[0], idList[3] ) );
      graph.insertEdge( gum::Edge( idList[0], idList[1] ) );
      graph.insertEdge( gum::Edge( idList[2], idList[4] ) );
      graph.insertEdge( gum::Edge( idList[3], idList[4] ) );
      graph.insertEdge( gum::Edge( idList[2], idList[3] ) );
      graph.insertEdge( gum::Edge( idList[1], idList[3] ) );
      graph.insertEdge( gum::Edge( idList[1], idList[4] ) );
      graph.insertEdge( gum::Edge( idList[1], idList[2] ) );

      return graph;
    }

};

