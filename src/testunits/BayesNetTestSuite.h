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
 *   (gumSize) 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.   *
 ***************************************************************************/

#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/BayesNetGenerator.h>
#include <agrum/multidim/discreteVariable.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/multidim/labelizedVariable.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class BayesNetTestSuite: public CxxTest::TestSuite {

    private:
      void fillTopo( gum::BayesNet<float> &bn, gum::List<gum::NodeId> &idList ) {
        try {
          idList.insert( bn.addVariable( *var1 ) );
          idList.insert( bn.addVariable( *var2 ) );
          idList.insert( bn.addVariable( *var3 ) );
          idList.insert( bn.addVariable( *var4 ) );
          idList.insert( bn.addVariable( *var5 ) );

          bn.insertArc( idList[0], idList[2] );
          bn.insertArc( idList[2], idList[4] );
          bn.insertArc( idList[1], idList[3] );
          bn.insertArc( idList[0], idList[3] );
          bn.insertArc( idList[3], idList[4] );
          bn.insertArc( idList[1], idList[4] );

        } catch( gum::Exception &e ) {
          std::cerr << std::endl << e.content() << std::endl;
          throw ;
        }
      }

      void fill( gum::BayesNet<float> &bn, gum::List<gum::NodeId> &idList ) {
        fillTopo( bn, idList );

        try {
          const gum::Potential<float>& p1 = bn.cpt( idList[0] );
          {
            // FILLING PARAMS
            const float t[2] = {0.2, 0.8};
            int n = 2;
            const std::vector<float> v( t, t + n );
            p1.fillWith( v );
          }

          const gum::Potential<float>& p2 = bn.cpt( idList[1] );
          {
            // FILLING PARAMS
            const float t[2] = {0.3, 0.7};
            int n = 2;
            const std::vector<float> v( t, t + n );
            p2.fillWith( v );
          }

          const gum::Potential<float>& p3 = bn.cpt( idList[2] );
          {
            // FILLING PARAMS
            const float t[4] = {0.1, 0.9, 0.9, 0.1};
            int n = 4;
            const std::vector<float> v( t, t + n );
            p3.fillWith( v );
          }

          const gum::Potential<float>& p4 = bn.cpt( idList[3] );
          {
            // FILLING PARAMS
            const float t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0};
            int n = 8;
            const std::vector<float> v( t, t + n );
            p4.fillWith( v );
          }

          const gum::Potential<float>& p5 = bn.cpt( idList[4] );
          {
            // FILLING PARAMS
            const float t[24] = {0.3, 0.6, 0.1, 0.5, 0.5, 0.0, 0.5, 0.5, 0.0, 1.0, 0.0, 0.0,
                                 0.4, 0.6, 0.0, 0.5, 0.5, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0
                                };
            int n = 24;
            const std::vector<float> v( t, t + n );
            p5.fillWith( v );
            //printProba(p5);
          }

        } catch( gum::Exception &e ) {
          std::cerr << std::endl << e.content() << std::endl;
          throw ;
        }
      }

      gum::UndiGraph getRealMoralGraph( const gum::BayesNet<float>& bn, const gum::List<gum::NodeId> &idList ) {
        gum::UndiGraph graph;
        graph.populateNodes( bn.dag() );

        graph.insertEdge( idList[0], idList[2] );
        graph.insertEdge( idList[0], idList[3] );
        graph.insertEdge( idList[0], idList[1] );
        graph.insertEdge( idList[2], idList[4] );
        graph.insertEdge( idList[3], idList[4] );
        graph.insertEdge( idList[2], idList[3] );
        graph.insertEdge( idList[1], idList[3] );
        graph.insertEdge( idList[1], idList[4] );
        graph.insertEdge( idList[1], idList[2] );

        return graph;
      }

    public:
      gum::LabelizedVariable *var1, *var2, *var3, *var4, *var5;

      void setUp() {
        var1 = new gum::LabelizedVariable( "var1", "1", 2 );
        var2 = new gum::LabelizedVariable( "var2", "2", 2 );
        var3 = new gum::LabelizedVariable( "var3", "3", 2 );
        var4 = new gum::LabelizedVariable( "var4", "4", 2 );
        var5 = new gum::LabelizedVariable( "var5", "(gum::Size) 5", 3 );
      }


      void tearDown() {
        delete var1;
        delete var2;
        delete var3;
        delete var4;
        delete var5;
      }

      void testConstructor() {
        gum::BayesNet<float> *topology = NULL;
        TS_GUM_ASSERT_THROWS_NOTHING( topology = new gum::BayesNet<float>() );
        TS_ASSERT_EQUALS( topology->size(),( gum::Idx )0 );
        TS_ASSERT_EQUALS( topology->nbrArcs(),( gum::Idx )0 );
        TS_ASSERT_EQUALS( topology->dim(),( gum::Idx )0 );

        TS_ASSERT_THROWS( topology->insertArc( 1, 2 ), gum::InvalidNode );

        gum::List<gum::NodeId> idList;
        TS_GUM_ASSERT_THROWS_NOTHING( fill( *topology, idList ) );

        TS_ASSERT_THROWS( topology->add( *var1 ), gum::DuplicateLabel );

        TS_ASSERT_EQUALS( topology->toString(), "BN{nodes: 5, arcs: 6, domainSize: 48, parameters: 40, compression ratio: 16% }" );
        TS_ASSERT_EQUALS( topology->size(),( gum::Idx )5 );
        TS_ASSERT_EQUALS( topology->nbrArcs(),( gum::Idx )6 );
        TS_ASSERT_EQUALS( topology->dim(),( gum::Idx )24 );

        TS_GUM_ASSERT_THROWS_NOTHING( delete topology );

      }

      void testCopyConstructor() {
        gum::BayesNet<float> source;
        gum::List<gum::NodeId> idList;
        TS_GUM_ASSERT_THROWS_NOTHING( fill( source, idList ) );

        gum::BayesNet<float> *copy = NULL;
        TS_GUM_ASSERT_THROWS_NOTHING( copy = new gum::BayesNet<float> ( source ) );

        TS_ASSERT_EQUALS( source.dag().size(), copy->dag().size() );
        TS_ASSERT_EQUALS( source.dag().sizeArcs(), copy->dag().sizeArcs() );
        //const gum::NodeSet& nodes=source.dag().nodes();
        const gum::DAG dag = source.dag();

        for( gum::DAG::NodeIterator nodeIter = dag.beginNodes();
             nodeIter != dag.endNodes();
             ++nodeIter ) {
          TS_ASSERT( copy->dag().exists( *nodeIter ) );

          const gum::DiscreteVariable &srcVar = source.variable( *nodeIter );
          const gum::DiscreteVariable &cpVar = copy->variable( *nodeIter );
          TS_ASSERT_EQUALS( srcVar.name(), cpVar.name() );

          if( srcVar.domainSize() == cpVar.domainSize() ) {
            for( gum::Idx i = 0; i < srcVar.domainSize(); i++ ) {
              TS_ASSERT_EQUALS( srcVar.label( i ), cpVar.label( i ) );
            }

          } else {
            TS_ASSERT( false );
          }

          const gum::NodeSet& parentList = source.dag().parents( *nodeIter );

          for( gum::NodeSet::iterator arcIter = parentList.begin();
               arcIter != parentList.end();
               ++arcIter ) {
            TS_ASSERT( copy->dag().existsArc( gum::Arc( *arcIter, *nodeIter ) ) );
          }

          const gum::Potential<float> &srcCPT = source.cpt( *nodeIter );

          const gum::Potential<float> &cpCPT = copy->cpt( *nodeIter );

          gum::Instantiation srcInst( srcCPT );

          gum::Instantiation cpInst( cpCPT );

          for( cpInst.setFirst(); !cpInst.end(); cpInst.inc() ) {
            for( gum::Idx i = 0; i < cpInst.nbrDim(); i++ ) {
              gum::Id id = copy->nodeId( cpInst.variable( i ) );
              srcInst.chgVal( source.variable( id ), cpInst.val( i ) );
            }

            TS_ASSERT_EQUALS( cpCPT[cpInst], srcCPT[srcInst] );
          }
        }

        if( copy ) delete copy;
      }

      void testInsertion_1() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.addVariable( *var1 ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.addVariable( *var2 ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.addVariable( *var3 ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.addVariable( *var4 ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.addVariable( *var5 ) ) );

        TS_ASSERT_EQUALS( bn.size(), ( gum::Size ) 5 );
        TS_ASSERT_EQUALS( bn.dag().size(), ( gum::Size ) 5 );

        // Test for uniqueness of the ids

        for( gum::Idx i = 0; i < idList.size() - 1; i++ ) {
          for( gum::Idx j = i + 1; j < idList.size(); j++ ) {
            if( idList[i] != idList[j] ) {
              TS_ASSERT( true );

            } else {
              TS_ASSERT( false );
            }
          }
        }

        gum::LabelizedVariable *varPtr = NULL;

        TS_GUM_ASSERT_THROWS_NOTHING( varPtr = ( gum::LabelizedVariable* ) & bn.variable( idList[0] ) );
        TS_ASSERT_EQUALS( *varPtr, *var1 );

        TS_ASSERT_EQUALS( * ( ( gum::LabelizedVariable* ) &bn.variable( idList[0] ) ), *var1 );
        TS_ASSERT_EQUALS( * ( ( gum::LabelizedVariable* ) &bn.variable( idList[1] ) ), *var2 );
        TS_ASSERT_EQUALS( * ( ( gum::LabelizedVariable* ) &bn.variable( idList[2] ) ), *var3 );
        TS_ASSERT_EQUALS( * ( ( gum::LabelizedVariable* ) &bn.variable( idList[3] ) ), *var4 );
        TS_ASSERT_EQUALS( * ( ( gum::LabelizedVariable* ) &bn.variable( idList[4] ) ), *var5 );
      }

      void testInsertion_2() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.addVariable( *var1 ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.addVariable( *var2 ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.addVariable( *var3 ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.addVariable( *var4 ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.addVariable( *var5 ) ) );

        TS_ASSERT_EQUALS( bn.size(), ( gum::Size ) 5 );
        TS_ASSERT_EQUALS( bn.dag().size(), ( gum::Size ) 5 );

        gum::LabelizedVariable *varPtr = NULL;
        TS_GUM_ASSERT_THROWS_NOTHING( varPtr = ( gum::LabelizedVariable* ) & bn.variable( idList[0] ) );
        TS_ASSERT_EQUALS( *varPtr, *var1 );

        TS_ASSERT_EQUALS( * ( ( gum::LabelizedVariable* ) &bn.variable( idList[0] ) ), *var1 );
        TS_ASSERT_EQUALS( * ( ( gum::LabelizedVariable* ) &bn.variable( idList[1] ) ), *var2 );
        TS_ASSERT_EQUALS( * ( ( gum::LabelizedVariable* ) &bn.variable( idList[2] ) ), *var3 );
        TS_ASSERT_EQUALS( * ( ( gum::LabelizedVariable* ) &bn.variable( idList[3] ) ), *var4 );
        TS_ASSERT_EQUALS( * ( ( gum::LabelizedVariable* ) &bn.variable( idList[4] ) ), *var5 );
      }

      void testArcInsertion() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        idList.insert( bn.addVariable( *var1 ) );
        idList.insert( bn.addVariable( *var2 ) );
        idList.insert( bn.addVariable( *var3 ) );
        idList.insert( bn.addVariable( *var4 ) );
        idList.insert( bn.addVariable( *var5 ) );

        TS_GUM_ASSERT_THROWS_NOTHING( bn.insertArc( idList[0], idList[2] ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bn.insertArc( idList[2], idList[4] ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bn.insertArc( idList[1], idList[3] ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bn.insertArc( idList[0], idList[3] ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bn.insertArc( idList[3], idList[4] ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bn.insertArc( idList[1], idList[4] ) );

        TS_ASSERT_EQUALS( bn.dag().sizeArcs(), ( gum::Size ) 6 );
      }

      void testEraseVar() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        TS_ASSERT( bn.empty() );
        TS_ASSERT( bn.dag().emptyArcs() );

        fill( bn, idList );

        TS_ASSERT( ! bn.empty() );
        TS_ASSERT( ! bn.dag().emptyArcs() );

        TS_ASSERT_EQUALS( bn.size(), ( gum::Size ) 5 );
        TS_ASSERT_EQUALS( bn.dag().size(), ( gum::Size ) 5 );
        TS_ASSERT_EQUALS( bn.dag().sizeArcs(), ( gum::Size ) 6 );


        bn.eraseVariable( idList[0] );

        for( gum::List<gum::NodeId>::iterator iter = idList.begin();
             iter != idList.end();
             ++iter ) {
          bn.eraseVariable( *iter );
        }

        TS_ASSERT( bn.empty() );


        TS_ASSERT( bn.dag().emptyArcs() );


        TS_ASSERT_EQUALS( bn.size(), ( gum::Size ) 0 );
        TS_ASSERT_EQUALS( bn.dag().size(), ( gum::Size ) 0 );
        TS_ASSERT_EQUALS( bn.dag().sizeArcs(), ( gum::Size ) 0 );


        idList.clear();
        TS_GUM_ASSERT_THROWS_NOTHING( fill( bn, idList ) );


        TS_ASSERT( ! bn.empty() );
        TS_ASSERT( ! bn.dag().emptyArcs() );


        TS_ASSERT_EQUALS( bn.size(), ( gum::Size ) 5 );
        TS_ASSERT_EQUALS( bn.dag().size(), ( gum::Size ) 5 );
        TS_ASSERT_EQUALS( bn.dag().sizeArcs(), ( gum::Size ) 6 );
      }

      void testEraseArc() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        TS_ASSERT( bn.empty() );
        TS_ASSERT( bn.dag().emptyArcs() );

        fill( bn, idList );

        TS_ASSERT( ! bn.empty() );
        TS_ASSERT( ! bn.dag().emptyArcs() );

        TS_ASSERT_EQUALS( bn.size(), ( gum::Size ) 5 );
        TS_ASSERT_EQUALS( bn.dag().size(), ( gum::Size ) 5 );
        TS_ASSERT_EQUALS( bn.dag().sizeArcs(), ( gum::Size ) 6 );

        TS_GUM_ASSERT_THROWS_NOTHING( bn.eraseArc( gum::Arc( idList[0], idList[2] ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bn.eraseArc( gum::Arc( idList[2], idList[4] ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bn.eraseArc( gum::Arc( idList[1], idList[3] ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bn.eraseArc( gum::Arc( idList[0], idList[3] ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bn.eraseArc( gum::Arc( idList[3], idList[4] ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bn.eraseArc( gum::Arc( idList[1], idList[4] ) ) );

        TS_ASSERT( ! bn.empty() );
        TS_ASSERT( bn.dag().emptyArcs() );
      }

      void testIterator() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        for( gum::DAG::NodeIterator iter = bn.beginNodes(); iter != bn.endNodes(); ++iter ) {
          TS_ASSERT( idList.exists( *iter ) );
        }
      }

      void testMoralGraph() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        fill( bn, idList );

        gum::UndiGraph graph;
        TS_GUM_ASSERT_THROWS_NOTHING( graph = bn.moralGraph() );
        TS_ASSERT_EQUALS( graph, getRealMoralGraph( bn, idList ) );
      }

      void testTopologicalOrder() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        fill( bn, idList );

        gum::Sequence<gum::NodeId> topoOrder;
        TS_GUM_ASSERT_THROWS_NOTHING( topoOrder = bn.topologicalOrder() );

        TS_ASSERT_EQUALS( topoOrder.size(), ( gum::Size ) 5 );
      }

      void testcpt() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        fill( bn, idList );
        TS_ASSERT_EQUALS( bn.cpt( idList[0] ).domainSize(), ( gum::Size ) 2 );
        TS_ASSERT_EQUALS( bn.cpt( idList[1] ).domainSize(), ( gum::Size ) 2 );
        TS_ASSERT_EQUALS( bn.cpt( idList[2] ).domainSize(), ( gum::Size ) 4 );
        TS_ASSERT_EQUALS( bn.cpt( idList[3] ).domainSize(), ( gum::Size ) 8 );
        TS_ASSERT_EQUALS( bn.cpt( idList[4] ).domainSize(), ( gum::Size ) 24 );
      }

      void testCPTCoherencyVarRemoval() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        fill( bn, idList );

        TS_ASSERT_EQUALS( bn.dag().parents( idList[3] ).size(), ( gum::Size ) 2 );
        TS_ASSERT( bn.dag().existsArc( idList[0], idList[3] ) );
        TS_ASSERT( bn.dag().existsArc( idList[1], idList[3] ) );

        gum::Size dmnSize_1 = bn.variable( idList[0] ).domainSize() * bn.variable( idList[1] ).domainSize();
        dmnSize_1 *= bn.variable( idList[3] ).domainSize();
        TS_ASSERT_EQUALS( bn.cpt( idList[3] ).domainSize(), dmnSize_1 );

        TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[0] ) ) );
        TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[1] ) ) );
        TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[3] ) ) );

        TS_GUM_ASSERT_THROWS_NOTHING( bn.eraseVariable( idList[1] ) );

        TS_ASSERT_EQUALS( bn.dag().parents( idList[3] ).size(), ( gum::Size ) 1 );
        TS_ASSERT( bn.dag().existsArc( idList[0], idList[3] ) );
        TS_ASSERT( ! bn.dag().existsArc( idList[1], idList[3] ) );

        gum::Size dmnSize_2 = bn.variable( idList[0] ).domainSize();
        dmnSize_2 *= bn.variable( idList[3] ).domainSize();
        TS_ASSERT_EQUALS( bn.cpt( idList[3] ).domainSize(), dmnSize_2 );

        TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[0] ) ) );
        TS_ASSERT_THROWS_ANYTHING( bn.cpt( idList[3] ).contains( bn.variable( idList[1] ) ) );
        TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[3] ) ) );
      }

      void testCPTCoherencyArcRemoval() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        fill( bn, idList );

        TS_ASSERT_EQUALS( bn.dag().parents( idList[3] ).size(), ( gum::Size ) 2 );
        TS_ASSERT( bn.dag().existsArc( idList[0], idList[3] ) );
        TS_ASSERT( bn.dag().existsArc( idList[1], idList[3] ) );

        gum::Size dmnSize_1 = bn.variable( idList[0] ).domainSize() * bn.variable( idList[1] ).domainSize();
        dmnSize_1 *= bn.variable( idList[3] ).domainSize();
        TS_ASSERT_EQUALS( bn.cpt( idList[3] ).domainSize(), dmnSize_1 );

        TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[0] ) ) );
        TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[1] ) ) );
        TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[3] ) ) );

        TS_GUM_ASSERT_THROWS_NOTHING( bn.eraseArc( gum::Arc( idList[1], idList[3] ) ) );

        TS_ASSERT_EQUALS( bn.dag().parents( idList[3] ).size(), ( gum::Size ) 1 );
        TS_ASSERT( bn.dag().existsArc( idList[0], idList[3] ) );
        TS_ASSERT( ! bn.dag().existsArc( idList[1], idList[3] ) );

        gum::Size dmnSize_2 = bn.variable( idList[0] ).domainSize();
        dmnSize_2 *= bn.variable( idList[3] ).domainSize();
        TS_ASSERT_EQUALS( bn.cpt( idList[3] ).domainSize(), dmnSize_2 );

        TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[0] ) ) );
        TS_ASSERT( ! bn.cpt( idList[3] ).contains( bn.variable( idList[1] ) ) );
        TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[3] ) ) );
      }

      void testStreamOperator() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        fill( bn, idList );
        std::stringstream sBuff;
        TS_GUM_ASSERT_THROWS_NOTHING( sBuff << bn );
      }

      void testAccessorByName() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        fill( bn, idList );

        for( gum::DAG::NodeIterator iter = bn.beginNodes(); iter != bn.endNodes(); ++iter ) {
          TS_ASSERT_EQUALS( bn.idFromName( bn.variable( *iter ).name() ), *iter );
          TS_ASSERT_EQUALS( &bn.variableFromName( bn.variable( *iter ).name() ), &bn.variable( *iter ) );
        }

        TS_ASSERT_THROWS( bn.idFromName( "choucroute" ), gum::NotFound );

        TS_ASSERT_THROWS( bn.variableFromName( "choucroute" ), gum::NotFound );

        TS_GUM_ASSERT_THROWS_NOTHING( bn.idFromName( "var1" ) );
        bn.erase( bn.idFromName( "var1" ) );
        TS_ASSERT_THROWS( bn.idFromName( "var1" ), gum::NotFound );
      }

      void testCopyAndEqualityOperators() {
        gum::BayesNetGenerator generator;
        gum::BayesNet<float>* bn_1 = generator.generateBNF( 50, 0.1, 4 );
        gum::BayesNet<float>* bn_2 = generator.generateBNF( 25, 0.01, 10 );
        gum::BayesNet<float> bn_cpy_1 = *bn_1;
        gum::BayesNet<float> bn_cpy_2 = *bn_2;
        TS_ASSERT( bn_cpy_1 == bn_cpy_1 );
        TS_ASSERT( bn_cpy_1 == ( *bn_1 ) );
        TS_ASSERT( bn_cpy_2 == bn_cpy_2 );
        TS_ASSERT( bn_cpy_2 == ( *bn_2 ) );
        TS_ASSERT( ( *bn_1 ) != ( *bn_2 ) );
        TS_ASSERT( bn_cpy_1 != bn_cpy_2 );

        TS_ASSERT_EQUALS( bn_cpy_1, bn_cpy_1 );
        TS_ASSERT_EQUALS( bn_cpy_1, ( *bn_1 ) );
        TS_ASSERT_EQUALS( bn_cpy_2, bn_cpy_2 );
        TS_ASSERT_EQUALS( bn_cpy_2, ( *bn_2 ) );
        TS_ASSERT_DIFFERS( ( *bn_1 ), ( *bn_2 ) );
        TS_ASSERT_DIFFERS( bn_cpy_1, bn_cpy_2 );

        gum::BayesNet<float> cpy = *bn_1;
        TS_ASSERT( cpy == ( *bn_1 ) );
        TS_ASSERT( cpy != ( *bn_2 ) );

        try {
          cpy = *bn_2;

        } catch( gum::Exception& e ) {
          TS_GUM_ASSERT_THROWS_NOTHING( throw e );
        }

        TS_ASSERT_EQUALS( cpy, ( *bn_2 ) );

        TS_ASSERT( cpy == ( *bn_2 ) );
        TS_ASSERT_DIFFERS( cpy, ( *bn_1 ) );
        TS_ASSERT( cpy != ( *bn_1 ) );

        try {
          cpy = cpy;

        } catch( gum::Exception& e ) {
          TS_ASSERT_THROWS_NOTHING( throw e );
        }

        TS_ASSERT_EQUALS( cpy, ( *bn_2 ) );

        TS_ASSERT( cpy == ( *bn_2 ) );
        TS_ASSERT_DIFFERS( cpy, ( *bn_1 ) );
        TS_ASSERT( cpy != ( *bn_1 ) );

        std::string s1 = cpy.toString();
        delete bn_2;
        delete bn_1;
        std::string s2 = cpy.toString();
        TS_ASSERT_EQUALS( s1, s2 );
      }

      void testJointProbability() {
        gum::BayesNet<float> bn;
        gum::List<gum::NodeId> idList;

        fill( bn, idList );

        gum::Instantiation i; bn.completeInstantiation( i );
        TS_ASSERT_EQUALS( i.nbrDim(),bn.size() );

        TS_ASSERT_EQUALS( i.domainSize(),( gum::Size )( 2*2*2*2*3 ) );

        float a = 0.0;
        TS_ASSERT_THROWS_NOTHING( a= bn.jointProbability( i ) );

        TS_ASSERT_DELTA( a,0.00072,1e-4 );

        gum::Instantiation j;
        float b = 0.0;
        TS_ASSERT_THROWS_NOTHING( b = bn.jointProbability( j ) );

        TS_ASSERT_EQUALS( a,b );
      }
  };

} //gum_tests
